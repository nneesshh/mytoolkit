/** 
 @file
 Based on Chipmunk cpArray.
 cpArray is a faster alternative to CCMutableArray, it does pretty much the
 same thing (stores array_obj_ts and retains/releases them appropriately). It's
 faster because:
 - it uses a plain C interface so it doesn't incur Objective-c messaging overhead 
 - it assumes you know what you're doing, so it doesn't spend time on safety checks
   (index out of bounds, required capacity etc.)
 - comparisons are done using pointer equality instead of isEqual
 */

#ifndef cparray_H
#define cparray_H

#ifdef __cplusplus 
extern "C" { 
#endif

#include <stdlib.h>
#include <string.h>

#include "../platform/types.h"

/** for each **/
#define CPARRAY_OBJECT_FOREACH(__array__, __object__, __index__)																							\
	__object__=&((__array__)->arr[0]); for(__index__=0; (unsigned int)__index__<(__array__)->num; ++__index__, __object__=&((__array__)->arr[__index__]))	\

/** **/
typedef struct _array_obj_t {
	void	*data;
} array_obj_t;

/** **/
typedef struct _cparray_t {
	unsigned int	num, max;
	array_obj_t		*arr; //equals array_obj_t** arr;
} cparray_t;

/** Initializes an exist array with specified capacity */
INLINE void cparray_init(cparray_t *cparray, unsigned int capacity) {
	unsigned int i;

	if (capacity == 0) {
		capacity = 1; 
	}

	cparray->num = 0;
	cparray->max = capacity;
	cparray->arr =  (array_obj_t*)malloc( capacity*sizeof(array_obj_t) );

	for (i=0; i<capacity; ++i) {
		cparray->arr[0].data = NULL;
	}
	return;
}

/** Allocates and initializes a new array with specified capacity */
INLINE cparray_t* cparray_alloc(unsigned int capacity) {
	cparray_t *cparray = (cparray_t*)malloc( sizeof(cparray_t) );
	cparray_init(cparray, capacity);
	return cparray;
}

/** Clear array. */
INLINE void cparray_clear(cparray_t *cparray) {
	cparray->num = 0;
	cparray->max = 0;
	free(cparray->arr);
	cparray->arr=NULL;
}

INLINE void cparray_remove_and_release_all_objects(cparray_t *cparray);

/** Clear array. */
INLINE void cparray_clear_with_release(cparray_t *cparray) {
	cparray_remove_and_release_all_objects(cparray);
	cparray->num = 0;
	cparray->max = 0;
	free(cparray->arr);
	cparray->arr=NULL;
}

/** Frees array after removing all remaining objects. Silently ignores nil arr. */
INLINE void cparray_free(cparray_t *cparray) {
	if( cparray == NULL ) { return; }

	cparray_clear(cparray);
	free(cparray);
}

/** Doubles array capacity */
INLINE void cparray_double_capacity(cparray_t *cparray) {
	array_obj_t* newArr;
	cparray->max *= 2;
    newArr = (array_obj_t*)realloc( cparray->arr, cparray->max*sizeof(array_obj_t) );
    // will fail when there's not enough memory
    //CCAssert(newArr != NULL, "cpArrayDoubleCapacity failed. Not enough memory");
    cparray->arr = newArr;
}

/** Increases array capacity such that max >= num + extra. */
INLINE void cparray_ensure_extra_capacity(cparray_t *cparray, unsigned int extra) {
	while (cparray->max < cparray->num + extra) {
		cparray_double_capacity(cparray); 
	}
}

/** shrinks the array so the memory footprint corresponds with the number of items */
INLINE void cparray_shrink(cparray_t *cparray) {
    unsigned int newSize;

    //only resize when necessary
    if (cparray->max > cparray->num && !(cparray->num==0 && cparray->max==1)) {
        if (cparray->num!=0) {
            newSize=cparray->num;
            cparray->max=cparray->num; 
        } else {//minimum capacity of 1, with 0 elements the array would be free'd by realloc
            newSize=1;
            cparray->max=1;
        }

        cparray->arr = (array_obj_t*) realloc(cparray->arr, newSize*sizeof(array_obj_t) );
        //CCAssert(arr->arr != NULL, "could not reallocate the memory");
    }
} 

/** Returns index of first occurrence of object, UXNotFound if object not found. */
INLINE unsigned int cparray_get_index(cparray_t *cparray, void *dataptr) {
	unsigned int i;
	for (i = 0; i < cparray->num; ++i) {
		if (cparray->arr[i].data == dataptr) {
			return i;
		}
	}
	return UINT_MAX; 
}

/** Appends an object. Bahavior undefined if array doesn't have enough capacity. */
INLINE unsigned int cparray_append_object(cparray_t *cparray, void *data) {
	cparray->arr[cparray->num].data = data;
	return cparray->num++;
}

/** Appends an object. Capacity of arr is increased if needed. */
INLINE void cparray_append_object_with_resize(cparray_t *cparray, void *data) {
	cparray_ensure_extra_capacity(cparray, 1);
	cparray_append_object(cparray, data);
}

/** Appends objects from plusArr to arr. Behavior undefined if arr doesn't have
 enough capacity. */
INLINE void cparray_append_array(cparray_t *cparray, cparray_t *plusArr) {
	unsigned int i;
	for(i = 0; i < plusArr->num; ++i) {
		cparray_append_object(cparray, plusArr->arr[i].data); 
	}
}

/** Appends objects from plusArr to arr. Capacity of arr is increased if needed. */
INLINE void cparray_append_array_with_resize(cparray_t *cparray, cparray_t *plusArr) {
	cparray_ensure_extra_capacity(cparray, plusArr->num); 
	cparray_append_array(cparray, plusArr);
}

/** Inserts an object at index */
INLINE void cparray_insert_object_at_index(cparray_t *cparray, void *data, unsigned int index) {
	unsigned int remaining;
    //CCAssert(index<=arr->num, "Invalid index. Out of bounds");

    cparray_ensure_extra_capacity(cparray, 1);

    remaining = cparray->num - index;
    if( remaining > 0)
        memmove(&cparray->arr[index+1], &cparray->arr[index], sizeof(array_obj_t*) * remaining );

	cparray->arr[index].data = data;
    cparray->num++;
}

/** Swaps two objects */
INLINE void cparray_swap_objects_at_indexes(cparray_t *cparray, unsigned int index1, unsigned int index2) {
	array_obj_t tmp;
    //CCAssert(index1 < arr->num, "(1) Invalid index. Out of bounds");
    //CCAssert(index2 < arr->num, "(2) Invalid index. Out of bounds");

    tmp = cparray->arr[index1];

    cparray->arr[index1] = cparray->arr[index2];
    cparray->arr[index2] = tmp;
}

/** Removes all objects from arr */
INLINE void cparray_remove_all_objects(cparray_t *cparray) {
	cparray->num = 0;
}

/** Removes object at specified index and pushes back all subsequent objects.
 Behavior undefined if index outside [0, num-1]. */
INLINE void cparray_remove_object_at_index(cparray_t *cparray, unsigned int index) {
	unsigned int remaining;
	cparray->num--;

	remaining = cparray->num - index;
	if (remaining > 0) {
		memmove(&cparray->arr[index], &cparray->arr[index+1], remaining * sizeof(void*));
	}
}

/** Removes object at specified index and fills the gap with the last object,
 thereby avoiding the need to push back subsequent objects.
 Behavior undefined if index outside [0, num-1]. */
INLINE void cparray_fast_remove_object_at_index(cparray_t *cparray, unsigned int index) {
	unsigned int last = --cparray->num;
	cparray->arr[index] = cparray->arr[last];
}

/** */
INLINE void cparray_fast_remove_object(cparray_t *cparray, void *dataptr) {
	unsigned int index = cparray_get_index(cparray, dataptr);
	if (UINT_MAX != index)
		cparray_fast_remove_object_at_index(cparray, index);
}

/** Searches for the first occurance of object and removes it. If object is not
 found the function has no effect. */
INLINE void cparray_remove_object(cparray_t *cparray, void *dataptr) {
	unsigned int index = cparray_get_index(cparray, dataptr);

	if (UINT_MAX != index)
	{
		cparray_remove_object_at_index(cparray, index);
	}
}

/** Removes from arr all objects in minusArr. For each object in minusArr, the
 first matching instance in arr will be removed. */
INLINE void cparray_remove_array(cparray_t *cparray, cparray_t *minusArr) {
	unsigned int i;
	for(i = 0; i < minusArr->num; ++i)
	{
		cparray_remove_object(cparray, minusArr->arr[i].data);
	}
}

/** Removes all objects from arr */
INLINE void cparray_remove_and_release_all_objects(cparray_t *cparray) {
	while(cparray->num > 0) {
		free(cparray->arr[--cparray->num].data);
	}
}

/** Removes object at specified index and pushes back all subsequent objects.
 Behavior undefined if index outside [0, num-1]. */
INLINE void cparray_remove_and_release_object_at_index(cparray_t *cparray, unsigned int index) {
	free(cparray->arr[index].data);
	cparray_remove_object_at_index(cparray, index);
}

/** Removes object at specified index and fills the gap with the last object,
 thereby avoiding the need to push back subsequent objects.
 Behavior undefined if index outside [0, num-1]. */
INLINE void cparray_fast_remove_and_release_object_at_index(cparray_t *cparray, unsigned int index) {
	free(cparray->arr[index].data);
	cparray_fast_remove_object_at_index(cparray, index);
}

/** */
INLINE void cparray_fast_remove_and_release_object(cparray_t *cparray, void *dataptr) {
    unsigned int index = cparray_get_index(cparray, dataptr);
    if (UINT_MAX != index)
        cparray_fast_remove_and_release_object_at_index(cparray, index);
}

/** Searches for the first occurance of object and removes it. If object is not
 found the function has no effect. */
INLINE void cparray_remove_and_release_object(cparray_t *cparray, void *dataptr) {
	unsigned int index = cparray_get_index(cparray, dataptr);
	if (UINT_MAX != index)
	{
		cparray_remove_and_release_object_at_index(cparray, index);
	}
}

/** Removes from arr all objects in minusArr. For each object in minusArr, the
 first matching instance in arr will be removed. */
INLINE void cparray_remove_and_release_array(cparray_t *cparray, cparray_t *minusArr) {
	unsigned int i;
	for(i = 0; i < minusArr->num; ++i) {
		cparray_remove_and_release_object(cparray, minusArr->arr[i].data);
	}
}

/** Removes from arr all objects in minusArr. For each object in minusArr, all
 matching instances in arr will be removed. */
INLINE void cparray_full_remove_array(cparray_t *cparray, cparray_t *minusArr) {
	unsigned int back = 0;
	unsigned int i;
	for(i = 0; i < cparray->num; ++i) {
		if( UINT_MAX != cparray_get_index(minusArr, cparray->arr[i].data) ) {
			/** contains object */
			back++;
		} else {
			cparray->arr[i - back] = cparray->arr[i];
		}
	}
	
	cparray->num -= back;
}

#ifdef __cplusplus 
} 
#endif

#endif // cparray_H