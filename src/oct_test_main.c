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
	assert(oct_OString_createFromCString(ctx, "Hello", &s1));
	assert(oct_OString_createFromCString(ctx, "Hello", &s2));
	bs1.ptr = s1.ptr; // borrow s1
	bs2.ptr = s2.ptr; // borrow s2
	assert(oct_BString_equals(ctx, bs1, bs2, &result));
	assert(result);
	assert(oct_OString_destroy(ctx, s1));
	assert(oct_OString_destroy(ctx, s2));
	// END Equals

	// Reading
	assert(oct_OString_createFromCString(ctx, "\"Hello\"", &s1));
	bs1.ptr = s1.ptr; // borrow s1
	assert(oct_OStringStream_create(ctx, bs1, &ss));
	bss.ptr = ss.ptr;
	assert(oct_BStringStream_asCharStream(ctx, bss, &charStream));
	reader.ptr = ctx->reader; // TODO: method for this
	assert(oct_Reader_read(ctx, reader, charStream, &readResult));
	assert(readResult.variant == OCT_READRESULT_READABLE);
	assert(readResult.readable.ptr->variant == OCT_READABLE_STRING);
	bs2.ptr = &readResult.readable.ptr->string;
	assert(oct_OStringStream_destroy(ctx, ss));

	assert(oct_OString_destroy(ctx, s1));
	assert(oct_OString_createFromCString(ctx, "Hello", &s1));
	bs1.ptr = s1.ptr; // borrow s1

	assert(oct_BString_equals(ctx, bs1, bs2, &result));
	assert(result);
	assert(oct_OString_destroy(ctx, s1));
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
	assert(oct_OString_createFromCString(ctx, "theName", &name));
	assert(oct_OString_createFromCString(ctx, "theValue", &valStr));
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
	oct_BReadable breadable;
	oct_AnyOption evalResult;
	reader.ptr = ctx->reader;

	oct_OString_createFromCString(ctx, "- . ! ? 1 2 3 -37 1.5 0.34 .34 1e16 -0.8 -.8 -.main .main -main { [ hello \"hej\" \"hell o workdl\" (this is a (nested) \"list\" of 8 readables ) (def mac \"mac\")", &str);

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
		breadable.ptr = rr.readable.ptr;
		oct_Compiler_eval(ctx, breadable, &evalResult);
		oct_ReadResult_dtor(ctx, &rr);
		if(evalResult.variant == OCT_ANYOPTION_ANY) {
			oct_Any_dtor(ctx, evalResult.any);
		}
	};

	// End of scope, destroy in reverse order.
	oct_OStringStream_destroy(ctx, ss);
	oct_OString_destroy(ctx, str);
	oct_Runtime_destroy(rt, &error);

	StringTests();
	NamespaceTests();

	return 0;
}
