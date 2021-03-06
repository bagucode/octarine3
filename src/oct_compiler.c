#include "oct_compiler.h"
#include "oct_symbol.h"
#include "oct_namespace.h"
#include "oct_context.h"
#include "oct_runtime.h"
#include "oct_pointertype.h"
#include "oct_list.h"
#include "oct_object.h"
#include "oct_copyable.h"

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

static oct_Bool is_symbol(oct_Context* ctx, oct_CType t) {
	return ctx->rt->builtInTypes.Symbol.ptr == t.ptr;
}

static oct_Bool is_string(oct_Context* ctx, oct_CType t) {
	return ctx->rt->builtInTypes.String.ptr == t.ptr;
}

static oct_Bool is_list(oct_Context* ctx, oct_CType t) {
	return ctx->rt->builtInTypes.List.ptr == t.ptr;
}

static oct_Bool eval_sym(struct oct_Context* ctx, oct_OObject form, oct_Any* out_result) {
	oct_BNamespace ns;
	oct_BSymbol bsym;
	oct_OSymbol osym;
	oct_BHashtableKey key;
	oct_Bool result = oct_True;

	ns.ptr = ctx->ns;
	osym.ptr = (oct_Symbol*)form.self.self;
	bsym.ptr = (oct_Symbol*)form.self.self;
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
	oct_OList formlist;
    oct_BList blist;
    oct_OList rest;
    oct_OObjectOption tmp;
	oct_BSymbol bsym;
	oct_Any nsVal;
	oct_BCopyable bcopyable;
	oct_BGeneric bself;
	oct_Bool result = oct_True;

    formlist.ptr = (oct_List*)form.self.self;
	blist.ptr = (oct_List*)form.self.self;
    CHECK(oct_List_count(ctx, blist, &count));
	if(count < 2 || count > 3) {
		oct_Context_setErrorWithCMessage(ctx, "Wrong number of arguments to def");
		goto error;
	}
    // Drop "def"
    CHECK(oct_List_rest(ctx, blist, &rest));
	CHECK(oct_List_destroyOwned(ctx, formlist)); // destroy head
    formlist = rest;
	blist.ptr = formlist.ptr;
    CHECK(oct_List_first(ctx, blist, &tmp));
	if(tmp.object.vtable->type.ptr != ctx->rt->builtInTypes.Symbol.ptr) {
    	oct_Context_setErrorWithCMessage(ctx, "First argument to def must be a Symbol");
		goto error;
    }
	bsym.ptr = (oct_Symbol*)tmp.object.self.self;
	CHECK(oct_Symbol_asHashtableKey(ctx, bsym, &key.borrowed));
	key.variant = OCT_HASHTABLEKEYOPTION_OWNED;
	// eval second argument if we have one
	if(count == 3) {
        CHECK(oct_List_first(ctx, blist, &tmp));
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
	result = oct_List_destroyOwned(ctx, formlist) && result;
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


static oct_Bool eval_quote(oct_Context* ctx, oct_OObject form, oct_Any* out_result) {
    oct_OList olist;
    oct_BList blist;
    oct_OList rest;
	oct_OObjectOption first;
    oct_Bool result = oct_True;
    out_result->variant = OCT_ANY_NOTHING;
    
	//oct_Object_destroyOwned(ctx, form);

	// Drop quote symbol
    blist.ptr = (oct_List*)form.self.self;
    CHECK(oct_List_rest(ctx, blist, &rest)); // detach ...
    olist.ptr = (oct_List*)form.self.self;
    CHECK(oct_List_destroyOwned(ctx, olist)); // ... and drop head
	// Return the first item in the list that is left. Anything else will be silently
	// eaten by the quote form. Throw an error here instead if there is more than one item?
	blist.ptr = rest.ptr;
	CHECK(oct_List_first(ctx, blist, &first));
	if(first.variant == OCT_OOBJECTOPTION_OBJECT) {
		out_result->variant = OCT_ANY_OOBJECT;
		out_result->oobject = first.object;
	}
	CHECK(oct_List_destroyOwned(ctx, rest));
    
    goto end;
error:
    result = oct_False;
end:
    
    return result;
}


static oct_Bool eval_list(oct_Context* ctx, oct_OObject form, oct_Any* out_result) {
	oct_Bool eq;
	oct_OList olist;
	oct_BList blist;
	oct_BString bstr;
	oct_BObjectOption val;
	oct_Bool result = oct_True;

	olist.ptr = (oct_List*)form.self.self;
	blist.ptr = (oct_List*)form.self.self;

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
            CHECK(oct_BStringCString_equals(ctx, bstr, "quote", &eq));
            if(eq) {
                return eval_quote(ctx, form, out_result);
            }
		}
	}

	// TODO: Finish implementing this function :)
	// Just return nothing here for now
	out_result->variant = OCT_ANY_NOTHING;
	out_result->nothing.dummy = 0;

	goto end;
error:
	result = oct_False;
end:
	return oct_List_destroyOwned(ctx, olist) && result;
}

static oct_Bool is_atom(oct_Context* ctx, oct_CType t) {
	// TODO: should this return true for any opaque type?
	oct_Bool result = oct_False;
	if(t.ptr == ctx->rt->builtInTypes.String.ptr) {
		result = oct_True;
	}
    else if(t.ptr == ctx->rt->builtInTypes.U8.ptr) {
        result = oct_True;
    }
    else if(t.ptr == ctx->rt->builtInTypes.I8.ptr) {
        result = oct_True;
    }
    else if(t.ptr == ctx->rt->builtInTypes.U16.ptr) {
        result = oct_True;
    }
    else if(t.ptr == ctx->rt->builtInTypes.I16.ptr) {
        result = oct_True;
    }
    else if(t.ptr == ctx->rt->builtInTypes.U32.ptr) {
        result = oct_True;
    }
    else if(t.ptr == ctx->rt->builtInTypes.I32.ptr) {
        result = oct_True;
    }
    else if(t.ptr == ctx->rt->builtInTypes.U64.ptr) {
        result = oct_True;
    }
    else if(t.ptr == ctx->rt->builtInTypes.I64.ptr) {
        result = oct_True;
    }
    else if(t.ptr == ctx->rt->builtInTypes.F32.ptr) {
        result = oct_True;
    }
    else if(t.ptr == ctx->rt->builtInTypes.F64.ptr) {
        result = oct_True;
    }
    else if(t.ptr == ctx->rt->builtInTypes.Uword.ptr) {
        result = oct_True;
    }
    else if(t.ptr == ctx->rt->builtInTypes.Word.ptr) {
        result = oct_True;
    }
    else if(t.ptr == ctx->rt->builtInTypes.Char.ptr) {
        result = oct_True;
    }
    else if(t.ptr == ctx->rt->builtInTypes.Bool.ptr) {
        result = oct_True;
    }
	return result;
}

static oct_Bool eval_atom(oct_Context* ctx, oct_OObject form, oct_Any* out_result) {
	// Just return it as-is. The vtable should already be for Object.
	out_result->variant = OCT_ANY_OOBJECT;
	out_result->oobject = form;
	return oct_True;
}

typedef struct VTable {
	oct_CType type;
	oct_U8 data[];
} VTable;

oct_Bool oct_Compiler_eval(struct oct_Context* ctx, oct_OObject form, oct_Any* out_result) {
	oct_CType t;

	t = ((VTable*)form.vtable)->type;
    if(is_symbol(ctx, t)) {
        return eval_sym(ctx, form, out_result);
    }
	else if(is_list(ctx, t)) {
		return eval_list(ctx, form, out_result);
	}
	else if(is_atom(ctx, t)) {
		return eval_atom(ctx, form, out_result);
	}

	// TODO: finish implementing this function :)
	// Just return nothing here for now
	out_result->nothing.dummy = 0;
	out_result->variant = OCT_ANY_NOTHING;
	return oct_Object_destroyOwned(ctx, form);
}
