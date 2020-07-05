#include <random>

#include <tbb/flow_graph.h>
#include <tbb/task_scheduler_init.h>

#include <lmgl/renderer.h>
#include <lmlib/flow_graph.h>
#include <lmtk/app_flow_graph.h>
#include <lmtk/rect_border.h>

#include <future>
#include <lmlib/variant_visitor.h>
#include <utility>

namespace lmtk
{
using dep_node = tbb::flow::continue_node<tbb::flow::continue_msg>;

template <typename callable_type>
dep_node create_node(tbb::flow::graph &g, callable_type fn)
{
    return dep_node{
      g,
      [fn = std::move(fn)](auto) {
          fn();
          return tbb::flow::continue_msg{};
      },
    };
}

app_resources::app_resources()
{
    using namespace tbb::flow;

    graph init_graph;

    broadcast_node<continue_msg> root_node{init_graph};

    auto window_node = continue_node<lmpl::iwindow *>(init_graph, [&](auto) {
        display = lmpl::create_display();

        window_size =
          display->get_primary_screen()->get_size().proportion({2, 3});

        window = display->create_window(lmpl::window_init{
          .size = window_size,
        });

        return window.get();
    });

    make_edge(root_node, window_node);

    auto renderer_node =
      continue_node<lmgl::irenderer *>(init_graph, [&](auto) {
          renderer = lmgl::create_renderer({});
          return renderer.get();
      });

    make_edge(root_node, renderer_node);

    using stage_node_input_type = tuple<lmpl::iwindow *, lmgl::irenderer *>;

    auto stage_inputs_node = join_node<stage_node_input_type>{init_graph};

    lm::make_edge(window_node, stage_inputs_node);
    lm::make_edge(renderer_node, stage_inputs_node);

    auto font_node = continue_node<continue_msg>(
      init_graph, [&](auto) { font_loader = lmtk::create_font_loader(); });

    make_edge(root_node, font_node);

    auto stage_node =
      function_node<stage_node_input_type>(init_graph, 1, [&](auto inputs) {
          stage =
            get<1>(inputs)->create_stage(lmgl::stage_init{get<0>(inputs)});
      });

    make_edge(stage_inputs_node, stage_node);

    root_node.try_put(continue_msg{});
    init_graph.wait_for_all();
}

template <typename func_type>
static auto
  set_promise_on_exception(func_type const &func, std::promise<void> &promise)
    -> decltype(func())
{
    try
    {
        if constexpr (std::is_void_v<decltype(func())>)
        {
            func();
            return;
        }
        else
        {
            return func();
        }
    }
    catch (...)
    {
        promise.set_value();
        throw;
    }
}

using namespace tbb::flow;

app_flow_graph::app_flow_graph(
  app_resources &resources,
  input_event_handler on_input_event,
  new_frame_handler on_new_frame,
  quit_handler on_quit)
    : resources{resources},
      on_input_event{std::move(on_input_event)},
      on_new_frame{std::move(on_new_frame)},
      on_quit{std::move(on_quit)},
      wait_for_window_msg_node(
        app_lifetime_graph,
        [&](request_window_msg_msg) {
            return set_promise_on_exception(
              [&]() { return resources.display->wait_for_message(); },
              done_promise);
        }),
      frame_limiter_node{app_lifetime_graph, 1},
      wait_for_frame_node{
        app_lifetime_graph,
        [&](request_frame_msg) {
            return set_promise_on_exception(
              [&]() { return new_frame_msg{wait_for_frame()}; }, done_promise);
        },
      },
      render_frame_node{
        app_lifetime_graph,
        1,
        [&](auto &render_frame_msg) {
            return set_promise_on_exception(
              [&]() {
                  render_frame(render_frame_msg.frame.get());
                  return frame_submitted_msg{render_frame_msg.frame};
              },
              done_promise);
        },
      },
      wait_frame_finish_node{
        app_lifetime_graph,
        [this](auto frame_submitted_msg) {
            return set_promise_on_exception(
              [&]() {
                  frame_submitted_msg.frame->wait_complete();
                  return frame_complete_msg{frame_submitted_msg.frame};
              },
              done_promise);
        },
      },
      handle_app_msg_node{
        app_lifetime_graph,
        1,
        [this](appmsg msg, proc_msg_ports_type &outputs) {
            return set_promise_on_exception(
              [&]() { handle_app_msg(msg, outputs); }, done_promise);
        },
      }
{
    make_edge(wait_for_window_msg_node, handle_app_msg_node);
    make_edge(wait_for_frame_node, handle_app_msg_node);
    make_edge(wait_frame_finish_node, handle_app_msg_node);

    lm::make_edge(handle_app_msg_node, wait_for_window_msg_node);
    lm::make_edge(handle_app_msg_node, frame_limiter_node);
    lm::make_edge(handle_app_msg_node, render_frame_node);

    make_edge(frame_limiter_node, wait_for_frame_node);
    make_edge(render_frame_node, wait_frame_finish_node);
}

std::shared_ptr<lmgl::iframe> app_flow_graph::wait_for_frame()
{
    auto frame = resources.stage->wait_for_frame();
    return std::move(frame);
}

void app_flow_graph::render_frame(lmgl::iframe *frame) const
{
    frame->clear_colour({0.05f, 0.05f, 0.05f, 1.f});
    frame->build();
    frame->submit();
}

void app_flow_graph::handle_app_msg(
  appmsg &msg,
  app_flow_graph::proc_msg_ports_type &output_ports)
{
    msg >> lm::variant_visitor{
             [&](new_frame_msg const &new_frame_msg) {
                 if (quitting)
                     return;

                 resources.resource_sink.add_frame(new_frame_msg.frame.get());

                 frame_limiter_node.decrement.try_put(continue_msg{});

                 if (on_new_frame(new_frame_msg.frame.get()))
                     get_frame_async(output_ports);

                 start_render_async(output_ports, new_frame_msg.frame);
             },
             [&](frame_complete_msg const &frame_complete_msg) {
                 resources.resource_sink.free_frame(
                   frame_complete_msg.frame.get(), resources.renderer.get());
             },
             [&](lmpl::window_message const &window_message) {
                 if (quitting)
                     return;

                 window_message >>
                   lm::variant_visitor{
                     [&](lmpl::close_message) {
                         quitting = true;
                         done_promise.set_value();
                         on_quit();
                         resources.resource_sink.free_orphans(
                           resources.renderer.get());
                     },
                     [&](lmpl::repaint_message) {
                         get_window_msg_async(output_ports);
                         get_frame_async(output_ports);
                     },
                     [&](auto msg) {
                         bool dirty{false};
                         auto input_event =
                           lmtk::create_input_event(msg, resources.input_state);

                         if (input_event)
                         {
                             dirty = on_input_event(input_event.value());
                         }

                         get_window_msg_async(output_ports);

                         if (dirty)
                         {
                             get_frame_async(output_ports);
                         }
                     },
                   };
             }};
}

void app_flow_graph::get_window_msg_async(
  app_flow_graph::proc_msg_ports_type &output_ports) const
{
    lm::try_put<proc_msg_outputs_type>(output_ports, request_window_msg_msg{});
}

void app_flow_graph::get_frame_async(
  app_flow_graph::proc_msg_ports_type &output_ports) const
{
    lm::try_put<proc_msg_outputs_type>(output_ports, request_frame_msg{});
}

void app_flow_graph::start_render_async(
  app_flow_graph::proc_msg_ports_type &output_ports,
  std::shared_ptr<lmgl::iframe> frame) const
{
    lm::try_put<proc_msg_outputs_type>(output_ports, render_frame_msg{frame});
}

void app_flow_graph::enter()
{
    resources.window->show();
    wait_for_window_msg_node.try_put(request_window_msg_msg{});
    // Hack to get around the fact graph.wait_for_all() uses a spin lock.
    done_promise.get_future().get();
    app_lifetime_graph.wait_for_all();
}

app_flow_graph::~app_flow_graph() {}

} // namespace lmtk
