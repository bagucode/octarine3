#include "oct_list.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_context.h"
#include "oct_readable.h"
#include "oct_readable_pointers.h"

#include <stddef.h>
#include <stdlib.h>

// Template types
// ListOptionTemplate would be generated automatically as an inner type in octarine
// by using the Option template inside the List template
// but in C we have to create it manually

oct_Bool _oct_ListT_initType(oct_Context* ctx) {
	oct_Type* t = ctx->rt->builtInTypes.ListT;
	oct_Bool result;
	t->variant = OCT_TYPE_TEMPLATE;
	t->templateType.targetType = OCT_TEMPLATETARGET_STRUCT;
	result = oct_OATemplateParam_alloc(ctx, 2, &t->templateType.params);
	if(!result) {
		return result;
	}
	t->templateType.params.ptr->data[0].variant = OCT_TEMPLATEPARAM_TEMPLATE;
	t->templateType.params.ptr->data[0].type.ptr = ctx->rt->builtInTypes.Type;
	t->templateType.params.ptr->data[1].variant = OCT_TEMPLATEPARAM_NORMAL;
	t->templateType.params.ptr->data[1].type.ptr = ctx->rt->builtInTypes.ListOptionT;
	return oct_True;
}

oct_Bool _oct_ListOptionT_initType(struct oct_Context* ctx) {
	oct_Type* t = ctx->rt->builtInTypes.ListOptionT;
	oct_Bool result;
	t->variant = OCT_TYPE_TEMPLATE;
	t->templateType.targetType = OCT_TEMPLATETARGET_VARIADIC;
	result = oct_OATemplateParam_alloc(ctx, 1, &t->templateType.params);
	if(!result) {
		return result;
	}
	t->templateType.params.ptr->data[0].variant = OCT_TEMPLATEPARAM_TEMPLATE;
	t->templateType.params.ptr->data[0].type.ptr = ctx->rt->builtInTypes.Type;
	return oct_True;
}

// Concrete types used in reader

oct_Bool _oct_ReadableList_initType(struct oct_Context* ctx) {
	oct_Type* t = ctx->rt->builtInTypes.ReadableList;
	oct_Bool result;
	t->variant = OCT_TYPE_STRUCT;
	t->structType.size = sizeof(oct_ReadableList);
	t->structType.alignment = 0;
	result = oct_OAField_alloc(ctx, 2, &t->structType.fields);
	if(!result) {
		return result;
	}
	t->structType.fields.ptr->data[0].offset = offsetof(oct_ReadableList, readable);
	t->structType.fields.ptr->data[0].type.ptr = ctx->rt->builtInTypes.OReadableOption;
	t->structType.fields.ptr->data[1].offset = offsetof(oct_ReadableList, next);
	t->structType.fields.ptr->data[1].type.ptr = ctx->rt->builtInTypes.ReadableListOption;
	return oct_True;
}

oct_Bool _oct_OReadableList_initType(struct oct_Context* ctx) {
	oct_Type* t = ctx->rt->builtInTypes.OReadableList;
	t->variant = OCT_TYPE_POINTER;
	t->pointerType.kind = OCT_POINTER_OWNED;
	t->pointerType.type.ptr = ctx->rt->builtInTypes.ReadableList;
	return oct_True;
}

oct_Bool _oct_ReadableListOption_initType(struct oct_Context* ctx) {
	oct_Type* t = ctx->rt->builtInTypes.ReadableListOption;
	oct_Bool result;
	t->variant = OCT_TYPE_VARIADIC;
	t->variadicType.alignment = 0;
	t->variadicType.size = sizeof(oct_ReadableListOption);
	result = oct_OABType_alloc(ctx, 2, &t->variadicType.types);
	if(!result) {
		return result;
	}
	t->variadicType.types.ptr->data[0].ptr = ctx->rt->builtInTypes.Nothing;
	t->variadicType.types.ptr->data[1].ptr = ctx->rt->builtInTypes.ReadableList;
	return oct_True;
}

// Public

oct_Bool oct_ReadableList_ctor(struct oct_Context* ctx, oct_ReadableList* rll) {
	rll->next.variant = OCT_READABLE_LINKED_LIST_OPTION_NOTHING;
	rll->next.nothing.dummy = 0;
	rll->readable.variant = OCT_OREADABLEOPTION_NOTHING;
	rll->readable.nothing.dummy = 0;
	return oct_True;
}

oct_Bool oct_ReadableList_dtor(struct oct_Context* ctx, oct_ReadableList* rl) {
	oct_Bool result = oct_True;
	if(rl->readable.variant == OCT_OREADABLEOPTION_OREADABLE) {
		switch(rl->readable.readable.ptr->variant) {
		case OCT_READABLE_INT:
			break;
		case OCT_READABLE_FLOAT:
			break;
		case OCT_READABLE_STRING:
			result = result && oct_String_dtor(ctx, &rl->readable.readable.ptr->string);
			break;
		case OCT_READABLE_SYMBOL:
			result = result && oct_Symbol_dtor(ctx, &rl->readable.readable.ptr->symbol);
			break;
		case OCT_READABLE_LIST:
			result = result && oct_ReadableList_dtor(ctx, &rl->readable.readable.ptr->list);
			break;
		case OCT_READABLE_VECTOR:
			result = oct_False;
			break;
		case OCT_READABLE_MAP:
			result = oct_False;
			break;
		}
		free(rl->readable.readable.ptr);
	}
	if(rl->next.variant == OCT_READABLE_LINKED_LIST_OPTION_LIST) {
		result = result && oct_ReadableList_dtor(ctx, rl->next.rll.ptr);
		free(rl->next.rll.ptr);
	}
	return result;
}

oct_Bool oct_ReadableList_append(struct oct_Context* ctx, oct_BReadableList lst, oct_OReadable readable) {
	oct_Bool result;
	oct_OReadableList newNode;
	// go to end of list
	while(lst.ptr->next.variant == OCT_READABLE_LINKED_LIST_OPTION_LIST) {
		lst.ptr = lst.ptr->next.rll.ptr;
	}
	if(!oct_ReadableList_empty(ctx, lst, &result)) {
		return oct_False;
	}
	if(result) {
		// Empty, use this node
		lst.ptr->readable.variant = OCT_OREADABLEOPTION_OREADABLE;
		lst.ptr->readable.readable = readable;
		result = oct_True;
	}
	else {
		// allocate new node
		newNode.ptr = (oct_ReadableList*)malloc(sizeof(oct_ReadableList));
		if(!newNode.ptr) {
			// TODO: set OOM in ctx
			return oct_False;
		}
		if(!oct_ReadableList_ctor(ctx, newNode.ptr)) {
			free(newNode.ptr);
			return oct_False;
		}
		newNode.ptr->readable.variant = OCT_OREADABLEOPTION_OREADABLE;
		newNode.ptr->readable.readable = readable;
		lst.ptr->next.variant = OCT_READABLE_LINKED_LIST_OPTION_LIST;
		lst.ptr->next.rll = newNode;
		result = oct_True;
	}
	return result;
}

oct_Bool oct_ReadableList_empty(struct oct_Context* ctx, oct_BReadableList lst, oct_Bool* out_result) {
	if(lst.ptr->next.variant == OCT_READABLE_LINKED_LIST_OPTION_NOTHING
		&& lst.ptr->readable.variant == OCT_OREADABLEOPTION_NOTHING) {
		(*out_result) = oct_True;
	}
	else {
		(*out_result) = oct_False;
	}
	return oct_True;
}

oct_Bool _oct_BReadableList_initType(struct oct_Context* ctx) {
	ctx->rt->builtInTypes.BReadableList->variant = OCT_TYPE_POINTER;
	ctx->rt->builtInTypes.BReadableList->pointerType.kind = OCT_POINTER_BORROWED;
	ctx->rt->builtInTypes.BReadableList->pointerType.type.ptr = ctx->rt->builtInTypes.ReadableList;
	return oct_True;
}

oct_Bool oct_OReadableList_ctor(struct oct_Context* ctx, oct_OReadableList* out_result) {
	out_result->ptr = (oct_ReadableList*)malloc(sizeof(oct_ReadableList));
	if(!out_result->ptr) {
		// TODO: OOM in ctx
		return oct_False;
	}
	if(!oct_ReadableList_ctor(ctx, out_result->ptr)) {
		free(out_result->ptr);
		return oct_False;
	}
	return oct_True;
}

oct_Bool oct_OReadableList_dtor(struct oct_Context* ctx, oct_OReadableList orl) {
	if(!oct_ReadableList_dtor(ctx, orl.ptr)) {
		free(orl.ptr);
		return oct_False;
	}
	else {
		free(orl.ptr);
		return oct_True;
	}
}

