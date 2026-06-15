#include "cString.h"

#include <string.h>
#include <stdlib.h>
#include <stdint.h>

cString *cstring_init(const char *str){
    if(UNLIKELY(!str)) return NULL;
    cString *obj = malloc(sizeof(cString));
    if(UNLIKELY(!obj)) return NULL;
    obj->len = strlen(str);
    obj->cap = obj->len + 1;
    obj->data = malloc(obj->cap);
    if(UNLIKELY(!obj->data)){
        free(obj);
        return NULL;
    }
    memcpy(obj->data, str, obj->len);
    obj->data[obj->len] = '\0';
    return obj;
}

void cstring_free(cString *obj){
    if(UNLIKELY(!obj)) return;
    free(obj->data);
    free(obj);
}

size_t cstring_size(const cString *obj){
    if(UNLIKELY(!obj)) return 0;
    return obj->len;
}

size_t cstring_length(const cString *obj){
    if(UNLIKELY(!obj)) return 0;
    return obj->len;
}

char cstring_at(const cString *obj, size_t idx){
    if(UNLIKELY(!obj || !(idx < obj->len))) return '\0';
    return obj->data[idx];
}

void cstring_push_back(cString *restrict obj, const char *restrict str) {
    if(UNLIKELY(!obj || !str)) return;
    if(UNLIKELY(str[0] == '\0')) return;
    size_t str_len = strlen(str);
    if(UNLIKELY(str_len > SIZE_MAX - obj->len - 1)) return;
    size_t target_cap = obj->len + str_len + 1;
    if(UNLIKELY(target_cap > obj->cap)){
        size_t new_cap = obj->cap < 16 ? 16 : obj->cap << 1; 
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

