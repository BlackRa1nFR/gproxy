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