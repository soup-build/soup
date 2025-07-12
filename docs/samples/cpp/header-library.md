# C++ Header Library
This is a console application that has a single header library dependency.

[Source](https://github.com/soup-build/soup/tree/main/samples/cpp/header-library)

## [library/recipe.sml](https://github.com/soup-build/soup/tree/main/samples/cpp/header-library/library/recipe.sml)
The Recipe file that defines the header library "Samples.Cpp.HeaderLibrary.Library".
```sml
Name: 'Samples.Cpp.HeaderLibrary.Library'
Language: 'C++|0'
Version: 1.0.0
PublicHeaders: [
  {
    Root: './'
    Files: [
      'Library.h'
    ]
  }
]
```

## [library/library.h](https://github.com/soup-build/soup/tree/main/samples/cpp/header-library/library/library.h)
The header file.
```cpp
#include <string>

namespace Samples::Cpp::HeaderLibrary::Library
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

## [application/recipe.sml](https://github.com/soup-build/soup/tree/main/samples/cpp/header-library/application/recipe.sml)
The Recipe file that defines the executable "Samples.Cpp.HeaderLibrary.Application".
```sml
Name: 'Samples.Cpp.HeaderLibrary.Application'
Language: 'C++|0'
Type: 'Executable'
Version: 1.0.0
Dependencies: {
  Runtime: [
    '../library/'
  ]
}
```

## [application/package-lock.sml](https://github.com/soup-build/soup/tree/main/samples/cpp/header-library/application/package-lock.sml)
The package lock that was generated to capture the unique dependencies required to build this project and the dynamic library dependency.
```sml
Version: 5
Closures: {
  Root: {
    'C++': {
      'Samples.Cpp.HeaderLibrary.Application': { Version: './', Build: 'Build0', Tool: 'Tool0' }
      'Samples.Cpp.HeaderLibrary.Library': { Version: '../library/', Build: 'Build0', Tool: 'Tool0' }
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

## [application/main.cpp](https://github.com/soup-build/soup/tree/main/samples/cpp/header-library/application/main.cpp)
A simple main method that prints our "Hello World, Soup Style!" by using the header from the library.
```cpp
#include <iostream>
#include <library.h>

using namespace Samples::Cpp::HeaderLibrary::Library;

int main()
{
  std::cout << "Hello World, " << Helper::GetName() << " Style!" << std::endl;
  return 0;
}
```

## [.gitignore](https://github.com/soup-build/soup/tree/main/samples/cpp/header-library/.gitignore)
A simple git ignore file to exclude all Soup build output.
```
out/
```