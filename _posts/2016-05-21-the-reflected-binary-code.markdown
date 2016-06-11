---
layout: post
title: "The Reflected Binary Code"
date: 2016-05-21 17:32:43 +0200
categories: gray code
---
In this article, we define and explain most of the sets that will be in the rest of the blog, as well as the most common
operations to operate on them. We also define a couple of additional symbols akin to programming language literals that will
be used to represent values of the different sets in a terse manner.

### Sets and basic operations

Let \\( \\{0, 1\\}^* \\) be the set of bit vectors of any size. When the size of a bit vector is explicitly needed, we will
use a subset of the form \\( \\{0, 1\\}^n \\) where \\( n \\) is the number of bits in the bit vector. The special subset
\\( \\{0, 1\\} \\) without a superscript denotes the [Boolean domain][wiki-boolean-domain] (the mathematical symbol
\\( \mathbb{B} \\) is sometimes used to represent the Boolean domain, but we give it another meaning in this blog), where
\\( 0 \\) means « false » and \\( 1 \\) means « true ».

Let \\( \mathbb{B} \\) and \\( \mathbb{G} \\) be two sets of numbers, each of them sufficient to model the set of natural
numbers \\( \mathbb{N} \\). Both of these sets support all of the operations supported by \\( \mathbb{N} \\) with the same
semantics. However, their behaviour differs with regard to the following \\( repr \\) bijective function:

$$\begin{array}{ccccc}
repr & : & \mathbb{G} & \longrightarrow  & \{0, 1\}^*\\
repr & : & \mathbb{B} & \longrightarrow  & \{0, 1\}^*
\end{array}$$

The \\( repr \\) function associates a different bit vector from \\( \\{0, 1\\}^* \\) to every element of \\( \mathbb{B} \\)
and \\( \mathbb{G} \\) as follows:

* To any element of \\( \mathbb{B} \\), \\( repr \\) associates a bit vector corresponding to the usual representation of
the element in two's complement, or [« natural binary code »][wiki-natural-binary].
* To any element of \\( \mathbb{G} \\), \\( repr \\) associates a bit vector corresponding to the reflected binary [Gray
code][wiki-gray-code] representation of the element.

We abuse a few mathematical symbols to define the following bitwise operations on \\( \\{0, 1\\}^* \\) akin to those used in
C++ and other well-known programming languages:

* \\( \& \\) is a bitwise AND.
* \\( \| \\) is a bitwise OR.
* \\( \oplus \\) is a bitwise XOR.
* \\( \ll \\) is a binary left shift.
* \\( \gg \\) is a binary right shift.
* \\( \sim \\) is a prefix unary bitwise NOT.

We also define all of the operations above on \\( \mathbb{B} \\) and \\( \mathbb{G} \\) in the following manner: let
\\( \otimes \\) be any of the binary operators above, and let \\( \mathbb{X} \\) be either \\( \mathbb{B} \\) or
\\( \mathbb{G} \\):

$$ \forall x, y \in \mathbb{X} : x \otimes y = repr^{-1}(repr(x) \otimes repr(y)) $$

I guess that how \\( \sim \\) is defined on \\( \mathbb{B} \\) and \\( \mathbb{G} \\) is pretty obvious, so I won't repeat
a similar formula.

### Literals

To easily understand which kinds of values we are operating on, we define additional symbols that I will call « literals »
since it's the name we give them in the computer science world (which is distinct from the mathematical notion of literals).
We define literals for both \\( \mathbb{N} \\), \\( \mathbb{B} \\), \\( \mathbb{G} \\) and \\( \\{0, 1\\}^* \\) as follows:
use the following notations to represent numbers:

* When it does not matter whether a number belongs to \\( \mathbb{B} \\) or \\( \mathbb{G} \\), we will use the usual
integral numbers notation and assume that we work on \\( \mathbb{N} \\):
 
    $$ 0, 1, 16, 23 $$

* A number explicitly belonging to \\( \mathbb{B} \\) will be denoted by a subscript \\( \mathbb{B} \\): 

    $$ 0_{\mathbb{B}}, 1_{\mathbb{B}}, 16_{\mathbb{B}}, 23_{\mathbb{B}} $$

* A number explicitly belonging to \\( \mathbb{G} \\) will be denoted by a subscript \\( \mathbb{G} \\): 

    $$ 0_{\mathbb{G}}, 1_{\mathbb{G}}, 16_{\mathbb{G}}, 23_{\mathbb{G}} $$

* When needed, the binary representation of the numbers will use \\( \\{0, 1\\}^* \\) literals, which are bit vectors
prefixed by \\( 0b \\) to resemble binary literals in several programming languages:

  $$ 0b0, 0b1, 0b10000, 0b10111 $$

### Additional subsets of \\( \mathbb{G} \\)

Now that the required notation has been explained, let's come back to Gray codes and their properties. As explained in [this
StackOverflow answer][so-reflected], the term « reflected binary code » comes from the way Frank Gray constructed the binary
representation of Gray codes: when we look at the binary representation of the first \\( n \\) Gray codes, where \\( n \\)
is a power of \\( 2 \\), we can notice that the first half and the second half have a similar binary representation (except
for the first bit), but in reverse order as if reflected by a mirror.

```
0000            x000
0001            x001
0011            x011
0010            x010
0110            x110
0111            x111
0101            x101
0100            x100
------ mirror ------
1100            x100
1101            x101
1111            x111
1110            x110
1010            x010
1011            x011
1001            x001
1000            x000
```

This pattern is rather interesting, because the « normal » and the « reflected » parts tend to have different properties
when bitwise operations are applied to them. Formally, we will define the sets \\( \mathbb{G}^- \\) and \\( \mathbb{G}^+ \\)
as follows:

$$ \forall n \in \mathbb{N}: \mathbb{G}_n^- = \{ x \in \mathbb{G} : 2^n \le x < 2^n + 2^{n-1} \} $$

$$ \mathbb{G}^- = \bigcup_{i=1}^{\infty} \mathbb{G}_i^- $$

$$ \forall n \in \mathbb{N}: \mathbb{G}_n^+ = \{ x \in \mathbb{G} : 2^n + 2^{n-1} \le x < 2^{n+1} \} $$

$$ \mathbb{G}^+ = \bigcup_{i=1}^{\infty} \mathbb{G}_i^+ $$

These subsets of \\( \mathbb{G} \\) are interesting because some bitwise operations only work on \\( \mathbb{G}^- \\) while
some others only work on \\( \mathbb{G}^+ \\) as we will see in other articles. The properties tend to become a little bit
wonky for the values \\( 0_{\mathbb{G}} \\) and \\( 1_{\mathbb{G}} \\), and the algorithms almost always need special cases
to handle these values, which is why they are not properly handled by the definitions above. Putting everything together, we
can provide an alternative definition for \\( \mathbb{G} \\) even though it ends up being a bit recursive:

$$ \mathbb{G} = \{ 0_{\mathbb{G}}, 1_{\mathbb{G}} \} \cup \mathbb{G}^- \cup \mathbb{G}^+ $$

That's pretty much it for the abuses of notation that we will use to describe operations on natural numbers encoded either
in two's complement or as Gray codes in this blog. Don't hesitate to comment if you have questions or if you believe that
you found errors in the code or in the descriptions.


  [so-reflected]: http://stackoverflow.com/a/34555899/1364752
  [wiki-boolean-domain]: https://en.wikipedia.org/wiki/Boolean_domain
  [wiki-gray-code]: https://en.wikipedia.org/wiki/Gray_code
  [wiki-natural-binary]: https://en.wikipedia.org/wiki/Binary_number
