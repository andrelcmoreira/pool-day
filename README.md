## pool-day

[![CI](https://github.com/andrelcmoreira/pool-day/actions/workflows/ci.yaml/badge.svg)](https://github.com/andrelcmoreira/pool-day/actions/workflows/ci.yaml)
[![License: LGPL v3](https://img.shields.io/badge/License-LGPL_v3-blue.svg)](https://www.gnu.org/licenses/lgpl-3.0)

#### Overview

`pool-day` is a simple thread pool library for C/C++ applications built around POSIX threads.

#### Building

The library relies on `cmake` tool to be built:

```cmake
$ cmake -S . -B build
$ cmake --build build
$ sudo cmake --install build
```

Additional flags can be supplied as parameter to cmake according to the table
below:

|      Flag      |                 Description          |
|----------------|--------------------------------------|
|  BUILD_SAMPLES | Build the library's samples          |
| ENABLE_LOGGING | Enable the library's logging feature |

#### Support

- linux
