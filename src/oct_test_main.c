#include "octarine.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#define TEST(X) if(!X) { printf("%s\n", &ctx->err.error.ptr->message.ptr->utf8Data.ptr->data[0]); abort(); }

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
	TEST(oct_String_createOwnedFromCString(ctx, "Hello", &s1));
	TEST(oct_String_createOwnedFromCString(ctx, "Hello", &s2));
	bs1.ptr = s1.ptr; // borrow s1
	bs2.ptr = s2.ptr; // borrow s2
	TEST(oct_BString_equals(ctx, bs1, bs2, &result));
	TEST(result);
	TEST(oct_String_destroyOwned(ctx, s1));
	TEST(oct_String_destroyOwned(ctx, s2));
	// END Equals

	// Reading
	TEST(oct_String_createOwnedFromCString(ctx, "\"Hello\"", &s1));
	bs1.ptr = s1.ptr; // borrow s1
	TEST(oct_OStringStream_create(ctx, bs1, &ss));
	bss.ptr = ss.ptr;
	TEST(oct_BStringStream_asCharStream(ctx, bss, &charStream));
	reader.ptr = ctx->reader; // TODO: method for this
	TEST(oct_Reader_read(ctx, reader, charStream, &readResult));
	TEST(readResult.variant == OCT_READRESULT_OK);
	TEST(oct_Any_stringp(ctx, readResult.result, &result));
	TEST(result);
	TEST(oct_Any_getPtr(ctx, readResult.result, (void**)&bs2.ptr));
	TEST(oct_OStringStream_destroy(ctx, ss));

	TEST(oct_String_destroyOwned(ctx, s1));
	TEST(oct_String_createOwnedFromCString(ctx, "Hello", &s1));
	bs1.ptr = s1.ptr; // borrow s1

	TEST(oct_BString_equals(ctx, bs1, bs2, &result));
	TEST(result);
	TEST(oct_String_destroyOwned(ctx, s1));
	TEST(oct_ReadResult_dtor(ctx, &readResult));
	// END Reading

	TEST(oct_Runtime_destroy(rt, &error));
}

static void NamespaceTests() {
	oct_Runtime* rt;
	oct_Context* ctx;
	oct_BNamespace ns;
	oct_OObjectOption val;
	oct_OObjectOption lookedUp;
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
	TEST(oct_String_createOwnedFromCString(ctx, "theName", &name));
	TEST(oct_String_createOwnedFromCString(ctx, "theValue", &valStr));
	TEST(oct_Symbol_createOwned(ctx, name, &sym));
	val.variant = OCT_OOBJECTOPTION_OBJECT;
	TEST(oct_String_asObject(ctx, valStr, &val.object));
	TEST(oct_Namespace_bind(ctx, ns, sym, val));
	// End Binding

	// Lookup
	bsym.ptr = sym.ptr;
	TEST(oct_Namespace_lookup(ctx, ns, bsym, &lookedUp));
	TEST(lookedUp.variant == val.variant);
	TEST((lookedUp.object.object.object == (void*)valStr.ptr));
	// End Lookup

	TEST(oct_Runtime_destroy(rt, &error));
}

static void defTest() {
	oct_Runtime* rt;
	oct_Context* ctx;
	oct_BNamespace ns;
	oct_OObjectOption lookedUp;
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
	oct_OObjectOption evalResult;
	const char* error;

	rt = oct_Runtime_create(&error);
	assert(rt);
	ctx = oct_Runtime_currentContext(rt);
	assert(ctx);
	assert(ctx->ns);
	ns.ptr = ctx->ns;

	// Eval
	TEST(oct_String_createOwnedFromCString(ctx, "(def hello \"Hello\")", &str));
    TEST(str.ptr);
	bstr.ptr = str.ptr;
	TEST(oct_OStringStream_create(ctx, bstr, &ss));
    TEST(ss.ptr);
	bss.ptr = ss.ptr;
	TEST(oct_BStringStream_asCharStream(ctx, bss, &stream));
    TEST(stream.vtable);
	reader.ptr = ctx->reader;
	TEST(oct_Reader_read(ctx, reader, stream, &readResult));
	TEST(oct_Compiler_eval(ctx, readResult.result.object, &evalResult));

	// Lookup
	TEST(oct_String_createOwnedFromCString(ctx, "hello", &str));
	TEST(oct_Symbol_createOwned(ctx, str, &osym));
	bsym.ptr = osym.ptr;
	TEST(oct_Namespace_lookup(ctx, ns, bsym, &lookedUp));
	TEST(oct_String_createOwnedFromCString(ctx, "Hello", &str));
	bs1.ptr = str.ptr;
	bs2.ptr = (oct_String*)lookedUp.object.object.object;
	TEST(oct_BString_equals(ctx, bs1, bs2, &result));
	TEST(result);

	TEST(oct_Runtime_destroy(rt, &error));
}

static void graphCopyOwnedTest() {
	oct_Runtime* rt;
	oct_Context* ctx;
	oct_ReadResult readResult;
	oct_BReader reader;
	const char* error;
    oct_OObject copy;
	oct_BObject bob;
    
	rt = oct_Runtime_create(&error);
	assert(rt);
	ctx = oct_Runtime_currentContext(rt);
	assert(ctx);

    reader.ptr = ctx->reader;
    TEST(oct_Reader_readFromCString(ctx, reader, "(a list of symbols)", &readResult));
	bob.object = readResult.result.object.object;
    TEST(oct_Type_deepCopyGraphOwned(ctx, bob, &copy));
    
	TEST(oct_Runtime_destroy(rt, &error));
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
	oct_OObjectOption evalResult;
	reader.ptr = ctx->reader;

	oct_String_createOwnedFromCString(ctx, "- . ! ? 1 2 3 -37 1.5 0.34 .34 1e16 -0.8 -.8 -.main .main -main { [ hello \"hej\" \"hell o workdl\" (this is a (nested) \"list\" of 8 readables ) ()", &str);

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
		oct_Compiler_eval(ctx, rr.result.object, &evalResult);
		oct_ReadResult_dtor(ctx, &rr);
		if(evalResult.variant == OCT_OOBJECTOPTION_OBJECT) {
			oct_Object_destroyOwned(ctx, evalResult.object);
		}
	};

	// End of scope, destroy in reverse order.
	oct_OStringStream_destroy(ctx, ss);
	oct_String_destroyOwned(ctx, str);
	oct_Runtime_destroy(rt, &error);

	StringTests();
	NamespaceTests();
	defTest();
    graphCopyOwnedTest();

	return 0;
}
