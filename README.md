# Chatrooms

Chatroom programs for UNIX based systems, written in C. 

## 2-Way Chatroom

This program allows two users to communicate over a network.
* One program functions as the server which will wait for a connection.
* The other program functions as the client and it will connect to the server.
* After establishing the connection, both the server and the client behave in the same way.

## Multi-threaded Chatroom (1,000+ Concurrent Clients)

This program allows for many users to communicate over a network instantaneously.
* One program functions as the server which will handle message passing between clients. 
  - The server does not participate direcly in the communication process.
* All other programs function as clients and will connect to the server.
* Anything that a client says will be passed to all other clients by the server.
