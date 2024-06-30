#pragma once

#include <cstddef>
#include <iterator>
#include <ranges>
#include <algorithm>
#include <rusty/concepts.hxx>
#include <rusty/type_traits.hxx>
#include <crabi/option.hxx>
#include <stdexcept>
#include <format>
#include <tuple>
#include <string_view>

namespace crabi{
    template<typename T, std::size_t N> struct _array_storage{
        T _m_items[N];
        constexpr T* data() noexcept {
            return _m_items;
        }
        constexpr const T* data() const noexcept{
            return _m_items;
        }
    };


    template<typename T, std::size_t N> struct array : _array_storage<T,N>{
    public:
        using element_type = T;
        using reference = T&;
        using const_reference = const T&;
        using iterator = T*;
        using const_iterator = const T*;
        using reverse_iterator = std::reverse_iterator<T*>;
        using const_reverse_iterator = std::reverse_iterator<const T*>;

        constexpr std::size_t size() const noexcept{
            return N;
        }

        constexpr bool empty() const noexcept{
            return !N;
        }

        constexpr iterator begin() noexcept{
            return this->data();
        }

        constexpr const_iterator begin() const noexcept{
            return this->data();
        }

        constexpr const_iterator cbegin() const noexcept{
            return this->begin();
        }
        constexpr iterator end() noexcept{
            return this->data()+N;
        }
        constexpr const_iterator end() const noexcept{
            return this->data()+N;
        }

        constexpr const_iterator cend() const noexcept{
            return this->data()+N;
        }

        constexpr reverse_iterator rbegin() noexcept{
            return std::reverse_iterator{this->end()};
        }
        constexpr const_reverse_iterator rbegin() const noexcept{
            return std::reverse_iterator{this->end()};
        }
        constexpr const_reverse_iterator crbegin() const noexcept{
            return std::reverse_iterator{this->end()};
        }
        constexpr reverse_iterator rend() noexcept{
            return std::reverse_iterator{this->begin()};
        }
        constexpr const_reverse_iterator rend() const noexcept{
            return std::reverse_iterator{this->begin()};
        }
        constexpr const_reverse_iterator crend() const noexcept{
            return std::reverse_iterator{this->begin()};
        }

        constexpr void swap(array<T,N>& a) noexcept(std::is_nothrow_swappable_v<T>) requires std::swappable<T>{
            std::ranges::swap_ranges(*this, a);
        }

        constexpr friend void swap(array<T,N>& a1, array<T,N>& a2) noexcept(std::is_nothrow_swappable_v<T>) requires std::swappable<T>{
            std::ranges::swap_ranges(a1, a2);
        }

        constexpr reference operator[](std::size_t l) noexcept{
            return this->data()[l];
        }

        constexpr reference at(std::size_t l) {
            using namespace std::string_view_literals;
            if (l >= N) [[unlikely]]
                throw std::out_of_range{std::format("Index {} out of bounds for array with length {}"sv, l, N)};
            return this->data()[l];
        }

        constexpr crabi::option<reference> get_ref(std::size_t l) noexcept {
            if (l >= N)
                return crabi::none;
            else
                return crabi::some(this->data()[1]);
        }

        constexpr const_reference operator[](std::size_t l) const noexcept{
            return this->data()[l];
        }

        constexpr const_reference at(std::size_t l) const {
            using namespace std::string_view_literals;
            if (l >= N) [[unlikely]]
                throw std::out_of_range{std::format("Index {} out of bounds for array with length {}"sv, l, N)};
            return this->data()[l];
        }

        constexpr crabi::option<const_reference> get_ref(std::size_t l) const noexcept {
            if (l >= N)
                return crabi::none;
            else
                return crabi::some(this->data()[1]);
        }

        
        template<std::size_t I> requires (I < N) reference get() & noexcept{
            return this->data()[I];
        }

        template<std::size_t I> requires (I < N) const_reference get() const & noexcept{
            return this->data()[I];
        }

        template<std::size_t I> requires (I < N) T&& get() && noexcept{
            return std::move(this->data()[I]);
        }

        template<std::size_t I> requires (I < N) const T&& get() const&& noexcept{
            return std::move(this->data()[I]);
        }

        template<std::size_t I> requires (I < N) friend reference get(array<T, N>& arr) noexcept{
            return arr.get();
        }
        template<std::size_t I> requires (I < N) friend const_reference get(const array<T,N>& arr) noexcept{
            return arr.get();
        }

        template<std::size_t I> requires (I < N) friend T&& get(array<T, N>&& arr) noexcept{
            return std::move(arr.get());
        }
        template<std::size_t I> requires (I < N) friend const_reference get(const array<T,N>&& arr) noexcept{
            return std::move(arr.get());
        }
    };

    template<typename T, typename... Ts> requires (std::same_as<T, Ts> && ...) array(T, Ts...) -> array<T,(1 + sizeof...(Ts))>;

}

namespace std{
    template<typename T, std::size_t N> struct tuple_size<crabi::array<T,N>> : std::integral_constant<std::size_t, N>{};

    template<typename T, std::size_t N, std::size_t I> requires (I<N) struct tuple_element<I, crabi::array<T,N>> : std::type_identity<T>{};
}