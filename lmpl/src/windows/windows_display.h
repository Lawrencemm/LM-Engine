#pragma once

#include <future>

#include <windef.h>
#include <windows.h>

#include <tbb/concurrent_queue.h>

#include <lmlib/geometry.h>
#include <lmlib/remote_executor.h>
#include <lmpl/lmpl.h>

#define U_WM_RUN_FN WM_APP + 1

namespace lmpl
{
class vulkan_renderer;
}

namespace lmpl
{

class windows_screen : public iscreen
{
  public:
    windows_screen(HMONITOR handle) : handle{handle} {}

    lm::size2i get_size() override;

    HMONITOR handle;
};

class GuiThread : public lm::remote_executor
{
  public:
    GuiThread() : remote_executor([this]() { init(); }) {}

    template <typename Func> auto run(Func &&func) -> decltype(func());

    void quit() { PostThreadMessage(gui_thread_id, WM_QUIT, 0, 0); }

  private:
    void init()
    {
        gui_thread_id = GetCurrentThreadId();
        createMessageQueue();
    }
    DWORD gui_thread_id;
    void createMessageQueue();
};

template <typename Func> auto GuiThread::run(Func &&func) -> decltype(func())
{
    using FuncReturn = decltype(func());

    if (GetCurrentThreadId() == gui_thread_id)
    {
        if constexpr (std::is_same<FuncReturn, void>::value)
        {
            func();
        }
        else
        {
            return func();
        }
    }
    else
    {
        if constexpr (std::is_same<FuncReturn, void>::value)
        {
            remote_executor(std::move(func));
        }
        else
        {
            return remote_executor(std::move(func));
        }
    }
}

class windows_display : public idisplay
{

  public:
    windows_display &init();
    ~windows_display();

    window create_window(const window_init &init);

    idisplay &show_cursor(bool b) override;

    idisplay &confine_cursor(
      iwindow &window,
      int from_x,
      int from_y,
      int to_x,
      int to_y) override;

    idisplay &free_cursor() override;
    screen get_primary_screen() override;

    template <typename Functor> auto run(const Functor &fn) -> decltype(fn())
    {
        if (GetCurrentThreadId() == gui_thread_id)
        {
            return fn();
        }
        else
        {
            using return_type = decltype(fn());
            std::promise<return_type> promise;
            // Post client message with pointer to fn.
            std::function<void()> function;
            if constexpr (std::is_same<return_type, void>::value)
            {
                function = [this, &promise, &fn] {
                    fn();
                    promise.set_value();
                };
            }
            else
            {
                function = [this, &promise, &fn] { promise.set_value(fn()); };
            }
            PostThreadMessage(gui_thread_id, U_WM_RUN_FN, 0, (LPARAM)&function);
            return promise.get_future().get();
        }
    }
    window_message wait_for_message() override;
    idisplay &break_wait() override;

  private:
    friend class windows_window;

    std::promise<void> message_loop_ready_promise;
    std::thread message_loop_thread;
    DWORD gui_thread_id;
    tbb::concurrent_bounded_queue<window_message> queued_messages;
};

const char *
  MessageToString(unsigned dwMessage, bool bShowFrequentMessages = true);
} // namespace lmpl
