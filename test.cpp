//
// Created by Brekcel on 7.10.2018.
//
#define RTARRAY_ALLOC
#define RTARRAY_COPY
#include "RTArray.h"
#include <iostream>

struct Test {
	int x;
	Test(Test&& move) : x(move.x) {
		move.x = -1;
		std::cout << "Moving test x: " << x << std::endl;
	}
	Test(const Test& copy) :x(copy.x) {
		std::cout << "Copying test x: " << x << std::endl;
	}
	Test(int x) : x(x) {
		std::cout << "Creating test x: " << x << std::endl;
	}
	Test& operator =(const Test& eq) {
		this->x = eq.x;
		std::cout << "Assigning test x: " << x << std::endl;
		return *this;
	}
	~Test() {
		std::cout << "Destroying test x:" << x << std::endl;
	}
};

#include <array>
int main(int argc, char* argv[]) {
	//std::array
	//std::vector
	std::cout << "RTArraySize: " << sizeof(RTArray<Test>) << std::endl;
	std::array<Test, 6> arr4 = {Test(3), Test(3), Test(3), Test(3), Test(3), Test(3)};
	RTArray<Test> arrIter(arr4.begin(), arr4.end());
	int j = 0;
	RTArray<int> arr(5, [&j](size_t idx) {
		j += 2;
		return static_cast<int>(j * idx);
	});
	RTArray<int> arr2 = arr;
	RTArray<int> arr3 = std::move(arr);
	int i = arr3[3]; //i == 24
	std::cout << "arr[3]: " << i << std::endl;
	return 0;
}
