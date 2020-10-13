//TODO Code from -> Example E - UNBUFFERED CHANNELS talks obedmr gorutines and channels intro
//~ CLIENTE

package main

import (
	"os"
	"io"
	"log"
	"net"
	"strings"
)

func mustCopy(dst io.Writer, src io.Reader) {		//& code from class
    if _, err := io.Copy(dst, src); err != nil {
        log.Fatal(err)
    }
}

func main() {
	if len(os.Args) < 2 {
		log.Fatal("MISSING ARGUMENTS -> go run clockWall.go NewYork=localhost:8010 ...Tokyo=localhost:8020 London=localhost:8030")
		os.Exit(1)
	}
		channel := make(chan int) 
    	for _, arg := range os.Args[1:] { //* Iteration FOR EACH argument in COMMAND LINE
			port := arg[strings.LastIndex(arg, "=")+1:]  //* Getting the argument and port 
			conn, err := net.Dial("tcp", port) //~ connection
			if err != nil {	 //! error		
				log.Fatal(err)
			}
			defer conn.Close()
			go mustCopy(os.Stdout, conn)
		}
		x:=1
		x=<-channel		//^ HEY!! -> wait for background goroutine to finish
		log.Println("channel closed with value: ", x)
		close(channel)	
}