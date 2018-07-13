#pragma once

#ifndef RTARRAY_H
#define RTARRAY_H

//Created by Clayton Breckel on 7.10.2018
//Header only implementation of a Fixed-Sized known at Runtime (RT)Array

#include <memory>

//Until very recently, __cplusplus is 199711L on msvc compiler and _MSVC_LANG is the cpp version so check both
#define RTARRAY_CPPVERSION_ATLEAST(VERSION) ( __cplusplus >= VERSION || _MSVC_LANG >= VERSION)

//c++17
#if RTARRAY_CPPVERSION_ATLEAST(201703L) //201703L == c++17
#define RTARRAY_MUST_USE [[nodiscard]]
#else
#define RTARRAY_MUST_USE
#endif

//c++11
#if RTARRAY_CPPVERSION_ATLEAST(201103L) //201103L == c++11

#define RTARRAY_MOVE(x) std::move(x)

#ifndef RTARRAY_NO_FUNCTIONAL
#include <functional>
#define RTARRAY_FUNCTIONAL
#endif

#else

#ifdef RTARRAY_FUNCTIONAL
#error To use functional constructors, you must use c++11 or greater.
#endif

#define RTARRAY_MOVE(x) x

#endif

//T is the type that's to be used for the array
template <class T, class Alloc = std::allocator<T>>
class RTArray {
public:
	//Standard std::container typedefs
	typedef T value_type;
	typedef T* pointer;
	typedef T& reference;
	typedef const T* const_pointer;
	typedef const T& const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

private:

	Alloc allocator;
	pointer data;
	size_type length;

public:
#pragma region CONSTRUCTORS AND DESTRUCTOR
//CONSTRUCTORS AND DESTRUCTOR

#ifdef RTARRAY_FUNCTIONAL
	///Construct a new RTArray with a given length, function pointer, allocator, and optional arguments.
	///NOTES
	///If you do not have an allocator or wish to use the default allocator, use the other ctor
	///ARGUMENTS
	///size_type length: The length of the array
	///std::function<T(size_type)> function: The function pointer that returns a type T. It get's passed the current index of the array.
	///const Alloc& allocator: The allocator to be used. Defaults to creating a new allocator.
	template <class... Args>
	RTArray(size_type length, std::function<T(size_type)> function, Alloc allocator = Alloc()) :
		allocator(std::move(allocator)), data(allocator.allocate(length)), length(length) {
		for (size_type i = 0; i < length; ++i) {
			data[i] = RTARRAY_MOVE(function(i));
		}
	}
#endif

	///Constructs a new RTArray with a given length and object to copy into each index.
	///ARGUMENTS
	///size_type length: The length of the array
	///const_reference to_be_copied: The value to be copied into each index
	///const Alloc& allocator: The allocator to be used. Defaults to creating a new allocator.
	RTArray(size_type length, const_reference to_be_copied, Alloc allocator = Alloc()) :
		allocator(std::move(allocator)), data(allocator.allocate(length)), length(length) {
		for (size_type i = 0; i < length; ++i) {
			allocator.construct(&data[i], to_be_copied);
		}
	}
/*
	///Construct a new RTArray with a given length, allocator, and optional arguments.
	///NOTES
	///If you do not have an allocator or wish to use the default allocator, use the other ctor. Due to the nature of variadic template arguments,
	///different ctor's must be created for wether or not we have an allocator.
	///ARGUMENTS
	///size_type length: The length of the array
	///const Alloc& allocator: The allocator to be used
	///Args&&... args: All the arguments to be forwarded to the objects constructor
	template <class... Args>
	RTArray(size_type length, const Alloc& allocator, Args&&... args) :
		allocator(allocator), data(allocator.allocate(length)), length(length) {
		for (size_type i = 0; i < length; ++i) {
			allocator.construct(&data[i], std::forward<Args>(args)...);
		}
	}

	///Construct a new RTArray with a given length, and optional arguments. This ctor creates a new allocator to be used.
	///ARGUMENTS
	///size_type length: The length of the array
	///Args&&... args: All the arguments to be forwarded to the objects constructor
	template <class... Args>
	RTArray(size_type length, Args&&... args) :
		allocator(Alloc()), data(allocator.allocate(length)), length(length) {
		for (size_type i = 0; i < length; ++i) {
			allocator.construct(&data[i], std::forward<Args>(args)...);
		}
	}
*/
	///Destroys all elements in the array, then deallocates the data used by the array.
	~RTArray() {
		//Destroy each element in reverse order
		for (size_type i = length - 1; i > 0; --i) {
			allocator.destroy(&data[i]);
		}
		//Finally, deallocated the array
		allocator.deallocate(data, length);
	}

//END CONSTRUCTORS AND DESTUCTOR
#pragma endregion
	///Pointer to the beginning of the array
	RTARRAY_MUST_USE inline pointer begin() {
		return &data[0];
	}

	///Pointer to the end of the array
	RTARRAY_MUST_USE inline pointer end() {
		return &data[length];
	}

	///Const pointer to the beginning of the array
	RTARRAY_MUST_USE inline const_pointer begin() const {
		return &data[0];
	}

	///Const pointer to the end of the array
	RTARRAY_MUST_USE inline const_pointer end() const {
		return &data[length];
	}

	RTARRAY_MUST_USE inline reference operator[](size_t idx) {
		return data[idx];
	}

	RTARRAY_MUST_USE inline const_reference operator[](size_t idx) const {
		return data[idx];
	}

};

//Undefine all macros used
#undef RTARRAY_MUST_USE
#undef RTARRAY_MOVE
#undef RTARRAY_FUNCTIONAL
#undef RTARRAY_CPPVERSION_ATLEAST

#endif //RTARRAY_H
