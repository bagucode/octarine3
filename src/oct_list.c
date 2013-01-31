#include "oct_list.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_context.h"
#include "oct_readable_pointers.h"
#include "oct_exchangeheap.h"
#include "oct_managedheap.h"

#include <stddef.h>
#include <stdlib.h>

oct_Bool _oct_List_initType(struct oct_Context* ctx) {
	oct_Type* t = ctx->rt->builtInTypes.List;
	oct_Bool result;
	t->variant = OCT_TYPE_STRUCT;
	t->structType.alignment = 0;
	t->structType.size = sizeof(oct_List);
	result = oct_OAField_alloc(ctx, 2, &t->structType.fields);
	if(!result) {
		return result;
	}
	t->structType.fields.ptr->data[0].offset = offsetof(oct_List, data);
	t->structType.fields.ptr->data[0].type.ptr = ctx->rt->builtInTypes.AnyOption;
	t->structType.fields.ptr->data[1].offset = offsetof(oct_List, next);
	t->structType.fields.ptr->data[1].type.ptr = ctx->rt->builtInTypes.OListOption;
	return oct_True;
}

oct_Bool _oct_OList_initType(struct oct_Context* ctx) {
	oct_Type* t = ctx->rt->builtInTypes.OList;
	t->variant = OCT_TYPE_POINTER;
	t->pointerType.kind = OCT_POINTER_OWNED;
	t->pointerType.type.ptr = ctx->rt->builtInTypes.List;
	return oct_True;
}

oct_Bool _oct_MList_initType(struct oct_Context* ctx) {
	oct_Type* t = ctx->rt->builtInTypes.MList;
	t->variant = OCT_TYPE_POINTER;
	t->pointerType.kind = OCT_POINTER_MANAGED;
	t->pointerType.type.ptr = ctx->rt->builtInTypes.List;
	return oct_True;
}

oct_Bool _oct_BList_initType(struct oct_Context* ctx) {
	oct_Type* t = ctx->rt->builtInTypes.BList;
	t->variant = OCT_TYPE_POINTER;
	t->pointerType.kind = OCT_POINTER_BORROWED;
	t->pointerType.type.ptr = ctx->rt->builtInTypes.List;
	return oct_True;
}

oct_Bool _oct_OListOption_initType(struct oct_Context* ctx) {
    oct_Type* t = ctx->rt->builtInTypes.OListOption;
    oct_Bool result;
    t->variant = OCT_TYPE_VARIADIC;
    t->variadicType.alignment = 0;
    t->variadicType.size = sizeof(oct_OListOption);
    result = oct_OABType_alloc(ctx, 2, &t->variadicType.types);
    if(!result) {
        return oct_False;
    }
    t->variadicType.types.ptr->data[0].ptr = ctx->rt->builtInTypes.Nothing;
    t->variadicType.types.ptr->data[0].ptr = ctx->rt->builtInTypes.OList;
    return oct_True;
}

// Public

// Create & Destroy
oct_Bool oct_List_ctor(struct oct_Context* ctx, oct_List* self) {
    self->data.variant = OCT_ANYOPTION_NOTHING;
    self->next.variant = OCT_LISTOPTION_NOTHING;
    return oct_True;
}

oct_Bool oct_List_createOwned(struct oct_Context* ctx, oct_OList* out_result) {
    if(!oct_ExchangeHeap_alloc(ctx, sizeof(oct_List), (void**)&out_result->ptr)) {
        return oct_False;
    }
    return oct_List_ctor(ctx, out_result->ptr);
}

oct_Bool oct_List_createManaged(struct oct_Context* ctx, oct_MList* out_result) {
	oct_BType bt;
	bt.ptr = ctx->rt->builtInTypes.List;
	if(!oct_ManagedHeap_alloc(ctx, bt, (void**)&out_result->ptr)) {
		return oct_False;
	}
	return oct_List_ctor(ctx, out_result->ptr);
}

oct_Bool oct_List_borrowOwned(struct oct_Context* ctx, oct_OList lst, oct_BList* out_lst) {
	out_lst->ptr = lst.ptr;
	return oct_True;
}

oct_Bool oct_List_borrowManaged(struct oct_Context* ctx, oct_MList lst, oct_BList* out_lst) {
	out_lst->ptr = lst.ptr;
	return oct_True;
}

oct_Bool oct_List_destroyOwned(struct oct_Context* ctx, oct_OList lst) {
    oct_Bool result = oct_True;
    oct_List* prev;
    while(oct_True) {
        if(lst.ptr->data.variant == OCT_ANYOPTION_ANY) {
            result = result && oct_Any_dtor(ctx, lst.ptr->data.any);
        }
        if(lst.ptr->next.variant == OCT_LISTOPTION_NOTHING) {
            oct_ExchangeHeap_free(ctx, lst.ptr);
            break;
        }
        prev = lst.ptr;
        lst.ptr = lst.ptr->next.list.ptr;
        oct_ExchangeHeap_free(ctx, prev);
    }
    return result;
}

// Util
oct_Bool oct_List_getType(struct oct_Context* ctx, struct oct_BType* out_type) {
	out_type->ptr = ctx->rt->builtInTypes.List;
	return oct_True;
}

// Operations
oct_Bool oct_List_prepend(struct oct_Context* ctx, oct_OList lst, oct_Any obj, oct_OList* out_lst) {
	if(!oct_List_createOwned(ctx, out_lst)) {
		return oct_False;
	}

	out_lst->ptr->next.variant = OCT_LISTOPTION_LIST;
	out_lst->ptr->next.list.ptr = lst.ptr;

	out_lst->ptr->data.variant = OCT_ANYOPTION_ANY;
	if(!oct_Any_move(ctx, obj, &out_lst->ptr->data.any)) {
		oct_List_destroyOwned(ctx, *out_lst);
		return oct_False;
	}
	return oct_True;
}

oct_Bool oct_List_append(struct oct_Context* ctx, oct_BList lst, oct_Any obj) {
	oct_Bool b;
	if(!oct_List_emptyp(ctx, lst, &b)) {
		return oct_False;
	}
	if(b) {
		if(!oct_Any_move(ctx, obj, &lst.ptr->data.any)) {
			return oct_False;
		}
		lst.ptr->data.variant = OCT_ANYOPTION_ANY;
	}
	else {
		while(lst.ptr->next.variant == OCT_LISTOPTION_LIST) {
			lst.ptr = lst.ptr->next.list.ptr;
		}
		if(!oct_List_createOwned(ctx, &lst.ptr->next.list)) {
			return oct_False;
		}
		if(!oct_Any_move(ctx, obj, &lst.ptr->next.list.ptr->data.any)) {
			oct_List_destroyOwned(ctx, lst.ptr->next.list);
			return oct_False;
		}
		lst.ptr->next.list.ptr->data.variant = OCT_ANYOPTION_ANY;
		lst.ptr->next.variant = OCT_LISTOPTION_LIST;
	}
	return oct_True;
}

oct_Bool oct_List_emptyp(struct oct_Context* ctx, oct_BList lst, oct_Bool* out_result) {
	if(lst.ptr->data.variant == OCT_ANYOPTION_NOTHING
		&& lst.ptr->next.variant == OCT_LISTOPTION_NOTHING) {
		*out_result = oct_True;
	}
	else {
		*out_result = oct_False;
	}
	return oct_True;
}

oct_Bool oct_List_first(struct oct_Context* ctx, oct_BList lst, oct_AnyOption* out_any) {
	if(lst.ptr->data.variant == OCT_ANYOPTION_NOTHING) {
		out_any->variant = OCT_ANYOPTION_NOTHING;
	}
	else {
		if(!oct_Any_borrow(ctx, lst.ptr->data.any, &out_any->any)) {
			return oct_False;
		}
		out_any->variant = OCT_ANYOPTION_ANY;
	}
	return oct_True;
}

oct_Bool oct_List_rest(struct oct_Context* ctx, oct_BList lst, oct_BListOption* out_lst) {
	if(lst.ptr->next.variant == OCT_LISTOPTION_NOTHING) {
		out_lst->variant = OCT_LISTOPTION_NOTHING;
	}
	else {
		out_lst->variant = OCT_LISTOPTION_LIST;
		out_lst->list.ptr = lst.ptr->next.list.ptr;
	}
	return oct_True;
}

oct_Bool oct_List_count(struct oct_Context* ctx, oct_BList lst, oct_Uword* out_count) {
	oct_Bool emptyp;
	if(!oct_List_emptyp(ctx, lst, &emptyp)) {
		return oct_False;
	}
	if(!emptyp) {
		*out_count = 1;
	}
	else {
		*out_count = 0;
	}
	while(lst.ptr->next.variant == OCT_LISTOPTION_LIST) {
		++(*out_count);
		lst.ptr = lst.ptr->next.list.ptr;
	}
	return oct_True;
}

oct_Bool oct_List_nth(struct oct_Context* ctx, oct_BList lst, oct_Uword idx, oct_AnyOption* out_any) {
	oct_Uword current;
	for(current = 0; current < idx && lst.ptr->next.variant == OCT_LISTOPTION_LIST; ++current) {
		lst.ptr = lst.ptr->next.list.ptr;
	}
	if(current == idx && lst.ptr->data.variant == OCT_ANYOPTION_ANY) {
		if(!oct_Any_borrow(ctx, lst.ptr->data.any, &out_any->any)) {
			return oct_False;
		}
		out_any->variant = OCT_ANYOPTION_ANY;
	}
	else {
		out_any->variant = OCT_ANYOPTION_NOTHING;
	}
	return oct_True;
}
