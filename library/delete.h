/* $APPASERVER_HOME/library/delete_database.h				*/
/* -------------------------------------------------------------------- */
/* This is the appaserver delete_database ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef DELETE_DATABASE_H
#define DELETE_DATABASE_H

#include <stdio.h>
#include "list.h"
#include "dictionary.h"
#include "process.h"
#include "related_folder.h"

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	LIST *primary_data_list;
} DELETE_ROW;

typedef struct
{
	char *folder_name;
	PROCESS *post_change_process;
	LIST *primary_key_list;
	boolean null_1tom_upon_delete;
	LIST *update_null_attribute_name_list;
	LIST *delete_row_list;
} DELETE_FOLDER;

typedef struct
{
	/* Input */
	/* ----- */
	char *application_name;
	char *login_name;
	char *role_name;
	LIST *primary_data_list;
	boolean dont_delete_mto1_isa;
	char *folder_name;

	/* Process */
	/* ------- */
	FOLDER *folder;
	LIST *delete_folder_list;
} DELETE;

DELETE_DATABASE *delete_database_new(
			char *application_name,
			char *login_name,
			char *role_name,
			boolean dont_delete_mto1_isa,
			char *folder_name,
			LIST *primary_attribute_data_list,
			char *sql_executable );

LIST *delete_database_get_delete_folder_list(
			char *application_name,
			char *folder_name,
			PROCESS *post_change_process,
			LIST *primary_key_list,
			boolean row_level_non_owner_forbid,
			boolean row_level_non_owner_view_only,
			LIST *mto1_isa_related_folder_list,
			LIST *one2m_recursive_related_folder_list,
			boolean dont_delete_mto1_isa,
			LIST *primary_attribute_data_list,
			char *login_name );

DELETE_FOLDER *delete_database_delete_folder_new(
			char *folder_name,
			boolean null_1tom_upon_delete,
			PROCESS *post_change_process,
			LIST *primary_key_list );

DELETE_ROW *delete_database_delete_row_new(
			LIST *primary_attribute_data_list );

LIST *delete_database_get_delete_row_list(
			char *application_name,
			char *folder_name,
			boolean row_level_non_owner_forbid,
			boolean row_level_non_owner_view_only,
			LIST *select_attribute_name_list,
			LIST *where_attribute_name_list,
			LIST *where_attribute_data_list,
			char *login_name );

void delete_database_append_mto1_isa_delete_folder_list(
			LIST *delete_folder_list,
			char *application_name,
			LIST *mto1_isa_related_folder_list,
			LIST *primary_attribute_data_list );

void delete_database_append_one2m_delete_folder_list(
			LIST *delete_folder_list,
			char *application_name,
			char *one2m_related_folder_name,
			LIST *where_attribute_name_list,
			LIST *where_attribute_data_list,
			char *login_name,
			LIST *one2m_related_folder_list,
			LIST *primary_key_list,
			PROCESS *post_change_process );

void delete_database_execute_delete_folder_list(
			char *application_name,
			LIST *delete_folder_list,
			char *sql_executable );

void delete_database_set_parameter_dictionary(
			DICTIONARY *parameter_dictionary,
			LIST *primary_key_list,
			LIST *primary_attribute_data_list );

void delete_database_execute_delete_row_list(
			char *application_name,
			char *folder_name,
			LIST *delete_row_list,
			LIST *primary_key_list,
			char *sql_executable,
			PROCESS *post_change_process );

void delete_database_execute_delete_row_list_pipe(
			FILE *output_pipe,
			FILE *stderr_pipe,
			DICTIONARY *parameter_dictionary,
			char *application_name,
			char *folder_name,
			LIST *delete_row_list,
			LIST *primary_key_list,
			PROCESS *post_change_process );

void delete_database_execute_update_to_null_row(
			char *application_name,
			char *folder_name,
			LIST *delete_row_list,
			LIST *primary_key_list,
			LIST *update_null_attribute_name_list,
			char *sql_executable );

void delete_database_execute_update_to_null_row_pipe(
			FILE *output_pipe,
			FILE *stderr_pipe,
			LIST *delete_row_list,
			LIST *update_null_attribute_name_list );

char *delete_database_display(
			DELETE_DATABASE *delete_database );

void delete_database_append_mto1_isa_one2m_recursive(
			LIST *delete_folder_list,
			LIST *delete_row_list,
			LIST *primary_key_list,
			LIST *one2m_recursive_related_folder_list,
			char *application_name );

DELETE_FOLDER *delete_database_get_or_set_delete_folder(
			LIST *delete_folder_list,
			char *folder_name,
			PROCESS *post_change_process,
			LIST *primary_key_list );

void delete_database_append_null_1tom_delete_folder_list(
			LIST *delete_folder_list,
			char *application_name,
			char *one2m_related_folder_name,
			PROCESS *one2m_related_post_change_process,
			LIST *one2m_related_primary_key_list,
			LIST *foreign_attribute_name_list,
			LIST *primary_attribute_data_list,
			char *login_name );

void delete_database_append_one2m_related_folder_list(
			LIST *delete_folder_list,
			LIST *one2m_related_folder_list,
			char *application_name,
			LIST *primary_attribute_data_list,
			char *login_name );

LIST *delete_database_get_update_null_attribute_name_list(
			LIST *foreign_attribute_name_list,
			LIST *one2m_related_primary_key_list );

void delete_database_refresh_row_count(
			char *application_name,
			LIST *delete_folder_list,
			char *session,
			char *role_name );

#endif
