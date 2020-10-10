package main

import (
	"fmt"
	"io"
	"log"
	"net"
	"os"
	"strings"
)

func handleConn(place string, connection net.Conn) {
	for {
		data := make([]byte, 11)
		_, err := connection.Read(data)
		if err != nil {
			connection.Close()
			fmt.Printf("%s", err)
			return

		} else {
			fmt.Printf("%s: %s", place, data)
		}
	}
}

func mustCopy(dst io.Writer, src io.Reader) {
	if _, err := io.Copy(dst, src); err != nil {
		log.Fatal(err)
	}

}

func main() {
	var listaNombres = [3]string{}
	var listaPuertos = [3]string{}
	args := os.Args[1:]
	sizeArgs := len(args)
	fmt.Println(args, ", with lenght: ", sizeArgs)
	for i := 0; i < sizeArgs; i++ {
		fmt.Println()
		s := strings.Split(args[i], "=")
		listaPuertos[i] = s[1]
		listaNombres[i] = s[0]
		conn, err := net.Dial("tcp", s[1])
		if len(s) != 2 || err != nil {
			fmt.Printf("Failed to connect\n")
			return
		}
		go handleConn(listaNombres[i], conn)
	}

	for {
	}

}
