#ifndef _HOTPATCH_H_
#define _HOTPATCH_H_

#define HOTPATCH(new, old, pp) hotPatch((void*)new, (void*)old, (void**)pp)
#define HOTCALL(ftype, addr) (*((typeof(&ftype))(size_t(addr))))
bool hotPatch(void* lpNewProc, void* lpOldProc, void** lpPatchProc);

#endif
