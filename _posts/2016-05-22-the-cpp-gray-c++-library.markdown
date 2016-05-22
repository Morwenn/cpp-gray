---
layout: post
title: The cpp-gray C++ Library
date: 2016-05-22 17:32:43 +0200
categories: [gray code, c++]
---
The previous article introduced the notation needed to understand Gray code algorithms from a pseudo-formal points of view.
This new article presents the other side of the coin: the **cpp-gray** C++ library that we will be using to implement the
different algorithms, and more specifically the design of the `gray_code` class template.

### Basic operations

[**cpp-gray**][cpp-gray] is the second part of the project, this blog being the first. It is a C++14 library designed to
easily manipulate unsigned integer types with bits arranged in Gray code order rather than in « natural binary » order. The
class template `gray_code` wraps a built-in integer of a given type and ensures that its bits are always organized in Gray
code order. `gray_code` can be constructed from an unsigned integer of the wrapped type or from a `bool` instance, and has
an `explicit` conversion operator to transform the Gray code back to an unsigned integer with the same value. The class
template can be summarized as follows:

```cpp
template<typename Unsigned>
struct gray_code
{
    static_assert(std::is_unsigned<Unsigned>::value,
                  "gray_code only supports built-in unsigned integers");

    // Underlying unsigned integer type
    using value_type = Unsigned;

    // Unsigned integer used to store the Gray code
    value_type value;

    // Construction operations
    constexpr gray_code() noexcept;
    constexpr explicit gray_code(value_type value) noexcept;
    constexpr explicit gray_code(bool b) noexcept;

    // Assignment operations
    auto operator=(value_type other) & noexcept
        -> gray_code&;
    auto operator=(bool other) & noexcept
        -> gray_code&;

    // Conversion operations
    explicit operator value_type() const noexcept;
    constexpr explicit operator bool() const noexcept;
};

template<typename Unsigned>
auto swap(gray_code<Unsigned>& lhs, gray_code<Unsigned>& rhs) noexcept
    -> void;
```

The goal of `gray_code` is to be just like another unsigned integer type, minus the implicit conversion, except that users
manipulate a Gray code representation instead of a « natural binary » representation. When translating algorithms to code
examples, we will typically use `unsigned` for \\( \mathbb{B} \\) and `gray_code<unsigned>` for \\( \mathbb{G} \\).

All of the available operations are designed so that `unsigned` and `gray_code<unsigned>` are equivalent to each other:
when trying to use both at once in a binary operation, users will likely need to convert one to the other first. For
example, if `b` is an `unsigned` and `g` is a `gray_code<unsigned>`, the expression `b ^ g` won't compile. Users will need
to decide the type of the result they want, *e.g.* they will need to write `b ^ g.value` if they want to get an `unsigned`
result. However, augmented operations such as `b ^= g` work out of the box since the resulting type is unambiguous. The lack
of implicit conversions between `unsigned` and `gray_code<unsigned>` helps to enforce this rule. The types `unsigned` and
`gray_code<unsigned>` can be thought of as either proper unsigned integers—in which case one needs to use the converting
constructor and conversion operator to get the different representations of the same integral value—or as collections of
bits. One can construct an `unsigned` with the bits of a `gray_code<unsigned>` by accessing the public member variable
`gray_code::value`; however there is currently no way to construct a `gray_code<unsigned>` directly from the bits of an
`unsigned`. `gray_code::operator=` also transforms the passed `unsigned` into a Gray code instead of reinterpreting its
bits.

Any `gray_code` specialization also handles `bool` values thanks to an interesting observation: `true` and `false` in C++
respectively have the binary representations \\( 0b1 \\) and \\( 0b0 \\), which also happen to be the binary representations
of \\( 1_\mathbb{B} \\) and \\( 0_\mathbb{B} \\) as well as those of \\( 1_\mathbb{G} \\) and \\( 0_\mathbb{G} \\). This
means that we can safely convert `true` to \\( 1_\mathbb{G} \\) and `false` to \\( 0_\mathbb{G} \\) without even having to
apply any conversion algorithm. The `explicit` conversion to `bool` mainly exists to convert a `gray_code` into a logical
value, where \\( 0_\mathbb{G} \\) converts to `false` and any other value converts to `true`.

### Bitwise operations

When using bitwise operations, `gray_code` is considered as a mere sequence of bits, and bitwise operations can be performed
between it and other suitable sequences of bits. `gray_code` handles binary bitwise operations between three kinds of types:

* Between two `gray_code` instances with the same `value_type`. In this case the bitwise operations are performed on their
`value` member variable. The operations `&`, `|`, `^`, `&=`, `|=` and `^=` are supported.
* Between a `gray_code<T>` and a `T`. In this case the bitwise operations are performed between the `T` instance and the
`value` member variable of the `gray_code`. The operations `&=`, `|=` and `^=` are supported and the return type is that of
the left operand. The operations `&`, `|` and `^` are not supported because there is no « obvious » return type.
* Between a `gray_code` and a `bool`. In this case the `bool` instance is considered as either \\( 0_\mathbb{G} \\) or
\\( 1_\mathbb{G} \\) depending on its value, and the bitwise operation is performed between that and the `value` member
variable of `gray_code`. The operations `&`, `|`, `^`, `&=`, `|=` and `^=` are supported and always return a `gray_code`.
The operations `&=`, `|=` and `^=` when the left operand is a `bool`.

The shift operators `>>`, `<<`, `>>=` and `<<=` are also overloaded so that they can take a `gray_code` as left operand and
an `std::size_t` as right operand, and return a `gray_code` whose `value` has been shifted by the given amount of bits.
There wasn't an obvious type to select for the right operand so I decided to follow the design choice of the standard 
library, which uses `std::size_t` for the right operand of [`std::bitset`][bitset]'s shift operators.

The bitwise operator `~` can also be applied to `gray_code`, in which case the result is  another `gray_code` whose `value`
member variable corresponds to `~` applied to the original one.

### Comparison operations

When using comparison operations, a `gray_code` instance is considered to be an integral value instead of a mere chain of
bits. The operators `==` and `!=` are overloaded to handle two kinds of scenarios:

* When comparing two `gray_code` instances with the same underlying type, the result is that of the comparison operator
applied to their `value` member variable.
* When comparing a `gray_code<T>` instance `g` and a `T` instance `b`, the result of the comparison is equivalent to the
comparison performed between `static_cast<T>(g)` and `b`. The side of the operator on which `b` appears does not matter.

The operators `==` and `!=` are not overloaded to handle the comparison between a `gray_code` and a `bool`, mostly because
they do not make much sense. The remaining comparison operators `<`, `>`, `<=` and `>=` are not overloaded because no fast
algorithm to compute them is known; see the next section for more details.

### Arithmetic operations

When using arithmetic operations, a `gray_code` instance is considered to be an integral value instead of a mere chain of
bits. Only a handful of arithmetic operations are handled because of a specific design choice in **cpp-gray**: such an
operation on `gray_code` is only implemented when it can be made faster than converting the `gray_code` instance(s) to a
regular built-in type instance(s), performing the equivalent operation on a built-in type and converting the result back to
a `gray_code` instance. This requirement is pretty tough to meet because the conversions between `T` and `gray_code<T>` are
really fast as we will see in another article, and beating them with raw operations on `gray_code` is hard. Because of that
design choice, only the following few arithmetic operations are actually implemented for `gray_code`:

* The increment and decrement operators `++` and `--`. Both of them allow to use either the prefix of postfix overload.
* The free functions `is_even` and `is_odd` that allow to query the [parity][parity] of a `gray_code`.

Subsequent articles will describe the algorithms used to compute these arithmetic operations as well as a few more that
either too slow to be included in the library, or too specific to have dedicated functions (for example multiplying a
`gray_code` by \\( 3 \\) in \\( O(1) \\) on some specific processor architectures).

### Example

What's a library introduction without a small example? The following snippet showcases some things that are dable directly
in the Gray code domain with `gray_code`:

```cpp
using cppgray::gray_code;

auto gr = gray_code<unsigned>(24u);
unsigned u = gr;        // u == 24u (0b11000)
unsigned g = gr.value;  // g == 20u (0b10100)

++u; ++gr;
assert(u == gr);
```

To be honest, there isn't much to showcase yet and the library really isn't evolving at a fast pace considering that any
other fast Gray code algorithm has yet to be discovered. If any new algorithm makes its way into the library, it will surely
be documented here, and will probably have a dedicated article to describe it as well. As always, if you feel that something
is missing, don't hesitate to report it in the [issues][issues] section of the associated repository with the « blog »
label.


  [bitset]: http://en.cppreference.com/w/cpp/utility/bitset
  [cpp-gray]: https://github.com/Morwenn/cpp-gray
  [issues]: https://github.com/Morwenn/cpp-gray/issues
  [parity]: https://en.wikipedia.org/wiki/Parity_%28mathematics%29
