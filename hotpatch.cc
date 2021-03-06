// Hotpatch V2.25, 21/01/2015
// DeadFish Shitware

#include <windows.h>
#include <string.h>
#include "hotpatch.h"
#include "mempatch.h"

#define SKIP 0x00
#define WROD 0x10
#define SEGM 0x20
#define REGM 0x30
#define IMPL 0x40
#define OFFS 0x50
#define GRP5 0x60
#define GRP3 0x70
#define FLOW 0x80

#ifdef __x86_64__
 #define IM64 0x90
 #define REXD SEGM
 #define REXQ 0xA0
#else 
 #define IM64 IMPL
 #define REXD IMPL
 #define REXQ IMPL
#endif

static const BYTE OneByte[] = {
	0|REGM, 0|REGM, 0|REGM, 0|REGM, 1|IMPL, 4|IMPL, 0|IMPL, 0|IMPL, // 00
	0|REGM, 0|REGM, 0|REGM, 0|REGM, 1|IMPL, 4|IMPL, 0|IMPL, 0|SKIP, // 08
	0|REGM, 0|REGM, 0|REGM, 0|REGM, 1|IMPL, 4|IMPL, 0|IMPL, 0|IMPL, // 10
	0|REGM, 0|REGM, 0|REGM, 0|REGM, 1|IMPL, 4|IMPL, 0|IMPL, 0|IMPL, // 18
	0|REGM, 0|REGM, 0|REGM, 0|REGM, 1|IMPL, 4|IMPL, 0|SEGM, 0|IMPL, // 20
	0|REGM, 0|REGM, 0|REGM, 0|REGM, 1|IMPL, 4|IMPL, 0|SEGM, 0|IMPL, // 28
	0|REGM, 0|REGM, 0|REGM, 0|REGM, 1|IMPL, 4|IMPL, 0|SEGM, 0|IMPL, // 30
	0|REGM, 0|REGM, 0|REGM, 0|REGM, 1|IMPL, 4|IMPL, 0|SEGM, 0|IMPL, // 38
	0|REXD, 0|REXD, 0|REXD, 0|REXD, 0|REXD, 0|REXD, 0|REXD, 0|REXD, // 40
	0|REXQ, 0|REXQ, 0|REXQ, 0|REXQ, 0|REXQ, 0|REXQ, 0|REXQ, 0|REXQ, // 48
	0|IMPL, 0|IMPL, 0|IMPL, 0|IMPL, 0|IMPL, 0|IMPL, 0|IMPL, 0|IMPL, // 50
	0|IMPL, 0|IMPL, 0|IMPL, 0|IMPL, 0|IMPL, 0|IMPL, 0|IMPL, 0|IMPL, // 58
	0|IMPL, 0|IMPL, 0|SKIP, 0|SKIP, 0|SEGM, 0|SEGM, 0|WROD, 0|SKIP, // 60
	4|IMPL, 4|REGM, 1|IMPL, 1|REGM, 0|IMPL, 0|IMPL, 0|IMPL, 0|IMPL, // 68
	0|SKIP, 0|SKIP, 0|SKIP, 0|SKIP, 0|SKIP, 0|SKIP, 0|SKIP, 0|SKIP, // 70
	0|SKIP, 0|SKIP, 0|SKIP, 0|SKIP, 0|SKIP, 0|SKIP, 0|SKIP, 0|SKIP, // 78
	1|REGM, 4|REGM, 0|SKIP, 1|REGM, 0|REGM, 0|REGM, 0|REGM, 0|REGM, // 80
	0|REGM, 0|REGM, 0|REGM, 0|REGM, 0|SKIP, 0|REGM, 0|SKIP, 0|REGM, // 88
	0|IMPL, 0|IMPL, 0|IMPL, 0|IMPL, 0|IMPL, 0|IMPL, 0|IMPL, 0|IMPL, // 90
	0|IMPL, 0|IMPL, 0|SKIP, 0|SKIP, 0|SKIP, 0|SKIP, 0|SKIP, 0|SKIP, // 98
	4|OFFS, 4|OFFS, 4|OFFS, 4|OFFS, 0|IMPL, 0|IMPL, 0|IMPL, 0|IMPL, // A0
	1|IMPL, 4|IMPL, 0|IMPL, 0|IMPL, 0|IMPL, 0|IMPL, 0|IMPL, 0|IMPL, // A8
	1|IMPL, 1|IMPL, 1|IMPL, 1|IMPL, 1|IMPL, 1|IMPL, 1|IMPL, 1|IMPL, // B0
	4|IM64, 4|IM64, 4|IM64, 4|IM64, 4|IM64, 4|IM64, 4|IM64, 4|IM64, // B8
	1|REGM, 1|REGM, 0|SKIP, 0|SKIP, 0|SKIP, 0|SKIP, 0|REGM, 4|REGM, // C0
	4|IMPL, 0|IMPL, 0|SKIP, 0|SKIP, 0|SKIP, 0|SKIP, 0|SKIP, 0|SKIP, // C8
	0|REGM, 0|REGM, 0|REGM, 0|REGM, 0|IMPL, 0|IMPL, 0|SKIP, 0|SKIP, // D0
	0|SKIP, 0|SKIP, 0|SKIP, 0|SKIP, 0|SKIP, 0|SKIP, 0|SKIP, 0|SKIP, // D8
	0|SKIP, 0|SKIP, 0|SKIP, 0|SKIP, 1|IMPL, 1|IMPL, 1|IMPL, 1|IMPL, // E0
	4|FLOW, 0|SKIP, 0|SKIP, 0|SKIP, 0|IMPL, 0|IMPL, 0|IMPL, 0|IMPL, // E8
	0|SKIP, 0|SKIP, 0|SKIP, 0|SKIP, 0|SKIP, 0|SKIP, 1|GRP3, 4|GRP3, // F0
	0|IMPL, 0|IMPL, 0|IMPL, 0|IMPL, 0|IMPL, 0|IMPL, 0|REGM, 0|GRP5  // F8
}; 

#define CASE0(x, ...) if(0){ case x>>4: __VA_ARGS__; }
#define CASEX(x, ...) { case x>>4: __VA_ARGS__; }

InstLen hotPatch_instLen(void* ptr, int flags)
{
	unsigned char* bptr =
		(unsigned char*)ptr;
	int length = 0;
	int offset = 0;
	bool word = false;
	bool quad = false;
	
	while(1)
	{
		int opcode = OneByte[*bptr++];
		int regmem = *bptr;
		int size = opcode & 0x0F;
		int type = opcode >> 4;
		length++;

		switch(type)
		{
		CASEX(SKIP, return {-1, 0});
		CASE0(WROD, word = true);
#ifdef __x86_64__
		CASE0(REXQ, quad = true);
#endif
		CASEX(SEGM, continue);
		
		CASEX(GRP3, 
			if((regmem >> 3) & 7)	
				size = 1
		);
		
		CASE0(GRP5, 
			switch((regmem >> 3) & 7) {
			case 0: case 1: case 6: break;
			default: return {-1, 0}; }
		);
		
		CASEX(REGM, length++;
			if(regmem < 0xC0) {
				if((regmem & 7) == 4)	length++;
				if(regmem >= 0x40) { length++;
					if(regmem >= 0x80) length += 3;
				} else if((regmem & 7) == 5) {
					offset = length; length += 4; }
			}
		);
			
#ifdef __x86_64__
		CASE0(IM64, if(quad) size += 4)
#endif
		CASEX(IMPL,	if(word && (size == 4)) size = 2)
		CASE0(FLOW, if(!flags) return {-1, 0})
		CASE0(OFFS, offset = length)
			length += size;	
			break;
		default:
			__builtin_unreachable();
		}
		break;
	}
	return {length, offset};
}

static
void hotPatchError()
{
	RaiseException(0xC0000001,
		EXCEPTION_NONCONTINUABLE, 0, NULL);
}

static
void hotPatch_makeJump(BYTE* src_, void* dst_)
{
	BYTE *src = (BYTE*)src_, *dst = (BYTE*)dst_;
	*src = 0xE9; *(int*)(src+1) = (dst-src)-5;
}

static 
void* hotPatch_getJump(void* ptr_)
{
	BYTE* ptr = (BYTE*)ptr_;
	return ptr+*(int*)(ptr+1)+5;
}

int hotPatch_getLen(BYTE* funcBase, int bytesNeeded)
{
	int bytesTaken = 0;
	while(bytesTaken < bytesNeeded)
	{
		int len = hotPatch_instLen(funcBase+bytesTaken,0).len;
		if(len < 0) return len;
		bytesTaken += len;
	}
	return bytesTaken;
}

void* hotPatch_getCall_(void* ptr_, int i)
{
	BYTE* ptr = (BYTE*)ptr_;
	while((*ptr != 0xE8) || (--i >= 0)) { 
		ptr += hotPatch_instLen(ptr,1).len; }
	return ptr;
}

void* hotPatch_getCall(void* ptr, int i)
{
	return hotPatch_getJump(
		hotPatch_getCall_(ptr, i));
}

void hotPatch_static(void* lpPatchProc,
	void* lpOldProc, DWORD maxSize)
{
	BYTE* funcBase = (BYTE*)(lpOldProc);
	int bytesTaken = hotPatch_getLen(funcBase, 5);
	if(bytesTaken > maxSize) hotPatchError();
	memcpy(lpPatchProc, funcBase, bytesTaken);
	hotPatch_makeJump((BYTE*)lpPatchProc+bytesTaken, funcBase+bytesTaken);
}

void hotPatch(void* lpOldProc, void* lpNewProc, void** lpPatchProc)
{
	// check signature
	int bytesNeeded = 5;
	BYTE* funcBase = (BYTE*)(lpOldProc);
	if(memcmp(funcBase-5, "\x90\x90\x90\x90\x90", 5) == 0) {
		bytesNeeded = (*((WORD*)funcBase) != 0xFF8B) ? 2 : 0; }
	
	if( lpPatchProc != NULL )
	{
		// try and room for jump
		if(bytesNeeded == 0) {
			*lpPatchProc = (void*)(funcBase+2);
			goto PATCH_CODE; }
		
		int bytesTaken = hotPatch_getLen(
			funcBase, bytesNeeded);
		BYTE* pe = (BYTE*)xheap_alloc(bytesTaken+5);
		if(pe == NULL) hotPatchError();
		
		memcpy(pe, funcBase, bytesTaken);
		hotPatch_makeJump(pe+bytesTaken, 
			funcBase+bytesTaken);		
		*lpPatchProc = (void*)pe;
	}
	
	// Patch the code
PATCH_CODE:
	UnProtect unProtect(funcBase-5, 13);
	if(bytesNeeded != 5)
	{
		hotPatch_makeJump(funcBase-5, lpNewProc);
		*(WORD*)(funcBase) = 0xF9EB;
	}
	else
	{
		hotPatch_makeJump(funcBase, lpNewProc);
	}
}
