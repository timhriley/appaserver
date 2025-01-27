/* library/fifo.h */
/* -------------- */
#ifndef FIFO_H
#define FIFO_H

#include "boolean.h"

/* Constants */
/* --------- */
#define FIFO_FILENAME_TEMPLATE		"/tmp/fifo_%d"
#define FIFO_SPOOL_FILENAME_TEMPLATE	"/tmp/fifo_spool_%d"

typedef struct
{
	FILE *output_file;
	FILE *input_pipe;
	char *process;
	char *fifo_filename;
	char *spool_filename;
} FIFO;

/* Prototypes */
/* ---------- */
FIFO *fifo_new(		char *process,
			pid_t pid );

FILE *fifo_open_write(
			char *fifo_filename,
			char *spool_filename );

FILE *fifo_open_read(
			char *fifo_filename );

void fifo_set(		FILE *output_file,
			char *string );

char *fifo_get(		FILE *input_pipe );

void fifo_close(	FIFO *fifo );

void fifo_make(		char *fifo_filename );

char *fifo_get_fifo_filename(
			pid_t pid );

char *fifo_get_spool_filename(
			pid_t pid );

void fifo_execute_process(
			char *spool_filename,
			char *process,
			char *fifo_filename );

FILE *fifo_open_input_pipe(
			char *spool_filename,
			char *process );

#endif
