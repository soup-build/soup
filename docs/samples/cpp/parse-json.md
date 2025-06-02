#  Parse Json File
A console application that reads in a json file using the an external module and prints a single known property value.

[Source](https://github.com/soup-build/soup/tree/main/samples/cpp/parse-json)

## Recipe.sml
The Recipe file that sets the standard name, type, version, as well as the single external dependency of the [json11](https://github.com/dropbox/json11) project.
```sml
Name: 'Samples.Cpp.ParseJson'
Language: (C++@0)
Version: 1.0.0
Type: 'Executable'
Dependencies: {
  Runtime: [ 'mwasplund|json11@1' ]
}
```

## PackageLock.sml
The package lock that was generated to capture the unique dependencies required to build this project.
```
Version: 5
Closures: {
  Root: {
    'C++': {
      'mwasplund|json11': { Version: 1.1.3, Build: 'Build0', Tool: 'Tool0' }
      'Samples.Cpp.ParseJsonFile': { Version: '../ParseJsonFile', Build: 'Build0', Tool: 'Tool0' }
    }
  }
  Build0: {
    Wren: {
      'Soup|Cpp': { Version: 0.15.2 }
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

## Message.json
A json file containing a single property containing a message for the application to print.
```json
{
  "message": "Hello!"
}
```

## Main.cpp
A simple main method that reads the contents of a single json file, parses the json content and prints a single message from a known property.
```cpp
#include <fstream>
#include <iostream>
#include <streambuf>
#include <string>

import json11;

int main()
{
  // Read in the contents of the json file
  auto jsonFile = std::ifstream("./Message.json");
  auto jsonContent = std::string(
    std::istreambuf_iterator<char>(jsonFile),
    std::istreambuf_iterator<char>());

  // Parse the json
  std::string errorMessage;
  auto json = json11::Json::parse(jsonContent, errorMessage);

  // Print the single property value
  std::cout << "Message: " << json["message"].string_value() << std::endl;

  return 0;
}
```

## .gitignore
A simple git ignore file to exclude all Soup build output.
```
out/
```