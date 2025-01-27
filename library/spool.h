/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/spool.h	 				*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#ifndef SPOOL_H
#define SPOOL_H

typedef struct
{
	char *output_filename;
	char *create_system_string;
	char *output_system_string;
	FILE *output_pipe;
} SPOOL;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
SPOOL *spool_new(
		char *system_string,
		boolean capture_stderr_boolean );

/* Process */
/* ------- */
SPOOL *spool_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *spool_output_filename(
		pid_t process_id );

/* Returns static memory */
/* --------------------- */
char *spool_create_system_string(
		char *spool_output_filename );

/* Returns static memory */
/* --------------------- */
char *spool_output_system_string(
		char *system_string, 
		boolean capture_stderr_boolean,
		char *spool_output_filename );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FILE *spool_output_pipe(
		char *spool_output_system_string );

/* Usage */
/* ----- */
LIST *spool_list(
		char *spool_output_filename );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *spool_remove_system_string(
		char *spool_output_filename );

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

#endif
