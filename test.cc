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
	HOTPATCH(0x77E72EC0, &Beep_hook, &Beep_orgPtr);
	Beep(1000, 100); Beep(2000, 100);
}


