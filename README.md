cppcomm: header-only cpp communications
=========================================

Communicate with less effort in C++.

Background
------------

At the time of writing (2022-03-28) I have gotten tired of the greenless state of my github page, and I want to get addicted again.
So I decided to start a project to work on consistently, but hmm, what should I do?
A specific application was out of the question because I don't have any problems to solve in my life that I can't solve with existing software.
So how about a library? I might use it in my own projects, or someone might fork it and make it into something useful.

This project was chosen in particular because, had I written something like this prior to writing [Splite](https://github.com/mrryyi/Splite), I'd'n't've'd to spend approximately 50% of Splite development on pointer debugging.

https://github.com/mrryyi/Splite/blob/master/REPORT%202020-05-24.pdf

Discussion
----------

### What is cppcomm supposed to do?

cppcomm should at least help facillitate client-server topology.

"But ryyi, nobody will ever use your library! There are way better libs and cope and mald"

### design choices

I like snake_case.

I like writing C++ in C-style, but I dislike not having access to STL (fuck you C-strings)

I just wanna make my keyboard make clicky noises and produce aesthetically pleasing pixels on my screen that might or might not segfault.
