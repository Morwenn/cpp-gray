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

////////////////////////////////////////////////////////////
// Construction operations

template<typename Unsigned>
constexpr gray_code<Unsigned>::gray_code() noexcept:
    value(0)
{}

template<typename Unsigned>
constexpr gray_code<Unsigned>::gray_code(value_type value) noexcept:
    value( (value >> 1) ^ value )
{}

template<typename Unsigned>
constexpr gray_code<Unsigned>::gray_code(bool value) noexcept:
    value(value)
{}

////////////////////////////////////////////////////////////
// Assignment operations

template<typename Unsigned>
constexpr auto gray_code<Unsigned>::operator=(value_type other) & noexcept
    -> gray_code&
{
    value = (other >> 1) ^ other;
    return *this;
}

template<typename Unsigned>
constexpr auto gray_code<Unsigned>::operator=(bool other) & noexcept
    -> gray_code&
{
    value = other;
    return *this;
}

////////////////////////////////////////////////////////////
// Conversion operations

template<typename Unsigned>
constexpr gray_code<Unsigned>::operator value_type() const noexcept
{
    value_type res = value;
    for (value_type mask = std::numeric_limits<value_type>::digits / 2
         ; mask ; mask >>= 1)
    {
        res ^= res >> mask;
    }
    return res;
}

template<typename Unsigned>
constexpr gray_code<Unsigned>::operator bool() const noexcept
{
    return static_cast<bool>(value);
}

////////////////////////////////////////////////////////////
// Increment/decrement operations

template<typename Unsigned>
constexpr auto gray_code<Unsigned>::operator++() noexcept
    -> gray_code&
{
    constexpr value_type msb = 1 << (std::numeric_limits<value_type>::digits - 1);

    if (is_odd(*this))
    {
        if (value == msb)
        {
            value = 0;
        }
        else
        {
            auto y = value & -value;
            value ^= (y << 1);
        }
    }
    else
    {
        value ^= 1;
    }
    return *this;
}

template<typename Unsigned>
constexpr auto gray_code<Unsigned>::operator++(int) noexcept
    -> gray_code
{
    auto res = *this;
    operator++();
    return res;
}

template<typename Unsigned>
constexpr auto gray_code<Unsigned>::operator--() noexcept
    -> gray_code&
{
    constexpr value_type msb = 1 << (std::numeric_limits<value_type>::digits - 1);

    if (is_odd(*this))
    {
        value ^= 1;
    }
    else
    {
        if (value == 0)
        {
            value = msb;
        }
        else
        {
            auto y = value & -value;
            value ^= (y << 1);
        }
    }
    return *this;
}

template<typename Unsigned>
constexpr auto gray_code<Unsigned>::operator--(int) noexcept
    -> gray_code
{
    auto res = *this;
    operator--();
    return res;
}

////////////////////////////////////////////////////////////
// Bitwise assignment operations

template<typename Unsigned>
constexpr auto gray_code<Unsigned>::operator&=(gray_code other) noexcept
    -> gray_code&
{
    value &= other.value;
    return *this;
}

template<typename Unsigned>
constexpr auto gray_code<Unsigned>::operator&=(value_type other) noexcept
    -> gray_code&
{
    value &= other;
    return *this;
}

template<typename Unsigned>
constexpr auto gray_code<Unsigned>::operator&=(bool other) noexcept
    -> gray_code&
{
    value &= other;
    return *this;
}

template<typename Unsigned>
constexpr auto gray_code<Unsigned>::operator|=(gray_code other) noexcept
    -> gray_code&
{
    value |= other.value;
    return *this;
}

template<typename Unsigned>
constexpr auto gray_code<Unsigned>::operator|=(value_type other) noexcept
    -> gray_code&
{
    value |= other;
    return *this;
}

template<typename Unsigned>
constexpr auto gray_code<Unsigned>::operator|=(bool other) noexcept
    -> gray_code&
{
    value |= other;
    return *this;
}

template<typename Unsigned>
constexpr auto gray_code<Unsigned>::operator^=(gray_code other) noexcept
    -> gray_code&
{
    value ^= other.value;
    return *this;
}

template<typename Unsigned>
constexpr auto gray_code<Unsigned>::operator^=(value_type other) noexcept
    -> gray_code&
{
    value ^= other;
    return *this;
}

template<typename Unsigned>
constexpr auto gray_code<Unsigned>::operator^=(bool other) noexcept
    -> gray_code&
{
    value ^= other;
    return *this;
}

template<typename Unsigned>
constexpr auto gray_code<Unsigned>::operator>>=(std::size_t pos) noexcept
    -> gray_code&
{
    value >>= pos;
    return *this;
}

template<typename Unsigned>
constexpr auto gray_code<Unsigned>::operator<<=(std::size_t pos) noexcept
    -> gray_code&
{
    value <<= pos;
    return *this;
}

////////////////////////////////////////////////////////////
// Construction function

template<typename Unsigned>
constexpr auto gray(Unsigned value) noexcept
    -> gray_code<Unsigned>
{
    return gray_code<Unsigned>(value);
}

////////////////////////////////////////////////////////////
// Comparison operations

template<typename Unsigned>
constexpr auto operator==(gray_code<Unsigned> lhs, gray_code<Unsigned> rhs) noexcept
    -> bool
{
    return lhs.value == rhs.value;
}

template<typename Unsigned>
constexpr auto operator!=(gray_code<Unsigned> lhs, gray_code<Unsigned> rhs) noexcept
    -> bool
{
    return lhs.value != rhs.value;
}

template<typename Unsigned>
constexpr auto operator==(gray_code<Unsigned> lhs, Unsigned rhs) noexcept
    -> bool
{
    return (rhs ^ (rhs >> 1)) == lhs.value;
}

template<typename Unsigned>
constexpr auto operator!=(gray_code<Unsigned> lhs, Unsigned rhs) noexcept
    -> bool
{
    return (rhs ^ (rhs >> 1)) != lhs.value;
}

template<typename Unsigned>
constexpr auto operator==(Unsigned lhs, gray_code<Unsigned> rhs) noexcept
    -> bool
{
    return (lhs ^ (lhs >> 1)) == rhs.value;
}

template<typename Unsigned>
constexpr auto operator!=(Unsigned lhs, gray_code<Unsigned> rhs) noexcept
    -> bool
{
    return (lhs ^ (lhs >> 1)) != rhs.value;
}

////////////////////////////////////////////////////////////
// Bitwise operations

template<typename Unsigned>
constexpr auto operator&(gray_code<Unsigned> lhs, gray_code<Unsigned> rhs) noexcept
    -> gray_code<Unsigned>
{
    return lhs &= rhs;
}

template<typename Unsigned>
constexpr auto operator|(gray_code<Unsigned> lhs, gray_code<Unsigned> rhs) noexcept
    -> gray_code<Unsigned>
{
    return lhs |= rhs;
}

template<typename Unsigned>
constexpr auto operator^(gray_code<Unsigned> lhs, gray_code<Unsigned> rhs) noexcept
    -> gray_code<Unsigned>
{
    return lhs ^= rhs;
}

template<typename Unsigned>
constexpr auto operator~(gray_code<Unsigned> val) noexcept
    -> gray_code<Unsigned>
{
    val.value = ~val.value;
    return val;
}

template<typename Unsigned>
constexpr auto operator>>(gray_code<Unsigned> val, std::size_t pos) noexcept
    -> gray_code<Unsigned>
{
    return val >>= pos;
}

template<typename Unsigned>
constexpr auto operator<<(gray_code<Unsigned> val, std::size_t pos) noexcept
    -> gray_code<Unsigned>
{
    return val <<= pos;
}

////////////////////////////////////////////////////////////
// Bitwise operations with bool

template<typename Unsigned>
constexpr auto operator&(gray_code<Unsigned> lhs, bool rhs) noexcept
    -> gray_code<Unsigned>
{
    return lhs &= rhs;
}

template<typename Unsigned>
constexpr auto operator&(bool lhs, gray_code<Unsigned> rhs) noexcept
    -> gray_code<Unsigned>
{
    return rhs &= lhs;
}

template<typename Unsigned>
constexpr auto operator|(gray_code<Unsigned> lhs, bool rhs) noexcept
    -> gray_code<Unsigned>
{
    return lhs |= rhs;
}

template<typename Unsigned>
constexpr auto operator|(bool lhs, gray_code<Unsigned> rhs) noexcept
    -> gray_code<Unsigned>
{
    return rhs |= lhs;
}

template<typename Unsigned>
constexpr auto operator^(gray_code<Unsigned> lhs, bool rhs) noexcept
    -> gray_code<Unsigned>
{
    return lhs ^= rhs;
}

template<typename Unsigned>
constexpr auto operator^(bool lhs, gray_code<Unsigned> rhs) noexcept
    -> gray_code<Unsigned>
{
    return rhs ^= lhs;
}

////////////////////////////////////////////////////////////
// Bitwise assignment operations

template<typename Unsigned>
constexpr auto operator&=(Unsigned& lhs, gray_code<Unsigned> rhs) noexcept
    -> Unsigned&
{
    return lhs &= rhs.value;
}

template<typename Unsigned>
constexpr auto operator|=(Unsigned& lhs, gray_code<Unsigned> rhs) noexcept
    -> Unsigned&
{
    return lhs |= rhs.value;
}

template<typename Unsigned>
constexpr auto operator^=(Unsigned& lhs, gray_code<Unsigned> rhs) noexcept
    -> Unsigned&
{
    return lhs ^= rhs.value;
}

////////////////////////////////////////////////////////////
// Utility functions

template<typename Unsigned>
constexpr auto swap(gray_code<Unsigned>& lhs, gray_code<Unsigned>& rhs) noexcept
    -> void
{
    auto tmp = lhs.value;
    lhs.value = rhs.value;
    rhs.value = tmp;
}

////////////////////////////////////////////////////////////
// Mathematical functions

template<typename Unsigned>
constexpr auto is_even(gray_code<Unsigned> code) noexcept
    -> bool
{
    return not is_odd(code);
}

template<typename Unsigned>
constexpr auto is_odd(gray_code<Unsigned> code) noexcept
    -> bool
{
    // A Gray code is odd when the number of bits set in
    // its representation is odd

    #if defined(__GNUC__) || defined(__clang__)

        // Compiler intrinsics tend to be the fastest
        return static_cast<bool>(__builtin_parity(code.value));

    #else

        for (std::size_t i = std::numeric_limits<Unsigned>::digits / 2 ; i ; i >>= 1)
        {
            code.value ^= (code.value >> i);
        }
        return static_cast<bool>(code.value & 1);

    #endif
}
