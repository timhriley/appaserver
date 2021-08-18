/* library/appaserver_error.h */
/* -------------------------- */

#ifndef APPASERVER_ERROR_H
#define APPASERVER_ERROR_H

/* Includes */
/* -------- */

/* Constants */
/* --------- */
#ifndef APPASERVER_DATABASE_ERROR_FILE
#define APPASERVER_DATABASE_ERROR_FILE		"/var/log/appaserver_%s.err"
#endif

#ifndef APPASERVER_ERROR_FILE
#define APPASERVER_ERROR_FILE			"/var/log/appaserver.err"
#endif

#define APPASERVER_OLD_ERROR_FILE		"/var/appaserver.err"

/* Data structures */
/* --------------- */

/* Protoypes */
/* --------- */
void m(			char *message );

char *appaserver_error_filename(
			char *application_name );

void appaserver_error_stderr(
			int argc,
			char **argv );

void appaserver_error_argv_append_file(
			int argc,
			char **argv,
			char *application_name );

void appaserver_error_login_name_append_file(
			int argc,
			char **argv,
			char *application_name,
			char *login_name );

FILE *appaserver_error_open_append_file(
			char *application_name );

void appaserver_output_error_message(
			char *application_name,
			char *message,
			char *login_name );

void output_error_message(
			char *message,
			char *login_name );

void m2(		char *application_name,
			char *message );

#endif
