#ifndef cArray_h
#define cArray_h

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__GNUC__) || defined(__clang__)
    #define LIKELY(x) __builtin_expect(!!(x), 1)
    #define UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
    #define LIKELY(x) (x)
    #define UNLIKELY(x) (x)
#endif

#if defined(_MSC_VER)
    #define FORCE_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
    #define FORCE_INLINE inline __attribute__((always_inline))
#else
    #define FORCE_INLINE inline
#endif

#if defined(__cplusplus)
    #if defined(_MSC_VER)
        #define restrict __restrict
    #elif defined(__GNUC__) || defined(__clang__)
        #define restrict __restrict__
    #else
        #define restrict
    #endif
#endif

#define carray_length(obj) carray_size(obj)

/**
 * @brief A dynamic heap-allocated array object.
 *
 * @param data  Pointer to the first element in the array.
 * @param len   Number of elements in the array.
 * @param cap   Allocated buffer capacity in elements, always greater or equal to len.
 * @param size  Size of a single element, in bytes.
 *
 * @note Should only be created via carray_init() and freed via carray_free().
 * @note Direct modification of struct members is discouraged.
 * @note Unlike cString, cArray buffers are not null/sentinel terminated.
 */

typedef struct cArray {
    void *data;
    size_t len;
    size_t cap; // Oguri Cap
    size_t size;
} cArray;

/**
 * @brief cArray constructor.
 *
 * @param ptr  Initial array pointer to copy from. May be NULL if len is 0.
 * @param size Size of a single element, in bytes. Must not be 0.
 * @param len  Initial element count.
 *
 * @note Returns NULL on invalid input (size == 0, or ptr == NULL while len > 0).
 * @note The intended way for the object to be created.
 *
 * @example
 *   cArray *arr = carray_init((int[]){1, 2, 3, 4, 5}, sizeof(int), 5);
 *   // cArray object created!!!
 *   // ((int*)(arr->data))[0] == 1
 *   // ((int*)(arr->data))[2] == 3
 *   // arr->size == sizeof(int)
 *   // arr->len == 5
 *   // arr->cap == 16
 */

cArray *carray_init(const void *ptr, size_t size, size_t len);

/**
 * @brief cArray deep copy.
 *
 * @param dst  Destination cArray to copy to. Must not be NULL.
 * @param src  Target cArray to copy from. Must not be NULL.
 *
 * @note Silently returns on NULL input or mismatched element size.
 *
 * @example
 *   cArray *a1 = carray_init((int[]){1,2,3}, sizeof(int), 3);
 *   cArray *a2 = carray_init((int[]){0}, sizeof(int), 0);
 *   carray_copy(a2, a1);
 *   // a1 == a2
 */

void carray_copy(cArray *restrict dst, const cArray *restrict src);

/**
 * @brief cArray deep clone.
 *
 * @param src  Target cArray to clone from. Must not be NULL.
 *
 * @note Returns NULL on NULL input.
 *
 * @example
 *   cArray *a1 = carray_init((int[]){1,2,3}, sizeof(int), 3);
 *   cArray *a2 = carray_clone(a1);
 *   // a1 == a2
 */

cArray *carray_clone(const cArray *src);

/**
 * @brief Memory deallocator for cArray object.
 *
 * @param obj  The cArray. Must not be NULL.
 *
 * @note Silently returns on NULL input.
 * @note Always call this when done with a cArray to avoid memory leaks.
 *
 * @example
 *   cArray *arr = carray_init((int[]){1,2,3,4,5}, sizeof(int), 5);
 *   carray_free(arr);
 *   // Memory deallocated!!!
 */

void carray_free(cArray *obj);

/**
 * @brief Getter for cArray->len.
 *
 * @param obj  The cArray. Must not be NULL.
 *
 * @note Returns 0 on NULL input.
 * @note Identical to carray_length(). Both exist for convenience.
 *
 * @example
 *   cArray *arr = carray_init((int[]){1,2,3,4,5}, sizeof(int), 5);
 *   // carray_size(arr) == 5
 */

FORCE_INLINE static size_t carray_size(const cArray *obj){
    if(UNLIKELY(!obj)) return 0;
    return obj->len;
}

/**
 * @brief Getter for the beginning pointer.
 *
 * @param obj  The cArray. Must not be NULL.
 *
 * @note Returns NULL on NULL input.
 *
 * @example
 *   cArray *arr = carray_init((int[]){1,2,3,4,5}, sizeof(int), 5);
 *   void *x = carray_begin(arr);
 *   // x == arr->data
 */

FORCE_INLINE static void *carray_begin(cArray *obj){
    if(UNLIKELY(!obj)) return NULL;
    return obj->data;
}

/**
 * @brief Getter for the ending pointer (one past the last element).
 *
 * @param obj  The cArray. Must not be NULL.
 *
 * @note Returns NULL on NULL input.
 *
 * @example
 *   cArray *arr = carray_init((int[]){1,2,3,4,5}, sizeof(int), 5);
 *   void *x = carray_end(arr);
 *   // x == (char*)arr->data + arr->len * arr->size
 */

FORCE_INLINE static void *carray_end(cArray *obj){
    if(UNLIKELY(!obj)) return NULL;
    return (char *)obj->data + obj->len * obj->size;
}

/**
 * @brief Getter for is_empty.
 *
 * @param obj  The cArray. Must not be NULL.
 *
 * @note Returns -1 on NULL input.
 *
 * @example
 *   cArray *arr = carray_init((int[]){1,2,3,4,5}, sizeof(int), 5);
 *   // carray_empty(arr) == 0
 */

FORCE_INLINE static int carray_empty(cArray *obj){
    if(UNLIKELY(!obj)) return -1;
    return obj->len == 0 ? 1 : 0;
}

/**
 * @brief Clears cArray buffer.
 *
 * @param obj  The cArray. Must not be NULL.
 *
 * @note Silently returns on NULL input.
 * @note Does not shrink the allocated capacity. Use carray_fit() afterwards if desired.
 *
 * @example
 *   cArray *arr = carray_init((int[]){1,2,3,4,5}, sizeof(int), 5);
 *   carray_clear(arr);
 *   // arr->len == 0
 */

FORCE_INLINE static void carray_clear(cArray *obj){
    if(UNLIKELY(!obj)) return;
    obj->len = 0;
}

/**
 * @brief Getter for a pointer to the element at a given index.
 *
 * @param obj  The cArray. Must not be NULL.
 * @param idx  The index of the element. Must not exceed carray_size(obj) - 1.
 *
 * @note Returns NULL on invalid index or NULL input.
 *
 * @example
 *   cArray *arr = carray_init((int[]){1,2,3,4,5}, sizeof(int), 5);
 *   // *(int*)carray_at(arr, 2) == 3
 */

FORCE_INLINE static void *carray_at(const cArray *obj, size_t idx){
    if(UNLIKELY(!obj || !(idx < obj->len))) return NULL;
    return (char *)obj->data + idx * obj->size;
}

/**
 * @brief Appends n elements to the end of a cArray object.
 *
 * @param obj  The cArray to append to. Must not be NULL.
 * @param ptr  Pointer to the elements to append. Must not be NULL.
 * @param n    The number of elements to append.
 *
 * @note Silently returns on NULL input or n == 0.
 * @note Capacity is doubled when exceeded, with a minimum of 16 elements.
 *
 * @example
 *   cArray *arr = carray_init((int[]){1,2,3}, sizeof(int), 3);
 *   carray_push_back(arr, (int[]){4,5}, 2);
 *   // arr->data == {1,2,3,4,5}
 */

void carray_push_back(cArray *restrict obj, const void *restrict ptr, size_t n);

/**
 * @brief Prepends n elements to a cArray object.
 *
 * @param obj  The cArray to prepend to. Must not be NULL.
 * @param ptr  Pointer to the elements to prepend. Must not be NULL.
 * @param n    The number of elements to prepend.
 *
 * @note Silently returns on NULL input or n == 0.
 * @note Capacity is doubled when exceeded, with a minimum of 16 elements.
 *
 * @example
 *   cArray *arr = carray_init((int[]){3,4,5}, sizeof(int), 3);
 *   carray_push_front(arr, (int[]){1,2}, 2);
 *   // arr->data == {1,2,3,4,5}
 */

void carray_push_front(cArray *restrict obj, const void *restrict ptr, size_t n);

/**
 * @brief Inserts n elements into a cArray object at a given index.
 *
 * @param obj  The cArray to insert into. Must not be NULL.
 * @param ptr  Pointer to the elements to insert. Must not be NULL.
 * @param n    The number of elements to insert.
 * @param idx  The index position to insert at. Clamped to carray_size(obj) if out of range.
 *
 * @note Returns a pointer to the first inserted element, or NULL on failure.
 *
 * @example
 *   cArray *arr = carray_init((int[]){1,2,5}, sizeof(int), 3);
 *   carray_insert(arr, (int[]){3,4}, 2, 2);
 *   // arr->data == {1,2,3,4,5}
 */

void *carray_insert(cArray *restrict obj, const void *restrict ptr, size_t n, size_t idx);

/**
 * @brief Remove n elements from the back.
 *
 * @param obj  The cArray to remove from. Must not be NULL.
 * @param n    The number of elements to remove. Clamped to carray_size(obj).
 *
 * @example
 *   cArray *arr = carray_init((int[]){1,2,3,4,5}, sizeof(int), 5);
 *   carray_pop_back(arr, 2);
 *   // arr->data == {1,2,3}
 */

void carray_pop_back(cArray *obj, size_t n);

/**
 * @brief Remove n elements from the front.
 *
 * @param obj  The cArray to remove from. Must not be NULL.
 * @param n    The number of elements to remove. Clamped to carray_size(obj).
 *
 * @example
 *   cArray *arr = carray_init((int[]){1,2,3,4,5}, sizeof(int), 5);
 *   carray_pop_front(arr, 2);
 *   // arr->data == {3,4,5}
 */

void carray_pop_front(cArray *obj, size_t n);

/**
 * @brief Remove n elements starting from a specific index.
 *
 * @param obj  The cArray to modify. Must not be NULL.
 * @param idx  The index to erase from.
 * @param n    The number of elements to remove. If n exceeds the number of
 *             elements remaining from idx to the end, it is clamped: every
 *             element from idx onward is removed.
 *
 * @note If idx >= carray_size(obj), this is a no-op and returns the end
 *       pointer ((char*)obj->data + obj->len * obj->size).
 *
 * @example
 *   cArray *arr = carray_init((int[]){1,2,99,99,3}, sizeof(int), 5);
 *   carray_erase(arr, 2, 2);
 *   // arr->data == {1,2,3}
 */

void *carray_erase(cArray *obj, size_t idx, size_t n);

/**
 * @brief Compares two cArray objects byte-for-byte.
 *
 * @param a  First cArray. Must not be NULL.
 * @param b  Second cArray. Must not be NULL.
 *
 * @note Returns 0 on equal, -1 on a < b, 1 on a > b (as raw bytes, via memcmp).
 * @note Arrays of different element size or length are still compared as
 *       raw byte buffers up to the shorter total byte length.
 * @note NULL input(s) are undefined behavior (unchecked precondition).
 *
 * @example
 *   cArray *a = carray_init((int[]){1,2,3}, sizeof(int), 3);
 *   cArray *b = carray_init((int[]){1,2,3}, sizeof(int), 3);
 *   // carray_compare(a, b) == 0
 */

int carray_compare(const cArray *restrict a, const cArray *restrict b);

/**
 * @brief Checks if two cArray objects are equal.
 *
 * @param a  First cArray. Must not be NULL.
 * @param b  Second cArray. Must not be NULL.
 *
 * @note Returns 1 if equal (same len, same size, same bytes), else returns 0.
 * @note NULL input(s) are undefined behavior (unchecked precondition).
 *
 * @example
 *   cArray *a = carray_init((int[]){1,2,3}, sizeof(int), 3);
 *   cArray *b = carray_init((int[]){1,2,3}, sizeof(int), 3);
 *   // carray_equals(a, b) == 1
 */

int carray_equals(const cArray *restrict a, const cArray *restrict b);

/**
 * @brief Fit allocated memory to array length.
 *
 * @param obj  The cArray. Must not be NULL.
 *
 * @note Silently returns on NULL input.
 * @note Use when array will not be changed any further for maximum efficiency.
 *
 * @example
 *   cArray *arr = carray_init((int[]){1,2,3}, sizeof(int), 3);
 *   carray_fit(arr);
 *   // arr->cap == 3 instead of 16
 */

void carray_fit(cArray *obj);

#ifdef __cplusplus
}
#endif

#endif
