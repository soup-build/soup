package main

// #cgo LDFLAGS: -L/home/mwasplund/repos/soup/out/cpp/local/soup-native/1.0.0/Oltq7cGwk0Rbgy1I-3mCMDDE5yM/bin/ -l:soup-native.so -Wl,-rpath,/home/mwasplund/repos/soup/out/cpp/local/soup-native/1.0.0/Oltq7cGwk0Rbgy1I-3mCMDDE5yM/bin/
// #include <stdlib.h>
// #include "hello.h"
import "C"

import "fmt"
import "unsafe"

func main(){
    // let's call it
    workingDirectory := C.CString("/home/mwasplund/repos/soup/code/tools/view/")
    defer C.free(unsafe.Pointer(workingDirectory))

    buildGraphValue := C.LoadBuildGraphSimple(workingDirectory)
    defer C.free(unsafe.Pointer(buildGraphValue))

    buildGraph := C.GoString(buildGraphValue)
    fmt.Println(buildGraph)
}