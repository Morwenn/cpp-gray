---
layout: post
title: Gray Codes Addition — Harold Lucal's Addition
date: 2016-11-06 19:59:15 +0200
categories: [gray code, c++]
---
*Disclaimer: this article is far from a theorical one, it's more of an optimization story, mostly taken from [the
corresponding Q&A on Code Review][cr-gray-addition] that I wrote a while ago.*

Exit the trivial and simple operations: here comes the addition. Apparently adding two Gray codes without having to convert
the operands to two's complement integers, adding these integers and converting the result back to a Gray code is a problem
that hasn't been well-researched, probably due to the efficiency of the $$ to\_gray $$ and $$ from\_gray $$ operations,
which allow us to perform the addition in \\( O(\log{n}) \\) time on usual architectures.

In this article, we present and optimize an addition algorithm described by R. W. Doran in [*The Gray Code*][the-gray-code]
and attributed to Harold Lucal. Here is the original pseudocode, taken straight from the aforementioned paper:

{% highlight vhdl %}
procedure add (n: integer; A,B:word; PA,PB:bit;
               var S:word; var PS:bit; var CE, CF:bit);
var i: integer; E, F, T: bit;
begin
   E := PA; F := PB;
   for i:= 0 to n-1 do begin {in parallel, using previous inputs}
       S[i] := (E and F) ⊕ A[i] ⊕ B[i];
       E := (E and (not F)) ⊕ A[i];
       F := ((not E) and F) ⊕ B[i];
   end;
   CE := E; CF := F;
end;
{% endhighlight %}

This algorithm runs in $$ O(n) $$ time where $$n$$ in the number of bits in the Gray code. Here is a naive C++ translation
of the algorithm above, implemented using the [**cpp-gray**][cpp-gray] library:

{% highlight cpp %}
template<typename Unsigned>
auto operator+(gray_code<Unsigned> lhs, gray_code<Unsigned> rhs) noexcept
    -> gray_code<Unsigned>
{
    // parity of lhs and rhs
    bool lhs_p = is_odd(lhs);
    bool rhs_p = is_odd(rhs);

    gray_code<Unsigned> res{};
    for (Unsigned i{} ; i < std::numeric_limits<Unsigned>::digits ; ++i) {
        // Get the ith bit of rhs and lhs
        bool lhs_i = (lhs.value >> i) & 1u;
        bool rhs_i = (rhs.value >> i) & 1u;

        // Copy lhs_p and rhs_p (see {in parallel} in the original algorithm)
        bool lhs_p_cpy = lhs_p;
        bool rhs_p_cpy = rhs_p;

        // Set the ith bit of res
        Unsigned res_i = (lhs_p_cpy & rhs_p_cpy) ^ lhs_i ^ rhs_i;
        res |= (res_i << i);

        // Update e and f
        lhs_p = (lhs_p_cpy & (!rhs_p_cpy)) ^ lhs_i;
        rhs_p = ((!lhs_p_cpy) & rhs_p_cpy) ^ rhs_i;
    }
    return res;
}
{% endhighlight %}

The rest of this article is composed of subsections describing various incremental benchmark-driven optimizations to this
algorithm.

### Loop invariants code motion

This is the name of an optimization performed by most compilers: when they detect code that actually does not depend on the
state of the loop, they move it out of the loop. It's not as obvious as the usual invariants, but let's have a look at this
line:

{% highlight cpp %}
Unsigned res_i = (lhs_p_cpy & rhs_p_cpy) ^ lhs_i ^ rhs_i;
{% endhighlight %}

`lhs_i` and `rhs_i` correspond to the $$ i_{th} $$ bit of `lhs` and `rhs`. These bits do not depend on the loop (the loop
does not change them) and can all be computed at once by performing `lhs ^ rhs` before running the loop. Therefore, we can
simplify the function by moving their computation out of the loop. If we store them directly in `res`, we can even simplify
the assignment to `res` to a mere `res ^= res_i << i`:

{% highlight cpp %}
template<typename Unsigned>
auto operator+(gray_code<Unsigned> lhs, gray_code<Unsigned> rhs) noexcept
    -> gray_code<Unsigned>
{
    // parity of lhs and rhs
    bool lhs_p = is_odd(lhs);
    bool rhs_p = is_odd(rhs);

    gray_code<Unsigned> res = lhs ^ rhs;
    for (Unsigned i{} ; i < std::numeric_limits<Unsigned>::digits ; ++i) {
        // Get the ith bit of rhs and lhs
        bool lhs_i = (lhs.value >> i) & 1u;
        bool rhs_i = (rhs.value >> i) & 1u;

        // Copy lhs_p and rhs_p (see {in parallel} in the original algorithm)
        bool lhs_p_cpy = lhs_p;
        bool rhs_p_cpy = rhs_p;

        // Set the ith bit of res
        Unsigned res_i = lhs_p_cpy & rhs_p_cpy;
        res ^= res_i << i;

        // Update e and f
        lhs_p = (lhs_p_cpy & (!rhs_p_cpy)) ^ lhs_i;
        rhs_p = ((!lhs_p_cpy) & rhs_p_cpy) ^ rhs_i;
    }
    return res;
}
{% endhighlight %}

Unfortunately, `lhs_i` and `rhs_i` are also used to compute the new values of `lhs_p` and `rhs_p`, so we can't remove them
from the body of the loop. If we are to optimize the algorithm even more, we will need something else, for example...

### Getting rid of the temporaries

The part of the Gray addition which is likely to be be the slowest is the body of the `for` loop. To find a possible
optimization, I decided to check whether it was possible to get rid of `lhs_p_cpy` and `rhs_p_cpy`, which are copies of
`lhs_p` and `rhs_p` required to compute the new values of `lhs_p` and `rhs_p`. While it is easy to get rid of `rhs_p_cpy` by
replacing it by `rhs_p` directly, getting rid of `lhs_p_cpy` is harder because `lhs_p` has already been updated in the
meantime. Without any trickery, the following loop body is the best we can get:

{% highlight cpp %}
Unsigned res_i = lhs_p & rhs_p;
res ^= res_i << i;

bool lhs_p_cpy = lhs_p;
bool lhs_i = (lhs.value >> i) & 1u;
bool rhs_i = (rhs.value >> i) & 1u;
lhs_p = (lhs_p_cpy & not rhs_p) ^ lhs_i;
rhs_p = (rhs_p & not lhs_p_cpy) ^ rhs_i;
{% endhighlight %}

Therefore, I decided to build some truth tables. In the following table, $$ lhs_{old} $$ and $$ rhs_{old} $$ are possible
values of `lhs_p` and `rhs_p` *before* the update while $$ lhs_{new} $$ and $$ rhs_{new} $$ are the values of the same
variables *after* the update. I did not take any interest in `lhs_i` and `rhs_i` since none of them is used to compute both
$$ lhs_{new} $$ and $$ rhs_{new} $$; therefore, I just ignore them in the rest of this reflection.

$$\begin{array} {|cc|cc|}
\hline
lhs_{old} & rhs_{old} & lhs_{new} & rhs_{new} \\
\hline
0 & 0 & 0 & 0\\
0 & 1 & 0 & 1\\
1 & 0 & 1 & 0\\
1 & 1 & 0 & 0\\
\hline
\end{array}$$

We can easily replace `lhs_p_cpy` by $$ lhs_{old} $$ in the computation of $$ lhs_{new} $$ but we can't use it to compute
$$ rhs_{new} $$ since the update already occured. My first thought to get rid of `lhs_p_cpy` was « can we compute
$$ rhs_{new} $$ with only $$ rhs_{old} $$ and $$ lhs_{new} $$? ». Looking at the truth table above, it appears that it is
not possible. Trying to compute $$ rhs_{new} $$ before $$ lhs_{new} $$ doesn't solve the problem either.

However, we have already computed another value in the loop: `res_i`. Injecting $$ res_i $$ in the previous truth table
yields the following table:

$$\begin{array} {|cc|c|cc|}
\hline
lhs_{old} & rhs_{old} & res_i = lhs_{old} \land rhs_{old}  & lhs_{new} & rhs_{new} \\
\hline
0 & 0 & 0 & 0 & 0\\
0 & 1 & 0 & 0 & 1\\
1 & 0 & 0 & 1 & 0\\
1 & 1 & 1 & 0 & 0\\
\hline
\end{array}$$

We can infer from this « truth table » that $$ rhs_{old} \land \lnot lhs_{old} = rhs_{old} \land \lnot res_i $$. If we
inject this new discovery back in the code, we can use it to totally get rid of `lhs_p_cpy`:

{% highlight cpp %}
Unsigned res_i = lhs_p & rhs_p;
res ^= res_i << i;

bool lhs_i = (lhs.value >> i) & 1u;
bool rhs_i = (rhs.value >> i) & 1u;
lhs_p = (lhs_p & not rhs_p) ^ lhs_i;
rhs_p = (rhs_p & not res_i) ^ rhs_i;
{% endhighlight %}

By the way `res_i` can *also* be used to compute $$ lhs_{new} $$ so that the code looks more symmetrical (even though it
does not improve the performance):

{% highlight cpp %}
Unsigned res_i = lhs_p & rhs_p;
res ^= res_i << i;

bool lhs_i = (lhs.value >> i) & 1u;
bool rhs_i = (rhs.value >> i) & 1u;
lhs_p = (lhs_p & not res_i) ^ lhs_i;
rhs_p = (rhs_p & not res_i) ^ rhs_i;
{% endhighlight %}

With all optimizations turned on, this code is a little bit faster than the previous version, but it's still far from being
as efficient as the version which converts the Gray code operands to a two's complement integer, performs an integer
addition then converts the result back to a Gray code. Additional twisting will be needed to optimize this algorithm even
more.

### Find a better end condition

Currently, the algorithm loops over every bit in `res` before ending. It might be possible to end the loop sooner, so let's
have a look at a new huge truth table that we will use for the rest of the article. For the sake of brevity, I used the
names from the original pseudocode to name the columns. I hope that the parallel with the variable names in the code isn't
too hard to make ($$ A = lhs_i $$, $$ B = rhs_i $$, $$ E_{old} = lhs_p $$, $$ F_{old} = rhs_p $$,
$$ S = A \oplus B \oplus res_i $$, $$ E_{new} = lhs_p' $$, $$ F_{new} = rhs_p' $$)

$$\begin{array} {|cccc|cc|cc|}
\hline
A & B & E_{old} & F_{old} & A \oplus B & S & E_{new} & F_{new}\\
\hline
0 & 0 & 0 & 0 & 0 & 0 & 0 & 0\\
0 & 0 & 0 & 1 & 0 & 0 & 0 & 1\\
0 & 0 & 1 & 0 & 0 & 0 & 1 & 0\\
0 & 0 & 1 & 1 & 0 & 1 & 0 & 0\\
0 & 1 & 0 & 0 & 1 & 1 & 0 & 1\\
0 & 1 & 0 & 1 & 1 & 1 & 0 & 0\\
0 & 1 & 1 & 0 & 1 & 1 & 1 & 1\\
0 & 1 & 1 & 1 & 1 & 0 & 0 & 1\\
\hline
1 & 0 & 0 & 0 & 1 & 1 & 1 & 0\\
1 & 0 & 0 & 1 & 1 & 1 & 1 & 1\\
1 & 0 & 1 & 0 & 1 & 1 & 0 & 0\\
1 & 0 & 1 & 1 & 1 & 0 & 1 & 0\\
1 & 1 & 0 & 0 & 0 & 0 & 1 & 1\\
1 & 1 & 0 & 1 & 0 & 0 & 1 & 0\\
1 & 1 & 1 & 0 & 0 & 0 & 0 & 1\\
1 & 1 & 1 & 1 & 0 & 1 & 1 & 1\\
\hline
\end{array}$$

Looking at the very first line of that table, we can see that when $$ A = B = E_{old} = F_{old} = 0 $$, we have
$$ S = E_{new} = F_{new} = 0 $$, which means that since our result was filled with $$ A \oplus B $$ right from the start,
then we can stop the loop when the aforementioned condition is met (everything equals $$0$$) since the following bits will
always be $$0$$. An easy way to do so is to "consume" `lhs` and `rhs` in the code with right shifts so that they eventually
reach $$0b0$$ and to change the loop condition with bitwise ORs (here, only the `for` is exposed loop for the sake of
brevity):

{% highlight cpp %}
for (Unsigned i{} ;
     lhs | rhs | lhs_p | rhs_p ;
     ++i, lhs >>= 1u, rhs >>= 1u) {
    Unsigned res_i = lhs_p & rhs_p;
    res ^= res_i << i;

    bool lhs_i = lhs.value & 1u;
    bool rhs_i = rhs.value & 1u;
    lhs_p = (lhs_p & not res_i) ^ lhs_i;
    rhs_p = (rhs_p & not res_i) ^ rhs_i;
}
{% endhighlight %}

While it might reduce the running time of the algorithm for small values (a higher most significant set bit means a bigger
value in Gray code), `lhs | rhs | lhs_p | rhs_p` is still a bunch of operations by itself for a loop condition. Our goal is
to stop the loop iterating when all of these variables are $$0$$. One thing we can do is find before the loop which of `lhs`
and `rhs` has the highest set bit and loop until that one is consumed. Since both `lhs` and `rhs` are consumed at the same
speed, when the one with the highest set bit is consumed, the other one has already been consumed, so we only have to check
whether the biggest of the two is $$0$$. We can easily find which one has the highest bit by checking which one has the
biggest value when considering the underlying bits as a two's complement integer. Here is the algorithm once modified:

{% highlight cpp %}
template<typename Unsigned>
auto operator+(gray_code<Unsigned> lhs, gray_code<Unsigned> rhs) noexcept
    -> gray_code<Unsigned>
{
    // Make sure lhs.value has the highest set bit
    if (lhs.value < rhs.value) {
        swap(lhs, rhs);
    }

    // parity of lhs and rhs
    bool lhs_p = is_odd(lhs);
    bool rhs_p = is_odd(rhs);

    gray_code<Unsigned> res = lhs ^ rhs;
    for (Unsigned i{} ;
         lhs | lhs_p | rhs_p ;
         ++i, lhs >>= 1u, rhs >>= 1u) {
        Unsigned res_i = lhs_p & rhs_p;
        res ^= res_i << i;

        bool lhs_i = lhs.value & 1u;
        bool rhs_i = rhs.value & 1u;
        lhs_p = (lhs_p & not res_i) ^ lhs_i;
        rhs_p = (rhs_p & not res_i) ^ rhs_i;
    }
    return res;
}
{% endhighlight %}

### Have a better look at the truth table

From the big truth table we built, we can see that when $$ A = B = 0 $$, the only time when $$ S = 1 $$ is when
$$ E_{old} = F_{old} = 1 $$. However, when $$ A = B = 0 $$, we can also see that $$ E_{new} $$ and $$ F_{new} $$ can never
be $$1$$ at the same time. That means that once `lhs` and `rhs` are consumed, `res` can change only one more time and only
if `(lhs_p & rhs_p)` is `true`. Therefore, we can extract the `lhs_p | rhs_p` part from the loop condition and apply exactly
*one* `res` assignment after the loop (I used a `while` loop since the `for` loop was becoming a bit messy for the job):

{% highlight cpp %}
template<typename Unsigned>
auto operator+(gray_code<Unsigned> lhs, gray_code<Unsigned> rhs) noexcept
    -> gray_code<Unsigned>
{
    // Make sure lhs.value has the highest set bit
    if (lhs.value < rhs.value) {
        std::swap(lhs, rhs);
    }

    // parity of lhs and rhs
    bool lhs_p = is_odd(lhs);
    bool rhs_p = is_odd(rhs);

    gray_code<Unsigned> res = lhs ^ rhs;
    Unsigned i{};
    while (lhs) {
        Unsigned res_i = lhs_p & rhs_p;
        res ^= res_i << i;

        bool lhs_i = lhs.value & 1u;
        bool rhs_i = rhs.value & 1u;
        lhs_p = (lhs_p & not res_i) ^ lhs_i;
        rhs_p = (rhs_p & not res_i) ^ rhs_i;

        ++i;
        lhs >>= 1u;
        rhs >>= 1u;
    }
    // Last value in case lhs_p and rhs_p are 1
    res ^= (lhs_p & rhs_p) << i;
    return res;
}
{% endhighlight %}

### Sometimes, more is less

Currently, we use the fact that at some point both `lhs` and `rhs` will eventually become $$0$$ and that only one
modification can occur right after this point. However, we don't use the fact that at some point `rhs` is $$0$$ while `lhs`
might not yet be totally consumed. We can actually write two loops instead of one: one loop that runs until the smallest of
`lhs.value` and `rhs.value` is $$0$$, and another that runs until the other one reaches $$0$$. The second loop can be
simplified since we know that one `rhs` (the smallest value from the `std::swap`) is $$0$$ during the iteration:

{% highlight cpp %}
template<typename Unsigned>
auto operator+(gray_code<Unsigned> lhs, gray_code<Unsigned> rhs) noexcept
    -> gray_code<Unsigned>
{
    // Make sure lhs.value has the highest set bit
    if (lhs.value < rhs.value) {
        std::swap(lhs, rhs);
    }

    // parity of lhs and rhs
    bool lhs_p = is_odd(lhs);
    bool rhs_p = is_odd(rhs);

    gray_code<Unsigned> res = lhs ^ rhs;
    Unsigned i{};
    while (rhs) {
        Unsigned res_i = lhs_p & rhs_p;
        res ^= res_i << i;

        bool lhs_i = lhs.value & 1u;
        bool rhs_i = rhs.value & 1u;
        lhs_p = (lhs_p & not res_i) ^ lhs_i;
        rhs_p = (rhs_p & not res_i) ^ rhs_i;

        ++i;
        lhs >>= 1u;
        rhs >>= 1u;
    }

    // We know that rhs is 0 now, let's get rid of it
    while (lhs) {
        Unsigned res_i = lhs_p & rhs_p;
        res ^= res_i << i;

        bool lhs_i = lhs.value & 1u;
        lhs_p = (lhs_p & not res_i) ^ lhs_i;
        rhs_p = rhs_p & not res_i;

        ++i;
        lhs >>= 1u;
    }

    // Last value in case lhs_p and rhs_p are 1
    res ^= (lhs_p & rhs_p) << i;
    return res;
}
{% endhighlight %}

### Rethink the logic

The second loop can be rethought as "what happens when `res_i` is $$0$$ and what happens when it is $$1$$"? In terms of
code, with some simplifications applied, it yields the following loop:

{% highlight cpp %}
while (lhs) {
    Unsigned res_i = lhs_p & rhs_p;
    bool lhs_i = lhs.value & 1u;

    if (res_i) {
        res ^= res_i << i;
        lhs_p = lhs_i;
        rhs_p = false;
    } else {
        lhs_p ^= lhs_i;
    }

    ++i;
    lhs >>= 1u;
}
{% endhighlight %}

While this algorithm is actually generally slower than the previous one because of branching, it permits to realize some
interesting things: one of the branches always sets `rhs_p` to `false` and cannot be executed when `rhs_p == false`. The
other branch does not update `rhs_p` which means that the loop can be uderstood as « do stuff while `rhs_p is true` », so we
can rewrite the original second loop (not the new transformed second loop) as follows:

{% highlight cpp %}
while (rhs_p) {
    res ^= lhs_p << i;
    rhs_p = not lhs_p;
    lhs_p = lhs.value & 1u;

    ++i;
    lhs >>= 1u;
}
{% endhighlight %}

Many simplifications have been applied since we know during this loop that `rhs_p == true`. We end up with a second loop in
the algorithm almost free compared to the first one, which means that when we add a number with a big highest set bit to one
with a small highest set bit, it could perform significantly faster than the previous versions. Also note that since the
last loop ends when `rhs_p == false`, then `rhs_p & lhs_p` is always `false` in the post-last-loop operation, so we can get
rid of it altogether.

Now, we can observe that the loop updates `res` with `lhs_p` until `rhs_p` is `false`. However, `rhs_p` is set to
`not lhs_p` which means that `res` is updated with `lhs_p` while `lhs_p` is $$0$$ except for the last iteration. In other 
words, `res` is meaningfully updated only once since when we reach `lhs_p == 1`, we leave the loop right after the next
iteration. So we can tweak the loop to kick the `res` update out of it:

{% highlight cpp %}
if (rhs_p) {
    while (not lhs_p) {
        lhs_p = lhs.value & 1u;
        ++i;
        lhs >>= 1u;
    }
    res ^= lhs_p << i;
}
{% endhighlight %}

Combined one after the other, these optimizations leave us with the following algorithm:

{% highlight cpp %}
template<typename Unsigned>
auto operator+(gray_code<Unsigned> lhs, gray_code<Unsigned> rhs) noexcept
    -> gray_code<Unsigned>
{
    if (lhs.value < rhs.value) {
        std::swap(lhs, rhs);
    }

    bool lhs_p = is_odd(lhs);
    bool rhs_p = is_odd(rhs);

    gray_code<Unsigned> res = lhs ^ rhs;
    Unsigned i{};
    // Algorithm until the smallest number is zero
    while (rhs) {
        Unsigned res_i = lhs_p & rhs_p;
        res ^= res_i << i;

        bool lhs_i = lhs.value & 1u;
        bool rhs_i = rhs.value & 1u;
        lhs_p = (lhs_p & not res_i) ^ lhs_i;
        rhs_p = (rhs_p & not res_i) ^ rhs_i;

        ++i;
        lhs >>= 1u;
        rhs >>= 1u;
    }
    // Algorithm until the largest number is zero
    if (rhs_p) {
        while (not lhs_p) {
            lhs_p = lhs.value & 1u;
            ++i;
            lhs >>= 1u;
        }
        res ^= lhs_p << i;
    }
    return res;
}
{% endhighlight %}

While this is the most versatile and optimized version of the algorithm since the beginning of this answer, it is still an
order of magnitude slower than converting the Gray code operands to two's complement integers, performing a regular addition
and converting the result back to a Gray code. Which isn't surprising either since we're comparing a $$ O(n) $$ algorithm to
a $$ O(\log{n}) $$ one in the end.

It's also worth noting that this algorithm does not provide a circular behaviour on overflow.


  [cpp-gray]: https://github.com/Morwenn/cpp-gray
  [cr-gray-addition]: http://codereview.stackexchange.com/q/69122/15094
  [the-gray-code]: https://www.cs.auckland.ac.nz/research/groups/CDMTCS//researchreports/304bob.pdf
