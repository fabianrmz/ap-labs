package main

import "golang.org/x/tour/pic"

func Pic(dx, dy int) [][]uint8 {
    var total[][]uint8
    total = make([][]uint8, dy)
    for k := range total {
        total[k] = make([]uint8, dx)
        for i := range total[k] {
            total[k][i] = uint8((i + k) /2)
        }
    }
    return total
}

func main() {
	pic.Show(Pic)
}