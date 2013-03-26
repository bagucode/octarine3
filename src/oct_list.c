#include "oct_list.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_context.h"
#include "oct_exchangeheap.h"
#include "oct_object.h"
#include "oct_object_vtable.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#define CHECK(X) if(!X) return oct_False;

oct_Bool _oct_List_init(struct oct_Context* ctx) {
	oct_BType t;

	// ListNode
	t = ctx->rt->builtInTypes.ListNode;
	t.ptr->variant = OCT_TYPE_STRUCT;
	t.ptr->structType.alignment = 0;
	t.ptr->structType.size = sizeof(oct_ListNode);
	CHECK(oct_AField_createOwned(ctx, 2, &t.ptr->structType.fields));
	t.ptr->structType.fields.ptr->data[0].offset = offsetof(oct_ListNode, data);
	t.ptr->structType.fields.ptr->data[0].type = ctx->rt->builtInTypes.OObject;
	t.ptr->structType.fields.ptr->data[1].offset = offsetof(oct_ListNode, next);
	t.ptr->structType.fields.ptr->data[1].type = ctx->rt->builtInTypes.OListNodeOption;

	// OListNode
	t = ctx->rt->builtInTypes.OListNode;
	t.ptr->variant = OCT_TYPE_POINTER;
	t.ptr->pointerType.kind = OCT_POINTER_OWNED;
	t.ptr->pointerType.type = ctx->rt->builtInTypes.ListNode;

	// OListNodeOption
	t = ctx->rt->builtInTypes.OListNodeOption;
	t.ptr->variant = OCT_TYPE_VARIADIC;
	t.ptr->variadicType.alignment = 0;
	t.ptr->variadicType.size = sizeof(oct_OListNodeOption);
	CHECK(oct_ABType_createOwned(ctx, 2, &t.ptr->variadicType.types));
	t.ptr->variadicType.types.ptr->data[0] = ctx->rt->builtInTypes.Nothing;
	t.ptr->variadicType.types.ptr->data[1] = ctx->rt->builtInTypes.OListNode;
	
	// List
	t = ctx->rt->builtInTypes.List;
	t.ptr->variant = OCT_TYPE_STRUCT;
	t.ptr->structType.alignment = 0;
	t.ptr->structType.size = sizeof(oct_List);
	CHECK(oct_AField_createOwned(ctx, 1, &t.ptr->structType.fields));
	t.ptr->structType.fields.ptr->data[0].offset = offsetof(oct_List, head);
	t.ptr->structType.fields.ptr->data[0].type = ctx->rt->builtInTypes.OListNodeOption;

	// Object protocol {dtor}
	CHECK(_oct_Protocol_addBuiltIn(ctx, ctx->rt->builtInProtocols.Object, 1, &ctx->rt->vtables.ListAsObject, t, oct_List_dtor));

	// Printable protocol {print}
	CHECK(_oct_Protocol_addBuiltIn(ctx, ctx->rt->builtInProtocols.Printable, 1, &ctx->rt->vtables.ListAsPrintable, t, oct_List_print));

	// Seq protocol {first,rest,prepend,append,nth}
	CHECK(_oct_Protocol_addBuiltIn(ctx, ctx->rt->builtInProtocols.Seq, 5, &ctx->rt->vtables.ListAsSeq, t, oct_List_first, oct_List_rest, oct_List_prepend, oct_List_append, oct_List_nth));

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

	return oct_True;
}

// Create & Destroy
oct_Bool oct_List_ctor(struct oct_Context* ctx, oct_List* self) {
	self->head.variant = OCT_LISTNODEOPTION_NOTHING;
    return oct_True;
}

oct_Bool oct_List_dtor(struct oct_Context* ctx, oct_BSelf self) {
    oct_Bool result = oct_True;
	oct_List* lst = (oct_List*)self.self;
	oct_ListNode* node;
	oct_ListNode* next;
	
	if(lst->head.variant == OCT_LISTNODEOPTION_NODE) {
		node = lst->head.node.ptr;
		while(oct_True) {
			result = oct_Object_destroyOwned(ctx, node->data) && result;
			if(node->next.variant == OCT_LISTNODEOPTION_NODE) {
				next = node->next.node.ptr;
				OCT_FREEOWNED(node);
				node = next;
			}
			else {
				OCT_FREEOWNED(node);
				break;
			}
		}
		lst->head.variant = OCT_LISTNODEOPTION_NOTHING;
	}

    return result;
}

oct_Bool oct_List_createOwned(struct oct_Context* ctx, oct_OList* out_result) {
    if(!OCT_ALLOCOWNED(sizeof(oct_List), (void**)&out_result->ptr, "oct_List_createOwned")) {
        return oct_False;
    }
    return oct_List_ctor(ctx, out_result->ptr);
}

oct_Bool oct_List_destroyOwned(struct oct_Context* ctx, oct_OList lst) {
	oct_BSelf self;
	oct_Bool result;
	self.self = lst.ptr;
	result = oct_List_dtor(ctx, self);
	return OCT_FREEOWNED(lst.ptr) && result;
}

// Operations
oct_Bool oct_List_prepend(struct oct_Context* ctx, oct_BList lst, oct_OObject obj) {
	oct_OListNode newNode;
	CHECK(OCT_ALLOCOWNED(sizeof(oct_ListNode), (void**)&newNode.ptr, "oct_List_prepend"));
	newNode.ptr->data = obj;
	newNode.ptr->next.variant = lst.ptr->head.variant;
	if(lst.ptr->head.variant == OCT_LISTNODEOPTION_NODE) {
		newNode.ptr->next.node = lst.ptr->head.node;
	}
	lst.ptr->head.variant = OCT_LISTNODEOPTION_NODE;
	lst.ptr->head.node = newNode;
	return oct_True;
}

oct_Bool oct_List_append(struct oct_Context* ctx, oct_BList lst, oct_OObject obj) {
	oct_Bool b;
	oct_ListNode* node;
	oct_OListNode newNode;
	CHECK(oct_List_emptyp(ctx, lst, &b));
	if(b) {
		return oct_List_prepend(ctx, lst, obj);
	}
	else {
		node = lst.ptr->head.node.ptr;
		while(node->next.variant == OCT_LISTNODEOPTION_NODE) {
			node = node->next.node.ptr;
		}
		CHECK(OCT_ALLOCOWNED(sizeof(oct_ListNode), (void**)&newNode.ptr, "oct_List_append"));
		newNode.ptr->data = obj;
		newNode.ptr->next.variant = OCT_LISTNODEOPTION_NOTHING;
		node->next.variant = OCT_LISTNODEOPTION_NODE;
		node->next.node = newNode;
	}
	return oct_True;
}

oct_Bool oct_List_emptyp(struct oct_Context* ctx, oct_BList lst, oct_Bool* out_result) {
	if(lst.ptr->head.variant == OCT_LISTNODEOPTION_NOTHING) {
		*out_result = oct_True;
	}
	else {
		*out_result = oct_False;
	}
	return oct_True;
}

oct_Bool oct_List_first(struct oct_Context* ctx, oct_BList lst, oct_OObjectOption* out_value) {
	oct_ListNode* tmp;
	oct_Bool result = oct_True;
	if(lst.ptr->head.variant == OCT_LISTNODEOPTION_NOTHING) {
		out_value->variant = OCT_OOBJECTOPTION_NOTHING;
	}
	else {
		out_value->variant = OCT_OOBJECTOPTION_OBJECT;
		out_value->object = lst.ptr->head.node.ptr->data;
		if(lst.ptr->head.node.ptr->next.variant == OCT_LISTNODEOPTION_NODE) {
			tmp = lst.ptr->head.node.ptr->next.node.ptr;
			result = OCT_FREEOWNED(lst.ptr->head.node.ptr) && result;
			lst.ptr->head.node.ptr = tmp;
		}
		else {
			result = OCT_FREEOWNED(lst.ptr->head.node.ptr) && result;
			lst.ptr->head.variant = OCT_LISTNODEOPTION_NOTHING;
		}
	}
	return result;
}

oct_Bool oct_List_rest(struct oct_Context* ctx, oct_BList lst, oct_OList* out_lst) {
	CHECK(oct_List_createOwned(ctx, out_lst));
	if(lst.ptr->head.variant == OCT_LISTNODEOPTION_NODE) {
		if(lst.ptr->head.node.ptr->next.variant == OCT_LISTNODEOPTION_NODE) {
			// Move the next-from-head node over to the new list
			out_lst->ptr->head.variant = OCT_LISTNODEOPTION_NODE;
			out_lst->ptr->head.node = lst.ptr->head.node.ptr->next.node;
			lst.ptr->head.node.ptr->next.variant = OCT_LISTNODEOPTION_NOTHING;
		}
	}
	return oct_True;
}

oct_Bool oct_List_count(struct oct_Context* ctx, oct_BList lst, oct_Uword* out_count) {
	oct_Bool emptyp;
	oct_ListNode* node;
	if(!oct_List_emptyp(ctx, lst, &emptyp)) {
		return oct_False;
	}
	if(emptyp) {
		*out_count = 0;
	}
	else {
		*out_count = 1;
		node = lst.ptr->head.node.ptr;
		while(node->next.variant == OCT_LISTNODEOPTION_NODE) {
			++(*out_count);
			node = node->next.node.ptr;
		}
	}
	return oct_True;
}

oct_Bool oct_List_nth(struct oct_Context* ctx, oct_BList lst, oct_Uword idx, oct_OObjectOption* out_value) {
	oct_Uword current;
	oct_ListNode* node;
	oct_ListNode* prev;
	oct_Bool result = oct_True;

	if(idx == 0) {
		return oct_List_first(ctx, lst, out_value);
	}
	node = lst.ptr->head.node.ptr;

	for(current = 0; current < idx && node->next.variant == OCT_LISTNODEOPTION_NODE; ++current) {
		prev = node;
		node = node->next.node.ptr;
	}
	if(current == idx) {
		out_value->variant = OCT_OOBJECTOPTION_OBJECT;
		out_value->object = node->data;
		prev->next = node->next;
		result = OCT_FREEOWNED(node) && result;
	}
	else {
		// Out of bounds
		out_value->variant = OCT_OOBJECTOPTION_NOTHING;
	}
	return result;
}

oct_Bool oct_List_borrowFirst(struct oct_Context* ctx, oct_BList lst, oct_BObjectOption* out_value) {
	oct_Bool eq;
	CHECK(oct_List_emptyp(ctx, lst, &eq));
	if(eq) {
		out_value->variant = OCT_BOBJECTOPTION_NOTHING;
	}
	else {
		out_value->variant = OCT_BOBJECTOPTION_OBJECT;
		out_value->object.self.self = lst.ptr->head.node.ptr->data.self.self;
		out_value->object.vtable = lst.ptr->head.node.ptr->data.vtable;
	}
	return oct_True;
}

oct_Bool oct_List_asObject(struct oct_Context* ctx, oct_OList lst, oct_OObject* out_object) {
	out_object->self.self = lst.ptr;
	out_object->vtable = (oct_ObjectVTable*)ctx->rt->vtables.ListAsObject.ptr;
	return oct_True;
}

oct_Bool oct_List_print(struct oct_Context* ctx, oct_BList lst) {
	oct_BSelf bself;
	oct_BPrintable prn;
	oct_ListNode* node;

	putc('(', stdout);

	if(lst.ptr->head.variant == OCT_LISTNODEOPTION_NODE) {
		node = lst.ptr->head.node.ptr;
		while(oct_True) {
			// TODO: global print function for unprintables?
			bself.self = node->data.self.self;
			if(!oct_Object_as(ctx, bself, node->data.vtable->type, ctx->rt->builtInProtocols.Printable, (oct_BObject*)&prn)) {
				printf("#<UNPRINTABLE %p>", node->data.self.self);
				oct_Context_clearError(ctx);
			}
			else {
				CHECK(oct_Printable_print(ctx, prn));
			}
			if(node->next.variant == OCT_LISTNODEOPTION_NODE) {
				node = node->next.node.ptr;
				putc(' ', stdout);
			}
			else {
				break;
			}
		}
	}

	putc(')', stdout);

	return oct_True;
}

oct_Bool oct_List_asPrintable(struct oct_Context* ctx, oct_BList lst, oct_BPrintable* out_prn) {
	out_prn->self.self = lst.ptr;
	out_prn->vtable = (oct_PrintableVTable*)ctx->rt->vtables.ListAsPrintable.ptr;
	return oct_True;
}

