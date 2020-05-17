#pragma once

#include "font.h"
#include "input_event.h"
#include "lmgl/resource_sink.h"
#include <future>
#include <lmgl/fwd_decl.h>
#include <lmlib/flow_graph.h>
#include <lmpl/lmpl.h>
#include <readerwriterqueue.h>
#include <tbb/task_scheduler_init.h>

namespace lmtk
{
class app_resources;

class app_flow_graph
{
    using input_event_handler = std::function<bool(input_event const &)>;
    using new_frame_handler = std::function<bool(lmgl::iframe *)>;
    using quit_handler = std::function<void()>;

  private:
    app_resources &resources;

    input_event_handler on_input_event;
    new_frame_handler on_new_frame;
    quit_handler on_quit;

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
        lmgl::iframe *frame;
    };
    struct submit_frame_msg
    {
        lmgl::iframe *frame;
    };
    struct frame_submitted_msg
    {
        lmgl::iframe *frame;
    };
    struct frame_complete_msg
    {
        lmgl::iframe *frame;
    };

    using appmsg =
      std::variant<lmpl::window_message, new_frame_msg, frame_complete_msg>;

    using proc_msg_outputs_type = tbb::flow::tuple<
      request_window_msg_msg,
      request_frame_msg,
      submit_frame_msg,
      quit_app_msg>;

    using proc_msg_node_type =
      tbb::flow::multifunction_node<appmsg, proc_msg_outputs_type>;

    using proc_msg_ports_type = proc_msg_node_type::output_ports_type;

    tbb::flow::graph app_lifetime_graph;

    lm::wait_node<request_window_msg_msg, appmsg> wait_for_window_msg_node;
    tbb::flow::limiter_node<request_frame_msg> frame_limiter_node;
    lm::wait_node<request_frame_msg, appmsg> wait_for_frame_node;
    tbb::flow::function_node<submit_frame_msg, frame_submitted_msg>
      submit_frame_node;
    lm::wait_node<frame_submitted_msg, appmsg> wait_frame_finish_node;

    proc_msg_node_type handle_app_msg_node;

    tbb::flow::function_node<quit_app_msg> quit_app_node;

    std::promise<void> done_promise;

    void handle_app_msg(appmsg &msg, proc_msg_ports_type &output_ports);
    void handle(
      lmpl::window_message &window_message,
      app_flow_graph::proc_msg_ports_type &output_ports);

    bool handle_input_event(lmtk::input_event const &input_event);
    void handle_quit_msg();
    void ask_for_render(proc_msg_ports_type &output_ports, lmgl::iframe *frame)
      const;
    void ask_for_frame(proc_msg_ports_type &output_ports) const;
    void ask_for_window_msg(proc_msg_ports_type &output_ports) const;
    void ask_for_quit(proc_msg_ports_type &output_ports) const;
    void handle(
      app_flow_graph::new_frame_msg const &new_frame_msg,
      app_flow_graph::proc_msg_ports_type &output_ports);
    void handle(
      app_flow_graph::frame_complete_msg const &frame_complete_msg,
      app_flow_graph::proc_msg_ports_type &output_ports);
    lmgl::iframe *wait_for_frame();
    void submit_frame(lmgl::iframe *frame) const;

  public:
    app_flow_graph(
      app_resources &resources,
      input_event_handler on_input_event,
      new_frame_handler on_new_frame,
      quit_handler on_quit);
    app_flow_graph(app_flow_graph const &) = delete;
    ~app_flow_graph();

    void enter();
};

class app_resources
{
  public:
    lmpl::display display;
    lmgl::renderer renderer;
    lmpl::window window;
    lmgl::stage stage;

    lmtk::font_loader font_loader;

    lm::size2i window_size;

    lmtk::input_state input_state;
    lmgl::resource_sink resource_sink;

    moodycamel::ReaderWriterQueue<lmgl::frame> frames;

  public:
    app_resources();
    app_resources(app_resources const &) = delete;
};
} // namespace lmtk
