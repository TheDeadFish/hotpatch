#ifndef _HOTPATCH_H_
#define _HOTPATCH_H_

#define HOTPATCH(new, old) hotPatch((void*)new, (void*)old)
#define HOTCALL(ftype, addr) (*((typeof(&ftype))(size_t(addr)+2)))
bool hotPatch(void* lpNewProc, void* lpOldProc);
void* callPatch(void* lpNewProc, void* lpCall);
void jmpPatch(void* lpNewProc, void* lpJmp);

#endif
