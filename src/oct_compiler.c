#include "oct_compiler.h"
#include "oct_symbol.h"
#include "oct_namespace.h"
#include "oct_context.h"
#include "oct_runtime.h"
#include "oct_pointertype.h"
#include "oct_list.h"
#include "oct_object.h"
#include "oct_copyable.h"

// DEBUG
#include <stdio.h>
// END DEBUG

// Built in forms:
// def    -> establish binding in current namespace
// let    -> establish binding in current frame
// if     -> conditional execution
// do     -> eval multiple forms and return result of last
// quote  -> suppress evaluation
// fn     -> create function (lambda)
// throw  -> throw exception (if used, also need catch and finally?)
// ?      -> get value
// !      -> set value

// One creation form for each kind of data type.
// prototype -> create prototype
// variadic  -> create variadic type
// struct    -> create struct
// array     -> create array or fixed size array (overload)
// protocol -> create protocol
// Pointer types are implicitly created for each type

#define CHECK(X) if(!X) goto error;

static oct_Bool is_symbol(oct_Context* ctx, oct_BType t) {
	return ctx->rt->builtInTypes.Symbol.ptr == t.ptr;
}

static oct_Bool is_string(oct_Context* ctx, oct_BType t) {
	return ctx->rt->builtInTypes.String.ptr == t.ptr;
}

static oct_Bool is_list(oct_Context* ctx, oct_BType t) {
	return ctx->rt->builtInTypes.List.ptr == t.ptr;
}

static oct_Bool eval_sym(struct oct_Context* ctx, oct_OObject form, oct_Any* out_result) {
	oct_BNamespace ns;
	oct_BSymbol bsym;
	oct_OSymbol osym;
	oct_BHashtableKey key;
	oct_Bool result = oct_True;

	printf("eval_sym\n");

	ns.ptr = ctx->ns;
	osym.ptr = form.self.self;
	bsym.ptr = form.self.self;
	CHECK(oct_Symbol_asHashtableKey(ctx, bsym, &key));
	CHECK(oct_Namespace_lookup(ctx, ns, key, out_result));

	goto end;
error:
	result = oct_False;
end:
	return oct_Symbol_destroyOwned(ctx, osym) && result;
}

static oct_Bool eval_def(struct oct_Context* ctx, oct_OObject form, oct_Any* out_result) {
	// (def <symbol>)
	// (def <symbol> <value>)
	oct_BNamespace ns;
	oct_Uword count;
	oct_HashtableKeyOption key;
	oct_OList olist;
    oct_BList blist;
    oct_OListOption listOpt;
    oct_OObjectOption tmp;
	oct_BSymbol bsym;
	oct_Any nsVal;
	oct_BCopyable bcopyable;
	oct_BSelf bself;
	oct_Bool result = oct_True;

	printf("eval_def\n");
    
    olist.ptr = form.self.self;
	blist.ptr = form.self.self;
    CHECK(oct_List_count(ctx, blist, &count));
	if(count < 2 || count > 3) {
		oct_Context_setErrorWithCMessage(ctx, "Wrong number of arguments to def");
		goto error;
	}
    // Drop "def"
    CHECK(oct_List_rest(ctx, blist, &listOpt));
    olist.ptr = listOpt.list.ptr;
    CHECK(oct_List_first(ctx, olist, &tmp, &olist));
	if(tmp.object.vtable->type.ptr != ctx->rt->builtInTypes.Symbol.ptr) {
    	oct_Context_setErrorWithCMessage(ctx, "First argument to def must be a Symbol");
		goto error;
    }
	bsym.ptr = (oct_Symbol*)tmp.object.self.self;
	CHECK(oct_Symbol_asHashtableKey(ctx, bsym, &key.borrowed));
	key.variant = OCT_HASHTABLEKEYOPTION_OWNED;
	// eval second argument if we have one
	if(count == 3) {
        CHECK(oct_List_first(ctx, olist, &tmp, &olist));
		CHECK(oct_Compiler_eval(ctx, tmp.object, out_result));
	}
	else {
		out_result->variant = OCT_ANY_NOTHING;
		out_result->nothing.dummy = 0;
	}

	goto end;
error:
	result = oct_False;
end:
	// clean up
	result = oct_List_destroyOwned(ctx, olist) && result;
	if(result) {
		// bind in current namespace
		ns.ptr = ctx->ns;
		nsVal = *out_result;
		if(nsVal.variant == OCT_ANY_OOBJECT) {
			// if the result of the call to eval is an owned object we need to copy it because
			// otherwise we will not be able to both store it in the ns and return it
			bself.self = out_result->bobject.self.self;
			if(!oct_Object_as(ctx, bself, out_result->bobject.vtable->type, ctx->rt->builtInProtocols.Copyable, (oct_BObject*)&bcopyable)) {
				oct_Object_destroyOwned(ctx, out_result->oobject);
				return oct_False;
			}
			if(!oct_Copyable_copyOwned(ctx, bcopyable, &nsVal.oobject)) {
				oct_Object_destroyOwned(ctx, out_result->oobject);
				return oct_False;
			}
		}
		result = oct_Namespace_bind(ctx, ns, key, nsVal) && result;
	}
	else if(out_result->variant == OCT_ANY_OOBJECT) {
		oct_Object_destroyOwned(ctx, out_result->oobject);
	}
	return result;
}

static oct_Bool eval_list(oct_Context* ctx, oct_OObject form, oct_Any* out_result) {
	oct_Bool eq;
	oct_OList olist;
	oct_BList blist;
	oct_BString bstr;
	oct_BObjectOption val;
	oct_Bool result = oct_True;

	olist.ptr = form.self.self;
	blist.ptr = form.self.self;

	CHECK(oct_List_emptyp(ctx, blist, &eq));
	if(eq) {
		out_result->variant = OCT_ANY_OOBJECT;
		out_result->oobject = form;
		return oct_True;
	}
	CHECK(oct_List_borrowFirst(ctx, blist, &val));
	if(val.variant == OCT_BOBJECTOPTION_OBJECT) {
		if(is_symbol(ctx, val.object.vtable->type)) {
			// if the first element of the list is not a quote symbol then this is a function call
			// TODO: dispatch table for built ins
			bstr.ptr = ((oct_Symbol*)val.object.self.self)->name.ptr;
			CHECK(oct_BStringCString_equals(ctx, bstr, "def", &eq));
			if(eq) {
				return eval_def(ctx, form, out_result);
			}
		}
	}

	goto end;
error:
	result = oct_False;
end:
	return oct_List_destroyOwned(ctx, olist) && result; // TODO: Finish implementing this function :)
}

static oct_Bool eval_string(oct_Context* ctx, oct_OObject form, oct_Any* out_result) {
	oct_OString ostr;
	ostr.ptr = form.self.self;
	out_result->variant = OCT_ANY_OOBJECT;
	return oct_String_asObjectOwned(ctx, ostr, &out_result->oobject);
}

typedef struct VTable {
	oct_BType type;
	oct_U8 data[];
} VTable;

oct_Bool oct_Compiler_eval(struct oct_Context* ctx, oct_OObject form, oct_Any* out_result) {
	oct_BType t;

	t = ((VTable*)form.vtable)->type;
    if(is_symbol(ctx, t)) {
        return eval_sym(ctx, form, out_result);
    }
	else if(is_list(ctx, t)) {
		return eval_list(ctx, form, out_result);
	}
	else if(is_string(ctx, t)) {
		return eval_string(ctx, form, out_result);
	}

	return oct_True;
}
