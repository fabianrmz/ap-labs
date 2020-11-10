package main

import (
	"fmt"
	"os"
	"os/signal"
	"time"
)

func main() {

	q := make(chan int)
	var i int64
	start := time.Now()
	go func() {
		q <- 1
		for {
			i++
			q <- <-q

		}
	}()
	go func() {
		for {
			q <- <-q
		}
	}()
	fmt.Println("ctrl + c for cut goroutine ->")
	c := make(chan os.Signal, 1)
	signal.Notify(c, os.Interrupt)
	<-c

	fmt.Println("Communications Per Second : ", float64(i)/float64(time.Since(start)), "seconds")

}
