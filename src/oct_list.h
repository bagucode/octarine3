#ifndef oct_list
#define oct_list

#include "oct_object.h"
#include "oct_nothing.h"

struct oct_List;

typedef struct oct_OList {
	struct oct_List* ptr;
} oct_OList;

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
	oct_OObjectOption data;
	oct_OListOption next;
} oct_List;

// Private

struct oct_Context;
struct oct_BType;

oct_Bool _oct_List_init(struct oct_Context* ctx);

// Public

// Create & Destroy
oct_Bool oct_List_ctor(struct oct_Context* ctx, oct_List* self);
oct_Bool oct_List_dtor(struct oct_Context* ctx, oct_List* self);
oct_Bool oct_List_createOwned(struct oct_Context* ctx, oct_OList* out_result);
//oct_Bool oct_List_createManaged(struct oct_Context* ctx, oct_MList* out_result);
oct_Bool oct_List_destroyOwned(struct oct_Context* ctx, oct_OList lst);

// Mutating Operations
oct_Bool oct_List_prepend(struct oct_Context* ctx, oct_OList lst, oct_OObject obj, oct_OList* out_lst);
oct_Bool oct_List_append(struct oct_Context* ctx, oct_BList lst, oct_OObject obj);
oct_Bool oct_List_first(struct oct_Context* ctx, oct_OList lst, oct_OObject* out_value, oct_OList* out_lst);
oct_Bool oct_List_rest(struct oct_Context* ctx, oct_BList lst, oct_OListOption* out_lst);
oct_Bool oct_List_nth(struct oct_Context* ctx, oct_OList lst, oct_Uword idx, oct_OObject* out_value, oct_OList* out_lst);

// Non-mutating Operations
oct_Bool oct_List_borrowFirst(struct oct_Context* ctx, oct_BList lst, oct_BObject* out_value);
oct_Bool oct_List_emptyp(struct oct_Context* ctx, oct_BList lst, oct_Bool* out_result);
oct_Bool oct_List_count(struct oct_Context* ctx, oct_BList lst, oct_Uword* out_count);

// Protocol casts
oct_Bool oct_List_asObject(struct oct_Context* ctx, oct_OList lst, oct_OObject* out_object);

#endif
