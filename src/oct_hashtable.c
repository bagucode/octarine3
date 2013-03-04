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

static oct_Uword nextp2(oct_Uword n) {
	oct_Uword p2 = 2;
	while(p2 < n) {
		p2 <<= 1;
	}
	return p2;
}

static oct_Uword hash1(oct_Uword h) {
	return h;
}

static oct_Uword hash2(oct_Uword h) {
	return h * 17;
}

static oct_Uword hash3(oct_Uword h) {
	return h * 751;
}

oct_Bool oct_Hashtable_ctor(struct oct_Context* ctx, oct_BHashtable self, oct_Uword initialCap) {
	return oct_AHashtableEntry_createOwned(ctx, nextp2(initialCap), &self.ptr->table);
}

oct_Bool oct_Hashtable_dtor(struct oct_Context* ctx, oct_BHashtable self) {
	return oct_AHashtableEntry_destroyOwned(ctx, self.ptr->table);
}

static oct_Bool keyHash(oct_Context* ctx, oct_HashtableEntry* entry, oct_Uword* result) {
	oct_BSelf bself;
	bself.self = entry->key.self.self;
	return entry->key.vtable->functions.hashable.hash(ctx, bself, result);
}

static oct_Bool keyIsNothing(oct_Context* ctx, oct_OHashtableKey key) {
	return key.vtable->type.ptr == ctx->rt->builtInTypes.Nothing.ptr;
}

static oct_Bool keyEq(oct_Context* ctx, oct_OHashtableKey key1, oct_OHashtableKey key2, oct_Bool* out_eq) {
	oct_BSelf key1Self;
	oct_BSelf key2Self;

	key1Self.self = key1.self.self;
	key2Self.self = key2.self.self;

	return key1.vtable->functions.eq.equals(ctx, key1Self, key2Self, out_eq);
}

static oct_Bool oct_Hashtable_tryPut(oct_Context* ctx, oct_BHashtable self, oct_HashtableEntry* entry) {
	oct_Uword i, mask, key;
	oct_HashtableEntry tmp;
	oct_Bool eq;
    
	mask = self.ptr->table.ptr->size - 1;
    
	CHECK(keyHash(ctx, entry, &key));
	i = hash1(key) & mask;
	tmp = self.ptr->table.ptr->table[i];
	self.ptr->table.ptr->table[i] = *entry;
	eq = keyIsNothing(ctx, tmp.key);
	if(!eq) {
		CHECK(keyEq(ctx, tmp.key, entry->key, &eq));
	}
	if(eq) {
#error MAC WAS HERE
		// Destroy old hash table entry here!
		CHECK(oct_HashtableE
		return oct_True;
	}
	if(keyIsNothing(ctx, entry->key) || tmp.key == entry->key) {
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



static void PointerTranslationTable_Destroy(PointerTranslationTable* table) {
	free(table->table);
	table->capacity = 0;
	table->size = 0;
	table->table = NULL;
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
