#include <stdio.h>
#include <string.h>
#include "hotpatch.h"
#include <windows.h>

class UnProtect
{
public:
	UnProtect(void* base, int length)
	{
		size_t funcBase = (size_t)base;
		pageBase = funcBase & ~4095;
		pageEnd = (funcBase+4095+length) & ~4095;
		VirtualProtect((PVOID)pageBase, pageEnd-pageBase,
			PAGE_EXECUTE_READWRITE, &flOldProtect);	
	}
	~UnProtect()
	{
		VirtualProtect((PVOID)pageBase, pageEnd-pageBase,
			flOldProtect, &flOldProtect);	
	}

private:
	size_t pageBase;
	size_t pageEnd;
	DWORD flOldProtect;
};

bool hotPatch(void* lpNewProc, void* lpOldProc)
{
	// check signature
	BYTE* funcBase = (BYTE*)(lpOldProc)-5;
	if(memcmp(funcBase, "\x90\x90\x90\x90\x90\x8B\xFF", 7))
		return false;
	if(!lpNewProc)
		return true;

	// modify code
	UnProtect unProtect(funcBase, 7);
	*(BYTE*)(funcBase+0) = 0xE9;
	*(size_t*)(funcBase+1) = (BYTE*)(lpNewProc)-funcBase-5;
	*(BYTE*)(funcBase+5) = 0xEB;
	*(BYTE*)(funcBase+6) = 0xF9;
	return true;
}

void* callPatch(void* lpNewProc, void* lpCall)
{
	// check signature
	BYTE* callBase = (BYTE*)lpCall;
	void* lpOldCall = *(void**)(callBase+1);
	if(callBase[0] != 0xE8)
		return NULL;
	if(!lpNewProc)
		return lpOldCall;
		
	// modify code
	UnProtect unProtect(callBase+1, 4);
	*(size_t*)(callBase+1) = (BYTE*)(lpNewProc)-callBase-5;
	return lpOldCall;
}

void jmpPatch(void* lpNewProc, void* lpJmp)
{
	// modify code (no check)
	BYTE* jmpBase = (BYTE*)lpJmp;
	UnProtect unProtect(jmpBase, 5);
	*(BYTE*)(jmpBase+0) = 0xE9;
	*(size_t*)(jmpBase+1) = (BYTE*)(lpNewProc)-jmpBase-5;
}	
