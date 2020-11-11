// max 2054419 pipes
package main

import (
	"fmt"
	"os"
	"runtime"
	"time"
)

var maxStages int

// "https://golangcode.com/print-the-current-memory-usage/"
func PrintMemUsage() {
	var m runtime.MemStats
	runtime.ReadMemStats(&m)
	fmt.Printf("Sys = %v MEMORY USE \n", bToMb(m.Sys))
}
func PrintMemUsageFinal() {
	var m runtime.MemStats
	runtime.ReadMemStats(&m)
	if m.TotalAlloc/1024/1024 > 1024 {
		fmt.Printf("NO MORE MEMORY \n")
		fmt.Printf("USED = %v \n", bToMb(m.Sys))
		fmt.Println("Maximum number of pipeline stages   : ", maxStages)
		defer os.Exit(1)
	}
}

func bToMb(b uint64) uint64 {
	return b / 1024 / 1024
}

func test(startPipe chan int, cont int) {
	last := startPipe
	x := 0
	for x < cont {
		aux := last
		go func() {
			for data := range aux {
				last <- data
			}
		}()
		x++
		maxStages++
		PrintMemUsageFinal()
	}
}

func main() {
	startPipe := make(chan int)

	var cont = 1000000

	fmt.Printf("MEMORY USED BEFORE PIPE CREATION ")
	PrintMemUsage()

	startingTime := time.Now()
	test(startPipe, cont)
	endTime := time.Now()

	transitTime := endTime.Sub(startingTime)

	fmt.Println("Maximum number of pipeline stages   : ", maxStages)
	fmt.Println("Time to transit trough the pipeline : ", transitTime)
	fmt.Printf("MEMORY USED AFTER PIPE CREATION ")
	defer PrintMemUsage()
}
