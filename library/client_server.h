/* library/client_server.h */
/* ----------------------- */
#ifndef CLIENT_SERVER_H
#define CLIENT_SERVER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "boolean.h"

/* Constants */
/* --------- */
#define CLIENT_SERVER_CLIENT_SERVER_PORT	12345
#define CLIENT_SERVER_SERVER_CLIENT_PORT	12346
#define CLIENT_SERVER_IP_ADDRESS		"127.0.0.1"

typedef struct
{
	PIPE *client_server_pipe;
	PIPE *server_client_pipe;
	char *server_command_line;
} CLIENT_SERVER;

/* Prototypes */
/* ---------- */
boolean client_server_send_client_to_server(
				PIPE *client_server_pipe,
				char *message );

boolean client_server_send_server_to_client(
				PIPE *server_client_pipe,
				char *message );

CLIENT_SERVER *client_server_new(
				char *server_command_line );

void client_server_free(	CLIENT_SERVER *client_server );

#endif
