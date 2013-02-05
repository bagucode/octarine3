#include "octarine.h"
#include <assert.h>

static void StringTests() {
	oct_Runtime* rt;
	oct_Context* ctx;
	oct_OString s1;
	oct_OString s2;
	oct_BString bs1;
	oct_BString bs2;
	oct_Bool result;
	oct_Charstream charStream;
	oct_OStringStream ss;
	oct_BStringStream bss;
	oct_ReadResult readResult;
	oct_BReader reader;
	const char* error;

	rt = oct_Runtime_create(&error);
	assert(rt);
	ctx = oct_Runtime_currentContext(rt);
	assert(ctx);

	// Equals
	assert(oct_String_createOwnedFromCString(ctx, "Hello", &s1));
	assert(oct_String_createOwnedFromCString(ctx, "Hello", &s2));
	bs1.ptr = s1.ptr; // borrow s1
	bs2.ptr = s2.ptr; // borrow s2
	assert(oct_BString_equals(ctx, bs1, bs2, &result));
	assert(result);
	assert(oct_String_destroyOwned(ctx, s1));
	assert(oct_String_destroyOwned(ctx, s2));
	// END Equals

	// Reading
	assert(oct_String_createOwnedFromCString(ctx, "\"Hello\"", &s1));
	bs1.ptr = s1.ptr; // borrow s1
	assert(oct_OStringStream_create(ctx, bs1, &ss));
	bss.ptr = ss.ptr;
	assert(oct_BStringStream_asCharStream(ctx, bss, &charStream));
	reader.ptr = ctx->reader; // TODO: method for this
	assert(oct_Reader_read(ctx, reader, charStream, &readResult));
	assert(readResult.variant == OCT_READRESULT_ANY);
	assert(oct_Any_stringp(ctx, readResult.result, &result));
	assert(result);
	assert(oct_Any_getPtr(ctx, readResult.result, (void**)&bs2.ptr));
	assert(oct_OStringStream_destroy(ctx, ss));

	assert(oct_String_destroyOwned(ctx, s1));
	assert(oct_String_createOwnedFromCString(ctx, "Hello", &s1));
	bs1.ptr = s1.ptr; // borrow s1

	assert(oct_BString_equals(ctx, bs1, bs2, &result));
	assert(result);
	assert(oct_String_destroyOwned(ctx, s1));
	assert(oct_ReadResult_dtor(ctx, &readResult));
	// END Reading

	assert(oct_Runtime_destroy(rt, &error));
}

static void NamespaceTests() {
	oct_Runtime* rt;
	oct_Context* ctx;
	oct_BNamespace ns;
	oct_AnyOption val;
	oct_AnyOption lookedUp;
	oct_OSymbol sym;
	oct_BSymbol bsym;
	oct_OString name;
	oct_OString valStr;
	oct_BType type;
	const char* error;

	rt = oct_Runtime_create(&error);
	assert(rt);
	ctx = oct_Runtime_currentContext(rt);
	assert(ctx);
	assert(ctx->ns);
	ns.ptr = ctx->ns;

	// Binding
	assert(oct_String_createOwnedFromCString(ctx, "theName", &name));
	assert(oct_String_createOwnedFromCString(ctx, "theValue", &valStr));
	assert(oct_OSymbol_alloc(ctx, name, &sym));
	val.variant = OCT_ANYOPTION_ANY;
	assert(oct_Any_setPtrKind(ctx, &val.any, OCT_POINTER_OWNED));
	assert(oct_Any_setPtr(ctx, &val.any, valStr.ptr));
	type.ptr = ctx->rt->builtInTypes.String;
	assert(oct_Any_setType(ctx, &val.any, type));
	assert(oct_Namespace_bind(ctx, ns, sym, val));
	// End Binding

	// Lookup
	bsym.ptr = sym.ptr;
	assert(oct_Namespace_lookup(ctx, ns, bsym, &lookedUp));
	assert(lookedUp.variant == val.variant);
	assert(lookedUp.any.data[0] == val.any.data[0]);
	assert(lookedUp.any.data[1] == val.any.data[1]);
	// End Lookup

	assert(oct_Runtime_destroy(rt, &error));
}

static void defTest() {
	oct_Runtime* rt;
	oct_Context* ctx;
	oct_BNamespace ns;
	oct_AnyOption lookedUp;
	oct_OString str;
	oct_BString bs1;
	oct_BString bs2;
	oct_ReadResult readResult;
	oct_BReader reader;
	oct_BString bstr;
	oct_OStringStream ss;
	oct_BStringStream bss;
	oct_Charstream stream;
	oct_BSymbol bsym;
	oct_OSymbol osym;
	oct_Bool result;
	oct_AnyOption evalResult;
	void* outStr;
	const char* error;

	rt = oct_Runtime_create(&error);
	assert(rt);
	ctx = oct_Runtime_currentContext(rt);
	assert(ctx);
	assert(ctx->ns);
	ns.ptr = ctx->ns;

	// Eval
	assert(oct_String_createOwnedFromCString(ctx, "(def hello \"Hello\")", &str));
    assert(str.ptr);
	bstr.ptr = str.ptr;
	assert(oct_OStringStream_create(ctx, bstr, &ss));
    assert(ss.ptr);
	bss.ptr = ss.ptr;
	assert(oct_BStringStream_asCharStream(ctx, bss, &stream));
    assert(stream.vtable);
	reader.ptr = ctx->reader;
	assert(oct_Reader_read(ctx, reader, stream, &readResult));
	assert(oct_Compiler_eval(ctx, readResult.result, &evalResult));

	// Lookup
	assert(oct_String_createOwnedFromCString(ctx, "hello", &str));
	assert(oct_OSymbol_alloc(ctx, str, &osym));
	bsym.ptr = osym.ptr;
	assert(oct_Namespace_lookup(ctx, ns, bsym, &lookedUp));
	assert(oct_Any_getPtr(ctx, lookedUp.any, &outStr));
	assert(oct_String_createOwnedFromCString(ctx, "Hello", &str));
	bs1.ptr = str.ptr;
	bs2.ptr = (oct_String*)outStr;
	assert(oct_BString_equals(ctx, bs1, bs2, &result));
	assert(result);

	assert(oct_Runtime_destroy(rt, &error));
}

int main(int argc, char** argv) {
	const char* error;
	oct_Charstream stream;
	oct_ReadResult rr;
	oct_BReader reader;
	oct_Runtime* rt = oct_Runtime_create(&error);
	oct_Context* ctx = oct_Runtime_currentContext(rt);
	oct_OString str;
	oct_BString bstr;
	oct_OStringStream ss;
	oct_BStringStream bss;
	oct_AnyOption evalResult;
	reader.ptr = ctx->reader;

	oct_String_createOwnedFromCString(ctx, "- . ! ? 1 2 3 -37 1.5 0.34 .34 1e16 -0.8 -.8 -.main .main -main { [ hello \"hej\" \"hell o workdl\" (this is a (nested) \"list\" of 8 readables ) (def mac \"mac\")", &str);

	// Borrow string pointer
	bstr.ptr = str.ptr;

	oct_OStringStream_create(ctx, bstr, &ss);

	bss.ptr = ss.ptr;
	oct_BStringStream_asCharStream(ctx, bss, &stream);

	while(oct_True) {
		oct_Reader_read(ctx, reader, stream, &rr);
		if(rr.variant == OCT_READRESULT_ERROR) {
			break;
		}
		oct_Compiler_eval(ctx, rr.result, &evalResult);
		oct_ReadResult_dtor(ctx, &rr);
		if(evalResult.variant == OCT_ANYOPTION_ANY) {
			oct_Any_dtor(ctx, evalResult.any);
		}
	};

	// End of scope, destroy in reverse order.
	oct_OStringStream_destroy(ctx, ss);
	oct_String_destroyOwned(ctx, str);
	oct_Runtime_destroy(rt, &error);

	StringTests();
	NamespaceTests();
	defTest();

	return 0;
}
