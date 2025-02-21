/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/appaserver.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef APPASERVER_H
#define APPASERVER_H

#include <stdio.h>
#include "list.h"
#include "boolean.h"

#define APPASERVER_CONFIG_FILESPEC		"/etc/appaserver.config"
#define APPASERVER_DROP_DOWN_ORIGINAL_LABEL	"original_"

#define APPASERVER_INSERT_STATE			"insert"
#define APPASERVER_UPDATE_STATE			"update"
#define APPASERVER_LOOKUP_STATE			"lookup"
#define APPASERVER_VIEWONLY_STATE		"viewonly"
#define APPASERVER_DELETE_STATE			"delete"
#define APPASERVER_PREDELETE_STATE		"predelete"
#define APPASERVER_EXECUTE_YN			"execute_yn"
#define APPASERVER_STATION_TABLE		"station"
#define APPASERVER_NAVD88_ATTRIBUTE		"ngvd29_navd88_increment"
#define APPASERVER_CAPACITY_THRESHOLD		75.0
#define APPASERVER_NULL_STRING			"null"
#define APPASERVER_MAILNAME_FILESPECIFICATION	"/etc/mailname"

#define APPASERVER_WINDOW_OPEN_TEMPLATE		\
"window.open('%s','%s','menubar=no,resizable=yes,scrollbars=yes,status=no,toolbar=no,location=no', 'false')"

/* Usage */
/* ----- */

/* Returns heap memory or "" */
/* ------------------------- */
char *appaserver_select(
		LIST *folder_attribute_name_list );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *appaserver_spreadsheet_heading_string(
		LIST *folder_attribute_name_list );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *appaserver_spreadsheet_output_system_string(
		char delimiter,
		char *spreadsheet_filename );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *appaserver_system_string(
		char *select,
		char *table,
		char *where );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FILE *appaserver_input_pipe(
		char *appaserver_system_string );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FILE *appaserver_output_file(
		char *filename );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FILE *appaserver_append_file(
		char *filename );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FILE *appaserver_input_file(
		char *filename );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FILE *appaserver_output_pipe(
		char *output_system_string );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *appaserver_spool_filename(
		char *application_name,
		char *folder_name,
		char *session_key,
		pid_t process_id,
		char *appaserver_parameter_data_directory );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *appaserver_mnemonic(
		LIST *key_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *appaserver_execute_bit_system_string(
		char *shell_filename );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *appaserver_table_name(
		char *folder_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *appaserver_mailname(
		const char *appaserver_mailname_filespecification );

/* Usage */
/* ----- */
boolean appaserver_executable_exists_boolean(
		char *executable );

#endif

