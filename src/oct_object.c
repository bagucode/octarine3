#include "oct_object.h"
#include "oct_context.h"
#include "oct_runtime.h"
#include "oct_object_vtable.h"
#include "oct_exchangeheap.h"
#include "oct_type.h"
#include "oct_function.h"

#include <memory.h>

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_Object_initProtocol(struct oct_Context* ctx) {
	oct_BHashtable table;
	CHECK(OCT_ALLOCOWNED(sizeof(oct_ProtocolBinding), (void**)&ctx->rt->builtInProtocols.Object.ptr, "_oct_Object_initProtocol"));
	ctx->rt->builtInProtocols.Object.ptr->protocolType = ctx->rt->builtInTypes.Object;
	table.ptr = &ctx->rt->builtInProtocols.Object.ptr->implementations;
	return oct_Hashtable_ctor(ctx, table, 100);
}

oct_Bool _oct_Object_init(struct oct_Context* ctx) {
	oct_BFunction fn;

	// Object protocol.
	oct_BType t = ctx->rt->builtInTypes.Object;
	t.ptr->variant = OCT_TYPE_PROTOCOL;
	CHECK(oct_ABFunction_createOwned(ctx, 1, &t.ptr->protocolType.functions));
	t.ptr->protocolType.functions.ptr->data[0] = ctx->rt->functions.dtor;

	// OObject
	t = ctx->rt->builtInTypes.OObject;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_OWNED_PROTOCOL;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.Object;

	// BObject
	t = ctx->rt->builtInTypes.BObject;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_BORROWED_PROTOCOL;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.Object;

	// OObjectOption
	t = ctx->rt->builtInTypes.OObjectOption;
	t.ptr->variant = OCT_TYPE_VARIADIC;
	t.ptr->variadicType.alignment = 0;
	t.ptr->variadicType.size = sizeof(oct_OObjectOption);
	CHECK(oct_ABType_createOwned(ctx, 2, &t.ptr->variadicType.types));
	t.ptr->variadicType.types.ptr->data[0] = ctx->rt->builtInTypes.Nothing;
	t.ptr->variadicType.types.ptr->data[1] = ctx->rt->builtInTypes.OObject;

	// BObjectOption
	t = ctx->rt->builtInTypes.BObjectOption;
	t.ptr->variant = OCT_TYPE_VARIADIC;
	t.ptr->variadicType.alignment = 0;
	t.ptr->variadicType.size = sizeof(oct_BObjectOption);
	CHECK(oct_ABType_createOwned(ctx, 2, &t.ptr->variadicType.types));
	t.ptr->variadicType.types.ptr->data[0] = ctx->rt->builtInTypes.Nothing;
	t.ptr->variadicType.types.ptr->data[1] = ctx->rt->builtInTypes.BObject;

	// dtor function signature
	CHECK(OCT_ALLOCOWNED(sizeof(oct_Function), (void**)&ctx->rt->functions.dtor.ptr, "functions.dtor"));
	fn = ctx->rt->functions.dtor;
	CHECK(oct_ABType_createOwned(ctx, 1, &fn.ptr->paramTypes));
	CHECK(oct_ABType_createOwned(ctx, 0, &fn.ptr->returnTypes));
	fn.ptr->paramTypes.ptr->data[0] = ctx->rt->builtInTypes.BSelf;

	// AOObjectOption
	t = ctx->rt->builtInTypes.AOObjectOption;
	t.ptr->variant = OCT_TYPE_ARRAY;
	t.ptr->arrayType.elementType = ctx->rt->builtInTypes.OObjectOption;

	// Object protocol {dtor}
	CHECK(_oct_Protocol_addBuiltIn(ctx, ctx->rt->builtInProtocols.Object, 1, &ctx->rt->vtables.AOObjectOptionAsObject, t, oct_AOObjectOption_dtor));

	// OAOObjectOption
	t = ctx->rt->builtInTypes.OAOObjectOption;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_OWNED;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.AOObjectOption;

	// BAOObjectOption
	t = ctx->rt->builtInTypes.BAOObjectOption;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_BORROWED;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.AOObjectOption;

	return oct_True;
}

oct_Bool oct_AOObjectOption_createOwned(struct oct_Context* ctx, oct_Uword size, oct_OAOObjectOption* out_arr) {
	oct_Uword i;
	CHECK(OCT_ALLOCOWNED(sizeof(oct_AOObjectOption) + sizeof(oct_OObjectOption) * size, (void**)&out_arr->ptr, "oct_AOObjectOption_createOwned"));
	out_arr->ptr->size = size;
	for(i = 0; i < size; ++i) {
		out_arr->ptr->data[i].variant = OCT_OOBJECTOPTION_NOTHING;
	}
	return oct_True;
}

oct_Bool oct_AOObjectOption_dtor(struct oct_Context* ctx, oct_BAOObjectOption self) {
	oct_Uword i;
	oct_Bool result = oct_True;
	for(i = 0; i < self.ptr->size; ++i) {
		if(self.ptr->data[i].variant == OCT_OOBJECTOPTION_OBJECT) {
			result = oct_Object_destroyOwned(ctx, self.ptr->data[i].object) && result;
		}
	}
	return result;
}

// The output is BObject because C does not have templates but the output should be safe to manually cast to the given protocol
oct_Bool oct_Object_as(oct_Context* ctx, oct_BSelf object, oct_BType selfType, oct_BProtocolBinding protocol, oct_BObject* out_casted) {
	oct_BHashtable table;
	oct_BHashtableKey key;
	oct_Any vtable;

	table.ptr = &protocol.ptr->implementations;
	CHECK(oct_BType_asHashtableKey(ctx, selfType, &key));
	CHECK(oct_Hashtable_borrow(ctx, table, key, &vtable));
	if(vtable.variant == OCT_ANY_NOTHING) {
		CHECK(oct_Context_setErrorWithCMessage(ctx, "No implementation of protocol found"));
		return oct_False;
	}
	out_casted->self = object;
	out_casted->vtable = (oct_ObjectVTable*)vtable.bobject.self.self;
	return oct_True;
}

oct_Bool oct_Object_destroyOwned(oct_Context* ctx, oct_OObject obj) {
	oct_BSelf bself;
	oct_Bool result;
	bself.self = obj.self.self;
	result = obj.vtable->functions.dtor(ctx, bself);
	return OCT_FREEOWNED(obj.self.self) && result;
}

// Object graph walk support code
// Cuckoo hash table for pointer translation and seen check

typedef struct PointerTranslationTableEntry {
    void* key;
    void* val;
} PointerTranslationTableEntry;

typedef struct PointerTranslationTable {
	oct_Uword capacity;
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
	return (oct_Uword)ptr;
}

static oct_Bool PointerTranslationTable_Create(oct_Context* ctx, oct_Uword initialCap, PointerTranslationTable* table) {
	oct_Uword byteSize;
    
	table->capacity = nextp2(initialCap);
	byteSize = table->capacity * sizeof(PointerTranslationTableEntry);
	CHECK(OCT_ALLOCOWNED(byteSize, (void**)&table->table, "PointerTranslationTable_Create"));
	memset(table->table, 0, byteSize);
    
	return oct_True;
}

static oct_Bool PointerTranslationTable_Destroy(oct_Context* ctx, PointerTranslationTable* table) {
	CHECK(OCT_FREEOWNED(table->table));
	table->capacity = 0;
	table->table = NULL;
	return oct_True;
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
		return oct_True;
	}
	*entry = tmp;

	i = hash2(hashPointer(entry->key)) & mask;
	tmp = table->table[i];
	table->table[i] = *entry;
	if(tmp.key == NULL || tmp.key == entry->key) {
		return oct_True;
	}
	*entry = tmp;

	i = hash3(hashPointer(entry->key)) & mask;
	tmp = table->table[i];
	table->table[i] = *entry;
	if(tmp.key == NULL || tmp.key == entry->key) {
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
				PointerTranslationTable_Destroy(ctx, &bigger);
				if(!PointerTranslationTable_Create(ctx, cap, &bigger)) {
					return oct_False;
				}
				i = 0;
			}
		}
	}
	PointerTranslationTable_Destroy(ctx, table);
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

static oct_Bool PointerTranslationTable_Get(PointerTranslationTable* table, void* key, void** val) {
	oct_Uword i, mask, ptrHash;

	mask = table->capacity - 1;
	ptrHash = hashPointer(key);

	i = hash1(ptrHash) & mask;
	if(table->table[i].key == key) {
		*val = table->table[i].val;
		return oct_True;
	}

	i = hash2(ptrHash) & mask;
	if(table->table[i].key == key) {
		*val = table->table[i].val;
		return oct_True;
	}

	i = hash3(ptrHash) & mask;
	if(table->table[i].key == key) {
		*val = table->table[i].val;
		return oct_True;
	}

	return oct_False;
}

// Util functions to find embedded pointers in an object and return them in an array

typedef struct FieldPointer {
	oct_Type* type;
	oct_Uword ptrKind;
	void* value;
	oct_Uword offset;
} FieldPointer;

typedef struct FieldPointerArray {
	oct_Uword size;
	oct_Uword capacity;
	FieldPointer* data;
} FieldPointerArray;

static oct_Bool FieldPointerArray_Create(oct_Context* ctx, FieldPointerArray* fpa, oct_Uword initialCap) {
	CHECK(OCT_ALLOCOWNED(sizeof(FieldPointer) * initialCap, (void**)&fpa->data, "FieldPointerArray_Create"));
	if(!fpa->data) {
		oct_Context_setErrorOOM(ctx);
		return oct_False;
	}
	fpa->capacity = initialCap;
	fpa->size = 0;
	return oct_True;
}

static oct_Bool FieldPointerArray_Destroy(oct_Context* ctx, FieldPointerArray* fpa) {
	CHECK(OCT_FREEOWNED(fpa->data));
	fpa->capacity = 0;
	fpa->data = NULL;
	fpa->size = 0;
	return oct_True;
}

static oct_Bool FieldPointerArray_Add(oct_Context* ctx, FieldPointerArray* fpa, FieldPointer fp) {
	oct_Uword newCap;
	FieldPointer* newArray;

	if(fpa->size == fpa->capacity) {
		newCap = fpa->capacity * 2;
		CHECK(OCT_ALLOCOWNED(sizeof(FieldPointer) * newCap, (void**)&newArray, "FieldPointerArray_Add"));
		if(!newArray) {
			oct_Context_setErrorOOM(ctx);
			return oct_False;
		}
		memcpy(newArray, fpa->data, fpa->capacity);
		CHECK(OCT_FREEOWNED(fpa->data));
		fpa->capacity = newCap;
		fpa->data = newArray;
	}

	fpa->data[fpa->size++] = fp;
	return oct_True;
}

typedef struct Pointer {
	void* ptr;
} Pointer;

typedef struct Variadic {
	oct_Uword variant;
	oct_U8 Union[];
} Variadic;

typedef struct Array {
	oct_Uword size;
	oct_U8 data[];
} Array;

static oct_Bool findEmbeddedPointers(oct_Context* ctx, oct_Type* type, void* object, FieldPointerArray* pointerArray, oct_Uword offset) {
	Pointer* ptr;
	Variadic* var;
	Array* arr;
	oct_Uword i;
	oct_AField* fields;
	FieldPointer fp;
	oct_Uword elemSize;

	switch(type->variant) {
	case OCT_TYPE_PROTOTYPE:
	case OCT_TYPE_PROTOCOL:
		oct_Context_setErrorWithCMessage(ctx, "Runtime internal error: attempt to find pointers in Prototype or Protocol instance");
		return oct_False;
	case OCT_TYPE_POINTER:
		// This works for protocol instance pointers as well because their first member is the object pointer
		ptr = (Pointer*)object;
		fp.value = ptr->ptr;
		fp.offset = offset;
		fp.type = type->pointerType.type.ptr;
		fp.ptrKind = type->pointerType.kind;
		return FieldPointerArray_Add(ctx, pointerArray, fp);
	case OCT_TYPE_VARIADIC:
		var = (Variadic*)object;
		offset += sizeof(oct_Uword);
		return findEmbeddedPointers(ctx, type->variadicType.types.ptr->data[var->variant].ptr, (void*)&var->Union[0], pointerArray, offset);
	case OCT_TYPE_STRUCT:
        fields = type->structType.fields.ptr;
        for(i = 0; i < fields->size; ++i) {
            if(!findEmbeddedPointers(ctx, fields->data[i].type.ptr, (void*)(((char*)object) + fields->data[i].offset), pointerArray, offset + fields->data[i].offset)) {
                return oct_False;
            }
        }
        return oct_True;
	case OCT_TYPE_FIXED_SIZE_ARRAY:
		oct_Type_sizeOf(ctx, type->fixedSizeArray.elementType, &elemSize);
		for(i = 0; i < type->fixedSizeArray.size; ++i) {
			offset += elemSize * i;
			if(!findEmbeddedPointers(ctx, type->fixedSizeArray.elementType.ptr, object, pointerArray, offset)) {
				return oct_False;
			}
		}
		return oct_True;
	case OCT_TYPE_ARRAY:
		arr = (Array*)object;
		oct_Type_sizeOf(ctx, type->arrayType.elementType, &elemSize);
		for(i = 0; i < arr->size; ++i) {
			offset += elemSize * i;
			if(!findEmbeddedPointers(ctx, type->arrayType.elementType.ptr, &arr->data[0], pointerArray, offset)) {
				return oct_False;
			}
		}
		return oct_True;
	}
	oct_Context_setErrorWithCMessage(ctx, "Runtime internal error: Fell through switch statement in findEmbeddedPointers");
	return oct_False;
}

// Stack used to store call frames on the heap to prevent blowing the C stack when traversing large graphs

typedef struct FrameStackEntry {
	oct_OSelf original;
	oct_OSelf updated;
	oct_BType type;
	FieldPointerArray fieldPointers;
	oct_Uword fieldIndex;
} FrameStackEntry;

typedef struct FrameStack {
    oct_Uword capacity;
    oct_Uword top;
    FrameStackEntry* stack;
} FrameStack;

static oct_Bool FrameStack_Create(oct_Context* ctx, FrameStack* stack, oct_Uword initialCap) {
    stack->capacity = initialCap;
    stack->top = 0;
	CHECK(OCT_ALLOCOWNED(sizeof(FrameStackEntry) * initialCap, (void**)&stack->stack, "FrameStack_Create"));
	if(!stack->stack) {
		oct_Context_setErrorOOM(ctx);
		return oct_False;
	}
    return oct_True;
}

static oct_Bool FrameStack_Destroy(oct_Context* ctx, FrameStack* stack) {
	oct_Uword i;
	oct_Bool result = oct_True;

	for(i = 0; i < stack->top; ++i) {
        result = FieldPointerArray_Destroy(ctx, &stack->stack[i].fieldPointers) && result;
	}
    result = OCT_FREEOWNED(stack->stack) && result;
	stack->capacity = 0;
	stack->top = 0;
	stack->stack = NULL;
	return result;
}

static oct_Bool FrameStack_Push(oct_Context* ctx, FrameStack* stack, FrameStackEntry* entry) {
	FrameStack bigger;
    
    if(stack->capacity == stack->top) {
		bigger = (*stack);
		bigger.capacity *= 2;
		CHECK(OCT_ALLOCOWNED(sizeof(FrameStackEntry) * bigger.capacity, (void**)&bigger.stack, "FrameStack_Push"));
		if(!bigger.stack) {
			oct_Context_setErrorOOM(ctx);
			return oct_False;
		}
		memcpy(bigger.stack, stack->stack, stack->capacity * sizeof(FrameStackEntry));
		FrameStack_Destroy(ctx, stack);
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

//static oct_Bool copyObjectOwned(oct_Context* ctx, oct_Type* type, void* object, void** copy) {
//	oct_Uword size = 0;
//	oct_BType bt;
//
//	bt.ptr = type;
//    
//	switch(type->variant) {
//	case OCT_TYPE_PROTOTYPE:
//		{
//			oct_Context_setErrorWithCMessage(ctx, "Runtime internal error: Trying to copy object of type Prototype");
//			return oct_False;
//		}
//	case OCT_TYPE_PROTOCOL:
//		{
//			// TODO: this is probably ok? For the "protocol objects" expression problem solution
//			oct_Context_setErrorWithCMessage(ctx, "Runtime internal error: Trying to copy object of type Protocol");
//			return oct_False;
//		}
//	case OCT_TYPE_POINTER:
//		{
//			oct_Context_setErrorWithCMessage(ctx, "Runtime internal error: Trying to copy object of type Pointer");
//			return oct_False;
//		}
//	case OCT_TYPE_VARIADIC:
//	case OCT_TYPE_STRUCT:
//	case OCT_TYPE_FIXED_SIZE_ARRAY:
//		oct_sizeOf(ctx, bt, &size);
//		break;
//	case OCT_TYPE_ARRAY:
//		oct_sizeOf(ctx, type->arrayType.elementType, &size);
//		size = sizeof(oct_Uword) + ( ((Array*)object)->size * size );
//		break;
//	}
//
//	if(size == 0) {
//		oct_Context_setErrorWithCMessage(ctx, "Runtime internal error: Fell through copyObjectOwned switch statement");
//		return oct_False;
//	}
//	else {
//		if(!OCT_ALLOCOWNED(size, copy)) {
//			return oct_False;
//		}
//		memcpy(*copy, object, size);
//		return oct_True;
//	}
//}

#undef CHECK
#define CHECK(X) if(!X) goto error;

oct_Bool oct_Object_preWalk(oct_Context* ctx, oct_OSelf root, oct_BType rootType, oct_PrewalkFn fn) {
	PointerTranslationTable ptt;
	FrameStack stack;
	FrameStackEntry currentFrame;
	oct_Bool result;
    oct_OSelf object;
    oct_Uword i;
	oct_Type* type;
    void** embeddedPtr;
	void* dummy;

	ptt.table = NULL;
	stack.stack = NULL;
	result = oct_True;

	// TODO: move these instances to the context instead so that they are not re-created during every graph traversal?
	CHECK(FrameStack_Create(ctx, &stack, 500));
	CHECK(PointerTranslationTable_Create(ctx, 1000, &ptt));

	currentFrame.original = root;
	currentFrame.type = rootType;
	currentFrame.fieldIndex = 0;
    CHECK(FieldPointerArray_Create(ctx, &currentFrame.fieldPointers, 10));
    CHECK(findEmbeddedPointers(ctx, currentFrame.type.ptr, currentFrame.original.self, &currentFrame.fieldPointers, 0));    
	CHECK(fn(ctx, currentFrame.original, currentFrame.type, &currentFrame.updated));
    CHECK(PointerTranslationTable_Put(ctx, &ptt, currentFrame.original.self, currentFrame.updated.self));

    while (oct_True) {
        if(currentFrame.fieldIndex < currentFrame.fieldPointers.size) {
            object.self = currentFrame.fieldPointers.data[currentFrame.fieldIndex].value;
			type = currentFrame.fieldPointers.data[currentFrame.fieldIndex].type;
            ++currentFrame.fieldIndex;
            if(PointerTranslationTable_Get(&ptt, object.self, &dummy) == oct_False) {
                // Go depth first so we can fix the child pointers in the same pass
                CHECK(FrameStack_Push(ctx, &stack, &currentFrame));
                currentFrame.original = object;
                currentFrame.type.ptr = type;
                currentFrame.fieldIndex = 0;
			    CHECK(FieldPointerArray_Create(ctx, &currentFrame.fieldPointers, 10));
				CHECK(findEmbeddedPointers(ctx, currentFrame.type.ptr, currentFrame.original.self, &currentFrame.fieldPointers, 0));    
				CHECK(fn(ctx, currentFrame.original, currentFrame.type, &currentFrame.updated));
			    CHECK(PointerTranslationTable_Put(ctx, &ptt, currentFrame.original.self, currentFrame.updated.self));
            }
        }
        else {
            // All children traversed. Fix up the pointers.
            for(i = 0; i < currentFrame.fieldPointers.size; ++i) {
				if(currentFrame.updated.self != NULL) {
					embeddedPtr = (void**)(((char*)currentFrame.updated.self) + currentFrame.fieldPointers.data[i].offset);
					PointerTranslationTable_Get(&ptt, *embeddedPtr, embeddedPtr);
				}
            }
            CHECK(FieldPointerArray_Destroy(ctx, &currentFrame.fieldPointers));
            // Pop previous frame off stack
            if(FrameStack_Pop(&stack, &currentFrame) == oct_False) {
                // All done!
                goto end;
            }
        }
    }

error:
	result = oct_False;
    // TODO: probably need a way to clean up here?
end:
	if(stack.stack) {
		FrameStack_Destroy(ctx, &stack);
	}
	if(ptt.table) {
		PointerTranslationTable_Destroy(ctx, &ptt);
	}
	return result;
}



