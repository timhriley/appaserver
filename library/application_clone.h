/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/application_clone.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef APPLICATION_CLONE_H
#define APPLICATION_CLONE_H

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include "boolean.h"
#include "list.h"
#include "application.h"
#include "appaserver_user.h"
#include "application_create.h"

#define APPLICATION_CLONE_SQL_GZ \
	"template/mysqldump_template.sql.gz"

typedef struct
{
	APPLICATION_CREATE *application_create;
	char *gz_system_string;
	char *title;
	LIST *zaptable_list;
	char *database_system_string;
	char *table_name;
	APPLICATION *application;
	char *application_system_string;
	APPASERVER_USER *appaserver_user;
	char *insert_user_system_string;
	char *insert_role_system_string;
	char *upgrade_system_string;
} APPLICATION_CLONE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
APPLICATION_CLONE *application_clone_new(
		char *current_application_name,
		char *login_name,
		char *destination_application_name,
		char *application_clone_sql_gz_filename,
		char *application_title,
		char *document_root,
		char *data_directory,
		char *download_directory,
		char *log_directory,
		char *mount_point );

/* Process */
/* ------- */
APPLICATION_CLONE *application_clone_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *application_clone_gz_system_string(
		char *destination_application_name,
		char *application_clone_sql_gz_filespecification );

/* Returns heap memory */
/* ------------------- */
char *application_clone_upgrade_system_string(
		char *destination_application_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *application_clone_sql_gz_filename(
		char *mount_point );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *application_clone_application_system_string(
		const char *application_table,
		char *destination_appliction_name,
		char *application_title,
		boolean ssl_support_boolean );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *application_clone_insert_user_system_string(
		const char *appaserver_user_table,
		char *destination_application_name,
		APPASERVER_USER *appaserver_user );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *application_clone_insert_role_system_string(
		const char *role_system,
		const char *role_supervisor,
		char *login_name,
		char *destination_application_name );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
APPASERVER_USER *application_clone_appaserver_user(
		char *login_name );

/* Usage */
/* ----- */
LIST *application_clone_zaptable_list(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *application_clone_database_system_string(
		char *current_application_name,
		char *destination_application_name,
		LIST *application_create_zaptable_list );

/* Usage */
/* ----- */
void application_clone_system(
		boolean execute_boolean,
		char *system_string );

/* Usage */
/* ----- */
char *application_clone_sql_gz_filespecification(
		const char *application_clone_sql_gz,
		char *mount_point );

#endif
