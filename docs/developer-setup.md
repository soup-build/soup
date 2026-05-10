# Developer Setup

## Requirements

### Windows
* Windows 11
* Build Tools
  * [Visual Studio 2022](https://visualstudio.microsoft.com/downloads/) with "Desktop development with c++" workload.
  * OR
  * [Build Tools For Visual Studio 2022](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022) with "c++ build tools" workload.
* [Latest Release](https://github.com/soup-build/soup/releases)

### Linux
* Verified on Ubuntu 22.04.1 LTS WSL
* Install GCC 12
  ```
  sudo apt install g++-12
  ```
* Install .Net SDK 6.0
  ```
  sudo apt install dotnet-sdk-6.0
  ```
* [Latest Release](https://github.com/soup-build/soup/releases)

## Setup
Clone the repository and all submodules recursively.

```
git clone --recursive https://github.com/soup-build/soup.git
```

## Build

### Build the Client
```
cd code/client/cli
soup restore
soup build
```

## LSP Generation

Soup does not yet integrate directly into clangd language service so we need to generate the compile commands before loading the service so it knowns how to compile all of our source files.

Install [Bear](https://github.com/rizsotto/Bear)
```
bear -- soup build code/client/cli/ -force
```
this will force rebuild the entire project so bear can track commands to compile each source file.

