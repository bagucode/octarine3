#include "oct_type.h"
#include "oct_type_pointers.h"
#include "oct_runtime.h"
#include "oct_field.h"
#include "oct_context.h"
#include "oct_exchangeheap.h"

#include <stddef.h>
#include <stdlib.h>
#include <memory.h>

// Private

oct_Bool _oct_Type_initType(struct oct_Context* ctx) {
	oct_Type* t = ctx->rt->builtInTypes.Type;
	oct_Bool result;
	t->variant = OCT_TYPE_VARIADIC;
	t->variadicType.alignment = 0;
	t->variadicType.size = sizeof(oct_Type);
	result = oct_OABType_alloc(ctx, 7, &t->variadicType.types);
	if(!result) {
		return result;
	}
	t->variadicType.types.ptr->data[0].ptr = ctx->rt->builtInTypes.ProtoType;
	t->variadicType.types.ptr->data[1].ptr = ctx->rt->builtInTypes.VariadicType;
	t->variadicType.types.ptr->data[2].ptr = ctx->rt->builtInTypes.StructType;
	t->variadicType.types.ptr->data[3].ptr = ctx->rt->builtInTypes.ArrayType;
	t->variadicType.types.ptr->data[4].ptr = ctx->rt->builtInTypes.FixedSizeArrayType;
	t->variadicType.types.ptr->data[5].ptr = ctx->rt->builtInTypes.PointerType;
	t->variadicType.types.ptr->data[6].ptr = ctx->rt->builtInTypes.InterfaceType;
	return oct_True;
}

oct_Bool _oct_BType_initType(struct oct_Context* ctx) {
	ctx->rt->builtInTypes.BType->variant = OCT_TYPE_POINTER;
	ctx->rt->builtInTypes.BType->pointerType.kind = OCT_POINTER_BORROWED;
	ctx->rt->builtInTypes.BType->pointerType.type.ptr = ctx->rt->builtInTypes.Type;
	return oct_True;
}

oct_Bool _oct_ABType_initType(struct oct_Context* ctx) {
	ctx->rt->builtInTypes.ABType->variant = OCT_TYPE_ARRAY;
	ctx->rt->builtInTypes.ABType->arrayType.elementType.ptr = ctx->rt->builtInTypes.BType;
	return oct_True;
}

oct_Bool _oct_OABType_initType(struct oct_Context* ctx) {
	ctx->rt->builtInTypes.OABType->variant = OCT_TYPE_POINTER;
	ctx->rt->builtInTypes.OABType->pointerType.kind = OCT_POINTER_OWNED;
	ctx->rt->builtInTypes.OABType->pointerType.type.ptr = ctx->rt->builtInTypes.ABType;
	return oct_True;
}

// Public

oct_Bool oct_OABType_alloc(struct oct_Context* ctx, oct_Uword size, oct_OABType* out_result) {
	oct_Uword i;
    if(!oct_ExchangeHeap_alloc(ctx, sizeof(oct_ABType) + (sizeof(oct_BType) * size), (void**)&out_result->ptr)) {
        return oct_False;
    }
	out_result->ptr->size = size;
	// Initialize all to Nothing
	for(i = 0; i < size; ++i) {
		out_result->ptr->data[i].ptr = ctx->rt->builtInTypes.Nothing;
	}
	return oct_True;
}

// Helper code for copying object graphs. Need a space efficient hash table and a stack.

// Cuckoo hash table for pointer translation during deep copy

typedef struct PointerTranslationTableEntry {
    void* key;
    void* val;
} PointerTranslationTableEntry;

typedef struct PointerTranslationTable {
	oct_Uword capacity;
	oct_Uword size;
	PointerTranslationTableEntry* table;
} PointerTranslationTable;

static oct_Uword nextp2(oct_Uword n) {
	oct_Uword p2 = 2;
	while(p2 < n) {
		p2 <<= 1;
	}
	return p2;
}

#ifdef OCT64
#define FNV_PRIME 1099511628211U
#define FNV_OFFSET_BASIS 14695981039346656037U
#else
#define FNV_PRIME 16777619U
#define FNV_OFFSET_BASIS 2166136261U
#endif

static oct_Uword fnv1a(const oct_U8* data, oct_Uword datasize) {
	oct_Uword hash = FNV_OFFSET_BASIS;
	oct_Uword i;
	for(i = 0; i < datasize; ++i) {
		hash = hash ^ data[i];
		hash = hash * FNV_PRIME;
	}
	return hash;
}

static oct_Uword hashPointer(void* ptr) {
	return fnv1a((const oct_U8*)ptr, sizeof(void*));
}

static oct_Bool PointerTranslationTable_Create(oct_Uword initialCap, PointerTranslationTable* table) {
	oct_Uword byteSize;
    
	table->capacity = nextp2(initialCap);
	table->size = 0;
	byteSize = table->capacity * sizeof(PointerTranslationTableEntry);
	table->table = (PointerTranslationTableEntry*)malloc(byteSize);
	if(table->table == NULL) {
		return oct_False;
	}
	memset(table->table, 0, byteSize);
    
	return oct_True;
}

static void PointerTranslationTable_Destroy(PointerTranslationTable* table) {
	free(table->table);
	table->capacity = 0;
	table->size = 0;
	table->table = NULL;
}

static oct_Uword hash1(oct_Uword h) {
	return h;
}

static oct_Uword hash2(oct_Uword h) {
	return h >> 4;
}

static oct_Uword hash3(oct_Uword h) {
	return h * 31;
}

static oct_Bool PointerTranslationTable_TryPut(PointerTranslationTable* table, PointerTranslationTableEntry* entry) {
	oct_Uword i, mask;
    PointerTranslationTableEntry tmp;
    
	mask = table->capacity - 1;
    
	i = hash1(hashPointer(entry->key)) & mask;
	tmp = table->table[i];
	table->table[i] = *entry;
	if(tmp.key == NULL || tmp.key == entry->key) {
		++table->size;
		return oct_True;
	}
	*entry = tmp;

	i = hash2(hashPointer(entry->key)) & mask;
	tmp = table->table[i];
	table->table[i] = *entry;
	if(tmp.key == NULL || tmp.key == entry->key) {
		++table->size;
		return oct_True;
	}
	*entry = tmp;

	i = hash3(hashPointer(entry->key)) & mask;
	tmp = table->table[i];
	table->table[i] = *entry;
	if(tmp.key == NULL || tmp.key == entry->key) {
		++table->size;
		return oct_True;
	}
	*entry = tmp;

	return oct_False;
}

static oct_Bool PointerTranslationTable_Grow(PointerTranslationTable* table) {
	PointerTranslationTable bigger;
	oct_Uword i, cap;

	if(!PointerTranslationTable_Create(table->capacity + 1, &bigger)) {
		return oct_False;
	}
	for(i = 0; i < table->capacity; ++i) {
		if(table->table[i].key != NULL) {
			// Try more than once here? Table might balloon?
			if(PointerTranslationTable_TryPut(&bigger, &table->table[i]) == oct_False) {
				cap = bigger.capacity + 1;
				PointerTranslationTable_Destroy(&bigger);
				if(!PointerTranslationTable_Create(cap, &bigger)) {
					return oct_False;
				}
				i = 0;
			}
		}
	}
	PointerTranslationTable_Destroy(table);
	(*table) = bigger;
}

static oct_Bool PointerTranslationTable_Put(PointerTranslationTable* table, void* key, void* val) {
	oct_Uword i;
	PointerTranslationTableEntry entry;

	entry.key = key;
	entry.val = val;
	while(oct_True) {
		for(i = 0; i < 5; ++i) {
			if(PointerTranslationTable_TryPut(table, &entry)) {
				return;
			}
		}
		PointerTranslationTable_Grow(table);
	}
}

static void* PointerTranslationTable_Get(PointerTranslationTable* table, void* key) {
	oct_Uword i, mask, ptrHash;

	mask = table->capacity - 1;
	ptrHash = hashPointer(key);

	i = hash1(ptrHash) & mask;
	if(table->table[i].key == key)
		return table->table[i].val;

	i = hash2(ptrHash) & mask;
	if(table->table[i].key == key)
		return table->table[i].val;

	i = hash3(ptrHash) & mask;
	if(table->table[i].key == key)
		return table->table[i].val;

	return NULL;
}

// Stack used to store call frames on the heap to prevent blowing the C stack when traversing large graphs

typedef struct FrameStackEntry {
	oct_Type* type;
	void* object;
} FrameStackEntry;

typedef struct FrameStack {
    oct_Uword capacity;
    oct_Uword top;
    FrameStackEntry* stack;
} FrameStack;

static oct_Bool FrameStack_Create(FrameStack* stack, oct_Uword initialCap) {
    stack->capacity = initialCap;
    stack->top = 0;
    stack->stack = (FrameStackEntry*)malloc(sizeof(FrameStackEntry) * initialCap);
	if(!stack->stack) {
		return oct_False;
	}
    return oct_True;
}

static void FrameStack_Destroy(FrameStack* stack) {
    free(stack->stack);
	stack->capacity = 0;
	stack->top = 0;
	stack->stack = NULL;
}

static oct_Bool FrameStack_Push(FrameStack* stack, FrameStackEntry* entry) {
    oct_Uword index;
	FrameStack bigger;
    
    if(stack->capacity == stack->top) {
		bigger = (*stack);
		bigger.capacity *= 2;
		bigger.stack = (FrameStackEntry*)malloc(sizeof(FrameStackEntry) * bigger.capacity);
		if(!bigger.stack) {
			return oct_False;
		}
		memcpy(bigger.stack, stack->stack, stack->capacity * sizeof(FrameStackEntry));
		FrameStack_Destroy(stack);
		(*stack) = bigger;
    }
    stack->stack[stack->top++] = (*entry);
	return oct_True;
}

static oct_Bool FrameStack_Pop(FrameStack* stack, FrameStackEntry* out) {
    if(stack->top == 0) {
        return oct_False;
    }
	(*out) = stack->stack[--stack->top];
    return oct_True;
}

// Does a deep copy of an object graph and returns an owned copy
// Will fail if the given graph contains any non-copyable objects.
oct_Bool oct_Type_deepCopyGraphOwned(struct oct_Context* ctx, oct_Any root, oct_Any* out_ownedCopy) {

}

