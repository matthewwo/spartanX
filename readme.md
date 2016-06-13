# SpartanX - asynchronous socket framework

SpartanX is an extremely easy to use asynchronous socket programming framework that allow you to program in native programming languages (C, C++, Obj-C, swift)

SpartanX is design for medium-small (<50k clients/host) enviornment, performance might vary depends on setup. In fact, it is design for writing [personal computer / mobile] applications that require server-side capability. Therefore, it has the ability to assign different qos for different connection in runtime, it can even run on GPU if you're using OS X. Unlike most of other socket framework, all the sx_runtime_object are passively inherit their status from their owner. Since ownership is transferable, the behaviour and lifetime of a connection can change dramatically just by changing a pointer. One of the usage is SOCKS proxy, which offen need to support different versions. Imagine instead of checking the tag of the connection, or confirm the version number every single time, we can now transfer the ownership of the SXQueue (which is the abstraction of a incoming connection) from a SOCKSv4 running SXServer object to another. Since all the sx_runtime_object are passively inferit handler from their owner, we can route different connections without any overhead.

SpartanX is build directly on top of UNIX socket, and use libdispatch as its threading library. Therefore, clang (with -fblocks enabled) and libdispatch is required in order to build SpartanX. 

pthread, std::function, function pointer support are deprecated to simplify model.




