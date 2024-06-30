#pragma once

#include <iterator>
#include <concepts>
#include <bit>
#include <utility>
#include <stdexcept>
#include <span>

#include <rusty/type_traits.hxx>
#include <rusty/concepts.hxx>
#include <crabi/option.hxx>
#include <ranges>
#include <exception>
#include <format>

namespace crabi{
    namespace slice{
        
        template<typename T> struct raw_slice_ptr{
            T* p_data;
            std::size_t p_len;

        };

        template<typename T> struct slice{
        private:
            raw_slice_ptr<T> _m_ptr;
        public:
            using value_type = T;
            using reference = T&;
            using const_reference = const T&;
            using iterator = T*;
            using const_iterator = const T*;


            template<std::ranges::contiguous_range Range> requires std::convertible_to<std::remove_reference_t<std::ranges::range_reference_t<Range>>(&)[], T(&)[]>
                constexpr slice(Range&& c) : raw_slice_ptr{std::ranges::data(c), std::ranges::size(c)}{}

            template<std::contiguous_iterator Iter, std::sized_sentinel_for<Iter> Sentinel>
                requires std::convertible_to<std::remove_reference_t<std::iter_reference_t<Iter>>(&)[], T(&)[]>
                constexpr slice(Iter begin, Sentinel end) : 
                    raw_slice_ptr{std::addressof(*begin), std::ranges::distance(begin, end)}{}

            template<std::contiguous_iterator Iter>
                requires std::convertible_to<std::remove_reference_t<std::iter_reference_t<Iter>>(&)[], T(&)[]>
                constepxr slice(Iter begin, std::size_t len) noexcept : raw_slice_ptr{std::addressof(*begin), len}{}

            template<typename U,std::size_t N>
                requires std::convertible_to<U(&)[], T(&)[]>
                constexpr slice(U (&array)[N]) noexcept : raw_slice_ptr{array, N}{}

            template<typename U, std::size_t N>
                requires std::convertible_to<U(&)[], T(&)[]>
                constexpr slice(array<U,N>& arr) noexcept : raw_slice_ptr{arr.data(), N}{}
            template<typename U, std::size_t N>
                requires std::convertible_to<const U(&)[], T(&)[]>
                constexpr slice(const array<U,N>& arr) noexcept : raw_slice_ptr{arr.data(), N}{}

            template<typename U, std::size_t N>
                requires std::convertible_to<U(&)[], T(&)[]>
                constexpr slice(std::array<U,N>& arr) noexcept : raw_slice_ptr{arr.data(), N}{}
            template<typename U, std::size_t N>
                requires std::convertible_to<const U(&)[], T(&)[]>
                constexpr slice(const std::array<U,N>& arr) noexcept : raw_slice_ptr{arr.data(), N}{}

            template<typename U>
                requires std::convertible_to<U(&)[], T(&)[]>
                constexpr slice(slice<U> sl) noexcept : raw_slice_ptr{sl.data(), sl.size()}{}

            template<typename U, std::size_t Ex>
                requires std::convertible_to<U(&)[], T(&)[]>
                constexpr slice(std::span<U,Ex>)

            constexpr slice(const slice& sl) noexcept = default;
            constexpr slice(slice&& sl) noexcept = default;

            // This const cast is valid because we never modify the (zero sized) range
            constexpr slice() noexcept : slice(const_cast<array<T,0>&>(_empty_array)){}

            constexpr T* data() const noexcept{
                return _m_ptr.p_data;
            }

            constexpr const T* cdata() const noexcept{
                return _m_ptr.p_data;
            }

            constexpr std::size_t size() const noexcept{
                return _m_ptr.p_len;
            }
        };
    }

    template<typename T> struct optional_niche<crabi::slice::slice<T>>{
        using type = crabi::slice::raw_slice_ptr<T>;
        constexpr const crabi::slice::raw_slice_ptr value{0,0};
    };
}