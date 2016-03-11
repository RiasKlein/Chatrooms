/******************************************************************************

	Name: 		Shunman Tse
	NID:		N13382418
	Net ID:		st1637
	Course:		UNIX Programming
	Homework:	2-Way Chat

	sharedFunc.c
		Contains functions utilized by both server.c and client.c 

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

/*	check_bind
	Checks the return value of bind call
*/
void check_bind (int bind_success) {
	if (bind_success == -1){
		perror ("bind call failed");
		exit(1);
	}
}

/*	check_connect
	Checks the return value of connect call
*/
void check_connect (int con_ret) {
	if (con_ret == -1) {
		perror ("connect call failed");
		exit(1);
	}
}

/*	check_socketfd
	Checks the return value of socket call
*/
void check_socketfd (int socketfd) {
	if (socketfd == -1) {
		perror ("socket call failed");
		exit(1);
	}
}

/*	check_select
	Checks the return value of select call
*/
void check_select (int select_ret) {
	if (select_ret == -1) {
		perror ("select call failed");
		exit(1);
	}
}

/*	check_write
	Checks the return value of write call
*/
void check_write(int write_ret) {
	if (write_ret == -1) { 
		perror ("write call failed"); 
		exit(1); 
	}
}

/*	create_socket
	Socket call to create communication endpoint & returns the FD	
*/
int create_socket () {
	int socketfd = socket(AF_INET, SOCK_STREAM, 0);	// Socket call
	check_socketfd (socketfd);	// Check return value of socket call
	return socketfd;
}

/*	gen_username
	Creates and returns char* that contains "username: "
*/
char* gen_username (char** argv) {
	char* user = (char*) malloc ((strlen (argv[1]) + 3) * sizeof(char));
	strcpy (user, argv[1]);
	char* sep = ": ";
	strcat (user, sep);
	return user;
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


