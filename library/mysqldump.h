/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/mysqldump.h					*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#ifndef MYSQLDUMP_H
#define MYSQLDUMP_H

#include "list.h"
#include "boolean.h"
#include "fork_control.h"

#define MYSQLDUMP_COUNT_PREFIX		"count"
#define MYSQLDUMP_DELIMITER		'='
#define MYSQLDUMP_DROP_THRESHOLD	0.02

#define MYSQLDUMP_MKDIR_SCRIPT		"mysqldump_mkdir.sh"
#define MYSQLDUMP_PASSWORD_FILE_SCRIPT	"mysqldump_password_file.sh"
#define MYSQLDUMP_LOG_SCRIPT		"mysqldump_log.sh"
#define MYSQLDUMP_ROTATE_SCRIPT		"mysqldump_rotate.sh"

typedef struct
{
	char *table_name;
	char *mysqldump_filename;
	char *mysqldump_system_string;
	char *system_string;
} MYSQLDUMP_CONSOLIDATE;

/* Usage */
/* ----- */
LIST *mysqldump_consolidate_list(
		char *database,
		char *mysql_user,
		char *password_filename,
		char *date_yyyymmdd,
		LIST *separate_block_table_list,
		LIST *separate_line_table_list,
		LIST *exclude_table_list,
		LIST *mysqldump_this_run_list );

/* Usage */
/* ----- */
MYSQLDUMP_CONSOLIDATE *mysqldump_consolidate_new(
		char *database,
		char *mysql_user,
		char *password_filename,
		char *date_yyyymmdd,
		char *table_name );

/* Process */
/* ------- */
MYSQLDUMP_CONSOLIDATE *mysqldump_consolidate_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *mysqldump_consolidate_system_string(
		char *mysqldump_system_string,
		char *mysqldump_filename );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *mysqldump_consolidate_tar_filename(
		char *database,
		char *date_yyyymmdd );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *mysqldump_consolidate_tar_system_string(
		LIST *mysqldump_consolidate_list,
		char *mysqldump_consolidate_tar_filename );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *mysqldump_consolidate_rm_system_string(
		LIST *mysqldump_consolidate_list );

typedef struct
{
	char *table_name;
	char *mysqldump_filename;
	char *mysqldump_system_string;
	char *system_string;
} MYSQLDUMP_SEPARATE;

/* Usage */
/* ----- */
LIST *mysqldump_separate_list(
		char *database,
		char *mysql_user,
		char *password_filename,
		char *date_yyyymmdd,
		LIST *separate_table_list,
		boolean is_line );

/* Usage */
/* ----- */
MYSQLDUMP_SEPARATE *mysqldump_separate_new(
		char *database,
		char *mysql_user,
		char *password_filename,
		char *date_yyyymmdd,
		char *table_name,
		boolean is_line );

/* Process */
/* ------- */
MYSQLDUMP_SEPARATE *mysqldump_separate_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *mysqldump_separate_system_string(
		char *mysqldump_system_string,
		char *mysqldump_filename );

typedef struct
{
	char *table_name;
	int row_count;
	double percentage_drop;
} MYSQLDUMP_THIS_RUN;

/* Usage */
/* ----- */
LIST *mysqldump_this_run_list(
		char mysqldump_delimiter,
		char *database,
		char *mysqldump_this_run_filename,
		LIST *mysqldump_last_run_list );

/* Usage */
/* ----- */
MYSQLDUMP_THIS_RUN *mysqldump_this_run_new(
		LIST *mysqldump_last_run_list,
		char *table_name,
		int row_count );

/* Process */
/* ------- */
MYSQLDUMP_THIS_RUN *mysqldump_this_run_calloc(
		void );

/* Usage */
/* ----- */
double mysqldump_this_run_percentage_drop(
		LIST *mysqldump_last_run_list,
		char *table_name,
		int row_count );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *mysqldump_this_run_threshold_display(
		const double mysqldump_drop_threshold,
		char *database,
		LIST *mysqldump_this_run_list );

/* Process */
/* ------- */
LIST *mysqldump_this_run_threshold_name_list(
		const double mysqldump_drop_threshold,
		LIST *mysqldump_this_run_list );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *mysqldump_this_run_filename(
		const char *mysqldump_count_prefix,
		char *mysqldump_input_date_yyyymmdd,
		char *mysqldump_input_son );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *mysqldump_this_run_system_string(
		char *mysqldump_this_run_filename );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *mysqldump_this_run_display_system_string(
		char *database,
		char *mysqldump_this_run_filename );

typedef struct
{
	char *table_name;
	int row_count;
} MYSQLDUMP_LAST_RUN;

/* Usage */
/* ----- */
LIST *mysqldump_last_run_list(
		char mysqldump_delimiter,
		char *mysqldump_last_run_filename );

/* Usage */
/* ----- */
MYSQLDUMP_LAST_RUN *mysqldump_last_run_new(
		char *table_name,
		int row_count );

/* Process */
/* ------- */
MYSQLDUMP_LAST_RUN *mysqldump_last_run_calloc(
		void );

/* Usage */
/* ----- */
int mysqldump_last_run_row_count(
		LIST *mysqldump_last_run_list,
		char *table_name );

/* Process */
/* ------- */
MYSQLDUMP_LAST_RUN *mysqldump_last_run_seek(
		LIST *mysqldump_last_run_list,
		char *table_name );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *mysqldump_last_run_filename(
		char *mysqldump_count_prefix,
		char *date_yyyymmdd,
		char *son );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *mysqldump_last_run_yyyymmdd(
		char *mysqldump_last_run_filename );

typedef struct
{
	char *database;
	int parallel_count;
	char *separate_block_table_list_string;
	char *separate_line_table_list_string;
	char *exclude_table_list_string;
	char *offsite_rsync_destination;
	char *offsite_scp_destination;
	char *mysql_user;
	char *password;
	char *log_directory;
	char *backup_directory;
	char *password_filename;
	char *date_yyyymmdd;
	char *mysqldump_this_run_filename;
	char *basename;
	char *son;
	char *father;
	char *grandfather;
	char *greatgrandfather;
	char *greatgreatgrandfather;
	LIST *separate_block_table_list;
	LIST *separate_line_table_list;
	LIST *exclude_table_list;
} MYSQLDUMP_INPUT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
MYSQLDUMP_INPUT *mysqldump_input_fetch(
		char *argv_0,
		char *config_filename );

/* Process */
/* ------- */
MYSQLDUMP_INPUT *mysqldump_input_calloc(
		void );

void mysqldump_input_parse(
		char *config_filename,
		char **database /* out */,
		int *parallel_count /* out */,
		char **separate_block_table_list_string /* out */,
		char **separate_line_table_list_string /* out */,
		char **exclude_table_list_string /* out */,
		char **offsite_rsync_destination /* out */,
		char **offsite_scp_destination /* out */ );

void mysqldump_input_appaserver_parse(
		char *database,
		char **mysql_user /* out */,
		char **mysql_password /* out */,
		char **log_directory /* out */,
		char **backup_directory /* out */ );

/* Returns static memory */
/* --------------------- */
char *mysqldump_input_date_yyyymmdd(
		void );

/* Returns static memory */
/* --------------------- */
char *mysqldump_input_basename(
		char *argv_0 );

/* Returns static memory */
/* --------------------- */
char *mysqldump_input_son(
		char *database,
		char *backup_directory );

/* Returns static memory */
/* --------------------- */
char *mysqldump_input_father(
		char *database,
		char *backup_directory );

/* Returns static memory */
/* --------------------- */
char *mysqldump_input_grandfather(
		char *database,
		char *backup_directory );

/* Returns static memory */
/* --------------------- */
char *mysqldump_input_greatgrandfather(
		char *database,
		char *backup_directory );

/* Returns static memory */
/* --------------------- */
char *mysqldump_input_greatgreatgrandfather(
		char *database,
		char *backup_directory );

LIST *mysqldump_input_separate_block_table_list(
		char *separate_block_table_list_string );

LIST *mysqldump_input_separate_line_table_list(
		char *separate_line_table_list_string );

LIST *mysqldump_input_exclude_table_list(
		char *exclude_table_list_string );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *mysqldump_input_password_filename(
		const char *mysqldump_password_file_script
			/* mysqldump_password_file.sh */,
		char *password );

typedef struct
{
	MYSQLDUMP_INPUT *mysqldump_input;
	boolean subdirectory_exists_boolean;
	char *mkdir_system_string;
	char *mysqldump_last_run_filename;
	char *mysqldump_last_run_yyyymmdd;
	LIST *mysqldump_last_run_list;
	LIST *mysqldump_this_run_list;
	char *mysqldump_this_run_threshold_display;
	char *mysqldump_this_run_display_system_string;
	LIST *mysqldump_separate_block_list;
	LIST *mysqldump_separate_line_list;
	LIST *mysqldump_consolidate_list;
	FORK_CONTROL *fork_control;
	char *remove_password_system_string;
	char *mysqldump_consolidate_tar_filename;
	char *mysqldump_consolidate_tar_system_string;
	char *mysqldump_consolidate_rm_system_string;
	char *log_system_string;
	char *rotate_system_string;
	char *rsync_system_string;
	char *scp_system_string;
} MYSQLDUMP;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
MYSQLDUMP *mysqldump_new(
		char *argv_0,
		char *config_filename );

/* Process */
/* ------- */
MYSQLDUMP *mysqldump_calloc(
		void );

boolean mysqldump_subdirectory_exists_boolean(
		char *son );

/* Returns static memory */
/* --------------------- */
char *mysqldump_mkdir_system_string(
		const char *mysqldump_mkdir_script,
		char *database,
		char *backup_directory );

/* Returns static memory */
/* --------------------- */
char *mysqldump_remove_password_system_string(
		char *password_filename );

/* Returns heap memory */
/* ------------------- */
char *mysqldump_log_system_string(
		const char *mysqldump_log_script,
		const char *application_log_extension,
		char *database,
		char *log_directory,
		char *date_yyyymmdd,
		char *son );

/* Returns heap memory */
/* ------------------- */
char *mysqldump_rotate_system_string(
		const char *mysqldump_rotate_script,
		char *date_yyyymmdd,
		char *son,
		char *father,
		char *grandfather,
		char *greatgrandfather,
		char *greatgreatgrandfather );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *mysqldump_filename(
		char *date_yyyymmdd,
		char *table_name,
		boolean is_gzip );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *mysqldump_system_string(
		char *database,
		char *mysql_user,
		char *password_filename,
		char *table_name,
		boolean is_line );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FORK_CONTROL *mysqldump_fork_control(
		int parallel_count,
		LIST *mysqldump_separate_block_list,
		LIST *mysqldump_separate_line_list,
		LIST *mysqldump_consolidate_list );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *mysqldump_rsync_system_string(
		char *son,
		char *offsite_rsync_destination );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *mysqldump_scp_system_string(
		char *database,
		char *offsite_scp_destination,
		char *mysqldump_consolidate_tar_filename );

#endif
