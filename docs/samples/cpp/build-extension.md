# C++ Build Extension
This is a console application that has a custom build extension that alters the build state. The custom build Tasks will run before and after the core Build Task and will simply print a nice hello message.

[Source](https://github.com/soup-build/soup/tree/main/samples/cpp/build-extension)

## Tool/recipe.sml
The Recipe file that defines the executable "Samples.Cpp.SimpleBuildExtension.Tool" that will be run as part of the build.
```sml
Name: 'Samples.SimpleBuildExtension.Tool'
Language: 'C++|0'
Type: 'Executable'
Version: 1.0.0
```

## Tool/package-lock.sml
The package lock that was generated to capture the unique build dependencies required to build this project.
```sml
Version: 5
Closures: {
  Root: {
    'C++': {
      'Samples.SimpleBuildExtension.Tool': { Version: './', Build: 'Build0', Tool: 'Tool0' }
    }
  }
  Build0: {
    Wren: {
      'Soup|Cpp': { Version: 'C:/Users/mwasp/dev/repos/soup-cpp/code/extension/' }
    }
  }
  Tool0: {
    'C++': {
      'mwasplund|copy': { Version: 'C:/Users/mwasp/dev/repos/soup/code/tools/copy/' }
      'mwasplund|mkdir': { Version: 'C:/Users/mwasp/dev/repos/soup/code/tools/mkdir/' }
      'mwasplund|parse.modules': { Version: 'C:/Users/mwasp/dev/repos/soup/code/tools/parse-modules/' }
    }
  }
}
```

## Tool/Main.cpp
A simple main method that prints our "Tool, Soup Style!" during the build.
```cpp
#include <iostream>

int main()
{
  std::cout << "Tool, Soup Style!" << std::endl;
  return 0;
}
```

## Extension/recipe.sml
The Recipe file that defines the build extension dynamic library "Samples.Cpp.BuildExtension.Extension" that will register new build tasks.
```sml
Name: 'Samples.Cpp.BuildExtension.Extension'
Language: 'Wren|0'
Version: 1.0.0
Source: [
  'CustomBuildTask.wren'
]
Dependencies: {
  Runtime: [
    'Soup|Build.Utils@0'
  ]
  Tool: [
    '../tool/'
  ]
}
```

## Extension/package-lock.sml
The package lock that was generated to capture the unique dependencies required to build this project.
```sml
Version: 5
Closures: {
  Root: {
    Wren: {
      'Samples.Cpp.BuildExtension.Extension': { Version: './', Build: 'Build0', Tool: 'Tool0' }
      'Soup|Build.Utils': { Version: 0.7.0, Build: 'Build0', Tool: 'Tool0' }
    }
  }
  Build0: {
    Wren: {
      'Soup|Wren': { Version: 0.4.3 }
    }
  }
  Tool0: {
    'C++': {
      'mwasplund|copy': { Version: 'C:/Users/mwasp/dev/repos/soup/code/tools/copy/' }
      'mwasplund|mkdir': { Version: 'C:/Users/mwasp/dev/repos/soup/code/tools/mkdir/' }
    }
  }
}
```

## Extension/CustomBuildTask.wren
A Wren file defining a custom build Task that will run before the build definition and sets a custom preprocessor definition to show how a user can alter the build state through an extension.
```wren
// <copyright file="CustomBuildTask.wren" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

import "soup" for Soup, SoupTask
import "Soup|Build.Utils:./Path" for Path
import "Soup|Build.Utils:./ListExtensions" for ListExtensions
import "Soup|Build.Utils:./MapExtensions" for MapExtensions
import "Soup|Build.Utils:./SharedOperations" for SharedOperations

class CustomBuildTask is SoupTask {
  /// <summary>
  /// Get the run before list
  /// </summary>
  static runBefore { [
    "BuildTask"
  ] }

  /// <summary>
  /// Get the run after list
  /// </summary>
  static runAfter { [] }

  /// <summary>
  /// Core Evaluate
  /// </summary>
  static evaluate() {
    Soup.info("Running Before Build!")

    // Get the build table
    var buildTable = MapExtensions.EnsureTable(Soup.activeState, "Build")

    // As a sample of what a build extension can do we set a new
    // preprocessor definition to influence the build
    var preprocessorDefinitions = [
      "SPECIAL_BUILD",
    ]

    ListExtensions.Append(
      MapExtensions.EnsureList(buildTable, "PreprocessorDefinitions"),
      preprocessorDefinitions)

    var contextTable = Soup.globalState["Context"]
    var packageRoot = Path.new(contextTable["PackageDirectory"])

    CustomBuildTask.CreateCustomToolOperation(packageRoot)
  }

  /// <summary>
  /// Create a build operation that will create a directory
  /// </summary>
  static CreateCustomToolOperation(workingDirectory) {
    // Discover the dependency tool
    var toolExecutable = SharedOperations.ResolveRuntimeDependencyRunExectable("Samples.SimpleBuildExtension.Tool")

    var title = "Run Custom Tool"

    var program = Path.new(toolExecutable)
    var inputFiles = []
    var outputFiles = []

    // Build the arguments
    var arguments = []

    Soup.createOperation(
      title,
      program.toString,
      arguments,
      workingDirectory.toString,
      ListExtensions.ConvertFromPathList(inputFiles),
      ListExtensions.ConvertFromPathList(outputFiles))
  }
}
```

## Executable/recipe.sml
The Recipe file that defines the executable "Samples.Cpp.BuildExtension.Executable". The one interesting part is the relative path reference to the custom build extension through "Build" Dependencies.
```sml
Name: 'Samples.SimpleBuildExtension.Executable'
Language: 'C++|0'
Type: 'Executable'
Version: 1.0.0
Dependencies: {
  Build: [
    '../extension/'
  ]
}
```

## Executable/package-lock.sml
The package lock that was generated to capture the unique build dependencies required to build this project.
```sml
Version: 5
Closures: {
  Root: {
    'C++': {
      'Samples.SimpleBuildExtension.Executable': { Version: './', Build: 'Build0', Tool: 'Tool0' }
    }
  }
  Build0: {
    Wren: {
      'Samples.Cpp.BuildExtension.Extension': { Version: '../extension/' }
      'Soup|Cpp': { Version: 'C:/Users/mwasp/dev/repos/soup-cpp/code/extension/' }
    }
  }
  Tool0: {
    'C++': {
      'Samples.SimpleBuildExtension.Tool': { Version: '../tool/' }
      'mwasplund|copy': { Version: 'C:/Users/mwasp/dev/repos/soup/code/tools/copy/' }
      'mwasplund|mkdir': { Version: 'C:/Users/mwasp/dev/repos/soup/code/tools/mkdir/' }
      'mwasplund|parse.modules': { Version: 'C:/Users/mwasp/dev/repos/soup/code/tools/parse-modules/' }
    }
  }
}
```

## Executable/Main.cpp
A simple main method that prints our "Hello World, Soup Style!" only if the build extension was able to set the custom preprocessor definition "SPECIAL_BUILD".
```cpp
#include <iostream>

int main()
{
#ifdef SPECIAL_BUILD
  std::cout << "Hello World, Soup Style!" << std::endl;
#else
  std::cout << "Hello World..." << std::endl;
#endif

  return 0;
}
```

## .gitignore
A simple git ignore file to exclude all Soup build output.
```
out/
```
