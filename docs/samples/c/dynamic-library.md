# C Dynamic Library
This is a console application that has a single dynamic library dependency.

[Source](https://github.com/soup-build/soup/tree/main/samples/c/dynamic-library)

## [library/recipe.sml](https://github.com/soup-build/soup/tree/main/samples/c/dynamic-library/library/recipe.sml)
The Recipe file that defines the static library "Samples.C.DynamicLibrary.Library".
```sml
Name: 'Samples.C.DynamicLibrary.Library'
Language: 'C|0'
Version: 1.0.0
Type: 'DynamicLibrary'
IncludePaths: [
  'public/'
]
PublicHeaders: [
  {
    Root: 'public/'
    Files: [
      'library.h'
    ]
  }
]
```

## [library/library.h](https://github.com/soup-build/soup/tree/main/samples/c/dynamic-library/library/library.h)
A header interface file that declares a single sample function.
```c
#ifdef _WIN32
__declspec(dllexport)
#endif
const char* GetName();
```

## [library/library.c](https://github.com/soup-build/soup/tree/main/samples/c/dynamic-library/library/library.c)
A implementation file that defines a single sample function.
```c
#include "library.h"

const char* GetName()
{
  return "Soup";
}
```

## [application/recipe.sml](https://github.com/soup-build/soup/tree/main/samples/c/dynamic-library/application/recipe.sml)
The Recipe file that defines the executable "Samples.C.DynamicLibrary.Application".
```sml
Name: 'Samples.C.DynamicLibrary.Application'
Language: 'C|0'
Type: 'Executable'
Version: 1.0.0
Dependencies: {
  Runtime: [
    '../library/'
  ]
}
```

## [application/package-lock.sml](https://github.com/soup-build/soup/tree/main/samples/c/dynamic-library/application/package-lock.sml)
The package lock that was generated to capture the unique dependencies required to build this project and the dynamic library dependency.

## [application/main.c](https://github.com/soup-build/soup/tree/main/samples/c/dynamic-library/application/main.c)
A simple main method that prints our "Hello World, Soup Style!" by using the module from the library.
```c
#include <stdio.h>
#include <Library.h>

int main()
{
  printf("Hello World, %s Style!", GetName());
  return 0;
}
```

## [.gitignore](https://github.com/soup-build/soup/tree/main/samples/c/dynamic-library/.gitignore)
A simple git ignore file to exclude all Soup build output.