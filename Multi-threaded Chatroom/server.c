/******************************************************************************

	Name: 		Shunman Tse
	NID:		N13382418
	Net ID:		st1637
	Course:		UNIX Programming
	Homework:	Multi-threading

	server.c
		Usage: ./server <optional port num>

******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include "sharedFunc.h"
#include "server.h"

#define BACKLOG 	1024
#define MESS_MAX 	2000000

/* Globals */
char** client_list;									// Client list
MessageQueue queue = {NULL, NULL};					// Message Queue
pthread_mutex_t llock = PTHREAD_MUTEX_INITIALIZER;	// Client list lock	
pthread_mutex_t qlock = PTHREAD_MUTEX_INITIALIZER;	// Message queue lock
pthread_cond_t qcond = PTHREAD_COND_INITIALIZER;	// Queue condition variable

int main (int argc, char** argv){
	client_list = (char**) malloc (BACKLOG * sizeof (char*));	// Clients
	checknzero_client_list (client_list);	// Check and zero the client list

	int serverfd, clientfd;						// file descriptors
	int PORT_NUM = get_PORT_NUM (argc, argv);	// set port number
	serverfd = create_socket(); 				// Create socket

	bind_serverfd(serverfd, PORT_NUM);			// bind serverfd 

	// System will use this socket for listening 
	listen (serverfd, BACKLOG);

	// Create thread to handle the message queue
	gen_queue_thread();

	while (1) {
		// Wait for a connection 
		clientfd = accept(serverfd, NULL, NULL);

		// Create thread to handle new client connection
		gen_client_thread (clientfd); 
	}

	return 0;
}

/*	bind_serverfd
	Configure sockaddr_in struct and then bind to listening fd
*/
void bind_serverfd (int serverfd, int PORT_NUM) {
	struct sockaddr_in servaddr;	// struct for handling networking
	int bind_ret;					// return value of bind call

	// Configure servaddr
	memset (&servaddr, 0, sizeof (servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons (PORT_NUM);	
	servaddr.sin_addr.s_addr = htonl (INADDR_ANY);

	// Bind to listening file descriptor
	bind_ret = bind (serverfd, (struct sockaddr*) &servaddr, sizeof(servaddr));

	if (bind_ret == -1) { 
		fprintf  (stderr, "Error: bind call failed: Address already in use \n");
		print_help_server();
	}
}

/*	checknzero_client_list
	Verfies that the client list has been created properly
	Sets all entries to be NULL
*/
void checknzero_client_list (char** client_list) {
	// Check the queue
	if (!client_list) {
		perror ("Error: malloc of client_list failed\n");
		exit(1);
	}

	// "Zero" the queue
	for (int i = 0; i < BACKLOG; i++) {
		client_list[i] = NULL;
	}
}

/*	check_pthread_create
	Checks the return value of pthread_create call
	Exit if return value isn't 0
*/
void check_pthread_create (int create_ret) {
	if (create_ret != 0) {
		fprintf (stderr, "Error: pthread_create returned %i\n", create_ret);
		exit(1);
	}
}

/*	enqueue_message
	Adds a MessageNode to the back of the global MessageQueue
*/
void enqueue_message (MessageNode* node) {
	pthread_mutex_lock (&qlock);	// Acquire MessageQueue lock
	if (queue.front == NULL) {		// If there are no nodes in the queue
		node -> next = NULL;		// Then the current node will be both
		queue.front = node;			// the front and the back of the LL
		queue.back = node;			
	}
	else {
		queue.back -> next = node;	// If there are elements already
		queue.back = node;			// Add the new node to the back
	}
	pthread_mutex_unlock (&qlock);	// Release MessageQueue lock
}

/*	gen_client_thread
	Creates thread to service new client (also checks the return value)
	The FD for the connection is provided to the thread
*/
void gen_client_thread(int clientfd) {
	// Generate thread to service client
	pthread_t tid;
	int create_ret;
	create_ret = pthread_create(&tid, NULL, handle_client, (void*)&clientfd);
	check_pthread_create (create_ret);
}

/*	gen_MessageNode
	Allocates memory for a MessageNode to be added to the MessageQueue
	Populates the MessageNode with relevant username & message
	Wakes up the server thread that handles sending out the queue
*/
void gen_MessageNode (char* username, char* message) {
	MessageNode* node = (MessageNode*)malloc(sizeof(MessageNode));	
	node -> username = username;	// Populate the username field
	node -> message = message;		// Populate the message field
	enqueue_message (node);			// Add the message to MessageQueue
	int cond_ret = pthread_cond_signal (&qcond);	// Wake up the queue thread
	if (cond_ret != 0) {							// Check the return value
		perror ("Error: pthread_cond_signal Failure\n");
		exit(1);
	}
}

/*	gen_queue_thread
	Creates thread to handle the MessageQueue
	That is, send out the messages in the message queue
*/
void gen_queue_thread () {
	// Generate thread to handle the MessageQueue
	pthread_t tid;
	int create_ret;
	create_ret = pthread_create(&tid, NULL, handle_MessageQueue, NULL);
	check_pthread_create (create_ret);
}

/*	get_PORT_NUM
	Function acquires the port number of the server to connect to from argv
	Usage message is printed if necessary
*/
int get_PORT_NUM (int argc, char** argv) {
	int PORT_NUM;
	if (argc > 2) print_help_server();	// check number of args
	if (argc == 2) PORT_NUM = nstrtol(argv[1]);	// use optional port if given
	else PORT_NUM = 5555;						// else use default port

	printf ("Port_NUM: %d\n", PORT_NUM);

	return PORT_NUM;
}

/*	handle_client
	Utilized by each server thread servicing a client
	Follows the connection creation procedure specified in README
*/
void *handle_client (void* fd) {
	int* clientfd = fd;		// FD for this thread
	char* username = (char*) malloc (MESS_MAX * sizeof (char));
	int read_ret;

	// Get client username (data will sent from client)
	read_ret = read (*clientfd, username, MESS_MAX); // Read username to buffer
	if (read_ret == 0) {							 // Check return value
		fprintf (stderr, "Connection Terminated...\n");
		pthread_exit(NULL);
	}

	// Server log that a connection has been established
	fprintf (stderr, "Connection Established with user: '%s'\n", username);

	// Add client username to proper location on client_list 
	pthread_mutex_lock (&llock);				// Acquire lock on client list
	client_list [(*clientfd - 3)] = username;	// Update the client list
	print_client_list(*clientfd);				// Tell client who is online
	pthread_mutex_unlock (&llock);				// Release lock on client list

	// Notify all users that this client is online via a message
	login_message(username);	// Notify online clients that user is in room

	handle_messages(*clientfd, username);		// Handle user input

	// Cleanup
	free (username);
	pthread_exit(NULL);
}

/*	handle_MessageQueue
	Utilized by a server thread
	Dequeues from message queue and sends the message to online users
*/
void *handle_MessageQueue (void* ignored) {
	while (1) {
		// Lock MessageQueue before attempting access
		pthread_mutex_lock (&qlock);

		// Block on condition variable if there are no messages
		while (queue.front == NULL) {
			int ret_val = pthread_cond_wait (&qcond, &qlock);
			if (ret_val != 0) {
				perror ("Error: pthread_cond_wait call failure\n");
				exit(1);
			}
		}

		// We now have message(s) to send out
		pthread_mutex_lock (&llock);	// Lock client list to prevent changes
		while (queue.front != NULL) {
			// Send message to every online user (except the author)
			for (int i = 0; i < BACKLOG; i++) {
				if (client_list[i] != NULL) {
					// Do not send the message to the author
					if (strcmp (queue.front->username, client_list[i]) != 0) {
						int ret_val = write (i+3, queue.front->message, strlen(queue.front->message));
						check_write (ret_val);
					}
				}
			}

			// Update front of MessageQueue 
			update_queue_front();
		}

		// Release Locks
		pthread_mutex_unlock (&llock);
		pthread_mutex_unlock (&qlock);
	}
}

/*	handle_messages
	Client messages are added to the message queue
	Starts the connection termination procedure if client leaves
*/
int handle_messages (int clientfd, char* username) {
	struct timeval tv;		// Timeout struct for select call
	tv.tv_sec = 1;			// 1 second
	tv.tv_usec = 0;			// 0 microseconds
	fd_set readfds;			// FDs to check for read

	// Use select to determine if:
	//	   Message has arrived from the other side of connection
	// Note: the client username is in the message already
	while (1) {
		FD_ZERO (&readfds);					// Clear FD set
		FD_SET (clientfd, &readfds);		// Include clientfd to set
		int select_ret = select (clientfd + 1, &readfds, NULL, NULL, &tv);
		check_select (select_ret);			// Check select return value

		if (FD_ISSET (clientfd, &readfds)) { // Received message to display
			char* message = (char*)malloc (MESS_MAX * sizeof(char));
			memset (message, 0, MESS_MAX);			// Clear message buffer
			int read_ret = read (clientfd, message, MESS_MAX);// Read to buffer
			if (read_ret == 0) {		// Connection has terminated
				free (message);			// Free the recently allocated buffer
				remove_client (clientfd, username);	// Terminate the connection
				return 0;				// Exit this function
			} 
			else {						// Add message to message queue
				gen_MessageNode (username, message);	// Add message to queue
			}
		}
	}
}

/*	login_message
	Creates & enqueues message notifying online users about our client
	That is, telling online users that our client just entered the chatroom
*/
void login_message (char* username) {
	// Generate message
	char* message = (char*) malloc (MESS_MAX * sizeof(char));
	char* login = " has entered the chatroom\n";
	strcpy (message, username);				
	strcat (message, login);

	// Add message to MessageQueue
	gen_MessageNode (username, message);
}

/*	nstrtol
	Function uses strtol to convert the provided argument to an int
	Includes error checking for underflow, overflow, or non-int input
	Program's error message will print and program will exit if failure
*/
int nstrtol (const char *ptr) {
	char *endptr;
	int return_value;
	errno = 0;

	return_value = strtol(ptr, &endptr, 10);
	if (endptr == ptr) { // Do we have something that's not a number?
		print_help_server();
	}
	if (errno == ERANGE) { // Do we have an over/underflow
		print_help_server();
	}

	return return_value;
}

/*	print_client_list
	Writes the current client_list to file descriptor clientfd
	Used to tell newly logged in clients who is currently online
	WARNING: Make sure that llock is acquired before calling
*/
void print_client_list(int clientfd) {
	char* title = "Users Currently Online:\n";
	char* newline = "\n";
	int write_ret = write (clientfd, title, strlen(title));
	check_write(write_ret);
	for (int i = 0; i < BACKLOG; i++) {
		if (client_list[i] != NULL) {
			write_ret = write(clientfd, client_list[i], strlen(client_list[i]));
			check_write(write_ret);
			write_ret = write(clientfd, newline, strlen(newline));
			check_write(write_ret);
		}
	}
}

/*	print_help_server
	Prints usage instructions for the server
*/
void print_help_server () {
	printf ("\tUsage: ./server <optional port number>\n");
	printf ("The default port used is port 5555\n");
	exit(1);
}

/*	remove_client
	Handles termination of client connection
	Removes the username from the list of online clients
	Notifies all online clients that this user has left the chatroom
*/
void remove_client (int clientfd, char* username) {
	// Remove client username from client_list
	pthread_mutex_lock (&llock);			// Acquire lock on client list
	client_list [(clientfd - 3)] = NULL;	// Update the client list
	close(clientfd);						// Close connection on server end
	pthread_mutex_unlock (&llock);			// Release lock on client list

	// Write termination message to Server (to have a log)
	printf ("Connection Terminated with user: '%s'\n", username);

	// Add termination message to MessageQueue (to send to clients)
	char* term_notice = " has left the chat room\n";
	char* message = (char*) malloc (MESS_MAX * sizeof(char));
	strcpy (message, username);				// Add username to message
	strcat (message, term_notice);			// Finish the mesage
	gen_MessageNode (username, message);	// Send out the message
}

/*	update_queue_front
	Updates the front of MessageQueue (queue of messages)
	Also does cleanup for the current MessageQueue front
*/
void update_queue_front () {
	MessageNode* oldfront = queue.front;
	queue.front = queue.front -> next;

	// Cleanup the old front node
	free (oldfront -> message);
	free (oldfront);
}