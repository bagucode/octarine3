#include "oct_charstream.h"

// typedef struct oct_CharstreamFunctions {
// 	oct_Bool (*read)(struct oct_Context* ctx, oct_BSelf self, oct_Char* out_read);
// 	oct_Bool (*peek)(struct oct_Context* ctx, oct_BSelf self, oct_Char* out_peeked);
// } oct_CharstreamFunctions;

// typedef struct oct_CharstreamVTable {
// 	oct_BType type;
// 	oct_CharstreamFunctions functions;
// } oct_CharstreamVTable;

// typedef struct oct_BCharstream {
// 	oct_BSelf object;
// 	oct_CharstreamVTable* vtable;
// } oct_BCharstream;

// typedef struct oct_OCharstream {
// 	oct_OSelf object;
// 	oct_CharstreamVTable* vtable;
// } oct_OCharstream;

// oct_Bool _oct_Charstream_init(struct oct_Context* ctx);

	// Protocol
	// t = ctx->rt->builtInTypes.Protocol;
	// t.ptr->variant = OCT_TYPE_STRUCT;
	// t.ptr->structType.size = sizeof(oct_ProtocolType);
	// t.ptr->structType.alignment = 0;
	// CHECK(oct_AField_createOwned(ctx, 1, &t.ptr->structType.fields));
	// t.ptr->structType.fields.ptr->data[0].offset = offsetof(oct_ProtocolType, functions);
	// t.ptr->structType.fields.ptr->data[0].type = ctx->rt->builtInTypes.OABFunction;

oct_Bool _oct_Charstream_init(struct oct_Context* ctx) {
	Protocol
	t = ctx->rt->builtInTypes.Protocol;
	t.ptr->variant = OCT_TYPE_STRUCT;
	t.ptr->structType.size = sizeof(oct_ProtocolType);
	t.ptr->structType.alignment = 0;
	CHECK(oct_AField_createOwned(ctx, 1, &t.ptr->structType.fields));
	t.ptr->structType.fields.ptr->data[0].offset = offsetof(oct_ProtocolType, functions);
	t.ptr->structType.fields.ptr->data[0].type = ctx->rt->builtInTypes.OABFunction;
	
}

