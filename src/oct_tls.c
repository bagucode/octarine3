#include "oct_tls.h"

#ifdef _WIN32

void oct_TLSInit(oct_TLS tls) {
    tls.index = TlsAlloc();
}

void oct_TLSDestroy(oct_TLS tls) {
    TlsFree(tls.index);
}

void oct_TLSSet(oct_TLS tls, void* val) {
	TlsSetValue(tls.index, val);
}

void* oct_TLSGet(oct_TLS tls) {
	return TlsGetValue(tls.index);
}

#elif defined (__APPLE__)

void oct_TLSInit(oct_TLS tls) {
    pthread_key_create(&tls.key, NULL);
}

void oct_TLSDestroy(oct_TLS tls) {
    pthread_key_delete(tls.key);
}

void oct_TLSSet(oct_TLS tls, void* val) {
	pthread_setspecific(tls.key, val);
}

void* oct_TLSGet(oct_TLS tls) {
	return pthread_getspecific(tls.key);
}

#else

#endif

