---
layout: post
title: Gray Codes and Parity
date: 2016-05-29 18:08:43 +0200
categories: [gray code, c++]
---
One of the most basic operations in $$ \mathbb{G} $$ is to check the parity of a number, to know whether it is even or
odd. Several algorithms behave differently depending on the parity of a Gray, which makes the operation pretty fundamental
to develop more evolved algorithms. As a small reminder, the parity of an integer can be defined as follows:

$$\begin{array}{ccccc}
parity & : & \mathbb{N} & \longrightarrow  & \{0, 1\}
\end{array}$$

$$\forall n \in \mathbb{N} : parity(n) = \begin{cases}
0 & \text{if } n \bmod 2 = 0\\
1 & \text{otherwise}
\end{cases}$$

Note that the name [« parity function »][wiki-parity-function] is generally used for a function that returns whether the
number of bits set in a bit vector is even or odd. I don't know whether overloading functions is a thing in mathematical
notation, but the concept should be obvious to most programmers, so we will add the following overload for $$ parity $$
that operates on $$ \{0, 1\}^* $$ and uses the overload of the function on $$ \mathbb{N} $$ as well as the function
[$$ popcount $$][wiki-popcount] that returns the number of bits set in a bit vector:

$$\begin{array}{ccccc}
parity & : & \{0, 1\}^* & \longrightarrow  & \{0, 1\}
\end{array}$$

$$ \forall n \in \{0, 1\}^* : parity(n) = parity(popcount(n)) $$

Now let's come back to Gray codes. Computing the parity of a Gray code is rather easy thanks to the following property:

$$\begin{array}{ccccc}
parity & : & \mathbb{G} & \longrightarrow  & \{0, 1\}
\end{array}$$

$$ \forall n \in \mathbb{G} : parity(n) = parity(repr(n)) $$

Basically, a Gray code is even if the number of bits set in its representation is even, and it is odd when the numbers of
bits set in its representation is odd. This property stems from a simple observation: one of the most fundamental properties
of Gray codes is that adjacent values differ by a single bit, which means that the parity of the number of bits set in a
Gray code always differs from that of its neighbours. The following lines give the inductive intuition needed to prove the
property:

* $$ repr(0_{\mathbb{G}}) = 0b0 \Rightarrow parity(0_{\mathbb{G}}) = parity(repr(0_{\mathbb{G}})) $$;
* $$ \forall n \in \mathbb{G} : parity(n) \ne parity(n+1) $$ because $$ \mathbb{G} $$ models $$ \mathbb{N} $$;
* $$ \forall n \in \mathbb{G} : parity(repr(n)) \ne parity(repr(n+1)) $$ because a single bit is flipped whenever
a Gray code is incremented.

Assuming that the population count returns a value in $$ \mathbb{B} $$, computing its parity is just a matter of checking
whether the last bit of its representation is equal to one or zero. However, the population count of an unsigned integer is
typically an operation that runs in $$ O(n) $$, which is a bit expensive. Fortunately, there are [several dedicated
methods][bit-twiddling] to compute the parity of a bit vector by hand. My favourite one is probably the one named « Compute
parity in parallel », with the following simplified generic implementation:

{% highlight cpp %}
template<typename Unsigned>
constexpr auto parity(Unsigned value)
    -> bool
{
    for (std::size_t i = std::numeric_limits<Unsigned>::digits / 2 ;
         i > 0 ; i >>= 1)
    {
        value ^= (value >> i);
    }
    return static_cast<bool>(value & 1);
}
{% endhighlight %}

The reasoning behind this algorithm comes from the parity of a bit vector can be obtained by xoring together every bit. Let
$$ k $$ be the number of bits in the bit vector, we have:

$$\begin{array}{ccccc}
parity & : & \{0, 1\}^k & \longrightarrow  & \{0, 1\}
\end{array}$$

$$ \forall n \in \{0, 1\}^k : parity(n) = \bigoplus_{i=0}^{k-1} n_i $$

The algorithm above does that in parallel: for a 32-bit vector, it xors the 16 most significant bits into the 16 least
significant ones, then it only consider the remaining 16 least significant bits: it xors their 8 most significant bits into
their 8 least significant bits, etc... until the least significant bit contains the result of xoring every bit together. It
is a $$ O(\log{n}) $$ algorithm while the naive algorithm to xor every bit runs in $$ O(n) $$. Now, the algorithm on Bit
Twiddling Hacks contains an additional trick to reduce the number of instructions, which we can also add to our algorithm:

{% highlight cpp %}
template<typename Unsigned>
constexpr auto parity(Unsigned value)
    -> bool
{
    for (std::size_t i = std::numeric_limits<Unsigned>::digits / 2 ;
         i > 2 ; i >>= 1)
    {
        value ^= (value >> i);
    }
    value &= 0xf;
    return static_cast<bool>(
        (0b0110'1001'1001'0110 >> value) & 1
    );
}
{% endhighlight %}

Once the number of remaining bits to xor together is 4, we isolate them with `value &= 0xf`, and we are guaranteed to obtain
a number between $$ 0 $$ and $$ 15 $$. We build a build lookup table `0b0110'1001'1001'0110` where the bits correspond
to the parity of the bit vectors corresponding the numbers $$ 0_\mathbb{B} $$ to $$ 15_\mathbb{B} $$. We only have to
shift that lookup table by `value` bits and the least significant bit of the result will correspond to the parity of
`value`. We would need at least 256-bit integers to provide a lookup table big enough to strip a few more instructions.

Anyway, many compilers provide intrisics to query either the population count or the parity of a bit vector (well, as long
as we consider that a built-in integer is a bit vector). The **cpp-gray** library provides two functions to query the parity
of a `gray_code`: `is_odd` and `is_even`. They are implemented as follows:

{% highlight cpp %}
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
    for (std::size_t i = std::numeric_limits<Unsigned>::digits / 2 ;
         i > 2 ; i >>= 1)
    {
        code.value ^= (code.value >> i);
    }
    code.value &= 0xf;
    return static_cast<bool>(
        (0b0110'1001'1001'0110 >> code.value) & 1
    );
#endif
}

template<typename Unsigned>
constexpr auto is_even(gray_code<Unsigned> code) noexcept
    -> bool
{
    return not is_odd(code);
}
{% endhighlight %}

No `parity` function is provided because `is_odd` and `is_even` feel clearer. The compilter intrinsic `__builtin_parity`
apparently calls architecture-specific `popcnt` instructions when they can, making the whole thing faster than our handmade
algorithm, which is why we only use it as a fallback algorithm. I am no expert when it comes to other compilers, which is
why the use of intrinsics is limited to g++ and clang++. Another solution to compute the parity of a bit vector would have
been to use [`std::bitset::count`][bitset-count], which is the function in the standard library the most likely to map to a
native `popcnt` instruction.


  [bit-twiddling]: http://graphics.stanford.edu/~seander/bithacks.html#ParityNaive
  [bitset-count]: http://en.cppreference.com/w/cpp/utility/bitset/count
  [wiki-parity-function]: https://en.wikipedia.org/wiki/Parity_function
  [wiki-popcount]: https://en.wikipedia.org/wiki/Hamming_weight
