#include "oct_runtime.h"
#include "oct_primitives.h"
#include "oct_type.h"
#include "oct_arraytype.h"
#include "oct_pointertype.h"
#include "oct_u8array.h"
#include "oct_string.h"
#include "oct_readable.h"
#include "oct_list.h"
#include "oct_option.h"
#include "oct_string.h"
#include "oct_symbol.h"
#include "oct_namespace.h"
#include "oct_any.h"
#include "oct_context.h"
#include "oct_reader.h"
#include "oct_primitive_pointers.h"
#include "oct_initTarget.h"

#include <stdlib.h>

static void alloc_builtIn(oct_Runtime* rt) {
	oct_Uword iters = sizeof(oct_BuiltInTypes) / sizeof(oct_Type*);
	oct_Uword i;
	oct_Type** place;

	for(i = 0; i < iters; ++i) {
		char* dummy = (char*)(&rt->builtInTypes);
		dummy += sizeof(oct_Type*) * i;
		place = (oct_Type**)dummy;
		(*place) = (oct_Type*)malloc(sizeof(oct_Type));
	}
}

static oct_Bool bind_type(oct_Context* ctx, oct_BNamespace ns, const char* name, oct_Type* type) {
	oct_OString str;
	oct_OSymbol sym;
	oct_Any val;
	oct_BType btype;
	
	btype.ptr = ctx->rt->builtInTypes.Type;

	if(!oct_OString_createFromCString(ctx, name, &str)) return oct_False;
	if(!oct_OSymbol_alloc(ctx, str, &sym)) return oct_False;
	if(!oct_Any_setPtrKind(ctx, &val, OCT_POINTER_BORROWED)) return oct_False;
	if(!oct_Any_setType(ctx, &val, btype)) return oct_False;
	if(!oct_Any_setPtr(ctx, &val, type)) return oct_False;
	return oct_Namespace_bind(ctx, ns, sym, val);
}

struct oct_Runtime* oct_Runtime_create(const char** out_error) {
	oct_Runtime* rt;
	oct_Context* mainCtx;
	oct_OString str;
	oct_OSymbol sym;
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
	alloc_builtIn(rt);

	// Initialize all built in types
	// TODO: intern the types? Difficult, have to care about order because pointers may change...

	_oct_Primitives_initType(mainCtx);
	_oct_ArrayType_initType(mainCtx);
	_oct_FixedSizeArrayType_initType(mainCtx);
	_oct_PointerType_initType(mainCtx);
	_oct_ProtoType_initType(mainCtx);
	_oct_TemplateType_initType(mainCtx);
	_oct_TemplateParam_initType(mainCtx);
    _oct_ATemplateParam_initType(mainCtx);
	_oct_VariadicType_initType(mainCtx);
	_oct_StructType_initType(mainCtx);
	_oct_Type_initType(mainCtx);
	_oct_BType_initType(mainCtx);
	_oct_ABType_initType(mainCtx);
	_oct_OABType_initType(mainCtx);
	_oct_Field_initType(mainCtx);
	_oct_AField_initType(mainCtx);
	_oct_OAField_initType(mainCtx);
    _oct_AU8_initType(mainCtx);
	_oct_String_initType(mainCtx);
	_oct_ListT_initType(mainCtx);
	_oct_ReadableList_initType(mainCtx);
	_oct_Readable_initType(mainCtx);
	_oct_OReadableList_initType(mainCtx);
	_oct_BReadableList_initType(mainCtx);
	_oct_ReadableListOption_initType(mainCtx);
	_oct_OReadable_initType(mainCtx);
	_oct_Nothing_initType(mainCtx);
	_oct_OptionT_initType(mainCtx);
	_oct_ListOptionT_initType(mainCtx);
	_oct_Symbol_initType(mainCtx);
	_oct_Any_initType(mainCtx);
	_oct_NamespaceBinding_initType(mainCtx);
	_oct_ANamespaceBinding_initType(mainCtx);
	_oct_OANamespaceBinding_initType(mainCtx);
	_oct_Namespace_initType(mainCtx);
	_oct_OSymbolOption_initType(mainCtx);
	_oct_OReadableOption_initType(mainCtx);
	//_oct_PrimitivePointers_initType(mainCtx);
	_oct_Interface_initType(mainCtx);
	_oct_Function_initType(mainCtx);
	_oct_BFunction_initType(mainCtx);
	_oct_ABFunction_initType(mainCtx);
	_oct_OABFunction_initType(mainCtx);
	_oct_AChar_initType(mainCtx);
	_oct_OAChar_initType(mainCtx);

	//_oct_ReadResult_initType(mainCtx);
	//_oct_Reader_initType(mainCtx);

	// *** 3. Create octarine namespace.

	oct_OString_createFromCString(mainCtx, "octarine", &str);
	oct_OSymbol_alloc(mainCtx, str, &sym);
	oct_Namespace_create(mainCtx, sym, &octarine);

	// *** 4. Register all built in types and functions in octarine namespace.
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
	bind_type(mainCtx, octarine, "List<>", rt->builtInTypes.ListT);
	bind_type(mainCtx, octarine, "Symbol", rt->builtInTypes.Symbol);
	bind_type(mainCtx, octarine, "~Symbol", rt->builtInTypes.OSymbol);
	bind_type(mainCtx, octarine, "~SymbolOption", rt->builtInTypes.OSymbolOption);
	bind_type(mainCtx, octarine, "String", rt->builtInTypes.String);
	bind_type(mainCtx, octarine, "~String", rt->builtInTypes.OString);
	bind_type(mainCtx, octarine, "Type", rt->builtInTypes.Type);
	bind_type(mainCtx, octarine, "StructType", rt->builtInTypes.StructType);
	bind_type(mainCtx, octarine, "ProtoType", rt->builtInTypes.ProtoType);
	bind_type(mainCtx, octarine, "VariadicType", rt->builtInTypes.VariadicType);
	bind_type(mainCtx, octarine, "TemplateType", rt->builtInTypes.TemplateType);
	bind_type(mainCtx, octarine, "PointerType", rt->builtInTypes.PointerType);
	bind_type(mainCtx, octarine, "ArrayType", rt->builtInTypes.ArrayType);
	bind_type(mainCtx, octarine, "FixedSizeArrayType", rt->builtInTypes.FixedSizeArrayType);
	bind_type(mainCtx, octarine, "TemplateParam", rt->builtInTypes.TemplateParam);
	bind_type(mainCtx, octarine, "+TemplateParam", rt->builtInTypes.ATemplateParam);
	bind_type(mainCtx, octarine, "~+TemplateParam", rt->builtInTypes.OATemplateParam);
	bind_type(mainCtx, octarine, "Field", rt->builtInTypes.Field);
	bind_type(mainCtx, octarine, "+Field", rt->builtInTypes.AField);
	bind_type(mainCtx, octarine, "~+Field", rt->builtInTypes.OAField);
	bind_type(mainCtx, octarine, "&Type", rt->builtInTypes.BType);
	bind_type(mainCtx, octarine, "+&Type", rt->builtInTypes.ABType);
	bind_type(mainCtx, octarine, "~+&Type", rt->builtInTypes.OABType);
	bind_type(mainCtx, octarine, "+U8", rt->builtInTypes.AU8);
	bind_type(mainCtx, octarine, "~+U8", rt->builtInTypes.OAU8);
	bind_type(mainCtx, octarine, "Readable", rt->builtInTypes.Readable);
	bind_type(mainCtx, octarine, "ReadableList", rt->builtInTypes.ReadableList);
	bind_type(mainCtx, octarine, "~ReadableList", rt->builtInTypes.OReadableList);
	bind_type(mainCtx, octarine, "&ReadableList", rt->builtInTypes.BReadableList);
	bind_type(mainCtx, octarine, "ReadableListOption", rt->builtInTypes.ReadableListOption);
	bind_type(mainCtx, octarine, "~Readable", rt->builtInTypes.OReadable);
	// Don't like this syntax for the owned readable option:
	bind_type(mainCtx, octarine, "~ReadableOption", rt->builtInTypes.OReadableOption);
	bind_type(mainCtx, octarine, "Nothing", rt->builtInTypes.Nothing);
	bind_type(mainCtx, octarine, "ListOption<>", rt->builtInTypes.ListOptionT);
	bind_type(mainCtx, octarine, "Option<>", rt->builtInTypes.OptionT);
	bind_type(mainCtx, octarine, "Any", rt->builtInTypes.Any);
	bind_type(mainCtx, octarine, "+Any", rt->builtInTypes.AAny);
	bind_type(mainCtx, octarine, "~+Any", rt->builtInTypes.OAAny);
	bind_type(mainCtx, octarine, "NamespaceBinding", rt->builtInTypes.NamespaceBinding);
	bind_type(mainCtx, octarine, "+NamespaceBinding", rt->builtInTypes.ANamespaceBinding);
	bind_type(mainCtx, octarine, "~+NamespaceBinding", rt->builtInTypes.OANamespaceBinding);
	bind_type(mainCtx, octarine, "Namespace", rt->builtInTypes.Namespace);
	bind_type(mainCtx, octarine, "&Namespace", rt->builtInTypes.BNamespace);
	bind_type(mainCtx, octarine, "Reader", rt->builtInTypes.Reader);
	bind_type(mainCtx, octarine, "ReadResult", rt->builtInTypes.ReadResult);
	bind_type(mainCtx, octarine, "InterfaceType", rt->builtInTypes.InterfaceType);

	return rt;
}

oct_Bool oct_Runtime_destroy(oct_Runtime* rt, const char** out_error) {
	// TODO: ... implement?
	return oct_True;
}

oct_Context* oct_Runtime_currentContext(oct_Runtime* rt) {
	return (oct_Context*)oct_TLSGet(rt->currentContext);
}
