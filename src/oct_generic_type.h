#ifndef oct_generic_type
#define oct_generic_type

#include "oct_generic.h"
#include "oct_primitive_types.h"
#include "oct_type_type.h"

// Generic is treated specially in the octarine runtime as a placeholder 
// type in recursive definitions and protocol function signatures

OCT_DEF_PRIMITIVE(Generic);
OCT_DEF_C_POINTER(CGeneric, Generic);
OCT_DEF_B_POINTER(BGeneric, Generic);
OCT_DEF_O_POINTER(OGeneric, Generic);
OCT_DEF_M_POINTER(MGeneric, Generic);

#endif
