#ifndef oct_vector
#define oct_vector

#include "oct_object.h"
#include "oct_nothing.h"
#include "oct_printable.h"

typedef struct oct_Vector {
	oct_Uword size;
	oct_OAOObjectOption data;
} oct_Vector;



#endif
