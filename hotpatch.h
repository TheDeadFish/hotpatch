// Hotpatch V2.1, 14/12/2013
// DeadFish Shitware

#ifndef _HOTPATCH_H_
#define _HOTPATCH_H_

#define HOTPATCH(old, new, pp) hotPatch((void*)old, (void*)new, (void**)pp)
#define HOTCALL(ftype, addr) (*((typeof(&ftype))(size_t(addr))))
bool hotPatch(void* lpOldProc, void* lpNewProc, void** lpPatchProc);

#endif
