#include "vthook.h"
#include <Windows.h>

VTHook::VTHook(void *object)
{
	hookedclass = object;
	oldvtable = *(vtable *)object;
	vtable curindex = oldvtable;
	size = 0;
	while (!IsBadCodePtr((FARPROC)curindex++)) size++;
	newvtable = new ptr[size];
	for (vtindex i = 0; i < size; i++)
		newvtable[i] = oldvtable[i];
}

VTHook::~VTHook(void)
{
	delete[] newvtable;
	UnHook();
}