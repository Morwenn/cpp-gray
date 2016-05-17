---
layout: post
title:  "Introduction"
date:   2016-05-17 21:27:10 +0200
categories: help
---
Let's be honest: I suck at introductions. Have a [copy-paste of Wikipedia][wiki-gray-code] instead.

> The **reflected binary code** (RBC), also known as **Gray code** after Frank Gray, is a binary numeral system where
two successive values differ in only one bit (binary digit). The reflected binary code was originally designed to prevent
spurious output from electromechanical switches. Today, Gray codes are widely used to facilitate error correction in
digital communications such as digital terrestrial television and some cable TV systems.

Welcome to this blog! It is intended to be part of a two-part project about reflected binary code. The other part is
a [C++14 library][cpp-gray] providing a `gray_code` class template as well as some additional functions related to Gray
codes. This blog will contain small articles about reflected binary code and the algorithms used to work with it. Some of
the algorithms described here are actually used in the library, while some others were considered but didn't prove to be
fast enough to warrant an inclusion in the library. Anyway, describing, analyzing and improving new algorithms is
always worth it... hence this blog. Also, before going anywhere else, note that I'm not a mathematician, but I will
be using and abusing mathematical symbols, for they help to reason about things whenever one needs to be a little
formal.

That's it for the introduction. If you ever find mistakes, or things you feel are downright wrong in this blog, don't
hesitate to share your thoughts and improvements by opening an [issue][issues] with the « blog » label in the
corresponding [GitHub repository][cpp-gray].


[cpp-gray]: https://github.com/Morwenn/cpp-gray
[issues]: https://github.com/Morwenn/cpp-gray/issues
[wiki-gray-code]: https://en.wikipedia.org/wiki/Gray_code
