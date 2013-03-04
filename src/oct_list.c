#include "oct_list.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_context.h"
#include "oct_exchangeheap.h"
#include "oct_object.h"
#include "oct_object_vtable.h"

#include <stddef.h>
#include <stdlib.h>

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_List_init(struct oct_Context* ctx) {
	// List
	oct_BType t = ctx->rt->builtInTypes.List;
	t.ptr->variant = OCT_TYPE_STRUCT;
	t.ptr->structType.alignment = 0;
	t.ptr->structType.size = sizeof(oct_List);
	CHECK(oct_AField_createOwned(ctx, 2, &t.ptr->structType.fields));
	t.ptr->structType.fields.ptr->data[0].offset = offsetof(oct_List, data);
	t.ptr->structType.fields.ptr->data[0].type = ctx->rt->builtInTypes.OObjectOption;
	t.ptr->structType.fields.ptr->data[1].offset = offsetof(oct_List, next);
	t.ptr->structType.fields.ptr->data[1].type = ctx->rt->builtInTypes.OListOption;

	// OList
	t = ctx->rt->builtInTypes.OList;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_OWNED;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.List;

	// BList
	t = ctx->rt->builtInTypes.BList;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_BORROWED;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.List;

	// OListOption
	t = ctx->rt->builtInTypes.OListOption;
    t.ptr->variant = OCT_TYPE_VARIADIC;
    t.ptr->variadicType.alignment = 0;
    t.ptr->variadicType.size = sizeof(oct_OListOption);
    CHECK(oct_ABType_createOwned(ctx, 2, &t.ptr->variadicType.types));
    t.ptr->variadicType.types.ptr->data[0] = ctx->rt->builtInTypes.Nothing;
    t.ptr->variadicType.types.ptr->data[1] = ctx->rt->builtInTypes.OList;

	// BListOption
	t = ctx->rt->builtInTypes.BListOption;
    t.ptr->variant = OCT_TYPE_VARIADIC;
    t.ptr->variadicType.alignment = 0;
    t.ptr->variadicType.size = sizeof(oct_BListOption);
    CHECK(oct_ABType_createOwned(ctx, 2, &t.ptr->variadicType.types));
    t.ptr->variadicType.types.ptr->data[0] = ctx->rt->builtInTypes.Nothing;
    t.ptr->variadicType.types.ptr->data[1] = ctx->rt->builtInTypes.BList;

	return oct_True;
}


// Public

// Create & Destroy
oct_Bool oct_List_ctor(struct oct_Context* ctx, oct_List* self) {
	self->data.variant = OCT_OBJECTOPTION_NOTHING;
    self->next.variant = OCT_LISTOPTION_NOTHING;
    return oct_True;
}

oct_Bool oct_List_dtor(struct oct_Context* ctx, oct_List* self) {
    oct_Bool result = oct_True;
    oct_List* prev;

	// Don't free first node, could be on stack or managed heap
	if(self->data.variant == OCT_OBJECTOPTION_OBJECT) {
		result = oct_Object_destroyOwned(ctx, self->data.object);
		self->data.variant = OCT_OBJECTOPTION_NOTHING;
	}
	if(self->next.variant == OCT_LISTOPTION_LIST) {
		self = self->next.list.ptr;
		while(oct_True) {
			if(self->data.variant == OCT_OBJECTOPTION_OBJECT) {
				result = oct_Object_destroyOwned(ctx, self->data.object) && result;
			}
			if(self->next.variant == OCT_LISTOPTION_NOTHING) {
				break;
			}
			prev = self;
			self = self->next.list.ptr;
			oct_ExchangeHeap_freeRaw(ctx, prev);
		}
	}
    return result;
}

oct_Bool oct_List_createOwned(struct oct_Context* ctx, oct_OList* out_result) {
    if(!oct_ExchangeHeap_allocRaw(ctx, sizeof(oct_List), (void**)&out_result->ptr)) {
        return oct_False;
    }
    return oct_List_ctor(ctx, out_result->ptr);
}

oct_Bool oct_List_destroyOwned(struct oct_Context* ctx, oct_OList lst) {
	oct_Bool result = oct_List_dtor(ctx, lst.ptr);
	return oct_ExchangeHeap_freeRaw(ctx, lst.ptr) && result;
}

// Operations
oct_Bool oct_List_prepend(struct oct_Context* ctx, oct_OList lst, oct_OObject obj, oct_OList* out_lst) {
	if(!oct_List_createOwned(ctx, out_lst)) {
		return oct_False;
	}

	out_lst->ptr->next.variant = OCT_LISTOPTION_LIST;
	out_lst->ptr->next.list.ptr = lst.ptr;

	out_lst->ptr->data.variant = OCT_OBJECTOPTION_OBJECT;
	out_lst->ptr->data.object = obj;
	return oct_True;
}

oct_Bool oct_List_append(struct oct_Context* ctx, oct_BList lst, oct_OObject obj) {
	oct_Bool b;
	if(!oct_List_emptyp(ctx, lst, &b)) {
		return oct_False;
	}
	if(b) {
		lst.ptr->data.variant = OCT_OBJECTOPTION_OBJECT;
		lst.ptr->data.object = obj;
	}
	else {
		while(lst.ptr->next.variant == OCT_LISTOPTION_LIST) {
			lst.ptr = lst.ptr->next.list.ptr;
		}
		if(!oct_List_createOwned(ctx, &lst.ptr->next.list)) {
			return oct_False;
		}
		lst.ptr->next.variant = OCT_LISTOPTION_LIST;
		lst.ptr->next.list.ptr->data.variant = OCT_OBJECTOPTION_OBJECT;
		lst.ptr->next.list.ptr->data.object = obj;
	}
	return oct_True;
}

oct_Bool oct_List_emptyp(struct oct_Context* ctx, oct_BList lst, oct_Bool* out_result) {
	if(lst.ptr->data.variant == OCT_OBJECTOPTION_NOTHING
		&& lst.ptr->next.variant == OCT_LISTOPTION_NOTHING) {
		*out_result = oct_True;
	}
	else {
		*out_result = oct_False;
	}
	return oct_True;
}

oct_Bool oct_List_first(struct oct_Context* ctx, oct_OList lst, oct_ObjectOption* out_value, oct_OList* out_lst) {
	oct_Bool result = oct_True;
	if(lst.ptr->data.variant == OCT_OBJECTOPTION_NOTHING) {
		out_value->variant = OCT_OBJECTOPTION_NOTHING;
		*out_lst = lst;
	}
	else {
		out_value->variant = OCT_OBJECTOPTION_OBJECT;
		out_value->object = lst.ptr->data.object;
		lst.ptr->data.variant = OCT_OBJECTOPTION_NOTHING;

		if(lst.ptr->next.variant == OCT_LISTOPTION_NOTHING) {
			*out_lst = lst;
		}
		else {
			*out_lst = lst.ptr->next.list;
			lst.ptr->next.variant = OCT_LISTOPTION_NOTHING;
			result = oct_List_destroyOwned(ctx, lst);
		}
	}
	return result;
}

oct_Bool oct_List_rest(struct oct_Context* ctx, oct_BList lst, oct_OListOption* out_lst) {
	*out_lst = lst.ptr->next;
	if(lst.ptr->next.variant == OCT_LISTOPTION_LIST) {
		// Move operation
		lst.ptr->next.variant = OCT_LISTOPTION_NOTHING;
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

oct_Bool oct_List_nth(struct oct_Context* ctx, oct_OList lst, oct_Uword idx, oct_ObjectOption* out_value, oct_OList* out_lst) {
	oct_Uword current;
	oct_List* prev;
	oct_Bool result = oct_True;

	if(idx == 0) {
		return oct_List_first(ctx, lst, out_value, out_lst);
	}
	*out_lst = lst;

	for(current = 0; current < idx && lst.ptr->next.variant == OCT_LISTOPTION_LIST; ++current) {
		prev = lst.ptr;
		lst.ptr = lst.ptr->next.list.ptr;
	}
	if(current == idx && lst.ptr->data.variant == OCT_OBJECTOPTION_OBJECT) {
		out_value->variant = OCT_OBJECTOPTION_OBJECT;
		out_value->object = lst.ptr->data.object;
		lst.ptr->data.variant = OCT_OBJECTOPTION_NOTHING;
		prev->next = lst.ptr->next;
		lst.ptr->next.variant = OCT_LISTOPTION_NOTHING;
		result = oct_List_destroyOwned(ctx, lst);
	}
	else {
		// Out of bounds or the value was nothing
		out_value->variant = OCT_OBJECTOPTION_NOTHING;
	}
	return result;
}
