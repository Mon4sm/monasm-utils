#include "cArray.h"

#include <string.h>
#include <stdlib.h>
#include <stdint.h>

static int carray_grow(cArray *obj, size_t target_cap){
    if(LIKELY(target_cap <= obj->cap)) return 1;
    size_t new_cap = obj->cap < 16 ? 16 : obj->cap;
    if(new_cap <= SIZE_MAX / 2){
        new_cap <<= 1;
    }
    if(new_cap < target_cap){
        new_cap = target_cap;
    }
    if(UNLIKELY(obj->size != 0 && new_cap > SIZE_MAX / obj->size)){
        return 0;
    }
    void *data = realloc(obj->data, new_cap * obj->size);
    if(UNLIKELY(!data)) return 0;
    obj->data = data;
    obj->cap = new_cap;
    return 1;
}

cArray *carray_init(const void *ptr, size_t size, size_t len){
    if(UNLIKELY(size == 0)) return NULL;
    if(UNLIKELY(!ptr && len > 0)) return NULL;
    cArray *obj = malloc(sizeof(cArray));
    if(UNLIKELY(!obj)) return NULL;
    obj->len = len;
    obj->cap = len < 16 ? 16 : len;
    obj->size = size;
    if(UNLIKELY(obj->cap > SIZE_MAX / size)){
        free(obj);
        return NULL;
    }
    obj->data = malloc(obj->cap * size);
    if(UNLIKELY(!obj->data)){
        free(obj);
        return NULL;
    }
    if(ptr) memcpy(obj->data, ptr, len * size);
    return obj;
}

void carray_copy(cArray *restrict dst, const cArray *restrict src){
    if(UNLIKELY(!dst || !src)) return;
    if(UNLIKELY(dst == src)) return;
    if(UNLIKELY(dst->size != src->size)) return;
    void *new_data = malloc(src->cap ? src->cap * src->size : 1);
    if(UNLIKELY(!new_data)) return;
    free(dst->data);
    dst->len = src->len;
    dst->cap = src->cap;
    dst->data = new_data;
    if(dst->len) memcpy(dst->data, src->data, dst->len * dst->size);
}

cArray *carray_clone(const cArray *src){
    if(UNLIKELY(!src)) return NULL;
    cArray *obj = malloc(sizeof(cArray));
    if(UNLIKELY(!obj)) return NULL;
    obj->len = src->len;
    obj->cap = src->cap;
    obj->size = src->size;
    obj->data = malloc(obj->cap ? obj->cap * obj->size : 1);
    if(UNLIKELY(!obj->data)){
        free(obj);
        return NULL;
    }
    if(obj->len) memcpy(obj->data, src->data, obj->len * obj->size);
    return obj;
}

void carray_free(cArray *obj){
    if(UNLIKELY(!obj)) return;
    free(obj->data);
    free(obj);
}

void carray_push_back(cArray *restrict obj, const void *restrict ptr, size_t n){
    if(UNLIKELY(!obj || !ptr)) return;
    if(UNLIKELY(n == 0)) return;
    if(UNLIKELY(n > SIZE_MAX - obj->len)) return;
    if(UNLIKELY(!carray_grow(obj, obj->len + n))) return;
    memcpy((char *)obj->data + obj->len * obj->size, ptr, n * obj->size);
    obj->len += n;
}

void carray_push_front(cArray *restrict obj, const void *restrict ptr, size_t n){
    if(UNLIKELY(!obj || !ptr)) return;
    if(UNLIKELY(n == 0)) return;
    if(UNLIKELY(n > SIZE_MAX - obj->len)) return;
    if(UNLIKELY(!carray_grow(obj, obj->len + n))) return;
    memmove((char *)obj->data + n * obj->size, obj->data, obj->len * obj->size);
    memcpy(obj->data, ptr, n * obj->size);
    obj->len += n;
}

void *carray_insert(cArray *restrict obj, const void *restrict ptr, size_t n, size_t idx){
    if(UNLIKELY(!obj || !ptr)) return NULL;
    if(UNLIKELY(idx > obj->len)) idx = obj->len;
    if(UNLIKELY(n == 0)) return (char *)obj->data + idx * obj->size;
    if(UNLIKELY(n > SIZE_MAX - obj->len)) return NULL;
    if(UNLIKELY(!carray_grow(obj, obj->len + n))) return NULL;
    char *base = (char *)obj->data;
    memmove(base + (idx + n) * obj->size, base + idx * obj->size, (obj->len - idx) * obj->size);
    memcpy(base + idx * obj->size, ptr, n * obj->size);
    obj->len += n;
    return base + idx * obj->size;
}

void carray_pop_back(cArray *obj, size_t n){
    if(UNLIKELY(!obj)) return;
    if(n > obj->len) n = obj->len;
    obj->len -= n;
}

void carray_pop_front(cArray *obj, size_t n){
    if(UNLIKELY(!obj)) return;
    if(n > obj->len) n = obj->len;
    memmove(obj->data, (char *)obj->data + n * obj->size, (obj->len - n) * obj->size);
    obj->len -= n;
}

void *carray_erase(cArray *obj, size_t idx, size_t n){
    if(UNLIKELY(!obj)) return NULL;
    if(UNLIKELY(idx >= obj->len)) return (char *)obj->data + obj->len * obj->size;
    char *base = (char *)obj->data;
    if(n > obj->len - idx){
        obj->len = idx;
        return base + idx * obj->size;
    }
    else{
        memmove(base + idx * obj->size, base + (idx + n) * obj->size, (obj->len - idx - n) * obj->size);
        obj->len -= n;
        return base + idx * obj->size;
    }
}

int carray_compare(const cArray *restrict a, const cArray *restrict b){
    size_t a_bytes = a->len * a->size;
    size_t b_bytes = b->len * b->size;
    size_t min_bytes = a_bytes < b_bytes ? a_bytes : b_bytes;
    int ans = min_bytes ? memcmp(a->data, b->data, min_bytes) : 0;
    if(ans != 0) return ans > 0 ? 1 : -1;
    if(a_bytes == b_bytes) return 0;
    return a_bytes > b_bytes ? 1 : -1;
}

int carray_equals(const cArray *restrict a, const cArray *restrict b){
    if(a->len != b->len || a->size != b->size) return 0;
    size_t bytes = a->len * a->size;
    return bytes ? memcmp(a->data, b->data, bytes) == 0 : 1;
}

void carray_fit(cArray *obj){
    if(UNLIKELY(!obj)) return;
    void *new_data = realloc(obj->data, obj->len * obj->size);
    if(UNLIKELY(!new_data && obj->len != 0)) return;
    obj->data = new_data;
    obj->cap = obj->len;
}
