#include "oct_compiler.h"
#include "oct_symbol.h"
#include "oct_namespace.h"
#include "oct_context.h"
#include "oct_runtime.h"
#include "oct_pointertype.h"
#include "oct_list.h"

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

#define CHECK(X) if(!X) return oct_False;

static oct_Bool is_symbol(oct_Context* ctx, oct_BType t) {
	return ctx->rt->builtInTypes.Symbol == t.ptr;
}

static oct_Bool is_string(oct_Context* ctx, oct_BType t) {
	return ctx->rt->builtInTypes.String == t.ptr;
}

static oct_Bool is_list(oct_Context* ctx, oct_BType t) {
	return ctx->rt->builtInTypes.List == t.ptr;
}

static oct_Bool eval_sym(struct oct_Context* ctx, oct_Any asym, oct_AnyOption* out_result) {
	oct_BNamespace ns;
	oct_BSymbol bsym;

	printf("eval_sym\n");

    CHECK(oct_Any_getPtr(ctx, asym, (void**)&bsym.ptr));
	ns.ptr = ctx->ns;
	CHECK(oct_Namespace_lookup(ctx, ns, bsym, out_result));
	return oct_Any_dtor(ctx, asym);
}

static oct_Bool eval_def(struct oct_Context* ctx, oct_Any form, oct_AnyOption* out_result) {
	// (def <symbol>)
	// (def <symbol> <value>)
	oct_BNamespace ns;
	oct_Uword count;
	oct_OSymbol sym;
    oct_BList list;
    oct_BListOption listOpt;
    oct_AnyOption tmp;
	oct_Bool b;

	printf("eval_def\n");
    
    CHECK(oct_Any_getPtr(ctx, form, (void**)&list.ptr));
    CHECK(oct_List_count(ctx, list, &count));
	if(count < 2 || count > 3) {
		CHECK(oct_Context_setErrorWithCMessage(ctx, "Wrong number of arguments to def"));
		return oct_False;
	}
    // Drop "def"
    CHECK(oct_List_rest(ctx, list, &listOpt));
    list.ptr = listOpt.list.ptr;
    CHECK(oct_List_first(ctx, list, &tmp));
	CHECK(oct_Any_symbolp(ctx, tmp.any, &b));
    if(!b) {
    	CHECK(oct_Context_setErrorWithCMessage(ctx, "First argument to def must be a Symbol"));
        return oct_False;
    }
    CHECK(oct_Any_getPtr(ctx, tmp.any, (void**)&sym.ptr));
	// eval second argument if we have one
	if(count == 3) {
        CHECK(oct_List_nth(ctx, list, 1, &tmp));
		CHECK(oct_Compiler_eval(ctx, tmp.any, out_result));
	}
	else {
		out_result->variant = OCT_ANYOPTION_NOTHING;
	}
	// bind in current namespace
	ns.ptr = ctx->ns;
	return oct_Namespace_bind(ctx, ns, sym, *out_result);
}

static oct_Bool eval_list(oct_Context* ctx, oct_Any alist, oct_AnyOption* out_result) {
	oct_BList list;
	oct_Bool eq;
	oct_AnyOption aopt;
	oct_Symbol* sym;
	oct_BString bstr;

	CHECK(oct_Any_getPtr(ctx, alist, (void**)&list.ptr));
	CHECK(oct_List_emptyp(ctx, list, &eq));
	if(eq) {
		out_result->variant = OCT_ANYOPTION_ANY;
		CHECK(oct_Any_assign(ctx, alist, &out_result->any));
		return oct_Any_dtor(ctx, alist);
	}
	CHECK(oct_List_first(ctx, list, &aopt));
	if(aopt.variant == OCT_ANYOPTION_ANY) {
		CHECK(oct_Any_symbolp(ctx, aopt.any, &eq));
		// if the first element of the list is not a quote symbol then this is a function call
		if(eq) {
			CHECK(oct_Any_getPtr(ctx, aopt.any, (void**)&sym));
			bstr.ptr = sym->name.ptr;
			// TODO: dispatch table for built ins
			CHECK(oct_BStringCString_equals(ctx, bstr, "def", &eq));
			if(eq) {
				return eval_def(ctx, alist, out_result);
			}
		}
	}
	return oct_True; // TODO: Finish implementing this function :)
}

static oct_Bool eval_string(oct_Context* ctx, oct_Any astr, oct_AnyOption* out_result) {
		out_result->variant = OCT_ANYOPTION_ANY;
		CHECK(oct_Any_assign(ctx, astr, &out_result->any));
		return oct_Any_dtor(ctx, astr);
}

oct_Bool oct_Compiler_eval(struct oct_Context* ctx, oct_Any form, oct_AnyOption* out_result) {
	oct_AnyOption aopt;
	oct_BType t;

	CHECK(oct_Any_getType(ctx, form, &t));

	aopt.variant = OCT_ANYOPTION_ANY;
	aopt.any = form;

    if(is_symbol(ctx, t)) {
        return eval_sym(ctx, form, out_result);
    }
	else if(is_list(ctx, t)) {
		return eval_list(ctx, form, out_result);
	}
	else if(is_string(ctx, t)) {
		return eval_string(ctx, form, out_result);
	}

	return oct_True;  // TODO: Finish implementing this function :)
}
