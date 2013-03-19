#include "oct_hashtable.h"
#include "oct_namespace.h"
#include "oct_type.h"
#include "oct_symbol.h"
#include "oct_context.h"
#include "oct_runtime.h"
#include "oct_exchangeheap.h"

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

oct_Bool oct_AHashtableEntry_createOwned(struct oct_Context* ctx, oct_Uword initialCap, oct_OAHashtableEntry* out_result) {
	return oct_ExchangeHeap_allocRaw(ctx, initialCap * sizeof(oct_HashtableEntry) + sizeof(oct_AHashtableEntry), (void**)&out_result->ptr);
}

oct_Bool oct_AHashtableEntry_destroyOwned(struct oct_Context* ctx, oct_OAHashtableEntry self) {
	// TODO: Actually destroy values here! This currently creates memory leaks!
	return oct_ExchangeHeap_freeRaw(ctx, self.ptr);
}

oct_Bool oct_Hashtable_ctor(struct oct_Context* ctx, oct_BHashtable self, oct_Uword initialCap) {
	return oct_AHashtableEntry_createOwned(ctx, nextp2(initialCap), &self.ptr->table);
}

oct_Bool oct_Hashtable_dtor(struct oct_Context* ctx, oct_BHashtable self) {
	return oct_AHashtableEntry_destroyOwned(ctx, self.ptr->table);
}

static oct_Bool keyHash(oct_Context* ctx, oct_OHashtableKey key, oct_Uword* result) {
	oct_BSelf bself;
	bself.self = key.self.self;
	return key.vtable->functions.hashable.hash(ctx, bself, result);
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

static oct_Bool oct_Hashtable_putOne(oct_Context* ctx, oct_BHashtable self, oct_HashtableEntry* entry, oct_Uword i, oct_Bool* result) {
	oct_HashtableEntry tmp;
	oct_OObject oobj;
	oct_Bool eq;

	tmp = self.ptr->table.ptr->table[i];
	self.ptr->table.ptr->table[i] = *entry;
	eq = keyIsNothing(ctx, tmp.key);
	if(!eq) {
		CHECK(keyEq(ctx, tmp.key, entry->key, &eq));
		if(eq) {
			// Keys are equal, destroy old entry
			oobj.self = tmp.key.self;
			oobj.vtable = (oct_ObjectVTable*)tmp.key.vtable;
			CHECK(oct_Object_destroyOwned(ctx, oobj));
			if(entry->val.variant == OCT_ANY_OOBJECT) {
				CHECK(oct_Object_destroyOwned(ctx, tmp.val.oobject));
			}
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
    
	CHECK(keyHash(ctx, entry->key, &key));
	i = hash1(key) & mask;
	CHECK(oct_Hashtable_putOne(ctx, self, entry, i, out_result));
	if(*out_result) {
		return oct_True;
	}

	// Note: do not remove re-hash of key. Entry is changed after a failed call to putOne
	CHECK(keyHash(ctx, entry->key, &key));
	i = hash2(key) & mask;
	CHECK(oct_Hashtable_putOne(ctx, self, entry, i, out_result));
	if(*out_result) {
		return oct_True;
	}

	// Note: do not remove re-hash of key. Entry is changed after a failed call to putOne
	CHECK(keyHash(ctx, entry->key, &key));
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
		if(self.ptr->table.ptr->table[i].key.vtable->type.ptr != ctx->rt->builtInTypes.Nothing.ptr) {
			entry = self.ptr->table.ptr->table[i];
			didPut = oct_False;
			for(j = 0; j < 5 && (!didPut); ++j) {
				CHECK(oct_Hashtable_tryPut(ctx, bBigger, &entry, &didPut));
			}
			if(!didPut) {
				cap = bBigger.ptr->table.ptr->size + 1;
				CHECK(oct_Hashtable_dtor(ctx, bBigger));
				CHECK(oct_Hashtable_ctor(ctx, bBigger, cap));
				i = 0;
			}
		}
	}
	CHECK(oct_Hashtable_dtor(ctx, self));
	(*self.ptr) = bigger;

	return oct_True;
}

oct_Bool oct_Hashtable_put(struct oct_Context* ctx, oct_BHashtable self, oct_OHashtableKey key, oct_Any value) {
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

static oct_Bool oct_Hashtable_get(struct oct_Context* ctx, oct_BHashtable self, oct_BHashtableKey key, oct_Bool isTake, oct_Any* out_value) {
	oct_Uword i, mask, hash;
	oct_Bool eq;
	oct_Bool result = oct_True;
	oct_OHashtableKey tmpOwnedKey; // not really owned
	oct_OObject oobject;

	tmpOwnedKey.self.self = key.self.self;
	tmpOwnedKey.vtable = key.vtable;

	mask = self.ptr->table.ptr->size - 1;
	CHECK(keyHash(ctx, tmpOwnedKey, &hash));

	i = hash1(hash) & mask;
	CHECK(keyEq(ctx, tmpOwnedKey, self.ptr->table.ptr->table[i].key, &eq));
	if(eq) {
		goto match;
	}

	i = hash2(hash) & mask;
	CHECK(keyEq(ctx, tmpOwnedKey, self.ptr->table.ptr->table[i].key, &eq));
	if(eq) {
		goto match;
	}

	i = hash3(hash) & mask;
	CHECK(keyEq(ctx, tmpOwnedKey, self.ptr->table.ptr->table[i].key, &eq));
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
		if(isTake) {
			// Remove entry from the table; just drop the key and replace with nil
			// TODO: replace this nil stuff with an Option type for the key. nil feels wrong. It obscures information and when it is not a value type, can a global instance really be used safely?
			oobject.self.self = self.ptr->table.ptr->table[i].key.self.self;
			oobject.vtable = (oct_ObjectVTable*)self.ptr->table.ptr->table[i].key.vtable;
			CHECK(oct_Object_destroyOwned(ctx, oobject));
			self.ptr->table.ptr->table[i].key.self.self = &ctx->rt->nil;
			self.ptr->table.ptr->table[i].key.vtable = (oct_HashtableKeyVTable*)ctx->rt->vtables.NothingAsHashtableKey.ptr;
		}
		else { // borrow
			(*out_value).variant = OCT_ANY_BOBJECT;
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
	return oct_Hashtable_get(ctx, self, key, oct_True, out_value);
}

// Get a value from the table. If the value is borrowed, return a reference to it. If the value is owned, return
// a borrowed reference to it. In both cases the value remains in the table.
oct_Bool oct_Hashtable_borrow(struct oct_Context* ctx, oct_BHashtable self, oct_BHashtableKey key, oct_Any* out_value) {
	return oct_Hashtable_get(ctx, self, key, oct_False, out_value);
}
