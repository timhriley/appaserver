/* library/appaserver_parameter.h					*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef APPASERVER_PARAMETER_H
#define APPASERVER_PARAMETER_H

#include "boolean.h"
#include "dictionary.h"

/* Constants */
/* --------- */
#define APPASERVER_PARAMETER_DEFAULT_DIRECTORY		"/etc"
#define APPASERVER_PARAMETER_NAME			"appaserver.config"
#define APPASERVER_PARAMETER_APPLICATION_FILE_NAME	"appaserver_%s.config"

/* Type definitions */
/* ---------------- */
typedef struct
{
	char *user;
	char *password;
	char *flags;
	char *MYSQL_HOST;
	char *MYSQL_TCP_PORT;
	char *appaserver_mount_point;
	char *apache_cgi_directory;
	char *cgi_home;
	char *document_root;
	char *appaserver_error_directory;
	char *appaserver_data_directory;
	char *parameter_file_full_path;
	boolean mysql_password_syntax;

} APPASERVER_PARAMETER;

/* Prototypes */
/* ---------- */
APPASERVER_PARAMETER *appaserver_parameter_new(
			void );

char *appaserver_parameter_mount_point(
			void );

char *appaserver_parameter_cgi_directory(
			void );

char *appaserver_parameter_error_directory(
			void );

char *appaserver_parameter_data_directory(
			void );

char *appaserver_parameter_cgi_home(
			void );

char *appaserver_parameter_document_root(
			void );

DICTIONARY *appaserver_parameter_load_record_dictionary(
			FILE *input_pipe,
			int delimiter );

APPASERVER_PARAMETER *appaserver_parameter_fetch(
			FILE *f,
			char *parameter_file_full_path );

APPASERVER_PARAMETER *appaserver_parameter_application(
			char *application_name );

FILE *appaserver_parameter_open(
			char *filename,
			char *application_name );

#endif
