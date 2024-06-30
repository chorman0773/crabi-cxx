#include <cstdlib>
#include <iostream>
#include <format>
#include <concepts>
#include <iterator>
#include <utility>
#include <source_location>
#include <string_view>

template<typename... Args> [[noreturn]] void assertion_failed(std::source_location loc, std::format_string<Args...> fmt, Args&&... args) noexcept{
    std::cerr << "Assertion Failed [" << loc.file_name() << " @ " << loc.line() << ":" << loc.column() << "]: ";
    std::format_to(std::ostream_iterator<char>{std::cerr}, fmt, std::forward<Args>(args)...);
    std::cerr << std::endl;
    std::abort();
}

template<typename T, typename U> requires std::equality_comparable_with<T,U>
    void assert_eq(T&& t, U&& u, std::source_location loc = std::source_location::current()) noexcept{
        using namespace std::string_view_literals;
        if(t !=u){
            assertion_failed(loc, "left!=right: left={}, right={}"sv, t, u);
        }
    }

template<typename T, typename U, typename... Args> requires std::equality_comparable_with<T,U>
    void assert_eq(T&& t, U&& u, std::format_string<Args...> fmt, Args&&... args, std::source_location loc = std::source_location::current()) noexcept{
        if(t != u){
            assertion_failed(loc, fmt, std::forward<Args>(args)...);
        }
    }