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