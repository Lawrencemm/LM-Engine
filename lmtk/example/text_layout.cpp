#include "lmtk_example.h"

#include <lmtk/text_layout.h>

class lmtk_text_layout_example : public lmtk_example
{
  public:
    lmtk_text_layout_example()
        : lmtk_example{},
          font{
            font_loader->create_font(lmtk::font_init{
              .renderer = renderer.get(),
              .typeface_name = "Arial",
              .pixel_size = 32,
            }),
          },
          font_material{lmtk::text_layout::create_material(*renderer)},
          text_layout{
            (lmtk::text_layout_init{
              .renderer = *renderer,
              .resource_cache = resource_cache,
              .position = {window_size.width / 2, window_size.height / 2},
              .text = "Hello.",
            }),
          }
    {
    }

  protected:
    bool on_msg(lmtk::input_event const &event) override { return false; }

  protected:
    void on_render(lmgl::iframe *frame) override { text_layout.render(frame); }

  private:
    lmtk::font font;
    lmgl::material font_material;
    lmtk::text_layout text_layout;
};

int main()
{
    lmtk_text_layout_example example;
    example.main();
}
