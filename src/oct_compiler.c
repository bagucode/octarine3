#include "oct_compiler.h"
#include "oct_symbol.h"
#include "oct_namespace.h"
#include "oct_context.h"
#include "oct_runtime.h"
#include "oct_readable.h"

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
// template  -> create template (not needed?)
// variadic  -> create variadic type
// struct    -> create struct
// array     -> create array or fixed size array (overload)
// interface -> create interface
// Pointer types are implicitly created for each type

static oct_Bool eval_sym(struct oct_Context* ctx, oct_Symbol* sym, oct_AnyOption* out_result) {
	oct_BNamespace ns;
	oct_BSymbol bsym;

	printf("eval_sym\n");

	ns.ptr = ctx->ns;
	bsym.ptr = sym;
	return oct_Namespace_lookup(ctx, ns, bsym, out_result);
}

static oct_Bool eval_def(struct oct_Context* ctx, oct_BReadableList args, oct_AnyOption* out_result) {
	oct_BNamespace ns;
	oct_BReadable readable;
	oct_BReadableList lst;
	oct_OReadableOption first;
	oct_ReadableListOption rest;

	printf("eval_def\n");

	if(args.ptr->readable.variant == OCT_OREADABLEOPTION_NOTHING) {
		// arg list empty, should this be an error result rather than an "exception"?
		return oct_False;
	}
	readable.ptr = args.ptr->readable.readable.ptr;
	if(oct_ReadableList_first(ctx, lst, &first)) {
		return oct_False;
	}
	// first argument needs to be a symbol
	if(first.variant == OCT_OREADABLEOPTION_NOTHING) {
		// weird
		return oct_False;
	}
	if(first.readable.ptr->variant != OCT_READABLE_SYMBOL) {
		// expected symbol, was ... something else
		return oct_False;
	}


	ns.ptr = ctx->ns;
	
	if(!oct_Compiler_eval(ctx, arg, out_result)) {
		return oct_False;
	}
	// as a side effect of def, the eval result of arg is bound to sym in the current namespace
	if(!oct_Namespace_bind(ctx, ns, sym, *out_result)) {
		return oct_False;
	}
	return oct_True;
}

oct_Bool oct_Compiler_eval(struct oct_Context* ctx, oct_BReadable readable, oct_AnyOption* out_result) {
	oct_BString bstring;
	oct_OReadableOption first;
	oct_BReadableList lst;
	oct_ReadableListOption rest;
	oct_Bool eq;

    if(readable.ptr->variant == OCT_READABLE_SYMBOL) {
        return eval_sym(ctx, &readable.ptr->symbol, out_result);
    }
	else if(readable.ptr->variant == OCT_READABLE_LIST) {
		lst.ptr = &readable.ptr->list;
		if(!oct_ReadableList_first(ctx, lst, &first)) {
			return oct_False;
		}
		if(first.variant == OCT_OREADABLEOPTION_OREADABLE) {
			// if the first element of the list is not a quote symbol then this is a function call
			if(first.readable.ptr->variant == OCT_READABLE_SYMBOL) {
				bstring.ptr = first.readable.ptr->symbol.name.ptr;
				// TODO: dispatch table for built ins
				if(!oct_BStringCString_equals(ctx, bstring, "def", &eq)) {
					return oct_False;
				}
				if(eq) {
					if(!oct_ReadableList_rest(ctx, lst, &rest)) {
						return oct_False;
					}
					if(rest.variant == OCT_READABLE_LINKED_LIST_OPTION_LIST) {
						lst.ptr = rest.rll.ptr;
						return eval_def(ctx, lst, out_result);
					}
				}
			}
		}
	}
}
