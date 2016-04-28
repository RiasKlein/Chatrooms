/******************************************************************************

	Name: 		Shunman Tse
	NID:		N13382418
	Net ID:		st1637
	Course:		UNIX Programming
	Homework:	Chat Room

	Version:	1.0

******************************************************************************/

Focus:
1.	Concurrency

This project contains a chatroom in which a server will host the room and 
allow for logging of client interactions. The client program will connect
to the server and be able to communicate with other clients.

This is basically an instant messaging system that supports up to 1023 users.

Usage for Server & Client
1. Server
	./server <optional port num>
2. Client
	./client <user name> <Server IP> <Server Port>
3. Client Users can exit by using the command: /exit

Procedure: Establishing Client Connection
When a new client connects to the server:
1. Server creates a new thread to handle the new client
2. Server thread receives the username of the client's user 
3. Server thread inserts client username into a list of clients using the method:
	list[clientFD - 3] = username
4. Server thread informs client regarding currently online users
5. Server thread notifies online users that the client has entered the chat room
6. Server thread now waits for input from client

Procedure: Terminating Client Connection
When a client's connection to the server is terminated:
1. Set the client's username in the client list to be NULL, that is:
	list[clientFD - 3] = NULL
2. Close the file descriptor for the connection on the server end
3. Notify all online users that the client has left the chat room

Client: Modifications since the server-client 2-Way Chat
1. Client has been modified to send the username to server at the start

Limitations on Chatroom
1. Maximum message size (including the username in it) is: 2000000
2. Maximum number of users is 1024

Notes:
1. Regarding the thread that handles the message queue / writing to clients
	a. The lock acquisition order is: 1. qlock, 2. llock
		to lock the message queue and client list respectively
	b. It blocks on condition variable qcond if there are no messages to process
	c. It is awakened when a new message has been added to the message queue
2. Regarding the data structures:
	a. To keep track of online clients a char** array is used
		The connection FD (clientFD) is used as the index into the array
		The stored data is the client username
		If the stored data is NULL, then there is no connection at that FD
	b. The message queue is implemented using a linked list 
		Nodes of this linked list contain the username, message, 
			and ptr to the next node
		The username is used to make sure that the message is not
			sent back to the original user that sent the message
3. Tar Compressed Archive:
tar -zcvf st1637_HW8.tar.gz <all source file names>
4. Tar Compressed Extract:
tar -zxvf st1637_HW8.tar.gz