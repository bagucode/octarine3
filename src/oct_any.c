#include "oct_any.h"
#include "oct_any_type.h"
#include "oct_context.h"
#include "oct_runtime.h"
#include "oct_type.h"
#include "oct_type_pointers.h"
#include "oct_protocoltype.h"
#include "oct_object_type.h"

#define CHECK(X) if(!X) return oct_False;

oct_Bool oct_Any_dtor(struct oct_Context* ctx, oct_BAny self) {
	if(self.ptr->variant == OCT_ANY_OOBJECT) {
		return oct_Object_destroyOwned(ctx, self.ptr->oobject);
	}
	return oct_True;
}

void* _Object_fns[] = {oct_Any_dtor};
oct_VTable _Object_vtable = {
	{(oct_Type*)&_oct_AnyType},
	_Object_fns
};
oct_CVTable _Object_cvtable = {&_Object_vtable};

oct_Bool _oct_Any_init(struct oct_Context* ctx) {
	oct_BProtocolBinding pb;
	oct_CType ct;
	oct_Bool result = _oct_Runtime_bindBuiltInType(ctx, "Any", &_oct_AnyType);
	pb.ptr = &_oct_ObjectProtocol;
	ct.ptr = (oct_Type*)&_oct_AnyType;
	return result && oct_Protocol_addImplementation(ctx, pb, ct, _Object_cvtable);
}
