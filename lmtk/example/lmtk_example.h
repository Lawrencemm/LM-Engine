#pragma once

#include <lmgl/frame.h>
#include <lmgl/renderer.h>
#include <lmgl/stage.h>
#include <lmlib/variant_visitor.h>
#include <lmpl/lmpl.h>
#include <lmtk/lmtk.h>
#include <lmtk/resource_sink.h>

class lmtk_example
{
  public:
    lmtk_example()
        : renderer{lmgl::create_renderer({})},
          display{lmpl::create_display()},
          window_size{
            display->get_primary_screen()->get_size().proportion({1, 2})},
          window{display->create_window(lmpl::window_init{window_size})},
          stage{renderer->create_stage(lmgl::stage_init{window.get()})}
    {
    }

    int main()
    {
        window->show();

        bool quit{false};
        while (!quit)
        {
            auto msg = display->wait_for_message();
            msg >> lm::variant_visitor{
                     [&](lmpl::close_message) { quit = true; },
                     [&](lmpl::repaint_message) { render(); },
                     [&](auto &msg) {
                         auto input_event =
                           lmtk::create_input_event(msg, input_state);
                         if (!input_event)
                             return;
                         if (on_msg(input_event.value()))
                             render();
                     },
                   };
        }
        return 0;
    }
    void render()
    {
        auto frame = stage->wait_for_frame();
        resource_sink.add_frame(frame.get());
        frame->clear_colour({1.f, 1.f, 1.f, 1.f});
        on_render(frame.get());
        frame->build();
        frame->submit();
        frame->wait_complete();
        resource_sink.free_frame(frame.get(), renderer.get());
    }

  protected:
    virtual void on_render(lmgl::iframe *frame) = 0;
    virtual bool on_msg(lmtk::input_event const &event) = 0;

    lmgl::renderer renderer;
    lmpl::display display;
    lm::size2i window_size;
    lmpl::window window;
    lmgl::stage stage;

    lmtk::input_state input_state;
    lmtk::resource_sink resource_sink;
};
