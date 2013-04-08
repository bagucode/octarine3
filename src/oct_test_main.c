#include "octarine.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#define TEST(X) if(!X) { printf("%s\n", &ctx->err.berror.ptr->message.ptr->utf8Data.ptr->data[0]); abort(); }

static void StringTests() {
	oct_Runtime* rt;
	oct_Context* ctx;
	oct_OString s1;
	oct_OString s2;
	oct_BString bs1;
	oct_BString bs2;
	oct_Bool result;
	oct_BCharstream charStream;
	oct_OStringstream ss;
	oct_BStringstream bss;
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
	assert(result);
	TEST(oct_String_destroyOwned(ctx, s1));
	TEST(oct_String_destroyOwned(ctx, s2));
	// END Equals

	// Reading
	TEST(oct_String_createOwnedFromCString(ctx, "\"Hello\"", &s1));
	bs1.ptr = s1.ptr; // borrow s1
	TEST(oct_OStringstream_create(ctx, bs1, &ss));
	bss.ptr = ss.ptr;
	TEST(oct_Stringstream_asCharStream(ctx, bss, &charStream));
	reader.ptr = ctx->reader; // TODO: method for this

	TEST(oct_Reader_read(ctx, reader, charStream, &readResult));
	TEST(oct_OStringstream_destroy(ctx, ss));
	TEST(oct_String_destroyOwned(ctx, s1));

	assert(readResult.variant == OCT_READRESULT_OK);
	assert(readResult.result.object.vtable->type.ptr == ctx->rt->builtInTypes.String.ptr);
	bs2.ptr = (oct_String*)readResult.result.object.self.self;

	TEST(oct_String_createOwnedFromCString(ctx, "Hello", &s1));
	bs1.ptr = s1.ptr; // borrow s1

	TEST(oct_BString_equals(ctx, bs1, bs2, &result));
	assert(result);
	TEST(oct_String_destroyOwned(ctx, s1));
	TEST(oct_Object_destroyOwned(ctx, readResult.result.object));
	// END Reading

	TEST(oct_Runtime_destroy(rt, &error));
}

static void NamespaceTests() {
	oct_Runtime* rt;
	oct_Context* ctx;
	oct_BNamespace ns;
	oct_Any val;
	oct_Any lookedUp;
	oct_OSymbol sym;
	oct_BSymbol bsym;
	oct_OString name;
	oct_OString valStr;
	oct_HashtableKeyOption key;
	oct_BString eq1;
	oct_BString eq2;
	oct_Bool eq;
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
	TEST(oct_String_asObjectOwned(ctx, valStr, &val.oobject));
	bsym.ptr = sym.ptr;
	TEST(oct_Symbol_asHashtableKey(ctx, bsym, &key.borrowed));
	key.variant = OCT_HASHTABLEKEYOPTION_OWNED;
	TEST(oct_Namespace_bind(ctx, ns, key, val));
	// End Binding

	// Lookup
	TEST(oct_Namespace_lookup(ctx, ns, key.borrowed, &lookedUp));
	assert(lookedUp.variant == val.variant);
	TEST(oct_String_createOwnedFromCString(ctx, "theValue", &valStr));
	eq1.ptr = valStr.ptr;
	eq2.ptr = (oct_String*)lookedUp.oobject.self.self;
	TEST(oct_BString_equals(ctx, eq1, eq2, &eq));
	assert(eq);
	TEST(oct_String_destroyOwned(ctx, valStr));
	TEST(oct_Object_destroyOwned(ctx, lookedUp.oobject));

	// End Lookup

	TEST(oct_Runtime_destroy(rt, &error));
}

static void defTest() {
	oct_Runtime* rt;
	oct_Context* ctx;
	oct_BNamespace ns;
	oct_Any lookedUp;
	oct_OString str;
	oct_BString bs1;
	oct_BString bs2;
	oct_ReadResult readResult;
	oct_BReader reader;
	oct_BString bstr;
	oct_OStringstream ss;
	oct_BStringstream bss;
	oct_BCharstream stream;
	oct_BSymbol bsym;
	oct_OSymbol osym;
	oct_Bool result;
	oct_Any evalResult;
	oct_BHashtableKey key;
	const char* error;

	rt = oct_Runtime_create(&error);
	assert(rt);
	ctx = oct_Runtime_currentContext(rt);
	assert(ctx);
	assert(ctx->ns);
	ns.ptr = ctx->ns;

	// Eval
	TEST(oct_String_createOwnedFromCString(ctx, "(def hello \"Hello\")", &str));
    assert(str.ptr);
	bstr.ptr = str.ptr;
	TEST(oct_OStringstream_create(ctx, bstr, &ss));
    assert(ss.ptr);
	bss.ptr = ss.ptr;
	TEST(oct_Stringstream_asCharStream(ctx, bss, &stream));
    assert(stream.vtable);
	reader.ptr = ctx->reader;
	TEST(oct_Reader_read(ctx, reader, stream, &readResult));
	TEST(oct_OStringstream_destroy(ctx, ss));
	TEST(oct_String_destroyOwned(ctx, str));
	TEST(oct_Compiler_eval(ctx, readResult.result.object, &evalResult));
	if(evalResult.variant == OCT_ANY_OOBJECT) {
		TEST(oct_Object_destroyOwned(ctx, evalResult.oobject));
	}

	// Lookup
	TEST(oct_String_createOwnedFromCString(ctx, "hello", &str));
	TEST(oct_Symbol_createOwned(ctx, str, &osym));
	bsym.ptr = osym.ptr;
	TEST(oct_Symbol_asHashtableKey(ctx, bsym, &key));
	TEST(oct_Namespace_lookup(ctx, ns, key, &lookedUp));
	TEST(oct_Symbol_destroyOwned(ctx, osym));
	TEST(oct_String_createOwnedFromCString(ctx, "Hello", &str));
	bs1.ptr = str.ptr;
	bs2.ptr = (oct_String*)lookedUp.oobject.self.self;
	TEST(oct_BString_equals(ctx, bs1, bs2, &result));
	assert(result);
	TEST(oct_String_destroyOwned(ctx, str));
	TEST(oct_Object_destroyOwned(ctx, lookedUp.oobject));

	TEST(oct_Runtime_destroy(rt, &error));
}

void testListAsSeq() {
	const char* error;
	oct_Runtime* rt;
	oct_Context* ctx;
	oct_OList lst;
	oct_BSeq seq;
	oct_BGeneric self;
	oct_OList rest;
	oct_BList blst;
	oct_OString tmpStr;
	oct_OObject tmpObj;
	oct_OObjectOption out;
	oct_BString cmp1;
	oct_Bool eq;

	rt = oct_Runtime_create(&error);
	ctx = oct_Runtime_currentContext(rt);

	TEST(oct_List_createOwned(ctx, &lst));
	self.self = lst.ptr;
	TEST(oct_Object_as(ctx, self, rt->builtInTypes.List, rt->builtInProtocols.Seq, (oct_BObject*)&seq));
	TEST(oct_String_createOwnedFromCString(ctx, "Hello", &tmpStr));
	TEST(oct_String_asObjectOwned(ctx, tmpStr, &tmpObj));
	TEST(oct_Seq_append(ctx, seq, tmpObj));
	TEST(oct_String_createOwnedFromCString(ctx, "Really", &tmpStr));
	TEST(oct_String_asObjectOwned(ctx, tmpStr, &tmpObj));
	TEST(oct_Seq_append(ctx, seq, tmpObj));
	TEST(oct_String_createOwnedFromCString(ctx, "Big", &tmpStr));
	TEST(oct_String_asObjectOwned(ctx, tmpStr, &tmpObj));
	TEST(oct_Seq_append(ctx, seq, tmpObj));
	TEST(oct_String_createOwnedFromCString(ctx, "World!", &tmpStr));
	TEST(oct_String_asObjectOwned(ctx, tmpStr, &tmpObj));
	TEST(oct_Seq_append(ctx, seq, tmpObj));
	TEST(oct_Seq_first(ctx, seq, &out));
	cmp1.ptr = (oct_String*)out.object.self.self;
	TEST(oct_BStringCString_equals(ctx, cmp1, "Hello", &eq));
	assert(eq);
	TEST(oct_Object_destroyOwned(ctx, out.object));
	TEST(oct_Seq_nth(ctx, seq, 2, &out));
	cmp1.ptr = (oct_String*)out.object.self.self;
	TEST(oct_BStringCString_equals(ctx, cmp1, "World!", &eq));
	assert(eq);
	TEST(oct_Object_destroyOwned(ctx, out.object));
	TEST(oct_Seq_rest(ctx, seq, (oct_OGeneric*)&rest));
	TEST(oct_Seq_first(ctx, seq, &out));
	cmp1.ptr = (oct_String*)out.object.self.self;
	TEST(oct_BStringCString_equals(ctx, cmp1, "Really", &eq));
	assert(eq);
	TEST(oct_Object_destroyOwned(ctx, out.object));
	blst.ptr = rest.ptr;
	TEST(oct_List_first(ctx, blst, &out));
	cmp1.ptr = (oct_String*)out.object.self.self;
	TEST(oct_BStringCString_equals(ctx, cmp1, "Big", &eq));
	assert(eq);
	TEST(oct_Object_destroyOwned(ctx, out.object));
	TEST(oct_List_destroyOwned(ctx, rest));
	TEST(oct_List_destroyOwned(ctx, lst));

	oct_Runtime_destroy(rt, &error);
}

void testVecAsSeq() {
	const char* error;
	oct_Runtime* rt;
	oct_Context* ctx;
	oct_OVector lst;
	oct_BSeq seq;
	oct_BGeneric self;
	oct_OVector rest;
	oct_BVector blst;
	oct_OString tmpStr;
	oct_OObject tmpObj;
	oct_OObjectOption out;
	oct_BString cmp1;
	oct_Bool eq;

	rt = oct_Runtime_create(&error);
	ctx = oct_Runtime_currentContext(rt);

	TEST(oct_Vector_createOwned(ctx, &lst));
	self.self = lst.ptr;
	TEST(oct_Object_as(ctx, self, rt->builtInTypes.Vector, rt->builtInProtocols.Seq, (oct_BObject*)&seq));
	TEST(oct_String_createOwnedFromCString(ctx, "Hello", &tmpStr));
	TEST(oct_String_asObjectOwned(ctx, tmpStr, &tmpObj));
	TEST(oct_Seq_append(ctx, seq, tmpObj));
	TEST(oct_String_createOwnedFromCString(ctx, "Really", &tmpStr));
	TEST(oct_String_asObjectOwned(ctx, tmpStr, &tmpObj));
	TEST(oct_Seq_append(ctx, seq, tmpObj));
	TEST(oct_String_createOwnedFromCString(ctx, "Big", &tmpStr));
	TEST(oct_String_asObjectOwned(ctx, tmpStr, &tmpObj));
	TEST(oct_Seq_append(ctx, seq, tmpObj));
	TEST(oct_String_createOwnedFromCString(ctx, "World!", &tmpStr));
	TEST(oct_String_asObjectOwned(ctx, tmpStr, &tmpObj));
	TEST(oct_Seq_append(ctx, seq, tmpObj));

	TEST(oct_Seq_first(ctx, seq, &out));
	cmp1.ptr = (oct_String*)out.object.self.self;
	TEST(oct_BStringCString_equals(ctx, cmp1, "Hello", &eq));
	assert(eq);
	TEST(oct_Object_destroyOwned(ctx, out.object));
	
	TEST(oct_Seq_nth(ctx, seq, 3, &out));
	cmp1.ptr = (oct_String*)out.object.self.self;
	TEST(oct_BStringCString_equals(ctx, cmp1, "World!", &eq));
	assert(eq);
	TEST(oct_Object_destroyOwned(ctx, out.object));
	
	TEST(oct_Seq_rest(ctx, seq, (oct_OGeneric*)&rest));
	blst.ptr = rest.ptr;
	
	TEST(oct_Vector_first(ctx, blst, &out));
	cmp1.ptr = (oct_String*)out.object.self.self;
	TEST(oct_BStringCString_equals(ctx, cmp1, "Really", &eq));
	assert(eq);
	
	TEST(oct_Object_destroyOwned(ctx, out.object));
	TEST(oct_Vector_destroyOwned(ctx, rest));
	TEST(oct_Vector_destroyOwned(ctx, lst));

	oct_Runtime_destroy(rt, &error);
}

int main(int argc, char** argv) {
	const char* error;
	oct_BCharstream stream;
	oct_ReadResult rr;
	oct_BReader reader;
	oct_Runtime* rt = oct_Runtime_create(&error);
	oct_Context* ctx = oct_Runtime_currentContext(rt);
	oct_OString str;
	oct_BString bstr;
	oct_OStringstream ss;
	oct_BStringstream bss;
	oct_Any evalResult;
    oct_BPrintable prn;
	oct_BGeneric bself;
	reader.ptr = ctx->reader;

	oct_String_createOwnedFromCString(ctx, "- . ! ? 1 2 3 -37 1.5 0.34 .34 1e16 -0.8 -.8 -.main .main -main \"hej\" \"hell o workdl\" (quote (this is a (nested) \"list\" of 8 readables )) () (quote a) hello (def hello \"Hello!\") hello hello", &str);

	// Borrow string pointer
	bstr.ptr = str.ptr;

	oct_OStringstream_create(ctx, bstr, &ss);

	bss.ptr = ss.ptr;
	oct_Stringstream_asCharStream(ctx, bss, &stream);

	while(oct_True) {
		oct_Reader_read(ctx, reader, stream, &rr);
		if(rr.variant == OCT_READRESULT_ERROR) {
			break;
		}
		if(rr.result.variant == OCT_OOBJECTOPTION_OBJECT) {
			bself.self = rr.result.object.self.self;
            if(!oct_Object_as(ctx, bself, rr.result.object.vtable->type, rt->builtInProtocols.Printable, (oct_BObject*)&prn)) {
		        printf("#<UNPRINTABLE: %p>", bself.self);
                oct_Context_clearError(ctx);
            }
            else {
                oct_Printable_print(ctx, prn);
            }

			oct_Compiler_eval(ctx, rr.result.object, &evalResult);
			
			if(evalResult.variant == OCT_ANY_NOTHING) {
				printf(" => Nothing\n");
			}
			else {
                if(!oct_Object_as(ctx, evalResult.bobject.self, evalResult.bobject.vtable->type, rt->builtInProtocols.Printable, (oct_BObject*)&prn)) {
                    printf("%s\n", &ctx->err.berror.ptr->message.ptr->utf8Data.ptr->data[0]);
                    oct_Context_clearError(ctx);
                }
                else {
					printf(" => ");
                    oct_Printable_print(ctx, prn);
					printf("\n");
                }
                if(evalResult.variant == OCT_ANY_OOBJECT) {
                    oct_Object_destroyOwned(ctx, evalResult.oobject);
                }
            }
		}
	};

	// End of scope, destroy in reverse order.
	oct_OStringstream_destroy(ctx, ss);
	oct_String_destroyOwned(ctx, str);
	oct_Runtime_destroy(rt, &error);

	StringTests();
	NamespaceTests();
	defTest();
	testListAsSeq();
	testVecAsSeq();

#ifdef OCT_DEBUG
	oct_ExchangeHeap_report(NULL);
#endif
	return 0;
}
