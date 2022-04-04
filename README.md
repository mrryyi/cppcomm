cppcomm: header-only c++20 windows socket communication
=========================================

Communicate with less effort in C++20. You can find the newest version in `src/cppcomm.hpp`

Features
-----------

| Feature | Status | Tested |
|---------|--------|--------|
| UDP nonblocking | Implemented | ✅ |
| UDP blocking | TBI | n/a |
| TCP nonblocking | TBI | n/a |
| TCP blocking | TBI | n/a |
| Message Buffer Wrapping | Implemented | ✅ |
| Thread-safe | TBI | n/a |
| Memory-safe | TBI | n/a |
| Compiletime msg def | TBI | n/a |
| Minimal overhead | TBI | n/a |

Examples
--------

### UDP nonblocking

```
#include "cppcomm.hpp"
#define PORT_CLIENT 68421

void main() {

  const char * local_inet_address = "127.0.0.1";

  cppcomm::Communicator communicator;
  auto ret = communicator.init_nonblocking_udp(local_inet_address, PORT_CLIENT);
  if (ret != cppcomm::SUCCESS_INIT) {
    WSACleanup();
    return 0;
  }

  SOCKADDR_IN server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons( PORT_SERVER );
  server_address.sin_addr.S_un.S_addr = inet_addr(local_inet_address);

  cppcomm::Message msg;
  uint32 msg_size = cppcomm::client_msg_one_write(msg.buffer);
  auto ret = communicator.send_msg(msg, server_address);
  if (!ret) { /* ... */ }
  // Continues after sending without blocking
  // ...
}
```

Requirements
------------

- Windows
- Compileflags: `-static -static-libgcc -static-libstdc++ -std=c++20`
- LDflags: `-lwsock32 -Wall -Wextra`

Versioning
-----------

a.b.c.d

- a = Huge new version with breaking or game-changing changes.
- b = Major improvements to existing code (new functionality)
- c = Minor improvements (implementation optimizations, etc)
- d = Miscellaneous (Added tests, etc.)

Versions are added to legacy folder at every a, b or c version.

If tests were added at main.1.2.0.0, new version would be 1.2.0.1
However, legacy folders at 1.2.0.0 would not have those tests, and there would be no 1.2.0.1 legacy folder.
Tests added at 1.2.>0.>0 would get to legacy when 1.3.0.0 is released.

Background
------------

At the time of writing (2022-03-28) I have gotten tired of the greenless state of my github page, and I want to get addicted again.
So I decided to start a project to work on consistently, but hmm, what should I do?
A specific application was out of the question because I don't have any problems to solve in my life that I can't solve with existing software.
So how about a library? I might use it in my own projects, or someone might fork it and make it into something useful.

This project was chosen in particular because, had I written something like this prior to writing [Splite](https://github.com/mrryyi/Splite), I'd'n't've'd to spend approximately 50% of Splite development on pointer debugging.

Discussion
----------

### What is cppcomm supposed to do?

cppcomm should at least help facillitate client-server topology.

### Design choices

I like snake_case.

I like writing C++ in C-style, but I dislike not having access to STL.

I just wanna make my keyboard do clicky noises and produce aesthetically pleasing pixels on my screen that might or might not segfault.
