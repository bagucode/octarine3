#ifndef oct_self_type
#define oct_self_type

#include "oct_self.h"
#include "oct_primitive_types.h"
#include "oct_type_type.h"

// Self is treated specially in the octarine runtime as a placeholder 
// type in recursive definitions and protocol function signatures

OCT_DEF_PRIMITIVE(Self);
OCT_DEF_C_POINTER(CSelf, Self);
OCT_DEF_B_POINTER(BSelf, Self);
OCT_DEF_O_POINTER(OSelf, Self);
OCT_DEF_M_POINTER(MSelf, Self);

#endif
