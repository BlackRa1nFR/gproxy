#ifndef MEMORYTOOLS_H
#define MEMORYTOOLS_H

typedef unsigned char *address;

extern void *getmodulebase(const char *name);
#define getmodulebaselib(name) address(getmodulebase(name ".dll"))

extern void *sigscan(const char *sig, void *_base);

extern void write_over_protected(void *addr, void *data, size_t length);

#endif