#pragma once

#include <entt/core/type_traits.hpp>

#define LMNG_NAMED_TYPE(type, name)\
    template<>\
    struct entt::named_type_traits<type>\
        : std::integral_constant<ENTT_ID_TYPE, entt::basic_hashed_string<std::remove_cv_t<std::remove_pointer_t<std::decay_t<decltype(name)>>>>{name}>\
    {\
        static_assert(std::is_same_v<std::remove_cv_t<type>, type>);\
        static_assert(std::is_object_v<type>);\
    };
