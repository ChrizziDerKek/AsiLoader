#include <Windows.h>
#include <vector>

__declspec(dllexport) void ConsoleCreate() {
	if (!AllocConsole())
		return;
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	SetConsoleTitleA("AsiLoader");
	SetConsoleCP(CP_UTF8);
}

__declspec(dllexport) void ConsoleRemove() {
	FreeConsole();
	HWND console = FindWindowA(NULL, "AsiLoader");
	if (!console)
		return;
	ShowWindow(console, 0);
}

__declspec(dllexport) void ConsoleLog(const char* str) {
	printf("%s\n", str);
}