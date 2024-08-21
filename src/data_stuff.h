// Manual allocation Management
// Dynamic arrays

#pragma once

#include <cassert>
#include <cstdlib>

/* dyn_array has

size_t capacity;
size_t count;
void*  data;     // any pointer for that matter!

*/

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

// free the array
#define array_free(dyn_array) \
	do { \
		DYN_ARRAY_FREE((dyn_array)->data); \
		(dyn_array)->data = NULL; \
		(dyn_array)->count = 0; \
		(dyn_array)->capacity = 0; \
	} while (0)\

// String dynamic array
typedef struct {
	char* data;
	size_t count;
	size_t capacity;
} String_Builder;

