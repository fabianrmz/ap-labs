// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 254.
//!+

// Chat is a server that lets clients chat with each other.
package main

import (
	"bufio"
	"fmt"
	"log"
	"net"
	"os"
	"strconv"
	"strings"
	"time"
)

type user struct {
	userName  string
	ip        string
	admin     bool
	ch        chan string
	notKicked bool
}

//!+broadcaster
type client chan<- string // an outgoing message channel

var ()

var (
	entering       = make(chan client)
	leaving        = make(chan client)
	messages       = make(chan string) // all incoming client messages
	serverInitLine = "irc-server ➜ "
	usersList      = make([]*user, 0, 100)
)

func broadcaster() {
	clients := make(map[client]bool) // all connected clients
	for {

		select {
		case msg := <-messages:
			// Broadcast incoming message to all
			// clients' outgoing message channels.
			for cli := range clients {
				cli <- msg
			}

		case cli := <-entering:
			clients[cli] = true

		case cli := <-leaving:
			delete(clients, cli)
			removeUser(cli)
			close(cli)
		}
	}

}

func leaveALider(usr *user) *user {
	if len(usersList) > 1 {
		if usersList[0] != usr {
			return usersList[0]
		}
		return usersList[1]
	}
	return nil
}

//!-broadcaster

func removeUser(cli client) {
	for i, user := range usersList {
		if user.ch == cli {
			usersList = append(usersList[:i], usersList[i+1:]...)
		}
	}
}

func addNewUser(username, ip string) *user {
	return &user{userName: username, ip: ip}
}

func userNameFromConn(conn net.Conn) string {
	// get username
	buf := make([]byte, 512)
	n, _ := conn.Read(buf)
	return string(buf[:n])
}
func getUsers() string {
	switch len(usersList) {
	case 0:
		return ""
	case 1:
		return "Only user: " + usersList[0].userName

	}

	sep := ", "
	n := len(sep) * (len(usersList) - 1)
	for i := 0; i < len(usersList); i++ {
		n += len(usersList[i].userName)
	}

	var b strings.Builder
	b.Grow(n)
	b.WriteString(usersList[0].userName)
	for _, s := range usersList[1:] {
		b.WriteString(sep)
		b.WriteString(s.userName)
	}
	return serverInitLine + b.String()
}

func getTime() string {
	location, err := time.LoadLocation("America/Mazatlan")
	if err != nil {
		return serverInitLine + "Failed loading location. Retry."
	}
	return serverInitLine + "Local Time: " + location.String() + " " + time.Now().Format("15:04:05")
}

func getUser(userName string) *user {

	for _, user := range usersList {
		if user.userName == userName {
			return user
		}
	}
	return nil
}

func makeMessage(dst *user, who string, message string) string {
	dst.ch <- who + " (DIRECT MESSAGE) ( ಠ ͜ʖಠ)➜ " + message
	return who + " sent to: " + dst.userName + " ➜ " + message
}

func getUserInfo(user *user) string {
	return serverInitLine + "User: " + user.userName + ", IP: " + user.ip + " Admin: " + strconv.FormatBool(user.admin)
}

func kickOutUser(user, me *user) string {
	if user == me {
		return serverInitLine + "you cannot kick yourself dumbass ( ͡ಥ ͜ʖ ͡ಥ)"
	}
	user.notKicked = false
	user.ch <- serverInitLine + "you have been kicked out of the chat by admin"
	leaving <- user.ch
	messages <- serverInitLine + me.userName + " has kicked " + user.userName + " out"
	return serverInitLine + "successfuly kicked out"
}

//!+handleConn
func handleConn(conn net.Conn) {

	ch := make(chan string) // outgoing client messages
	go clientWriter(conn, ch)
	username := userNameFromConn(conn)

	who := username
	ch <- serverInitLine + "Welcome to Simple IRC Server"
	ch <- serverInitLine + "You are successfuly logged with username: " + who
	messages <- serverInitLine + who + " has arrived to the chat"
	entering <- ch

	userObject := addNewUser(username, conn.RemoteAddr().String())
	userObject.ch = ch
	userObject.notKicked = true
	usersList = append(usersList, userObject)

	fmt.Println(serverInitLine + "New user has jump into the server: " + who + " ᕦ( ͡° ͜ʖ ͡°)ᕤ")
	if len(usersList) == 1 {
		ch <- serverInitLine + "Since you are the first user you are now ADMIN ( ͡~ ͜ʖ ͡°)"
		fmt.Println(serverInitLine + who + " is ADMIN (▀̿Ĺ̯▀̿ ̿)")
		userObject.admin = true
	}

	input := bufio.NewScanner(conn)
	for input.Scan() {

		switch tokens := strings.Split(input.Text(), " "); tokens[0] {
		case "/users":
			// get all users from user list
			fmt.Println(serverInitLine + "User list requested by: " + who)
			if len(tokens) == 1 {

				ch <- who + " ➜  Listing  users: ( ͡°( ͡° ͜ʖ( ͡° ͜ʖ ͡°)ʖ ͡°) ͡°)"
				ch <- getUsers()
			} else {
				ch <- serverInitLine + "Error, too much parameters ( ͠° ͟ʖ ͡°)"
				ch <- serverInitLine + "usage: ➜ /user"
			}

		case "/msg":
			fmt.Println(serverInitLine + "Message service requested by " + who)
			if len(tokens) < 3 {
				ch <- serverInitLine + "Error, complete the message format ( ͠° ͟ʖ ͡°)"
				ch <- serverInitLine + "usage: ➜ /msg <user> <your-message>"
			} else {
				userToSend := getUser(tokens[1])
				if userToSend != nil {
					ch <- makeMessage(userToSend, who, strings.Join(tokens[2:], " "))
				} else {
					ch <- serverInitLine + "Error, user not found ( ͠° ͟ʖ ͡°)"
				}
			}
		case "/time":
			fmt.Println(serverInitLine + "Time service requested by " + who)
			ch <- who + " ➜  Time request: "
			ch <- who + " ➜ " + getTime()
		case "/user":
			fmt.Println(serverInitLine + "User info requested by " + who)
			if len(tokens) != 2 {
				ch <- serverInitLine + "Error, complete the message format ( ͠° ͟ʖ ͡°)"
				ch <- serverInitLine + "usage: ➜ /user <user>"
			} else {
				userToSend := getUser(tokens[1])
				if userToSend != nil {
					ch <- getUserInfo(userToSend)
				} else {
					ch <- serverInitLine + "Failed, user does not exist (╯ ͠° ͟ʖ ͡°)╯┻━┻"
				}
			}
		case "/kick":
			if userObject.admin {
				if len(tokens) != 2 {
					ch <- serverInitLine + "Error, complete the kick format ( ͠° ͟ʖ ͡°)"
					ch <- serverInitLine + "usage: ➜ /user <user>"
				} else {
					fmt.Println(serverInitLine + " " + who + " requested to kick " + tokens[1] + " out (ง ͠° ͟ل͜ ͡°)ง")
					userToSend := getUser(tokens[1])
					if userToSend != nil {
						ch <- kickOutUser(userToSend, userObject)
					} else {
						ch <- serverInitLine + "Failed, user does not exist (╯ ͠° ͟ʖ ͡°)╯┻━┻"
					}
				}
			} else {
				ch <- serverInitLine + "You are not allowed to kick users out ( ͡° ʖ̯ ͡°)"
			}
		default:
			if userObject.notKicked {
				messages <- who + " ➜ " + input.Text()
			} else {
				conn.Close()
			}
		}

	}
	// NOTE: ignoring potential errors from input.Err()
	if userObject.admin {
		if newAdmin := leaveALider(userObject); newAdmin != nil {
			newAdmin.admin = true
			messages <- serverInitLine + newAdmin.userName + " is now ADMIN"
		}
	}
	if userObject.notKicked {
		leaving <- ch
		messages <- serverInitLine + who + " has left"
	}
	conn.Close()
}

func clientWriter(conn net.Conn, ch <-chan string) {
	for msg := range ch {
		fmt.Fprintln(conn, msg) // NOTE: ignoring network errors
	}
}

//!-handleConn

//!+main
func main() {
	argsWithoutProg := os.Args[1:]

	if len(argsWithoutProg) != 4 {
		fmt.Println("Failed, misisng parameters")
		fmt.Println("Usage: ➜ go run server.go -host <localhost> -port <9000> ")
		return
	} else if argsWithoutProg[0] != "-host" || argsWithoutProg[2] != "-port" {
		fmt.Println("Failed, error in format parameters")
		fmt.Println("Usage: ➜ go run server.go -host <localhost> -port <9000> ")
		return
	}
	hostNamePort := argsWithoutProg[1] + ":" + argsWithoutProg[3]

	fmt.Println("irc-server ➜ Simple IRC Server started at ", hostNamePort)

	listener, err := net.Listen("tcp", hostNamePort)
	if err != nil {
		log.Fatal(err)
	}

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

//!-main
