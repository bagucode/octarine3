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
	fpa->data = (FieldPointer*)malloc(sizeof(FieldPointer) * initialCap);
	if(!fpa->data) {
		oct_Context_setErrorOOM(ctx);
		return oct_False;
	}
	fpa->capacity = initialCap;
	fpa->size = 0;
	return oct_True;
}

static void FieldPointerArray_Destroy(FieldPointerArray* fpa) {
	free(fpa->data);
	fpa->capacity = 0;
	fpa->data = NULL;
	fpa->size = 0;
}

static oct_Bool FieldPointerArray_Add(oct_Context* ctx, FieldPointerArray* fpa, FieldPointer fp) {
	oct_Uword newCap;
	FieldPointer* newArray;

	if(fpa->size == fpa->capacity) {
		newCap = fpa->capacity * 2;
		newArray = (FieldPointer*)malloc(sizeof(FieldPointer) * newCap);
		if(!newArray) {
			oct_Context_setErrorOOM(ctx);
			return oct_False;
		}
		memcpy(newArray, fpa->data, fpa->capacity);
		free(fpa->data);
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

static oct_Uword getTypeSize(oct_Type* type) {
	switch(type->variant) {
	case OCT_TYPE_PROTO:
		return sizeof(void*);
	case OCT_TYPE_INTERFACE:
		return sizeof(void*) * 2;
	case OCT_TYPE_POINTER:
		return sizeof(void*);
	case OCT_TYPE_VARIADIC:
		return type->variadicType.size;
	case OCT_TYPE_STRUCT:
		return type->structType.size;
	case OCT_TYPE_FIXED_SIZE_ARRAY:
		return type->fixedSizeArray.size * getTypeSize(type->fixedSizeArray.elementType.ptr);
	case OCT_TYPE_ARRAY:
		return 0; // incorrect, but the actual size is unknown without looking at the instance
	}
	return 0;
}

static oct_Bool findEmbeddedPointers(oct_Context* ctx, oct_Type* type, void* object, FieldPointerArray* pointerArray, oct_Uword offset) {
	Pointer* ptr;
	Variadic* var;
	Array* arr;
	oct_Uword i;
	oct_AField* fields;
	FieldPointer fp;
	oct_Uword elemSize;

	switch(type->variant) {
	case OCT_TYPE_PROTO:
	case OCT_TYPE_INTERFACE:
		// TODO: it's probably ok to do this to an interface object.
		oct_Context_setErrorWithCMessage(ctx, "Runtime internal error: attempt to find pointers in Prototype or Interface instance");
		return oct_False;
	case OCT_TYPE_POINTER:
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
		elemSize = getTypeSize(type->fixedSizeArray.elementType.ptr);
		for(i = 0; i < type->fixedSizeArray.size; ++i) {
			offset += elemSize * i;
			if(!findEmbeddedPointers(ctx, type->fixedSizeArray.elementType.ptr, object, pointerArray, offset)) {
				return oct_False;
			}
		}
		return oct_True;
	case OCT_TYPE_ARRAY:
		arr = (Array*)object;
		elemSize = getTypeSize(type->arrayType.elementType.ptr);
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
	void* object;
	void* copy;
	oct_Type* type;
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
    stack->stack = (FrameStackEntry*)malloc(sizeof(FrameStackEntry) * initialCap);
	if(!stack->stack) {
		oct_Context_setErrorOOM(ctx);
		return oct_False;
	}
    return oct_True;
}

static void FrameStack_Destroy(FrameStack* stack) {
	oct_Uword i;

	for(i = 0; i < stack->top; ++i) {
        FieldPointerArray_Destroy(&stack->stack[i].fieldPointers);
	}
    free(stack->stack);
	stack->capacity = 0;
	stack->top = 0;
	stack->stack = NULL;
}

static oct_Bool FrameStack_Push(oct_Context* ctx, FrameStack* stack, FrameStackEntry* entry) {
	FrameStack bigger;
    
    if(stack->capacity == stack->top) {
		bigger = (*stack);
		bigger.capacity *= 2;
		bigger.stack = (FrameStackEntry*)malloc(sizeof(FrameStackEntry) * bigger.capacity);
		if(!bigger.stack) {
			oct_Context_setErrorOOM(ctx);
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

static oct_Bool copyObjectOwned(oct_Context* ctx, oct_Type* type, void* object, void** copy) {
	oct_Uword size = 0;

	switch(type->variant) {
	case OCT_TYPE_PROTO:
		{
			oct_Context_setErrorWithCMessage(ctx, "Runtime internal error: Trying to copy object of type Prototype");
			return oct_False;
		}
	case OCT_TYPE_INTERFACE:
		{
			// TODO: this is probably ok? For the "interface objects" expression problem solution
			oct_Context_setErrorWithCMessage(ctx, "Runtime internal error: Trying to copy object of type Interface");
			return oct_False;
		}
	case OCT_TYPE_POINTER:
		{
			oct_Context_setErrorWithCMessage(ctx, "Runtime internal error: Trying to copy object of type Pointer");
			return oct_False;
		}
	case OCT_TYPE_VARIADIC:
	case OCT_TYPE_STRUCT:
	case OCT_TYPE_FIXED_SIZE_ARRAY:
		size = getTypeSize(type);
		break;
	case OCT_TYPE_ARRAY:
		size = sizeof(oct_Uword) + ( ((Array*)object)->size * getTypeSize(type->arrayType.elementType.ptr) );
		break;
	}

	if(size == 0) {
		oct_Context_setErrorWithCMessage(ctx, "Runtime internal error: Fell through copyObjectOwned switch statement");
		return oct_False;
	}
	else {
		if(!oct_ExchangeHeap_alloc(ctx, size, copy)) {
			return oct_False;
		}
		memcpy(*copy, object, size);
		return oct_True;
	}
}

#define CHECK(X) if(!X) goto error;

// Does a deep copy of an object graph and returns an owned copy
// Will fail if the given graph contains any non-copyable objects.
oct_Bool oct_Type_deepCopyGraphOwned(struct oct_Context* ctx, oct_Any root, oct_Any* out_ownedCopy) {
	PointerTranslationTable ptt;
	FrameStack stack;
	FrameStackEntry currentFrame;
	oct_Bool result;
	oct_BType bt;
    void* object;
    oct_Type* type;
    oct_Uword i;
    void** embeddedPtr;

	ptt.table = NULL;
	stack.stack = NULL;
	result = oct_True;

	// TODO: move these instances to the context instead so that they are not re-created during every graph copy
	CHECK(FrameStack_Create(ctx, &stack, 500));
	CHECK(PointerTranslationTable_Create(ctx, 1000, &ptt));

    CHECK(oct_Any_getPtr(ctx, root, &currentFrame.object));
	CHECK(oct_Any_getType(ctx, root, &bt));
	currentFrame.type = bt.ptr;
	currentFrame.fieldIndex = 0;
    CHECK(copyObjectOwned(ctx, currentFrame.type, currentFrame.object, &currentFrame.copy));
    CHECK(PointerTranslationTable_Put(ctx, &ptt, currentFrame.object, currentFrame.copy));
    CHECK(FieldPointerArray_Create(ctx, &currentFrame.fieldPointers, 10));
    CHECK(findEmbeddedPointers(ctx, currentFrame.type, currentFrame.object, &currentFrame.fieldPointers, 0));    

    // TODO: check that the type is actually deeply copyable? What could prevent it from being so?
	// The presence of a destructor might inhibit copying becuase the destructor could be used to
	// release or clean up some global resource and that could then be done more than once if the
	// responsible object is copied.

    while (oct_True) {
        if(currentFrame.fieldIndex < currentFrame.fieldPointers.size) {
            object = currentFrame.fieldPointers.data[currentFrame.fieldIndex].value;
            type = currentFrame.fieldPointers.data[currentFrame.fieldIndex].type;
            ++currentFrame.fieldIndex;
            if(PointerTranslationTable_Get(&ptt, object) == NULL) {
                // Go depth first so we can fix the child pointers in the same pass
                CHECK(FrameStack_Push(ctx, &stack, &currentFrame));
                currentFrame.object = object;
                currentFrame.type = type;
                currentFrame.fieldIndex = 0;
                CHECK(copyObjectOwned(ctx, currentFrame.type, currentFrame.object, &currentFrame.copy));
                CHECK(PointerTranslationTable_Put(ctx, &ptt, currentFrame.object, currentFrame.copy));
                CHECK(FieldPointerArray_Create(ctx, &currentFrame.fieldPointers, 10));
                CHECK(findEmbeddedPointers(ctx, currentFrame.type, currentFrame.object, &currentFrame.fieldPointers, 0));    
            }
        }
        else {
            // All children copied. Fix up the pointers.
            for(i = 0; i < currentFrame.fieldPointers.size; ++i) {
                embeddedPtr = (void**)(((char*)currentFrame.copy) + currentFrame.fieldPointers.data[i].offset);
                *embeddedPtr = PointerTranslationTable_Get(&ptt, currentFrame.fieldPointers.data[i].value);
            }
            FieldPointerArray_Destroy(&currentFrame.fieldPointers);
            // Pop previous frame off stack
            if(FrameStack_Pop(&stack, &currentFrame) == oct_False) {
                // All done!
                goto end;
            }
        }
    }

error:
	result = oct_False;
end:
	if(stack.stack) {
		FrameStack_Destroy(&stack);
	}
	if(ptt.table) {
        // TODO: release memory of any copies here if result is oct_False?
		PointerTranslationTable_Destroy(&ptt);
	}
	return result;
}





















