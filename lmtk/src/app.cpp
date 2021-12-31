#include <random>

#include <spdlog/spdlog.h>
#include <tbb/flow_graph.h>
#include <SDL.h>

#include <lmgl/renderer.h>
#include <lmlib/flow_graph.h>
#include <lmtk/app.h>
#include <lmtk/rect_border.h>
#include <lmtk/resource_cache.h>

#include <future>
#include <lmlib/variant_visitor.h>
#include <utility>

namespace lmtk
{
app_resources::app_resources()
{
    static auto sdl_result = SDL_Init(SDL_INIT_TIMER);
    using namespace tbb::flow;

    graph init_graph;

    broadcast_node<continue_msg> root_node{init_graph};

    auto font_loader_node = continue_node<lmtk::ifont_loader *>(
      init_graph,
      [&](auto)
      {
          font_loader = lmtk::create_font_loader();
          return font_loader.get();
      });

    make_edge(root_node, font_loader_node);

    auto window_node = continue_node<lmpl::iwindow *>(
      init_graph,
      [&](auto)
      {
          display = lmpl::create_display();

          window = display->create_window(lmpl::window_init{
            .size =
              display->get_primary_screen()->get_size().proportion({2, 3}),
          });

          return window.get();
      });

    make_edge(root_node, window_node);

    auto renderer_node = continue_node<lmgl::irenderer *>(
      init_graph,
      [&](auto)
      {
          renderer = lmgl::create_renderer({});
          return renderer.get();
      });

    make_edge(root_node, renderer_node);

    using resource_cache_node_input_type =
      tuple<lmgl::irenderer *, lmtk::ifont_loader *>;

    auto resource_cache_inputs_join_node =
      join_node<resource_cache_node_input_type>{init_graph};

    lm::make_edge(renderer_node, resource_cache_inputs_join_node);
    lm::make_edge(font_loader_node, resource_cache_inputs_join_node);

    auto resource_cache_node = function_node<resource_cache_node_input_type>(
      init_graph,
      1,
      [&](auto inputs)
      {
          resource_cache =
            std::make_unique<lmtk::resource_cache>(resource_cache_init{
              .renderer = std::get<0>(inputs),
              .font_loader = std::get<1>(inputs),
              .body_font = lmtk::font_description{
                .typeface_name = "Arial",
                .pixel_size = 24,
              }});
          return tbb::flow::continue_msg{};
      });

    make_edge(resource_cache_inputs_join_node, resource_cache_node);

    using stage_node_input_type = tuple<lmpl::iwindow *, lmgl::irenderer *>;

    auto stage_inputs_node = join_node<stage_node_input_type>{init_graph};

    lm::make_edge(window_node, stage_inputs_node);
    lm::make_edge(renderer_node, stage_inputs_node);

    auto stage_node = function_node<stage_node_input_type>(
      init_graph,
      1,
      [&](auto inputs) {
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

app::app()
    : resources{},
      wait_for_window_msg_node(
        app_lifetime_graph,
        [&](request_window_msg_msg)
        {
            return set_promise_on_exception(
              [&]() { return resources.display->wait_for_message(); },
              done_promise);
        }),
      frame_request_buffer_node{app_lifetime_graph},
      frame_limiter_node{app_lifetime_graph, 1},
      frame_limit_recreate_stage_node{
        app_lifetime_graph,
        1,
        [](auto) {
            return -1;
        }},
      wait_for_frame_node{
        app_lifetime_graph,
        [&](request_frame_msg)
        {
            return set_promise_on_exception(
              [&]()
              {
                  auto frame = wait_for_frame();
                  return new_frame_msg{frame};
              },
              done_promise);
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
      },
      recreate_stage_buffer_node{app_lifetime_graph},
      recreate_stage_limiter_node{app_lifetime_graph, 1},
      recreate_stage_limit_new_frame_node{
        app_lifetime_graph,
        1,
        [](auto) {
            return -1;
        }},
      recreate_stage_node{
        app_lifetime_graph,
        1,
        [this](recreate_stage_msg) {
            return set_promise_on_exception(
              [&]() {
                  this->resources.stage =
                    this->resources.renderer->create_stage(
                      {this->resources.window.get()});
                  return stage_recreated_msg{};
              },
              done_promise);
        },
      }
{
    make_edge(wait_for_window_msg_node, handle_app_msg_node);
    make_edge(wait_for_frame_node, handle_app_msg_node);
    make_edge(wait_frame_finish_node, handle_app_msg_node);
    make_edge(recreate_stage_node, handle_app_msg_node);

    lm::make_edge(handle_app_msg_node, wait_for_window_msg_node);
    lm::make_edge(handle_app_msg_node, frame_request_buffer_node);
    lm::make_edge(handle_app_msg_node, render_frame_node);
    lm::make_edge(handle_app_msg_node, recreate_stage_buffer_node);

    make_edge(recreate_stage_buffer_node, recreate_stage_limiter_node);
    make_edge(recreate_stage_limiter_node, recreate_stage_node);
    make_edge(recreate_stage_limiter_node, recreate_stage_limit_new_frame_node);
    make_edge(
      recreate_stage_limit_new_frame_node, frame_limiter_node.decrement);

    make_edge(frame_request_buffer_node, frame_limiter_node);
    make_edge(frame_limiter_node, wait_for_frame_node);
    make_edge(frame_limiter_node, frame_limit_recreate_stage_node);
    make_edge(
      frame_limit_recreate_stage_node, recreate_stage_limiter_node.decrement);

    make_edge(render_frame_node, wait_frame_finish_node);
}

std::shared_ptr<lmgl::iframe> app::wait_for_frame()
{
    auto frame = resources.stage->wait_for_frame();
    return std::move(frame);
}

void app::render_frame(lmgl::iframe *frame) const
{
    frame->clear_colour({0.05f, 0.05f, 0.05f, 1.f});
    frame->build();
    frame->submit();
}

void app::handle_app_msg(appmsg &msg, app::proc_msg_ports_type &output_ports)
{
    msg >>
      lm::variant_visitor{
        [&](new_frame_msg const &new_frame_msg)
        {
            if (quitting)
                return;

            frame_limiter_node.decrement.try_put(1);

            resources.resource_sink.add_frame(new_frame_msg.frame.get());

            lmtk::draw_event event{
              *resources.renderer,
              *new_frame_msg.frame,
              resources.resource_sink,
              *resources.resource_cache,
              resources.input_state};

            auto state = on_event(event);
            if (state.request_draw_in)
            {
                SPDLOG_INFO(
                  "new frame message handler returned request for frame in {}",
                  state.request_draw_in.value());
                float dt = state.request_draw_in.value();
                schedule_request_frame(dt);
            }

            start_render_async(output_ports, new_frame_msg.frame);
        },
        [&](frame_complete_msg const &frame_complete_msg) {
            recreate_stage_limiter_node.decrement.try_put(1);

            resources.resource_sink.free_frame(
              frame_complete_msg.frame.get(), resources.renderer.get());
        },
        [&](stage_recreated_msg const &stage_recreated_msg) {

            stage_recreate_pending = false;
            on_event(resize_event{*resources.window});
            frame_limiter_node.decrement.try_put(1);
            recreate_stage_limiter_node.decrement.try_put(1);
            make_edge(frame_request_buffer_node, frame_limiter_node);
        },
        [&](lmpl::window_message const &window_message) {
            if (quitting)
                return;

            window_message >>
              lm::variant_visitor{
                [&](lmpl::close_message) {
                    quitting = true;
                    done_promise.set_value();
                    on_event(quit_event{});
                    resources.resource_sink.free_orphans(
                      resources.renderer.get());
                },
                [&](lmpl::repaint_message) {
                    get_window_msg_async(output_ports);
                    get_frame_async(output_ports);
                },
                [&](lmpl::resize_message const &resize_message) {
                    stage_recreate_pending = true;
                    remove_edge(frame_request_buffer_node, frame_limiter_node);
                    recreate_stage_async(output_ports);
                    get_window_msg_async(output_ports);
                },
                [&](auto msg) {
                    lmtk::component_state state;
                    auto input_event =
                      lmtk::create_input_event(msg, resources.input_state);

                    if (input_event)
                    {
                        state = on_event(input_event.value());
                    }

                    get_window_msg_async(output_ports);

                    if (state.request_draw_in)
                    {
                        SPDLOG_INFO(
                          "window message handler returned request for frame "
                          "in {}",
                          state.request_draw_in.value());
                        schedule_request_frame(state.request_draw_in.value());
                    }
                },
              };
        }};
}

uint32_t app::sdl_frame_timer_callback(uint32_t interval, void *param)
{
    SPDLOG_INFO("Frame request timer activated");
    auto app = (lmtk::app *)param;
    app->frame_request_buffer_node.try_put({});
    return 0;
}

void app::schedule_request_frame(float dt)
{
    using namespace std::chrono;
    SDL_RemoveTimer(frame_schedule_timer_id);
    if (dt == 0.f)
    {
        frame_request_buffer_node.try_put({});
    }
    else
    {
        auto dt_duration = duration<float>{dt};
        auto new_frame_request =
          std::chrono::steady_clock::now() +
          duration_cast<steady_clock::duration>(dt_duration);

        auto milliseconds = std::chrono::duration_cast<
          std::chrono::duration<uint32_t, std::milli>>(
          new_frame_request - steady_clock::now());

        frame_schedule_timer_id =
          SDL_AddTimer(milliseconds.count(), sdl_frame_timer_callback, this);

        SPDLOG_INFO(
          "Frame request timer added for {} milliseconds",
          milliseconds.count());

        if (frame_schedule_timer_id == 0)
        {
            throw std::runtime_error{fmt::format(
              "Failed to schedule SDL timer for new frame.\nSDL Error:\n{}",
              SDL_GetError())};
        }
    }
}

void app::get_window_msg_async(app::proc_msg_ports_type &output_ports) const
{
    lm::try_put<proc_msg_outputs_type>(output_ports, request_window_msg_msg{});
}

void app::get_frame_async(app::proc_msg_ports_type &output_ports)
{
    lm::try_put<proc_msg_outputs_type>(output_ports, request_frame_msg{});
}

void app::start_render_async(
  app::proc_msg_ports_type &output_ports,
  std::shared_ptr<lmgl::iframe> frame)
{
    lm::try_put<proc_msg_outputs_type>(output_ports, render_frame_msg{frame});
}

void app::recreate_stage_async(app::proc_msg_ports_type &output_ports)
{
    lm::try_put<proc_msg_outputs_type>(output_ports, recreate_stage_msg{});
}

void app::enter()
{
    resources.window->show();
    wait_for_window_msg_node.try_put(request_window_msg_msg{});
    // Hack to get around the fact graph.wait_for_all() uses a spin lock.
    done_promise.get_future().get();
    app_lifetime_graph.wait_for_all();
}

app::~app()
{
    resources.resource_cache->move_resources(resources.resource_sink);
    resources.resource_sink.free_orphans(resources.renderer.get());
}

lmtk::component_state app::on_event(const event &) { return {}; }

} // namespace lmtk
