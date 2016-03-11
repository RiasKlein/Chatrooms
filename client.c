/******************************************************************************

	Name: 		Shunman Tse
	NID:		N13382418
	Net ID:		st1637
	Course:		UNIX Programming
	Homework:	2-Way Chat

	client.c
		Usage:	./client <user name> <Server IP> <Server Port>

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

#define MESS_MAX 	2000000

/* Function Prototypes */
int 	get_PORT_NUM (int argc, char** argv);
int 	nstrtol (const char *ptr);
void 	print_help_client ();

int main (int argc, char** argv) {
	struct sockaddr_in servaddr;			// struct for handling networking
	int con_ret;							// return value of connect

	int PORT_NUM = get_PORT_NUM(argc, argv);	// Get server port number
	char* username = gen_username (argv);		// Get client username
	int clientfd = create_socket();				// Create Socket

	// Configure servaddr
	memset (&servaddr, 0, sizeof (servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons (PORT_NUM);	
	inet_pton (AF_INET, argv[2], &servaddr.sin_addr.s_addr);

	// Connect to provided server
	con_ret = connect(clientfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	check_connect (con_ret);					// Check return value
	fprintf (stderr, "Connection established...\n");

	int write_ret = write(1, username, strlen(username)); 	// print username
	check_write (write_ret);								// check write
	handle_chat(clientfd, username, 1);						// 2-way chatting
	
	// Cleanup
	close(clientfd);							
	free (username);

	return 0;
}

/* 	get_PORT_NUM
	Function acquires the port number of the server to connect to from argv
	Usage message is printed if necessary
*/
int get_PORT_NUM (int argc, char** argv) {
	int PORT_NUM;
	if (argc != 4) print_help_client();	// check number of args
	PORT_NUM = nstrtol(argv[3]);		// Port is argv[3]

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
		print_help_client();
	}
	if (errno == ERANGE) { // Do we have an over/underflow
		print_help_client();
	}

	return return_value;
}

/* 	print_help_client
	Prints usage instructions for client 
*/
void print_help_client () {
	printf ("\tUsage: ./client <user name> <Server IP addr> <Server Port Number>\n");
	exit(1);
}