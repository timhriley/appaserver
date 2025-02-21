/* -------------------------------------------------------------------- */
/* $APPASERVER_PARAMETER/library/appaserver_parameter.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef APPASERVER_PARAMETER_H
#define APPASERVER_PARAMETER_H

#include "boolean.h"
#include "dictionary.h"

#define APPASERVER_PARAMETER_APPLICATION_NAME	"/etc/appaserver_%s.config"
#define APPASERVER_PARAMETER_GENERIC_NAME	"/etc/appaserver.config"

typedef struct
{
	char *application_name;
	char *filename;
	FILE *file;
	DICTIONARY *dictionary;
	char *mysql_user;
	char *password;
	char *flags;
	char *MYSQL_HOST;
	char *MYSQL_TCP_PORT;
	char *mount_point;
	char *apache_cgi_directory;
	char *cgi_home;
	char *document_root;
	char *log_directory;
	char *data_directory;
	char *upload_directory;
	char *backup_directory;
	boolean mysql_password_syntax;
} APPASERVER_PARAMETER;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
APPASERVER_PARAMETER *appaserver_parameter_new(
		void );

/* Process */
/* ------- */
APPASERVER_PARAMETER *appaserver_parameter_calloc(
		void );

/* Safely returns */
/* -------------- */
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

/* Returns component of global_appaserver_parameter */
/* ------------------------------------------------ */
char *appaserver_parameter_mount_point(
		void );

/* Returns component of global_appaserver_parameter */
/* ------------------------------------------------ */
char *appaserver_parameter_cgi_directory(
		void );

/* Returns component of global_appaserver_parameter */
/* ------------------------------------------------ */
char *appaserver_parameter_log_directory(
		void );

/* Returns component of global_appaserver_parameter */
/* ------------------------------------------------ */
char *appaserver_parameter_data_directory(
		void );

/* Returns heap memory */
/* ------------------- */
char *appaserver_parameter_application_data_directory(
		char *application_name,
		char *appaserver_parameter_data_directory );

/* Returns component of global_appaserver_parameter */
/* ------------------------------------------------ */
char *appaserver_parameter_cgi_home(
		void );

/* Returns component of global_appaserver_parameter */
/* ------------------------------------------------ */
char *appaserver_parameter_document_root(
		void );

/* Returns component of global_appaserver_parameter */
/* ------------------------------------------------ */
char *appaserver_parameter_upload_directory(
		void );

/* Returns component of global_appaserver_parameter */
/* ------------------------------------------------ */
char *appaserver_parameter_backup_directory(
		void );

#endif
