package main

import (
	"math"
	"fmt"
	"math/rand"
	"time"
	"os"
	"strconv"
)

// POINT 
type Point struct{ x, y float64 }

// GETTER THE X
func (p Point) X() float64 {
	return p.x
}

// GETTER THE Y
func (p Point) Y() float64 {
	return p.y
}

// HYPO OF 2 POINTS  -> Distance traditional function
func Distance(p, q Point) float64 {
	return math.Hypot(q.X()-p.X(), q.Y()-p.Y())
}

// SAME BUT DIFFERENT BUT STILL THE SAME -> Distance same thing, but as a method of the Point type
func (p Point)Distance(q Point) float64 {
	return math.Round(math.Hypot(q.X()-p.X(), q.Y()-p.Y())*100) / 100
}

// PATH->STRAIGHT LINE CONNECTING THE POINTS -> path de puntos 
type Path []Point

// DISTANCE OF PATH
func (path Path)Distance() float64 {
	sum := 0.0
	for i := range path {
		if i > 0 {
			sum += path[i-1].Distance(path[i])
		}
	}
	return sum
}

// FUNCTION GIVEN 3 COLINEAR POINT P,Q,R THE FUNCTION CHECKS IF POINT Q LIES ON THE SEGMENT PR
func onSegment(p, q, r Point) bool{
	if (q.X() <= math.Max(p.X(), r.X())) && (q.X() >= math.Min(p.X(), r.X())) && (q.Y() <= math.Max(p.Y(), r.Y())) && (q.Y() >= math.Min(p.Y(), r.Y())){
		return true
	}
	return false
}

 
// FUNCTION TO FIND ORIENTATION OF ORDERED TRIPLET (P,Q R)
func orientation(p, q, r Point)  float64{
	val:= (q.Y() - p.Y()) * (r.X() - q.X()) - (q.X() - p.X()) * (r.Y() - q.Y())
	if val==0{
		return 0  //ARE COLINEAR
	}else{
		if val>0 {
			return 1 //CLOCKWISE
		}else{
			return 2 //COUNTERCLOCKWISE
		}
	}
}
 
// THE MAIN FUNCTION THAT RETURNS TRUE IF LINE SEGMENT P1Q1 AND P2Q2 INTERSECT
func doIntersect(p1,q1,p2,q2 Point) bool {
	o1 := orientation(p1, q1, p2); 
    o2 := orientation(p1, q1, q2); 
    o3 := orientation(p2, q2, p1); 
	o4 := orientation(p2, q2, q1); 
	
	if (o1 != o2) && (o3 != o4){
		return true
	} else if (o1 == 0) && (onSegment(p1, p2, q1)){
		return true
	}else if (o2 == 0) && (onSegment(p1, q2, q1)){
		return true
	}else if (o3 == 0) && (onSegment(p2, p1, q2)){
		return true
	}else if (o4 == 0) && (onSegment(p2, q1, q2)){
		return true
	}
	return false
}

// FUNCTION TO CREATE RANDOM NUMBER 
func randomPoint()float64 {
	seed := rand.NewSource(time.Now().UnixNano())
    random := rand.New(seed)
	min:=-100.0
	max:=100.0
	return (random.Float64()*max-min) + min
}

// FUNCTION TO CHECK IF AN INTERSECTION OCCURS 
func (figure Path)check()bool  {
	var intersection bool
	for x := 0; x < len(figure)-3; x++ {
			intersection = doIntersect(figure[x],figure[x+1],figure[x+2],figure[x+3])
			if intersection==true{
				break;
		}
	}
	return intersection
}


func main() {
	figure := Path{}
	var perimeter float64
	if len(os.Args) <= 1 {
		fmt.Println("MISSING ARGUMENTS")
		return
	}
	sides,error:= strconv.Atoi(os.Args[1])
	if error != nil {
		fmt.Println(error)
	}
	if sides < 3 {
		fmt.Println("YOU NEED AT LEAST 3 POINTS")
		return
	}
	fmt.Printf("- Generating a [%d] sides figure\n", sides)
	for x := 0; x < sides; x++ {
		figure = append(figure, Point{randomPoint(), randomPoint()})
		for figure.check(){ //while
			figure[x] = Point{randomPoint(), randomPoint()}	
		}
	}
	fmt.Printf("- Figure's vertices\n")
	for x := range figure {
		fmt.Printf("\t- ( %.2f, %.2f)\n", figure[x].X(), figure[x].Y())
	}
	fmt.Println("- Figure's Perimeter")
	fmt.Printf("- ")
	for x := 0; x < sides; x++ {
		perimeter=perimeter + figure[x].Distance(figure[(x+1)%sides])
		if x ==0 {
			fmt.Printf("%.2f  ", figure[x].Distance(figure[(x+1)%sides]))
		}else{
			fmt.Printf("+ ")
			fmt.Printf("%.2f  ", figure[x].Distance(figure[(x+1)%sides]))
		}
	}
	fmt.Printf("= ")
	fmt.Printf("%.2f \n",perimeter)
}