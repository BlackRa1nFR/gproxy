#include "vthook.h"
#include <Windows.h>
#include "color.h"

typedef int(__cdecl *ColorSpewMessage_t)(int type, const Color &color, const char *format, ...);
extern ColorSpewMessage_t ColorSpewMessage;

VTHook::VTHook(void *object)
{
	hookedclass = object;
	oldvtable = *(vtable *)object;
	vtable curindex = oldvtable;
	size = 0;
	while (!IsBadCodePtr((FARPROC)*curindex++)) size++;
	ColorSpewMessage(0, Color(), "VTable at %p is %u indices big\n", object, size);
	newvtable = new ptr[size];
	for (vtindex i = 0; i < size; i++)
		newvtable[i] = oldvtable[i];
}

VTHook::~VTHook(void)
{
	delete[] newvtable;
	UnHook();
}