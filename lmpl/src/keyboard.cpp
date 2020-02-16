#include <lmpl/keyboard.h>

namespace lmpl
{
std::map<key_code, std::string> const key_code_name_map{
  {key_code::A, "A"},
  {key_code::B, "B"},
  {key_code::C, "C"},
  {key_code::D, "D"},
  {key_code::E, "E"},
  {key_code::F, "F"},
  {key_code::G, "G"},
  {key_code::H, "H"},
  {key_code::I, "I"},
  {key_code::J, "J"},
  {key_code::K, "K"},
  {key_code::L, "L"},
  {key_code::M, "M"},
  {key_code::N, "N"},
  {key_code::O, "O"},
  {key_code::P, "P"},
  {key_code::Q, "Q"},
  {key_code::R, "R"},
  {key_code::S, "S"},
  {key_code::T, "T"},
  {key_code::U, "U"},
  {key_code::V, "V"},
  {key_code::W, "W"},
  {key_code::X, "X"},
  {key_code::Y, "Y"},
  {key_code::Z, "Z"},
  {key_code::KEY_1, "1"},
  {key_code::KEY_2, "2"},
  {key_code::KEY_3, "3"},
  {key_code::KEY_4, "4"},
  {key_code::KEY_5, "5"},
  {key_code::KEY_6, "6"},
  {key_code::KEY_7, "7"},
  {key_code::KEY_8, "8"},
  {key_code::KEY_9, "9"},
  {key_code::KEY_0, "0"},
  {key_code::LeftShift, "Shift"},
};
}

std::ostream &std::operator<<(std::ostream &os, lmpl::key_code key_code)
{
    os << lmpl::key_code_name_map.at(key_code);
    return os;
}
