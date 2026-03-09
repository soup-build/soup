# C Build Extension
This is a console application that has a custom build extension that alters the build state. The custom build Tasks will run before and after the core Build Task and will simply print a nice hello message.

[Source](https://github.com/soup-build/soup/tree/main/samples/c/build-extension)

## [extension/recipe.sml](https://github.com/soup-build/soup/blob/main/samples/c/build-extension/extension/recipe.sml)
The Recipe file that defines the build extension dynamic library "Samples.C.BuildExtension.Extension" that will register new build tasks.
```sml
Name: 'Samples.C.BuildExtension.Extension'
Language: 'Wren|0'
Version: 1.0.0
Dependencies: {
  Runtime: [
    'Soup|Build.Utils@0'
  ]
}
```

## [extension/package-lock.sml](https://github.com/soup-build/soup/blob/main/samples/c/build-extension/extension/package-lock.sml)
The package lock that was generated to capture the unique dependencies required to build this project.

## [extension/custom-build-task.wren](https://github.com/soup-build/soup/blob/main/samples/c/build-extension/extension/custom-build-task.wren)
A Wren file defining a custom build Task that will run before the build definition and sets a custom preprocessor definition to show how a user can alter the build state through an extension.
```wren
// <copyright file="custom-build-task.wren" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

import "soup" for Soup, SoupTask
import "Soup|Build.Utils:./list-extensions" for ListExtensions
import "Soup|Build.Utils:./map-extensions" for MapExtensions

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
  }
}
```

## [application/recipe.sml](https://github.com/soup-build/soup/blob/main/samples/c/build-extension/application/recipe.sml)
The Recipe file that defines the executable "Samples.C.BuildExtension.Application". The one interesting part is the relative path reference to the custom build extension through "Build" Dependencies.
```sml
Name: 'Samples.C.SimpleBuildExtension.Application'
Language: 'C|0'
Type: 'Executable'
Version: 1.0.0
Dependencies: {
  Build: [
    '../extension/'
  ]
}
```

## [application/package-lock.sml](https://github.com/soup-build/soup/blob/main/samples/c/build-extension/application/package-lock.sml)
The package lock that was generated to capture the unique build dependencies required to build this project.

## [application/main.c](https://github.com/soup-build/soup/blob/main/samples/c/build-extension/application/main.c)
A simple main method that prints our "Hello World, Soup Style!" only if the build extension was able to set the custom preprocessor definition "SPECIAL_BUILD".
```c
#include <stdio.h>

int main()
{
#ifdef SPECIAL_BUILD
  printf("Hello World, Soup Style!");
#else
  printf("Hello World...");
#endif
  return 0;
}
```

## [.gitignore](https://github.com/soup-build/soup/blob/main/samples/c/build-extension/.gitignore)
A simple git ignore file to exclude all Soup build output.