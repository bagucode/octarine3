#ifndef oct_templatetype_private
#define oct_templatetype_private

#include "oct_primitives.h"
#include "oct_type_pointers.h"

#define OCT_TEMPLATEPARAM_NORMAL 0
#define OCT_TEMPLATEPARAM_TEMPLATE 1

typedef struct oct_TemplateParam {
	oct_Uword variant; // OCT_TEMPLATEPARAM_*
	oct_BType type;
} oct_TemplateParam;

typedef struct oct_ATemplateParam {
	oct_Uword size;
	oct_TemplateParam data[];
} oct_ATemplateParam;

typedef struct oct_OATemplateParam {
	oct_ATemplateParam* ptr;
} oct_OATemplateParam;

#define OCT_TEMPLATETARGET_STRUCT 0
#define OCT_TEMPLATETARGET_VARIADIC 1

typedef struct oct_TemplateType {
	oct_Uword targetType; // OCT_TEMPLATETARGET_*
	oct_OATemplateParam params;
} oct_TemplateType;

// Private

struct oct_Context;

oct_Bool _oct_TemplateType_initType(struct oct_Context* ctx);
oct_Bool _oct_TemplateParam_initType(struct oct_Context* ctx);
oct_Bool _oct_ATemplateParam_initType(struct oct_Context* ctx);

// Public

struct oct_Context;

oct_Bool oct_TemplateParam_ctor(struct oct_Context* ctx, oct_TemplateParam* tparam);
oct_Bool oct_TemplateParam_dtor(struct oct_Context* ctx, oct_TemplateParam* tparam);

oct_Bool oct_OATemplateParam_alloc(struct oct_Context* ctx, oct_Uword size, oct_OATemplateParam* out_result);
oct_Bool oct_OATemplateParam_free(struct oct_Context* ctx, oct_OATemplateParam oatparam);

#endif
