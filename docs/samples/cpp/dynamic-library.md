# C++ Dynamic Library
This is a console application that has a single dynamic library dependency.

[Source](https://github.com/soup-build/soup/tree/main/samples/cpp/dynamic-library)

## [library/recipe.sml](https://github.com/soup-build/soup/tree/main/samples/cpp/dynamic-library/library/recipe.sml)
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
      'library.h'
    ]
  }
]
```

## [library/public/library.h](https://github.com/soup-build/soup/tree/main/samples/cpp/dynamic-library/library/public/library.h)
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

## [library/library.cpp](https://github.com/soup-build/soup/tree/main/samples/cpp/dynamic-library/library/library.cpp)
A library file implements a sample class.
```cpp
#include "library.h"

namespace Samples::Cpp::DynamicLibrary::Library
{
  const char* Helper::GetName()
  {
    return "Soup";
  }
}
```

## [application/recipe.sml](https://github.com/soup-build/soup/tree/main/samples/cpp/dynamic-library/application/recipe.sml)
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

## [application/package-lock.sml](https://github.com/soup-build/soup/tree/main/samples/cpp/dynamic-library/application/package-lock.sml)
The package lock that was generated to capture the unique dependencies required to build this project and the dynamic library dependency.

## [application/main.cpp](https://github.com/soup-build/soup/tree/main/samples/cpp/dynamic-library/application/main.cpp)
A simple main method that prints our "Hello World, Soup Style!" by using the module from the library.
```cpp
#include <iostream>
#include <library.h>

using namespace Samples::Cpp::DynamicLibrary::Library;

int main()
{
  std::cout << "Hello World, " << Helper::GetName() << " Style!" << std::endl;
  return 0;
}
```

## [.gitignore](https://github.com/soup-build/soup/tree/main/samples/cpp/dynamic-library/)
A simple git ignore file to exclude all Soup build output.