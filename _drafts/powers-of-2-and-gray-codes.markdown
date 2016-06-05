---
layout: post
title: Powers of 2 and Gray Codes
date: 2016-06-05 12:08:43 +0200
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


  [wiki-egyptian-multiplication]: https://en.wikipedia.org/wiki/Ancient_Egyptian_multiplication
  [wiki-euclidean-division]: https://en.wikipedia.org/wiki/Euclidean_division