// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 156.

// Package geometry defines simple types for plane geometry.
//!+point
package main

	
import (
	"fmt"
	"math"
	"math/rand"
	"os"
	"strconv"
	"time"
)

type Point struct{ x, y float64 }

// traditional function
func Distance(p, q Point) float64 {
	return math.Hypot(q.x-p.x, q.y-p.y)
}

// same thing, but as a method of the Point type
func (p Point) Distance(q Point) float64 {
	return math.Hypot(q.x-p.x, q.y-p.y)
}


//!-point
func (p Point) Y() float64{
	return p.y
}

func (p Point) X() float64{
	return p.x
}

//!+path

// A Path is a journey connecting the points with straight lines.
type Path []Point

// Distance returns the distance traveled along the path.
func (path Path) Distance() float64 {
	sum := 0.0
	for i := range path {
		if i > 0 {
			sum += path[i-1].Distance(path[i])
		}
	}
	return sum
}
func getRandomNum() float64{
	rand.Seed(time.Now().UnixNano())
    return math.Round(((rand.Float64()*200)-100)*100) / 100
}

func onSegment(p, q, r Point) bool {
	if (q.X() <= math.Max(p.X(), r.X())) && (q.X() >= math.Min(p.X(), r.X())) && (q.Y() <= math.Max(q.Y(), r.Y())) && (q.Y() >= math.Min(p.Y(), r.Y())) {
		return true
	}
	return false
}
func orientation(p, q, r Point) int {
	val := (q.Y()-p.Y())*(r.X()-q.X()) - (q.X()-p.X())*(r.Y()-q.Y())
	if val == 0 {
		return 0
	}
	if val > 0 {
		return 1
	}
	return 2
}
func doIntersect(p1, q1, p2, q2 Point) bool{
	o1 := orientation(p1, q1, p2)
	o2 := orientation(p1, q1, q2)
	o3 := orientation(p2, q2, p1)
	o4 := orientation(p2, q2, q1)
	if (o1 != o2) && (o3 != o4) {
		return true
	}
	if (o1 == 0) && onSegment(p1, p2, q1) {
		return true
	}
	if (o2 == 0) && onSegment(p1, q2, q1) {
		return true
	}
	if (o3 == 0) && onSegment(p2, p1, q2) {
		return true
	}
	if (o4 == 0) && onSegment(p2, q1, q2) {
		return true
	}
	return false
}
func (p Path) intersectionExist() bool{
	intersection := false
	for i := 0;i<(len(p)-3);i++ {
		for j:=0;j<(i+1);j++ {
			intersection = doIntersect(p[j], p[j+1], p[len(p)-2], p[len(p)-1])
		}
	}
	return intersection
}

func getPerimeter(lines int, path Path){
	fmt.Println("- Figure's Perimeter")
	var totalPerimeter float64
	fmt.Printf("  -")
	for i:=0;i<lines;i++{
		totalPerimeter+=  path[(i%lines)].Distance( path[((i+1)%lines)])
		fmt.Printf(" %.1f ", path[(i%lines)].Distance( path[((i+1)%lines)]) )
		if (lines != (i-1)) {
			fmt.Printf("+")
		}

		
	}
	fmt.Printf(" = %.1f \n", totalPerimeter)

}

func main() {
	linesNumber, _ := strconv.Atoi(os.Args[1])
	if(linesNumber<3){
		fmt.Println("Error, figure has to be more than 2 sides")
		return
	}
	fmt.Printf("- Generating a [%d] sides figure\n", linesNumber)
	fmt.Println("- Figure's vertices")
	figure := Path{}
	
	for i := 0; i < linesNumber; i++ {
		figure = append(figure, Point{getRandomNum(),getRandomNum()})

		for figure.intersectionExist(){
			figure[i]=Point{getRandomNum(), getRandomNum()}
		}
		fmt.Printf("  - (  %.1f,   %.1f)\n",figure[i].X(), figure[i].Y())
	}

	getPerimeter(linesNumber, figure)
	
		
	
}

//!-path