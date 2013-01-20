#include "oct_tls.h"

#ifdef _WIN32

void oct_TLSSet(oct_TLS tls, void* val) {
	TlsSetValue(tls.index, val);
}

void* oct_TLSGet(oct_TLS tls) {
	return TlsGetValue(tls.index);
}

#else
#endif

