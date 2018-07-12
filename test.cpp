//
// Created by Brekcel on 7.10.2018.
//

#define RTARRAY_FUNCTIONAL
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


int main(int argc, char* argv[]) {
	//std::vector
	RTArray<Test> array(20, [](size_t idx) {
		return Test(static_cast<int>(idx));
	});
	RTArray<Test> arr(20, Test(5));
	__noop();
	return 0;
}
