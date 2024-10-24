//introduz canais na aplicação Go
package main

import (
	"fmt"
)

func tarefa(channel chan string)  {
	var quotes [2] string
	quotes[0] = "Oi Main, bom dia, tudo bem?"
	quotes[1] = "Certo, entendido."
	var output = ""

	for i := 0; i < 2; i++ {
		output = <- channel	
		fmt.Println("Main fala para goroutine: ", output)
		channel <- quotes[i]
	}
}

func main() {
	
	var quotes [2] string
	
	quotes[0] = "Olá Goroutine, bom dia!"
	quotes[1] = "Tudo bem! Vou terminar tá?"
	
	var output = ""

	channel := make(chan string)
	
	go tarefa(channel)
	
	for i := 0; i < 2; i++ {
		channel <- quotes[i]
		output = <- channel
		fmt.Println("Goroutine fala para main: ", output)
	}
}
