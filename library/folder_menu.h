/* library/folder_menu.h				   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#ifndef FOLDER_MENU_H
#define FOLDER_MENU_H

/* Includes */
/* -------- */
#include "boolean.h"
#include "list.h"

/* Constants */
/* --------- */
#define FOLDER_MENU_PERMISSIONS "insert,update,lookup"

/* Typedefs */
/* -------- */
typedef struct
{
	char *filename;
	LIST *folder_list;
} FOLDER_MENU;

/* Prototypes */
/* ---------- */
FOLDER_MENU *folder_menu_new(		char *application_name,
					char *session,
					char *appaserver_data_directory,
					char *role_name );

long int folder_menu_fetch_folder_count(char *application_name,
					char *folder_name );

boolean folder_menu_write_folder_count_list(
					char *filename,
					LIST *folder_list );

LIST *folder_menu_get_choose_folder_list(
					char *application_name,
					char *appaserver_data_directory,
					char *session,
					char *role_name,
					boolean with_count );

char *folder_menu_get_input_pipe_sys_string(
					char *application_name,
					char *session,
					char *appaserver_data_directory,
					char *role_name );

char *folder_menu_get_filename(
					char *application_name,
					char *session,
					char *appaserver_data_directory,
					char *role_name );

void folder_menu_create_spool_file(
			char *application_name,
			char *session,
			char *appaserver_data_directory,
			char *role_name,
			boolean with_count );

boolean folder_menu_file_exists(
			char *application_name,
			char *session,
			char *appaserver_data_directory,
			char *role_name );

void folder_menu_remove_file(
			char *application_name,
			char *session,
			char *appaserver_data_directory,
			char *role_name );

void folder_menu_refresh_row_count(
			char *application_name,
			char *folder_name,
			char *session,
			char *appaserver_data_directory,
			char *role_name );

void folder_menu_refresh_folder_name_list(
			char *application_name,
			LIST *folder_name_list,
			char *session,
			char *role_name );

void folder_menu_refresh_role(
			char *application_name,
			char *session,
			char *role_name );

#endif
