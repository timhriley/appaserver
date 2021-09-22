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

/* Structures */
/* ---------- */
typedef struct
{
	char *folder_name;
	unsigned long count;
	char *display;
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
	LIST *lookup_folder_name_list;
	LIST *insert_folder_name_list;
	LIST *lookup_count_list;
	char *filename;
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

LIST *folder_menu_lookup_folder_name_list(
			char *role_name );

LIST *folder_menu_insert_folder_name_list(
			char *role_name );

/* Returns heap memory or null */
/* --------------------------- */
char *folder_menu_filename(
			char *application_name,
			char *session_key,
			char *appaserver_data_directory,
			char *role_name );

/* FOLDER_MENU_COUNT operations */
/* ---------------------------- */
LIST *folder_menu_lookup_count_list(
			char *application_name,
			LIST *lookup_folder_name_list,
			char *filename );

FOLDER_MENU_COUNT *folder_menu_count_calloc(
			void );

FOLDER_MENU_COUNT *folder_menu_count_new(
			char *application_name,
			char *folder_name );

LIST *folder_menu_read_count_list(
			char *filename );

unsigned long folder_menu_count_fetch(
			char *folder_table_name );

void folder_menu_write_count_list(
			char *filename,
			LIST *lookup_count_list );

FOLDER_MENU_COUNT *folder_menu_count_seek(
			char *folder_name,
			LIST *folder_menu_lookup_count_list );

char *folder_menu_count_display(
			unsigned long count );

#endif
