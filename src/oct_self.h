#ifndef oct_self
#define oct_self

typedef struct oct_Self {
	oct_Uword dummy;
} oct_Self;

typedef struct oct_BSelf {
	void* self;
} oct_BSelf;

typedef struct oct_MSelf {
	void* self;
} oct_MSelf;

typedef struct oct_OSelf {
	void* self;
} oct_OSelf;

#endif
