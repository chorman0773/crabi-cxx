#pragma once

#include <optional>
#include <concepts>
#include <bit>
#include <utility>
#include <stdexcept>
#include <functional>
#include <compare>

#include <rusty/type_traits.hxx>

namespace crabi{
    /// @brief A type trait that can be specialized for a type `T` to indicate that it is subject to niche-optimization for `T`
    /// @tparam T The type to niche optimize
    /// 
    /// This trait can be specialized for a type `T` if it satisfies the following conditions:
    /// * `T` models `std::trivially_copyable`
    /// * The specialization of `optional_niche` defines a member type `type`, which models `std::trivially_copyable` and `std::equality_comparable`
    /// * The specialization of `optional_niche` defines a static data member `value`, which is of the type denoted by `type`,
    /// * Equality Comparison of `type` is equivalent to equality testing each byte in the value-representation of `type`
    /// * `T` and the member type are the same size
    /// * No constructible value of `T`, when converted to the member `type` via `std::bit_cast` will compare equal to `value`
    template<typename T> requires std::is_object_v<T> struct optional_niche{};


    template<typename T> using optional_niche_t = typename optional_niche<T>::type;
    template<typename T> constexpr inline std::same_as<optional_niche_t<T>> auto optional_niche_v =
         optional_niche<T>::value;


    
    namespace _detail{
        template<typename T> concept _optional_storage_niche = requires{
            typename optional_niche_t<T>;
        };
        template<typename T> struct _optional_storage{
        private:
            struct _storage_none{bool _m_engaged;};
            struct _storage_some{bool _m_engaged; T _m_val;};
            union{
                struct {bool _m_engaged;};
                _storage_none _m_none;
                _storage_some _m_some;
            };
        public:
            constexpr _optional_storage() noexcept :  _m_none{false}{}
            constexpr bool _has_value() const noexcept{
                return this->_m_engaged;
            }

            constexpr T& _get_value() noexcept{
                return this->_m_some._m_val;
            }

            constexpr const T& _get_value() const noexcept{
                return this->_m_some._m_val;
            }

            template<typename... Args> 
                constexpr void _emplace(Args&&... args) {
                    ::new(&_m_some) _storage_some{true,T{std::forward<Args>(args)...}};
                }

            constexpr void _destroy() noexcept{
                if(std::exchange(_m_engaged, false)){
                    _m_some._m_val.~T();
                }
            }

            constexpr const T* _get_storage() const noexcept{
                return &this->_m_some._m_val;
            }
        };

        template<typename T> requires std::is_reference_v<T>  struct _optional_storage<T>{
        private:
            std::remove_reference_t<T>* _m_val;
        public:
            constexpr _optional_storage() noexcept : _m_val{} {}
            constexpr bool _has_value() const noexcept{
                return static_cast<bool>(_m_val);
            }
            constexpr T& _get_value() noexcept{
                return *_m_val;
            }
            constexpr const T& _get_value() const noexcept{
                return *_m_val;
            }

            constexpr void _destroy() noexcept{
                _m_val = nullptr;
            }

            template<typename U> requires std::convertible_to<U(&&)[], T(&&)[]> && (std::is_lvalue_reference_v<T> == std::is_lvalue_reference_v<U>)
                void _emplace(U&& ref){
                    _m_val = &ref;
                }

            template<typename U> requires (std::is_lvalue_reference_v<T> && !std::is_lvalue_reference_v<U>)
                void _emplace(const U&&) = delete;
        };

        template<typename T> requires (!std::is_reference_v<T> && _optional_storage_niche<T>) struct _optional_storage<T>{
        private:
            using _storage_type = std::aligned_storage_t<sizeof(T), alignof(T)>;
            union{
                optional_niche_t<T> _m_niche_val;
                T _m_val;
            } _m_storage;
        public:
            constexpr _optional_storage() noexcept : _m_storage{._m_niche_val = optional_niche_v<T>}{}

            constexpr bool _has_value() const noexcept{
                auto val = std::bit_cast<optional_niche_t<T>>(_m_storage);

                return val!=optional_niche_v<T>;
            }

            constexpr T& _get_value() noexcept{
                return _m_storage._m_val;
            }
            constexpr const T& _get_value() const noexcept{
                return _m_storage._m_val;
            }

            constexpr void _destroy() noexcept{
                _m_storage._m_niche_val = optional_niche_v<T>;
            }

            template<typename... Args> constexpr void _emplace(Args&&... args){
                ::new(&_m_storage._m_val) T{std::forward<Args>(args)...};
            }

            constexpr const T* _get_storage() const noexcept{
                return &this->_m_val;
            }
        };
    }

    template<typename T> struct option;

    template<typename T> struct some_t;

    struct none_t;

    struct bad_unwrap : std::exception{
    private:
        template<typename T> friend struct option;
        bad_unwrap() = default;
    public:
        const char* what() const noexcept override{
            return "Attempt to call unwrap on a `none` value";
        }
    };


    template<typename T> struct option : private _detail::_optional_storage<T>{
    public:

        using value_type = T;
        using reference = T&;
        using const_reference = const T&;


        constexpr option(std::nullopt_t = std::nullopt){}

        template<typename U = T> requires std::constructible_from<T, U&&> 
            && (!rusty::is_specialization_v<std::remove_cvref_t<U>, option>)
            && (!rusty::is_specialization_v<std::remove_cvref_t<U>,std::optional>)
            && (!rusty::is_specialization_v<std::remove_cvref_t<U>, some_t>)
            && (!std::same_as<std::remove_cvref_t<U>, std::nullopt_t>)
            && (!std::same_as<std::remove_cvref_t<U>,std::in_place_t>)
            && (!std::same_as<std::remove_cvref_t<U>, none_t>)
            && (std::is_lvalue_reference_v<T> == std::is_lvalue_reference_v<T>)
        constexpr explicit(!std::convertible_to<U, T>) option(U&& val) noexcept(std::is_nothrow_constructible_v<T, U&&>) {
            this->_emplace(std::forward<U>(val));
        }

        template<typename U> requires (std::is_lvalue_reference_v<T> != std::is_lvalue_reference_v<T>)
            option(const U&&) = delete;

        template<typename... Args> requires std::constructible_from<T, Args&&...>&&(!std::is_reference_v<T>)
            constexpr explicit option(std::in_place_t, Args&&... args){
                this->_emplace(std::forward<Args>(args)...);
            }

        template<typename U> requires std::is_reference_v<T> && std::constructible_from<T, U&&> && (!std::is_lvalue_reference_v<T> || std::is_lvalue_reference_v<T>)
            constexpr explicit option(std::in_place_t, U&& u){
                this->_emplace(std::forward<U>(u));
            }

        template<typename U> requires std::is_reference_v<T> && std::constructible_from<T, U&&> && (std::is_lvalue_reference_v<T> && !std::is_lvalue_reference_v<T>)
            option(std::in_place_t, const U&&)=delete;


        constexpr option(const option& ) noexcept
         requires std::copy_constructible<T> && std::is_trivially_copy_constructible_v<T> = default;
        
        constexpr option(const option& opt) requires std::copy_constructible<T> && (!std::is_trivially_copy_constructible_v<T>){
            if (opt._has_value())
                this->_emplace(*opt);
        }

        constexpr option(option&&) noexcept 
            requires std::move_constructible<T> && std::is_trivially_move_constructible_v<T> = default;

        constexpr option(option&& opt) noexcept(std::is_nothrow_move_constructible_v<T>)
            requires std::move_constructible<T> && (!std::is_trivially_move_constructible_v<T>){
                if(opt._has_value()){
                    this->_emplace(std::forward<T>(*opt));
                    opt._destroy();
                }
            }

        template<typename U>
            requires std::constructible_from<T, const U&>
            constexpr explicit(!std::convertible_to<const U&,T>)
                option(const option<U>& val){
                    if(val._has_value()){
                        this->_emplace(*val);
                    }
                }
        
        template<typename U>
            requires std::constructible_from<T,U&&>
            constexpr explicit(!std::convertible_to<U,T>)
                option(option<U>&& val) noexcept(std::is_nothrow_constructible_v<T,U&&>){
                    if(val._has_value()){
                        this->_emplace(std::forward<T>(*val));
                        val._destroy();
                    }
                }
        template<typename U>
            requires std::constructible_from<T, const U&>
            constexpr explicit(!std::convertible_to<const U&,T>)
                option(const std::optional<U>& opt){
                    if(opt){
                        this->_emplace(*opt);
                    }
                }
        template<typename U>
            requires std::constructible_from<T, U&&>
            constexpr explicit(!std::convertible_to<U&&,T>)
                option(std::optional<U>&& opt)  noexcept(std::is_nothrow_constructible_v<T,U&&>){
                    if(opt){
                        this->_emplace(*opt);
                        opt.reset();
                    }
                }

        constexpr ~option() noexcept{
            this->_destroy();
        }

        constexpr bool has_value() const noexcept{
            return this->_has_value();
        }

        constexpr operator bool() const noexcept{
            return this->_has_value();
        }

        constexpr void reset() noexcept{
            this->_destroy();
        }

        constexpr option take() noexcept requires std::is_move_constructible_v<T>{
            return std::move(*this);
        }
        
        constexpr option& operator=(std::nullopt_t) noexcept{
            this->_destroy();
        }

        constexpr option& operator=(const option&) noexcept requires (std::copy_constructible<T> && std::is_copy_assignable_v<T>
            && std::is_trivially_copy_assignable_v<T>) || std::is_reference_v<T> = default;

        constexpr option& operator=(const option& opt) requires std::copy_constructible<T> && std::is_copy_assignable_v<T>
            && (!std::is_trivially_copy_assignable_v<T>) && (!std::is_reference_v<T>){
                if(opt){
                    if(*this){
                        this->_get_value() = opt._get_value();
                    }else{
                        this->_emplace(opt._get_value());
                    }
                }else{
                    this->_destroy();
                }
            }


        constexpr option& operator=(option&& ) noexcept requires (std::move_constructible<T> && std::is_move_assignable_v<T>
            && std::is_trivially_move_assignable_v<T>) || std::is_reference_v<T> = default;

        constexpr option& operator=(option&& opt) noexcept requires std::move_constructible<T> && std::is_move_assignable_v<T>
            && (!std::is_trivially_move_assignable_v<T>) && (!std::is_reference_v<T>) {
                if(opt){
                    if(*this){
                        this->_get_value() = std::forward<T>(opt._get_value());
                    }else{
                        this->_emplace(std::forward<T>(opt._get_value()));
                    }
                    opt._destroy();
                }
            }
        

        template<typename... Args> 
            requires std::constructible_from<T,Args&&...> 
            constexpr void emplace(Args&&... args) noexcept(std::is_nothrow_constructible_v<T,Args&&...>){
                this->_destroy();
                this->_emplace(std::forward<Args>(args)...);
            }

        constexpr void swap(option& other) noexcept requires std::is_reference_v<T>{
            option tmp{std::move(other)};
            other = std::move(*this);
            *this = std::move(tmp);
        }

        constexpr void swap(option& other) noexcept(std::is_nothrow_swappable_v<T> && std::is_nothrow_move_constructible_v<T>) requires std::swappable<T> && std::move_constructible<T> && (!std::is_reference_v<T>){
            if(other && *this){
                using std::swap;
                swap(this->_get_value(), other._get_value());
            }else if(other){
                this->_emplace(std::move(other._get_value()));
                other._destroy();
            }else if(*this){
                other._emplace(this->_get_value());
                this->_destroy();
            }
        }

        template<typename Ty> requires requires(option<Ty>& opt1, option<Ty>& opt2){
            opt1.swap(opt2);
        } constexpr friend void swap(option<Ty>& opt1, option<Ty>& opt2) noexcept(noexcept(opt1.swap(opt2))){
            opt1.swap(opt2);
        }

        constexpr T& operator*() & noexcept {
            return this->_get_value();
        }

        constexpr const T& operator*() const& noexcept{
            return this->_get_value();
        }

        constexpr T&& operator*() && noexcept{
            return std::forward<T>(this->_get_value());
        }

        constexpr const T&& operator*() const && noexcept{
            return std::forward<T>(this->_get_value());
        }


        constexpr T& unwrap() &{
            if(!this->_has_value())
                throw bad_unwrap{};
            else
                return this->_get_value();
        }

        constexpr const T& unwrap() const&{
            if(!this->_has_value())
                throw bad_unwrap{};
            else
                return this->_get_value();
        }
        constexpr T&& unwrap() &&{
            if(!this->_has_value())
                throw bad_unwrap{};
            else
                return std::forward<T>(this->_get_value());
        }

        constexpr const T&& unwrap() const&&{
            if(!this->_has_value())
                throw bad_unwrap{};
            else
                return std::forward<T>(this->_get_value());
        }

        template<typename U> requires std::constructible_from<T,U&&> && std::copy_constructible<T> 
            T unwrap_or(U&& val) const{
                if(this->_has_value())
                    return this->_get_value();
                else 
                    return T(std::forward<U>(val));
            }

        template<typename U> requires std::constructible_from<T,U&&> && std::move_constructible<T>
            T unwrap_or(U&& val) && noexcept(std::is_nothrow_constructible_v<T, U&&> && std::is_nothrow_move_constructible_v<T>) {
                if(this->_has_value())
                    return std::forward<T>(this->_get_value());
                else
                    return T(std::forward<U>(val));
            }

        template<typename F> requires std::invocable<F&&> && std::convertible_to<std::invoke_result_t<F&&>, T> && std::copy_constructible<T>
            T unwrap_or_else(F&& def) const{
                if(this->_has_value())
                    return this->_get_value();
                else
                    return std::invoke(std::forward<F>(def));
            }


        option<T&> as_ref() & noexcept{
            if(this->_has_value())
                return option<T&>{this->_get_value()};
            else
                return option<T&>{};
        }

        option<const T&> as_ref() const& noexcept{
            if(this->_has_value())
                return option<const T&>{this->_get_value()};
            else
                return option<const T&>{};
        }
        
        option<T&&> as_ref() && noexcept{
            if(this->_has_value())
                return option<T&&>{std::forward<T>(this->_get_value())};
            else
                return option<T&&>{};
        } 
        
        option<const T&&> as_ref() const&& noexcept{
            if(this->_has_value())
                return option<const T&&>{std::forward<T>(this->_get_value())};
            else
                return option<const T&&>{};
        } 

        template<typename F> requires std::invocable<F&&, const T&>
            option<std::invoke_result_t<F, const T&>> map(F&& f) const noexcept(std::is_nothrow_invocable_v<F,const T&>){
                if(this->_has_value())
                    return option{std::invoke(std::forward<F>(f), this->_get_value())};
                else
                    return std::nullopt;
            }

        template<typename F> requires std::invocable<F&&, T&&>
            option<std::invoke_result_t<F&&, T&&>> map(F&& f) && noexcept(std::is_nothrow_invocable_v<F,T&&>){
                if(this->_has_value())
                    return option{std::invoke(std::forward<F>(f), std::forward<T>(this->_get_value()))};
                else
                    return std::nullopt;
            }

        template<typename F> requires std::invocable<F&&, const T&> && rusty::is_specialization_v<std::invoke_result_t<F&&, const T&>, option>
            std::invoke_result_t<F&&, const T&> and_then(F&& f) const noexcept(std::is_nothrow_invocable_v<F&&, const T&>) {
                if(this->_has_value())
                    return std::invoke(std::forward<F>(f), this->_get_value());
                else
                    return std::nullopt;
            }

        template<typename F> requires std::invocable<F&&, T&&> && rusty::is_specialization_v<std::invoke_result_t<F&&, T&&>, option>
            std::invoke_result_t<F&&, T&&> and_then(F&& f) && noexcept(std::is_nothrow_invocable_v<F&&, T&&>) {
                if(this->_has_value())
                    return std::invoke(std::forward<F>(f), std::forward<T>(this->_get_value()));
                else
                    return std::nullopt;
            }

        template<typename F> requires std::invocable<F&&> && std::convertible_to<std::invoke_result_t<F&&>, option<T>>
            constexpr option<T> or_else(F&& f) const requires std::copy_constructible<T>{
                if(this->_has_value())
                    return *this;
                else
                    return std::invoke(std::forward<F>(f));
            }

        template<typename F> requires std::invocable<F&&> && std::convertible_to<std::invoke_result_t<F&&>, option<T>>
            constexpr option<T> or_else(F&& f) && noexcept(std::is_nothrow_move_constructible_v<T> && std::is_nothrow_invocable_v<F&&>) requires std::move_constructible<T> {
                if(this->_has_value())
                    return std::move(*this);
                else
                    return std::invoke(std::forward<F>(f));
            }

        constexpr T flatten() const requires std::copy_constructible<T> && rusty::is_specialization_v<T, option>{
            if(this->_has_value())
                return *this;
            else
                return std::nullopt;
        }

        constexpr T flatten() && noexcept(std::is_nothrow_move_constructible_v<T>) requires std::move_constructible<T> && rusty::is_specialization_v<T,option>{
            if(this->_has_value())
                return std::move(*this);
            else
                return std::nullopt;
        }

        template<typename U> requires std::move_constructible<U>
            constexpr crabi::option<U> and_then(crabi::option<U> opt) const noexcept(std::is_nothrow_move_constructible_v<T>){
                if(this->has_value())
                    return opt;
                else
                    return std::nullopt;
            }

        template<typename U> bool operator==(const crabi::option<T>& opt) const requires std::equality_comparable_with<T,U>{
            if(this->_has_value())
                return opt._has_value() && this->_get_value() == opt._get_value();
            else
                return !opt._has_value();
        }

        template<typename U> 
            constexpr std::compare_three_way_result_t<T,U> operator<=>(const crabi::option<U>& opt) const noexcept(noexcept((**this) <=> *opt)) requires std::three_way_comparable_with<T,U>{
                switch(static_cast<int>(this->_has_value())<< 1 | static_cast<int>(opt._has_value())){
                    case 0: return std::strong_ordering::equivalent;
                    case 1: return std::strong_ordering::less;
                    case 2: return std::strong_ordering::greater;
                    case 3: return this->_get_value() <=> opt._get_value();
                }
            }

        template<typename Ty, typename U> requires requires(const std::remove_reference_t<Ty>& t, const std::remove_reference_t<U>& u){
            std::strong_order(t,u);
        }
            constexpr friend std::strong_ordering strong_order(const crabi::option<Ty>& t, const crabi::option<U>& u) noexcept(noexcept(std::strong_order(*t, *u))){
                switch(static_cast<int>(t._has_value())<< 1 | static_cast<int>(u._has_value())){
                    case 0: return std::strong_ordering::equivalent;
                    case 1: return std::strong_ordering::less;
                    case 2: return std::strong_ordering::greater;
                    case 3: return std::strong_order(t._get_value(), u._get_value());
                }
            }

        template<typename Ty, typename U> requires requires(const std::remove_reference_t<Ty>& t, const std::remove_reference_t<U>& u){
            std::weak_order(t,u);
        }
            constexpr friend std::weak_ordering weak_order(const crabi::option<Ty>& t, const crabi::option<U>& u) noexcept(noexcept(std::weak_order(*t, *u))){
                switch(static_cast<int>(t._has_value())<< 1 | static_cast<int>(u._has_value())){
                    case 0: return std::strong_ordering::equivalent;
                    case 1: return std::strong_ordering::less;
                    case 2: return std::strong_ordering::greater;
                    case 3: return std::weak_order(t._get_value(), u._get_value());
                }
            }
    };

    template<typename T> requires (!rusty::is_specialization_v<T,crabi::option>) && (!rusty::is_specialization_v<T,std::optional>) && (!std::is_same_v<T,std::in_place_t>)
        explicit option(T) -> option<T>;

    

    struct none_t{
        template<typename T> constexpr bool matches(const option<T>& opt) noexcept{
            return !opt.has_value();
        }
        template<typename T> constexpr std::tuple<> bind(option<T>&& opt) noexcept{
            return std::tuple<>{};
        }

        template<typename T> constexpr operator option<T>() noexcept{
            return option<T>{};
        }

        template<typename T> requires std::equality_comparable<T> constexpr bool operator==(const option<T>& opt) const noexcept{
            return !opt._has_value();
        }

        template<typename T> requires std::three_way_comparable<T> constexpr std::compare_three_way_result_t<T,T> operator<=>(const option<T>& opt) const noexcept{
            if(opt._has_value())
                return std::strong_ordering::less;
            else
                return std::strong_ordering::equivalent;
        }

    };

    template<typename T> struct some_t{
        T _m_val;

        constexpr operator option<T>() const noexcept requires std::copy_constructible<T>{
            return option<T>{std::in_place, _m_val};
        }

        constexpr operator option<T>() && noexcept requires std::move_constructible<T>{
            return option<T>{std::in_place, std::move(_m_val)};
        }

        template<typename U> requires std::equality_comparable_with<T,U> constexpr bool operator==(const option<U>& opt) const noexcept(noexcept(_m_val==*opt)){
            return opt.has_value() && _m_val == *opt;
        }

        template<typename U> requires std::three_way_comparable_with<T,U> constexpr std::compare_three_way_result_t<T,U>
            operator<=>(const option<U>& opt) const noexcept(noexcept(_m_val <=> *opt)){ 
                if(opt._has_value())
                    return _m_val <=> *opt;
                else
                    return std::strong_ordering::greater;
            }
    };

    constexpr inline none_t none;

    constexpr inline auto some = [] <typename T> (T&& val) noexcept(std::is_nothrow_constructible_v<T, T&&>) -> some_t<T>{
        return some_t<T>{std::forward<T>(val)};
    };
}
