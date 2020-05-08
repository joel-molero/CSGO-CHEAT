#pragma once
#include <Windows.h>
#include <vector>
//https://guidedhacking.com/threads/codys-c-csgo-triggerbot-tutorial-external-part1.11401/
//https://guidedhacking.com/donate


class MemMan
{
public:
	MemMan();
	~MemMan();
	HANDLE gethandle()
	{
		return handle;
	}
	template <class val>
	val readMem(DWORD addr)
	{
		val x;
		ReadProcessMemory(handle, (LPBYTE*)addr, &x, sizeof(x), NULL);
		return x;
	}
	template <class val>
	val writeMem(DWORD addr, val x)
	{
		WriteProcessMemory(handle, (LPBYTE*)addr, &x, sizeof(x), NULL);
		return x;
	}
	DWORD getProcess(const char*);
	uintptr_t getModule(DWORD, const char*);
	DWORD getAddress(DWORD, std::vector<DWORD>);
	HANDLE handle;
};