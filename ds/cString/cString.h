#ifndef cString_h
#define cString_h

#include <stddef.h>

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

#define cstring_length(obj) cstring_size(obj)

/**
 * @brief A dynamic heap-allocated string object.
 *
 * @param data  Pointer to the null-terminated character buffer.
 * @param len   Number of characters in the string, excluding the null terminator.
 * @param cap   Allocated buffer size in bytes, always greater than len.
 *
 * @note Should only be created via cstring_init() and freed via cstring_free().
 * @note Direct modification of struct members is discouraged.
 */

typedef struct cString {
    char *data;
    size_t len;
    size_t cap; // Oguri Cap
} cString;


/**
 * @brief cString constructor.
 *
 * @param str  Initial string. Must not be NULL.
 *
 * @note Returns NULL on NULL input.
 * @note The intended way for the object to be created.
 *
 * @example
 *   cString *s = cstring_init("Oguri");
 *   // cString object created!!!
 *   // s->data == "Oguri"
 *   // s->len  == 5
 *   // s->cap  == 16 
 */

cString *cstring_init(const char *str);

/**
 * @brief Memory deallocator for cString object.
 *
 * @param obj  The cString. Must not be NULL.
 *
 * @note Silently returns on NULL input.
 * @note Always call this when done with a cString to avoid memory leaks.
 *
 * @example
 *   cString *s = cstring_init("Oguri");
 *   cstring_free(s);
 *   // Memory deallocated!!!
 */

void cstring_free(cString *obj);

/**
 * @brief Getter for cString->len.
 *
 * @param obj  The cString. Must not be NULL.
 *
 * @note Returns 0 on NULL input.
 * @note Identical to cstring_length(). Both exist for convenience.
 *
 * @example
 *   cString *s = cstring_init("Oguri");
 *   // cstring_size(s) == 5
 */

FORCE_INLINE static size_t cstring_size(const cString *obj){
    if(UNLIKELY(!obj)) return 0;
    return obj->len;
}

/**
 * @brief Getter for the beginning pointer.
 *
 * @param obj  The cString. Must not be NULL.
 *
 * @note Returns NULL on NULL input.
 *
 * @example
 *   cString *s = cstring_init("Oguri");
 *   char *x = cstring_begin(s);
 *   // x == s->data
 */

FORCE_INLINE static char *cstring_begin(cString *obj){
    if(UNLIKELY(!obj)) return NULL;
    return obj->data;
}

/**
 * @brief Getter for the ending pointer.
 *
 * @param obj  The cString. Must not be NULL.
 *
 * @note Returns NULL on NULL input.
 *
 * @example
 *   cString *s = cstring_init("Oguri");
 *   char *x = cstring_end(s);
 *   // x == s->data + s->len ('\0')
 */

FORCE_INLINE static char *cstring_end(cString *obj){
    if(UNLIKELY(!obj)) return NULL;
    return obj->data + obj->len;
}

/**
 * @brief Getter for is_empty.
 *
 * @param obj  The cString. Must not be NULL.
 *
 * @note Returns -1 on NULL input.
 *
 * @example
 *   cString *s = cstring_init("Oguri");
 *   // cstring_empty(s) == 0
 */

FORCE_INLINE static int cstring_empty(cString *obj){
    if(UNLIKELY(!obj)) return -1;
    return obj->len == 0 ? 1 : 0;
}

/**
 * @brief Getter for cString->data[index].
 *
 * @param obj  The cString. Must not be NULL.
 * @param idx  The index of cString->data. Must not exceed cString->len.
 *
 * @note Returns '\0' on invalid index or NULL input.
 *
 * @example
 *   cString *s = cstring_init("Oguri");
 *   // cstring_at(s, 2) == 'u'
 */

char cstring_at(const cString *obj, size_t idx);

/**
 * @brief Appends a char array to the end of a cString object.
 *
 * @param obj  The cString to append to. Must not be NULL.
 * @param str  The string to append. Must not be NULL.
 *
 * @note Silently returns on NULL input or empty string.
 * @note Capacity is doubled when exceeded, with a minimum of 16 bytes.
 *
 * @example
 *   cString *s = cstring_init("Oguri");
 *   cstring_push_back(s, " Cap");
 *   // s->data == "Oguri Cap"
 */

void cstring_push_back(cString *restrict obj, const char *restrict str);

/**
 * @brief Prepends a char array to a cString object.
 *
 * @param obj  The cString to append to. Must not be NULL.
 * @param str  The string to append. Must not be NULL.
 *
 * @note Silently returns on NULL input or empty string.
 * @note Capacity is doubled when exceeded, with a minimum of 16 bytes.
 *
 * @example
 *   cString *s = cstring_init("Oguri");
 *   cstring_push_front(s, "Cap ");
 *   // s->data == "Cap Oguri"
 */

void cstring_push_front(cString *restrict obj, const char *restrict str);

/**
 * @brief Inserts a char array into a cString object at a given index.
 *
 * @param obj  The cString to append to. Must not be NULL.
 * @param str  The string to append. Must not be NULL.
 * @param idx  The index position to insert.
 *
 * @note Returns the pointer of the inserted position.
 *
 * @example
 *   cString *s = cstring_init("Oguri Cap");
 *   cstring_insert(s, "Fat ", 6);
 *   // s->data == "Oguri Fat Cap"
 */

char *cstring_insert(cString *restrict obj, const char *restrict str, size_t idx);

#endif
