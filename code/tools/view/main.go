package main

// #cgo LDFLAGS: -L/home/mwasplund/repos/soup/out/cpp/local/soup-native/1.0.0/J_HqSstV55vlb-x6RWC_hLRFRDU/bin/ -l:soup-native.so -Wl,-rpath,/home/mwasplund/repos/soup/out/cpp/local/soup-native/1.0.0/J_HqSstV55vlb-x6RWC_hLRFRDU/bin/
// #include <stdlib.h>
// #include "hello.h"
import "C"

import (
    "encoding/json"
    "fmt"
    "unsafe"
)

func main(){
    // let's call it
    workingDirectory := C.CString("/home/mwasplund/repos/soup/code/tools/view/")
    defer C.free(unsafe.Pointer(workingDirectory))

    buildGraphValue := C.LoadBuildGraphSimple(workingDirectory)
    defer C.free(unsafe.Pointer(buildGraphValue))

    buildGraph := C.GoString(buildGraphValue)
    // fmt.Println(buildGraph)

    res := LoadBuildGraphResult{}
    json.Unmarshal([]byte(buildGraph), &res)
    if res.IsSuccess {
        fmt.Println(res.Graph)
    } else {
        fmt.Println(res.Message)
    }
}