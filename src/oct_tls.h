#ifndef oct_tls
#define oct_tls

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

typedef struct oct_TLS {
	DWORD index;
} oct_TLS;

#else
#endif

void oct_TLSSet(oct_TLS tls, void* val);
void* oct_TLSGet(oct_TLS tls);

#endif

