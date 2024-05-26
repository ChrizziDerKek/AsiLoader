# AsiLoader
This project was made just for fun but could possibly still be useful. It implements an injection technique called dll hijacking.

## How to use
1) Find a dll that gets loaded by your target program
2) Choose that dll in the release section or add it to the project if it isn't available yet
3) Optional: If the dll isn't a system dll from system32 or syswow64, you need to leave the original dll in the same directory and add an underscore at the start of its name. You need this for the steam_api64.dll file for example (The original dll now needs to be renamed to _steam_api64.dll)
4) Copy your decoy dll in the program's directory
5) Done! Now the dll will load any .asi file in the same directory when the program starts

## Extension / SDK
The project also comes with a small SDK with functions that may be useful. The file is called AsiLoaderExtension.asi and implements the following functions:

```cpp
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
```

## How it works
It works because windows tries to load certain system dlls from a program's execution directory first before looking for it in system32 or somewhere else. So all we have to do is find a random dll that the program loads and create our own dll with the same name. Theoretically, we can make this decoy dll run our own code, for example for injecting our scripts. But in practice, there's one more problem: The exports of the dll. The program probably doesn't load the dll we chose just for fun, but for using its api functions. So obviously our decoy dll also needs to export the same functions with the same functionality or else the program would crash because it's unable to find some functions. To fix this and because I didn't want to reimplement all the dlls for obvious reasons, I decided to create a simple struct that first loads the original dll and stores all its exported functions in an array. Through this array we get access to the real functions that we need. Then I also created a lot of stub functions:

![Alt text](images/6.png?raw=true "Title")

Each one of these calls a specific function in our array. Normally we would need to pass the correct arguments for each function for it to work correctly but again, this is really tedious so I decided to always call a specific array index. If we look at the assembly code, we notice something:

![Alt text](images/5.png?raw=true "Title")

All the stub functions get compiled as a simple jmp instruction to the real function. That means that we don't have to worry about any arguments because we just jump to the real function anyways. The only downsides are that this only works with a static size array and doesn't work in debug mode, but I guess it's better than having to define every single function correctly.

Now the last thing to do is map the stub functions to the real ones that we need to export. This is done by using a .def file for each supported dll:

![Alt text](images/7.png?raw=true "Title")

We can find the exports of a dll by using the GetExports function in dllmain.cpp and then all we have to do is to map each one to a stub function in the correct order. This can easily be expanded so more dlls are supported, as you can see below.

## Add your own dll
As stated earlier, you can easily expand the project to support more dlls. It already supports a lot of common ones, but here's a quick tutorial on how you can add your own dll:

First, open the configuration manager and add a new configuration with the same name as the dll you want to use (Based on Release to copy all build settings):

![Alt text](images/1.png?raw=true "Title")
![Alt text](images/2.png?raw=true "Title")
![Alt text](images/3.png?raw=true "Title")

After that, you can add a .def file for the functions that the dll exports (Also with the same name as the dll and configuration):

![Alt text](images/4.png?raw=true "Title")

And that's it, all you have to do now is to define your .def file and compile the program. It doesn't require any changes on the actual code.
