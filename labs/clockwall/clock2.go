//* Clock2 is a concurrent TCP server that periodically writes the time.
// TODO        Code from -> Example 2 - Concurrent Clock Server (1/4) talks obedmr gorutines and channels intro
//~ SERVER

package main

import (
	"os"
	"io"
	"log"
	"net"
	"time"
)

func handleConn(c net.Conn, local string, local2 *time.Location) { //& function from class
	defer c.Close()
	for {
		
		formatTime := local2.String() + " : " + time.Now().In(local2).Format("15:04:05\n")
		_, err := io.WriteString(c, formatTime)
		if err != nil {
			return //* e.g., client disconnected
		}
		time.Sleep(1 * time.Second)
	}
}

func main() {
	if len(os.Args) < 3 { //!	error handle
		log.Fatal("MISSING ARGUMENTS -> TZ=_______    go run clock2.go -port ____")
	}else{
		
		port:= os.Args[2]
		timeLocal := os.Getenv("TZ")
		
		local, err := time.LoadLocation(timeLocal)
		host := "localhost:"+ port
		listener, err2 := net.Listen("tcp", host) 
		
		if err != nil || err2 != nil { 		//!	error handle
			log.Fatal(err)
			os.Exit(1)
		}

		for {
			conn, err := listener.Accept()
			if err != nil {
				log.Print(err) //* e.g., connection aborted
				continue
			}
		go handleConn(conn, timeLocal, local) //~ handle connections concurrently
		}
	}
}