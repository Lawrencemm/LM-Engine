#include <lmlib/variant_visitor.h>
#include <lmtk/text_editor.h>
#include <utility>

namespace lmtk
{

text_editor::text_editor(std::string initial) : text{std::move(initial)} {}

bool text_editor::handle(event const &event)
{
    return event >> lm::variant_visitor{
                      [&](key_down_event const &event)
                      {
                          switch (event.key)
                          {
                          case lmpl::key_code::A:
                          case lmpl::key_code::B:
                          case lmpl::key_code::C:
                          case lmpl::key_code::D:
                          case lmpl::key_code::E:
                          case lmpl::key_code::F:
                          case lmpl::key_code::G:
                          case lmpl::key_code::H:
                          case lmpl::key_code::I:
                          case lmpl::key_code::J:
                          case lmpl::key_code::K:
                          case lmpl::key_code::L:
                          case lmpl::key_code::M:
                          case lmpl::key_code::N:
                          case lmpl::key_code::O:
                          case lmpl::key_code::P:
                          case lmpl::key_code::Q:
                          case lmpl::key_code::R:
                          case lmpl::key_code::S:
                          case lmpl::key_code::T:
                          case lmpl::key_code::U:
                          case lmpl::key_code::V:
                          case lmpl::key_code::W:
                          case lmpl::key_code::X:
                          case lmpl::key_code::Y:
                          case lmpl::key_code::Z:
                          {
                              lmpl::key_code a = lmpl::key_code::A;
                              auto letter_index = char((int)event.key - (int)a);
                              char ascii_offset =
                                event.input_state.key_state.shift() ? 65 : 97;
                              text += (char)(ascii_offset + letter_index);
                              return true;
                          }
                          case lmpl::key_code::KEY_1:
                          case lmpl::key_code::KEY_2:
                          case lmpl::key_code::KEY_3:
                          case lmpl::key_code::KEY_4:
                          case lmpl::key_code::KEY_5:
                          case lmpl::key_code::KEY_6:
                          case lmpl::key_code::KEY_7:
                          case lmpl::key_code::KEY_8:
                          case lmpl::key_code::KEY_9:
                          {

                              lmpl::key_code one = lmpl::key_code::KEY_1;
                              auto number_index = (int)event.key - (int)one;
                              text += (char)(49 + number_index);
                              return true;
                          }
                          case lmpl::key_code::KEY_0:
                              text += '0';
                              return true;
                          case lmpl::key_code::Enter:
                              break;
                          case lmpl::key_code::Escape:
                              break;
                          case lmpl::key_code::Delete:
                              if (text.empty())
                                  return false;

                              text.erase(text.size() - 1);
                              return true;

                          case lmpl::key_code::Tab:
                              break;
                          case lmpl::key_code::Space:
                              text += ' ';
                              return true;

                          case lmpl::key_code::Minus:
                              break;
                          case lmpl::key_code::Equals:
                              break;
                          case lmpl::key_code::LeftBracket:
                              break;
                          case lmpl::key_code::RightBracket:
                              break;
                          case lmpl::key_code::Backslash:
                              break;
                          case lmpl::key_code::Semicolon:
                              break;
                          case lmpl::key_code::Quote:
                              break;
                          case lmpl::key_code::Grave:
                              break;
                          case lmpl::key_code::Comma:
                              break;
                          case lmpl::key_code::Period:
                              text += '.';
                              return true;

                          case lmpl::key_code::Slash:
                              break;
                          case lmpl::key_code::CapsLock:
                              break;
                          case lmpl::key_code::F1:
                              break;
                          case lmpl::key_code::F2:
                              break;
                          case lmpl::key_code::F3:
                              break;
                          case lmpl::key_code::F4:
                              break;
                          case lmpl::key_code::F5:
                              break;
                          case lmpl::key_code::F6:
                              break;
                          case lmpl::key_code::F7:
                              break;
                          case lmpl::key_code::F8:
                              break;
                          case lmpl::key_code::F9:
                              break;
                          case lmpl::key_code::F10:
                              break;
                          case lmpl::key_code::F11:
                              break;
                          case lmpl::key_code::F12:
                              break;
                          case lmpl::key_code::PrintScreen:
                              break;
                          case lmpl::key_code::ScrollLock:
                              break;
                          case lmpl::key_code::Pause:
                              break;
                          case lmpl::key_code::Insert:
                              break;
                          case lmpl::key_code::Home:
                              break;
                          case lmpl::key_code::PageUp:
                              break;
                          case lmpl::key_code::DeleteForward:
                              break;
                          case lmpl::key_code::End:
                              break;
                          case lmpl::key_code::PageDown:
                              break;
                          case lmpl::key_code::Right:
                              break;
                          case lmpl::key_code::Left:
                              break;
                          case lmpl::key_code::Down:
                              break;
                          case lmpl::key_code::Up:
                              break;
                          case lmpl::key_code::NUM_NumLock:
                              break;
                          case lmpl::key_code::NUM_Divide:
                              break;
                          case lmpl::key_code::NUM_Multiply:
                              break;
                          case lmpl::key_code::NUM_Subtract:
                              break;
                          case lmpl::key_code::NUM_Add:
                              break;
                          case lmpl::key_code::NUM_Enter:
                              break;
                          case lmpl::key_code::NUM_1:
                              break;
                          case lmpl::key_code::NUM_2:
                              break;
                          case lmpl::key_code::NUM_3:
                              break;
                          case lmpl::key_code::NUM_4:
                              break;
                          case lmpl::key_code::NUM_5:
                              break;
                          case lmpl::key_code::NUM_6:
                              break;
                          case lmpl::key_code::NUM_7:
                              break;
                          case lmpl::key_code::NUM_8:
                              break;
                          case lmpl::key_code::NUM_9:
                              break;
                          case lmpl::key_code::NUM_0:
                              break;
                          case lmpl::key_code::NUM_Point:
                              break;
                          case lmpl::key_code::NonUSBackslash:
                              break;
                          case lmpl::key_code::NUM_Equals:
                              break;
                          case lmpl::key_code::F13:
                              break;
                          case lmpl::key_code::F14:
                              break;
                          case lmpl::key_code::F15:
                              break;
                          case lmpl::key_code::F16:
                              break;
                          case lmpl::key_code::F17:
                              break;
                          case lmpl::key_code::F18:
                              break;
                          case lmpl::key_code::F19:
                              break;
                          case lmpl::key_code::F20:
                              break;
                          case lmpl::key_code::F21:
                              break;
                          case lmpl::key_code::F22:
                              break;
                          case lmpl::key_code::F23:
                              break;
                          case lmpl::key_code::F24:
                              break;
                          case lmpl::key_code::Help:
                              break;
                          case lmpl::key_code::Menu:
                              break;
                          case lmpl::key_code::LeftControl:
                              break;
                          case lmpl::key_code::LeftShift:
                              break;
                          case lmpl::key_code::LeftAlt:
                              break;
                          case lmpl::key_code::LeftGUI:
                              break;
                          case lmpl::key_code::RightControl:
                              break;
                          case lmpl::key_code::RightShift:
                              break;
                          case lmpl::key_code::RightAlt:
                              break;
                          case lmpl::key_code::RightGUI:
                              break;
                          case lmpl::key_code::n_keys:
                              break;
                          }
                          return false;
                      },
                      [](auto &) { return false; },
                    };
}
} // namespace lmtk
