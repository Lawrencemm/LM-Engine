#include <fmt/format.h>
#include <windowsx.h>

#include <lmpl/windows_error.h>

#include "windows_display.h"
#include "windows_window.h"

namespace lmpl
{
const wchar_t *const wndClassName = L"Common WndClass";

const unsigned char WIN_NATIVE_TO_HID[256] = {
  255, 255, 255, 255, 255, 255, 255, 255, 42,  43,  255, 255, 255, 40,  255,
  255, 225, 224, 226, 72,  57,  255, 255, 255, 255, 255, 255, 41,  255, 255,
  255, 255, 44,  75,  78,  77,  74,  80,  82,  79,  81,  255, 255, 255, 70,
  73,  76,  255, 39,  30,  31,  32,  33,  34,  35,  36,  37,  38,  255, 255,
  255, 255, 255, 255, 255, 4,   5,   6,   7,   8,   9,   10,  11,  12,  13,
  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,
  29,  255, 255, 255, 255, 255, 98,  89,  90,  91,  92,  93,  94,  95,  96,
  97,  85,  87,  255, 86,  99,  84,  58,  59,  60,  61,  62,  63,  64,  65,
  66,  67,  68,  69,  104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114,
  115, 255, 255, 255, 255, 255, 255, 255, 255, 83,  71,  255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 51,  46,  54,  45,  55,  56,  53,  255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 47,  49,  48,  52,  255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255};

window windows_display::create_window(const window_init &init)
{
    return (run(
      [&]() { return std::make_unique<lmpl::windows_window>(this, init); }));
}

windows_window::windows_window(
  windows_display *display,
  const window_init &init)
    : display{display}, sizing{false}
{
    display->run([&]() {
        hwnd = windows_window::createHwnd(this, init);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (long long int)this);
        SetWindowLongPtr(hwnd, GWLP_WNDPROC, (long long int)&WndProc);
    });
}

windows_window &windows_window::show()
{
    display->run([this]() { ShowWindow(hwnd, SW_SHOW); });
    return *this;
}

void windows_window::hide() { ShowWindow(hwnd, SW_HIDE); }

HWND windows_window::get_hwnd() { return hwnd; }

RECT windows_window::rect()
{
    return display->run([this]() {
        RECT rect;
        GetWindowRect(hwnd, &rect);
        return rect;
    });
}

RECT windows_window::clientRect()
{
    return display->run([this]() {
        RECT rect;
        GetClientRect(hwnd, &rect);
        return rect;
    });
}

void windows_window::acceptDropFiles()
{
    display->run([this]() {
        SetWindowLongPtr(
          hwnd,
          GWL_EXSTYLE,
          GetWindowLongPtr(hwnd, GWL_EXSTYLE) | WS_EX_ACCEPTFILES);
    });
}

windows_window::operator HWND() { return hwnd; }

windows_window::operator bool() { return hwnd != NULL; }

void windows_window::move(int byX, int byY)
{
    display->run([this, byX, byY]() {
        tagRECT oldPos;
        GetWindowRect(hwnd, &oldPos);
        setPos(oldPos.left + byX, oldPos.top + byY);
    });
}

void windows_window::setPos(int newX, int newY)
{
    display->run([this, newX, newY]() {
        SetWindowPos(hwnd, NULL, newX, newY, 0, 0, SWP_NOSIZE);
    });
}

const wchar_t *windows_window::wndClassName{L"lmlib window class"};

void windows_window::createWindowClass(HINSTANCE hinstance)
{
    static bool registered = false;
    if (!registered)
    {
        const WNDCLASSEXW createStruct{
          sizeof(WNDCLASSEXW),
          CS_HREDRAW | CS_VREDRAW,
          &CreationWndProc,
          0,
          0,
          hinstance,
          NULL,
          LoadCursor(NULL, IDC_ARROW),
          HBRUSH(COLOR_BACKGROUND),
          NULL,
          wndClassName,
          NULL};
        const WORD wndClassAtom = RegisterClassEx(&createStruct);
        registered = true;
    }
}

HWND windows_window::createHwnd(windows_window *window, const window_init &init)
{
    createWindowClass(GetModuleHandle(NULL));
    HWND__ *hwnd = CreateWindowEx(
      0,
      wndClassName,
      L"lala",
      WS_OVERLAPPEDWINDOW,
      500,
      200,
      init.size.width,
      init.size.height,
      NULL,
      NULL,
      GetModuleHandle(NULL),
      &window);
    if (!hwnd)
    {
        throw windows_exception(ErrorString(GetLastError()));
    }
    return hwnd;
}

LONG_PTR CALLBACK
  windows_window::WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
    windows_window &me = GetReference(hwnd);

    switch (umsg)
    {
    case WM_KEYDOWN:
        me.send_message(
          key_down_message{&me, (key_code)WIN_NATIVE_TO_HID[wparam]});
        break;

    case WM_KEYUP:
        me.send_message(
          key_up_message{&me, (key_code)WIN_NATIVE_TO_HID[wparam]});
        break;

    case WM_SYSKEYDOWN:
        switch (wparam)
        {
        case VK_LMENU:
            me.send_message(key_down_message{&me, key_code::LeftAlt});
            break;

        case VK_RMENU:
            me.send_message(key_down_message{&me, key_code::RightAlt});
            break;

        case VK_F4:
            me.send_message(close_message{&me});
            break;

        default:
            me.send_message(
              key_down_message{&me, (key_code)WIN_NATIVE_TO_HID[wparam]});
            break;
        }
        break;

    case WM_SYSKEYUP:
        switch (wparam)
        {
        case VK_LMENU:
            me.send_message(key_up_message{&me, key_code::LeftAlt});
            break;

        case VK_RMENU:
            me.send_message(key_up_message{&me, key_code::RightAlt});
            break;

        default:
            me.send_message(
              key_up_message{&me, (key_code)WIN_NATIVE_TO_HID[wparam]});
            break;
        }
        break;

    case WM_MENUCHAR:
        return MNC_CLOSE << 16;

    case WM_LBUTTONDOWN:
        SetCapture(me);
        me.send_message(mouse_button_down_message{
          &me, mouse_button::L, {GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam)}});
        break;

    case WM_LBUTTONUP:
        SetCapture(NULL);
        me.send_message(mouse_button_down_message{
          &me, mouse_button::L, {GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam)}});
        break;

    case WM_RBUTTONDOWN:
        SetCapture(me);

        me.send_message(mouse_button_down_message{
          &me, mouse_button::R, {GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam)}});
        break;

    case WM_RBUTTONUP:
        SetCapture(NULL);
        me.send_message(mouse_button_up_message{
          &me, mouse_button::R, {GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam)}});
        break;

    case WM_MBUTTONDOWN:
        SetCapture(me);
        me.send_message(mouse_button_down_message{
          &me, mouse_button::M, {GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam)}});
        break;

    case WM_MBUTTONUP:
        SetCapture(NULL);
        me.send_message(mouse_button_up_message{
          &me, mouse_button::M, {GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam)}});
        break;

    case WM_MOUSEWHEEL:
        if (GET_WHEEL_DELTA_WPARAM(wparam) > 0)
            me.send_message(mouse_button_down_message{
              &me,
              mouse_button::U,
              {GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam)}});
        else
            me.send_message(mouse_button_up_message{
              &me,
              mouse_button::D,
              {GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam)}});
        break;

    case WM_MOUSEMOVE:
        me.send_message(mouse_move_message{
          &me, {GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam)}});
        break;

    case WM_CLOSE:
    {
        me.send_message(close_message{&me});
        break;
    }

    case WM_PAINT:
        PAINTSTRUCT ps;
        BeginPaint(me, &ps);
        if (!me.sizing)
            me.send_message(repaint_message{&me});
        EndPaint(me, &ps);
        break;

    case WM_ENTERSIZEMOVE:
        me.sizing = true;
        break;

    case WM_EXITSIZEMOVE:
        me.sizing = false;
        me.send_message(resize_message{&me});
        me.send_message(repaint_message{&me});
        break;

    case WM_SIZE:
        if (!wparam && !me.sizing)
        {
            me.send_message(
              resize_message{&me, LOWORD(lparam), HIWORD(lparam)});
            return 0;
        }
        return DefWindowProc(hwnd, umsg, wparam, lparam);

    default:
        return DefWindowProc(hwnd, umsg, wparam, lparam);
    }
    return 0;
}

LONG_PTR CALLBACK windows_window::CreationWndProc(
  HWND__ *hwnd,
  UINT umsg,
  UINT_PTR wparam,
  LONG_PTR lparam)
{
    return DefWindowProc(hwnd, umsg, wparam, lparam);
}

windows_window &windows_window::setText(WCHAR *text)
{

    display->run([this, text]() { SetWindowText(hwnd, text); });
    return *this;
}

windows_window &windows_window::GetReference(HWND__ *hwnd)
{
    return (windows_window &)*(windows_window *)GetWindowLongPtr(
      hwnd, GWLP_USERDATA);
}

windows_window::~windows_window()
{
    display->run([this]() {
        if (hwnd)
        {
            DestroyWindow(hwnd);
        }
    });
}

windows_window &windows_window::set_size(int width, int height)
{
    display->run([this, width, height]() {
        SetWindowPos(hwnd, NULL, 0, 0, width, height, SWP_NOMOVE);
    });
    return *this;
}

idisplay *windows_window::p_display() { return display; }

screen windows_window::get_screen()
{
    auto handle = display->run(
      [&]() { return MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY); });
    return std::unique_ptr<iscreen>{new windows_screen{handle}};
}

float windows_window::dpi_scale()
{
#if defined(__MINGW32__)
    return 2.f;
#else
    auto dpi = GetDpiForWindow(hwnd);
    return (float)dpi / 96;
#endif
}

lm::size2i windows_window::get_size_client()
{
    RECT rect;
    GetClientRect(hwnd, &rect);
    return lm::size2i{rect.right - rect.left, rect.bottom - rect.top};
}

void windows_window::send_message(const window_message &msg)
{
    display->queued_messages.emplace(msg);
}
} // namespace lmpl
