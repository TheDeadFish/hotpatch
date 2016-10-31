#include "hotpatch.cc"

DWORD WINAPI func(void* addr)
{
	printf("%s\n", addr);
	HOTCALL(func, 0x7C801A28);
}

DWORD WINAPI func2(void* addr)
{
	printf("!%s\n", addr);
}



int main()
{
	printf("%d\n", HOTPATCH(func, 0x7C801A28));
	jmpPatch((void*)func2, (void*)0x401E83);
	
	
	CreateFile("hello", 0, 0, 0, 0, 0, 0);
	CreateFile("goodbye", 0, 0, 0, 0, 0, 0);
	return 0;
}
