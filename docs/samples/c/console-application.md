#  C Console Application
This is the smallest amount of code to get a console application building using Soup.

[Source](https://github.com/soup-build/soup/tree/main/samples/c/console-application)

## Recipe.sml
The Recipe file that sets the name, type, version and the single source file.
```sml
Name: 'Samples.C.ConsoleApplication'
Language: (C@0)
Type: 'Executable'
Version: 1.0.0
Source: [
  'Main.c'
]
```

## PackageLock.sml
The package lock that was generated to capture the unique dependencies required to build this project.
```sml
Version: 5
Closures: {
  Root: {
    C: {
      'Samples.C.ConsoleApplication': { Version: '../ConsoleApplication', Build: 'Build0', Tool: 'Tool0' }
    }
  }
  Build0: {
    Wren: {
      'Soup|C': { Version: 0.4.0 }
    }
  }
  Tool0: {
    'C++': {
      'mwasplund|copy': { Version: 1.1.0 }
      'mwasplund|mkdir': { Version: 1.1.0 }
    }
  }
}
```

## Main.c
A simple main method that prints our "Hello World, Soup Style!" and returns a success status.
```c
#include <stdio.h>

int main()
{
  printf("Hello World, Soup Style!");
  return 0;
}
```

## .gitignore
A simple git ignore file to exclude all Soup build output.
```
out/
```