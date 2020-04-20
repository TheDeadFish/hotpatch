// ApiHook, importfun & callpatch
// DeadFish Shitware 2014
#define HOTCALL(ftype, addr) (*((typeof(&ftype))(size_t(addr))))
#ifndef _APIHOOK_H_
#define _APIHOOK_H_

// imageHlp functions
static IMAGE_NT_HEADERS* GetImageNtHeaders(Void imageData) {
	IMAGE_DOS_HEADER* dosHeadr = imageData;
	return imageData + dosHeadr->e_lfanew; }
static DWORD GetImageSizeOfImage(Void imageData) { return
	GetImageNtHeaders(imageData)->OptionalHeader.SizeOfImage; }
static LPVOID GetImageEndPtr(Void imageData) {
	return imageData + GetImageSizeOfImage(imageData); }
DEF_RETPAIR(ImageDirectoryEntryToData_t, Void, Addr, DWORD, Size);
ImageDirectoryEntryToData_t WINAPI ImageDirectoryEntryToData(
	Void imageData, USHORT DirectoryEntry);

struct UnProtect
{
	UnProtect(Void base, SIZE_T length);
	~UnProtect();
	static bool callPatch_FF15(void* lpCall,
		void* lpNewProc, void* lpOldIA);
	static void longjump(void* lpCall, void* lpNewProc);
	bool callPatch_FF15(void* lpNewProc, void* lpOldIA) {
		return callPatch_FF15(pageBase, lpNewProc, lpOldIA); }
	void longjump(void* lpNewProc) {
		return longjump(pageBase, lpNewProc); }
		
	static void jump64(void* lpCall, void* lpNewProc);
		

//private:
	Void pageBase;
	Void pageEnd;
	DWORD flOldProtect;
};

// importfun.h
class importDiscr
{
public:
	bool fromName(cch* name);
	bool fromBase(PVOID base);
	
	// name/function lookup
	cch* dllName(u32 dllIndex);
	cch* funcName(u32 dllIndex, u32 funcIndex);
	int findDll(cch* dllName);
	DEF_RETPAIR(findFunc_t, int, dllIndex, int, funcIndex);
	findFunc_t findFunc(cch* dll, cch* func);
		
	// function get/set
	PVOID* getSlot(u32 dllIndex, u32 funcIndex);
	PVOID* setFunc(u32 dllIndex, u32 funcIndex,
		Void pFunc, PVOID* pFuncPrev );
	PVOID* setFunc(cch* dll, cch* func,
		Void pFunc, PVOID* pFuncPrev );
	
//private:
	char* rvaToStr(DWORD rva) {
		return (char*)((PBYTE)hInstance+rva); }
	DWORD* rvaToInt(DWORD rva) {
		return (DWORD*)((PBYTE)hInstance+rva); }
	
	HINSTANCE hInstance;
	PIMAGE_IMPORT_DESCRIPTOR discr;
};

// callpatch.h
void apihook_jump64(Void lpCall, Void lpNewProc);

#endif
