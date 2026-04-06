package main

// #cgo LDFLAGS: -L/home/mwasplund/repos/soup/out/cpp/local/soup-view/1.0.0/J_HqSstV55vlb-x6RWC_hLRFRDU/bin/ -l:soup-view.so
// #include "hello.h"
import "C"

func main(){
    // let's call it
    C.hello();
}