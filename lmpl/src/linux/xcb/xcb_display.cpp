#include <cstdlib>
#include <string>

#include <glibmm/init.h>
#include <glibmm/main.h>

#include <lmpl/xcb.h>
#include <xcb/randr.h>
#include <xcb/xcb_aux.h>

#include <future>

#include <scope_guard.hpp>

#include "xcb_display.h"
#include "xcb_screen.h"
#include "xcb_window.h"

static auto LMLIB_QUIT_ATOM_NAME = "lmlib_quit_atom";

const unsigned char EVDEV_NATIVE_TO_HID[256] = {
  255, 255, 255, 255, 255, 255, 255, 255, 255, 41,  30,  31,  32,  33,  34,
  35,  36,  37,  38,  39,  45,  46,  42,  43,  20,  26,  8,   21,  23,  28,
  24,  12,  18,  19,  47,  48,  40,  224, 4,   22,  7,   9,   10,  11,  13,
  14,  15,  51,  52,  53,  225, 49,  29,  27,  6,   25,  5,   17,  16,  54,
  55,  56,  229, 85,  226, 44,  57,  58,  59,  60,  61,  62,  63,  64,  65,
  66,  67,  83,  71,  95,  96,  97,  86,  92,  93,  94,  87,  89,  90,  91,
  98,  99,  255, 255, 100, 68,  69,  255, 255, 255, 255, 255, 255, 255, 88,
  228, 84,  70,  230, 255, 74,  82,  75,  80,  79,  77,  81,  78,  73,  76,
  255, 255, 255, 255, 255, 103, 255, 72,  255, 255, 255, 255, 255, 227, 231,
  118, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 104, 105, 106, 107,
  108, 109, 110, 111, 112, 113, 114, 115, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255};

const lmpl ::mouse_button detail_to_mouse_button[] = {
  lmpl ::mouse_button::L,
  lmpl ::mouse_button::M,
  lmpl ::mouse_button::R,
  lmpl ::mouse_button::U,
  lmpl ::mouse_button::D};

using namespace std::literals::string_literals;
namespace lmpl
{

// XcbDisplay
XcbDisplay::XcbDisplay()
    : xcb_connection{nullptr},
      screen_number{0},
      xcb_setup{nullptr},
      xcb_api_screen{nullptr}
{

    xcb_connection = xcb_connect(nullptr, &screen_number);

    /* Get the screen whose number is screenNum */

    xcb_setup = xcb_get_setup(xcb_connection);
    xcb_screen_iterator_t iter = xcb_setup_roots_iterator(xcb_setup);

    // we want the screen at index screenNum of the iterator
    for (auto i = 0; i < screen_number; ++i)
    {
        xcb_screen_next(&iter);
    }

    xcb_api_screen = iter.data;

    dummy_window = xcb_generate_id(xcb_connection);

    xcb_create_window(
      xcb_connection,
      0,
      dummy_window,
      xcb_api_screen->root,
      0,
      0,
      1,
      1,
      0,
      0,
      0,
      0,
      0);

    auto wm_protocols_cookie =
      xcb_intern_atom(xcb_connection, 1, 12, "WM_PROTOCOLS");
    auto wm_delete_window_cookie =
      xcb_intern_atom(xcb_connection, 0, 16, "WM_DELETE_WINDOW");
    auto lmlib_quit_atom_cookie = xcb_intern_atom(
      xcb_connection, 0, strlen(LMLIB_QUIT_ATOM_NAME), LMLIB_QUIT_ATOM_NAME);

    xcb_generic_error_t *atom_err = nullptr;

    auto wm_protocols_reply =
      xcb_intern_atom_reply(xcb_connection, wm_protocols_cookie, &atom_err);

    if (atom_err)
        throw std::runtime_error{"xcb: WM_PROTOCOLS atom error."};

    wm_protocols_atom = wm_protocols_reply->atom;

    free(wm_protocols_reply);

    auto wm_delete_window_reply =
      xcb_intern_atom_reply(xcb_connection, wm_delete_window_cookie, &atom_err);

    if (atom_err)
        throw std::runtime_error{"xcb: WM_DELETE_WINDOW atom error."};

    wm_delete_window_atom = wm_delete_window_reply->atom;

    free(wm_delete_window_reply);

    auto lmlib_quit_atom_reply =
      xcb_intern_atom_reply(xcb_connection, lmlib_quit_atom_cookie, &atom_err);

    if (atom_err)
        throw std::runtime_error{"xcb: LMLIB_QUIT atom error."};

    lmlib_quit_atom = lmlib_quit_atom_reply->atom;

    free(lmlib_quit_atom_reply);
    Glib::init();

    text_scale_setting = Gio::Settings::create("org.gnome.desktop.interface");

    // Won't get processed without a Glib main loop. Use Glib?
    //    text_scale_setting->connect_property_changed(
    //      "text-scaling-factor", [this] { std::cout << "ASDFASF\n\n\n"; });

    dpi_scale_ = text_scale_setting->get_double("text-scaling-factor");

    flush();
}

XcbDisplay::~XcbDisplay()
{
    if (xcb_connection)
    {
        xcb_disconnect(xcb_connection);
    }
}

std::optional<window_message>
  XcbDisplay::process_xcb_event(xcb_generic_event_t *event)
{
    switch (event->response_type & ~0x80)
    {
    case XCB_BUTTON_PRESS:
    {
        auto button_event = (xcb_button_press_event_t *)event;
        auto window = context_map[button_event->event];
        if (button_event->detail <= 5)
        {
            return mouse_button_down_message{
              window,
              detail_to_mouse_button[button_event->detail - 1],
              {button_event->event_x, button_event->event_y},
            };
        }
        break;
    }

    case XCB_BUTTON_RELEASE:
    {
        auto button_event = (xcb_button_release_event_t *)event;
        auto window = context_map[button_event->event];
        if (button_event->detail <= 3)
        {
            return mouse_button_up_message{
              window,
              detail_to_mouse_button[button_event->detail - 1],
              {button_event->event_x, button_event->event_y},
            };
        }
    }

    case XCB_KEY_PRESS:
    {
        auto key_event = (xcb_key_press_event_t *)event;
        auto window = context_map[key_event->event];
        return key_down_message{
          window,
          key_code(EVDEV_NATIVE_TO_HID[key_event->detail]),
        };
    }

    case XCB_KEY_RELEASE:
    {
        auto key_event = (xcb_key_release_event_t *)event;
        auto window = context_map[key_event->event];
        return key_up_message{
          window,
          key_code(EVDEV_NATIVE_TO_HID[key_event->detail]),
        };
    }

    case XCB_MOTION_NOTIFY:
    {
        auto motion_event = (xcb_motion_notify_event_t *)event;
        auto window = context_map[motion_event->event];
        return mouse_move_message{
          window,
          {motion_event->event_x, motion_event->event_y},
        };
        break;
    }

    case XCB_CLIENT_MESSAGE:
    {
        auto client_event = (xcb_client_message_event_t *)event;
        if (client_event->data.data32[0] == wm_delete_window_atom)
        {
            auto window = context_map[client_event->window];
            return close_message{window};
        }
        else if (client_event->type == lmlib_quit_atom)
        {
            return window_message{};
        }
        break;
    }

    case XCB_EXPOSE:
    {
        auto expose_event = (xcb_expose_event_t *)event;
        auto window = context_map[expose_event->window];
        return repaint_message{window};
        break;
    }
        //
        //  case XCB_CONFIGURE_NOTIFY: {
        //    auto configure_event = (xcb_configure_notify_event_t *)event;
        //    auto window = context_map[configure_event->window];
        //    window->resize_handler(*window);
        //    break;
        //  }

    case XCB_PROPERTY_NOTIFY:
    {
        auto spec_event = (xcb_property_notify_event_t *)event;
        // std::cerr << '\t' << spec_event->atom << '\n' << '\t' <<
        // (int)spec_event->state << '\n';
        break;
    }

    case XCB_FOCUS_IN:
    {
        auto focus_event = (xcb_focus_in_event_t *)event;
        auto window = context_map[focus_event->event];
        return gain_focus_message{window};
    }

    case XCB_FOCUS_OUT:
    {
        auto focus_event = (xcb_focus_out_event_t *)event;
        auto window = context_map[focus_event->event];
        return lose_focus_message{window};
    }

    default:
        break;
    }
    return std::nullopt;
}

void XcbDisplay::flush() const { xcb_flush(xcb_connection); }

window XcbDisplay::create_window(const window_init &init)
{
    return std::make_unique<XcbWindow>(init, this);
}

idisplay &XcbDisplay::show_cursor(bool b) { return *this; }

idisplay &XcbDisplay::confine_cursor(
  iwindow &window,
  int from_x,
  int from_y,
  int to_x,
  int to_y)
{
    // xcb_grab_pointer(xcb_connection, 1, window->xcb_window,
    // XCB_EVENT_MASK_POINTER_MOTION, XCB_GRAB_MODE_ASYNC,
    // XCB_GRAB_MODE_ASYNC, 0, 0, XCB_CURRENT_TIME);
    return *this;
}

idisplay &XcbDisplay::free_cursor() { return *this; }

screen XcbDisplay::get_primary_screen() { return get_screen(dummy_window); }

screen XcbDisplay::get_screen(xcb_window_t xcb_window)
{

    xcb_randr_get_screen_resources_cookie_t screenResCookie = {};
    screenResCookie =
      xcb_randr_get_screen_resources(xcb_connection, xcb_window);

    // Receive reply from X server
    xcb_randr_get_screen_resources_reply_t *screenResReply = {};
    screenResReply = xcb_randr_get_screen_resources_reply(
      xcb_connection, screenResCookie, nullptr);

    int crtcs_num = 0;
    xcb_randr_crtc_t *firstCRTC;

    // Get a pointer to the first CRTC and number of CRTCs
    // It is crucial to notice that you are in fact getting
    // an array with firstCRTC being the first element of
    // this array and crtcs_length - length of this array
    if (!screenResReply)
        throw std::runtime_error{"No XCB screen result reply."};

    crtcs_num = xcb_randr_get_screen_resources_crtcs_length(screenResReply);

    firstCRTC = xcb_randr_get_screen_resources_crtcs(screenResReply);

    auto free_screen_res = sg::make_scope_guard([&] { free(screenResReply); });

    crtcs_num = xcb_randr_get_screen_resources_crtcs_length(screenResReply);

    firstCRTC = xcb_randr_get_screen_resources_crtcs(screenResReply);

    // Array of requests to the X server for CRTC info
    auto crtcResCookie = std::vector<xcb_randr_get_crtc_info_cookie_t>{};
    crtcResCookie.reserve(crtcs_num);
    for (int i = 0; i < crtcs_num; i++)
        crtcResCookie.push_back(
          xcb_randr_get_crtc_info(xcb_connection, *(firstCRTC + i), 0));

    // Array of pointers to replies from X server
    auto crtcResReply = std::vector<xcb_randr_get_crtc_info_reply_t *>{};
    crtcResReply.reserve(crtcs_num);

    for (int i = 0; i < crtcs_num; i++)
        crtcResReply.push_back(
          xcb_randr_get_crtc_info_reply(xcb_connection, crtcResCookie[i], 0));

    auto free_crtc_replies = sg::make_scope_guard([&] {
        for (auto reply : crtcResReply)
            free(reply);
    });

    XcbScreen screen;
    screen.width = (uint16_t)crtcResReply[0]->width;
    screen.height = (uint16_t)crtcResReply[0]->height;

    return std::make_unique<XcbScreen>(screen);
}

window_message XcbDisplay::wait_for_message()
{
    while (auto event = xcb_wait_for_event(xcb_connection))
    {
        auto msg = process_xcb_event(event);
        free(event);
        if (msg)
            return msg.value();
    }

    return window_message{};
}

idisplay &XcbDisplay::break_wait()
{
    xcb_client_message_event_t event;
    memset(&event, 0, sizeof(event));

    event.response_type = XCB_CLIENT_MESSAGE;
    event.format = 32;
    event.sequence = 0;
    event.window = dummy_window;
    event.type = lmlib_quit_atom;

    xcb_send_event(
      xcb_connection,
      false,
      dummy_window,
      XCB_EVENT_MASK_NO_EVENT,
      reinterpret_cast<const char *>(&event));

    xcb_flush(xcb_connection);
    return *this;
}
} // namespace lmpl
