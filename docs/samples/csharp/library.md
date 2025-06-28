# C# Console Application
This is a console application with the minimal amount of code to get up and running in C#.

[Source](https://github.com/soup-build/soup/tree/main/samples/csharp/console-application)

## library/recipe.sml
The Recipe file that defines the static library "Samples.CSharp.Library.Library".
```sml
Name: 'Samples.CSharp.Library.Library'
Language: 'C#|0'
TargetFramework: 'net9.0'
Version: 1.0.0
```

## library/Helper.cs
A C# file that implements the helper function.
```C#
namespace Samples.CSharp.Library
{
  public class Helper
  {
    public static string GetName()
    {
      return "Soup";
    }
  };
}
```

## application/recipe.sml
The Recipe file that defines the static library "Samples.CSharp.Library.ConsoleApplication".
```sml
Name: 'Samples.CSharp.Library.ConsoleApplication'
Language: 'C#|0'
TargetFramework: 'net9.0'
Type: 'Executable'
Version: 1.0.0
Dependencies: {
  Runtime: [
    '../library/'
  ]
}
```

## application/package-lock.sml
The package lock that was generated to capture the unique build dependencies required to build this project.
```sml
Version: 5
Closures: {
  Root: {
    'C#': {
      'Samples.CSharp.Library.ConsoleApplication': { Version: './', Build: 'Build0', Tool: 'Tool0' }
      'Samples.CSharp.Library.Library': { Version: '../library/', Build: 'Build0', Tool: 'Tool0' }
    }
  }
  Build0: {
    Wren: {
      'Soup|CSharp': { Version: 0.15.3 }
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

## Program.cs
A C# Program file that implements the main method.
```C#
using System;

namespace Samples.CSharp.Library.Application
{
  public class Program
  {
    public static void Main(string[] args)
    {
      Console.WriteLine($"Hello World, {Helper.GetName()} Style!");
    }
  }
}
```

## .gitignore
A simple git ignore file to exclude all Soup build output.
```
out/
```