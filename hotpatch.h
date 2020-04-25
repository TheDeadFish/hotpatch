// Hotpatch V2.22, 11/01/2014
// DeadFish Shitware

#ifndef _HOTPATCH_H_
#define _HOTPATCH_H_

#ifdef __cplusplus
extern "C" {
#endif


#define HOTPATCH(old, new, pp) hotPatch((void*)old, (void*)new, (void**)pp)
#define HOTCALL(ftype, addr) (*((typeof(&ftype))((size_t)(addr))))
void hotPatch(void* lpOldProc, void* lpNewProc, void** lpPatchProc);

void hotPatch_static(void* lpPatchProc,
	void* lpOldProc, DWORD maxSize);
	
typedef struct { int len; int ofs; } InstLen;
InstLen hotPatch_instLen(void* ptr, int flags);
int hotPatch_getLen(BYTE* funcBase, int bytesNeeded);
void* hotPatch_getCall_(void* ptr, int i);
void* hotPatch_getCall(void* ptr, int i);


// imported function address with stdcall
#ifdef _WIN64
	#define IMPGETX(fn,...)({ void* ret; void WINAPI __imp_##fn(__VA_ARGS__); *(void**)&__imp_##fn; })
#else
	#define IMPGETX(fn,...)({ void* ret; void WINAPI _imp__##fn(__VA_ARGS__); *(void**)&_imp__##fn; })
#endif
#define IMPGET0(fn) IMPGETX(fn,)
#define IMPGET1(fn) IMPGETX(fn,int)
#define IMPGET2(fn) IMPGETX(fn,int,int)
#define IMPGET3(fn) IMPGETX(fn,int,int)
#define IMPGET4(fn) IMPGETX(fn,int,int,int)
	
void* xheap_alloc(size_t size);

#ifdef __cplusplus
}
#endif

#endif
