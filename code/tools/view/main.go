package main

// #cgo LDFLAGS: -L/home/mwasplund/repos/soup/out/cpp/local/soup-native/1.0.0/J_HqSstV55vlb-x6RWC_hLRFRDU/bin/ -l:soup-native.so -Wl,-rpath,/home/mwasplund/repos/soup/out/cpp/local/soup-native/1.0.0/J_HqSstV55vlb-x6RWC_hLRFRDU/bin/
// #include <stdlib.h>
// #include "hello.h"
import "C"

import (
    "encoding/json"
    "fmt"
    "unsafe"
    "os"
    tea "charm.land/bubbletea/v2"
)


func main(){
    // let's call it
    workingDirectory := C.CString("/home/mwasplund/repos/soup/code/tools/view/")
    defer C.free(unsafe.Pointer(workingDirectory))

    buildGraphValue := C.LoadBuildGraphSimple(workingDirectory)
    defer C.free(unsafe.Pointer(buildGraphValue))

    buildGraph := C.GoString(buildGraphValue)

    res := LoadBuildGraphResult{}
    json.Unmarshal([]byte(buildGraph), &res)
    if !res.IsSuccess {
        fmt.Println(res.Message)
        os.Exit(2)
    }

    p := tea.NewProgram(initialModel())
    if _, err := p.Run(); err != nil {
        fmt.Printf("Alas, there's been an error: %v", err)
        os.Exit(1)
    }
}