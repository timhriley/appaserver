/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/application_backup.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef APPLICATION_BACKUP_H
#define APPLICATION_BACKUP_H

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include "boolean.h"
#include "list.h"

#define APPLICATION_BACKUP_MKDIR_SCRIPT	"mysqldump_mkdir.sh"

typedef struct
{
	char *appaserver_parameter_backup_directory;
	char *filename;
	char *create_file_system_string;
	char *delete_file_system_string;
	char *create_directory_system_string;
	char *directory_name;
	char *delete_directory_system_string;
} APPLICATION_BACKUP;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
APPLICATION_BACKUP *application_backup_new(
		const char *application_create_template_name,
		char *application_name,
		char *mount_point );

/* Process */
/* ------- */
APPLICATION_BACKUP *application_backup_calloc(
		void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *application_backup_filename(
		char *application_name,
		char *appaserver_parameter_backup_directory );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *application_backup_create_file_system_string(
		const char *application_create_template_name,
		char *application_name,
		char *mount_point,
		char *application_backup_filename );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *application_backup_directory_name(
		char *application_name,
		char *appaserver_parameter_backup_directory );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *application_backup_create_directory_system_string(
		const char *application_backup_mkdir_script,
		char *application_name,
		char *appaserver_parameter_backup_directory );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *application_backup_delete_file_system_string(
		char *application_backup_filename );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *application_backup_delete_directory_system_string(
		char *application_backup_directory_name );

#endif
