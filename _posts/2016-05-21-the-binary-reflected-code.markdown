---
layout: post
title: "The Binary Reflected Code"
date: 2016-05-21 17:32:43 +0200
categories: gray code
---
*In this article, we define and explain most of the symbols and operations that will be used in the rest of the blog.*

Let \\( \mathbb{G} \\) be the set of Gray code number sufficient to represent the set of natural numbers \\( \mathbb{N} \\).
The natural numbers represented by \\( \mathbb{G} \\) have all the mathematical properties of the numbers in
\\( \mathbb{N} \\). However, we also specify that the numbers in \\( \mathbb{G} \\) have a Gray code binary representation.
We also call \\( \mathbb{B} \\) the equivalent set of numbers with a two's complement (or « natural binary ») representation.
We insolently abuse the following mathematical symbols to define basic bitwise operations on both \\( \mathbb{G} \\) and
\\( \mathbb{B} \\) akin to those used in common programming languages:

* \\( \& \\) is a bitwise AND.
* \\( \| \\) is a bitwise OR.
* \\( \oplus \\) is a bitwise XOR.
* \\( \ll \\) is a binary left shift.
* \\( \gg \\) is a binary right shift.

Since the binary representation of the numbers in both \\( \mathbb{G} \\) and \\( \mathbb{B} \\) does matter, we will also
use the following notations to represent numbers:

* When it does not matter whether a number belongs to \\( \mathbb{G} \\) or \\( \mathbb{B} \\), we will use the usual
integral numbers notation:
 
    $$0, 1, 16, 23$$

* A number belonging to \\( \mathbb{G} \\) will be denoted by a subscript \\( \mathbb{G} \\): 

    $$0_{\mathbb{G}}, 1_{\mathbb{G}}, 16_{\mathbb{G}}, 23_{\mathbb{G}}$$

* A number belonging to \\( \mathbb{B} \\) will be denoted by a subscript \\( \mathbb{B} \\): 

    $$0_{\mathbb{B}}, 1_{\mathbb{B}}, 16_{\mathbb{B}}, 23_{\mathbb{B}}$$

* When the binary representation matters, a string of bits prefixed by \\( 0b \\) will be used as in several programming
languages:

  $$0b0, 0b1, 0b10000, 0b10111$$

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

$$ \mathbb{G}^- = \bigcup_{i=1}^{n} \mathbb{G}_i^- $$

$$ \forall n \in \mathbb{N}: \mathbb{G}_n^+ = \{ x \in \mathbb{G} : 2^n + 2^{n-1} \le x < 2^{n+1} \} $$

$$ \mathbb{G}^+ = \bigcup_{i=1}^{n} \mathbb{G}_i^+ $$

These subsets of \\( \mathbb{G} \\) are interesting because some bitwise operations only work on \\( \mathbb{G}^- \\) while
some others only work on \\( \mathbb{G}^+ \\) as we will see in other articles. The properties tend to become a little bit
wonky whenever \\( x = 0_{\mathbb{G}} \\) or \\( x = 1_{\mathbb{G}} \\) and the algorithms almost always have special cases
to handle these values, which is why they are not properly handled by the definitions above. Putting everything together, we
can provide an alternative definition for \\( \mathbb{G} \\) even though it ends up being a bit recursive:

$$ \mathbb{G} = \{ 0_{\mathbb{G}}, 1_{\mathbb{G}} \} \cup \mathbb{G}^- \cup \mathbb{G}^+ $$

That's pretty much it for the abuses of notation that we will use to describe operations on natural numbers encoded either
in two's complement or as Gray codes in this blog. As always, if you feel that something is missing, don't hesitate to
report it in the [issues][issues] section of the associated repository with the « blog » label.


  [issues]: https://github.com/Morwenn/cpp-gray/issues
  [so-reflected]: http://stackoverflow.com/a/34555899/1364752
