# C Static Library Reference
This is a console application that has a single static library dependency.

[Source](https://github.com/soup-build/soup/tree/main/samples/c/static-library)

## [library/recipe.sml](https://github.com/soup-build/soup/tree/main/samples/c/static-library/library/recipe.sml)
The Recipe file that defines the static library "Samples.C.StaticLibrary.Library".
```sml
Name: 'Samples.C.StaticLibrary.Library'
Language: 'C|0'
Version: 1.0.0
Type: 'StaticLibrary'
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

## [library/library.h](https://github.com/soup-build/soup/tree/main/samples/c/static-library/library/library.h)
A header interface file that declares a single sample function.
```c
const char* GetName();
```

## [library/library.c](https://github.com/soup-build/soup/tree/main/samples/c/static-library/library/library.c)
A implementation file that defines a single sample function.
```c
#include "library.h"

const char* GetName()
{
  return "Soup";
}
```

## [application/recipe.sml](https://github.com/soup-build/soup/tree/main/samples/c/static-library/application/recipe.sml)
The Recipe file that defines the executable "Samples.C.StaticLibrary.Application".
```sml
Name: 'Samples.C.StaticLibrary.Application'
Language: 'C|0'
Type: 'Executable'
Version: 1.0.0
Dependencies: {
  Runtime: [
    '../library/'
  ]
}
```

## [application/package-lock.sml](https://github.com/soup-build/soup/tree/main/samples/c/static-library/application/package-lock.sml)
The package lock that was generated to capture the unique dependencies required to build this project and the dependency static library.
```sml
Version: 5
Closures: {
  Root: {
    C: {
      'Samples.C.StaticLibrary.Application': { Version: './', Build: 'Build0', Tool: 'Tool0' }
      'Samples.C.StaticLibrary.Library': { Version: '../library/', Build: 'Build0', Tool: 'Tool0' }
    }
  }
  Build0: {
    Wren: {
      'Soup|C': { Version: 0.5.0 }
    }
  }
  Tool0: {
    'C++': {
      'mwasplund|copy': { Version: 1.2.0 }
      'mwasplund|mkdir': { Version: 1.2.0 }
    }
  }
}
```

## [application/main.c](https://github.com/soup-build/soup/tree/main/samples/c/static-library/application/main.c)
A simple main method that prints our "Hello World, Soup Style!" by using the module from the library.
```c
#include <stdio.h>
#include <library.h>

int main()
{
  printf("Hello World, %s Style!", GetName());
  return 0;
}
```

## [.gitignore](https://github.com/soup-build/soup/tree/main/samples/c/static-library/.gitignore)
A simple git ignore file to exclude all Soup build output.
```
out/
```