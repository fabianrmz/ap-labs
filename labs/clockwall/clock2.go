// Clock2 is a concurrent TCP server that periodically writes the time.
package main

import (
	"fmt"
	"io"
	"log"
	"net"
	"os"
	"time"
)

func handleConn(c net.Conn) {
	defer c.Close()
	for {
		_, err := io.WriteString(c, time.Now().Format("15:04:05\n"))
		if err != nil {
			return // e.g., client disconnected
		}
		time.Sleep(1 * time.Second)
	}
}

func main() {
	args := os.Args[1:]
	sizeArgs := len(args)
	if sizeArgs != 2 {
		fmt.Println("Missing parameters")
		os.Exit(3)
	} else if args[0] != "-port" {
		fmt.Println("Missing command format -port")
		fmt.Println("Example: $ TZ=US/Eastern go run clock2.go -port 8080  (timezone optional)")
		os.Exit(3)
	}
	port := args[1]
	completePort := "localhost:" + port

	listener, err := net.Listen("tcp", completePort)
	fmt.Println("Initialized server clock on: ", completePort)
	if err != nil {
		log.Fatal(err)
	}
	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Print(err) // e.g., connection aborted
			continue
		}
		go handleConn(conn) // handle connections concurrently
	}
}
