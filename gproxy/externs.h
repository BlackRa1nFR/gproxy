#ifndef EXTERNS_H
#define EXTERNS_H

#ifdef __linux
#define EXTENSION_DEFAULT ".so"
#elif defined(_MSC_VER)
#define EXTENSION_DEFAULT ".dll"
#else
#define EXTENSION_DEFAULT ".dylib"
#endif

extern void *GetExternFromLib(const char *modulename, const char *externname);

#define GetExternFromLib(mn, en) (GetExternFromLib(mn EXTENSION_DEFAULT, en))

#endif