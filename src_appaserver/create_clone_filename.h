/* create_clone_filename.h						*/
/* -------------------------------------------------------------------- */
/* This is the appaserver create_clone_filename ADT.			*/
/* It manages the creation of zipped filenames for both			*/
/* the create and clone application processes. And it			*/
/* manages the creation of shell script filenames for both the		*/
/* create and clone folder processes.					*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef CREATE_CLONE_FILENAME_H
#define CREATE_CLONE_FILENAME_H

#include "list.h"

/* Objects */
/* ------- */
typedef struct
{
	char *application_name;
	char *appaserver_data_directory;
	char *directory_name;
	LIST *folder_name_list;
} CREATE_CLONE_FILENAME;

/* Operations */
/* ---------- */
CREATE_CLONE_FILENAME *create_clone_filename_new(
			char *application_name,
			char *appaserver_data_directory );

char *create_clone_filename_get_directory_name(
			char *appaserver_data_directory,
			char *directory_name );

char *create_clone_filename_get_clone_application_compressed_filename(
			char **directory_name,
			char *application_name,
			char *appaserver_data_directory,
			char system_folders_yn,
			char *export_output );
char *create_clone_filename_get_create_application_filename(
			char **directory_name,
			char *application_name,
			char *appaserver_data_directory,
			char system_folders_yn );
char *create_clone_filename_get_shell_script_filename(
			char *destination,
			char **directory_name,
			char *application_name,
			char *appaserver_data_directory,
			char *folder_name );
char *create_clone_filename_get_clone_folder_compressed_filename(
			char *destination,
			char **directory_name,
			char *application_name,
			char *appaserver_data_directory,
			char *folder_name );
char *create_clone_filename_get_create_folder_filename(
			char *destination,
			char **directory_name,
			char *application_name,
			char *appaserver_data_directory,
			char *folder_name );
char *create_clone_filename_get_clone_folder_tee_process_string(
			char **directory_name,
			char *application_name,
			char *appaserver_data_directory,
			char *folder_name );
char *create_clone_filename_get_create_table_tee_process_string(
			char **directory_name,
			char *application_name,
			char *appaserver_data_directory,
			char *folder_name );
char *create_clone_filename_get_create_application_combine_sys_string(
			char **directory_name,
			char *application_name,
			char *appaserver_data_directory,
			LIST *folder_name_list,
			char system_folders_yn );
char *create_clone_filename_get_create_application_remove_sys_string(
			char **directory_name,
			char *application_name,
			char *appaserver_data_directory,
			LIST *folder_name_list );
char *create_clone_filename_get_clone_folder_filename(
			char *destination,
			char **directory_name,
			char *application_name,
			char *appaserver_data_directory,
			char *folder_name,
			char *export_output );
#endif
