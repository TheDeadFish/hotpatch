#include <windows.h>
#include "mempatch.h"

UnProtect::UnProtect(void* base, size_t length) 
{
	size_t funcBase = (size_t)base;
	pageBase = funcBase & ~4095;
	pageEnd = (funcBase+4095+length) & ~4095;
	VirtualProtect((PVOID)pageBase, pageEnd-pageBase,
		PAGE_EXECUTE_READWRITE, &flOldProtect);	
}

UnProtect::~UnProtect(void) {
		VirtualProtect((PVOID)pageBase, pageEnd-pageBase,
			flOldProtect, &flOldProtect);	
}
