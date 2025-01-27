/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/appaserver_upgrade.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef APPASERVER_UPGRADE_H
#define APPASERVER_UPGRADE_H

#include "boolean.h"
#include "list.h"

typedef struct
{
	char *application_name;
	LIST *system_string_list;
	LIST *insert_statement_list;
} APPASERVER_UPGRADE_EXECUTE;

/* Usage */
/* ----- */
LIST *appaserver_upgrade_execute_list(
		boolean step_boolean,
		LIST *appaserver_upgrade_application_name_list,
		LIST *appaserver_upgrade_directory_list,
		LIST *appaserver_upgrade_application_list );

/* Usage */
/* ----- */
APPASERVER_UPGRADE_EXECUTE *appaserver_upgrade_execute_new(
		boolean step_boolean,
		char *application_name,
		LIST *appaserver_upgrade_directory_list,
		LIST *appaserver_upgrade_application_list );

/* Process */
/* ------- */
APPASERVER_UPGRADE_EXECUTE *appaserver_upgrade_execute_calloc(
		void );

/* Usage */
/* ----- */
void appaserver_upgrade_execute_list_system(
		char *application_name,
		LIST *system_string_list,
		LIST *insert_statement_list );

typedef struct
{
	char *upgrade_script;
	char *execute_system_string;
	char *insert_statement;
} APPASERVER_UPGRADE_FILE;

/* Usage */
/* ----- */
LIST *appaserver_upgrade_file_list(
		char *appaserver_upgrade_root,
		char *appaserver_version );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *appaserver_upgrade_file_root(
		char *appaserver_upgrade_root,
		char *appaserver_version );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *appaserver_upgrade_file_system_string(
		char *appaserver_upgrade_file_root );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
APPASERVER_UPGRADE_FILE *appaserver_upgrade_file_new(
		char *appaserver_version,
		char *appaserver_upgrade_file_root,
		char *upgrade_script );

/* Process */
/* ------- */
APPASERVER_UPGRADE_FILE *appaserver_upgrade_file_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *appaserver_upgrade_file_execute_system_string(
		char *appaserver_upgrade_file_root,
		char *upgrade_script );

/* Returns heap memory */
/* ------------------- */
char *appaserver_upgrade_file_insert_statement(
		char *appaserver_upgrade_script_table,
		char *appaserver_upgrade_script_insert,
		char *appaserver_version,
		char *upgrade_script );

typedef struct
{
	char *appaserver_version;
	LIST *appaserver_upgrade_file_list;
} APPASERVER_UPGRADE_DIRECTORY;

/* Usage */
/* ----- */
LIST *appaserver_upgrade_directory_list(
		char *appaserver_upgrade_root );

/* Usage */
/* ----- */
APPASERVER_UPGRADE_DIRECTORY *appaserver_upgrade_directory_fetch(
		char *appaserver_upgrade_root,
		char *appaserver_version );

/* Process */
/* ------- */
APPASERVER_UPGRADE_DIRECTORY *appaserver_upgrade_directory_calloc(
		void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *appaserver_upgrade_directory_system_string(
		char *appaserver_upgrade_root );

typedef struct
{
	char *application_name;
	LIST *appaserver_upgrade_table_list;
} APPASERVER_UPGRADE_APPLICATION;

/* Usage */
/* ----- */
LIST *appaserver_upgrade_application_list(
		LIST *appaserver_upgrade_application_name_list,
		LIST *appaserver_upgrade_version_list );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
APPASERVER_UPGRADE_APPLICATION *appaserver_upgrade_application_new(
		char *application_name,
		LIST *appaserver_upgrade_version_list );

/* Process */
/* ------- */
APPASERVER_UPGRADE_APPLICATION *appaserver_upgrade_application_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *appaserver_upgrade_application_system_string(
		char *mount_point,
		char *appaserver_version,
		char *upgrade_script );

#define APPASERVER_UPGRADE_SCRIPT_TABLE		"upgrade_script"

#define APPASERVER_UPGRADE_SCRIPT_INSERT	"appaserver_version,"	\
						"upgrade_script"

typedef struct
{
	char *upgrade_script;
} APPASERVER_UPGRADE_SCRIPT;

/* Usage */
/* ----- */
LIST *appaserver_upgrade_script_list(
		char *appaserver_version );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *appaserver_upgrade_script_where(
		char *appaserver_version );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
APPASERVER_UPGRADE_SCRIPT *appaserver_upgrade_script_new(
		char *appaserver_version,
		char *upgrade_script );

/* Process */
/* ------- */
APPASERVER_UPGRADE_SCRIPT *appaserver_upgrade_script_calloc(
		void );

/* Usage */
/* ----- */
boolean appaserver_upgrade_script_exists(
		char *application_name,
		char *appaserver_version,
		char *upgrade_script,
		LIST *appaserver_upgrade_application_list );

/* Process */
/* ------- */
APPASERVER_UPGRADE_SCRIPT *appaserver_upgrade_script_seek(
		char *upgrade_script,
		LIST *appaserver_upgrade_script_list );

typedef struct
{
	char *appaserver_version;
	LIST *appaserver_upgrade_script_list;
} APPASERVER_UPGRADE_TABLE;

/* Usage */
/* ----- */
LIST *appaserver_upgrade_table_list(
		char *application_name,
		LIST *appaserver_upgrade_version_list );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
APPASERVER_UPGRADE_TABLE *appaserver_upgrade_table_new(
		char *appaserver_version );

/* Process */
/* ------- */
APPASERVER_UPGRADE_TABLE *appaserver_upgrade_table_calloc(
		void );

typedef struct
{
	LIST *application_list_argv;
	LIST *application_name_list;
	char *root;
	LIST *appaserver_upgrade_directory_list;
	LIST *version_list;
	LIST *appaserver_upgrade_application_list;
	LIST *appaserver_upgrade_execute_list;
} APPASERVER_UPGRADE;

/* Usage */

/* Safely returns */
/* -------------- */
APPASERVER_UPGRADE *appaserver_upgrade_fetch(
		char *application_log_extension,
		int argc,
		char **argv,
		char *application_name,
		boolean step_boolean,
		char *appaserver_parameter_log_directory,
		char *appaserver_parameter_mount_point );

/* Process */
/* ------- */
APPASERVER_UPGRADE *appaserver_upgrade_calloc(
		void );

LIST *appaserver_upgrade_application_list_argv(
		int argc,
		char **argv );

LIST *appaserver_upgrade_version_list(
		LIST *appaserver_upgrade_directory_list );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *appaserver_upgrade_root(
		char *mount_point );

/* Usage */
/* ----- */
LIST *appaserver_upgrade_application_name_list(
		char *application_log_extension,
		char *application_name,
		char *log_directory,
		LIST *appaserver_upgrade_application_list_argv );

#endif
