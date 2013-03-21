#include "oct_hashtable.h"
#include "oct_namespace.h"
#include "oct_type.h"
#include "oct_symbol.h"
#include "oct_context.h"
#include "oct_runtime.h"
#include "oct_exchangeheap.h"
#include "oct_copyable.h"

#include <stddef.h>

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_Hashtable_initProtocol(struct oct_Context* ctx) {
	oct_BHashtable table;
	CHECK(OCT_ALLOCRAW(sizeof(oct_ProtocolBinding), (void**)&ctx->rt->builtInProtocols.HashtableKey.ptr, "_oct_Hashtable_initProtocol"));
	ctx->rt->builtInProtocols.HashtableKey.ptr->protocolType = ctx->rt->builtInTypes.HashtableKey;
	table.ptr = &ctx->rt->builtInProtocols.HashtableKey.ptr->implementations;
	return oct_Hashtable_ctor(ctx, table, 100);
}

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

	// HashtableKeyOption
	t = ctx->rt->builtInTypes.HashtableKeyOption;
	t.ptr->variant = OCT_TYPE_VARIADIC;
	t.ptr->variadicType.alignment = 0;
	CHECK(oct_ABType_createOwned(ctx, 3, &t.ptr->variadicType.types));
	t.ptr->variadicType.types.ptr->data[0] = ctx->rt->builtInTypes.Nothing;
	t.ptr->variadicType.types.ptr->data[1] = ctx->rt->builtInTypes.OHashtableKey;
	t.ptr->variadicType.types.ptr->data[2] = ctx->rt->builtInTypes.BHashtableKey;

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

oct_Bool oct_AHashtableEntry_createOwned(struct oct_Context* ctx, oct_Uword initialCap, oct_OAHashtableEntry* out_result) {
	oct_Uword i;
	CHECK(OCT_ALLOCRAW(initialCap * sizeof(oct_HashtableEntry) + sizeof(oct_AHashtableEntry), (void**)&out_result->ptr, "oct_AHashtableEntry_createOwned"));
	out_result->ptr->size = initialCap;
	for(i = 0; i < out_result->ptr->size; ++i) {
		out_result->ptr->table[i].key.variant = OCT_HASHTABLEKEYOPTION_NOTHING;
	}
	return oct_True;
}

static oct_Bool destroyEntry(oct_Context* ctx, oct_HashtableEntry* entry) {
	oct_OObject obj;
	oct_BSelf self;
	if(entry->key.variant != OCT_HASHTABLEKEYOPTION_NOTHING) {
		if(entry->key.variant == OCT_HASHTABLEKEYOPTION_OWNED) {
			self.self = entry->key.borrowed.self.self;
			CHECK(oct_Object_as(ctx, self, entry->key.borrowed.vtable->type, ctx->rt->builtInProtocols.Object, (oct_BObject*)&obj));
			oct_Object_destroyOwned(ctx, obj);
		}
		if(entry->val.variant == OCT_ANY_OOBJECT) {
			obj.self.self = entry->val.oobject.self.self;
			obj.vtable = entry->val.oobject.vtable;
			oct_Object_destroyOwned(ctx, obj);
		}
		entry->key.variant = OCT_HASHTABLEKEYOPTION_NOTHING;
	}
    return oct_True;
}

oct_Bool oct_AHashtableEntry_destroyOwned(struct oct_Context* ctx, oct_OAHashtableEntry self) {
	oct_Uword i;
	for(i = 0; i < self.ptr->size; ++i) {
		destroyEntry(ctx, &self.ptr->table[i]);
	}
	return OCT_FREE(self.ptr);
}

oct_Bool oct_Hashtable_ctor(struct oct_Context* ctx, oct_BHashtable self, oct_Uword initialCap) {
	return oct_AHashtableEntry_createOwned(ctx, nextp2(initialCap), &self.ptr->table);
}

oct_Bool oct_Hashtable_dtor(struct oct_Context* ctx, oct_BSelf self) {
	return oct_AHashtableEntry_destroyOwned(ctx, ((oct_Hashtable*)self.self)->table);
}

static oct_Bool keyHash(oct_Context* ctx, oct_BHashtableKey key, oct_Uword* result) {
	oct_BSelf bself;
	bself.self = key.self.self;
	return key.vtable->functions.hashable.hash(ctx, bself, result);
}

static oct_Bool keyEq(oct_Context* ctx, oct_HashtableKeyOption key1, oct_HashtableKeyOption key2, oct_Bool* out_eq) {
	oct_Bool result = oct_True;

	if(key1.variant == OCT_HASHTABLEKEYOPTION_NOTHING && key2.variant == OCT_HASHTABLEKEYOPTION_NOTHING) {
		*out_eq = oct_True; // ehrm, do we really want it this way?
	}
	else if(key1.variant == OCT_HASHTABLEKEYOPTION_NOTHING && key2.variant != OCT_HASHTABLEKEYOPTION_NOTHING) {
		*out_eq = oct_False;
	}
	else if(key2.variant == OCT_HASHTABLEKEYOPTION_NOTHING && key1.variant != OCT_HASHTABLEKEYOPTION_NOTHING) {
		*out_eq = oct_False;
	}
	else if(key1.borrowed.vtable->type.ptr != key2.borrowed.vtable->type.ptr) {
		*out_eq = oct_False;
	}
	else {
		result = key1.borrowed.vtable->functions.eq.equals(ctx, key1.borrowed.self, key2.borrowed.self, out_eq);
	}

	return result;
}

static oct_Bool oct_Hashtable_putOne(oct_Context* ctx, oct_BHashtable self, oct_HashtableEntry* entry, oct_Uword i, oct_Bool* result) {
	oct_HashtableEntry tmp;
	oct_Bool eq;

	tmp = self.ptr->table.ptr->table[i];
	self.ptr->table.ptr->table[i] = *entry;
	eq = tmp.key.variant == OCT_HASHTABLEKEYOPTION_NOTHING;
	if(!eq) {
		CHECK(keyEq(ctx, tmp.key, entry->key, &eq));
		if(eq) {
			// Keys are equal, destroy old entry
			destroyEntry(ctx, &tmp);
		}
	}
	if(eq) { // Key was Nothing or equal to existing entry
		*result = oct_True;
	}
	else {
		*result = oct_False;
		// Set entry to tmp to continue the cukoo chain on failure
		*entry = tmp;
	}

	return oct_True;
}

static oct_Bool oct_Hashtable_tryPut(oct_Context* ctx, oct_BHashtable self, oct_HashtableEntry* entry, oct_Bool* out_result) {
	oct_Uword i, mask, key;
    
	mask = self.ptr->table.ptr->size - 1;
    
	CHECK(keyHash(ctx, entry->key.borrowed, &key));
	i = hash1(key) & mask;
	CHECK(oct_Hashtable_putOne(ctx, self, entry, i, out_result));
	if(*out_result) {
		return oct_True;
	}

	// Note: do not remove re-hash of key. Entry is changed after a failed call to putOne
	CHECK(keyHash(ctx, entry->key.borrowed, &key));
	i = hash2(key) & mask;
	CHECK(oct_Hashtable_putOne(ctx, self, entry, i, out_result));
	if(*out_result) {
		return oct_True;
	}

	// Note: do not remove re-hash of key. Entry is changed after a failed call to putOne
	CHECK(keyHash(ctx, entry->key.borrowed, &key));
	i = hash3(key) & mask;
	CHECK(oct_Hashtable_putOne(ctx, self, entry, i, out_result));

	return oct_True;
}

static oct_Bool oct_Hashtable_grow(oct_Context* ctx, oct_BHashtable self) {
	oct_Hashtable bigger;
	oct_BHashtable bBigger;
	oct_Uword i, j, cap;
	oct_Bool didPut;
	oct_HashtableEntry entry;

	bBigger.ptr = &bigger;

	CHECK(oct_Hashtable_ctor(ctx, bBigger, self.ptr->table.ptr->size + 1));
	for(i = 0; i < self.ptr->table.ptr->size; ++i) {
		if(self.ptr->table.ptr->table[i].key.variant != OCT_HASHTABLEKEYOPTION_NOTHING) {
			entry = self.ptr->table.ptr->table[i];
			didPut = oct_False;
			for(j = 0; j < 5 && (!didPut); ++j) {
				CHECK(oct_Hashtable_tryPut(ctx, bBigger, &entry, &didPut));
			}
			if(!didPut) {
				cap = bBigger.ptr->table.ptr->size + 1;
				// Just free the memory. If we run destructors here we will nuke part of the table
				CHECK(OCT_FREE(bBigger.ptr->table.ptr));
				CHECK(oct_Hashtable_ctor(ctx, bBigger, cap));
				i = 0;
			}
		}
	}
	// Just free the memory. If we run destructors here we will nuke the new table
	CHECK(OCT_FREE(self.ptr->table.ptr));
	(*self.ptr) = bigger;

	return oct_True;
}

oct_Bool oct_Hashtable_put(struct oct_Context* ctx, oct_BHashtable self, oct_HashtableKeyOption key, oct_Any value) {
	oct_Uword i;
	oct_HashtableEntry entry;
	oct_Bool didPut;

	entry.key = key;
	entry.val = value;
	while(oct_True) {
		for(i = 0; i < 5; ++i) {
			CHECK(oct_Hashtable_tryPut(ctx, self, &entry, &didPut));
			if(didPut) {
				return oct_True;
			}
		}
		CHECK(oct_Hashtable_grow(ctx, self));
	}
}

#undef CHECK
#define CHECK(X) if(!X) goto error;

#define GETMODE_BORROW 0
#define GETMODE_TAKE 1
#define GETMODE_COPY 2

static oct_Bool oct_Hashtable_get(struct oct_Context* ctx, oct_BHashtable self, oct_BHashtableKey key, oct_Uword getMode, oct_Any* out_value) {
	oct_Uword i, mask, hash;
	oct_Bool eq;
	oct_Bool result = oct_True;
	oct_HashtableKeyOption tmpKey;
	oct_BCopyable bcopyable;
	oct_BSelf bself;

	tmpKey.variant = OCT_HASHTABLEKEYOPTION_BORROWED;
	tmpKey.borrowed.self.self = key.self.self;
	tmpKey.borrowed.vtable = key.vtable;

	mask = self.ptr->table.ptr->size - 1;
	CHECK(keyHash(ctx, tmpKey.borrowed, &hash));

	i = hash1(hash) & mask;
	CHECK(keyEq(ctx, tmpKey, self.ptr->table.ptr->table[i].key, &eq));
	if(eq) {
		goto match;
	}

	i = hash2(hash) & mask;
	CHECK(keyEq(ctx, tmpKey, self.ptr->table.ptr->table[i].key, &eq));
	if(eq) {
		goto match;
	}

	i = hash3(hash) & mask;
	CHECK(keyEq(ctx, tmpKey, self.ptr->table.ptr->table[i].key, &eq));
	if(eq) {
		goto match;
	}

	// Key did not match anything. Return a Nothing-Any
	(*out_value).variant = OCT_ANY_NOTHING;
	(*out_value).nothing.dummy = 0;
	goto end;
match:
	*out_value = self.ptr->table.ptr->table[i].val;
	if((*out_value).variant == OCT_ANY_OOBJECT) {
		if(getMode == GETMODE_TAKE) {
			self.ptr->table.ptr->table[i].val.variant = OCT_ANY_NOTHING;
			destroyEntry(ctx, &self.ptr->table.ptr->table[i]);
		}
		else if(getMode == GETMODE_BORROW) { // borrow
			(*out_value).variant = OCT_ANY_BOBJECT;
		}
		else { // copy
			bself.self = out_value->oobject.self.self;
			CHECK(oct_Object_as(ctx, bself, out_value->oobject.vtable->type, ctx->rt->builtInProtocols.Copyable, (oct_BObject*)&bcopyable));
			CHECK(oct_Copyable_copyOwned(ctx, bcopyable, &out_value->oobject));
		}
	}
	goto end;
error:
	result = oct_False;
end:
	return result;
}

// Get a value from the table. If the value is owned, it will be removed from the hash table and returned. If the value
// is borrowed, a reference to it will be returned and it will remain in the hash table.
oct_Bool oct_Hashtable_take(struct oct_Context* ctx, oct_BHashtable self, oct_BHashtableKey key, oct_Any* out_value) {
	return oct_Hashtable_get(ctx, self, key, GETMODE_TAKE, out_value);
}

// Get a value from the table. If the value is borrowed, return a reference to it. If the value is owned, return
// a borrowed reference to it. In both cases the value remains in the table.
oct_Bool oct_Hashtable_borrow(struct oct_Context* ctx, oct_BHashtable self, oct_BHashtableKey key, oct_Any* out_value) {
	return oct_Hashtable_get(ctx, self, key, GETMODE_BORROW, out_value);
}

oct_Bool oct_Hashtable_copyOrBorrow(struct oct_Context* ctx, oct_BHashtable self, oct_BHashtableKey key, oct_Any* out_value) {
	return oct_Hashtable_get(ctx, self, key, GETMODE_COPY, out_value);
}
