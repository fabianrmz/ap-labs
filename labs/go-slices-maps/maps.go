package main

import (
	"golang.org/x/tour/wc"
	"strings"
)

func WordCount(s string) map[string]int {
	counter := make(map[string]int)
	words := strings.Fields(s)
    for _ ,word:= range words{
        counter[word]++
    }
    return counter
}

func main() {
	wc.Test(WordCount)
}
