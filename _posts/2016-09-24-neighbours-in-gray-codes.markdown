---
layout: post
title: Neighbours in Gray Codes
date: 2016-09-24 15:20:52 +0200
categories: [gray code, c++]
---
### Successor and predecessor of a Gray code

One of the most fundamental operations on integral numbers is finding the successor of a given number (and thus finding its
predecessor since the operations are inherently linked). When it comes to Gray Code, we can reuse the property that we used
to find the parity of a Gray to have an intuition about the implementation of the incrementation and decrementation
operations: the representations of adjacent Gray code numbers differ by a single bit. This means that we only need to find
which bit we have to change to implement these operations. Honestly, I don't have a proper explanation about why some bits
are picked rather than others, instead have some pseudocode from [this excellent answer][so-gray-code-increment] on
StackOverflow:

{% highlight swift %}
gray_inc(x):
  if parity of x is even:
    return x xor 1
  if parity of x is odd:
    let y be the rightmost 1 bit in x
    return x xor (y leftshift 1)
{% endhighlight %}

Since we already have a \\( parity_\mathbb{G} \\) function, the only new part in the function above is isolating the least
significant bit set in `x`. Fortunately, this is a well-known operation: the least significant bit of `x` is `x & -x`.
Taking this information into to account, we can implement `gray_code::operator++` as follows:

{% highlight cpp %}
template<typename Unsigned>
constexpr auto gray_code<Unsigned>::operator++() noexcept
    -> gray_code&
{
    if (is_odd(*this)) {
        auto y = value & -value;
        value ^= (y << 1);
    } else {
        value ^= 1;
    }
    return *this;
}
{% endhighlight %}

Getting the predecessor of a Gray code is really similar to getting to getting its successor: if a bit has been flipped to
get the successor of a Gray code, then flipping it again gives the original value, which happens to be the predecessor of
the successor. To implement `gray_code::operator--`, one only has to reverse the parity condition. Giving the implementation
here would be a bit redundant, so it will be left as an exercise for the reader (as well as postfix operators).

These algorithms can be translated to our pseudo-formal notation:

$$\begin{array}{ccccc}
successor & : & \mathbb{G} & \longrightarrow  & \mathbb{G}
\end{array}$$

$$\forall n \in \mathbb{G} : successor(n) = \begin{cases}
n \oplus 0b1 & \text{if } n \text{ is even}\\
n \oplus ((repr(n) \& -repr(n)) \ll 1) & \text{otherwise}
\end{cases}$$

$$\begin{array}{ccccc}
predecessor & : & \mathbb{G} & \longrightarrow  & \mathbb{G}
\end{array}$$

$$\forall n \in \mathbb{G} : predecessor(n) = \begin{cases}
n \oplus 0b1 & \text{if } n \text{ is odd}\\
n \oplus ((repr(n) \& -repr(n)) \ll 1) & \text{otherwise}
\end{cases}$$

### Adding a wrapping behaviour

Compared to the usual unsigned integer incrementation operation, it still has a « problem »: when the value of the Gray
code is the highest value representable by the given amount of bits, the algorithm above doesn't return \\( 0_\mathbb{G} \\)
but something else. If this kind of behaviour is wanted, one can special-case the algorithm to handle it. The Gray code
representation of the highest value has all bits set to \\( 0 \\) but the most significant one.

{% highlight cpp %}
template<typename Unsigned>
constexpr auto gray_code<Unsigned>::operator++() noexcept
    -> gray_code&
{
    constexpr value_type msb
        = 1 << (std::numeric_limits<value_type>::digits - 1);

    if (is_odd(*this)) {
        if (value == msb) {
            value = 0b0;
        } else {
            auto y = value & -value;
            value ^= (y << 1);
        }
    } else {
        value ^= 1;
    }
    return *this;
}
{% endhighlight %}

This additional check, even if properly positioned to only occur when the value is odd, still incurs an overhead. That
overhead can somehow be mitigated by using compiler extensions such as GCC's `__builtin_expect`, assuming that the wrapping
behaviour is exceptional.

No matter whether the additional check for the wrapping behaviour is performed or not, the complexity of the algorithm is
dominated by the complexity of the \\( parity_\mathbb{G} \\) operation, making it an \\( O(\log{n}) \\) algorithm.

### Getting both neighbours at once

A small but interesting observation is that the parity doesn't need to be known in order to return *both* neighbours of a
Gray code at once if the order doesn't matter. Even if one wants the wrapping behaviour, only two checks against specific
values are needed. While getting the successor or predecessor specifically is an \\( O(\log{n}) \\) operation, getting
both at once without a specific order is a mere \\( O(1) \\) operation.

{% highlight cpp %}
template<typename Unsigned>
constexpr auto neighbors(gray_code<Unsigned> code) noexcept
    -> std::pair<gray_code<Unsigned>, gray_code<Unsigned>>
{
    constexpr Unsigned msb
        = 1 << (std::numeric_limits<Unsigned>::digits - 1);

    gray_code<Unsigned> first, second;
    if (code.value == msb) {
        auto y = code.value & -code.value;
        first.value = code.value ^ (y << 1);
        second.value = 0b0;
    } else if (code.value == 0b0) {
        first.value = msb;
        second.value = 0b1;
    } else {
        auto y = code.value & -code.value;
        first.value = code.value ^ (y << 1);
        second.value = code.value ^ 1;
    }
    return std::make_pair(first, second);
}
{% endhighlight %}

### Repeteadly incrementing a Gray code

The Gray code incrementation operation being \\( O(\log{n}) \\), naively repeating it \\( n \\) times (for example
incrementing a Gray counter in a `for` loop) runs in \\( O(n \log{n}) \\). However, knowing the parity of a number is enough
to know the parity of its successor, which means that one can take this information into account if they want to repeatedly
increment a Gray code \\( n \\) times, turning such an \\( O(n \log{n}) \\) algorithm into an \\( O(n) \\) one.


  [so-gray-code-increment]: http://stackoverflow.com/a/17493235/1364752