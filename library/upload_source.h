/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/upload_source.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef UPLOAD_SOURCE_H
#define UPLOAD_SOURCE_H

#include "boolean.h"

typedef struct
{
	boolean file_exists_boolean;
	char *directory_filename;
	char *filename;
	char *destination_directory;
	boolean destination_directory_exists_boolean;
	char *destination_filename;
	char *copy_system_string;
	char *chmod_system_string;
	char *confirm_system_string;
} UPLOAD_SOURCE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
UPLOAD_SOURCE *upload_source_new(
		char *application_name,
		char *directory_filename_session,
		char *mount_point );

/* Process */
/* ------- */
UPLOAD_SOURCE *upload_source_calloc(
		void );

/* Usage */
/* ----- */
boolean upload_source_file_exists_boolean(
		char *directory_filename_session );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *upload_source_directory_filename(
		char *directory_filename_session );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *upload_source_filename(
		char *upload_source_directory_filename );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *upload_source_destination_directory(
		char *application_name,
		char *mount_point );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *upload_source_destination_filename(
		char *upload_source_filename,
		char *upload_source_destination_directory );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *upload_source_copy_system_string(
		char *directory_filename_session,
		char *upload_source_destination_filename );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *upload_source_chmod_system_string(
		char *upload_source_destination_filename );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *upload_source_confirm_system_string(
		char *upload_source_filename,
		char *upload_source_destination_directory );

#endif
