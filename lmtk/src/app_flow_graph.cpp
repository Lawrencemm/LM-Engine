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
            return resources.display->wait_for_message();
        }),
      frame_limiter_node{app_lifetime_graph, 1},
      wait_for_frame_node{
        app_lifetime_graph,
        [&](request_frame_msg) {
            auto frame = wait_for_frame();
            return new_frame_msg{frame};
        },
      },
      submit_frame_node{
        app_lifetime_graph,
        1,
        [&](auto &render_frame_msg) {
            submit_frame(render_frame_msg.frame);
            return frame_submitted_msg{render_frame_msg.frame};
        },
      },
      wait_frame_finish_node{
        app_lifetime_graph,
        [this](auto frame_submitted_msg) {
            frame_submitted_msg.frame->wait_complete();
            return frame_complete_msg{frame_submitted_msg.frame};
        },
      },
      handle_app_msg_node{
        app_lifetime_graph,
        1,
        [this](appmsg msg, proc_msg_ports_type &outputs) {
            handle_app_msg(msg, outputs);
        },
      },
      quit_app_node{
        app_lifetime_graph,
        1,
        [this](quit_app_msg) { handle_quit_msg(); },
      }
{
    make_edge(wait_for_window_msg_node, handle_app_msg_node);
    make_edge(wait_for_frame_node, handle_app_msg_node);
    make_edge(wait_frame_finish_node, handle_app_msg_node);

    lm::make_edge(handle_app_msg_node, wait_for_window_msg_node);
    lm::make_edge(handle_app_msg_node, frame_limiter_node);
    lm::make_edge(handle_app_msg_node, submit_frame_node);
    lm::make_edge(handle_app_msg_node, quit_app_node);

    make_edge(frame_limiter_node, wait_for_frame_node);
    make_edge(submit_frame_node, wait_frame_finish_node);
}

lmgl::iframe *app_flow_graph::wait_for_frame()
{
    auto frame = resources.stage->wait_for_frame();
    auto p_frame = frame.get();
    resources.frames.emplace(std::move(frame));
    return p_frame;
}

void app_flow_graph::submit_frame(lmgl::iframe *frame) const
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
             [&](auto &msg) { handle(msg, output_ports); },
           };
}

void app_flow_graph::handle(
  app_flow_graph::frame_complete_msg const &frame_complete_msg,
  app_flow_graph::proc_msg_ports_type &output_ports)
{
    resources.resource_sink.free_frame(
      frame_complete_msg.frame, resources.renderer.get());
    resources.frames.pop();
}

void app_flow_graph::handle(
  app_flow_graph::new_frame_msg const &new_frame_msg,
  app_flow_graph::proc_msg_ports_type &output_ports)
{
    resources.resource_sink.add_frame(new_frame_msg.frame);

    frame_limiter_node.decrement.try_put(continue_msg{});

    bool dirty;

    try
    {
        dirty = on_new_frame(new_frame_msg.frame);
    }
    catch (...)
    {
        done_promise.set_value();
        throw;
    }

    if (dirty)
        ask_for_frame(output_ports);

    ask_for_render(output_ports, new_frame_msg.frame);
}

void app_flow_graph::handle(
  lmpl::window_message &window_message,
  app_flow_graph::proc_msg_ports_type &output_ports)
{
    window_message >>
      lm::variant_visitor{
        [&](lmpl::close_message) {
            remove_edge(wait_for_frame_node, handle_app_msg_node);
            ask_for_quit(output_ports);
        },
        [&](lmpl::repaint_message) {
            ask_for_window_msg(output_ports);
            ask_for_frame(output_ports);
        },
        [&](auto msg) {
            bool dirty{false};
            auto input_event =
              lmtk::create_input_event(msg, resources.input_state);

            if (input_event)
            {
                dirty = handle_input_event(input_event.value());
            }

            ask_for_window_msg(output_ports);

            if (dirty)
            {
                ask_for_frame(output_ports);
            }
        },
      };
}

bool app_flow_graph::handle_input_event(lmtk::input_event const &input_event)
{
    try
    {
        return on_input_event(input_event);
    }
    catch (...)
    {
        done_promise.set_value();
        throw;
    }
}

void app_flow_graph::handle_quit_msg()
{
    try
    {
        on_quit();
    }
    catch (...)
    {
        done_promise.set_value();
        throw;
    }
    done_promise.set_value();
}

void app_flow_graph::ask_for_quit(
  app_flow_graph::proc_msg_ports_type &output_ports) const
{
    lm::try_put<proc_msg_outputs_type>(output_ports, quit_app_msg{});
}

void app_flow_graph::ask_for_window_msg(
  app_flow_graph::proc_msg_ports_type &output_ports) const
{
    lm::try_put<proc_msg_outputs_type>(output_ports, request_window_msg_msg{});
}

void app_flow_graph::ask_for_frame(
  app_flow_graph::proc_msg_ports_type &output_ports) const
{
    lm::try_put<proc_msg_outputs_type>(output_ports, request_frame_msg{});
}

void app_flow_graph::ask_for_render(
  app_flow_graph::proc_msg_ports_type &output_ports,
  lmgl::iframe *frame) const
{
    lm::try_put<proc_msg_outputs_type>(output_ports, submit_frame_msg{frame});
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
