//server 
package main

import (
	"bufio"
	"fmt"
	"strings"
	"log"
	"net"
	"os"
	"time"
	"flag"
)
//& --------------------------------------DECLARACION DE VARIABLES--------------------------------------------
type client chan<- string 					// an outgoing message channel
type user struct {							//information of user
	name string
	ip string
	channel chan string
	timeSince string
	con net.Conn
}
var (
	entering 	  = make(chan client)					// all clients entering channel
	leaving  	  = make(chan client)					// all client leaving channel
	messages 	  = make(chan string) 					// all incoming client messages
	clients 	  = make(map[client]bool)			// map declared for storing which clients are connected
	clientList    = make(map[string]bool)			// map declared for review if a user is connected or not			
	badWordsArray = [5]string{"&@*^","***","###","tontos","tonto"}	//bad words types array
	admin 		= ""			// variable for administration control
	contador =0
)
var informationUsers [20]user	//saving all users			
//& --------------------------------------------------------------------------------------------------------
//^ --------------------------FUNCION QUE TRANSMITE LOS MENSAJES A TODOS------------------------------------
func broadcaster() {	// Broadcast incoming message to all
	for {
		select {
		case msg := <-messages:
			for cli := range clients {
				cli <- msg
			}
		case cli := <-entering:
			clients[cli] = true
		case cli := <-leaving:
			delete(clients, cli)
			close(cli)
		}
	}
}
//^ --------------------------------------------------------------------------------------------------------
//TODO -------------------------------FUNCION DONDE SUCEDE TODO---------------------------------------------
func handleConn(conn net.Conn) {
	
	ch := make(chan string) // outgoing client messages
	go clientWriter(conn, ch)
	
	userNameConsole := make([]byte, 128)
	conn.Read(userNameConsole);
	username2 := ""
	for i := 0; i < len(userNameConsole); i++ {
		if userNameConsole[i] != 0 {
			username2 = username2 + string(userNameConsole[i])
		} else {
			break
		}
	}

	who := user{
		name: username2,
		ip: conn.RemoteAddr().String(),
		channel: ch,
		timeSince: time.Now().Format("15:04"),
		con: conn,
	}
	if val, ok := clientList[who.name]; ok && val {
		ch <- "irc-server > User already connected"
		ch <- ""
		conn.Close()
	} else {
		informationUsers[contador]=who
		clientList[who.name] = true
		ch <- "irc-server > Your user [ " + who.name + " ] is succesfully logged"
		messages <- "irc-server > New connected user [ " + who.name + " ]"
		fmt.Printf("irc-server > New connected user [" + who.name + " ]\n")
		
		if(admin==""){
			admin=who.name;
			fmt.Println("irc-server > "+"[ " + who.name + " ] " + "was promoted to channel ADMIN")
			ch <- "irc-server > Congrats, you are the first user"
			ch <- "irc-server > You are the new IRC Server ADMIN"
		}

		contador++;
		entering <- ch

		input := bufio.NewScanner(conn)
		for input.Scan() {
			line:=input.Text()
			command:=strings.Split(line," ")
			if(line==""){
				who.channel<-"irc-server > Please introduce someting"
			}else if(command[0]=="/users"){
				getTotalUsers(who.channel)
			}else if(command[0]=="/msg"){
				if(len(command)>1){
					command2 := strings.SplitN(line," ",3)
					sendMessage(who.channel,command2[1],command2[2],who.name)
				}else{
					who.channel<-"irc-server > Please introduce what user you wish to send a message"
				}
			}else if(command[0]=="/time"){ 
				getTime(ch)
			}else if(command[0]=="/user"){
				if(len(command)>1){
					getInfoUser(who.channel,command[1])
				}else{
					who.channel<-"irc-server > Please introduce what user you wish to check"
				}
			}else if(command[0]=="/kick"){
				if(len(command)>1){
					if(who.name==admin){
						kickUser(who.con,command[1],false)
					}else{
						who.channel<-"irc-server > You don't have permission to kick someone"
					}
				}else{
					who.channel<-"rc-server > Please introduce what user you wish to kick"
				}
			}else{
				messages <- who.name + " > " + input.Text()
				check:=checkBadWorgs(input.Text())
				if(check==true){
					kickUser(who.con,who.name,true)
				}
			}
		}
	}
	leaving <- ch
	messages <- who.name + " has left"
	clientList[who.name] = false
	if admin == who.name && contador>1{
		for i := range informationUsers {
			if informationUsers[i].name == admin {
				informationUsers[i].name=""
				admin = informationUsers[i+1].name
				informationUsers[i+1].channel <- "irc-server > You're the new IRC Server Admin"
				fmt.Printf("irc-server > "+admin+"was promoted as the channel ADMIN");
			}
		}
	}
	conn.Close()
}
//TODO ------------------------------------------------------------------------------------------------------
//~ ---------------------FUNCION QUE REGRESA LA LISTA DE TODOS LOS USUARIOS CONECTADOS-----------------------
func getTotalUsers(channel chan string){
	for i:=0;i<contador;i++{
		if(informationUsers[i].name!=""){
			channel <- "irc-server > username: " + informationUsers[i].name + " Logging time: " + informationUsers[i].timeSince
		}
	}
}
//~ ---------------------FUNCION QUE REGRESA LA HORA Y EL LUGAR ACTUAL DEL USUARIO---------------------------
func getTime(channel chan string){
	zone, _ := time.Now().Zone()
	channel <- "irc.server > Local Time: " + zone + " " + time.Now().Format("15:04")
}
//~ ---------------------FUNCION QUE REGRESA INFO DEL USUARIO SOLICITADO-------------------------------------
func getInfoUser(channel chan string, userRequested string){
	var found bool
	found = false
	for i := range informationUsers {
		if informationUsers[i].name == userRequested {
			request := informationUsers[i]
			channel <- "irc-server > username: "+request.name+" , IP: "+request.ip+" Connected since: " + request.timeSince
			found= true
			break
		}
	}
	if(found==false){
		channel <- "irc-server > username: "+userRequested+" not found"
	}
}
//~ ----------------------------FUNCION QUE MANDA UN MENSAJE------------------------------------------------
func sendMessage(channel chan string, userRequested string, dm string, from string){
	var found bool
	found = false
	for i := range informationUsers {
		if informationUsers[i].name == userRequested {
			request := informationUsers[i]
			request.channel <- "irc-server >  Direct message from [ "+ from + "] message: " + dm
			channel <- "irc-server > Message send to "+userRequested
			found= true
			break
		}
	}
	if(found==false){
		channel <- "irc-server > username: "+userRequested+" is not connected"
	}
}
//~ ----------------------------FUNCION QUE SACA PERSONAS----------------------------------------------------
func kickUser(connection net.Conn,userKick string, badWord bool)  {
	var found bool
	var request2 user
	found = false
	for i := range informationUsers {
		if informationUsers[i].name == userKick {
			request2 = informationUsers[i]
			informationUsers[i].name=""
			found= true
			request2.channel <- "irc-server < you have been kicked out of the channel"
			contador--;
			if(userKick==admin){
				admin = informationUsers[i+1].name
				informationUsers[i+1].channel <- "irc-server > You're the new IRC Server Admin"
				fmt.Printf("irc-server > "+admin+"was promoted as the channel ADMIN\n");
			}
			break
		}
	}
	if(found==true){
		if(badWord==true){
			messages <- "irc-server > [" + userKick + "] was kicked from this channel because BAD WORDS POLICY"
			request2.channel <- "irc-server < you have been kicked out because of BAD WORDS"
			fmt.Printf("irc-server > [" + userKick + "] was kicked from this channel because BAD WORDS POLICY\n")
			request2.con.Close()
		}else if(badWord==false){
			messages <- "irc-server > [" + userKick + "] was kicked from this channel"
			request2.channel <- "irc-server < you have been kicked out of the channel"
			fmt.Printf("irc-server > [" + userKick + "] was kicked from this channel \n")
			request2.con.Close()
		}
	}else{
		fmt.Printf("irc-server > [" + userKick + "] not found\n")
	}
	
}
//~ ---------------------------FUNCION QUE CHECA MALAS PALABRAS----------------------------------------------
func checkBadWorgs(dm string) bool{
	var check bool
	check=false
	for i:=range badWordsArray{
		if(dm==badWordsArray[i]){
			check=true;
		}
	}
	return check
}
//~ ---------------------------------------------------------------------------------------------------------
//* ----------------------------FUNCION QUE IMPRIME MENSAJES-------------------------------------------------
func clientWriter(conn net.Conn, ch <-chan string) {
	for msg := range ch {
		fmt.Fprintln(conn, msg) 
	}
}
//* ----------------------------------------------------------------------------------------------------------
//? -----------------------------------FUNCION MAIN-----------------------------------------------------------
func main() {
	if len(os.Args) != 5 {
		log.Fatal("Missing arguments      go run server.go -host localhost -port 9000")
	}else{
		host := flag.String("host","","name of host")
		port := flag.String("port","","port #")
		flag.Parse()
		listener, err := net.Listen("tcp", *host + ":" + *port)
		if err != nil {
			log.Fatal(err)
		}
		fmt.Println("irc-server > Simple IRC Server started at " + *host + ":" + *port)
		fmt.Println("irc-server > Ready for receiving new clients")
		go broadcaster()
		for {
			conn, err := listener.Accept()
			if err != nil {
				log.Print(err)
				continue
			}
			go handleConn(conn)
		}
	}
}
//? ---------------------------------------------------------------------------------------------------------