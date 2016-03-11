/******************************************************************************

	Name: 		Shunman Tse
	NID:		N13382418
	Net ID:		st1637
	Course:		UNIX Programming
	Homework:	2-Way Chat

	sharedFunc.h

******************************************************************************/

/* Function Prototypes */
void 	check_bind (int bind_success);
void 	check_connect (int con_ret);
void 	check_socketfd (int socketfd);
void 	check_select (int select_ret);
void 	check_write(int write_ret);
int 	create_socket ();
char* 	gen_username (char** argv);




