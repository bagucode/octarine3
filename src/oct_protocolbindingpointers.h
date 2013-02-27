#ifndef oct_protocolbindingpointers
#define oct_protocolbindingpointers

struct oct_ProtocolBinding;

// Only borrowed pointer with global scope. These are never deleted.
typedef struct oct_BProtocolBinding {
	struct oct_ProtocolBinding* ptr;
} oct_BProtocolBinding;

#endif
