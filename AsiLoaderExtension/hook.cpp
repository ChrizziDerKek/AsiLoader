#include <Windows.h>
#include <vector>

struct Hook {
	void* func;
	uintptr_t jump;
	std::vector<std::pair<size_t, int>> references;
};

void* relaySpace;
uintptr_t currentRelayLocation;
std::vector<Hook> hooks;

uintptr_t AllocateJumpSpace(uintptr_t* addr) {
	while ((*addr) % 16)
		(*addr)++;
	uintptr_t result = *addr;
	(*addr) += 12;
	return result;
}

std::vector<std::pair<size_t, int>> GetReferences(void* fn) {
	std::vector<std::pair<size_t, int>> result;
	size_t func = (size_t)fn;
	size_t exe = (size_t)GetModuleHandleA(NULL);
	IMAGE_NT_HEADERS* ntHeaders = (IMAGE_NT_HEADERS*)(exe + ((IMAGE_DOS_HEADER*)exe)->e_lfanew);
	for (size_t addr = exe; addr < exe + ntHeaders->OptionalHeader.SizeOfImage; addr++) {
		if (*(BYTE*)addr == 0xE9 || *(BYTE*)addr == 0xE8) {
			int rva = 0;
			memcpy(&rva, (BYTE*)addr + 1, sizeof(rva));
			if (addr + 5 + rva == func)
				result.push_back(std::make_pair(addr, rva));
		}
	}
	return result;
}

__declspec(dllexport) void HookCreate(void* func, void* hook) {
	if (hooks.size() == 0) {
		relaySpace = VirtualAlloc((void*)(0x20000000 + GetModuleHandleA(NULL)), 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		currentRelayLocation = uintptr_t(relaySpace);
	}
	Hook h = { func, AllocateJumpSpace(&currentRelayLocation), GetReferences(func) };
	BYTE shell[] = { 0x48, 0xB8, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xFF, 0xE0 };
	*(uintptr_t*)(shell + 2) = (uintptr_t)hook;
	memcpy((void*)h.jump, shell, sizeof(shell));
	for (auto& ref : h.references) {
		size_t addr = ref.first;
		DWORD old;
		VirtualProtect((void*)addr, 5, PAGE_EXECUTE_READWRITE, &old);
		*(int*)(addr + 1) = int(h.jump - addr - 5);
		VirtualProtect((void*)addr, 5, old, &old);
	}
	hooks.push_back(h);
}

__declspec(dllexport) bool HookRemove(void* func) {
	Hook* h = NULL;
	for (Hook& hook : hooks) {
		if (hook.func == func) {
			h = &hook;
			break;
		}
	}
	if (!h)
		return false;
	for (auto& ref : h->references) {
		size_t addr = ref.first;
		DWORD old;
		VirtualProtect((void*)addr, 5, PAGE_EXECUTE_READWRITE, &old);
		*(int*)(addr + 1) = ref.second;
		VirtualProtect((void*)addr, 5, old, &old);
	}
	memset((void*)h->jump, 0xCC, 12);
	for (auto it = hooks.begin(); it != hooks.end(); it++) {
		if (it->func == func) {
			hooks.erase(it);
			break;
		}
	}
	if (hooks.size() == 0) {
		VirtualFree(relaySpace, 0, MEM_RELEASE);
		relaySpace = NULL;
		currentRelayLocation = 0;
	}
	return true;
}