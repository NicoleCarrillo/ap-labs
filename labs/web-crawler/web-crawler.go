// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/
// See page 241.
// Crawl2 crawls web links starting with the command-line arguments.
// This version uses a buffered channel as a counting semaphore
// to limit the number of concurrent calls to links.Extract.
// Crawl3 adds support for depth limiting.

package main

import (
	"flag"
	"fmt"
	"os"

	"gopl.io/ch5/links"
)

type web struct {
	url   string
	level int
}

//!+sema   tokens is a counting semaphore used to  enforce a limit of 20 concurrent requests.
var tokens = make(chan struct{}, 20)
var con = 0

func crawl(txtfile *os.File, url web) []web {

	if url.level > 0 {
		_, errf := txtfile.WriteString(url.url + "\n") //? WRITE DATA INSIDE FILE
		if errf != nil {
			fmt.Println("ERROR IN CRAWL IN FILE")
		}
		con++
		tokens <- struct{}{} //* acquire a token
		list, err := links.Extract(url.url)
		linksWeb := make([]web, 0)
		for _, n := range list {
			linksWeb = append(linksWeb, web{url: n, level: url.level - 1})
		}
		<-tokens //* release the token

		if err != nil {
		}
		return linksWeb
	}
	linksWeb := make([]web, 0) //& ARRAY FOR EACH LINK SON SONS
	return linksWeb
}

//!-sema

func main() {
	worklist := make(chan []web)
	var n int              //* number of pending sends to worklist
	n++                    //* Start with the command-line arguments.
	data := make([]web, 0) //^ STORING WEB URLS ARRRAY SEARCH ONES

	seen := make(map[web]bool)
	if len(os.Args) < 3 {
		fmt.Printf("YOU ARE MISSING ARGUMENTS! -> ./web-crawler -depth=# -results=FILE.txt URL" + "\n")
	} else {
		depth := flag.Int("depth", 1, "links to crawl") //~GET DATA
		resultsFile := flag.String("results", "results.txt", "Results file to save links")
		flag.Parse() //~ACTIVATE THE FLAG

		txtfile, err := os.Create(*resultsFile) //? CREATE FILE TO WIRTE IN
		if err != nil {
			fmt.Println("ERROR WHILE CREATING THE FILE")
		}

		data = append(data, web{url: os.Args[3], level: *depth}) //^ STORE THE HEAD OF THE "TREE" URL IN MY ARRAY

		go func() { worklist <- data }() //* Crawl the web concurrently.

		for ; n > 0; n-- {
			list := <-worklist
			for _, link := range list {
				if !seen[link] {
					seen[link] = true
					n++

					go func(link web) {
						worklist <- crawl(txtfile, link)
					}(link)

				}
			}
		}
		defer txtfile.Close()
	}
}
