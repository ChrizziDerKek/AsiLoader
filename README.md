# AsiLoader
This project was made just for fun but could possibly still be useful. It implements an injection technique called dll hijacking.

## How it works
It works because windows tries to load certain system dlls from a program's execution directory first before looking for it in system32 or somewhere else. So all we have to do is find a random dll that the program loads and create our own dll with the same name. Theoretically, we can make this decoy dll run our own code, for example for injecting our scripts. But in practice, there's one more problem: The exports of the dll. The program probably doesn't load the dll we chose just for fun, but for using its api functions. So obviously our decoy dll also needs to export the same functions with the same functionality or else the program would crash because it's unable to find some functions. To fix this and because I didn't want to reimplement all the dlls for obvious reasons, I decided to create a simple struct that first loads the original dll and stores all its exported functions in an array. Through this array we get access to the real functions that we need. Then I also created a lot of stub functions:

![Alt text](images/6.png?raw=true "Title")

Each one of these calls a specific function in our array. Normally we would need to pass the correct arguments for each function for it to work correctly but again, this is really tedious so I decided to always call a specific array index. If we look at the assembly code, we notice something:

![Alt text](images/5.png?raw=true "Title")

All the stub functions get compiled as a simple jmp instruction to the real function. That means that we don't have to worry about any arguments because we just jump to the real function anyways. The only downside is that this only works with a static size array, but I guess it's better than having to define every single function correctly.

Now the last thing to do is map the stub functions to the real ones that we need to export. This is done by using a .def file for each supported dll:

![Alt text](images/7.png?raw=true "Title")
