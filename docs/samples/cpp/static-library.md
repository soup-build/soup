# Static Library Reference
This is a console application that has a single static library dependency.

[Source](https://github.com/soup-build/soup/tree/main/samples/cpp/static-library)

## [library/recipe.sml](https://github.com/soup-build/soup/tree/main/samples/cpp/static-library/library/recipe.sml)
The Recipe file that defines the static library "Samples.Cpp.StaticLibrary.Library".
```sml
Name: 'Samples.Cpp.StaticLibrary.Library'
Language: 'C++|0'
Version: 1.0.0
Type: 'StaticLibrary'
```

## [library/library.cpp](https://github.com/soup-build/soup/tree/main/samples/cpp/static-library/library/library.cpp)
A module interface file that exports a single sample class.
```cpp
module;

// Include all standard library headers in the global module
#include <string>

export module Samples.Cpp.StaticLibrary.Library;

// Note: The namespace does not have to match the module name
export namespace Samples::Cpp::StaticLibrary::Library
{
  class Helper
  {
  public:
    static std::string GetName()
    {
      return "Soup";
    }
  };
}
```

## [application/recipe.sml](https://github.com/soup-build/soup/tree/main/samples/cpp/static-library/application/recipe.sml)
The Recipe file that defines the executable "Samples.Cpp.StaticLibrary.Application".
```sml
Name: 'Samples.Cpp.StaticLibrary.Application'
Language: 'C++|0'
Type: 'Executable'
Version: 1.0.0
Dependencies: {
  Runtime: [
    '../library/'
  ]
}
```

## [application/package-lock.sml](https://github.com/soup-build/soup/tree/main/samples/cpp/static-library/application/package-lock.sml)
The package lock that was generated to capture the unique dependencies required to build this project and the dependency static library.
```sml
Version: 5
Closures: {
  Root: {
    'C++': {
      'Samples.Cpp.StaticLibrary.Application': { Version: './', Build: 'Build0', Tool: 'Tool0' }
      'Samples.Cpp.StaticLibrary.Library': { Version: '../library/', Build: 'Build0', Tool: 'Tool0' }
    }
  }
  Build0: {
    Wren: {
      'Soup|Cpp': { Version: 0.16.1 }
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

## [application/main.cpp](https://github.com/soup-build/soup/tree/main/samples/cpp/static-library/application/main.cpp)
A simple main method that prints our "Hello World, Soup Style!" by using the module from the library.
```cpp
#include <iostream>

import Samples.Cpp.StaticLibrary.Library;
using namespace Samples::Cpp::StaticLibrary::Library;

int main()
{
  std::cout << "Hello World, " << Helper::GetName() << " Style!" << std::endl;
  return 0;
}
```

## [.gitignore](https://github.com/soup-build/soup/tree/main/samples/cpp/static-library/.gitignore)
A simple git ignore file to exclude all Soup build output.
```
out/
```