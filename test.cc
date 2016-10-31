#include <windows.h>
#include <stdio.h>
#include "hotpatch.h"

void* sleepAddr;
__stdcall
int sleepHook(int time)
{
	printf("sleep %d\n", time);
	return HOTCALL(sleepHook, sleepAddr)(time);
}


int main()
{
	HOTPATCH( GetProcAddress(GetModuleHandle(
		"kernel32.dll"), "Sleep"),
		&sleepHook, &sleepAddr);
	Sleep(1000);
}
