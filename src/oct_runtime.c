#include "oct_runtime.h"
#include "oct_primitives.h"
#include "oct_type.h"
#include "oct_type_pointers.h"
#include "oct_arraytype.h"
#include "oct_pointertype.h"
#include "oct_u8array.h"
#include "oct_list.h"
#include "oct_string.h"
#include "oct_symbol.h"
#include "oct_namespace.h"
#include "oct_object.h"
#include "oct_context.h"
#include "oct_reader.h"
#include "oct_primitive_pointers.h"
#include "oct_initTarget.h"
#include "oct_charstream.h"
#include "oct_stringstream.h"
#include "oct_copyable.h"
#include "oct_exchangeheap.h"
#include "oct_printable.h"
#include "oct_primitives_functions.h"
#include "oct_vector.h"
#include "oct_seq.h"
#include "oct_seqable.h"

#include <stdlib.h>
#include <memory.h>

static void alloc_builtInTypes(oct_Runtime* rt) {
	oct_Uword iters = sizeof(oct_BuiltInTypes) / sizeof(oct_BType);
	oct_Uword i;
	oct_BType* place;
	oct_Context* ctx = NULL; // for the alloc macro

	for(i = 0; i < iters; ++i) {
		char* dummy = (char*)(&rt->builtInTypes);
		dummy += sizeof(oct_BType) * i;
		place = (oct_BType*)dummy;
		OCT_ALLOCOWNED(sizeof(oct_Type), (void**)&place->ptr, "alloc_builtInTypes");
	}
}

static void dealloc_buintInTypes(oct_Context* ctx) {
	oct_Uword iters = sizeof(oct_BuiltInTypes) / sizeof(oct_BType);
	oct_Uword i;
	oct_BType* place;
	oct_Runtime* rt = ctx->rt;

	for(i = 0; i < iters; ++i) {
		char* dummy = (char*)(&rt->builtInTypes);
		dummy += sizeof(oct_BType) * i;
		place = (oct_BType*)dummy;
		oct_Type_dtor(ctx, place->ptr);
		OCT_FREEOWNED(place->ptr);
	}
}

static void dealloc_builtInProtocols(oct_Context* ctx) {
	oct_Uword iters = sizeof(oct_BuiltInProtocols) / sizeof(oct_BProtocolBinding);
	oct_Uword i;
	oct_BProtocolBinding* place;
	oct_Runtime* rt = ctx->rt;
	oct_BSelf self;

	for(i = 0; i < iters; ++i) {
		char* dummy = (char*)(&rt->builtInProtocols);
		dummy += sizeof(oct_BProtocolBinding) * i;
		place = (oct_BProtocolBinding*)dummy;
		self.self = &place->ptr->implementations;
		oct_Hashtable_dtor(ctx, self);
		OCT_FREEOWNED(place->ptr);
	}
}

static void dealloc_builtInFunctions(oct_Context* ctx) {
	oct_Uword iters = sizeof(oct_BuiltInFunctions) / sizeof(oct_BFunction);
	oct_Uword i;
	oct_BFunction* place;
	oct_Runtime* rt = ctx->rt;
	oct_BSelf self;

	for(i = 0; i < iters; ++i) {
		char* dummy = (char*)(&rt->functions);
		dummy += sizeof(oct_BFunction) * i;
		place = (oct_BFunction*)dummy;
		self.self = place->ptr;
		oct_Function_dtor(ctx, self);
		OCT_FREEOWNED(place->ptr);
	}
}

static void dealloc_builtInVTables(oct_Runtime* rt) {
	oct_Uword iters = sizeof(oct_BuiltInVTables) / sizeof(oct_BVTable);
	oct_Uword i;
	oct_BVTable* place;
	oct_Context* ctx = NULL; // for the free macro

	for(i = 0; i < iters; ++i) {
		char* dummy = (char*)(&rt->vtables);
		dummy += sizeof(oct_BVTable) * i;
		place = (oct_BVTable*)dummy;
		OCT_FREEOWNED(place->ptr);
	}
}

#define CHECK(X) if(!X) return oct_False;

static oct_Bool bind_type(oct_Context* ctx, oct_BNamespace ns, const char* name, oct_BType type) {
	oct_OString str;
	oct_OSymbol sym;
	oct_BSymbol bSym;
	oct_HashtableKeyOption key;
    oct_Any value;
	
	CHECK(oct_String_createOwnedFromCString(ctx, name, &str));
	CHECK(oct_Symbol_createOwned(ctx, str, &sym));
	bSym.ptr = sym.ptr;
	CHECK(oct_Symbol_asHashtableKey(ctx, bSym, &key.borrowed));
	key.variant = OCT_HASHTABLEKEYOPTION_OWNED;
    value.variant = OCT_ANY_BOBJECT; // TODO: change this to OOBJECT and remove the explicit deallocation of builtins?
	CHECK(oct_Type_asObject(ctx, type, &value.bobject));
	return oct_Namespace_bind(ctx, ns, key, value);
}

struct oct_Runtime* oct_Runtime_create(const char** out_error) {
	oct_Runtime* rt;
	oct_Context* ctx = NULL;
	oct_OString str;
	oct_BNamespace octarine;
	oct_BHashtable namespaceTable;

	oct_initJITTarget();

	OCT_ALLOCOWNED(sizeof(oct_Runtime), (void**)&rt, "Runtime");
	if(!rt) {
		(*out_error) = "Out of memory";
		return NULL;
	}
	memset(rt, 0, sizeof(oct_Runtime));

	// *** 1. Create main thread context

	OCT_ALLOCOWNED(sizeof(oct_Context), (void**)&ctx, "Main context");
	if(!ctx) {
		(*out_error) = "Out of memory";
		return NULL;
	}
	memset(ctx, 0, sizeof(oct_Context));
	ctx->rt = rt;
	OCT_ALLOCOWNED(sizeof(oct_Reader), (void**)&ctx->reader, "Main reader");
    oct_TLSInit(&rt->currentContext);
	oct_TLSSet(rt->currentContext, ctx);

	// Allocate memory for all built in types up front to resolve circular dependencies.
	alloc_builtInTypes(rt);

	// *** 1.5 Create the built in protocols so that type init functions may add themselves
	
	OCT_ALLOCOWNED(sizeof(oct_VTable) + (sizeof(void*) * 2), (void**)&rt->vtables.NothingAsHashtableKey.ptr, "NothingAsHashtableKey");
	if(!rt->vtables.NothingAsHashtableKey.ptr) {
		(*out_error) = "Out of memory";
		return NULL;
	}
	rt->vtables.NothingAsHashtableKey.ptr->objectType = rt->builtInTypes.Nothing;

	_oct_Object_initProtocol(ctx);
	_oct_EqComparable_initProtocol(ctx);
	_oct_Hashable_initProtocol(ctx);
	_oct_Hashtable_initProtocol(ctx);
	_oct_Charstream_initProtocol(ctx);
	_oct_Copyable_initProtocol(ctx);
	_oct_Printable_initProtocol(ctx);
	_oct_Seq_initProtocol(ctx);
	_oct_Seqable_initProtocol(ctx);

	// *** 1.5.1 Manually fix up the dependencies for oct_Protocol_addImplementation
	_oct_VTable_init(ctx);
	_oct_Type_VTableInit(ctx);
	_oct_Nothing_VTableInit(ctx);

	// *** 2. Initialize all the built in types
	//        I.e. those needed by the reader and compiler and any dependencies
	//        of those types.

	// Initialize all built in types

	_oct_Primitives_init(ctx);
	_oct_Hashable_init(ctx);
	_oct_EqComparable_init(ctx);
	_oct_Hashtable_init(ctx);
	_oct_Type_init(ctx);
	_oct_Protocol_init(ctx);
	_oct_Function_init(ctx);
	_oct_Object_init(ctx);
	_oct_String_init(ctx);
	_oct_Array_init(ctx);
	_oct_AChar_init(ctx);
	_oct_AU8_init(ctx);
	_oct_List_init(ctx);
	_oct_Nothing_init(ctx);
	_oct_PointerType_init(ctx);
	_oct_StructType_init(ctx);
	_oct_Field_init(ctx);
	_oct_Symbol_init(ctx);
	_oct_ProtoType_init(ctx);
	_oct_VariadicType_init(ctx);
	_oct_Error_init(ctx);
	_oct_Any_init(ctx);
	_oct_Namespace_init(ctx);
	_oct_Charstream_init(ctx);
	_oct_Stringstream_init(ctx);
	_oct_Copyable_init(ctx);
	_oct_Printable_init(ctx);
	_oct_Vector_init(ctx);
	_oct_Seq_init(ctx);
	_oct_Seqable_init(ctx);
    _oct_Primitives_initProtocols(ctx);

	// Init built in errors

	_oct_Error_initBuiltInErrors(ctx);

	// Construct main reader

	oct_Reader_ctor(ctx, ctx->reader);

	// *** 3. Create octarine namespace.

	namespaceTable.ptr = &rt->namespaces;
	oct_Hashtable_ctor(ctx, namespaceTable, 100);
	oct_String_createOwnedFromCString(ctx, "octarine", &str);
	oct_Namespace_create(ctx, str, &octarine);
	ctx->ns = octarine.ptr;

	// *** 4. Register all built in types and functions in octarine namespace.
	// Primitives
	bind_type(ctx, octarine, "U8", rt->builtInTypes.U8);
	bind_type(ctx, octarine, "I8", rt->builtInTypes.I8);
	bind_type(ctx, octarine, "U16", rt->builtInTypes.U16);
	bind_type(ctx, octarine, "I16", rt->builtInTypes.I16);
	bind_type(ctx, octarine, "U32", rt->builtInTypes.U32);
	bind_type(ctx, octarine, "I32", rt->builtInTypes.I32);
	bind_type(ctx, octarine, "U64", rt->builtInTypes.U64);
	bind_type(ctx, octarine, "I64", rt->builtInTypes.I64);
	bind_type(ctx, octarine, "F32", rt->builtInTypes.F32);
	bind_type(ctx, octarine, "F64", rt->builtInTypes.F64);
	bind_type(ctx, octarine, "Uword", rt->builtInTypes.Uword);
	bind_type(ctx, octarine, "Word", rt->builtInTypes.Word);
	bind_type(ctx, octarine, "Char", rt->builtInTypes.Char);
	bind_type(ctx, octarine, "Bool", rt->builtInTypes.Bool);
	// Hashtable
	bind_type(ctx, octarine, "HashtableKey", rt->builtInTypes.HashtableKey);
	bind_type(ctx, octarine, "~HashtableKey", rt->builtInTypes.OHashtableKey);
	bind_type(ctx, octarine, "&HashtableKey", rt->builtInTypes.BHashtableKey);
	bind_type(ctx, octarine, "HashtableEntry", rt->builtInTypes.HashtableEntry);
	bind_type(ctx, octarine, "+HashtableEntry", rt->builtInTypes.AHashtableEntry);
	bind_type(ctx, octarine, "~+HashtableEntry", rt->builtInTypes.OAHashtableEntry);
	bind_type(ctx, octarine, "Hashtable", rt->builtInTypes.Hashtable);
	bind_type(ctx, octarine, "&Hashtable", rt->builtInTypes.BHashtable);
	// Type
	bind_type(ctx, octarine, "Type", rt->builtInTypes.Type);
	bind_type(ctx, octarine, "&Type", rt->builtInTypes.BType);
	bind_type(ctx, octarine, "+&Type", rt->builtInTypes.ABType);
	bind_type(ctx, octarine, "~+&Type", rt->builtInTypes.OABType);
	// Protocol
	bind_type(ctx, octarine, "Protocol", rt->builtInTypes.Protocol);
	bind_type(ctx, octarine, "VTable", rt->builtInTypes.VTable);
	bind_type(ctx, octarine, "&VTable", rt->builtInTypes.BVTable);
	bind_type(ctx, octarine, "ProtocolBinding", rt->builtInTypes.ProtocolBinding);
	bind_type(ctx, octarine, "&ProtocolBinding", rt->builtInTypes.BProtocolBinding);
	// Function
	bind_type(ctx, octarine, "Function", rt->builtInTypes.Function);
	bind_type(ctx, octarine, "&Function", rt->builtInTypes.BFunction);
	bind_type(ctx, octarine, "+&Function", rt->builtInTypes.ABFunction);
	bind_type(ctx, octarine, "~+&Function", rt->builtInTypes.OABFunction);
	// Object
	bind_type(ctx, octarine, "Object", rt->builtInTypes.Object);
	bind_type(ctx, octarine, "~Object", rt->builtInTypes.OObject);
	bind_type(ctx, octarine, "&Object", rt->builtInTypes.BObject);
	bind_type(ctx, octarine, "ObjectOption~", rt->builtInTypes.OObjectOption);
	bind_type(ctx, octarine, "ObjectOption&", rt->builtInTypes.BObjectOption);
	// String
	bind_type(ctx, octarine, "String", rt->builtInTypes.String);
	bind_type(ctx, octarine, "~String", rt->builtInTypes.OString);
	bind_type(ctx, octarine, "&String", rt->builtInTypes.BString);
	// Array
	bind_type(ctx, octarine, "Array", rt->builtInTypes.Array);
	bind_type(ctx, octarine, "FixedSizeArray", rt->builtInTypes.FixedSizeArray);
	// AChar
	bind_type(ctx, octarine, "+Char", rt->builtInTypes.AChar);
	bind_type(ctx, octarine, "~+Char", rt->builtInTypes.OAChar);
	// AU8
	bind_type(ctx, octarine, "+U8", rt->builtInTypes.AU8);
	bind_type(ctx, octarine, "~+U8", rt->builtInTypes.OAU8);
	// List
	bind_type(ctx, octarine, "List", rt->builtInTypes.List);
	bind_type(ctx, octarine, "~List", rt->builtInTypes.OList);
	bind_type(ctx, octarine, "&List", rt->builtInTypes.BList);
	// Nothing
	bind_type(ctx, octarine, "Nothing", rt->builtInTypes.Nothing);
	bind_type(ctx, octarine, "&Nothing", rt->builtInTypes.BNothing);
	// Pointer
	bind_type(ctx, octarine, "Pointer", rt->builtInTypes.Pointer);
	// Struct
	bind_type(ctx, octarine, "Struct", rt->builtInTypes.Struct);
	// Field
	bind_type(ctx, octarine, "Field", rt->builtInTypes.Field);
	bind_type(ctx, octarine, "+Field", rt->builtInTypes.AField);
	bind_type(ctx, octarine, "~+Field", rt->builtInTypes.OAField);
	// Symbol
	bind_type(ctx, octarine, "Symbol", rt->builtInTypes.Symbol);
	bind_type(ctx, octarine, "~Symbol", rt->builtInTypes.OSymbol);
	bind_type(ctx, octarine, "&Symbol", rt->builtInTypes.BSymbol);
	bind_type(ctx, octarine, "SymbolOption~", rt->builtInTypes.OSymbolOption);
	// Prototype
	bind_type(ctx, octarine, "Prototype", rt->builtInTypes.Prototype);
	// Variadic
	bind_type(ctx, octarine, "Variadic", rt->builtInTypes.Variadic);
	// Error
	bind_type(ctx, octarine, "Error", rt->builtInTypes.Error);
	bind_type(ctx, octarine, "~Error", rt->builtInTypes.OError);
	bind_type(ctx, octarine, "ErrorOption~", rt->builtInTypes.OErrorOption);
	// Any
	bind_type(ctx, octarine, "Any", rt->builtInTypes.Any);
	// Namespace
	bind_type(ctx, octarine, "Namespace", rt->builtInTypes.Namespace);
	bind_type(ctx, octarine, "&Namespace", rt->builtInTypes.BNamespace);
	bind_type(ctx, octarine, "NamespaceOption", rt->builtInTypes.NamespaceOption);

	// *** 5. Profit?
	
	return rt;
}

oct_Bool oct_Runtime_destroy(oct_Runtime* rt, const char** out_error) {
	oct_Context* ctx = oct_Runtime_currentContext(rt);
	oct_BSelf self;
	_oct_Error_destroyBuiltInErrors(ctx);
	self.self = &rt->namespaces;
	oct_Hashtable_dtor(ctx, self);
	dealloc_builtInFunctions(ctx);
	dealloc_builtInProtocols(ctx);
	dealloc_builtInVTables(rt);
	dealloc_buintInTypes(ctx);
	self.self = ctx->reader;
	oct_Reader_dtor(ctx, self);
	OCT_FREEOWNED(ctx->reader);
	OCT_FREEOWNED(ctx);
	OCT_FREEOWNED(rt);
	return oct_True;
}

oct_Context* oct_Runtime_currentContext(oct_Runtime* rt) {
	return (oct_Context*)oct_TLSGet(rt->currentContext);
}
