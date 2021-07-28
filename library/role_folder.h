/* $APPASERVER_HOME/library/role_folder.h				*/
/* -------------------------------------------------------------------- */
/* This is the appaserver role_folder ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef ROLE_FOLDER_H
#define ROLE_FOLDER_H

#include "list.h"

typedef struct
{
	char *role_name;
	char *folder_name;
	char insert_yn;
	char update_yn;
	char lookup_yn;
	char delete_yn;
} ROLE_FOLDER;

LIST *role_folder_list_fetch(
			char *role_name );

ROLE_FOLDER *role_folder_fetch( 
			char *role_name,
			char *folder_name );

ROLE_FOLDER *role_folder_new( 
			char *role_name,
			char *folder_name );

void role_folder_load(
			char *insert_yn,
			char *update_yn,
			char *lookup_yn,
			char *delete_yn,
			char *role_name,
			char *folder_name );

LIST *role_folder_insert_list_fetch(
			char *role_name );

LIST *role_folder_update_list_fetch(
			char *role_name );

LIST *role_folder_lookup_list_fetch(
			char *role_name );

char *role_folder_display(
			ROLE_FOLDER *role_folder );

void role_folder_populate_folder_insert_permission(
			LIST *folder_list, 
			LIST *role_folder_insert_list,
			char *application_name );

void role_folder_populate_folder_update_permission(
			LIST *folder_list, 
			LIST *role_folder_update_list,
			char *application_name );

void role_folder_populate_folder_lookup_permission(
			LIST *folder_list, 
			LIST *role_folder_lookup_list,
			char *application_name );

boolean role_folder_viewonly(
			ROLE_FOLDER *role_folder );

#endif
