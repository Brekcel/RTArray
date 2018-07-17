# RTArray 
### C++ Header only implementation of a Fixed-Sized known at Runtime (RT)Array

## Usage
```c++
#include<RTArray.h>
...
RTArray<int> array(10, 5);
//or
RTArray<int> array(5, [](size_t idx) {
    return static_cast<int>(idx * idx);
});
```

## Features

Safe array handling to allow use of a runtime sized and allocated array.

Optional usage of std::allocator to allow for different allocation methods to be used.

If in debug mode or RTARRAY_OOB_CHECK is defined, [] operator is checked for out of bounds errors



### Automatic detection of c++11 and c++17 enabling specific features:

#### C++11:

Usage of std::move in certain spots.

Allows use of std::funcion instead of a plain function pointer.

#### C++17:

Adds [[nodiscard]] attribute to necessary functions.


## License

RTArray is licensed under the MIT License.