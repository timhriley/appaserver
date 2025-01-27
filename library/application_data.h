/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/application_data.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef APPLICATION_DATA_H
#define APPLICATION_DATA_H

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include "boolean.h"
#include "list.h"

typedef struct
{
	char *directory;
	char *create_directory_system_string;
	char *texlive_2019_link_system_string;
	char *texlive_2021_link_system_string;
	char *delete_system_string;
} APPLICATION_DATA;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
APPLICATION_DATA *application_data_new(
		char *application_name,
		char *data_directory );

/* Process */
/* ------- */
APPLICATION_DATA *application_data_calloc(
		void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *application_data_directory(
		char *application_name,
		char *data_directory );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *application_data_create_directory_system_string(
		char *application_data_directory );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *application_data_textlive_link_system_string(
		const char *application_data_texlive_directory,
		char *application_data_directory,
		char *appaserver_parameter_mount_point );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *application_data_delete_system_string(
		char *application_data_directory );

#endif
