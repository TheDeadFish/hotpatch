#include <windows.h>
#include <stdio.h>
#include "hotpatch.h"

typeof(&Beep) Beep_orgPtr;
BOOL WINAPI Beep_hook(
	DWORD dwFreq, DWORD dwDuration)
{
	printf("Beep!\n");
	return Beep_orgPtr(dwFreq, dwDuration);

}
	

int main()
{
	HMODULE hK32 = GetModuleHandleA("kernel32.dll");
	FARPROC beep = GetProcAddress(hK32, "Beep");
	
	


	HOTPATCH(beep, &Beep_hook, &Beep_orgPtr);
	Beep(1000, 100); Beep(2000, 100);
}


