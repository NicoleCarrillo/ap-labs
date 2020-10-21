package main

import (
	"io"
	"log"
	"net"
	"os"
	"flag"
	"fmt"
)

func main() {
	if len(os.Args) != 5 {
		log.Fatal("Missing arguments     go run client.go -user user1 -server localhost:9000  ")
	} else {
		userNameConsole := flag.String("user","","username")			//* flag ("what you are looking","inicialization of variable","description")
		server := flag.String("server", "", "server:port")
		flag.Parse()
		conn, err := net.Dial("tcp", *server)
		if err != nil {
			log.Fatal(err)
		}
		fmt.Fprintf(conn, *userNameConsole)
		done := make(chan struct{})
		go func() {
			io.Copy(os.Stdout, conn) //* NOTE: ignoring errors
			log.Println("End of session")
			done <- struct{}{} //^ signal the main goroutine
		}()
		fmt.Println("irc-server > Welcome to the simple IRC Server ")
		mustCopy(conn, os.Stdin)
		conn.Close()
		<-done //& wait for background goroutine to finish
	}
}

func mustCopy(dst io.Writer, src io.Reader) {
	if _, err := io.Copy(dst, src); err != nil {
		log.Fatal(err)
	}
}