# C Windows Application
This is a windows application that demonstrates creating a GUI windows application.

[Source](https://github.com/soup-build/soup/tree/main/samples/c/windows-application)

## Library/recipe.sml
The Recipe file that defines the sample application.
```sml
Name: 'Samples.C.WindowsApplication'
Language: 'C|0'
Type: 'Windows'
Version: 1.0.0
Resources: 'windows-project.rc'
```

## Library/package-lock.sml
The package lock that was generated to capture the unique dependencies required to build this project.
```sml
Version: 5
Closures: {
  Root: {
    C: {
      'Samples.C.WindowsApplication': { Version: './', Build: 'Build0', Tool: 'Tool0' }
    }
  }
  Build0: {
    Wren: {
      'Soup|C': { Version: 0.4.1 }
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

## Content
The remaining content assumes that you are familiar with creating a windows applications. This is not meant as a windows tutorial and is only present as a sample of a more complex scenario for building within Soup.