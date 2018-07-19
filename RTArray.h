#pragma once

#ifndef RTARRAY_H
#define RTARRAY_H

//Created by Clayton Breckel on 7.10.2018
//Header only implementation of a Fixed-Sized known at Runtime (RT)Array

//Until very recently, __cplusplus is 199711L on msvc compiler and _MSVC_LANG is the cpp version so check both
#define RTARRAY_CPPVERSION_ATLEAST(VERSION) ( __cplusplus >= VERSION || _MSVC_LANG >= VERSION)

//c++17
#if RTARRAY_CPPVERSION_ATLEAST(201703L) //201703L == c++17
#define RTARRAY_MUST_USE [[nodiscard]]
#else /* ^^^ CAN HAZ [[nodiscard]] ^^^ // vvv NO CAN HAZ [[nodiscard]] vvv */ // <- yyvals.h line 431 in MSVC stdlib
#define RTARRAY_MUST_USE
#endif

//c++11
#if RTARRAY_CPPVERSION_ATLEAST(201103L) //201103L == c++11
#define RTARRAY_MOVE(x) std::move(x)
#else
#define RTARRAY_MOVE(x) x
#endif

#include <sstream>
#include <stdexcept>
//ANY VERSION
#ifdef _DEBUG
#define RTARRAY_OOB_CHECK
#endif

#ifdef RTARRAY_ALLOC
#include <memory>
#define RTARRAY_DO_ALLOCATE(LEN) allocator.allocate(LEN)
#define RTARRAY_DO_CONSTRUCT(POINTER, ...) allocator.construct(POINTER, __VA_ARGS__)
#define RTARRAY_DO_DESTROY(POINTER) allocator.destroy(POINTER)
#define RTARRAY_DO_DEALLOCATE(POINTER, LEN) allocator.deallocate(POINTER, LEN)
#else
#define RTARRAY_DO_ALLOCATE(LEN) reinterpret_cast<T*>(new char[sizeof(T) * LEN])
#define RTARRAY_DO_CONSTRUCT(POINTER, ...) new (POINTER) T(__VA_ARGS__)
#define RTARRAY_DO_DESTROY(POINTER) (POINTER)->~T()
#define RTARRAY_DO_DEALLOCATE(POINTER, LEN) delete[] reinterpret_cast<char*>(POINTER)
#endif

#define RTARRAY_DO_OOB_CHECK(idx, length) \
if (idx >= length) {\
std::ostringstream errMsg;\
errMsg << "Attempted to access element at position " << idx << " in an array of size " << length << ".";\
throw std::out_of_range(errMsg.str());\
		}\

//T is the type that's to be used for the array
template <class T
#ifdef RTARRAY_ALLOC
	, class Alloc = std::allocator<T>
#endif
>
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

#ifdef RTARRAY_ALLOC
	typedef Alloc Alloc;
#endif
private:

#ifdef RTARRAY_ALLOC
	Alloc allocator;
#endif

	//ptr is const as there is one and ONLY one situation where it should be changed after creation and that's move.
	//EVERY other usage of this variable should NOT change the pointed to memory.
	pointer const ptr;

	//see ptr for why it's const.
	const size_type arrSize;

public:
//CONSTRUCTORS AND DESTRUCTOR

	///Construct a new RTArray with a given length, function pointer, allocator, and optional arguments.
	///This version supports all c++ versions, and doesn't have the overhead of std::function
	///ARGUMENTS
	///size_type length: The length of the array
	///Function function: The function pointer that returns a type T. It get's passed the current index of the array.
	///const Alloc& allocator: The allocator to be used. Defaults to creating a new allocator.
	template <typename Function>
	RTArray(size_type arrSize, Function function
	#ifdef RTARRAY_ALLOC
		, Alloc allocator = Alloc()) : allocator(RTARRAY_MOVE(allocator)),
	#else
		) :
	#endif
		ptr(RTARRAY_DO_ALLOCATE(arrSize)), arrSize(arrSize) {
		for (size_type i = 0; i < arrSize; ++i) {
			ptr[i] = RTARRAY_MOVE(function(i));
		}
	}

	///Constructs a new RTArray with a given length and object to copy into each index.
	///ARGUMENTS
	///size_type length: The length of the array
	///const_reference to_be_copied: The value to be copied into each index
	///const Alloc& allocator: The allocator to be used. Defaults to creating a new allocator.

	RTArray(size_type arrSize, const_reference to_be_copied
	#ifdef RTARRAY_ALLOC
		, Alloc allocator = Alloc()) : allocator(RTARRAY_MOVE(allocator)),
	#else
		) :
	#endif
		ptr(RTARRAY_DO_ALLOCATE(arrSize)), arrSize(arrSize) {
		for (size_type i = 0; i < arrSize; ++i) {
			RTARRAY_DO_CONSTRUCT(&ptr[i], to_be_copied);
		}
	}

	//As an RTArray copy is a deep copy, copying an RTArray can be VERY expensive,
	//therefore disable access to copy ctor by default.
#ifdef RTARRAY_COPY
	///Performs a deep copy of one RTArray to another.
	RTArray(const RTArray& other) :
	#ifdef RTARRAY_ALLOC
		allocator(other.allocator),
	#endif
		ptr(RTARRAY_DO_ALLOCATE(other.arrSize)), arrSize(other.arrSize) {
		for (size_type i = 0; i < arrSize; ++i) {
			RTARRAY_DO_CONSTRUCT(&ptr[i], other[i]);
		}
	}
#else
	RTArray(const RTArray& other) = delete;
#endif

	//Move ctor can only exist if move exists
#ifdef RTARRAY_MOVE
	///Move one RTArray into another.
	///The RTArray that got moved should NOT be used again. Attempting to use it could cause nullptr accesses.
	RTArray(RTArray&& other) :
	#ifdef RTARRAY_ALLOC
		allocator(RTARRAY_MOVE(other.allocator)),
	#endif 
		ptr(other.ptr), arrSize(other.arrSize) {
		const_cast<pointer>(other.ptr) = nullptr;
		static_cast<size_type>(other.arrSize) = 0; //Not really sure why const_cast doesn't work with integer literals...
	}
#endif
///Destroys all elements in the array, then deallocates the data used by the array.
	~RTArray() {
	//ptr can only be null if this was moved. This can only be moved if std::move exists.
	#ifdef RTARRAY_MOVE
		if (ptr == nullptr) return;
	#endif
		//Destroy each element in reverse order
		for (size_type i = arrSize - 1; i > 0; --i) {
			RTARRAY_DO_DESTROY(&ptr[i]);
		}
		//Finally, deallocated the array
		RTARRAY_DO_DEALLOCATE(ptr, arrSize);
	}

//END CONSTRUCTORS AND DESTUCTOR

//ELEMENT ACCESS

///Retrieve a reference to the item at position idx in the array. ALWAYS performs bounds checking on the access
	RTARRAY_MUST_USE inline reference at(size_t idx) {
		RTARRAY_DO_OOB_CHECK(idx, arrSize);
		return ptr[idx];
	}

	///Retrieve a const_reference to the item at position idx in the array. ALWAYS performs bounds checking on the access
	RTARRAY_MUST_USE inline const_reference at(size_t idx) const {
		RTARRAY_DO_OOB_CHECK(idx, arrSize);
		return ptr[idx];
	}

	///Retrieve a reference to the item at position idx in the array. 
	///Performs bounds checking on the access if compiled in debug mode or if RTARRAY_DO_OOB_CHECK is defined
	RTARRAY_MUST_USE inline reference operator[](size_t idx) {
	#ifdef RTARRAY_OOB_CHECK
		RTARRAY_DO_OOB_CHECK(idx, arrSize);
	#endif
		return ptr[idx];
	}

	///Retrieve a const_reference to the item at position idx in the array. 
	///Performs bounds checking on the access if compiled in debug mode or if RTARRAY_DO_OOB_CHECK is defined
	RTARRAY_MUST_USE inline const_reference operator[](size_t idx) const {
	#ifdef RTARRAY_OOB_CHECK
		RTARRAY_DO_OOB_CHECK(idx, arrSize);
	#endif
		return ptr[idx];
	}

	///Returns the underlying pointer used by RTArray
	RTARRAY_MUST_USE inline pointer data() {
		return ptr;
	}

	///Returns the underlying pointer used by RTArray
	RTARRAY_MUST_USE inline const_pointer data() const {
		return ptr;
	}

	///Pointer to the beginning of the array
	RTARRAY_MUST_USE inline reference front() {
		return &ptr[0];
	}

	///Const pointer to the beginning of the array
	RTARRAY_MUST_USE inline const_reference front() const {
		return &ptr[0];
	}

	///Pointer to the end of the array
	RTARRAY_MUST_USE inline reference back() {
		return &ptr[arrSize];
	}

	///Const pointer to the end of the array
	RTARRAY_MUST_USE inline const_reference back() const {
		return &ptr[arrSize];
	}
//END ELEMENT ACCESS

//CAPACITY
	///Whether the array is empty or not.
	///Equivalent to size() == 0;
	RTARRAY_MUST_USE inline bool empty() const {
		return size == 0;
	}

	///Retrieves the size of the array
	RTARRAY_MUST_USE inline size_type size() const {
		return arrSize;
	}

	///Retrieves the maximum size of the array.
	///Really just returns the size. 
	RTARRAY_MUST_USE inline size_type max_size() const {
		return arrSize;
	}
//END CAPACITY

};

//Undefine all macros used
#undef RTARRAY_MUST_USE
#undef RTARRAY_MOVE
#undef RTARRAY_CPPVERSION_ATLEAST
#undef RTARRAY_DO_ALLOCATE
#undef RTARRAY_DO_CONSTRUCT
#undef RTARRAY_DO_DESTROY
#undef RTARRAY_DO_DEALLOCATE
#undef RTARRAY_CPPVERSION_ATLEAST

#endif RTARRAY_H
