#  C Console Application
This is the smallest amount of code to get a console application building using Soup.

[Source](https://github.com/soup-build/soup/tree/main/samples/c/console-application)

## [recipe.sml](https://github.com/soup-build/soup/tree/main/samples/c/console-application/recipe.sml)
The Recipe file that sets the name, type, version and the single source file.
```sml
Name: 'Samples.C.ConsoleApplication'
Language: 'C|0'
Type: 'Executable'
Version: 1.0.0
```

## [package-lock.sml](https://github.com/soup-build/soup/tree/main/samples/c/console-application/package-lock.sml)
The package lock that was generated to capture the unique dependencies required to build this project.
```sml
Version: 5
Closures: {
  Root: {
    C: {
      'Samples.C.ConsoleApplication': { Version: './', Build: 'Build0', Tool: 'Tool0' }
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

## [main.c](https://github.com/soup-build/soup/tree/main/samples/c/console-application/main.c)
A simple main method that prints our "Hello World, Soup Style!" and returns a success status.
```c
#include <stdio.h>

int main()
{
  printf("Hello World, Soup Style!");
  return 0;
}
```

## [.gitignore](https://github.com/soup-build/soup/tree/main/samples/c/console-application/.gitignore)
A simple git ignore file to exclude all Soup build output.
```
out/
```