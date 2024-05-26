# AsiLoader
This project was made just for fun but could possibly still be useful. It implements an injection technique called dll hijacking.

## How it works
It works because windows tries to load certain system dlls from a program's execution directory first before looking for it in system32 or somewhere else. So all we have to do is find a random dll that the program loads and create our own dll with the same name. Theoretically, we can make this decoy dll run our own code, for example for injecting our scripts. But in practice, there's one more problem: The exports of the dll. The program probably doesn't load the dll we chose just for fun, but for using its api functions. So obviously our decoy dll also needs to export the same functions with the same functionality or else the program would crash because it's unable to find some functions. 
