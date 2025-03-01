/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/alter_datatype.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef ALTER_DATATYPE_H
#define ALTER_DATATYPE_H

#include "folder_attribute.h"

typedef struct
{
	FOLDER_ATTRIBUTE *folder_attribute;
	char *folder_table_name;
	char *attribute_database_datatype;
	char *execute_system_string;
	char *shell_script;
	char *process_filespecification;
} ALTER_DATATYPE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
ALTER_DATATYPE *alter_datatype_new(
		char *application_name,
		char *folder_name,
		char *attribute_name,
		char *data_directory );

/* Process */
/* ------- */
ALTER_DATATYPE *alter_datatype_calloc(
		void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *alter_datatype_execute_system_string(
		char *attribute_name,
		char *folder_table_name,
		char *attribute_database_datatype );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *alter_datatype_process_filespecification(
		char *application_name,
		char *folder_name,
		char *attribute_name,
		char *data_directory );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *alter_datatype_shell_script(
		char *application_name,
		FOLDER_ATTRIBUTE *folder_attribute,
		char *folder_table_name,
		char *alter_datatype_execute_system_string );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *alter_datatype_shell_script_system_string(
		char *statement );

#endif
