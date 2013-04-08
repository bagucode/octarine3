#ifndef oct_list
#define oct_list

#include "oct_object.h"
#include "oct_nothing.h"
#include "oct_printable.h"

struct oct_ListNode;

typedef struct oct_OListNode {
	struct oct_ListNode* ptr;
} oct_OListNode;

#define OCT_LISTNODEOPTION_NOTHING 0
#define OCT_LISTNODEOPTION_NODE 1

typedef struct oct_OListNodeOption {
	oct_Uword variant;
	union {
		oct_Nothing nothing;
		oct_OListNode node;
	};
} oct_OListNodeOption;

typedef struct oct_ListNode {
	oct_OObject data;
	oct_OListNodeOption next;
} oct_ListNode;

typedef struct oct_List {
	oct_OListNodeOption head;
} oct_List;

typedef struct oct_BList {
	oct_List* ptr;
} oct_BList;

typedef struct oct_OList {
	oct_List* ptr;
} oct_OList;

// Private

struct oct_Context;
struct oct_CType;

oct_Bool _oct_List_init(struct oct_Context* ctx);

// Public

// Create & Destroy
oct_Bool oct_List_ctor(struct oct_Context* ctx, oct_List* self);
oct_Bool oct_List_dtor(struct oct_Context* ctx, oct_BGeneric self);
oct_Bool oct_List_createOwned(struct oct_Context* ctx, oct_OList* out_result);
//oct_Bool oct_List_createManaged(struct oct_Context* ctx, oct_MList* out_result);
oct_Bool oct_List_destroyOwned(struct oct_Context* ctx, oct_OList lst);

// Mutating Operations
oct_Bool oct_List_prepend(struct oct_Context* ctx, oct_BList lst, oct_OObject obj);
oct_Bool oct_List_append(struct oct_Context* ctx, oct_BList lst, oct_OObject obj);
oct_Bool oct_List_first(struct oct_Context* ctx, oct_BList lst, oct_OObjectOption* out_value);
oct_Bool oct_List_rest(struct oct_Context* ctx, oct_BList lst, oct_OList* out_lst);
oct_Bool oct_List_nth(struct oct_Context* ctx, oct_BList lst, oct_Uword idx, oct_OObjectOption* out_value);

// Non-mutating Operations
oct_Bool oct_List_borrowFirst(struct oct_Context* ctx, oct_BList lst, oct_BObjectOption* out_value);
oct_Bool oct_List_emptyp(struct oct_Context* ctx, oct_BList lst, oct_Bool* out_result);
oct_Bool oct_List_count(struct oct_Context* ctx, oct_BList lst, oct_Uword* out_count);
oct_Bool oct_List_print(struct oct_Context* ctx, oct_BList lst);

// Protocol casts
oct_Bool oct_List_asObject(struct oct_Context* ctx, oct_OList lst, oct_OObject* out_object);
oct_Bool oct_List_asPrintable(struct oct_Context* ctx, oct_BList lst, oct_BPrintable* out_prn);

#endif
