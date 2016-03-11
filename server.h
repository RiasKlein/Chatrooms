/******************************************************************************

	Name: 		Shunman Tse
	NID:		N13382418
	Net ID:		st1637
	Course:		UNIX Programming
	Homework:	Multi-threading

	server.h

******************************************************************************/

/* Structs */
typedef struct MessageNode {
	char* username;
	char* message;
	struct MessageNode* next;
} MessageNode;

typedef struct MessageQueue {
	MessageNode* front;
	MessageNode* back;
} MessageQueue;

/* Function Prototypes */
void 	bind_serverfd (int serverfd, int PORT_NUM);
void 	checknzero_client_list (char** client_list);
void 	check_pthread_create (int create_ret);
void 	enqueue_message (MessageNode* node);
void 	gen_client_thread(int clientfd);
void 	gen_MessageNode (char* username, char* message);
void 	gen_queue_thread ();
int 	get_PORT_NUM (int argc, char** argv);
void*	handle_client (void* fd);
void*	handle_MessageQueue (void* ignored);
int 	handle_messages (int clientfd, char* username);
void 	login_message (char* username);
int 	nstrtol (const char *ptr);
void 	print_client_list(int clientfd);
void 	print_help_server ();
void 	remove_client (int clientfd, char* username);
void 	update_queue_front ();
