#ifndef oct_list
#define oct_list

#include "oct_any.h"

// The basic linked list is a template type and is not defined in C
// Need a concrete type that can contain any readable type

struct oct_List;

typedef struct oct_OList {
	struct oct_List* ptr;
} oct_OList;

typedef struct oct_MList {
	struct oct_List* ptr;
} oct_MList;

typedef struct oct_BList {
	struct oct_List* ptr;
} oct_BList;

#define OCT_LISTOPTION_NOTHING 0
#define OCT_LISTOPTION_LIST 1

typedef struct oct_OListOption {
	oct_Uword variant;
	union {
		oct_Nothing nothing;
		oct_OList list;
	};
} oct_OListOption;

typedef struct oct_BListOption {
	oct_Uword variant;
	union {
		oct_Nothing nothing;
		oct_BList list;
	};
} oct_BListOption;

typedef struct oct_List {
	oct_AnyOption data;
	oct_OListOption next;
} oct_List;

typedef struct oct_ListBorrowed {
	oct_AnyOption data;
	oct_BListOption next;
} oct_ListBorrowed;

// Private

struct oct_Context;
struct oct_BType;

oct_Bool _oct_List_initType(struct oct_Context* ctx);
oct_Bool _oct_OList_initType(struct oct_Context* ctx);
oct_Bool _oct_MList_initType(struct oct_Context* ctx);
oct_Bool _oct_BList_initType(struct oct_Context* ctx);
oct_Bool _oct_OListOption_initType(struct oct_Context* ctx);

// Public

// Create & Destroy
oct_Bool oct_List_ctor(struct oct_Context* ctx, oct_List* self);
oct_Bool oct_List_dtor(struct oct_Context* ctx, oct_List* self);
oct_Bool oct_List_createOwned(struct oct_Context* ctx, oct_OList* out_result);
oct_Bool oct_List_createManaged(struct oct_Context* ctx, oct_MList* out_result);
oct_Bool oct_List_borrowOwned(struct oct_Context* ctx, oct_OList lst, oct_BList* out_lst);
oct_Bool oct_List_borrowManaged(struct oct_Context* ctx, oct_MList lst, oct_BList* out_lst);
oct_Bool oct_List_destroyOwned(struct oct_Context* ctx, oct_OList lst);

// Util
oct_Bool oct_List_getType(struct oct_Context* ctx, struct oct_BType* out_type);

// Operations
oct_Bool oct_List_append(struct oct_Context* ctx, oct_BList lst, oct_Any obj);
oct_Bool oct_List_emptyp(struct oct_Context* ctx, oct_BList lst, oct_Bool* out_result);
oct_Bool oct_List_first(struct oct_Context* ctx, oct_BList lst, oct_AnyOption* out_any);
// Not sure about the rest function... A borrowed list that contains owned pointers, is that ok?
// Perhaps the return value should be deeply borrowed?
oct_Bool oct_List_rest(struct oct_Context* ctx, oct_BList lst, oct_BListOption* out_lst);
oct_Bool oct_List_count(struct oct_Context* ctx, oct_BList lst, oct_Uword* out_count);
oct_Bool oct_List_nth(struct oct_Context* ctx, oct_BList lst, oct_Uword idx, oct_OReadableOption* out_result);

#endif
