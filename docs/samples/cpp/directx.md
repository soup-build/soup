# C++ DirectX
This is a desktop application that demonstrates creating a complex graphics windows application.

[Source](https://github.com/soup-build/soup/tree/main/samples/cpp/directx)

## recipe.sml
The Recipe file that defines the directX sample application. Note the special module partition hierarchy that explicitly sets the references for partition units. The application also copies over the shader files that will be compiled at runtime.
```sml
Name: 'Samples.Cpp.DirectX'
Language: 'C++|0'
Type: 'Windows'
Version: 1.0.0
PlatformLibraries: [
  'D3D12.lib'
  'D3DCompiler.lib'
  'DXGI.lib'
]
RuntimeDependencies: [
  'Shaders.hlsl'
]
```

## package-lock.sml
The package lock that was generated to capture the unique dependencies required to build this project.
```sml
Version: 5
Closures: {
  Root: {
    'C++': {
      'Samples.Cpp.DirectX': { Version: './', Build: 'Build0', Tool: 'Tool0' }
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

## Content
The remaining content assumes that you are familiar with creating a graphics application and windows. This is not meant as a graphics tutorial and is only present as a sample of a more complex scenario for building within Soup.