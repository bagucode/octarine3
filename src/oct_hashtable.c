#include "oct_hashtable.h"
#include "oct_namespace.h"
#include "oct_type.h"
#include "oct_symbol.h"
#include "oct_context.h"
#include "oct_runtime.h"

#include <stddef.h>

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_Hashtable_init(struct oct_Context* ctx) {

	// HashtableKey
	oct_BType t = ctx->rt->builtInTypes.HashtableKey;
	t.ptr->variant = OCT_TYPE_PROTOCOL;
	CHECK(oct_ABFunction_createOwned(ctx, 2, &t.ptr->protocolType.functions));
	t.ptr->protocolType.functions.ptr->data[0] = ctx->rt->functions.hash;
	t.ptr->protocolType.functions.ptr->data[1] = ctx->rt->functions.eq;

	// OHashtableKey
	t = ctx->rt->builtInTypes.OHashtableKey;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_OWNED_PROTOCOL;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.HashtableKey;

	// BHashtableKey
	t = ctx->rt->builtInTypes.BHashtableKey;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_BORROWED_PROTOCOL;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.HashtableKey;

	// HashtableEntry
	t = ctx->rt->builtInTypes.HashtableEntry;
	t.ptr->variant = OCT_TYPE_STRUCT;
	t.ptr->structType.alignment = 0;
	t.ptr->structType.size = sizeof(oct_HashtableEntry);
	CHECK(oct_AField_createOwned(ctx, 2, &t.ptr->structType.fields));
	t.ptr->structType.fields.ptr->data[0].offset = offsetof(oct_HashtableEntry, key);
	t.ptr->structType.fields.ptr->data[0].type = ctx->rt->builtInTypes.OHashtableKey;
	t.ptr->structType.fields.ptr->data[1].offset = offsetof(oct_HashtableEntry, val);
	t.ptr->structType.fields.ptr->data[1].type = ctx->rt->builtInTypes.Any;

	// AHashtableEntry
	t = ctx->rt->builtInTypes.AHashtableEntry;
	t.ptr->variant = OCT_TYPE_ARRAY;
	t.ptr->arrayType.elementType = ctx->rt->builtInTypes.HashtableEntry;

	// OAHashtableEntry
	t = ctx->rt->builtInTypes.OAHashtableEntry;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_OWNED;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.AHashtableEntry;

	// Hashtable
	t = ctx->rt->builtInTypes.Hashtable;
	t.ptr->variant = OCT_TYPE_STRUCT;
	t.ptr->structType.alignment = 0;
	t.ptr->structType.size = sizeof(oct_Hashtable);
	CHECK(oct_AField_createOwned(ctx, 1, &t.ptr->structType.fields));
	t.ptr->structType.fields.ptr->data[0].offset = offsetof(oct_Hashtable, table);
	t.ptr->structType.fields.ptr->data[0].type = ctx->rt->builtInTypes.OAHashtableEntry;

	// BHashtable
	t = ctx->rt->builtInTypes.BHashtable;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_BORROWED;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.Hashtable;

	return oct_True;
}

oct_Bool oct_Hashtable_ctor(struct oct_Context* ctx, oct_BHashtable self, oct_Uword initialCap) {

}

oct_Bool oct_Hashtable_put(struct oct_Context* ctx, oct_BHashtable self, oct_OHashtableKey key, oct_Any value) {
}

// Get a value from the table. If the value is owned, it will be removed from the hash table and returned. If the value
// is borrowed, a reference to it will be returned and it will remain in the hash table.
oct_Bool oct_Hashtable_take(struct oct_Context* ctx, oct_BHashtable self, oct_BHashtableKey key, oct_Any* out_value) {
}

// Get a value from the table. If the value is borrowed, return a reference to it. If the value is owned, return
// a borrowed reference to it. In both cases the value remains in the table.
oct_Bool oct_Hashtable_borrow(struct oct_Context* ctx, oct_BHashtable self, oct_BHashtableKey key, oct_Any* out_value) {
}



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

static oct_Uword hashPointer(void* ptr) {
	// Actually do something here?
	return (oct_Uword)ptr;
}

oct_Bool oct_Hashtable_ctor(struct oct_Context* ctx, oct_BHashtable self, oct_Uword initialCap) {
	
}

static oct_Bool PointerTranslationTable_Create(oct_Context* ctx, oct_Uword initialCap, PointerTranslationTable* table) {
	oct_Uword byteSize;
    
	table->capacity = nextp2(initialCap);
	table->size = 0;
	byteSize = table->capacity * sizeof(PointerTranslationTableEntry);
	table->table = (PointerTranslationTableEntry*)calloc(1, byteSize);
	if(table->table == NULL) {
		oct_Context_setErrorOOM(ctx);
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

static oct_Bool PointerTranslationTable_Grow(oct_Context* ctx, PointerTranslationTable* table) {
	PointerTranslationTable bigger;
	oct_Uword i, cap;

	if(!PointerTranslationTable_Create(ctx, table->capacity + 1, &bigger)) {
		return oct_False;
	}
	for(i = 0; i < table->capacity; ++i) {
		if(table->table[i].key != NULL) {
			// Try more than once here? Table might balloon?
			if(PointerTranslationTable_TryPut(&bigger, &table->table[i]) == oct_False) {
				cap = bigger.capacity + 1;
				PointerTranslationTable_Destroy(&bigger);
				if(!PointerTranslationTable_Create(ctx, cap, &bigger)) {
					return oct_False;
				}
				i = 0;
			}
		}
	}
	PointerTranslationTable_Destroy(table);
	(*table) = bigger;
    return oct_True;
}

static oct_Bool PointerTranslationTable_Put(oct_Context* ctx, PointerTranslationTable* table, void* key, void* val) {
	oct_Uword i;
	PointerTranslationTableEntry entry;

	entry.key = key;
	entry.val = val;
	while(oct_True) {
		for(i = 0; i < 5; ++i) {
			if(PointerTranslationTable_TryPut(table, &entry)) {
				return oct_True;
			}
		}
		if(!PointerTranslationTable_Grow(ctx, table)) {
			return oct_False;
		}
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


oct_Bool oct_Hashtable_put(struct oct_Context* ctx, oct_BHashtable self, oct_OHashtableKey key, oct_OObject value) {

}

oct_Bool oct_Hashtable_take(struct oct_Context* ctx, oct_BHashtable self, oct_BHashtableKey key, oct_OObject* out_value) {
}

oct_Bool oct_Hashtable_borrow (struct oct_Context* ctx, oct_BHashtable self, oct_BHashtableKey key, oct_BObject* out_value) {
}
