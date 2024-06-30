#pragma once

#include <type_traits>

namespace rusty{
    template<typename T, template<typename...> typename Te> struct is_specialization : std::bool_constant<false>{};

    template<template<typename...> typename Te, typename... Ts> struct is_specialization<Te<Ts...>, Te> : std::bool_constant<true>{};


    template<typename T, template<typename...> typename Te> constexpr inline bool is_specialization_v = is_specialization<T,Te>::value;

    template<typename T, typename U> struct forward_with: std::type_identity<U>{};

    template<typename T, typename U> using forward_with_t = typename forward_with<T,U>::type;

    template<typename T, typename U> struct forward_with<const T, U> : std::type_identity<std::add_const_t<forward_with_t<T,U>>>{};

    template<typename T, typename U> struct forward_with<T&, U> : std::type_identity<forward_with_t<T,U>&>{};
    template<typename T, typename U> struct forward_with<T&&, U> : std::type_identity<forward_with_t<T,U>&&>{};
}