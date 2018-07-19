# RTArray 
### C++ Header only implementation of a Fixed-Sized known at Runtime (RT)Array

## Example
```c++
#include<RTArray.h>

int j = 0;

RTArray<int> arr(5, [&j](size_t idx) {
	j += 2;
	return static_cast<int>(j * idx);
});

int i = arr[3]; //i == 24
```

## Features

Safe array handling to allow use of a runtime sized and allocated array.

Optional usage of std::allocator to allow for different allocation methods to be used.

If in debug mode or RTARRAY_OOB_CHECK is defined, [] operator is checked for out of bounds errors

Implements most of the std::containers functions. Only missing is Iterators.

### Automatic detection of c++11 and c++17 enabling specific features:

#### C++11:

Usage of std::move in certain spots.

#### C++17:

Adds [[nodiscard]] attribute to necessary functions.

## License

RTArray is licensed under the MIT License.