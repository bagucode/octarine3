#include "oct_compiler.h"
#include "oct_symbol.h"
#include "oct_namespace.h"
#include "oct_context.h"
#include "oct_runtime.h"
#include "oct_pointertype.h"

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

static oct_Bool eval_def(struct oct_Context* ctx, oct_Seq args, oct_AnyOption* out_result) {
	// (def <symbol>)
	// (def <symbol> <value>)
	oct_BNamespace ns;
	oct_BReadable readable;
	oct_OReadableOption first;
	oct_OReadableOption second;
	oct_Uword count;
	oct_OSymbol sym;

	printf("eval_def\n");

	if(!oct_ReadableList_count(ctx, args, &count)) {
		return oct_False;
	}
	if(count < 1 || count > 2) {
		// wrong number of args
		return oct_False;
	}
	if(!oct_ReadableList_first(ctx, args, &first)) {
		return oct_False;
	}
	if(first.readable.ptr->variant != OCT_READABLE_SYMBOL) {
		// expected symbol, was ... something else
		return oct_False;
	}
	// eval second argument if we have one
	if(count == 2) {
		if(!oct_ReadableList_nth(ctx, args, 1, &second)) {
			return oct_False;
		}
		readable.ptr = second.readable.ptr;
		if(!oct_Compiler_eval(ctx, readable, out_result)) {
			return oct_False;
		}
	}
	else {
		out_result->variant = OCT_ANYOPTION_NOTHING;
	}
	// bind in current namespace
	ns.ptr = ctx->ns;
	sym.ptr = &first.readable.ptr->symbol;
	return oct_Namespace_bind(ctx, ns, sym, *out_result);
}

oct_Bool oct_Compiler_eval(struct oct_Context* ctx, oct_BReadable readable, oct_AnyOption* out_result) {
	oct_BString bstring;
	oct_OReadableOption first;
	oct_BReadableList lst;
	oct_ReadableListOption rest;
	oct_Bool eq;
	oct_BType btype;

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
	else if(readable.ptr->variant == OCT_READABLE_STRING) {
		out_result->variant = OCT_ANYOPTION_ANY;
		oct_Any_setPtr(ctx, &out_result->any, &readable.ptr->string);
		oct_Any_setPtrKind(ctx, &out_result->any, OCT_POINTER_OWNED);
		btype.ptr = ctx->rt->builtInTypes.String;
		oct_Any_setType(ctx, &out_result->any, btype);
		return oct_True;
	}
}
