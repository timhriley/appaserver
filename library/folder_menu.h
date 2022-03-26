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

typedef struct
{
	char *folder_name;
	unsigned long count;
	char *display;
} FOLDER_MENU_COUNT;

/* FOLDER_MENU_COUNT operations */
/* ---------------------------- */

/* Usage */
/* ----- */
LIST *folder_menu_count_list(
			char *application_name,
			LIST *lookup_folder_name_list,
			char *filename );

FOLDER_MENU_COUNT *folder_menu_count_new(
			char *application_name,
			char *folder_name );

/* Process */
/* ------- */
unsigned long folder_menu_count_fetch(
			char *folder_table_name );

/* Returns heap memory */
/* ------------------- */
char *folder_menu_count_display(
			unsigned long count );

/* Public */
/* ------ */
void folder_menu_count_list_write(
			char *filename,
			LIST *folder_menu_count_list );

LIST *folder_menu_count_read_list(
			char *filename );

FOLDER_MENU_COUNT *folder_menu_count_seek(
			char *folder_name,
			LIST *folder_menu_count_list );

/* Private */
/* ------- */
FOLDER_MENU_COUNT *folder_menu_count_calloc(
			void );

typedef struct
{
	ROLE *role;
	LIST *lookup_folder_name_list;
	LIST *insert_folder_name_list;
	char *filename;
	LIST *count_list;
} FOLDER_MENU;

/* FOLDER_MENU operations */
/* ---------------------- */
FOLDER_MENU *folder_menu_new(
			char *application_name,
			char *session_key,
			char *role_name,
			char *appaserver_data_directory );

FOLDER_MENU *folder_menu_calloc(
			void );

LIST *folder_menu_lookup_folder_name_list(
			char *role_name );

LIST *folder_menu_insert_folder_name_list(
			char *role_name );

/* Returns heap memory or null */
/* --------------------------- */
char *folder_menu_filename(
			char *application_name,
			char *session_key,
			char *role_name,
			char *appaserver_data_directory );

#endif
