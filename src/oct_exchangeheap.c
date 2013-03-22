#include "oct_exchangeheap.h"
#include "oct_context.h"
#include "oct_type.h"
#include "oct_type_pointers.h"

#include <stdlib.h>

oct_Bool oct_ExchangeHeap_alloc(struct oct_Context* ctx, oct_Uword size, void** out_box) {
	*out_box = malloc(size);
	if(!*out_box) {
		oct_Context_setErrorOOM(ctx);
		return oct_False;
	}
	return oct_True;
}

//oct_Bool oct_ExchangeHeap_alloc(struct oct_Context* ctx, oct_BType type, void** out_box) {
//	oct_Uword size;
//	if(type.ptr->variant == OCT_TYPE_ARRAY) {
//		oct_Context_setErrorWithCMessage(ctx, "Use allocArray to allocate space for arrays");
//		return oct_False;
//	}
//	oct_Type_sizeOf(ctx, type, &size);
//	return OCT_ALLOCRAW(size, out_box);
//}
//
//oct_Bool oct_ExchangeHeap_allocArray(struct oct_Context* ctx, oct_BType type, oct_Uword size, void** out_box) {
//	oct_Uword elementSize;
//	if(type.ptr->variant != OCT_TYPE_ARRAY) {
//		oct_Context_setErrorWithCMessage(ctx, "Use alloc to allocate space for non-arrays");
//		return oct_False;
//	}
//	oct_Type_sizeOf(ctx, type.ptr->arrayType.elementType, &elementSize);
//	return OCT_ALLOCRAW(size * elementSize + sizeof(oct_Uword), out_box);
//}

oct_Bool oct_ExchangeHeap_free(struct oct_Context* ctx, void* box) {
	free(box);
	return oct_True;
}

//oct_Bool oct_ExchangeHeap_free(struct oct_Context* ctx, void* box, oct_BType type) {
//	// what the heck is the type for here?
//	return OCT_FREE(box);
//}

#ifdef _DEBUG

#include <memory.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

typedef struct allocInfo {
	void* addr;
	oct_Uword size;
	char* desc;
	const char* file;
	int line;
} allocInfo;

typedef struct allocInfoSetEntry {
    allocInfo ai;
} allocInfoSetEntry;

typedef struct allocInfoSet {
	oct_Uword capacity;
	allocInfoSetEntry* table;
} allocInfoSet;

static oct_Uword nextp2(oct_Uword n) {
	oct_Uword p2 = 2;
	while(p2 < n) {
		p2 <<= 1;
	}
	return p2;
}

static oct_Uword hashPointer(void* ptr) {
	return ((oct_Uword)ptr) * 761;
}

static oct_Bool allocInfoSet_Create(oct_Context* ctx, oct_Uword initialCap, allocInfoSet* table) {
	oct_Uword byteSize;
    
	table->capacity = nextp2(initialCap);
	byteSize = table->capacity * sizeof(allocInfoSetEntry);
	table->table = (allocInfoSetEntry*)malloc(byteSize);
	memset(table->table, 0, byteSize);
    
	return oct_True;
}

static oct_Bool allocInfoSet_Destroy(oct_Context* ctx, allocInfoSet* table) {
	free(table->table);
	table->capacity = 0;
	table->table = NULL;
	return oct_True;
}

static oct_Uword hash1(oct_Uword h) {
	return h;
}

static oct_Uword hash2(oct_Uword h) {
	return h * 17;
}

static oct_Uword hash3(oct_Uword h) {
	return h * 31;
}

static oct_Bool allocInfoSet_TryPut(allocInfoSet* table, allocInfoSetEntry* entry) {
	oct_Uword i, mask;
    allocInfoSetEntry tmp;
    
	mask = table->capacity - 1;
    
	i = hash1(hashPointer(entry->ai.addr)) & mask;
	tmp = table->table[i];
	table->table[i] = *entry;
	if(tmp.ai.addr == NULL || tmp.ai.addr == entry->ai.addr) {
		return oct_True;
	}
	*entry = tmp;

	i = hash2(hashPointer(entry->ai.addr)) & mask;
	tmp = table->table[i];
	table->table[i] = *entry;
	if(tmp.ai.addr == NULL || tmp.ai.addr == entry->ai.addr) {
		return oct_True;
	}
	*entry = tmp;

	i = hash3(hashPointer(entry->ai.addr)) & mask;
	tmp = table->table[i];
	table->table[i] = *entry;
	if(tmp.ai.addr == NULL || tmp.ai.addr == entry->ai.addr) {
		return oct_True;
	}
	*entry = tmp;

	return oct_False;
}

static oct_Bool allocInfoSet_Grow(oct_Context* ctx, allocInfoSet* table) {
	allocInfoSet bigger;
	oct_Uword i, cap;

	if(!allocInfoSet_Create(ctx, table->capacity + 1, &bigger)) {
		return oct_False;
	}
	for(i = 0; i < table->capacity; ++i) {
		if(table->table[i].ai.addr != NULL) {
			// Try more than once here? Table might balloon?
			if(allocInfoSet_TryPut(&bigger, &table->table[i]) == oct_False) {
				cap = bigger.capacity + 1;
				allocInfoSet_Destroy(ctx, &bigger);
				if(!allocInfoSet_Create(ctx, cap, &bigger)) {
					return oct_False;
				}
				i = 0;
			}
		}
	}
	allocInfoSet_Destroy(ctx, table);
	(*table) = bigger;
    return oct_True;
}

static oct_Bool allocInfoSet_Put(oct_Context* ctx, allocInfoSet* table, allocInfo ai) {
	oct_Uword i;
	allocInfoSetEntry entry;
	entry.ai = ai;
	while(oct_True) {
		for(i = 0; i < 5; ++i) {
			if(allocInfoSet_TryPut(table, &entry)) {
				return oct_True;
			}
		}
		if(!allocInfoSet_Grow(ctx, table)) {
			return oct_False;
		}
	}
}

static oct_Bool allocInfoSet_Get(allocInfoSet* table, void* addr, allocInfo* ai) {
	oct_Uword i, mask, ptrHash;

	mask = table->capacity - 1;
	ptrHash = hashPointer(addr);

	i = hash1(ptrHash) & mask;
	if(table->table[i].ai.addr == addr) {
		*ai = table->table[i].ai;
		table->table[i].ai.addr = NULL;
		return oct_True;
	}

	i = hash2(ptrHash) & mask;
	if(table->table[i].ai.addr == addr) {
		*ai = table->table[i].ai;
		table->table[i].ai.addr = NULL;
		return oct_True;
	}

	i = hash3(ptrHash) & mask;
	if(table->table[i].ai.addr == addr) {
		*ai = table->table[i].ai;
		table->table[i].ai.addr = NULL;
		return oct_True;
	}

	return oct_False;
}

static oct_Bool didInit = oct_False;
static allocInfoSet allocInfos;

oct_Bool oct_ExchangeHeap_debugAlloc(struct oct_Context* ctx, oct_Uword size, void** out_box, const char* description, const char* file, int line) {
	// TODO: need a lock here! (also this should probably be per runtime instead of global)
	allocInfo ai;
	oct_Bool result;
	if(!didInit) {
		allocInfoSet_Create(ctx, 1000000, &allocInfos);
		didInit = oct_True;
	}
	result = oct_ExchangeHeap_alloc(ctx, size, out_box);
	if(result) {
		ai.addr = *out_box;
		ai.desc = (char*)malloc(strlen(description) + 1);
		strcpy(ai.desc, description);
		ai.file = file;
		ai.line = line;
		ai.size = size;
		allocInfoSet_Put(ctx, &allocInfos, ai);
	}
	return result;
}

oct_Bool oct_ExchangeHeap_debugFree(struct oct_Context* ctx, void* box) {
	// TODO: lock!
	allocInfo out;
	out.addr = NULL;
	allocInfoSet_Get(&allocInfos, box, &out);
	if(out.addr) {
		free(out.desc);
		return oct_ExchangeHeap_free(ctx, box);
	}
	else {
		assert("Attempt to free unallocated memory" && oct_False);
		return oct_False;
	}
}

oct_Bool oct_ExchangeHeap_report(struct oct_Context* ctx) {
	// TODO: lock!
	oct_Uword i;
	oct_Uword nleaks = 0;
	oct_Uword bytes = 0;
	for(i = 0; i < allocInfos.capacity; ++i) {
		if(allocInfos.table[i].ai.addr) {
			++nleaks;
			bytes += allocInfos.table[i].ai.size;
			printf("LEAK: %p, size: %lu, %s, %s, %d\n",
				allocInfos.table[i].ai.addr,
				allocInfos.table[i].ai.size,
				allocInfos.table[i].ai.desc,
				allocInfos.table[i].ai.file,
				allocInfos.table[i].ai.line);
		}
	}
	printf("%lu leaks. %lu bytes.\n", nleaks, bytes);
	return oct_True;
}

#endif
