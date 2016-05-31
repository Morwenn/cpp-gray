/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Morwenn
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef CPPGRAY_GRAY_H_
#define CPPGRAY_GRAY_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <bitset>
#include <cstddef>
#include <limits>
#include <type_traits>

namespace cppgray
{
    /**
     * @brief Gray code unsigned integer.
     *
     * This class represents a Gray code unsigned
     * integer. It can be set by integers.
     *
     * auto gr = gray_code<std::uint16_t>{ 24 };
     * std::uint16_t u = gr;        // u == 24 (0b11000)
     * std::uint16_t g = gr.value;  // g == 20 (0b10100)
     */
    template<typename Unsigned>
    struct gray_code
    {
        static_assert(std::is_unsigned<Unsigned>::value,
                      "gray_code only supports built-in unsigned integers");

        // Underlying unsigned integer type
        using value_type = Unsigned;

        // Unsigned integer in Gray code
        value_type value;

        ////////////////////////////////////////////////////////////
        // Constructors operations

        // Default constructor
        constexpr gray_code() noexcept;

        /**
         * @brief Construction from an unsigned integer.
         *
         * The integer is converted to gray code. The value
         * is preserved. The representation is not.
         *
         * @param value Unsigned integer to convert
         */
        constexpr explicit gray_code(value_type value) noexcept;

        /**
         * @brief Construction from a `bitset`.
         *
         * The exact bit representation of the set is used
         * as the grey code.
         *
         * @param value `bitset` to convert
         */
        template<
            std::size_t N,
            typename = std::enable_if_t<(N >= std::numeric_limits<value_type>::digits)>
        >
        constexpr explicit gray_code(const std::bitset<N>& value) noexcept;

        /**
         * @brief Construction from a boolean.
         *
         * Regular integers and Gray code have the same
         * representation for 0 and 1, which are equivalent
         * to the booleans false and true. Therefore, the
         * construction from a boolean is a no-op.
         */
        constexpr explicit gray_code(bool b) noexcept;

        ////////////////////////////////////////////////////////////
        // Assignment operations

        constexpr auto operator=(value_type other) & noexcept
            -> gray_code&;

        constexpr auto operator=(bool other) & noexcept
            -> gray_code&;

        ////////////////////////////////////////////////////////////
        // Conversion operations

        /**
         * @brief Conversion to the underlying type.
         */
        explicit constexpr operator value_type() const noexcept;

        /**
         * @brief Conversion to the exact underlying bit representation.
         */
        explicit constexpr operator std::bitset<std::numeric_limits<value_type>::digits>() const noexcept;

        constexpr explicit operator bool() const noexcept;

        ////////////////////////////////////////////////////////////
        // Increment/decrement operations

        constexpr auto operator++() noexcept
            -> gray_code&;
        constexpr auto operator++(int) noexcept
            -> gray_code;

        constexpr auto operator--() noexcept
            -> gray_code&;
        constexpr auto operator--(int) noexcept
            -> gray_code;

        ////////////////////////////////////////////////////////////
        // Bitwise assignment operations

        constexpr auto operator&=(gray_code other) noexcept
            -> gray_code&;
        constexpr auto operator&=(value_type other) noexcept
            -> gray_code&;
        constexpr auto operator&=(bool other) noexcept
            -> gray_code&;

        constexpr auto operator|=(gray_code other) noexcept
            -> gray_code&;
        constexpr auto operator|=(value_type other) noexcept
            -> gray_code&;
        constexpr auto operator|=(bool other) noexcept
            -> gray_code&;

        constexpr auto operator^=(gray_code other) noexcept
            -> gray_code&;
        constexpr auto operator^=(value_type other) noexcept
            -> gray_code&;
        constexpr auto operator^=(bool other) noexcept
            -> gray_code&;

        constexpr auto operator>>=(std::size_t pos) noexcept
            -> gray_code&;
        constexpr auto operator<<=(std::size_t pos) noexcept
            -> gray_code&;
    };

    /**
     * @brief Creates a gray_code instance.
     *
     * The advantage of this function is its ability to
     * deduce the type of its parameter.
     *
     * @param value Unsigned integer to convert to gray code.
     */
    template<typename Unsigned>
    constexpr auto gray(Unsigned value) noexcept
        -> gray_code<Unsigned>;

    ////////////////////////////////////////////////////////////
    // Comparison operations

    template<typename Unsigned>
    constexpr auto operator==(gray_code<Unsigned> lhs, gray_code<Unsigned> rhs) noexcept
        -> bool;
    template<typename Unsigned>
    constexpr auto operator!=(gray_code<Unsigned> lhs, gray_code<Unsigned> rhs) noexcept
        -> bool;

    template<typename Unsigned>
    constexpr auto operator==(gray_code<Unsigned> lhs, Unsigned rhs) noexcept
        -> bool;
    template<typename Unsigned>
    constexpr auto operator!=(gray_code<Unsigned> lhs, Unsigned rhs) noexcept
        -> bool;

    template<typename Unsigned>
    constexpr auto operator==(Unsigned lhs, gray_code<Unsigned> rhs) noexcept
        -> bool;
    template<typename Unsigned>
    constexpr auto operator!=(Unsigned lhs, gray_code<Unsigned> rhs) noexcept
        -> bool;

    ////////////////////////////////////////////////////////////
    // Bitwise operations

    template<typename Unsigned>
    constexpr auto operator&(gray_code<Unsigned> lhs, gray_code<Unsigned> rhs) noexcept
        -> gray_code<Unsigned>;

    template<typename Unsigned>
    constexpr auto operator|(gray_code<Unsigned> lhs, gray_code<Unsigned> rhs) noexcept
        -> gray_code<Unsigned>;

    template<typename Unsigned>
    constexpr auto operator^(gray_code<Unsigned> lhs, gray_code<Unsigned> rhs) noexcept
        -> gray_code<Unsigned>;

    template<typename Unsigned>
    constexpr auto operator~(gray_code<Unsigned> val) noexcept
        -> gray_code<Unsigned>;

    template<typename Unsigned>
    constexpr auto operator>>(gray_code<Unsigned> val, std::size_t pos) noexcept
        -> gray_code<Unsigned>;

    template<typename Unsigned>
    constexpr auto operator<<(gray_code<Unsigned> val, std::size_t pos) noexcept
        -> gray_code<Unsigned>;

    ////////////////////////////////////////////////////////////
    // Bitwise operations with bool

    template<typename Unsigned>
    constexpr auto operator&(gray_code<Unsigned> lhs, bool rhs) noexcept
        -> gray_code<Unsigned>;

    template<typename Unsigned>
    constexpr auto operator&(bool lhs, gray_code<Unsigned> rhs) noexcept
        -> gray_code<Unsigned>;

    template<typename Unsigned>
    constexpr auto operator|(gray_code<Unsigned> lhs, bool rhs) noexcept
        -> gray_code<Unsigned>;

    template<typename Unsigned>
    constexpr auto operator|(bool lhs, gray_code<Unsigned> rhs) noexcept
        -> gray_code<Unsigned>;

    template<typename Unsigned>
    constexpr auto operator^(gray_code<Unsigned> lhs, bool rhs) noexcept
        -> gray_code<Unsigned>;

    template<typename Unsigned>
    constexpr auto operator^(bool lhs, gray_code<Unsigned> rhs) noexcept
        -> gray_code<Unsigned>;

    ////////////////////////////////////////////////////////////
    // Bitwise assignment operations

    template<typename Unsigned>
    constexpr auto operator&=(Unsigned& lhs, gray_code<Unsigned> rhs) noexcept
        -> Unsigned&;

    template<typename Unsigned>
    constexpr auto operator|=(Unsigned& lhs, gray_code<Unsigned> rhs) noexcept
        -> Unsigned&;

    template<typename Unsigned>
    constexpr auto operator^=(Unsigned& lhs, gray_code<Unsigned> rhs) noexcept
        -> Unsigned&;

    ////////////////////////////////////////////////////////////
    // Utility functions

    template<typename Unsigned>
    constexpr auto swap(gray_code<Unsigned>& lhs, gray_code<Unsigned>& rhs) noexcept
        -> void;

    ////////////////////////////////////////////////////////////
    // Mathematical functions

    template<typename Unsigned>
    constexpr auto is_even(gray_code<Unsigned> code) noexcept
        -> bool;

    template<typename Unsigned>
    constexpr auto is_odd(gray_code<Unsigned> code) noexcept
        -> bool;

    #include "gray.inl"
}

#endif // CPPGRAY_GRAY_H_
