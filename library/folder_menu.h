/* $APPASERVER_HOME/library/folder_menu.h	 */
/* --------------------------------------------- */
/* Freely available software: see Appaserver.org */
/* --------------------------------------------- */

#ifndef FOLDER_MENU_H
#define FOLDER_MENU_H

/* Includes */
/* -------- */
#include "boolean.h"
#include "list.h"
#include "role.h"

/* Constants */
/* --------- */
#define FOLDER_MENU_PERMISSIONS "insert,update,lookup"

/* Structures */
/* ---------- */
typedef struct
{
	char *folder_name;
	long int count;
} FOLDER_MENU_COUNT;

typedef struct
{
	/* Input */
	/* ----- */
	char *application_name;
	char *session_key;
	char *appaserver_data_directory;
	char *role_name;

	/* Process */
	/* ------- */
	ROLE *role;
	char *filename;
	LIST *count_list;
} FOLDER_MENU;

/* FOLDER_MENU operations */
/* ---------------------- */
FOLDER_MENU *folder_menu_calloc(
			void );

FOLDER_MENU *folder_menu_new(
			char *application_name,
			char *session_key,
			char *appaserver_data_directory,
			char *role_name );

boolean folder_menu_write(
			char *filename,
			LIST *folder_menu_count_list );

LIST *folder_menu_choose_folder_list(
			char *application_name,
			char *appaserver_data_directory,
			char *session_key,
			char *role_name,
			boolean with_count );

char *folder_menu_input_pipe_sys_string(
			char *application_name,
			char *session_key,
			char *appaserver_data_directory,
			char *role_name );

char *folder_menu_filename(
			char *application_name,
			char *session_key,
			char *appaserver_data_directory,
			char *role_name );

void folder_menu_create_spool_file(
			char *application_name,
			char *session_key,
			char *appaserver_data_directory,
			char *role_name,
			boolean with_count );

boolean folder_menu_file_exists(
			char *application_name,
			char *session_key,
			char *appaserver_data_directory,
			char *role_name );

void folder_menu_remove_file(
			char *application_name,
			char *session_key,
			char *appaserver_data_directory,
			char *role_name );

void folder_menu_refresh_row_count(
			char *application_name,
			char *folder_name,
			char *session_key,
			char *appaserver_data_directory,
			char *role_name );

void folder_menu_refresh_folder_name_list(
			char *application_name,
			LIST *folder_name_list,
			char *session_key,
			char *role_name );

void folder_menu_refresh_role(
			char *application_name,
			char *session_key,
			char *role_name );

/* FOLDER_MENU_COUNT operations */
/* ---------------------------- */
long int folder_menu_folder_count(
			char *folder_table_name );

#endif
