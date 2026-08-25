/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/spool.h	 				*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#ifndef SPOOL_H
#define SPOOL_H

typedef struct
{
	char *output_filespecification;
	char *create_system_string;
	char *output_system_string;
	FILE *output_pipe;
} SPOOL;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */

/* The output is in spool_list( spool_new()->output_filespecification ) */
/* -------------------------------------------------------------------- */
SPOOL *spool_new(
		char *system_string,
		boolean capture_stderr_boolean );

/* Process */
/* ------- */
SPOOL *spool_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *spool_output_filespecification(
		pid_t process_id );

/* Returns static memory */
/* --------------------- */
char *spool_create_system_string(
		char *spool_output_filespecification );

/* Returns static memory */
/* --------------------- */
char *spool_output_system_string(
		char *system_string, 
		boolean capture_stderr_boolean,
		char *spool_output_filespecification );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FILE *spool_output_pipe(
		char *spool_output_system_string );

/* Usage */
/* ----- */
LIST *spool_list(
		char *spool_output_filespecification );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *spool_remove_system_string(
		char *spool_output_filespecification );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *spool_pipe(
		char *system_string,
		boolean capture_stderr_boolean );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *spool_fetch(
		char *system_string,
		boolean capture_stderr_boolean );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *spool_data_string(
		char *system_string,
		char *data_string );

#endif
