#include "oct_runtime.h"
#include "oct_primitives.h"
#include "oct_type.h"
#include "oct_type_pointers.h"
#include "oct_arraytype.h"
#include "oct_pointertype.h"
#include "oct_u8array.h"
#include "oct_string.h"
#include "oct_list.h"
#include "oct_string.h"
#include "oct_symbol.h"
#include "oct_namespace.h"
#include "oct_object.h"
#include "oct_context.h"
#include "oct_reader.h"
#include "oct_primitive_pointers.h"
#include "oct_initTarget.h"

#include <stdlib.h>

static void alloc_builtInTypes(oct_Runtime* rt) {
	oct_Uword iters = sizeof(oct_BuiltInTypes) / sizeof(oct_BType);
	oct_Uword i;
	oct_BType* place;

	for(i = 0; i < iters; ++i) {
		char* dummy = (char*)(&rt->builtInTypes);
		dummy += sizeof(oct_BType) * i;
		place = (oct_BType*)dummy;
		place->ptr = (oct_Type*)malloc(sizeof(oct_Type));
	}
}

#define CHECK(X) if(!X) return oct_False;

static oct_Bool bind_type(oct_Context* ctx, oct_BNamespace ns, const char* name, oct_BType type) {
	oct_OString str;
	oct_OSymbol sym;
	oct_BSymbol bSym;
	oct_OHashtableKey key;
    oct_Any value;
	
	CHECK(oct_String_createOwnedFromCString(ctx, name, &str));
	CHECK(oct_Symbol_createOwned(ctx, str, &sym));
	bSym.ptr = sym.ptr;
	CHECK(oct_Symbol_asHashtableKey(ctx, bSym, (oct_BHashtableKey*)&key));
    value.variant = OCT_ANY_BOBJECT;
	CHECK(oct_Type_asObject(ctx, type, &value.bobject));
	return oct_Namespace_bind(ctx, ns, key, value);
}

struct oct_Runtime* oct_Runtime_create(const char** out_error) {
	oct_Runtime* rt;
	oct_Context* mainCtx;
	oct_OString str;
	oct_BNamespace octarine;

	oct_initJITTarget();

	rt = (oct_Runtime*)calloc(1, sizeof(oct_Runtime));
	if(!rt) {
		(*out_error) = "Out of memory";
		return NULL;
	}

	// *** 1. Create main thread context

	mainCtx = (oct_Context*)calloc(1, sizeof(oct_Context));
	if(!mainCtx) {
		(*out_error) = "Out of memory";
		return NULL;
	}
	mainCtx->rt = rt;
	mainCtx->reader = (oct_Reader*)malloc(sizeof(oct_Reader));
	oct_Reader_ctor(mainCtx, mainCtx->reader); // This is a little weird
    oct_TLSInit(&rt->currentContext);
	oct_TLSSet(rt->currentContext, mainCtx);

	// *** 2. Initialize all the built in types
	//        I.e. those needed by the reader and compiler and any dependencies
	//        of those types.

	// Allocate memory for all built in types up front to resolve circular dependencies.
	alloc_builtInTypes(rt);

	// Initialize all built in types
	// TODO: intern the types? Difficult, have to care about order because pointers may change...

	_oct_Primitives_init(mainCtx);
	_oct_Hashtable_init(mainCtx);
	_oct_Type_init(mainCtx);
	_oct_Protocol_init(mainCtx);
	_oct_Function_init(mainCtx);
	_oct_Object_init(mainCtx);
	_oct_String_init(mainCtx);
	_oct_Array_init(mainCtx);
	_oct_AChar_init(mainCtx);
	_oct_AU8_init(mainCtx);
	_oct_List_init(mainCtx);
	_oct_Nothing_init(mainCtx);
	_oct_PointerType_init(mainCtx);
	_oct_StructType_init(mainCtx);
	_oct_Field_init(mainCtx);
	_oct_Symbol_init(mainCtx);
	_oct_ProtoType_init(mainCtx);
	_oct_VariadicType_init(mainCtx);
	_oct_Error_init(mainCtx);
	_oct_Any_init(mainCtx);
	_oct_Namespace_init(mainCtx);

	// *** 3. Create octarine namespace.

	oct_String_createOwnedFromCString(mainCtx, "octarine", &str);
	oct_Namespace_create(mainCtx, str, &octarine);
	mainCtx->ns = octarine.ptr;

	// *** 4. Register all built in types and functions in octarine namespace.
	// Primitives
	bind_type(mainCtx, octarine, "U8", rt->builtInTypes.U8);
	bind_type(mainCtx, octarine, "I8", rt->builtInTypes.I8);
	bind_type(mainCtx, octarine, "U16", rt->builtInTypes.U16);
	bind_type(mainCtx, octarine, "I16", rt->builtInTypes.I16);
	bind_type(mainCtx, octarine, "U32", rt->builtInTypes.U32);
	bind_type(mainCtx, octarine, "I32", rt->builtInTypes.I32);
	bind_type(mainCtx, octarine, "U64", rt->builtInTypes.U64);
	bind_type(mainCtx, octarine, "I64", rt->builtInTypes.I64);
	bind_type(mainCtx, octarine, "F32", rt->builtInTypes.F32);
	bind_type(mainCtx, octarine, "F64", rt->builtInTypes.F64);
	bind_type(mainCtx, octarine, "Uword", rt->builtInTypes.Uword);
	bind_type(mainCtx, octarine, "Word", rt->builtInTypes.Word);
	bind_type(mainCtx, octarine, "Char", rt->builtInTypes.Char);
	bind_type(mainCtx, octarine, "Bool", rt->builtInTypes.Bool);
	// Hashtable
	bind_type(mainCtx, octarine, "HashtableKey", rt->builtInTypes.HashtableKey);
	bind_type(mainCtx, octarine, "~HashtableKey", rt->builtInTypes.OHashtableKey);
	bind_type(mainCtx, octarine, "&HashtableKey", rt->builtInTypes.BHashtableKey);
	bind_type(mainCtx, octarine, "HashtableEntry", rt->builtInTypes.HashtableEntry);
	bind_type(mainCtx, octarine, "+HashtableEntry", rt->builtInTypes.AHashtableEntry);
	bind_type(mainCtx, octarine, "~+HashtableEntry", rt->builtInTypes.OAHashtableEntry);
	bind_type(mainCtx, octarine, "Hashtable", rt->builtInTypes.Hashtable);
	bind_type(mainCtx, octarine, "&Hashtable", rt->builtInTypes.BHashtable);
	// Type
	bind_type(mainCtx, octarine, "Type", rt->builtInTypes.Type);
	bind_type(mainCtx, octarine, "&Type", rt->builtInTypes.BType);
	bind_type(mainCtx, octarine, "+&Type", rt->builtInTypes.ABType);
	bind_type(mainCtx, octarine, "~+&Type", rt->builtInTypes.OABType);
	// Protocol
	bind_type(mainCtx, octarine, "Protocol", rt->builtInTypes.Protocol);
	bind_type(mainCtx, octarine, "VTable", rt->builtInTypes.VTable);
	bind_type(mainCtx, octarine, "&VTable", rt->builtInTypes.BVTable);
	bind_type(mainCtx, octarine, "ProtocolBinding", rt->builtInTypes.ProtocolBinding);
	bind_type(mainCtx, octarine, "&ProtocolBinding", rt->builtInTypes.BProtocolBinding);
	// Function
	bind_type(mainCtx, octarine, "Function", rt->builtInTypes.Function);
	bind_type(mainCtx, octarine, "&Function", rt->builtInTypes.BFunction);
	bind_type(mainCtx, octarine, "+&Function", rt->builtInTypes.ABFunction);
	bind_type(mainCtx, octarine, "~+&Function", rt->builtInTypes.OABFunction);
	// Object
	bind_type(mainCtx, octarine, "Object", rt->builtInTypes.Object);
	bind_type(mainCtx, octarine, "~Object", rt->builtInTypes.OObject);
	bind_type(mainCtx, octarine, "&Object", rt->builtInTypes.BObject);
	bind_type(mainCtx, octarine, "ObjectOption~", rt->builtInTypes.OObjectOption);
	bind_type(mainCtx, octarine, "ObjectOption&", rt->builtInTypes.BObjectOption);
	// String
	bind_type(mainCtx, octarine, "String", rt->builtInTypes.String);
	bind_type(mainCtx, octarine, "~String", rt->builtInTypes.OString);
	bind_type(mainCtx, octarine, "&String", rt->builtInTypes.BString);
	// Array
	bind_type(mainCtx, octarine, "Array", rt->builtInTypes.Array);
	bind_type(mainCtx, octarine, "FixedSizeArray", rt->builtInTypes.FixedSizeArray);
	// AChar
	bind_type(mainCtx, octarine, "+Char", rt->builtInTypes.AChar);
	bind_type(mainCtx, octarine, "~+Char", rt->builtInTypes.OAChar);
	// AU8
	bind_type(mainCtx, octarine, "+U8", rt->builtInTypes.AU8);
	bind_type(mainCtx, octarine, "~+U8", rt->builtInTypes.OAU8);
	// List
	bind_type(mainCtx, octarine, "List", rt->builtInTypes.List);
	bind_type(mainCtx, octarine, "~List", rt->builtInTypes.OList);
	bind_type(mainCtx, octarine, "&List", rt->builtInTypes.BList);
	bind_type(mainCtx, octarine, "ListOption~", rt->builtInTypes.OListOption);
	bind_type(mainCtx, octarine, "ListOption&", rt->builtInTypes.BListOption);
	// Nothing
	bind_type(mainCtx, octarine, "Nothing", rt->builtInTypes.Nothing);
	// Pointer
	bind_type(mainCtx, octarine, "Pointer", rt->builtInTypes.Pointer);
	// Struct
	bind_type(mainCtx, octarine, "Struct", rt->builtInTypes.Struct);
	// Field
	bind_type(mainCtx, octarine, "Field", rt->builtInTypes.Field);
	bind_type(mainCtx, octarine, "+Field", rt->builtInTypes.AField);
	bind_type(mainCtx, octarine, "~+Field", rt->builtInTypes.OAField);
	// Symbol
	bind_type(mainCtx, octarine, "Symbol", rt->builtInTypes.Symbol);
	bind_type(mainCtx, octarine, "~Symbol", rt->builtInTypes.OSymbol);
	bind_type(mainCtx, octarine, "&Symbol", rt->builtInTypes.BSymbol);
	bind_type(mainCtx, octarine, "SymbolOption~", rt->builtInTypes.OSymbolOption);
	// Prototype
	bind_type(mainCtx, octarine, "Prototype", rt->builtInTypes.Prototype);
	// Variadic
	bind_type(mainCtx, octarine, "Variadic", rt->builtInTypes.Variadic);
	// Error
	bind_type(mainCtx, octarine, "Error", rt->builtInTypes.Error);
	bind_type(mainCtx, octarine, "~Error", rt->builtInTypes.OError);
	bind_type(mainCtx, octarine, "ErrorOption~", rt->builtInTypes.OErrorOption);
	// Any
	bind_type(mainCtx, octarine, "Any", rt->builtInTypes.Any);
	// Namespace
	bind_type(mainCtx, octarine, "Namespace", rt->builtInTypes.Namespace);
	bind_type(mainCtx, octarine, "&Namespace", rt->builtInTypes.BNamespace);
	bind_type(mainCtx, octarine, "NamespaceOption", rt->builtInTypes.NamespaceOption);

	// *** 5. Profit?
	
	return rt;
}

oct_Bool oct_Runtime_destroy(oct_Runtime* rt, const char** out_error) {
	// TODO: ... implement?
	return oct_True;
}

oct_Context* oct_Runtime_currentContext(oct_Runtime* rt) {
	return (oct_Context*)oct_TLSGet(rt->currentContext);
}
