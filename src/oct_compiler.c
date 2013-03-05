#include "oct_compiler.h"
#include "oct_symbol.h"
#include "oct_namespace.h"
#include "oct_context.h"
#include "oct_runtime.h"
#include "oct_pointertype.h"
#include "oct_list.h"
#include "oct_object.h"

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

static oct_Bool eval_sym(struct oct_Context* ctx, oct_OSymbol sym, oct_Any* out_result) {
	oct_BNamespace ns;
	oct_BSymbol bsym;
	oct_BHashtableKey key;
	oct_Bool result = oct_True;

	printf("eval_sym\n");

	ns.ptr = ctx->ns;
	bsym.ptr = sym.ptr;
	CHECK(oct_Symbol_asHashtableKey(ctx, bsym, &key));
	CHECK(oct_Namespace_lookup(ctx, ns, key, out_result));

	goto end;
error:
	result = oct_False;
end:
	return oct_Symbol_destroyOwned(ctx, sym) && result;
}

static oct_Bool eval_def(struct oct_Context* ctx, oct_OList olist, oct_Any* out_result) {
	// (def <symbol>)
	// (def <symbol> <value>)
	oct_BNamespace ns;
	oct_Uword count;
	oct_OHashtableKey key;
    oct_BList blist;
    oct_OListOption listOpt;
    oct_ObjectOption tmp;
	oct_BSymbol bsym;
	oct_Bool result = oct_True;

	printf("eval_def\n");
    
	blist.ptr = olist.ptr;
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
	CHECK(oct_Symbol_asHashtableKey(ctx, bsym, (oct_BHashtableKey*)&key));
	// eval second argument if we have one
	if(count == 3) {
        CHECK(oct_List_nth(ctx, olist, 1, &tmp, &olist));
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
		result = oct_Namespace_bind(ctx, ns, key, *out_result) && result;
	}
	return result;
}

static oct_Bool eval_list(oct_Context* ctx, oct_OList olist, oct_Any* out_result) {
	oct_Bool eq;
	oct_BObject bob;
	oct_BString bstr;
	oct_BList blist;
	oct_BSymbol bsym;
	oct_Bool result = oct_True;

	CHECK(oct_List_emptyp(ctx, blist, &eq));
	if(eq) {
		out_result->variant = OCT_OBJECTOPTION_OBJECT;
		return oct_List_asObject(ctx, olist, &out_result->object);
	}
	blist.ptr = olist.ptr;
	CHECK(oct_List_borrowFirst(ctx, blist, &bopt));
	if(bopt.variant == OCT_OBJECTOPTION_OBJECT) {
		bob.object = bopt.object.object;
		CHECK(oct_Object_symbolp(ctx, bob, &eq));
		// if the first element of the list is not a quote symbol then this is a function call
		if(eq) {
			bsym.ptr = (oct_Symbol*)bob.object.object;
			bstr.ptr = bsym.ptr->name.ptr;
			// TODO: dispatch table for built ins
			CHECK(oct_BStringCString_equals(ctx, bstr, "def", &eq));
			if(eq) {
				return eval_def(ctx, olist, out_result);
			}
		}
	}

	goto end;
error:
	result = oct_False;
end:
	return oct_List_destroyOwned(ctx, olist) && result; // TODO: Finish implementing this function :)
}

static oct_Bool eval_string(oct_Context* ctx, oct_OString ostr, oct_ObjectOption* out_result) {
	out_result->variant = OCT_OBJECTOPTION_OBJECT;
	return oct_String_asObject(ctx, ostr, &out_result->object);
}

typedef struct VTable {
	oct_BType type;
	oct_U8 data[];
} VTable;

oct_Bool oct_Compiler_eval(struct oct_Context* ctx, oct_OObject form, oct_Any* out_result) {
	oct_BType t;
	oct_BObject bob;
	oct_OSymbol sym;
	oct_OString str;
	oct_OList lst;
	oct_Bool result = oct_True;

	bob.object = form.object;

	t = ((VTable*)form.object.vtable)->type;
    if(is_symbol(ctx, t)) {
		sym.ptr = (oct_Symbol*)form.object.object;
        return eval_sym(ctx, sym, out_result);
    }
	else if(is_list(ctx, t)) {
		lst.ptr = (oct_List*)form.object.object;
		return eval_list(ctx, lst, out_result);
	}
	else if(is_string(ctx, t)) {
		str.ptr = (oct_String*)form.object.object;
		return eval_string(ctx, str, out_result);
	}

	goto end;
error:
	result = oct_False;
end:
	return result;  // TODO: Finish implementing this function :)
}
