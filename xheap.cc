#include <windows.h>
#include <string.h>

__attribute__((section(".hotPatch,\"xw\"#")))
static BYTE  hotPatch_data[4096];
static int hotPatch_size;

extern "C"
void* xheap_alloc(size_t size)
{
	int rem = sizeof(hotPatch_data)-hotPatch_size;
	if(rem < size) return NULL;
	void* p = hotPatch_data+hotPatch_size;
	hotPatch_size += size; return p;
}
