#include "stdshit.h"
#include "apihook.h"

static 
bool isWritable(DWORD protect) {
	return ((protect == PAGE_EXECUTE_READWRITE)
	 ||(protect == PAGE_EXECUTE_WRITECOPY)
	 ||(protect == PAGE_READWRITE)
	 ||(protect == PAGE_WRITECOPY)); }

UnProtect::UnProtect(Void base, SIZE_T length) 
{
	// check block
	pageBase = base;
	pageEnd = base+length;
	MEMORY_BASIC_INFORMATION mbi;
	if(!VirtualQuery(pageBase, &mbi, sizeof(mbi))) {
		pageBase = 0; return; }
	Void blockEnd = Void(mbi.BaseAddress)
		+ mbi.RegionSize;
	bool UnProtectFirst = !isWritable(mbi.Protect);
	if(pageEnd <= blockEnd) {
		if(UnProtectFirst == false) {
			pageEnd = pageBase; return; }
		goto UNPROTECT_BLOCK; }
	
	// check second block
	{if(!VirtualQuery(blockEnd, &mbi, sizeof(mbi))) {
		pageBase = 0; return; }
	Void blockEnd2 = Void(mbi.BaseAddress)
		+ mbi.RegionSize;
	bool UnProtectSecond = !isWritable(mbi.Protect);
	if((pageEnd > blockEnd2)
	||(UnProtectFirst && UnProtectSecond)) return;
	if(!UnProtectFirst && !UnProtectSecond) {
		pageEnd = pageBase; return; }
	if(UnProtectFirst) pageEnd = blockEnd;
		else pageBase = blockEnd; }

	// unprotect new block
UNPROTECT_BLOCK:
	length = pageEnd - pageBase;
	if(!VirtualProtect(pageBase, length, 
	PAGE_EXECUTE_READWRITE, &flOldProtect)) {
		pageBase = 0; return; }
}
UnProtect::~UnProtect(void) {
	VirtualProtect((PVOID)pageBase, pageEnd-pageBase,
		flOldProtect, &flOldProtect); 
}

bool UnProtect::callPatch_FF15(void* lpCall,
	void* lpNewProc, void* lpOldIA)
{
	BYTE* callBase = (BYTE*)lpCall;
	void* lpOldCall = *(void**)(callBase+2);
	if(( *(WORD*)lpCall != 0x15FF )
	||( lpOldCall != lpOldIA))
		return false;
	*(WORD*)callBase = 0xE890;	
	*(size_t*)(callBase+2) = (BYTE*)(lpNewProc)-callBase-6;
	return true;
}

void UnProtect::longjump(
	void* lpCall, void* lpNewProc)
{
	BYTE* callBase = (BYTE*)lpCall;
	callBase[0] = 0xE9; *(size_t*)(callBase+1) = 
		((BYTE*)lpNewProc)-callBase-5;
}

void UnProtect::jump64(
	void* lpCall, void* lpNewProc)
{
	BYTE* callBase = (BYTE*)lpCall;
	memcpy(callBase, "\x48\xff\x25\0\0\0", 7);
	*(void**)(callBase+7) = lpNewProc;
}

ImageDirectoryEntryToData_t WINAPI ImageDirectoryEntryToData(
	Void imageData, USHORT DirectoryEntry) {
	IMAGE_NT_HEADERS* peHeadr = GetImageNtHeaders(imageData);
	IMAGE_DATA_DIRECTORY* dataDir = peHeadr->OptionalHeader.DataDirectory;
	return ImageDirectoryEntryToData_t(imageData+dataDir[DirectoryEntry].
		VirtualAddress, dataDir[DirectoryEntry].Size); }
		
bool importDiscr::fromBase(PVOID base)
{
	hInstance = Void(base);
	if(hInstance == NULL) return false;
	discr = ImageDirectoryEntryToData(hInstance,
		IMAGE_DIRECTORY_ENTRY_IMPORT).Addr;
	return true;
}

bool importDiscr::fromName(cch* name)
{
	return fromBase(GetModuleHandleA(name));
}


#define NULL_CHECK_RET(x) if(NULL_CHECK(x))	return Void(x)

cch* importDiscr::dllName(u32 dllIndex)
{
	NULL_CHECK_RET(discr[dllIndex].Name);
	return Void(hInstance, discr[dllIndex].Name);
}

cch* importDiscr::funcName(u32 dllIndex, u32 funcIndex)
{
	size_t slot = Void(hInstance,discr[dllIndex]
		.OriginalFirstThunk).sizet(funcIndex);
	if(!slot) return 0; if(isNeg(slot)) {
		return (cch*)(slot&0xFFFF); }
	return Void(hInstance, slot)+2;
}

int importDiscr::findDll(cch* dll)
{
	for(u32 i = 0;; i++) { cch* DllName = dllName(i);
		if(DllName == NULL) return findFunc_t(-1);
		if(lstrcmpiA(DllName, dll) == 0) return i; }
}

importDiscr::findFunc_t importDiscr::
	findFunc(cch* dll, cch* func)
{
	int dllIndex = findDll(dll);
	if(dllIndex < 0) return findFunc_t(-1);
	for(int i = 0;; i++) { 
		cch* FuncName = funcName(dllIndex, i);
		if(FuncName == NULL) return findFunc_t(-1);
		if(IS_PTR(FuncName)&&IS_PTR(func)) {
		if(lstrcmpiA(FuncName, func) == 0)
			return findFunc_t(dllIndex, i);	
		} else { if(FuncName == func)
			return findFunc_t(dllIndex, i);	}
	}
}

PVOID*  importDiscr::setFunc(
	u32 dllIndex, u32 funcIndex, Void pFunc, PVOID* pFuncPrev)
{
	PVOID* pSlot = getSlot(dllIndex, funcIndex);
	UnProtect unProtect(pSlot, sizeof(pSlot));
	if(!unProtect.pageBase) return 0;
	if(pFuncPrev) *pFuncPrev = *pSlot;
	*pSlot = pFunc; return pSlot;
}

PVOID*  importDiscr::setFunc(cch* dll, 
	cch* func, Void pFunc, PVOID* pFuncPrev )
{
	ARGFIX(pFunc);
	auto result = findFunc(dll, func);
	if(result < 0) return NULL;
	return setFunc(result.dllIndex, result.funcIndex, pFunc, pFuncPrev);
}

PVOID* importDiscr::getSlot(u32 dllIndex, u32 funcIndex)
{
	return Void(hInstance,discr[dllIndex]
		.FirstThunk).ptr<PVOID>(funcIndex);
}

void apihook_jump64(Void lpCall, Void lpNewProc)
{
	UnProtect up(lpCall, 15);
	UnProtect::jump64(lpCall, lpNewProc);
}
