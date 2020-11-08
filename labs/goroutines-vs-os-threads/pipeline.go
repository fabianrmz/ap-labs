package main

import (
	"fmt"
)

func main() {
	var maxStages, transitTime int

	fmt.Println("Maximum number of pipeline stages   : ", maxStages)
	fmt.Println("Time to transit trough the pipeline : ", transitTime)

	var x, y int
	go func() {
		x = 1
		fmt.Print("y:", y, " ")
	}()
	go func() {
		y = 1
		fmt.Print("x:", x, " ")
	}()
}
