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
#define APPASERVER_PARAMETER_APPLICATION_NAME	"/etc/appaserver_%s.config"
#define APPASERVER_PARAMETER_GENERIC_NAME	"/etc/appaserver.config"

typedef struct
{
	/* Input */
	/* ----- */
	char *application_name;

	/* Attributes */
	/* ---------- */
	char *filename;
	FILE *file;
	DICTIONARY *dictionary;
	char *mysql_user;
	char *password;
	char *flags;
	char *MYSQL_HOST;
	char *MYSQL_TCP_PORT;
	char *appaserver_mount_point;
	char *apache_cgi_directory;
	char *cgi_home;
	char *document_root_directory;
	char *error_directory;
	char *data_directory;
	char *upload_directory;
	boolean mysql_password_syntax;
} APPASERVER_PARAMETER;

/* APPASERVER_PARAMETER operations */
/* -------------------------------- */
APPASERVER_PARAMETER *appaserver_parameter_calloc(
			void );

APPASERVER_PARAMETER *appaserver_parameter_new(
			void );

APPASERVER_PARAMETER *appaserver_parameter_application(
			char *application_name );

/* Returns heap memory or null */
/* --------------------------- */
char *appaserver_parameter_filename(
			char *application_name );

FILE *appaserver_parameter_open(
			char *filename );

DICTIONARY *appaserver_parameter_dictionary(
			FILE *file );

/* ---------------------------- */
/* Returns appaserver_parameter */
/* ---------------------------- */
APPASERVER_PARAMETER *appaserver_parameter_fetch(
			APPASERVER_PARAMETER *appaserver_parameter,
			DICTIONARY *dictionary );

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

char *appaserver_parameter_document_root_directory(
			void );

char *appaserver_parameter_document_root(
			void );

char *appaserver_parameter_upload_directory(
			void );

#endif
