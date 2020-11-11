package main

import (
	"fmt"
	"os"
	"time"
)

func connection(pingIn chan int, pongOut chan int) {
	for {
		pongOut <- (1 + <-pingIn)
	}
}

func main() {

	if len(os.Args) > 1 {
		fmt.Println("WRONG USAGE : ./ping-pong ")

	} else {
		total := 0

		pingIn := make(chan int)
		pongOut := make(chan int)

		go connection(pingIn, pongOut)
		go connection(pongOut, pingIn)

		for i := 0; i < 1; i++ {
			pingIn <- 0
			time.Sleep(time.Duration(1) * time.Second)
			ball := <-pingIn
			total += ball
		}

		fmt.Println("Communications Per Second : ", total)
	}
}
