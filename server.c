/******************************************************************************

	Name: 		Shunman Tse
	NID:		N13382418
	Net ID:		st1637
	Course:		UNIX Programming
	Homework:	2-Way Chat

	server.c
		Usage: ./server <user_name> <optional port num>

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
#include "sharedFunc.h"

#define BACKLOG 	1024
#define MESS_MAX 	2000000

/* Function Prototypes */
void 	bind_serverfd (int serverfd, int PORT_NUM);
int 	get_PORT_NUM (int argc, char** argv);
int 	nstrtol (const char *ptr);
void 	print_help_server ();

int main (int argc, char** argv){
	int serverfd, clientfd;						// file descriptors

	int PORT_NUM = get_PORT_NUM (argc, argv);	// set port number
	serverfd = create_socket(); 				// Create socket
	char* username = gen_username (argv);		// get username

	bind_serverfd(serverfd, PORT_NUM);			// bind serverfd 

	// System will use this socket for listening 
	listen (serverfd, BACKLOG);

	while (1) {
		// Wait for a connection & print message when connection is made
		fprintf (stderr, "Server: Waiting for connection...\n");
		clientfd = accept(serverfd, NULL, NULL);
		fprintf (stderr, "Connection established...\n");

		int write_ret = write(1, username, strlen(username));// print username
		check_write (write_ret);							 // check write
		handle_chat (clientfd, username, 0);				 // 2-way chatting
		close (clientfd);		// Close fd when connection is terminated
	}

	// Cleanup
	free (username);

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
	check_bind(bind_ret);		// Check return value of bind call
}

/*	get_PORT_NUM
	Function acquires the port number of the server to connect to from argv
	Usage message is printed if necessary
*/
int get_PORT_NUM (int argc, char** argv) {
	int PORT_NUM;
	if (argc > 3 || argc < 2) print_help_server();	// check number of args
	if (argc > 2) PORT_NUM = nstrtol(argv[2]);	// use optional port if given
	else PORT_NUM = 5555;						// else use default port

	return PORT_NUM;
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

/*	print_help_server
	Prints usage instructions for the server
*/
void print_help_server () {
	printf ("\tUsage: ./server <user name> <optional port number>\n");
	printf ("The default port used is port 5555\n");
	exit(1);
}