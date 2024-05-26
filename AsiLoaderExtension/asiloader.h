#pragma once

typedef unsigned char BYTE;
typedef unsigned long DWORD;
#ifdef _WIN64
typedef unsigned long long size_t;
#else
typedef unsigned int size_t;
#endif
#if ASILOADEREXTENSION_EXPORTS == 0
#pragma comment(lib, "AsiLoaderExtension.lib")
#endif



//******************
//HOOKING
//******************

/// <summary>
/// Creates a detour hook for the provided function.
/// This replaces all jumps and calls to the function
/// with a call or jump to the hook, so you can still
/// call the original function if you need it
/// </summary>
/// <param name="func">Function to hook</param>
/// <param name="hook">Hook function</param>
void HookCreate(void* func, void* hook);

/// <summary>
/// Removes a previously created hook and reverts
/// the function back to its old state
/// </summary>
/// <param name="func">Hooked function</param>
/// <returns>True on successful removal</returns>
bool HookRemove(void* func);



//******************
//PATTERN SCANNING
//******************

/// <summary>
/// Scans an IDA style pattern
/// </summary>
/// <param name="pattern">Pattern to scan for</param>
/// <returns>Pointer to the pattern on success, null on fail</returns>
void* ScanPattern(const char* pattern);

/// <summary>
/// Rips the specific call or jump, useful
/// for following jumps, calls, etc.
/// </summary>
/// <param name="address">Call or jump address</param>
/// <returns>Call or jump destination</returns>
void* Rip(void* address);

/// <summary>
/// Adds an offset to the provided address
/// </summary>
/// <param name="address">Address</param>
/// <param name="offset">Offset to add</param>
/// <returns>Address + offset</returns>
void* Add(void* address, int offset);

/// <summary>
/// Subtracts an offset from the provided address
/// </summary>
/// <param name="address">Address</param>
/// <param name="offset">Offset to subtract</param>
/// <returns>Address - offset</returns>
void* Sub(void* address, int offset);



//******************
//BYTE PATCHING
//******************

/// <summary>
/// Creates a bytepatch on the address with
/// a specific size. A created bytepatch is
/// inactive by default
/// </summary>
/// <param name="address">Address to patch</param>
/// <param name="patch">Patched bytes</param>
/// <param name="size">Number of bytes to patch</param>
void BytePatchCreate(void* address, BYTE* patch, size_t size);

/// <summary>
/// Enables or disables an existing bytepatch
/// </summary>
/// <param name="address">Address of the bytepatch</param>
/// <param name="enabled">True if you want to enable the patch, otherwise false</param>
/// <returns>True on success</returns>
bool BytePatchSet(void* address, bool enabled);

/// <summary>
/// Removes an existing bytepatch and reverts
/// the bytes back to their old state
/// </summary>
/// <param name="address">Address of the bytepatch</param>
/// <returns>True on success</returns>
bool BytePatchRemove(void* address);

/// <summary>
/// Applies the specified protection to the whole application
/// </summary>
/// <param name="protection">Protection to apply</param>
/// <returns>Old protection</returns>
DWORD VirtualProtectEverything(DWORD protection);



//******************
//CONSOLE
//******************

/// <summary>
/// Creates an empty console
/// </summary>
void ConsoleCreate();

/// <summary>
/// Removes an existing console
/// </summary>
void ConsoleRemove();

/// <summary>
/// Writes a string to the console
/// </summary>
/// <param name="str">String to write</param>
void ConsoleLog(const char* str);



//******************
//TASKS
//******************

/// <summary>
/// Runs all created tasks, should be called
/// in a loop in your own task scheduler thread
/// </summary>
void TaskTick();

/// <summary>
/// Creates a new task
/// </summary>
/// <param name="func">Task callback function</param>
void TaskCreate(void(*func)());

/// <summary>
/// Removes an existing task
/// </summary>
/// <param name="func">Task callback function to remove</param>
void TaskRemove(void(*func)());

/// <summary>
/// Makes the current task sleep for the provided time
/// </summary>
/// <param name="ms">Time to sleep in milliseconds</param>
void TaskSleep(DWORD ms);
