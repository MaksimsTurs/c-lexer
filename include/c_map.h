#ifndef H_CMAP
#define H_CMAP

#include "c_types.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct cmap_item {
	t_int64           m_hash;
	t_char*           m_key;
	t_any             m_value;
} cmap_item;

typedef struct {
	struct cmap_item* m_items;
	t_int64    				m_size;
	t_int64    				m_capacity;
	t_int8     				m_is_resizable;
	t_int8     				m_kind;
} cmap;

#define CMAP_MAX_MAP_CAPACITY CTYPE_INT64_MAX - 10

#define CMAP_KIND_STATIC  1
#define CMAP_KIND_DYNAMIC 2

#define CMAP_RESIZE_DIRECTION_GROWTH 1
#define CMAP_RESIZE_DIRECTION_SHRINK 0

#define CMAP_ERR_INCORRECT_MAP_CAPACITY        -1
#define CMAP_ERR_INCORRECT_KEY                 -2
#define CMAP_ERR_ITEM_NOT_FOUND                -3
#define CMAP_ERR_FREE_INDEX_NOT_FOUND          -4
#define CMAP_ERR_NULL_PTR                      -5
#define CMAP_ERR_MEM_ALLOCATION                -6
#define CMAP_ERR_MAP_IS_FULL_AND_NOT_RESIZABLE -7


#define CMAP_SUCCESS          0

#define CMAP_FAIL(cond, code) do { \
	if(cond) { \
		return code; \
	} \
} while(0); 
#define CMAP_MOVE_ITEM(new_item, old_item, new_hash) do { \
	new_item.m_key   = old_item.m_key; \
	new_item.m_value = old_item.m_value; \
	new_item.m_hash  = new_hash; \
} while(0);
#define CMAP_SAFE_CALL(expr_res) do { \
	t_int64 res = expr_res; \
	if(res < 0) { return res; } \
} while(0);

#define SHOULD_MAP_GROWTH(map) map->m_is_resizable && (((t_float32)(map)->m_capacity)) / (map)->m_size >= 0.75f
#define SHOULD_MAP_SHRINK(map) map->m_is_resizable && (((t_float32)(map)->m_capacity)) / (map)->m_size <= 0.50f

//###########################################
//################# Utility #################
//###########################################
void    cmap_print(const cmap* self);
t_int64 cmap_hash(const t_char* key);
t_int64 cmap_resize(cmap* self, t_uint8 direction);
void    cmap_find_item_index_by_hash(const cmap* self, t_int64 hash, t_int64* start);
void    cmap_find_free_index(const cmap_item* items, t_int64 capacity, t_int64* start);

//############################################
//################### Core ###################
//############################################
t_bool  cmap_has(const cmap* self, const t_char* key);
t_int64 cmap_sinit(cmap* self, cmap_item* buff, t_int64 size, t_int8 should_expand);
t_int64 cmap_dinit(cmap* self, t_int64 size, t_int8 should_expand);
t_int64 cmap_set(cmap* self, const t_char* key, t_any value);
t_int64 cmap_get(const cmap* self, cmap_item* item, const t_char* key);
t_int64 cmap_delete(cmap* self, const t_char* key);


#endif // H_CMAP