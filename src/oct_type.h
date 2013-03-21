#ifndef oct_type
#define oct_type

#include "oct_primitives.h"
#include "oct_prototype.h"
#include "oct_variadictype.h"
#include "oct_structtype.h"
#include "oct_arraytype.h"
#include "oct_pointertype.h"
#include "oct_protocoltype.h"

#define OCT_TYPE_PROTOTYPE 0
#define OCT_TYPE_VARIADIC 1
#define OCT_TYPE_STRUCT 2
#define OCT_TYPE_ARRAY 3
#define OCT_TYPE_FIXED_SIZE_ARRAY 4
#define OCT_TYPE_POINTER 5
#define OCT_TYPE_PROTOCOL 6

typedef struct oct_Type {
	oct_Uword variant; // OCT_TYPE_*
	union {
		oct_ProtoType protoType;
		oct_VariadicType variadicType;
		oct_StructType structType;
		oct_ArrayType arrayType;
		oct_FixedSizeArrayType fixedSizeArray;
		oct_PointerType pointerType;
		oct_ProtocolType protocolType;
	};
} oct_Type;

struct oct_Context;

// For Type, the vtables need to be added manually because of a circular dependency with protocols
oct_Bool _oct_Type_VTableInit(struct oct_Context* ctx);
oct_Bool _oct_Type_init(struct oct_Context* ctx);

oct_Bool oct_Type_dtor(struct oct_Context* ctx, oct_Type* self);

// Public

// Does a deep copy of an object graph and returns an owned copy
// Will fail if the given graph contains any non-copyable objects.
//oct_Bool oct_Type_deepCopyGraphOwned(struct oct_Context* ctx, oct_BObject root, oct_OObject* out_ownedCopy);
// Does a deep copy of an object graph and returns a managed copy
// Will fail if the given graph contains any non-copyable objects.
//oct_Bool oct_Type_deepCopyGraphManaged(struct oct_Context* ctx, oct_BObject root, oct_MObject* out_managedCopy);

#endif
