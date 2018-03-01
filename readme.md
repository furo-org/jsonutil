# jsonutil: Utilities to write serialization functions for nlohmann::json

Helper macros to write ```to_json(), from_json()``` for [nlohmann::json](https://github.com/nlohmann/json), similar to [MSGPACK_DEFINE](https://github.com/msgpack/msgpack-c/wiki/v2_0_cpp_adaptor) in [msgpack-c](https://github.com/msgpack/msgpack-c).

## Quick Start

First of all, copy ```jsonutil.hh``` somewhere the compiler finds. ```jsonutil.hh``` needs to be included after ```nlohmann/json.hpp``` is included.

List member variables with ```JSON_MEMBER()``` macro.

```c++
#include <nlohmann/json.hpp>
#include "jsonutil.hh"
struct data{
   int x,y,z;
   JSON_MEMBER(x,y,z);
}
```

Then the struct ```data``` is ready to convert to nlohmann::json.

```c++
 data d={1,2,3};
 nlohmann::json j=d;
 std::cout<<j.dump(4)<<std::endl;
```

## Map or Array

Structs marked with ```JSON_MEMBER()``` is serialized to json object by default.
They can be serialized to json array to a dedicated json type ```yos::array_json```.

```c++
 data d={1,2,3};
 yos::array_json j=d;
 std::cout<<j.dump(4)<<std::endl;
```

Or, a struct can be marked with ```JSON_MEMBER_ARRAY()``` to forbid
serialization to json object.

```c++
#include <nlohmann/json.hpp>
#include "jsonutil.hh"
struct data_arr{
    int x,y,z;
    JSON_MEMBER_ARRAY(x,y,z);
}
int main(){
  data_arr d={1,2,3};
  nlohmann::json j=d;
}

```

## Tested compilers

* gcc 5.4
* clang 3.8
