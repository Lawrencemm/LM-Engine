#pragma once

#include "font.h"
#include "event.h"
#include "lmgl/resource_sink.h"
#include "component.h"
#include <future>
#include <lmgl/fwd_decl.h>
#include <lmlib/flow_graph.h>
#include <lmpl/lmpl.h>

namespace lmtk
{
class app_resources
{
  public:
    lmpl::display display;
    lmgl::renderer renderer;
    lmpl::window window;
    lmgl::stage stage;

    lmtk::font_loader font_loader;
    std::unique_ptr<lmtk::resource_cache> resource_cache;

    lmtk::input_state input_state;
    lmgl::resource_sink resource_sink;

  public:
    app_resources();
    app_resources(app_resources const &) = delete;
};

class app
{
  public:
    tbb::flow::graph app_lifetime_graph;

  private:
    struct request_window_msg_msg
    {
    };
    struct request_frame_msg
    {
    };
    struct quit_app_msg
    {
    };
    struct new_frame_msg
    {
        std::shared_ptr<lmgl::iframe> frame;
    };
    struct render_frame_msg
    {
        std::shared_ptr<lmgl::iframe> frame;
    };
    struct frame_submitted_msg
    {
        std::shared_ptr<lmgl::iframe> frame;
    };
    struct frame_complete_msg
    {
        std::shared_ptr<lmgl::iframe> frame;
    };
    struct recreate_stage_msg
    {
    };
    struct stage_recreated_msg
    {
    };

    using appmsg = std::variant<
      lmpl::window_message,
      new_frame_msg,
      frame_complete_msg,
      stage_recreated_msg>;

    using proc_msg_outputs_type = tbb::flow::tuple<
      request_window_msg_msg,
      request_frame_msg,
      render_frame_msg,
      quit_app_msg,
      recreate_stage_msg>;

    using proc_msg_node_type =
      tbb::flow::multifunction_node<appmsg, proc_msg_outputs_type>;

    using proc_msg_ports_type = proc_msg_node_type::output_ports_type;

    lm::wait_node<request_window_msg_msg, appmsg> wait_for_window_msg_node;

    lm::overwrite_node<request_frame_msg> frame_request_buffer_node;
    lm::limiter_node<request_frame_msg> frame_limiter_node;
    tbb::flow::function_node<request_frame_msg, int, tbb::flow::lightweight>
      frame_limit_recreate_stage_node;
    lm::wait_node<request_frame_msg, appmsg> wait_for_frame_node;

    tbb::flow::function_node<render_frame_msg, frame_submitted_msg>
      render_frame_node;

    lm::overwrite_node<recreate_stage_msg> recreate_stage_buffer_node;
    lm::limiter_node<recreate_stage_msg> recreate_stage_limiter_node;
    tbb::flow::function_node<recreate_stage_msg, int, tbb::flow::lightweight>
      recreate_stage_limit_new_frame_node;
    tbb::flow::function_node<recreate_stage_msg, appmsg> recreate_stage_node;

    lm::wait_node<frame_submitted_msg, appmsg> wait_frame_finish_node;

    proc_msg_node_type handle_app_msg_node;

    std::promise<void> done_promise;
    bool quitting{false};
    bool stage_recreate_pending{false};
    int frame_schedule_timer_id{-1};

    void handle_app_msg(appmsg &msg, proc_msg_ports_type &output_ports);

    void start_render_async(
      proc_msg_ports_type &output_ports,
      std::shared_ptr<lmgl::iframe> frame);
    void get_frame_async(proc_msg_ports_type &output_ports);
    void get_window_msg_async(proc_msg_ports_type &output_ports) const;
    std::shared_ptr<lmgl::iframe> wait_for_frame();
    void render_frame(lmgl::iframe *frame) const;
    void recreate_stage_async(proc_msg_ports_type &output_ports);

    static uint32_t sdl_frame_timer_callback(uint32_t interval, void *param);

  public:
    app();
    app(app const &) = delete;
    ~app();

    void enter();

  protected:
    app_resources resources;
    virtual lmtk::component_state on_event(lmtk::event const &event);
    void schedule_request_frame(float dt);
};
} // namespace lmtk
