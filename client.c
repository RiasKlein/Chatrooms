/******************************************************************************

	Name: 		Shunman Tse
	NID:		N13382418
	Net ID:		st1637
	Course:		UNIX Programming
	Homework:	Multi-threading

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
int 	check_input_for_exit(char* input);
int 	get_PORT_NUM (int argc, char** argv);
int 	handle_chat (int clientfd, char* username, int termexit);
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

	int write_ret = write (clientfd, argv[1], strlen(argv[1]));// Send username
	check_write(write_ret);										// check write
	write_ret = write(1, username, strlen(username)); 		// print username
	check_write (write_ret);								// check write
	
	if (handle_chat(clientfd, username, 1)) {
		// If user used /exit command, send out proper message to server
		char* message = (char*) malloc (MESS_MAX * sizeof (char));
		char* ending = " is logging off\n";
		strcpy (message, argv[1]);
		strcat (message, ending);
		int write_ret = write (clientfd, message, strlen(message)); // Send msg
		check_write(write_ret);
		free (message);
	}	
	
	// Cleanup
	close(clientfd);		// Close connection FD					
	free (username);		// Free username

	return 0;
}

/*	check_input_for_exit
	Checks whether the user typed in the /exit command
*/
int check_input_for_exit(char* input) {
	if (strcmp (input, "/exit\n") == 0) {
		return 1;
	}
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

/*	handle_chat
	Checks if there's user input or a message received from other side
	If our user has finished input message, we send it to the other side
	If we receive a message, we print it to the STDOUT
	The above repeats until there's an error or connection termination
*/
int handle_chat (int clientfd, char* username, int termexit) {
	struct timeval tv;		// Timeout struct for select call
	tv.tv_sec = 1;			// 1 second
	tv.tv_usec = 0;			// 0 microseconds
	fd_set readfds;			// FDs to check for read
	int write_ret;			// return value of write call

	// Buffers for reading user input and formatting message to send
	char* input = (char*)malloc (MESS_MAX * sizeof(char));
	char* message = (char*)malloc (MESS_MAX * sizeof(char)); 

	// Use select to determine if:
	//     1. User has entered message to send in STDIN
	//	   2. Message has arrived from the other side of connection
	// Handle the above cases as needed
	while(1) {
		FD_ZERO (&readfds);					// Clear FD set
		FD_SET (clientfd, &readfds);		// Include clientfd to set
		FD_SET (STDIN_FILENO, &readfds);	// Include STDIN to set
		int select_ret = select (clientfd + 1, &readfds, NULL, NULL, &tv);
		check_select (select_ret);			// Check select return value
		if (FD_ISSET (0, &readfds)) {	// User entered message to send
			input = fgets (input, MESS_MAX, stdin);	// read stdin for input
			if (check_input_for_exit(input)) {	// If /exit command is used
				free (input);					// Free input / message
				free (message);					// Return proper value
				return 1;
			}
			memset (message, 0, MESS_MAX);			// Clear message buffer
			strcpy (message, username);				// Include username
			strcat (message, input);				// Include the message
			write_ret = write (1, username, strlen(username));	// username
			check_write(write_ret);
			write_ret = write (clientfd, message, strlen(message)); // Send msg
			check_write(write_ret);
		}
		if (FD_ISSET (clientfd, &readfds)) { // Received message to display
			memset (message, 0, MESS_MAX);	// Clear message buffer for read
			int read_ret = read (clientfd, message, MESS_MAX);// Read to buffer
			if (read_ret == 0) {		// Connection has terminated
				fprintf (stderr, "Connection terminated...\n");
				if (termexit) exit(1);	// client terminates if connection ends
				else { 					// server doesn't term. with connection
					// Cleanup
					free (input);
					free (message);
					return 0;
				}			
			} 
			else {						// write message to STDOUT
				write_ret = write(1, message, strlen(message));
				check_write (write_ret);	// Check write call's return value
			}
		}
	}
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