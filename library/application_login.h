/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/application_login.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef APPLICATION_LOGIN_H
#define APPLICATION_LOGIN_H

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include "boolean.h"
#include "list.h"

typedef struct
{
	char *title;
	char *destination_directory;
	char *directory_system_string;
	char *php_file_system_string;
	char *delete_system_string;
} APPLICATION_LOGIN;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
APPLICATION_LOGIN *application_login_new(
		const char *application_template_name,
		const char *application_create_default_title,
		char *application_name,
		char *application_title,
		char *document_root,
		char *mount_point );

/* Process */
/* ------- */
APPLICATION_LOGIN *application_login_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *application_login_directory_system_string(
		char *application_login_destination_directory );

/* Usage */
/* ----- */

/* Returns either parameter */
/* ------------------------ */
char *application_login_title(
		const char *application_create_default_title,
		char *application_title );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *application_login_destination_directory(
		char *application_name,
		char *document_root );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *application_login_source_filename(
		const char *application_template_name,
		const char *extension,
		char *mount_point );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *application_login_destination_filename(
		const char *extension,
		char *application_login_destination_directory );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *application_login_file_system_string(
		char *application_name,
		char *application_login_title,
		char *application_login_source_filename,
		char *application_login_destination_filename );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *application_login_delete_system_string(
		char *application_login_destination_directory );

#endif
