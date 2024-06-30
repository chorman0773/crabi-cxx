# Header `<crabi/option.hxx>` [crabi.option]

## Header `<crabi/option.hxx>` Synopsis [crabi.option.syn]

```c++
namespace crabi{
    template<typename T> struct option_niche;

    template<typename T> using option_niche_t = typename option_niche<T>::type;
    template<typename T> constexpr inline option_niche_t<T> option_niche_v = option_niche<T>::value;

    struct bad_unwrap;

    template<typename T> struct option;

    struct none_t;

    constexpr inline none_t none = /*see below*/;

    template<typename T> struct some_t;

    template<typename T> constexpr some_t<T> some(T) noexcept(/*see below*/);
}
```

## Class template `option_niche` [crabi.option.option_niche]

```c++
// in namespace crabi
template<typename T> struct option_niche{};

```

1. The primary template definition of `option_niche<T>` has no explicitly-defined member functions.

2. *Requires*: T is a complete object type or an array of unknown bound. 

3. The program may define a partial or explicit specialization for `crabi::option_niche<T>` where `T` depends on a user-defined type, if:
    * `T` models `std::trivially_copyable`,
    * The specialization defines a member type, `type`, which is an alias of some type `U`, which is a *literal type*,
    * The program defines a `constexpr` static data member `value`, of type `U`,
    * `U` models `std::trivially_copyable` and `rusty::structural_equality`,
    * `sizeof(T)==sizeof(U)` and `alignof(T)==alignof(U)`,
    * Given an lvalue of type `T`, t, which refers to an object that was not trivially default-initialized, `std::bit_cast<U>(t)` results in a value of type `U` that does not compare equal to the member `value`.

4. *Remarks*: The behaviour is undefined if the program defines a partial or explicit specialization for `crabi::option_niche<T>` where the requirements in clause 3 are not satisfied.

```c++
template<typename T> using option_niche_t = typename option_niche<T>::type;
template<typename T> constexpr inline option_niche_t<T> option_niche_v = option_niche<T>::value;
```

5. *Requires*: `crabi::option_niche<T>` refers to a partial or explicit specialization of `crabi::option_niche`.

5. `crabi::option_niche_t<T>` is a alias of the member `type` of `crabi::option_niche<T>`.

6. `crabi::option_niche_v<T>` is a value equivalent to the value of the memebr `value` of `crabi::option_niche<T>`.

## Class `bad_unwrap` [crabi.option.bad_unwrap]

```c++
struct bad_unwrap : std::exception{
public:
    const char* what() const noexcept;
};
```

1. The `bad_unwrap` class is thrown by `option::unwrap` when called on an empty option.

2. The `bad_unwrap` class has no accessible constructor, other than an implicitly-defined copy and move constructor.

```c++
const char* what() const noexcept;
```

3. Returns an unspecified string that contains `"called unwrap on a none value"`

## Class Template `option` [crabi.option.option]

1. *Mandates*: `T` is a complete object type or a reference type, such that `std::remove_reference_t<T>` is an object type.

2. If `T` is a reference type or satisfies `std::trivially_copyable`, then `crabi::option<T>` satisfies `std::trivially_copyable`.

3. If `T` models `rusty::structural_equality`, then `crabi::option<T>` models `rusty::structural_equality`.


### Class Template `option` synopsis [crabi.option.option.syn]

```c++
template<typename T> struct option<T>{
public:
    constexpr option() noexcept;
    constexpr option(std::nullopt_t) noexcept;

    constexpr option(const option& opt) noexcept(/*see below*/);
    constexpr option(option&& opt) noexcept(/*see below*/);

    template<typename U>
        constexpr option(U&&) noexcept(/*see below*/);
    template<typename... Args>
        constexpr option(std::in_place_t, Args&&... args) noexcept(/*see below*/);

    template<typename U>
        explicit(/*see below*/) constexpr option(const option<U>& opt);
    template<typename U>
        explicit(/*see below*/) constexpr option(option<U>&& opt) noexcept(/*see below*/);

    template<typename U>
        explicit(/*see below*/) constexpr option(const std::optional<U>& opt);
    template<typename U>
        explicit(/*see below*/) constexpr option(std::optional<U>&& opt) noexcept(/*see below*/);

    constexpr ~option() noexcept;


    constexpr option& operator=(const option& opt) noexcept(/*see below*/);
    constexpr option& operator=(option&& opt) noexcept(/*see below*/);

    constexpr option& operator=(std::nullopt_t) noexcept(/*see below*/);

    template<typename... Args>
        constexpr void emplace(Args&&... args) noexcept(/*see below*/);

    constexpr void swap(crabi::option<T>& b) noexcept(/*see below*/);

    constexpr bool has_value() const noexcept;
    constexpr explicit operator bool() const noexcept;

    constexpr T& operator*() & noexcept;
    constexpr T&& operator*() && noexcept;
    constexpr const T& operator*() const & noexcept;
    constexpr const T&& operator*() const && noexcept;

    constexpr T& value() & noexcept;
    constexpr T&& value() && noexcept;
    constexpr const T& value() const & noexcept;
    constexpr const T&& value() const && noexcept;


    template<typename U>
        constexpr T unwrap_or(U&&) const;
    
    template<typename U>
        constexpr T unwrap_or(U&&) && noexcept(/*see below*/);

    constexpr T* operator->() noexcept;
    constexpr const T* operator->() const noexcept;

    template<typename F>
        constexpr option<std::invoke_result_t<F&&, const T&>> map(F&& f) const noexcept(std::is_nothrow_invocable_v<F&&, const T&>);
    template<typename F>
        constexpr option<std::invoke_result_t<F&&, T&&>> map(F&& f) && noexcept(std::is_nothrow_invocable_v<F&& , T&&>);

    template<typename F>
        constexpr /*see below*/ and_then(F&& f) const noexcept(std::is_nothrow_invocable_v<F&&, T&&>);
    template<typename F>
        constexpr /*see below*/ and_then(F&& f) && noexcept(std::is_nothrow_invocable_v<F&&, const T&>);

    template<typename U>
        constexpr option<U> and_then(const option<U>& v) const noexcept(/*see below*/);
    template<typename U>
        constexpr option<U> and_then(option<U>&& v) const noexcept(/*see below*/);

    template<typename F>
        constexpr option<T> or_else(F&& f) const noexcept(std::is_nothrow_invocable_v<F&&, const T&>);
    template<typename F>
        constexpr option<T> or_else(F&& f) const noexcept(std::is_nothrow_invocable_v<F&&, const T&>);

    
    
    
};
template<typename T, typename U> constexpr std::strong_ordering strong_order(const crabi::option<T>& a, const crabi::option<U>& b) const noexcept(/*see below*/);
template<typename T, typename U> constexpr std::weak_ordering weak_order(const crabi::option<T>& a, const crabi::option<U>& b) const noexcept(/*see below*/);

template<typename T> constexpr void swap(crabi::option<T>& a, crabi::option<T>& b) noexcept(/*see below*/);
```

### Class template `option` Layout

1. If `crabi::option_niche<T>` refers to a partial or explicit specialization of the template `crabi::option_niche`, then the type `crabi::option<T>` satisfies the following properties:
    * An object `o` of type `crabi::option<T>` is *pointer-interconvertible* with `crabi::option_niche_t`,
    * `crabi::option<T>` has the same size and alignment requirement as `crabi::option_niche_t`,
    * Given a value `o` of type `crabi::option`, `std::bit_cast<crabi::option_niche_t<T>>(o)` compares equal to `crabi::option_niche_v` if `o` is empty, and `std::bit_cast<crabi::option_niche_t<T>>(o)` compares equal to `std::bit_cast<crabi::option_niche_t<T>>(*o)` if `o` is not empty.
2. If `T` is a reference type, then type `crabi::option<T>` satisfies the following properties, given `U` is the type `std::remove_reference_t<T>`:
    * An object `o` of type `crabi::option<T>` is *pointer-interconvertible* with `U*`,
    * `crabi::option<T>` has the same size and alignment requirement as `U*`,
    * Given a value `o` of type `crabi::option`, `std::bit_cast<U*>(o)` compares equal to `nullptr` if `o` is empty, and `std::bit_cast<U*>(o)` compares equal to `std::addressof(*o)` if `o` is not empty.
3. Otherwise, `crabi::option<T>` is *pointer-interconvertible* with `bool`, such that given an (potentially const-qualified) lvalue `o` of type `crabi::option<T>`, `reinterpret_cast<const bool&>(o)` is an equivalent expression to `static_cast<bool>(o)`. 

### Constructors and Destructor [crabi.option.ctor]

```c++
    constexpr option() noexcept; 
    constexpr option(std::nullopt_t) noexcept;
```

1. Constructs an empty `crabi::option`.

2. If `T` is a reference type, equivalent to value-initializing the contained `U*` member.

3.  If `T` is not a reference type, and `crabi::option_niche<T>` refers to the primary template of `crabi::option_niche`, then value-initializing the `option` zero-initializes the storage in which the subobject of type `T` would occupy within the `crabi::option<T>` if the `crabi::option<T>` value was not empty.


```c++
    constexpr option(const option& opt) noexcept(/*see below*/); 
```

4. Copy-constructs from `opt`,

5. *Requires*: `T` is a reference type, or `T` satisfies `std::copy_constructible`. The function is defined as deleted otherwise.

6. *Preconditions*: `T` is a reference type or `T` models `std::copy_constructible`.

7. *Postconditions*: If `opt` contains a value, then the resulting `option` contains a value that is copy-constructed from `*opt`. Otherwise the resulting `option` does not contain a value.

8. The noexcept specification of the function is as follows:
   * If `T` is a reference type, `noexcept(true)`
   * Otherwise, `noexcept(std::is_nothrow_copy_constructible_v<T>)`

9. The function is defined as default and is trivial if either `T` is a reference type or `std::is_trivially_copy_constructible_v<T>` is true.

```c++
   constexpr option(option&& opt) noexcept(/*see below*/);
```

10. Move-constructs from `opt`

11. *Requires*: `T` is a reference type, or `T` satisfies `std::move_constructible`. The function is defined as deleted otherwise.

12. *Preconditions*: `T` is a reference type or `T` models `std::move_constructible`.

13. *Postconditions*: If `opt` contains a value, then the resulting `option` contains a value that is move-constructed from `*opt`. Otherwise the resulting `option` does not contain a value.

14. The noexcept specification of the function is as follows:
    * If `T` is a reference type, `noexcept(true)`
    * Otherwise, `noexcept(std::is_nothrow_move_constructible_v<T>)`

15. The function is defined as default and is trivial if either `T` is a reference type or `std::is_trivially_move_constructible_v<T>` is true.

```c++
template<typename U>
    explicit(/*see below*/) constexpr option(U&& u) noexcept(/*see below*/);
```

16. Creates a new `option` that contains the value given by `u`

17. *Requires*: `T` satisfies `std::constructible_from<U&&>`, and `std::remove_cvref_t<U>` is not a specialization of any of the following types:
    * `std::in_place_t`
    * `std::optional`
    * `crabi::option`
    * `crabi::some_t`
    * `crabi::none_t`.

18. *Postconditions*: The resulting `option` contains the value `T(std::forward<U>(u))`.

19. If `T` is an lvalue-reference type, defined as deleted unless `U` is an lvalue reference type.

20. The function is `explicit` unless `U` satisfies `std::convertible_to<T>`

21. The noexcept specification is:
    * If `T` is a reference type, `noexcept(true)`,
    * If `T` is not a reference type, `noexcept(std::is_nothrow_constructible_v<T,U&&>)`

```c++
template<typename... Args>
    explicit constexpr option(std::in_place_t, Args&&... args) noexcept(/*see below*/);
```

21. Creates a new `option` that is constructed from `args...`

22. *Requires*: 
    * If `T` is a reference type, `sizeof...(Args)==1` and `T` satisfies `std::constructible_from<Args&&...>`,
    * If `T` is not a reference type, `T` satisfies `std::constructible_from<Args&&...>`

23. *Postconditions*: The resulting `option` contains the value `T(std::forward<Args>(args)...)`.

24. If `T` is an lvalue-reference type, defined as deleted unless the single element of `Args` is an lvalue reference type.

25. The noexcept specification is:
    * If `T` is a reference type, `noexcept(true)`,
    * If `T` is not a reference type, `noexcept(std::is_nothrow_constructible_v<T, Args&&...>)`

```c++
template<typename U>
    explicit(/*see below*/) constexpr option(const option<U>& opt) noexcept(/*see below*/);
template<typename U>
    explicit(/*see below*/) constexpr option(const std::optional<U>& opt) noexcept(/*see below*/);
```

26. Converts the value of `opt` to `T`

27. *Requires*: `std::constructible_from<T,const U&>`

28. *Postconditions*: If `opt` contains a value, then the resulting option contains the value `T{*opt}`. Otherwise, the resulting `option` does not contain a value.

29. The function is `explicit` unless `U` satisfies `std::convertible_to<T>`.

30. The noexcept specification is:
    * If `T` is a reference type, `noexcept(true)`,
    * If `T` is not a reference type, `noexcept(std::is_nothrow_constructible_v<T,const U&>)`

```c++
template<typename U>
    explicit(/*see below*/) constexpr option(option<U>&& opt) noexcept(/*see below*/);
template<typename U>
    explicit(/*see below*/) constexpr option(std::optional<U>&& opt) noexcept(/*see below*/);
```

31. Converts the value of `opt` to `T`

32. *Requires*: `std::constructible_from<T,const U&>`

33. *Postconditions*: If `opt` contains a value, then the resulting option contains the value `T{*opt}`. Otherwise, the resulting `option` does not contain a value.

34. The function is `explicit` unless `U` satisfies `std::convertible_to<T>`.

35. The noexcept specification is:
    * If `T` is a reference type, `noexcept(true)`,
    * If `T` is not a reference type, `noexcept(std::is_nothrow_constructible_v<T,const U&>)`

```c++
constexpr ~option() noexcept;
```

36. Destroys the contained value, if any. If destroying the contained value throws an exception, `std::terminate` is called.

37. The function is defined as default and is trivial if `std::is_trivially_destructible_v<T>` is true.


### Assignment Operators [crabi.option.assign]

```c++
constexpr option& operator=(const option& opt) noexcept(/*see below*/);
```

1. Reassigns the option to the value contained by `opt`.

2. *Requires*: Either `T` is a reference type or `T` satisfies `std::copy_constructible` and `std::is_copy_assignable_v<T>` is true.

3. *Effects*:  If `T` is a reference type, assigns the `U*` member of `this` to the `U*` member of `opt`, otherwise,
    * If `*this` contains a value and `opt` contains a value, then copy-assigns the contained value of `*this` from the contained value of `opt`,
    * If `*this` contains a value and `opt` does not contain a value, then destroys the contained value and resets the option so that it does not contain a value,
    * If `*this` does not contain a value and `opt` does contain a value, then copy-constructs the contained value for `*this` from `*opt`,
    * Otherwise, does nothing.

4. *Returns*: `*this`

5. The noexcept specification is:
    * If `T` is a reference type, `noexcept(true)`,
    * If `T` is not a reference type, `noexcept(std::is_nothrow_copy_constructible_v<T> && std::is_nothrow_copy_assignable_v<T>)`

6. This function is defined as default and is trivial if `T` is a reference type, or `std::is_trivially_copy_assignable_v<T>` is true.

```c++
constexpr option& operator=(option&& opt) noexcept(/*see below*/);
```

7. Reassigns the option to the value contained by `opt`.

8. *Requires*: Either `T` is a reference type or `T` satisfies `std::move_constructible` and `std::is_move_constructible_v<T>` is true.

9. *Effects*:  If `T` is a reference type, assigns the `U*` member of `this` to the `U*` member of `opt`, otherwise,
    * If `*this` contains a value and `opt` contains a value, then move-assigns the contained value of `*this` from the contained value of `opt`,
    * If `*this` contains a value and `opt` does not contain a value, then destroys the contained value and resets the option so that it does not contain a value,
    * If `*this` does not contain a value and `opt` does contain a value, then move-constructs the contained value for `*this` from `*opt`,
    * Otherwise, does nothing.

10. *Returns*: `*this`

11. The noexcept specification is:
    * If `T` is a reference type, `noexcept(true)`,
    * If `T` is not a reference type, `noexcept(std::is_nothrow_copy_constructible_v<T> && std::is_nothrow_copy_assignable_v<T>)`

12. This function is defined as default and is trivial if `T` is a reference type, or `std::is_trivially_copy_assignable_v<T>` is true.

```c++
constexpr option& operator=(std::nullopt_t) noexcept;
```

13. Destroys the contained value, if any, then resets the option so that it does not contain a value. If destroying the value throws an exception, `std::terminate()` is called.

14. *Returns*: `*this`

### Emplace and Reset [crabi.option.emplace]

```c++
template<typename... Args>
    void emplace(Args&&... args) noexcept(/*see below*/);
```

1. Destroys the contained value, if any, then constructs a new contained value from `std::forward<Args>(args)...`.

2. *Requires*:
    * If `T` is a reference type, `sizeof...(Args)==0`, and `T` satisfies `std::constructible_from<Args&&...>`
    * If `T` is not a reference type, `T` satisfies `std::constructible_from<Args&&...>`

3. If `T` is an lvalue reference type, defined as deleted if the one type in `Args...` is not an lvalue reference type.

4. The noexcept specification of this functions is:
    * If `T` is a reference type, `noexcept(true)`,
    * If `T` is not a reference type, `noexcept(std::is_nothrow_constructible_v<T, Args&&...>)`

```c++
void reset() noexcept;
```

5. Destroys the contained value, if any, and resets the option so that it does not contain a value. If destroying the value throws an exception, `std::terminate()` is called.

```c++
option<T> take() noexcept(/*see below*/);
```

6. Moves the contained value, if any, into the return value and destroys it, or returns an empty optional, then resets the optional to not contain a value

7. *Requires*: `T` is a reference type, or `T` satisfies `std::move_constructible`.

8. The noexcept specification of this functions is:
    * If `T` is a reference type, `noexcept(true)`,
    * If `T` is not a reference type, `noexcept(std::is_nothrow_move_constructible_v<T>)`

9. *Postconditions*: After `opt.take()` where `opt` is an lvalue of type `option<T>`, `opt` does not contain a value.

### has_value [crabi.option.has_value]


```c++
constexpr bool has_value() const noexcept;
constexpr explicit operator bool() const noexcept;
```

1. Returns whether or not the `option` contains a value.

2. *Returns*: `true` if the option contains a value, or `false` otherwise.

3. The functions cannot be called in the evaluation of a *core constant expression* if `crabi::option_niche<T>` refers to a partial or explicit specialization of `crabi::option_niche`, and the function `std::bit_cast<crabi::option_niche_t<T>, T>` is not a `constexpr` function.

### Accessors

1. The functions in this section are "Forwarding Functions". If `T` is not a reference type, each function has 4 definitions, for const qualified and unqualified lvalue and rvalue `this`. If `T` is an rvalue reference type, there 2 definitions, one for potentially-const qualified lvalue operands, and one for potentially-const qualified rvalue operands, and if `T` is an lvalue reference type, there is only one definition. In this section, the type `R` designates `T&` for non-const lvalue operands when `T` is not a reference type, `T&` for any lvalue operand when `T` is an rvalue reference type, and `T` when `T` is an lvalue reference type, `const T&` for const lvalue operands when `T` is not a reference type, `T&&` for non-const rvalue operands when `T` is not a reference type and `T` for any rvalue operand when `T` is an rvalue reference type, and `const T&&` for const rvalue operands when `T` is not a reference type.

```c++
R operator*() /*forward-quals*/ noexcept;
```

2. Unsafely accesses the contained value.

3. *Preconditions*: `*this` contains a value

4. *Returns*: 
    * If `T` is a reference type, the contained value, cast to `R`,
    * If `T` is not a reference type, a reference to the contained value.

```c++
R unwrap() /*forward-quals*/;
```

5. Safely accesses the contained value if one is present, otherwise throws `crabi::bad_unwrap`.

6. *Returns*: 
    * If `T` is a reference type, the contained value, cast to `R`,
    * If `T` is not a reference type, a reference to the contained value.

7. *Throws*: If `*this` does not contain a value, throws an instance of `crabi::bad_unwrap`

```c++
option<R> as_ref() /*forward-quals*/ noexcept;
```

8. Returns an option that contains a reference to the contained value if one is present, otherwise returns an empty option.

10. *Returns*:
    * If `T` is a reference type, `std::forward</*qualified-reciever-type*/>(*this)`, where if `T` is an rvalue reference type, `/*qualified-reciever-type*/` is `option<T>` for the rvalue overload, and in all other cases `/*qualified-reciever-type*/` is `const option<T>&`,
    * If `T` is not a reference type, then if `*this` contains a value, returns an option that contains `std::forward<R>(**this)`, otherwise returns an empty `option`.


```c++
template<typename U = T>
T unwrap_or(U&& u) /*forward-quals*/ noexcept(/*see below*/);
```

11. Returns the contained value if one is present, otherwise returns `u`.

12. *Requires*: `T` satisfies `std::constructible<R>` and `std::constructible<U&&>`.

13. *Returns*: 
    * If `*this` contains a value, a value constructed from `static_cast<R>(**this)`,
    * If `*this` does not contain a value, a value constructed from `std::forward<U>(u)`.

14. This function is defined as deleted if `T` is an lvalue reference type and `U` is not an lvalue reference type.

15. The noexcept specification for this function is:
    * If `T` is a reference type, `noexcept(true)`,
    * If `T` is not a reference type, `noexcept(std::is_nothrow_constructible_v<T,R> && std::is_nothrow_constructible_v<T,U&&>)`

```c++
template<typename F>
    T unwrap_or_else(F&& f) /*forward-quals*/ noexcept(/*see below*/);

```

16. Returns the contained value if one is present, otherwise returns the result of calling `f`.

17. *Requires*: `T` satisfies `std::constructible<R>`, `F` satisfies `std::invocable<>` and `std::invoke_result_t<F&&>` satisfies `std::convertible_to<T>`

18. *Returns*: 
    * If `*this` contains a value, a value constructed from `static_cast<R>(**this)`,
    * If `*this` does not contain a value, `std::invoke(std::forward<F>(f))`

19. The noexcept specification for this function is:
    * If `T` is a reference type, `noexcept(std::is_nothrow_invocable_v<F&&>)`,
    * If `T` is not a reference type, `noexcept(std::is_nothrow_constructible_v<T, R> && std::is_nothrow_invocable_v<F&&>)`

### Monadic Operations [crabi.option.emplace]

1. The functions in this section are "Forwarding Functions". If `T` is not an lvalue reference type, the functions have two definitions, one for a const-qualified operand or an lvalue `this`, and one for an non-const rvalue `this`.  The type `R` designates `T&&` for the rvalue qualified overload, and `const T&` for the const qualified overload. If `T` is an lvalue, then the rvalue qualified overload is not defined, and any operand will call the const qualified. `R` will always designate `T&` in this case. The behaviour is undefined if the program takes a pointer-to-member function to any "Forwarding Function"

```c++
template<typename F>
    option<std::invoke_result_t<F&&, R>> map(F&& f) /*forward-quals*/ noexcept(std::is_nothrow_invocable_v<F&&, R>);
```

2. Maps the contained value, if one is present, using `f` 

3. *Requires*: `std::invocable<F&&, R>`

4. *Returns*: 
    * If `*this` contains a value, an `option` that contains `std::invoke(std::forward<F>(f), std::forward<R>(*this))`, 
    * Otherwise, an `option` that does not contain a value.


```c++
template<typename F>
    std::invoke_result_t<F&&, R> and_then(F&& f) /*forward-quals*/ noexcept(std::is_nothrow_invocable_v<F&&, R>);
```

5. Transforms the contained value into another `option` value

6. *Requires*: `std::invocable<F&&, R>`, and `std::invoke_result_t<F&&, R>` is a specialization of `crabi::option`.

7. *Returns*:
    * If `*this` contains a value, `std::invoke(std::forward<F>(f), std::forward<R>(*this))`,
    * Otherwise, an `option` that does not contain a value


```c++
template<typename U>
    option<U> and_then(const option<U>& u) const noexcept(/*see below*/);
```

8. If `*this` contains a value, returns `u`, else returns an empty `option`.

9. *Requires*: `U` is a reference type, or `U` satisfies `std::copy_constructible`

10. *Returns*:
    * If `*this` contains a value, `option{u}`,
    * Otherwise, an `option` that does not contain a value

11. The noexcept specification of the function is:
    * If `U` is a reference type, `noexcept(true)`,
    * If `U` is not a reference type, `noexcept(std::is_nothrow_copy_constructible_v<U>)`

```c++
template<typename U>
    option<U> and_then(option<U>&& u) const noexcept(/*see below*/);
```

12. If `*this` contains a value, returns `u`, else returns an empty `option`.

13. *Requires*: `U` is a reference type, or `U` satisfies `std::move_constructible`

14. *Returns*:
    * If `*this` contains a value, `option{std::move(u)}`,
    * Otherwise, an `option` that does not contain a value

12. The noexcept specification of the function is:
    * If `U` is a reference type, `noexcept(true)`,
    * If `U` is not a reference type, `noexcept(std::is_nothrow_move_constructible_v<U>)`


```c++
template<typename F>
    option<T> or_else(F&& f) /*forward-quals*/ noexcept(std::is_nothrow_invocable_v<F&&>);
```

13. If this contains a value, returns `*this`, else returns the value constructed by invoking `f`.

14. *Requires*: `F` satisfies `std::invocable` and `std::invoke_result_t<F&&>` is convertible to `option<T>`

15. *Returns*:
    * If `*this` contains a value, an option that contains `T{std::forward<R>(**this)}`m
    * If `*this` does not contain a value, `std::invoke(std::forward<F>(f))`.


### Flatten [crabi.option.flatten]

1. The functions in this section are "Forwarding Functions". If `T` is not an lvalue reference type, the functions have two definitions, one for a const-qualified operand or an lvalue `this`, and one for an non-const rvalue `this`.  The type `R` designates `T&&` for the rvalue qualified overload, and `const T&` for the const qualified overload. If `T` is an lvalue, then the rvalue qualified overload is not defined, and any operand will call the const qualified. `R` will always designate `T&` in this case. The behaviour is undefined if the program takes a pointer-to-member function to any "Forwarding Function"

```c++
T flatten() /*forward-quals*/ noexcept(/*see below*/);
```

2. Flattens the contained `option` value.

3. *Requires*: `T` is a specialization of `crabi::option`, and `T` satisfies `std::constructible<R>`

4. *Returns*:
    * If `*this` contains a value, returns a value constructed from `static_cast<R>(**this)`,
    * If `*this` does not contain a value, an `option` that does not contain a value.

### Swap [crabi.option.swap]

```c++
void swap(option<T>& other) noexcept(/*see below*/);
```

1. Swaps `*this` with `other`.

2. *Requires*: `T` is a reference type, or `T` satisfies `std::move_constructible` and `std::swappable`

3. *Effects*:
    * If both `*this` and `other` contains a value, and `T` is not a reference type, then equivalent to `(void)swap(**this, *other)` where `swap` is called in a context where the function `std::swap` is available to unqualified lookup, and any ADL candidates defined in an associated namespace of `T`,
    * If both `*this` and `other` contains a value, then equivalent to `std::swap(/*this-ptr*/, /*other-ptr*/)` where `this-ptr` is an lvalue that denotes the contained `U*` member of `*this`, and `other-ptr` is an lvalue that denotes the contained `U*` member of `other`,
    * If `*this` contains a value and `other` does not, equivalent to `other.emplace(std::forward<T>(**this)); this->reset();`
    * If `other` contains a value and `*this` does not, equivalent to `this->emplace(std::forward<T>(*other)); other.reset();`
    * If neither `*this` nor `other` contains a value, then performs no action.

4. The noexcept specification of this function is:
    * If `T` is a reference type, `noexcept(true)`,
    * If `T` is not a reference type, `noexcept(std::is_nothrow_swappable_v<T> && std::is_nothrow_move_constructible_v<T>)`

```c++
// non-member function
template<typename T> void swap(option<T>& a, option<T>& b) noexcept(/*see below*/);
```

5. Swaps `a` with `b`.

6. *Requires*: `T` is a reference type, or `T` satisfies `std::move_constructible`

7. *Effects*: Equivalent to `a.swap(b)`.

8. The function has the same `noexcept` specification as `option<T>::swap`.

9. This is a "Lookup Inhibited Function". The behaviour is undefined if this function is referenced by the program via qualified lookup, or if it is called via unqualified lookup, except for Argument Dependent Lookup where `crabi::option<T>` is in the set of associated classes

### Comparison Operators [crabi.option.cmp]

```c++
template<typename U> constexpr bool operator==(const option<T>& opt) const noexcept(noexcept(**this == *opt));
```

1. Compares `*this` and `opt` for equality.

2. *Requires*: `T` satisfies `std::equality_comparable_with<U>`.

3. *Returns*:
    * If `*this` and `opt` both contain a value, `**this == *opt`,
    * If `*this` and `opt` both do not contain a value, `true`,
    * Otherwise, `false.

4. *Remarks*: If `T` models `rusty::structural_equality`, then `crabi::option<T>` also does.

5. This is a "Lookup Inhibited Operator". The behaviour is undefined if this function is called, except via an operator expression using Argument Dependent Lookup where `crabi::option<T>` is in the set of associated classes.


```c++
template<typename U> constexpr std::compare_three_way_result_t<T,U> operator<=>(const option<T>& opt) const noexcept(noexcept(**this <=> *opt));
```

6. Compares `*this` and `opt`.

7. *Requires*: `T` satisfies `std::three_way_comparable_with<U>`

8. *Returns*:
    * If `*this` and `opt` both contain a value, `**this <=> *opt`,
    * If `*this` and `opt` both do not contain a value, `std::strong_ordering::equivalent` converted to the return type,
    * If `*this` contains a value and `opt` does not, `std::strong_ordering::greater` converted to the return type,
    * If `*this` does not contain a value and `opt` does`, std::strong_ordering::less` converted to the return type.

9. This is a "Lookup Inhibited Operator". The behaviour is undefined if this function is called, except via an operator expression using Argument Dependent Lookup where `crabi::option<T>` is in the set of associated classes.

```c++
// non-member
template<typename T, typename U>
    std::strong_ordering strong_order(const option<T>& a, const option<U>& b) const noexcept(noexcept(std::strong_order(*a, *b)));
```

10. Strongly orders values of type `option<T>`.

11. *Requires*: A call to the function `std::strong_order` with parameters of types `T` and `U` is well-formed.

12. *Returns*:
    * If `a` and `b` both contain a value, `std::strong_order(*a,*b)`,
    * If `a` and `b` both do not contain a value, `std::strong_ordering::equivalent` converted to the return type,
    * If `a` contains a value and `b` does not, `std::strong_ordering::greater` converted to the return type,
    * If `a` does not contain a value and `b` does`, std::strong_ordering::less` converted to the return type.

13. This is a "Lookup Inhibited Function". The behaviour is undefined if this function is referenced by the program via qualified lookup, or if it is called via unqualified lookup, except for Argument Dependent Lookup where `crabi::option<T>` is in the set of associated classes.

```c++
// non-member
template<typename T, typename U>
    std::weak_ordering weak_order(const option<T>& a, const option<U>& b) const noexcept(noexcept(std::weak_order(*a, *b)));
```

14. Strongly orders values of type `option<T>`.

15. *Requires*: A call to the function `std::weak_order` with parameters of types `T` and `U` is well-formed.

16. *Returns*:
    * If `a` and `b` both contain a value, `std::weak_order(*a,*b)`,
    * If `a` and `b` both do not contain a value, `std::strong_ordering::equivalent` converted to the return type,
    * If `a` contains a value and `b` does not, `std::strong_ordering::greater` converted to the return type,
    * If `a` does not contain a value and `b` does`, std::strong_ordering::less` converted to the return type.

17. This is a "Lookup Inhibited Function". The behaviour is undefined if this function is referenced by the program via qualified lookup, or if it is called via unqualified lookup, except for Argument Dependent Lookup where `crabi::option<T>` is in the set of associated classes
