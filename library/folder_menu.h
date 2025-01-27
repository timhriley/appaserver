/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/folder_menu.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FOLDER_MENU_H
#define FOLDER_MENU_H

#include "boolean.h"
#include "list.h"
#include "role.h"

#define FOLDER_MENU_EXTENSION	"tab"

typedef struct
{
	char *folder_name;
	unsigned long count;
	char *display;
} FOLDER_MENU_COUNT;

/* Usage */
/* ----- */
LIST *folder_menu_count_list(
		char *application_name,
		LIST *folder_menu_lookup_folder_name_list,
		char *folder_menu_filename );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *folder_menu_count_system_string(
		char *application_name,
		char *folder_menu_filename );

/* Usage */
/* ----- */
FOLDER_MENU_COUNT *folder_menu_count_parse(
		char *input );

/* Usage */
/* ----- */
FOLDER_MENU_COUNT *folder_menu_count_new(
		char *folder_name,
		char *count_string );

/* Process */
/* ------- */
FOLDER_MENU_COUNT *folder_menu_count_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *folder_menu_count_display(
		unsigned long count );

/* Usage */
/* ----- */
FOLDER_MENU_COUNT *folder_menu_count_seek(
		char *folder_name,
		LIST *folder_menu_count_list );

typedef struct
{
	ROLE *role;
	LIST *lookup_folder_name_list;
	LIST *insert_folder_name_list;
	char *filename;
	LIST *count_list;
} FOLDER_MENU;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FOLDER_MENU *folder_menu_fetch(
		char *application_name,
		char *session_key,
		char *role_name,
		char *appaserver_data_directory,
		boolean folder_menu_remove_boolean );

/* Process */
/* ------- */
FOLDER_MENU *folder_menu_calloc(
		void );

LIST *folder_menu_lookup_folder_name_list(
		char *role_name );

LIST *folder_menu_insert_folder_name_list(
		char *role_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *folder_menu_filename(
		const char *folder_menu_extension,
		char *application_name,
		char *session_key,
		char *role_name,
		char *appaserver_data_directory );

/* Usage */
/* ----- */
void folder_menu_remove_file(
		char *folder_menu_filename );

#endif
