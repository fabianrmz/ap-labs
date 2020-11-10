package main

import (
	"fmt"
	"time"
)

func main() {
	var maxStages int
	maxStages = 100000
	start := time.Now()
	in, out := pipeline(maxStages)

	for i := 0; i < 200; i++ {
		in <- 1
		<-out
	}
	close(in)

	fmt.Println("Maximum number of pipeline stages   : ", maxStages)
	fmt.Println("Time to transit trough the pipeline : ", float64(10000)/float64(time.Since(start)), "seconds")

}

func pipeline(stages int) (in chan int, out chan int) {
	out = make(chan int)
	first := out
	for i := 0; i < stages; i++ {
		in = out
		out = make(chan int)
		go func(in chan int, out chan int) {
			for v := range in {
				out <- v
			}
			close(out)
		}(in, out)
	}
	return first, out
}
