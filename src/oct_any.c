#include "oct_any.h"
#include "oct_any_type.h"
#include "oct_runtime.h"
#include "oct_object_type.h"

oct_Bool oct_Any_dtor(struct oct_Context* ctx, oct_BAny self) {
	if(self.ptr->variant == OCT_ANY_OOBJECT) {
		return oct_Object_destroyOwned(ctx, self.ptr->oobject);
	}
	return oct_True;
}

oct_ObjectVTable _Object_vtable = {
	{(oct_Type*)&_oct_AnyType},
	(oct_Object_dtor)oct_Any_dtor
};

oct_Bool _oct_Any_init(struct oct_Context* ctx) {
	return _oct_Runtime_bindBuiltInType(ctx, "Any", &_Object_vtable);
}
