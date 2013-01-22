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

int main(int argc, char** argv) {
	const char* error;
	oct_Charstream stream;
	//oct_CharstreamVTable csVTable; // Where should this be stored? With the implementing type(StringStream)?
	oct_ReadResult rr;
	oct_BReader reader;
	oct_Runtime* rt = oct_Runtime_create(&error);
	oct_Context* ctx = oct_Runtime_currentContext(rt);
	oct_OString str;
	oct_BString bstr;
	oct_OStringStream ss;
	oct_BStringStream bss;
	oct_BReadable breadable;
	oct_Any evalResult;
	reader.ptr = ctx->reader;

	oct_OString_createFromCString(ctx, "- . ! 1 2 3 -37 1.5 0.34 .34 1e16 -0.8 -.8 -.main .main -main { [ hello \"hej\" \"hell o workdl\" (this is a (nested) \"list\" of 8 readables )", &str);
	//oct_OString_createFromCString(ctx, "(small)", &str);

	// Borrow string pointer
	bstr.ptr = str.ptr;

	oct_OStringStream_create(ctx, bstr, &ss);

	// Create charstream instance from the stringstream
	//csVTable.read = (oct_Bool(*)(struct oct_Context*, void*, oct_Char*)) oct_BStringStream_readChar;
	//csVTable.peek = (oct_Bool(*)(struct oct_Context*, void*, oct_Char*)) oct_BStringStream_peekChar;
	//stream.b_self = ss.ptr; // Borrow stringstream pointer
	//stream.vtable = &csVTable;
	bss.ptr = ss.ptr;
	oct_BStringStream_asCharStream(ctx, bss, &stream);

	// This loop leaks memory. All valid results are owned by caller and should be deleted before rr is re-used
	while(oct_True) {
		oct_Reader_read(ctx, reader, stream, &rr);
		if(rr.variant == OCT_READRESULT_ERROR) {
			break;
		}
		breadable.ptr = rr.readable.ptr;
		oct_Compiler_eval(ctx, breadable, &evalResult);
	};

	// End of scope, destroy in reverse order.
	oct_OStringStream_destroy(ctx, ss);
	oct_OString_destroy(ctx, str);
	oct_Runtime_destroy(rt, &error);

	StringTests();

	return 0;
}
