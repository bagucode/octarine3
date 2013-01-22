#include "oct_compiler.h"
#include "oct_symbol.h"
#include "oct_namespace.h"
#include "oct_context.h"
#include "oct_runtime.h"
#include "oct_readable.h"

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
	ns.ptr = ctx->ns;
	bsym.ptr = sym;
	return oct_Namespace_lookup(ctx, ns, bsym, out_result);
}

static oct_Bool eval_def(struct oct_Context* ctx, oct_OSymbol sym, oct_BReadable arg, oct_AnyOption* out_result) {
	oct_BNamespace ns;
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
    if(readable.ptr->variant == OCT_READABLE_SYMBOL) {
        return eval_sym(ctx, &readable.ptr->symbol, out_result);
    }
}
