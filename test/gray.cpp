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
#include <cstdint>
#include <limits>
#include <type_traits>
#include <cpp-gray/gray.h>

// Trick function used in tests with stateful functions
constexpr auto check(bool condition, int pos)
    -> std::uint64_t
{
    std::uint64_t res = condition;
    return res << pos;
}

constexpr auto assignment()
    -> std::uint64_t
{
    using namespace cppgray;

    std::uint64_t res = 0u;
    std::size_t pos = 0u;

    ////////////////////////////////////////////////////////////
    // Assignment operator

    // From another Gray code
    {
        gray_code<unsigned> gr1, gr2;
        gr1 = gray(58u);
        gr2 = gray(22u);

        res |= check(gr1 == gray(58u), pos++);
        res |= check(gr2 == gray(22u), pos++);
        res |= check(gr1 == 58u, pos++);
        res |= check(22u == gr2, pos++);
    }

    // From an unsigned integer
    {
        gray_code<unsigned> gr1, gr2;
        gr1 = 73u;
        gr2 = 194u;

        res |= check(gr1 == gray(73u), pos++);
        res |= check(gr2 == gray(194u), pos++);
        res |= check(gr1 == 73u, pos++);
        res |= check(194u == gr2, pos++);
    }

    ////////////////////////////////////////////////////////////
    // Swap function

    {
        auto gr1 = gray(52u);
        auto gr2 = gray(48u);

        swap(gr1, gr2);
        res |= check(gr1 == 48u, pos++);
        res |= check(gr2 == 52u, pos++);
    }

    ////////////////////////////////////////////////////////////
    // Bitwise assignment operators

    {
        unsigned a = 0b0110u;
        unsigned b = 0b1001u;
        unsigned c = 0b1101u;

        a |= gray(0u);
        res |= check(a == 0b0110u, pos++);
        a &= gray(4u);
        res |= check(a == 0b0110u, pos++);

        b |= gray(9u);
        res |= check(b == 0b1101u, pos++);

        c ^= gray(5u);
        res |= check(c == 0b1010u, pos++);
    }

    return res;
}

constexpr auto increment()
    -> std::uint64_t
{
    using namespace cppgray;

    std::uint64_t res = 0b011011u;
    std::size_t pos = 0u;
    ////////////////////////////////////////////////////////////
    // Incrementation

    // Simple case
    {
        unsigned i = 0u;
        for (auto val = gray(0u) ; val != gray(10u) ; ++val, ++i)
        {
            res &= ~!check(val == i, pos);
        }
        ++pos;

        i = 0u;
        for (auto val = gray(0u) ; val != gray(10u) ; val++, i++)
        {
            res &= ~!check(val == i, pos);
        }
        ++pos;
    }

    // Circular behaviour on overflow
    {
        auto max_uint = std::numeric_limits<unsigned>::max();
        auto gr = gray(max_uint);

        ++gr;
        res |= check(gr.value == 0u, pos++);
    }

    ////////////////////////////////////////////////////////////
    // Decrementation

    // Simple case
    {
        unsigned i = 35u;
        for (auto val = gray(35u) ; val != gray(23u) ; --val, --i)
        {
            res &= ~!(val == i, pos);
        }
        ++pos;

        i = 35u;
        for (auto val = gray(35u) ; val != gray(23u) ; val--, i--)
        {
            res &= ~!check(val == i, pos);
        }
        ++pos;
    }

    // Circular behaviour on overflow
    {
        auto max_uint = std::numeric_limits<unsigned>::max();
        auto gr = gray(0u);

        --gr;
        res |= check(gr == gray(max_uint), pos++);
    }

    return res;
}

int main()
{
    using namespace cppgray;

    ////////////////////////////////////////////////////////////
    // Conversions between two's complement integers and
    // Gray codes

    // Convert to and from zero
    {
        constexpr auto zero = gray(0u);
        static_assert(zero == 0u, "");
        static_assert(zero.value == 0b0000u, "");

        constexpr unsigned zero_u(zero);
        static_assert(zero_u == 0u, "");
    }

    // Convert to and from unsigned integer
    {
        constexpr auto gr = gray(5u);
        static_assert(gr == 5u, "");
        static_assert(gr.value == 0b0111u, "");

        constexpr unsigned five(gr);
        static_assert(five == 5u, "");
    }

    // Convert to and from std::numeric_limits::max
    {
        constexpr auto max_uint = std::numeric_limits<unsigned>::max();
        constexpr auto max_gr = gray(max_uint);
        static_assert(max_gr == max_uint, "");
    }

    ////////////////////////////////////////////////////////////
    // Construction

    {
        constexpr gray_code<unsigned> gr1{};
        static_assert(gr1.value == 0u, "");

        constexpr gray_code<unsigned long long> gr2{};
        static_assert(gr2.value == 0ull, "");
    }

    ////////////////////////////////////////////////////////////
    // Comparison operators

    {
        constexpr auto gr = gray(52u);

        static_assert(gr == gray(52u), "");
        static_assert(gr == 52u, "");
        static_assert(52u == gr, "");
        static_assert(gr != 56u, "");
        static_assert(54u != gr, "");
        static_assert(gr != gray(89u), "");
    }

    ////////////////////////////////////////////////////////////
    // Bitwise operations

    {
        constexpr auto gr1 = gray(42u);
        constexpr auto gr2 = gray(28u);

        static_assert((gr1 & gr2).value == (gr1.value & gr2.value), "");
        static_assert((gr1 | gr2).value == (gr1.value | gr2.value), "");
        static_assert((gr1 ^ gr2).value == (gr1.value ^ gr2.value), "");
        static_assert((~gr1).value == ~(gr1.value), "");
        static_assert((gr1 << 3u).value == (gr1.value << 3u), "");
        static_assert((gr1 >> 2u).value == (gr1.value >> 2u), "");
    }

    ////////////////////////////////////////////////////////////
    // gray() function type inference

    {
        constexpr auto llgr = gray(8ull);
        using value_type = decltype(llgr)::value_type;

        static_assert(std::is_same<value_type, unsigned long long>::value, "");
    }

    ////////////////////////////////////////////////////////////
    // Mathematical operations

    // is_even
    {
        static_assert(not is_even(gray(5u)), "");
        static_assert(is_even(gray(4u)), "");
        static_assert(is_even(gray(0u)), "");
        static_assert(not is_even(gray(12357u)), "");
        static_assert(is_even(gray(15328u)), "");
    }

    // is_odd
    {
        static_assert(not is_odd(gray(8u)), "");
        static_assert(not is_odd(gray(0u)), "");
        static_assert(is_odd(gray(5u)), "");
    }

    ////////////////////////////////////////////////////////////
    // Test assignment and swap functions

    {
        constexpr auto res = assignment();

        // Assignment from another Gray code
        static_assert(res >> 0 & 1, "");
        static_assert(res >> 1 & 1, "");
        static_assert(res >> 2 & 1, "");
        static_assert(res >> 3 & 1, "");

        // Assignment from an unsigned integer
        static_assert(res >> 4 & 1, "");
        static_assert(res >> 5 & 1, "");
        static_assert(res >> 6 & 1, "");
        static_assert(res >> 7 & 1, "");

        // Swap function
        static_assert(res >> 8 & 1, "");
        static_assert(res >> 9 & 1, "");

        // Bitwise assignment operators
        static_assert(res >> 10 & 1, "");
        static_assert(res >> 11 & 1, "");
        static_assert(res >> 12 & 1, "");
        static_assert(res >> 13 & 1, "");
    }

    ////////////////////////////////////////////////////////////
    // Test increment functions

    {
        constexpr auto res = increment();

        // Incrementation
        static_assert(res >> 0 & 1, "");
        static_assert(res >> 1 & 1, "");
        static_assert(res >> 2 & 1, "");

        // Decrementation
        static_assert(res >> 3 & 1, "");
        static_assert(res >> 4 & 1, "");
        static_assert(res >> 5 & 1, "");
    }
}
