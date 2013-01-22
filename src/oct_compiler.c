#include "oct_compiler.h"
#include "oct_symbol.h"
#include "oct_namespace.h"
#include "oct_context.h"
#include "oct_runtime.h"

// Built in forms:
// def    -> establish binding in current namespace
// let    -> establish binding in current frame
// if     -> conditional execution
// do     -> eval multiple forms and return result of last
// quote  -> suppress evaluation
// fn     -> create function (lambda)
// throw? -> throw exception (if used, also need catch and finally?)
// .      -> get value of struct member
// !      -> set value of struct member

// one creation form for each kind of data type...
// struct -> create struct
// ...

// 

static oct_Bool eval_sym(struct oct_Context* ctx, oct_Symbol* sym, oct_AnyOption* out_result) {
	oct_BNamespace ns;
	oct_BSymbol bsym;
	ns.ptr = ctx->ns;
	bsym.ptr = sym;
	return oct_Namespace_lookup(ctx, ns, bsym, out_result);
}

static oct_Bool eval_def(struct oct_Context* ctx, oct_BReadable args, oct_AnyOption* out_result) {
	// Need Seq interface?
}

oct_Bool oct_Compiler_eval(struct oct_Context* ctx, oct_BReadable readable, oct_AnyOption* out_result) {

}
