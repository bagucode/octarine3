#include "octarine.h"

int main(int argc, char** argv) {
	const char* error;
	oct_BCharstream stream;
	oct_CharstreamVTable csVTable; // Where should this be stored? With the implementing type(StringStream)?
	oct_ReadResult rr;
	oct_BReader reader;
	oct_Runtime* rt = oct_Runtime_create(&error);
	oct_Context* ctx = oct_Runtime_currentContext(rt);
	oct_OString str;
	oct_BString bstr;
	oct_OStringStream ss;
	oct_BReadable breadable;
	oct_Any evalResult;
	reader.ptr = ctx->reader;

	oct_OString_createFromCString(ctx, "- 1 2 3 -37 1.5 0.34 .34 1e16 -0.8 -.8 -.main .main -main { [ hello \"hej\" \"hell o workdl\" (this is a (nested) \"list\" of 8 readables )", &str);
	//oct_OString_createFromCString(ctx, "(small)", &str);

	// Borrow string pointer
	bstr.ptr = str.ptr;

	oct_OStringStream_create(ctx, bstr, &ss);

	// Create charstream instance from the stringstream
	csVTable.read = (oct_Bool(*)(struct oct_Context*, void*, oct_Char*)) oct_BStringStream_readChar;
	csVTable.peek = (oct_Bool(*)(struct oct_Context*, void*, oct_Char*)) oct_BStringStream_peekChar;
	stream.b_self = ss.ptr; // Borrow stringstream pointer
	stream.vtable = &csVTable;

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

	return 0;
}
