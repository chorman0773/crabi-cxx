# General API Usage [crabi.gen]

## Reserved Identifiers

1. Any identifier that begins with `_` is reserved. The behaviour of `#define`ing or `#undef`ing such an identifier, or of refering to an identifier by that name anywhere in the `rusty` or `crabi` namespace, is undefined.

2. The behaviour of defining any name within the `crabi` or `rusty` namespace, except a partial or full specialization of a class template provided in the library for a user-defined type, is undefined.

## Non-member and Operator functions not guaranteed to be visible

1. A non-member function defined by this API with any of the following names are not guaranteed to be visible to qualified or unqualified lookup, except to argument dependent lookup where one of the parameters types which is defined by the library is in the ADL lookup set. These functions are called "Lookup Inhibited Functions":
    * `swap`
    * `begin`
    * `end`
    * `cbegin`
    * `cend`
    * `rbegin`
    * `rend`
    * `crbegin`
    * `crend`
    * `get`, when defined as a function template that accepts a template parameter of type `std::size_t` as the first template parameter
    * `strong_order`
    * `weak_order`
    * `partial_order`
    * `data`
    * `cdata`
    * `size`
    * `ssize`
    * `empty`

2. An operator function defined by this API, whether as a member or non-member function, is not guaranteed to be visible to qualified or unqualified lookup, except to argument dependent lookup applied by an implicit call to the overloaded operators, where one of the parameters types which is defined by the library is in the ADL lookup set, unless the function is one of the following, in which case it is defined as a member function and visible to member function lookup. Such operator functions are called "Lookup Inhibited Operators":
    * `operator->`
    * `operator[]`
    * `operator=`
    * `operator++` (either form)
    * `operator--` (either form)
    * `operator()`

3. A conversion operator function defined by this API is not guaranteed to be visible to qualified or unqualified lookup, except to argument dependent lookup applied by an implicit or explicit conversion from the type which is defined by the library and defines the conversion operator function.

3. A function that is not guaranteed to be visible to qualified or unqualified lookup by this API additionally is not guaranteed to support explicit template parameters being provided to the function, except for the first parameter of type `std::size_t` on a non-member function `get`.

4. A program that refers to any function named in this section by qualified lookup, refers to an operator function named in this section as a member function or by unqualified lookup, or supplies an explicit template parameter list to any function named in this section other than the first parameter of type `std::size_t` on a non-member function `get`, is ill-formed. No diagnostic is required.