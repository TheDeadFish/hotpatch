#include <windows.h>
#include <string.h>

#define VARFIX(x) asm("" : "+r"(x))

#ifdef  _WIN64

#define MEM_RANGE 0x7E000000
#define HEAP_SIZE 4096
#define HEAP_NMAX 16

static
void* xheap_valloc(void* addr)
{
	while(1) {
		MEMORY_BASIC_INFORMATION mbi;
		if(!VirtualQuery(addr, &mbi, sizeof(mbi)))
			__builtin_trap();
		addr = VirtualAlloc(mbi.BaseAddress, HEAP_SIZE, 
			MEM_RESERVE|MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if(addr) return addr;
		size_t tmp = size_t(mbi.BaseAddress)+mbi.RegionSize;
		addr = (void*)((tmp+0xFFFF)&~0xFFFF);
	}
}

struct xHeap
{
	BYTE* addr; DWORD size;
	
	void* alloc(DWORD size_) 
	{
		void* p = addr+size; size += size_; 
		if(size > HEAP_SIZE) __builtin_trap();
		return p; 
	}
};

struct xHeaps
{
	DWORD nHeaps;
	xHeap xHeaps[HEAP_NMAX];
	
	void* alloc(size_t size, void *addr)
	{
		// locate heap in range
		xHeap* xPos = xHeaps;
		xHeap* xposEnd = xHeaps+nHeaps;
		for(;xPos < xposEnd; xPos++) {
			ssize_t diff = ssize_t(xPos->addr) - ssize_t(addr);
			if((diff < MEM_RANGE) && (diff > -MEM_RANGE))
				goto DO_ALLOC;
		}	
		
		// allocate new heap
		if(++nHeaps > HEAP_NMAX) __builtin_trap();
		xPos->addr = (BYTE*)xheap_valloc(addr);
		DO_ALLOC: return xPos->alloc(size);
	}
};


static xHeaps s_Heaps;

extern "C"
void* xheap_alloc(size_t size, void *addr)
{
	xHeaps* x = &s_Heaps; VARFIX(x);
	return x->alloc(size, addr);
}

#else

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

#endif


