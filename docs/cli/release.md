# Release
## Overview
Release a recipe by building the target package and publishing the artifact.
```
soup release <path> [-flavor <name>]
```

`path` - An optional parameter that directly follows the build command. If present this specifies the directory to look for a Recipe file to build. If not present then the command will use the current active directory.

`-flavor <name>` - An optional parameter to specify the build flavor. Common values include `Debug` or `Release`.

## Examples
Release a Recipe in the current directory for release.
```
soup release
```

Release a Recipe in a different directory.
```
soup release C:\Code\MyProject\
```
