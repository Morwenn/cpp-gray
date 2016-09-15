---
layout: post
title: "Introduction"
date: 2016-05-17 21:27:10 +0200
categories: help
---
Let's be honest: I suck at introductions. Have a [copy-paste of Wikipedia][wiki-gray-code] instead.

> The **reflected binary code** (RBC), also known as **Gray code** after Frank Gray, is a binary numeral system where
two successive values differ in only one bit (binary digit). The reflected binary code was originally designed to prevent
spurious output from electromechanical switches. Today, Gray codes are widely used to facilitate error correction in
digital communications such as digital terrestrial television and some cable TV systems.

Welcome to this blog!

It is intended to be part of a two-part project about reflected binary code. The other part is a [C++14 library][cpp-gray]
providing a `gray_code` class template as well as some additional functions related to Gray codes. This blog will contain
small articles about reflected binary code and the algorithms used to work with it. Some of the algorithms described here
are actually used in the library, while some others were considered but didn't prove to be fast enough to warrant an
inclusion in the library. Anyway, describing, analyzing and improving new algorithms is always worth it... hence this blog.
Also, before going anywhere else, note that I'm not a mathematician, but I will be using and abusing mathematical symbols,
for they help to reason about things whenever one needs to be a little formal.

While there are many kinds of interesting Gray codes satisfying different properties, the articles will only consider the
reflected binary code whenever the term « Gray code » is employed. There's already enough to say about this single code,
and the other ones have generally been less studied.

Here is the full list of available articles:

1. Introductory articles
  * [Basic concepts and notation](https://morwenn.github.io/cpp-gray/the-reflected-binary-code/)
  * [C++ library overview](https://morwenn.github.io/cpp-gray/the-cpp-gray-c++-library/)
2. Basic operations
  * [Converting to and from Gray code](https://morwenn.github.io/cpp-gray/converting-to-and-from-gray-code/)
  * [Gray codes and parity](https://morwenn.github.io/cpp-gray/gray-codes-and-parity/)
  * [Powers of 2 and Gray codes](https://morwenn.github.io/cpp-gray/powers-of-2-and-gray-codes/)
3. Addition algorithms
  * None for now, come back later :)
4. TODO: Algorithms complexity cheat sheet

That's it for the introduction. If you ever find mistakes, or things you feel are downright wrong in this blog, don't
hesitate to share your thoughts and improvements by opening an [issue][issues] with the « blog » label in the
corresponding [GitHub repository][cpp-gray]. For smaller things or article-specific things, you can use the comments
instead.


[cpp-gray]: https://github.com/Morwenn/cpp-gray
[issues]: https://github.com/Morwenn/cpp-gray/issues
[wiki-gray-code]: https://en.wikipedia.org/wiki/Gray_code
