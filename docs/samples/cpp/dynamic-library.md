# C++ Dynamic Library
This is a console application that has a single dynamic library dependency.

[Source](https://github.com/soup-build/soup/tree/main/samples/cpp/dynamic-library)

## Library/recipe.sml
The Recipe file that defines the static library "Samples.Cpp.DynamicLibrary.Library".
```sml
Name: 'Samples.Cpp.DynamicLibrary.Library'
Language: 'C++|0'
Version: 1.0.0
Type: 'DynamicLibrary'
Defines: [
  'EXPORT_LIBRARY'
]
IncludePaths: [
  'public/'
]
PublicHeaders: [
  {
    Root: 'public/'
    Files: [
      'Library.h'
    ]
  }
]
```

## Library/Public/Library.h
A module header file that declares a single sample class.
```cpp
#ifdef _WIN32
  #ifdef EXPORT_LIBRARY
    #define LIBRARY_API __declspec(dllexport)
  #else
    #define LIBRARY_API __declspec(dllimport)
  #endif
#else
  #define LIBRARY_API
#endif

namespace Samples::Cpp::DynamicLibrary::Library
{
  class Helper
  {
  public:
    LIBRARY_API static const char* GetName();
  };
}
```

## Library/Library.cpp
A library file implements a sample class.
```cpp
#include "Library.h"

namespace Samples::Cpp::DynamicLibrary::Library
{
  const char* Helper::GetName()
  {
    return "Soup";
  }
}
```

## Application/recipe.sml
The Recipe file that defines the executable "Samples.Cpp.DynamicLibrary.Application".
```sml
Name: 'Samples.Cpp.DynamicLibrary.Application'
Language: 'C++|0'
Type: 'Executable'
Version: 1.0.0
Dependencies: {
  Runtime: [
    '../library/'
  ]
}
```

## Application/package-lock.sml
The package lock that was generated to capture the unique dependencies required to build this project and the dynamic library dependency.
```sml
Version: 5
Closures: {
  Root: {
    'C++': {
      'Samples.Cpp.DynamicLibrary.Application': { Version: './', Build: 'Build0', Tool: 'Tool0' }
      'Samples.Cpp.DynamicLibrary.Library': { Version: '../library/', Build: 'Build0', Tool: 'Tool0' }
    }
  }
  Build0: {
    Wren: {
      'Soup|Cpp': { Version: 0.16.0 }
    }
  }
  Tool0: {
    'C++': {
      'mwasplund|copy': { Version: 1.2.0 }
      'mwasplund|mkdir': { Version: 1.2.0 }
      'mwasplund|parse.modules': { Version: 1.2.0 }
    }
  }
}
```

## Application/Main.cpp
A simple main method that prints our "Hello World, Soup Style!" by using the module from the library.
```cpp
#include <iostream>
#include <Library.h>

using namespace Samples::Cpp::DynamicLibrary::Library;

int main()
{
  std::cout << "Hello World, " << Helper::GetName() << " Style!" << std::endl;
  return 0;
}
```

## .gitignore
A simple git ignore file to exclude all Soup build output.
```
out/
```
