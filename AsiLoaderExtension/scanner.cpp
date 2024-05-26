#include <Windows.h>
#include <vector>

std::vector<std::pair<BYTE, bool>> ConvertPattern(const char* pattern) {
	std::vector<std::pair<BYTE, bool>> result;
	for (char* c = (char*)pattern; c < pattern + strlen(pattern); c++) {
		if (*c == '?') {
			c++;
			if (*c == '?')
				c++;
			result.push_back(std::make_pair(0, false));
			continue;
		}
		result.push_back(std::make_pair((BYTE)strtoul(c, &c, 0x10), true));
	}
	return result;
}

__declspec(dllexport) void* ScanPattern(const char* pattern) {
	HMODULE mod = GetModuleHandleA(NULL);
	PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)mod + ((PIMAGE_DOS_HEADER)mod)->e_lfanew);
	std::vector<std::pair<BYTE, bool>> bytes = ConvertPattern(pattern);
	auto scan = (BYTE*)mod;
	for (DWORD i = 0; i < ntHeaders->OptionalHeader.SizeOfImage - bytes.size(); ++i) {
		bool found = true;
		for (DWORD j = 0; j < bytes.size(); ++j) {
			if (scan[i + j] != bytes[j].first && bytes[j].second) {
				found = false;
				break;
			}
		}
		if (found)
			return &scan[i];
	}
	return NULL;
}

__declspec(dllexport) void* Rip(void* address) {
	return (void*)((uintptr_t)address + *(int*)address + sizeof(int));
}

__declspec(dllexport) void* Add(void* address, int offset) {
	return (void*)((uintptr_t)address + offset);
}

__declspec(dllexport) void* Sub(void* address, int offset) {
	return (void*)((uintptr_t)address - offset);
}