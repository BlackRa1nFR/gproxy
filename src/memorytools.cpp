#include <Windows.h>

void *getmodulebase(const char *name)
{
	return GetModuleHandleA(name);
}

void *sigscan(const char *sig, void *_base)
{
	char *base = (char *)_base;
	while (1)
	{
		const char *cursig = sig;
		char *curbase = base;
		while (1)
		{
			if (*cursig == 0)
				return base;
			if (*cursig != '?' && *cursig != *curbase)
				break;
			cursig++;
			curbase++;
		}

		base++;
	}
	return 0;
}

void write_over_protected(void *addr, void *data, size_t length) {
	DWORD oldprotect, garbage;
	VirtualProtect(addr, length, PAGE_READWRITE, &oldprotect);
	memcpy(addr, data, length);
	VirtualProtect(addr, length, oldprotect, &garbage);
}