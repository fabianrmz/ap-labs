// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 227.

// Netcat is a simple read/write client for TCP servers.
package main

import (
	"fmt"
	"io"
	"log"
	"net"
	"os"
)

//!+
func main() {

	argsWithoutProg := os.Args[1:]
	var username string
	var serverPort string
	if len(argsWithoutProg) != 4 {
		fmt.Println("Failed, misisng parameters")
		fmt.Println("Usage: ➜ go run client.go -user <user-name> -server <localhost:9000> ")
		os.Exit(1)
	} else if argsWithoutProg[0] == "-user" && argsWithoutProg[2] == "-server" {
		username = argsWithoutProg[1]
		serverPort = argsWithoutProg[3]
	} else if argsWithoutProg[2] == "-user" && argsWithoutProg[0] == "-server" {
		username = argsWithoutProg[3]
		serverPort = argsWithoutProg[1]
	} else {
		fmt.Println("Failed, error in format parameters")
		fmt.Println("Usage: ➜ go run client.go -user <user-name> -server <localhost:9000> ")
		os.Exit(1)
	}

	fmt.Println("--Jumping into the ", serverPort, "server with username: ", username)

	conn, err := net.Dial("tcp", serverPort)
	if err != nil {
		log.Fatal(err)
	}
	done := make(chan struct{})
	go func() {
		io.Copy(os.Stdout, conn) // NOTE: ignoring errors
		log.Println("done")
		done <- struct{}{} // signal the main goroutine
		return
	}()
	io.WriteString(conn, username)
	mustCopy(conn, os.Stdin)
	conn.Close()
	<-done // wait for background goroutine to finish
}

//!-

func mustCopy(dst io.Writer, src io.Reader) {
	if _, err := io.Copy(dst, src); err != nil {
		log.Fatal(err)
	}
}
