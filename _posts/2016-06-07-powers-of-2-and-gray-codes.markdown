---
layout: post
title: Powers of 2 and Gray Codes
date: 2016-06-07 21:38:43 +0200
categories: [gray code, c++]
---
Two's complement binary is inherently linked to \\( 2 \\) and its powers. Gray code is not linked to \\( 2 \\) as much as
two's complement is, but \\( 2 \\) and its powers still play an interesting role and have a few properties ready to be
exploited in algorithms.

### Powers of \\( 2 \\)

Powers of \\( 2 \\) are remarkable numbers in Gray code. Except for \\( 1_\mathbb{G} \\), the Gray code representation of a
power of \\( 2 \\) is made of two ones followed by zero or more zeroes. One could define them as:

$$\forall n \in \mathbb{N} : repr(2_\mathbb{G}^n) = \begin{cases}
0b1 & \text{if } n = 0\\
0b11 \ll (n - 1) & \text{otherwise}
\end{cases}$$

Note that if negative left shift was allowed, we wouldn't even need the special case for \\( n = 0 \\). However, this blog
focuses on C++, and a negative left shift is undefined behaviour in C++. The definition above can easily proved to be true.
We know that the two's representation of powers of \\( 2 \\) can be expressed as follows:

$$ \forall n \in \mathbb{N} : repr(2_\mathbb{B}^n) = 0b1 \ll n $$

And the function \\( to\\_gray \\) can be expressed as follows:

$$ \forall n \in \mathbb{B} : to\_gray(n) = n \oplus (n \gg 1) $$

Therefore, we have the following equivalence (ignoring the special case when \\( n = 0 \\)):

$$\begin{align*}
\forall n \in \mathbb{N} : repr(to\_gray(2_\mathbb{B}^n)) &= (0b1 \ll n) \oplus ((0b1 \ll n) \gg 1)\\
&= (0b1 \ll n) \oplus (0b1 \ll (n - 1))\\
&= (0b10 \ll (n - 1)) \oplus (0b1 \ll (n - 1))\\
&= 0b11 \ll (n - 1)
\end{align*}$$

### Multiplying and dividing by \\( 2 \\)

Even though multiplication and division by \\( 2 \\) are mere special cases of multiplication and division in the grand
scheme of things, they are often used as specific operations in other more evolved algorithms such as the [ancient Egyptian
multiplication][wiki-egyptian-multiplication]. We even have names for these operations: « doubling » and « halving ».
Halving a Gray code can be done as follows:

$$ \forall n \in \mathbb{G} : \frac{n}{2} = n \gg 1 $$

Of course this operation is an [Euclidean division][wiki-euclidean-division], which means that the remainder is lost when
an odd number is halved. Doubling a Gray code depends on whether the Gray code is even or odd:

$$ \forall n \in \mathbb{G}_{Even} : n * 2 = n \ll 1 $$

$$ \forall n \in \mathbb{G}_{Odd} : n * 2 = (n \ll 1) \oplus 1 $$

These two doubling formulae can be trivially turned into a generic single one using \\( parity \\):

$$ \forall n \in \mathbb{G} : n * 2 = (n \ll 1) \oplus parity(n) $$

There are no corresponding functions in **cpp-gray** since those formulae are too specific to warrant any, but knowing the
parity of a Gray code in advance can be used to avoid the potentially \\( \log{n} \\) call to \\( parity \\), which is
sometimes useful in more advanced algorithms.

### Multiplying and dividing by any power of \\( 2 \\)

Multiplying or dividing a Gray code by a power of \\( 2 \\) is no harder than multiplying or dividing it by \\( 2 \\) since
it's mostly a repeated operation. I don't think I have to explain anything so here are the formulae (ignoring the special
case when \\( k = 0 \\), which wouldn't need to be special cased if it was possible to shift things by a negative number):

$$ \forall n \in \mathbb{G}, k \in \mathbb{N} : \frac{n}{2^k} = n \gg k $$

$$ \forall n \in \mathbb{G}_{Even}, k \in \mathbb{N} : n * 2^k = n \ll k $$

$$ \forall n \in \mathbb{G}_{Odd}, k \in \mathbb{N} : n * 2^k = ((n \ll 1) \oplus 1) \ll (k - 1) $$

$$ \forall n \in \mathbb{G}, k \in \mathbb{N} : n * 2^k = ((n \ll 1) \oplus parity(n)) \ll (k - 1) $$

Just like with \\( 2 \\), these operations run in \\( O(1) \\), expect the generic multiplication by a power of \\( 2 \\)
which might run in \\( O( \log{n} ) \\) depending on its implementation, which makes them cheap to use to create more
evolved algorithms.

### Hyperfloor and hyperceiling

The [hyperfloor][se-math-hyperfloor] and hyperceiling functions, denoted \\( \lfloor \lfloor n \rfloor \rfloor \\) and
\\( \lceil \lceil n \rceil \rceil \\) are functions that respectively round an integer to the previous or next power of
\\( 2 \\):

$$ \forall n \in \mathbb{N} : \lfloor \lfloor n \rfloor \rfloor = 2^{\lfloor \log_2 n \rfloor} $$

$$ \forall n \in \mathbb{N} : \lceil \lceil n \rceil \rceil = 2^{\lceil \log_2 n \rceil} $$

In \\( \mathbb{B} \\), computing the hyperfloor of a number amounts to isolating its highest set bit. In \\( \mathbb{G} \\),
it amounts to isolating that same bit and performing \\( to\\_gray \\), which is pretty handy considering that we don't have
to convert the Gray code back to an integer first: the highest set bit of any number in \\( \mathbb{B} \\) is exactly the
same as in \\( \mathbb{G} \\). [The Aggregate Magic Algorithms][aggregate-magic-algo-msb] gives us the following
\\( \log{n} \\) algorithm to isolate the highest set bit in a 32-bit integer:

{% highlight c %}
unsigned int
msb32(register unsigned int x)
{
        x |= (x >> 1);
        x |= (x >> 2);
        x |= (x >> 4);
        x |= (x >> 8);
        x |= (x >> 16);
        return(x & ~(x >> 1));
}
{% endhighlight %}

This algorithm repeatedly ORs its value with itself shifted by powers of \\( 2 \\) to ensure that the most significant set
bit will remain the most significant set bit but that every bit to its right will be set too. The last line performs an
interesting operation: every bit is ANDed with the opposite of its left neighbour. The result is that any set bit with a
clear bit at its left will remain set while every other bit will be cleared. Since only the most significant bit has a clear
left neighbour priori to the operation, it will be the only remaining set bit. Let's write a generic C++ version that works
with unsigned integers of any size:

{% highlight cpp %}
template<typename Unsigned>
auto hyperfloor(Unsigned value)
    -> Unsigned
{
    for (std::size_t i = 1 ; i <= std::numeric_limits<Unsigned>::digits / 2 ;
         i <<= 1)
    {
        x |= x >> i;
    }
    return x & ~(x >> 1);
}
{% endhighlight %}

The Gray code version would be trivial to implement by taking the result and applying \\( res \oplus (res \gg 1) \\), but
there is a way to use even less operations: in the last step of the algorithm above, instead of the highest set bit, we want
the highest set bit *and* its right neighbour. Remember that before the last step, we have the highest set bit and all the
bits at its right set. Consequently, if with shift the value to AND by two bits instead of one, the highest set bit *and*
its neighbour will be ANDed with set bits (thanks to the zero fill on the left due to the shift performed before the NOT),
resulting in the Gray code's hyperfloor:

{% highlight cpp %}
template<typename Unsigned>
auto hyperfloor(gray_code<Unsigned> value)
    -> gray_code<Unsigned>
{
    for (std::size_t i = 1 ; i <= std::numeric_limits<Unsigned>::digits / 2 ;
         i <<= 1)
    {
        x |= x >> i;
    }
    return x & ~(x >> 2);
}
{% endhighlight %}

That's it: \\( \lfloor \lfloor n \rfloor \rfloor_\mathbb{G} \\) is just as cheap as \\( \lfloor \lfloor n \rfloor
\rfloor_\mathbb{B} \\), making it a good canditate to compose more advanced algorithms since it avoids the roundtrip by
two's complement integers. Computing the hyperceiling is mostly equivalent, so it will be left as an exercise to the reader.
Note however that the function returns \\( 0 \\) when the input is \\( 0 \\), which means that an additional check might be
needed to handle that case when it does matter.

As always, if you feel that something is missing, don't hesitate to report it in the [issues][issues] section of the
associated repository with the « blog » label.


  [aggregate-magic-algo-msb]: http://aggregate.org/MAGIC/#Most%20Significant%201%20Bit
  [issues]: https://github.com/Morwenn/cpp-gray/issues
  [se-math-hyperfloor]: http://math.stackexchange.com/a/1311232/36352
  [wiki-egyptian-multiplication]: https://en.wikipedia.org/wiki/Ancient_Egyptian_multiplication
  [wiki-euclidean-division]: https://en.wikipedia.org/wiki/Euclidean_division