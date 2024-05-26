#include <Windows.h>
#include <vector>

struct Patch {
	void* address;
	bool state;
	std::vector<BYTE> original;
	std::vector<BYTE> patch;
};
std::vector<Patch> patches;

__declspec(dllexport) void BytePatchCreate(void* address, BYTE* patch, size_t size) {
	Patch p = { address, false };
	for (size_t i = 0; i < size; i++) {
		p.patch.push_back(patch[i]);
		p.original.push_back(*(BYTE*)((uintptr_t)address + i));
	}
	patches.push_back(p);
}

__declspec(dllexport) bool BytePatchSet(void* address, bool enabled) {
	Patch* p = NULL;
	for (Patch& patch : patches) {
		if (patch.address == address) {
			p = &patch;
			break;
		}
	}
	if (!p || p->state == enabled)
		return false;
	std::vector<BYTE> data = enabled ? p->patch : p->original;
	p->state = enabled;
	DWORD old;
	VirtualProtect(address, data.size(), PAGE_EXECUTE_READWRITE, &old);
	for (size_t i = 0; i < data.size(); i++)
		*(BYTE*)((uintptr_t)address + i) = data[i];
	VirtualProtect(address, data.size(), old, &old);
	return true;
}

__declspec(dllexport) bool BytePatchRemove(void* address) {
	Patch* p = NULL;
	for (Patch& patch : patches) {
		if (patch.address == address) {
			p = &patch;
			break;
		}
	}
	if (!p)
		return false;
	BytePatchSet(address, false);
	for (auto it = patches.begin(); it != patches.end(); it++) {
		if (it->address == address) {
			patches.erase(it);
			break;
		}
	}
	return true;
}

__declspec(dllexport) DWORD VirtualProtectEverything(DWORD protection) {
	size_t exe = (size_t)GetModuleHandleA(NULL);
	IMAGE_NT_HEADERS* ntHeaders = (IMAGE_NT_HEADERS*)(exe + ((IMAGE_DOS_HEADER*)exe)->e_lfanew);
	DWORD result = NULL;
	VirtualProtect((void*)exe, ntHeaders->OptionalHeader.SizeOfImage, protection, &result);
	return result;
}