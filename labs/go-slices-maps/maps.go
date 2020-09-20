package main

import (
	"strings"
	"golang.org/x/tour/wc"
)
func WordCount(s string) map[string]int {
	w:=strings.Fields(s)
	m:=make(map[string]int)
	for i := range w {
        m[w[i]]++
    }
	return m
}

func main() {
	wc.Test(WordCount)
}