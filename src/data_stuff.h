// Copyright (C) 2024 Benjamin Froelich
// This file is part of https://github.com/bbeni/earth_bender_game
// For conditions of distribution and use, see copyright notice in project root.

// Manual allocation Management
// Dynamic arrays

/* Dynamic array macros:
	array_add
	array_unordered_remove
	array_free

dyn_array has to be smth like:

struct Example_Array {
	size_t capacity;
	size_t count;
	struct T {
		int   x;
		float y;
	};
	T*     data;     // any pointer!
}

Example_Array array = { 0 };

T t = T{ 1, 2.0f };
array_add(&array, t);
array_add(&array, t);

*/


#pragma once

#include <cassert>
#include <cstdlib>


#define DYN_ARRAY_REALLOC realloc       // standard realloc for now
#define DYN_ARRAY_FREE    free          // standard free for now
#define DYN_ARRAY_INITIAL_CAPACITY 128 

// append an item and reallocate array after it doubled in size
#define array_add(dyn_array, item) \
	do { \
		if ((dyn_array)->count >= (dyn_array)->capacity) { \
			if ((dyn_array)->capacity == 0) { \
				(dyn_array)->capacity = DYN_ARRAY_INITIAL_CAPACITY; \
			} else { \
				(dyn_array)->capacity *= 2; \
			} \
			(dyn_array)->data = static_cast<decltype((dyn_array)->data)>(DYN_ARRAY_REALLOC((dyn_array)->data, (dyn_array)->capacity*sizeof(*(dyn_array)->data) )); \
			assert((dyn_array)->data != NULL); \
		} \
		(dyn_array)->data[(dyn_array)->count++] = (item); \
	} while (0)

// #define array_init(dyn_array, cap)

// remove the item at index (put the last item there -> shrink size by 1, so order is not perserved!)
#define array_unordered_remove(dyn_array, index) \
	do { \
		assert((index) >= 0); \
		assert((index) < (dyn_array)->count); \
		(dyn_array)->data[index] = (dyn_array)->data[(dyn_array)->count-1]; \
		(dyn_array)->count -= 1; \
	} while(0)

// free the allocated memory of array
#define array_free(dyn_array) \
	do { \
		DYN_ARRAY_FREE((dyn_array)->data); \
		(dyn_array)->data = NULL; \
		(dyn_array)->count = 0; \
		(dyn_array)->capacity = 0; \
	} while (0)\

// String dynamic array example
typedef struct {
	char* data;
	size_t count;
	size_t capacity;
} String_Builder;

