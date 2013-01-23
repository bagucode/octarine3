#ifndef oct_list_private
#define oct_list_private

#include "oct_nothing.h"
#include "oct_readable_pointers.h"

// The basic linked list is a template type and is not defined in C
// Need a concrete type that can contain any readable type

struct oct_ReadableList;

typedef struct oct_OReadableList {
	struct oct_ReadableList* ptr;
} oct_OReadableList;

typedef struct oct_BReadableList {
	struct oct_ReadableList* ptr;
} oct_BReadableList;

#define OCT_READABLE_LINKED_LIST_OPTION_NOTHING 0
#define OCT_READABLE_LINKED_LIST_OPTION_LIST 1

typedef struct oct_ReadableListOption {
	oct_Uword variant; // OCT_READABLE_LINKED_LIST_OPTION_*
	union {
		oct_Nothing nothing;
		oct_OReadableList rll;
	};
} oct_ReadableListOption;

typedef struct oct_ReadableList {
	oct_OReadableOption readable;
	oct_ReadableListOption next;
} oct_ReadableList;

// Private

struct oct_Context;

// Linked list template
oct_Bool _oct_ListT_initType(struct oct_Context* ctx);
oct_Bool _oct_ListOptionT_initType(struct oct_Context* ctx);

// List<Readable>
oct_Bool _oct_ReadableList_initType(struct oct_Context* ctx);
oct_Bool _oct_OReadableList_initType(struct oct_Context* ctx);
oct_Bool _oct_BReadableList_initType(struct oct_Context* ctx);
oct_Bool _oct_ReadableListOption_initType(struct oct_Context* ctx);

// Public

oct_Bool oct_OReadableList_ctor(struct oct_Context* ctx, oct_OReadableList* out_result);
oct_Bool oct_OReadableList_dtor(struct oct_Context* ctx, oct_OReadableList orl);

oct_Bool oct_ReadableList_ctor(struct oct_Context* ctx, oct_ReadableList* rl);
oct_Bool oct_ReadableList_dtor(struct oct_Context* ctx, oct_ReadableList* rl);
oct_Bool oct_ReadableList_append(struct oct_Context* ctx, oct_BReadableList lst, oct_OReadable readable);
oct_Bool oct_ReadableList_empty(struct oct_Context* ctx, oct_BReadableList lst, oct_Bool* out_result);
oct_Bool oct_ReadableList_first(struct oct_Context* ctx, oct_BReadableList lst, oct_OReadableOption* out_result);
oct_Bool oct_ReadableList_rest(struct oct_Context* ctx, oct_BReadableList lst, oct_ReadableListOption* out_result);

#endif
