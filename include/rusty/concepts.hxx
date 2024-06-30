#pragma once

#include <concepts> // Public

#include <rusty/type_traits.hxx>
#include <type_traits>
#include <tuple>
#include <array>
#include <iterator>
#include <ranges>

namespace rusty{
    /// A concept that tests whether or not `T` is structurally equality comparable
    /// A type `T` satisfies `structural_equality` if it satisfies `std::equality_comparable`
    ///
    /// A type `T` models `structural_equality` if:
    /// * `T` models `std::equality_comparable`,
    /// * Each field and each base class of `T` models `structural_equality` or is an empty type,
    /// * Two (constructible) objects of type `T` are equal if they have the same value-representation.
    /// * If `T` satisfies `std::copy_constructible`, then `T` models `std::copy_constructible` and given a value `t` of type `T`, `t == T(t)` is true.
    /// * Equality comparison of two values of type `T` does not throw any exceptions.
    ///
    /// In particular `float`/`double` do not model `structural_equality`
    template<typename T> concept structural_equality = std::equality_comparable<T>;


    template<typename T> concept _boolean_testable_impl = requires(const T& val){
        static_cast<bool>(val);
    };

    /// A concept that models types which are equivalent to `bool`
    /// The concept is satisfied if `T` is explicitly convertible to `bool` and, given two values of type `T val` and `T val2`, and a value of type `bool bool_val`, 
    ///  all of the following are well-formed, and return a type explicitly convertible to `bool`:
    /// * `!val1`
    /// * `val1 && val2`
    /// * `val1 && val2`
    /// * `val1 && bool_val`
    /// * `bool_val && val2`
    /// * `val1 || bool_val`
    /// * `bool_val || val2`
    ///
    /// The concept is modeled if the expressions given above do not throw any exceptions and return a type which models `boolean_testable` and, given the values above, each of the following pairs of expressions are equivalent:
    /// * `static_cast<bool>(!val1)` and `!static_cast<bool>(val1)`
    /// * `static_cast<bool>(!!val1)` and `static_cast<bool>(val1)`
    /// * `static_cast<bool>(val1 && val2)` and `static_cast<bool>(val1) && static_cast<bool>(val2)`
    /// * `static_cast<bool>(val1 || val2)` and `static_cast<bool>(val1) || static_cast<bool>(val2)`
    /// * `static_cast<bool>(val1 && bool_val)` and `static_cast<bool>(val1) && bool_val`
    /// * `static_cast<bool>(bool_val && val2)` and `bool_val && static_cast<bool>(val2)`
    /// * `static_cast<bool>(val1 || bool_val)` and `static_cast<bool>(val1) && bool_val`
    /// * `static_cast<bool>(bool_val || val2)` and `bool_val && static_cast<bool>(val1)`
    template<typename T> concept boolean_testable = requires(const T& val, const bool& bool_val){
        static_cast<bool>(val);

        {!val} -> _boolean_testable_impl;
        {val && val} -> _boolean_testable_impl;
        {val || val} -> _boolean_testable_impl;
        {val && bool_val} -> _boolean_testable_impl;
        {bool_val && val} -> _boolean_testable_impl;
        {val || bool_val} -> _boolean_testable_impl;
        {bool_val || val} -> _boolean_testable_impl;
    };


    // template<typename T, std::size_t N = std::tuple_size_v<T>> struct _last_tuple_element : std::type_identity<std::tuple_element_t<N-1,T>>{};

    // template<typename T> struct _last_tuple_element<T,0> : std::type_identity<void>{};

    template<typename T> concept tuple_like = requires{
        std::tuple_size_v<T>;
        // typename _last_tuple_element<T>::type;
    };


    template<typename T, typename Idx> concept indexable = requires(T& t, const Idx& idx){
        t[idx];
    };

}