#pragma once

#include <bitset>
#include <map>

#include <lmlib/range.h>

namespace lmpl
{
enum class key_code
{
    A = 4,
    B = 5,
    C = 6,
    D = 7,
    E = 8,
    F = 9,
    G = 10,
    H = 11,
    I = 12,
    J = 13,
    K = 14,
    L = 15,
    M = 16,
    N = 17,
    O = 18,
    P = 19,
    Q = 20,
    R = 21,
    S = 22,
    T = 23,
    U = 24,
    V = 25,
    W = 26,
    X = 27,
    Y = 28,
    Z = 29,
    KEY_1 = 30,
    KEY_2 = 31,
    KEY_3 = 32,
    KEY_4 = 33,
    KEY_5 = 34,
    KEY_6 = 35,
    KEY_7 = 36,
    KEY_8 = 37,
    KEY_9 = 38,
    KEY_0 = 39,
    Enter = 40,
    Escape = 41,
    Delete = 42,
    Tab = 43,
    Space = 44,
    Minus = 45,
    Equals = 46,
    LeftBracket = 47,
    RightBracket = 48,
    Backslash = 49,
    Semicolon = 51,
    Quote = 52,
    Grave = 53,
    Comma = 54,
    Period = 55,
    Slash = 56,
    CapsLock = 57,
    F1 = 58,
    F2 = 59,
    F3 = 60,
    F4 = 61,
    F5 = 62,
    F6 = 63,
    F7 = 64,
    F8 = 65,
    F9 = 66,
    F10 = 67,
    F11 = 68,
    F12 = 69,
    PrintScreen = 70,
    ScrollLock = 71,
    Pause = 72,
    Insert = 73,
    Home = 74,
    PageUp = 75,
    DeleteForward = 76,
    End = 77,
    PageDown = 78,
    Right = 79,
    Left = 80,
    Down = 81,
    Up = 82,
    NUM_NumLock = 83,
    NUM_Divide = 84,
    NUM_Multiply = 85,
    NUM_Subtract = 86,
    NUM_Add = 87,
    NUM_Enter = 88,
    NUM_1 = 89,
    NUM_2 = 90,
    NUM_3 = 91,
    NUM_4 = 92,
    NUM_5 = 93,
    NUM_6 = 94,
    NUM_7 = 95,
    NUM_8 = 96,
    NUM_9 = 97,
    NUM_0 = 98,
    NUM_Point = 99,
    NonUSBackslash = 100,
    NUM_Equals = 103,
    F13 = 104,
    F14 = 105,
    F15 = 106,
    F16 = 107,
    F17 = 108,
    F18 = 109,
    F19 = 110,
    F20 = 111,
    F21 = 112,
    F22 = 113,
    F23 = 114,
    F24 = 115,
    Help = 117,
    Menu = 118,
    LeftControl = 224,
    LeftShift = 225,
    LeftAlt = 226,
    LeftGUI = 227,
    RightControl = 228,
    RightShift = 229,
    RightAlt = 230,
    RightGUI = 231,
    n_keys = 256
};

std::string get_keycode_string(key_code code);

class key_state
{
    friend struct std::hash<lmpl::key_state>;
    using bitset_type = std::bitset<(size_t)key_code::n_keys>;
    bitset_type map;

    size_t find_next(size_t from) const
    {
        for (auto i : lm::range(from + 1, map.size()))
        {
            if (map.test(i))
                return i;
        }
        return map.size();
    }

  public:
    key_state() = default;
    template <typename... arg_types> explicit key_state(arg_types... keys)
    {
        std::array key_array{keys...};
        set(key_array);
    }
    template <typename range_type> key_state(range_type const &range)
    {
        set(range);
    }
    key_state(key_code key) { map.set((size_t)key, true); }

    bool operator==(key_state const &other) const { return map == other.map; }

    bool operator[](key_code code) const { return map[(unsigned)code]; }

    key_state &set(lmpl::key_code code, bool status)
    {
        map.set((size_t)code, status);
        return *this;
    }
    template <typename range_type> key_state &set(range_type const &range)
    {
        for (auto key : range)
            set(key, true);

        return *this;
    }

    bool have_modifiers() const { return shift() || control(); }

    bool shift() const
    {
        return (*this)[key_code::LeftShift] || (*this)[key_code::RightShift];
    }

    bool control() const
    {
        return (*this)[key_code::LeftControl] ||
               (*this)[key_code::RightControl];
    }

    bool alt() const
    {
        return (*this)[key_code::LeftAlt] || (*this)[key_code::RightAlt];
    }

    template <typename function_type>
    void keys(function_type const &function) const
    {
        for (auto i : lm::range(map.size()))
        {
            if (map.test(i))
                function((key_code)i);
        }
    }

    template <typename function_type>
    void names(function_type const &function) const
    {
        keys([&](auto key) { function(get_keycode_string(key)); });
    }
};

inline key_state operator&(lmpl::key_code lhs, lmpl::key_code rhs)
{
    return key_state{lhs, rhs};
}
} // namespace lmpl

namespace std
{
template <> struct hash<lmpl::key_state>
{
    std::size_t operator()(lmpl::key_state const &is) const
    {
        return hash<lmpl::key_state::bitset_type>{}(is.map);
    }
};

std::ostream &operator<<(std::ostream &os, lmpl::key_code key_code);
} // namespace std
