#include <crabi/option.hxx>
#include <rusty/type_traits.hxx>
#include <rusty/concepts.hxx>

namespace crabi{
    template<typename T> requires (!std::is_const_v<T>) struct ref{
    private:
        const T* _m_inner;
    public:
        ref_mut(const T& val) : _m_inner(&val){}
        ref_mut(const T&&) = delete;

        const T& operator*() const noexcept{
            return *_m_inner;
        }

        const T* operator->() const noexcept{
            return _m_inner;
        }

        template<typename Idx>
            requires rusty::indexable<const T, Idx>
            decltype(auto) operator[](const Idx& idx) const{
                return (**this)[idx];
            }
    };
    template<typename T> requires (!std::is_const_v<T>) struct ref_mut{
    private:
        T* _m_inner;
    public:
        ref_mut(T& val) : _m_inner{&val}{}
        ref_mut(const T&) = delete;
        ref_mut(T&&) noexcept = default;
        ref_mut& operator=(const T&) = delete;
        ref_mut& operator=(T&&) noexcept = default;
        

        T& operator*() noexcept{
            return *_m_inner;
        }

        const T& operator*() const noexcept{
            return *_m_inner;
        }

        T* operator->() noexcept{
            return _m_inner;
        }

        const T* operator->() const noexcept{
            return _m_inner;
        }

        operator ref<T>() const noexcept{
            return ref{*_m_inner};
        }

        template<typename Idx>
            requires rusty::indexable<T, Idx>
            decltype(auto) operator[](const Idx& idx){
                return (**this)[idx];
            }

        template<typename Idx>
            requires rusty::indexable<const T, Idx>
            decltype(auto) operator[](const Idx& idx) const{
                return (**this)[idx];
            }

    };

    template<typename T> struct optional_niche<ref<T>>{
        using type = const T*;
        constexpr inline const T* value = nullptr;
    };

    template<typename T> struct optional_niche<ref_mut<T>>{
        using type = T*;
        constexpr inline T* value = nullptr;
    };
}
