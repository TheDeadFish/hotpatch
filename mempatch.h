#pragma once

class UnProtect
{
public:
	UnProtect(void* base, size_t length);
	~UnProtect();

private:
	size_t pageBase;
	size_t pageEnd;
	DWORD flOldProtect;
};
