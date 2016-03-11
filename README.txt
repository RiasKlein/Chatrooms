/******************************************************************************

	Name: 		Shunman Tse
	NID:		N13382418
	Net ID:		st1637
	Course:		UNIX Programming
	Homework:	2-Way Chat

	Version:	1.0

******************************************************************************/

Focus:
1.	Networking
2. 	Multiplexing

This program allows two users to communicate over a network.
One program functions as the server which will wait for a connection.
The other program functions as the client and it will connect to the server.

After establishing a connection, both server and client will function the same.

Regarding Connection Termination:
If the connection with the server is terminated, the client will simply exit; 
	it does not attempt to reconnect with the server. 
If the server's connection with the client is terminated, the server will await
	a connection request from the next client.

Usage for server:
	./server <user_name> <optional port num>
	The default port number is 5555.
e.g.	./server Rias
	The above would start a server with the username: Rias and port: 5555

Usage for client:
	./client <user name> <Server IP> <Server Port>
e.g. 	./client Jack 127.0.0.1 5555
	The above will start a client with the username: Jack, port: 5555, 
	and IP address: 127.0.0.1 

Note:
1.	The client will exit if no server is found with the provided IP / port
2. 	The client and server both require sharedFunc.h / sharedFunc.c

