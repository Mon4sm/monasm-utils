#include "cString.h"

#include <string.h>
#include <stdlib.h>
#include <stdint.h>

cString *cstring_init(const char *str){
    if(UNLIKELY(!str)) return NULL;
    cString *obj = malloc(sizeof(cString));
    if(UNLIKELY(!obj)) return NULL;
    obj->len = strlen(str);
    obj->cap = obj->len + 1 < 16 ? 16 : obj->len + 1;
    obj->data = malloc(obj->cap);
    if(UNLIKELY(!obj->data)){
        free(obj);
        return NULL;
    }
    memcpy(obj->data, str, obj->len);
    obj->data[obj->len] = '\0';
    return obj;
}

void cstring_copy(cString *restrict dst, const cString *restrict src){
    if(UNLIKELY(!dst || !src)) return;
    char *new_data = malloc(src->cap);
    if(UNLIKELY(!new_data)) return;
    free(dst->data);
    dst->len = src->len;
    dst->cap = src->cap;
    dst->data = new_data;
    memcpy(dst->data, src->data, dst->len);
    dst->data[dst->len] = '\0';
}

cString *cstring_clone(const cString *src){
    if(UNLIKELY(!src)) return NULL;
    cString *obj = malloc(sizeof(cString));
    if(UNLIKELY(!obj)) return NULL;
    obj->len = src->len;
    obj->cap = src->cap;
    obj->data = malloc(obj->cap);
    if(UNLIKELY(!obj->data)){
        free(obj);
        return NULL;
    }
    memcpy(obj->data, src->data, obj->len);
    obj->data[obj->len] = '\0';
    return obj;
}

void cstring_free(cString *obj){
    if(UNLIKELY(!obj)) return;
    free(obj->data);
    free(obj);
}

void cstring_push_back(cString *restrict obj, const char *restrict str){
    if(UNLIKELY(!obj || !str)) return;
    if(UNLIKELY(str[0] == '\0')) return;
    size_t str_len = strlen(str);
    if(UNLIKELY(str_len > SIZE_MAX - obj->len - 1)) return;
    size_t target_cap = obj->len + str_len + 1;
    if(UNLIKELY(target_cap > obj->cap)){
        size_t new_cap = obj->cap < 16 ? 16 : obj->cap;
        if(new_cap <= SIZE_MAX / 2){
            new_cap <<= 1;
        }
        if(new_cap < target_cap){
            new_cap = target_cap;
        }
        char *data = realloc(obj->data, new_cap);
        if(UNLIKELY(!data)) return;
        obj->data = data;
        obj->cap = new_cap;
    }
    memcpy(obj->data + obj->len, str, str_len);
    obj->len += str_len;
    obj->data[obj->len] = '\0';
}

void cstring_push_front(cString *restrict obj, const char *restrict str){
    if(UNLIKELY(!obj || !str)) return;
    if(UNLIKELY(str[0] == '\0')) return;
    size_t str_len = strlen(str);
    if(UNLIKELY(str_len > SIZE_MAX - obj->len - 1)) return;
    size_t target_cap = obj->len + str_len + 1;
    if(UNLIKELY(target_cap > obj->cap)){
        size_t new_cap = obj->cap < 16 ? 16 : obj->cap;
        if(new_cap <= SIZE_MAX / 2){
            new_cap <<= 1;
        }
        if(new_cap < target_cap){
            new_cap = target_cap;
        }
        char *data = realloc(obj->data, new_cap);
        if(UNLIKELY(!data)) return;
        obj->data = data;
        obj->cap = new_cap;
    }
    memmove(obj->data + str_len, obj->data, obj->len + 1);
    memcpy(obj->data, str, str_len);
    obj->len += str_len;
}

char *cstring_insert(cString *restrict obj, const char *restrict str, size_t idx){
    if(UNLIKELY(!obj || !str)) return NULL;
    if(UNLIKELY(idx > obj->len)) idx = obj->len;
    if(str[0] == '\0') return obj->data + idx;
    size_t str_len = strlen(str);
    if(UNLIKELY(str_len > SIZE_MAX - obj->len - 1)) return NULL;
    size_t target_cap = obj->len + str_len + 1;
    if(UNLIKELY(target_cap > obj->cap)){
        size_t new_cap = obj->cap < 16 ? 16 : obj->cap;
        if(new_cap <= SIZE_MAX / 2){
            new_cap <<= 1;
        }
        if(new_cap < target_cap){
            new_cap = target_cap;
        }
        char *data = realloc(obj->data, new_cap);
        if(UNLIKELY(!data)) return NULL;
        obj->data = data;
        obj->cap = new_cap;
    }
    memmove(obj->data + str_len + idx, obj->data + idx, obj->len - idx + 1);
    memcpy(obj->data + idx, str, str_len);
    obj->len += str_len;
    return obj->data + idx;
}

void cstring_pop_back(cString *obj, size_t n){
    if(UNLIKELY(!obj)) return;
    if(n > obj->len) n = obj->len;
    obj->data[obj->len - n] = '\0';
    obj->len -= n;    
}

void cstring_pop_front(cString *obj, size_t n){
    if(UNLIKELY(!obj)) return;
    if(n > obj->len) n = obj->len;
    memmove(obj->data, obj->data + n, obj->len - n + 1);
    obj->len -= n;
}
