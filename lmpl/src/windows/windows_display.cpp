#include <iostream>

#include <fmt/format.h>
#include <lmpl/lmpl.h>
#include <lmpl/windows_error.h>

#include "windows_display.h"
#include "windows_window.h"

auto blah = SetProcessDPIAware();

namespace lmpl
{

windows_display &windows_display::init()
{
    message_loop_thread = std::thread{[this] {
        MSG msg;
        PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
        gui_thread_id = GetCurrentThreadId();
        message_loop_ready_promise.set_value();
        while (GetMessage(&msg, NULL, 0, 0))
        {
            if (msg.message == U_WM_RUN_FN && !msg.hwnd)
            {
                auto p_fn = (std::function<void()> *)msg.lParam;
                p_fn->operator()();
                continue;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }};
    message_loop_ready_promise.get_future().get();
    return *this;
}

idisplay &windows_display::show_cursor(bool b)
{
    run([b]() { ShowCursor(b); });
    return *this;
}

idisplay &windows_display::confine_cursor(
  iwindow &window,
  int from_x,
  int from_y,
  int to_x,
  int to_y)
{
    run([&] {
        auto spec_window = dynamic_cast<windows_window *>(&window);
        POINT from_screen{from_x, from_y}, to_screen{to_x, to_y};
        ClientToScreen(*spec_window, &from_screen);
        ClientToScreen(*spec_window, &to_screen);
        auto rect =
          RECT{from_screen.x, from_screen.y, to_screen.x, to_screen.y};
        ClipCursor(&rect);
    });
    return *this;
}

idisplay &windows_display::free_cursor()
{
    run([&] { ClipCursor(NULL); });
    return *this;
}

screen windows_display::get_primary_screen()
{
    const POINT ptZero = {0, 0};
    return std::make_unique<windows_screen>(
      MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY));
}

window_message windows_display::wait_for_message()
{
    window_message msg;
    queued_messages.pop(msg);
    return msg;
}

idisplay &windows_display::break_wait()
{
    queued_messages.emplace(window_message{});
    return *this;
}

windows_display::~windows_display()
{
    PostThreadMessage(gui_thread_id, WM_QUIT, 0, 0);
    message_loop_thread.join();
}

LONG_PTR getCreateParams(LPARAM lparam)
{
    return (LONG_PTR)((LPCREATESTRUCT)lparam)->lpCreateParams;
}

lm::size2i windows_screen::get_size()
{
    MONITORINFO info;
    info.cbSize = sizeof(info);
    GetMonitorInfoA(handle, &info);
    return {info.rcMonitor.right - info.rcMonitor.left,
            info.rcMonitor.bottom - info.rcMonitor.top};
}
} // namespace lmpl
