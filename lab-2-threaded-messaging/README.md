Single Threaded Messaging Server and Client
----------------------

A project for CS360

NOTE: This uses C++11.


## TAs

There are two places you'll want to look:

1. `thread_pool.h` - Thread pool using only posix semaphores
2. `db.h` - DB using `std::mutex` and `std::lockguard`


## Server Options:

```
Argument           Definition
---------------------------------------------------------
-p [port]          port number of the messaging server
-t [thread count]  thread pool size: default 10
-b [client buffer] client buffer size: default 10
-d                 print debugging information
```

## Client Options
```
Argument           Definition
---------------------------------------------------------
-s [server]        machine name of the messaging server (e.g. hiking.cs.byu.edu)
-p [port]          port number of the messaging server
-d                 print debugging information
```