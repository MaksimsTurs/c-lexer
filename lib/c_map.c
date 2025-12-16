#include "../include/c_map.h"

void cmap_print(const cmap* self)
{
	printf("Size(%.5lli)\n", self->m_size);
	printf("Capacity(%.5lli)\n", self->m_capacity);

  for(t_int64 index = 0; index < self->m_capacity; index++)
  {
		printf("Index(%lli) Hash(%lli)\n", index, self->m_items[index].m_hash);
  }

	printf("\n");
}

t_int64 cmap_hash(const t_char* key)
{
	t_int64 hash = 0;

	while(*key)
	{
		hash += ((hash * 31) + hash) + *key++;
	}

	return hash;
}

void cmap_find_item_index_by_hash(const cmap* self, t_int64 hash, t_int64* start)
{
	t_int64 probe_count = 0;
	t_int64 index       = *start;
	
	while(probe_count < self->m_capacity) 
	{
		if(index == self->m_capacity)
		{
			index = 0;
		}
		
		if(self->m_items[index].m_hash == hash) 
		{
			*start = index;
			break;
		}
		
		probe_count++;
		index++;
	}
	
	if(probe_count == self->m_capacity)
	{
		*start = -1;
	}
}

void cmap_find_free_index(const cmap_item* items, t_int64 capacity, t_int64* start)
{
	t_int64 probe_count = 0;
	t_int64 index       = *start;

	while(probe_count < capacity)
	{
		if(index > capacity)
		{
			index = 0;
		}
		
		if(!items[index].m_hash)
		{
			*start = index;
			break;
		}

		probe_count++;
		index++;
	}

	if(probe_count == capacity)
	{
		*start = -1;
	}
}

t_int64 cmap_dinit(cmap* self, t_int64 capacity, t_int8 is_resizable)
{
	CMAP_FAIL(self == NULL,                     CMAP_ERR_NULL_PTR);
	CMAP_FAIL(capacity > CMAP_MAX_MAP_CAPACITY, CMAP_ERR_INCORRECT_MAP_CAPACITY);

	self->m_items = (cmap_item*)malloc(capacity * sizeof(cmap_item));
	CMAP_FAIL(self->m_items == NULL, CMAP_ERR_MEM_ALLOCATION);

	memset(self->m_items, 0, capacity * sizeof(cmap_item));

	self->m_is_resizable = is_resizable;
	self->m_kind = CMAP_KIND_DYNAMIC;
	self->m_size = 0;
	self->m_capacity = capacity;
	
	return CMAP_SUCCESS;
}

t_int64 cmap_sinit(cmap* self, cmap_item* buff, t_int64 capacity, t_int8 is_resizable)
{
	CMAP_FAIL(self == NULL || buff == NULL,     CMAP_ERR_NULL_PTR);
	CMAP_FAIL(capacity > CMAP_MAX_MAP_CAPACITY, CMAP_ERR_INCORRECT_MAP_CAPACITY);

	self->m_size         = 0;
	self->m_items        = buff;
	self->m_kind         = CMAP_KIND_STATIC;
	self->m_capacity     = capacity;
	self->m_is_resizable = is_resizable;
		
	return CMAP_SUCCESS;
}

t_bool cmap_has(const cmap* self, const t_char* key)
{
	t_int64 index = 0;
	t_int64 hash = 0;

	hash = cmap_hash(key);
	index = hash % self->m_capacity;

	if(!self->m_items[index].m_hash)
	{
		return false;
	}

	if(self->m_items[index].m_hash == hash)
	{
		return true;
	}

	if(self->m_items[index].m_hash != hash)
	{
		cmap_find_item_index_by_hash(self, hash, &index);

		return !(index == -1);
	}

	return false;
}

t_int64 cmap_set(cmap* self, const t_char* key, t_any value)
{
	CMAP_FAIL(self == NULL,                                              CMAP_ERR_NULL_PTR);
	CMAP_FAIL(*key == '\0',                                              CMAP_ERR_INCORRECT_KEY);
	CMAP_FAIL(self->m_capacity == 0,                                     CMAP_ERR_INCORRECT_MAP_CAPACITY);
	CMAP_FAIL(self->m_size == self->m_capacity && !self->m_is_resizable, CMAP_ERR_MAP_IS_FULL_AND_NOT_RESIZABLE);

	t_int64 hash  = 0;
	t_int64 index = 0;

	hash = cmap_hash(key);
	index = hash % self->m_capacity;

	if(self->m_items[index].m_hash)
	{
		// The collision is occur, first we need to check if is no element with the same
		// hash in the map.
		cmap_find_item_index_by_hash(self, hash, &index);
		
		if(index == -1)
		{
			// The element with the same hash doesn't exist in the map, find free index for new element
			// and save data.
			index = hash % self->m_size;
			cmap_find_free_index(self->m_items, self->m_capacity, &index);

			if(index == -1)
			{
				return CMAP_ERR_FREE_INDEX_NOT_FOUND;
			}
			
			self->m_items[index].m_hash = hash;
			self->m_items[index].m_key = (t_char*)key;
			self->m_items[index].m_value = value;
			
			self->m_size++;
		}
		else 
		{
			// The element with the same hash was found, override the old value with new value.
			self->m_items[index].m_value = value;
		}
	} 
	else
	{
		// There is no element with the same hash.
		self->m_items[index].m_hash  = hash;
		self->m_items[index].m_key   = (t_char*)key;
		self->m_items[index].m_value = value;
		
		self->m_size++;
	}
	
	if(SHOULD_MAP_GROWTH(self))
	{
		CMAP_SAFE_CALL(cmap_resize(self, CMAP_RESIZE_DIRECTION_GROWTH));
	}

	return CMAP_SUCCESS;
}

t_int64 cmap_resize(cmap* self, t_uint8 direction)
{	
	CMAP_FAIL(self == NULL, CMAP_ERR_NULL_PTR);

	t_int64 old_capacity  = self->m_capacity;
	t_int64 new_hash = 0;
	t_int64 new_index = 0;
	cmap_item* new_items = NULL;

	self->m_capacity = direction == CMAP_RESIZE_DIRECTION_GROWTH ? 
		self->m_capacity * 2 : 
		self->m_capacity / 2;
	new_items = (cmap_item*)malloc(self->m_capacity * sizeof(cmap_item));
	CMAP_FAIL(new_items == NULL, CMAP_ERR_NULL_PTR);

	memset(new_items, 0, self->m_capacity * sizeof(cmap_item));

	for(t_int64 index = 0; index < old_capacity; index++)
	{
		if(self->m_items[index].m_hash)
		{
			new_hash = cmap_hash(self->m_items[index].m_key);
			new_index = new_hash % self->m_size;
			
			if(new_items[new_index].m_hash)
			{
				cmap_find_free_index(new_items, self->m_size, &new_index);

				if(new_index == -1)
				{
					return CMAP_ERR_FREE_INDEX_NOT_FOUND;;
				}
				else
				{
					CMAP_MOVE_ITEM(new_items[new_index], self->m_items[index], new_hash);
				}
			}
			else
			{
				CMAP_MOVE_ITEM(new_items[new_index], self->m_items[index], new_hash);
			}
		}
	}
	
	if(self->m_kind == CMAP_KIND_DYNAMIC)
	{
		free(self->m_items);
		self->m_items = NULL;
	}
	
	self->m_items = new_items;

	return CMAP_SUCCESS;
}

t_int64 cmap_get(const cmap* self, cmap_item* item, const t_char* key)
{
	CMAP_FAIL(self == NULL || item == NULL || key == NULL, CMAP_ERR_NULL_PTR);
	CMAP_FAIL(*key == '\0',                                CMAP_ERR_INCORRECT_KEY);

	t_int64 hash  = 0;
	t_int64 index = 0;

	hash = cmap_hash(key);
	index = hash % self->m_size;

	if(self->m_items[index].m_hash == hash)
	{
		*item = self->m_items[index];
	}
	else
	{
		cmap_find_item_index_by_hash(self, hash, &index);

		if(index == -1) 
		{
			return CMAP_ERR_ITEM_NOT_FOUND;
		}
		else 
		{
			*item = self->m_items[index];
		}
	}

	return CMAP_SUCCESS;
}

t_int64 cmap_delete(cmap* self, const t_char* key)
{
	CMAP_FAIL(self == NULL || key == NULL, CMAP_ERR_NULL_PTR);
	CMAP_FAIL(*key == '\0',                CMAP_ERR_INCORRECT_KEY);

	t_int64 hash  = 0;
	t_int64 index = 0;

	hash = cmap_hash(key);
	index = hash % self->m_size;

	if(self->m_items[index].m_hash == hash)
	{
		self->m_items[index].m_hash  = 0;
		self->m_items[index].m_key   = NULL;
		self->m_items[index].m_value = NULL;
			
		self->m_size--;
	}
	else
	{
		cmap_find_item_index_by_hash(self, hash, &index);

		if(index == -1) 
		{
			return CMAP_ERR_ITEM_NOT_FOUND;
		}
		else 
		{
			self->m_items[index].m_hash = 0;
			self->m_items[index].m_key   = NULL;
			self->m_items[index].m_value = NULL;

			self->m_size--;
		}
	}

	if(SHOULD_MAP_SHRINK(self))
	{
		CMAP_SAFE_CALL(cmap_resize(self, CMAP_RESIZE_DIRECTION_SHRINK));
	}

	return CMAP_SUCCESS;
}