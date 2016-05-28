---
layout: post
title: Converting To and From Gray Code
date: 2016-05-25 21:50:43 +0200
categories: [gray code, c++]
---
Don't expect this article to be original or even interesting: how to convert a two's complement integer to and from a Gray
code is probably the single most asked and answered question when it comes to Gray codes. On the other hand, considering
that operations on two's complement integers are typically efficient and that the same operations in the Gray code domain
are mostly unknown, converting the numbers back and forth between their different representations is something that is
needed whenever one wants to work with Gray codes.

For the sake of it, we will introduce the two following functions, `to_gray` and `from_gray`, used to convert numbers
between their representations in \\( \mathbb{B} \\) and \\( \mathbb{G} \\):

$$\begin{array}{ccccc}
to\_gray & : & \mathbb{B} & \longrightarrow & \mathbb{G}
\end{array}$$

$$\begin{array}{ccccc}
from\_gray & : & \mathbb{G} & \longrightarrow & \mathbb{B}
\end{array}$$

Giving them a full mathematical definition would be pretty pointless since both of them are simply converting a number to
the exact same number, but with another binary representation. Instead, let's practice one of my favourite things when it
comes to programming: stealing code. [Wikipedia][wiki-gray-code] already has this covered. Ignoring the least efficient
algorithm to convert a Gray code to an unsigned two's complement integer, here is their C code:

{% highlight c %}
/*
 * This function converts an unsigned binary
 * number to reflected binary Gray code.
 *
 * The operator >> is shift right. The operator ^ is exclusive or.
 */
unsigned int binaryToGray(unsigned int num)
{
    return num ^ (num >> 1);
}

/*
 * A more efficient version, for Gray codes of 32 or fewer bits.
 */
unsigned int grayToBinary32(unsigned int num)
{
    num = num ^ (num >> 16);
    num = num ^ (num >> 8);
    num = num ^ (num >> 4);
    num = num ^ (num >> 2);
    num = num ^ (num >> 1);
    return num;
}
{% endhighlight %}

The first algorithm is already as good as it can be: it runs in \\( O(1) \\) time and works regardless of the number of bits
of the two's complement integer to convert. The second algorithm runs in \\( O(\log{n}) \\) and is apparently the fastest
known algorithm to convert a two's complement integer to a Gray code. However, the algorithm assumes that the integer has 32
bits (or fewer) while we would like a generic one. Well, let's rewrite these functions to make them more generic:

{% highlight cpp %}
template<typename Unsigned>
auto to_gray(Unsigned value)
    -> Unsigned
{
    return value ^ (value >> 1);
}

template<typename Unsigned>
auto from_gray(Unsigned value)
    -> Unsigned
{
    for (Unsigned mask = std::numeric_limits<Unsigned>::digits / 2 ;
         mask ; mask >>= 1)
    {
        value ^= value >> mask;
    }
    return value;
}
{% endhighlight %}

Now, be ready to be disappointed: I never understood how nor why those algorithms worked. The patterns were obvious enough
to make `from_gray` agnostic of the number of bits, but that's pretty much it. In **cpp-gray**, the function `to_gray` is
what is hidden in `gray_code`'s constructor from a `value_type`, while `from_gray` basically corresponds to the
implementation of its explicit conversion function `operator value_type`.


  [wiki-gray-code]: https://en.wikipedia.org/wiki/Gray_code#Converting_to_and_from_Gray_code
