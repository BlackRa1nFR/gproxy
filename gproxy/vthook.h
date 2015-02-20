#ifndef VTHOOK_H
#define VTHOOK_H

typedef void *ptr;
typedef ptr *vtable;
typedef unsigned short vtindex;

class VTHook
{
public:
	VTHook::VTHook(void *object);
	~VTHook();
	template<typename t>
	t GetIndex(vtindex number)
	{
		return (t)((*(vtable *)hookedclass)[number]);
	}
	template<typename t>
	t GetOldIndex(vtindex number)
	{
		return (t)(oldvtable[number]);
	}
	template<typename t>
	t GetNewIndex(vtindex number)
	{
		return (t)(newvtable[number]);
	}

	bool Hooked(void)
	{
		return *(vtable *)hookedclass == newvtable;
	}

	void Hook(void)
	{
		*(vtable *)hookedclass = newvtable;
	}
	void UnHook(void)
	{
		*(vtable *)hookedclass = oldvtable;
	}
	void SetFunc(vtindex num, void *func)
	{
		newvtable[num] = func;
	}
public:
	ptr hookedclass;
	vtable oldvtable;
	vtable newvtable;
	vtindex size;
};

#endif