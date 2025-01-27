/* library/pipe.h */
/* ----------------- */
#ifndef PIPE_H
#define PIPE_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "boolean.h"

#define MSGSIZE			4096
#define PIPE_ALL_DONE		"pipe_all_done"

typedef struct
{
	int read_port_number;
	int write_port_number;
	char *read_ip_address;
	char *write_ip_address;
} PIPE;

/* Prototypes */
/* ---------- */
char *pipe_receive(	char *read_ip_address,
			int read_port_number );

boolean pipe_send(	char *write_ip_address,
			int write_port_number,
			char *pipe_text );

PIPE *pipe_new(		int read_port_number,
			int write_port_number,
			char *read_ip_address,
			char *write_ip_address );

void pipe_free(	PIPE *pipe );

#endif
