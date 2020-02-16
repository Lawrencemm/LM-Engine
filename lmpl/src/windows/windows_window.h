#include <lmpl/windows.h>
#include <windef.h>

namespace lmpl
{

class windows_display;

class windows_window : public iwindows_window
{
  public:
    windows_window(windows_display *display, const window_init &init);

    windows_window &show();

    idisplay *p_display();

    screen get_screen() override;

    void hide();

    void acceptDropFiles();

    operator HWND();

    operator bool();

    void move(int byX, int byY);

    void setPos(int newX, int newY);

    windows_window &set_size(int width, int height) override;

    ~windows_window();

    RECT rect();

    RECT clientRect();

    void send_message(const window_message &msg);

    static LRESULT CALLBACK
    CreationWndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);

    static LRESULT CALLBACK
    WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);

    HWND hwnd;
    LRESULT ncState;
    bool sizing;
    windows_display *display;
    static const wchar_t *wndClassName;

    static void createWindowClass(HINSTANCE hinstance);

    windows_window &setText(TCHAR *text);

    static windows_window &GetReference(HWND hwnd);
    float dpi_scale() override;
    lm::size2i get_size_client() override;
    HWND get_hwnd() override;

  private:
    friend class windows_display;

    static HWND createHwnd(windows_window *window, const window_init &init);
};
} // namespace lmpl
