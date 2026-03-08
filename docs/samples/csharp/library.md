# C# Library
This is a console application and library with the minimal amount of code to get up and running in C#.

[Source](https://github.com/soup-build/soup/tree/main/samples/csharp/library)

## [library/recipe.sml](https://github.com/soup-build/soup/blob/main/samples/csharp/library/library/recipe.sml)
The Recipe file that defines the static library "Samples.CSharp.Library.Library".
```sml
Name: 'Samples.CSharp.Library.Library'
Language: 'C#|0'
TargetFramework: 'net10.0'
Version: 1.0.0
```

## [library/helper.cs](https://github.com/soup-build/soup/blob/main/samples/csharp/library/library/helper.cs)
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

## [application/recipe.sml](https://github.com/soup-build/soup/blob/main/samples/csharp/library/application/recipe.sml)
The Recipe file that defines the static library "Samples.CSharp.Library.ConsoleApplication".
```sml
Name: 'Samples.CSharp.Library.ConsoleApplication'
Language: 'C#|0'
TargetFramework: 'net10.0'
Type: 'Executable'
Version: 1.0.0
Dependencies: {
  Runtime: [
    '../library/'
  ]
}
```

## [application/package-lock.sml](https://github.com/soup-build/soup/blob/main/samples/csharp/library/application/package-lock.sml)
The package lock that was generated to capture the unique build dependencies required to build this project.

## [program.cs](https://github.com/soup-build/soup/blob/main/samples/csharp/library/application/program.cs)
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

## [.gitignore](https://github.com/soup-build/soup/blob/main/samples/csharp/library/.gitignore)
A simple git ignore file to exclude all Soup build output.