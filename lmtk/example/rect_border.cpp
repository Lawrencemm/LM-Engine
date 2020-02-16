#include "lmtk_example.h"

#include <lmtk/rect_border.h>

class rect_border_example : public lmtk_example
{
  public:
    rect_border_example()
        : lmtk_example{},
          border_thickness{1.f},
          border_material{lmtk::rect_border::create_material(renderer.get())},
          border{
            renderer.get(),
            border_material,
            {100, 100},
            {300, 300},
            {1.f, 0.f, 0.f, 1.f},
            border_thickness,
          }
    {
    }

    ~rect_border_example() { renderer->destroy_material(border_material); }

  protected:
    void on_render(lmgl::iframe *frame) override { border.add_to_frame(frame); }
    bool on_msg(lmtk::input_event const &event) override
    {
        return event >>
               lm::variant_visitor{
                 [&](lmtk::key_down_event const &key_down_event) {
                     if (key_down_event.key == lmpl::key_code::I)
                     {
                         border_thickness += 1.f;
                         border.set_thickness(border_thickness);
                         return true;
                     }
                     else if (key_down_event.key == lmpl::key_code::K)
                     {
                         if (std::abs(border_thickness - 1.f) < 0.01f)
                             return false;
                         border_thickness -= 1.f;
                         border.set_thickness(border_thickness);
                         return true;
                     }
                     return false;
                 },
                 [](auto) { return false; }};
    }

  private:
    float border_thickness;
    lmgl::material border_material;
    lmtk::rect_border border;
};

int main()
{
    rect_border_example example;
    example.main();
}
