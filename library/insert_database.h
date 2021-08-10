/* $APPASERVER_HOME/library/insert_database.h				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef INSERT_DATABASE_H
#define INSERT_DATABASE_H

#include <stdio.h>
#include "list.h"
#include "dictionary.h"
#include "boolean.h"
#include "process.h"
#include "related_folder.h"

#define INSERT_DATABASE_DONT_REMOVE_TMP_FILE	0

/* Structures */
/* ---------- */
typedef struct
{
	char *attribute_name;
	char *escaped_replaced_data;
} INSERT_DATABASE_ATTRIBUTE_DATA;

typedef struct
{
	char *application_name;
	char *session;
	char *folder_name;
	LIST *primary_key_list;
	LIST *attribute_name_list;
	LIST *data_list;
	DICTIONARY *row_dictionary;
	LIST *ignore_attribute_name_list;
	boolean insert_row_zero_only;
	boolean dont_remove_tmp_file;
	int rows_inserted;
	char *post_insert_process;
	char *login_name;
	char *tmp_file_name;
	FILE *tmp_file;
	LIST *attribute_list;
	LIST *attribute_data_list;
} INSERT_DATABASE;

/* Operations */
/* ---------- */
INSERT_DATABASE_ATTRIBUTE_DATA *insert_database_attribute_data_new(
			char *attribute_name,
			char *escaped_replaced_data );

INSERT_DATABASE *insert_database_new(
			char *application_name,
			char *session,
			char *folder_name,
			LIST *primary_key_list,
			LIST *attribute_name_list,
			DICTIONARY *row_dictionary,
			DICTIONARY *ignore_dictionary,
			LIST *attribute_list );

void set_insert_row_zero_only(
			INSERT_DATABASE *insert_database );

void insert_database_set_row_zero_only(
			INSERT_DATABASE *insert_database );

int insert_database_execute(
			char **message,
			char *application_name,
			char *session,
			char *folder_name,
			char *role_name,
			LIST *primary_key_list,
			LIST *insert_required_attribute_name_list,
			LIST *attribute_name_list,
			DICTIONARY *row_dictionary,
			LIST *ignore_attribute_name_list,
			int insert_row_zero_only,
			boolean dont_remove_tmp_file,
			PROCESS *post_change_process,
			char *login_name,
			LIST *mto1_related_folder_list,
			LIST *common_non_primary_key_list,
			LIST *attribute_list,
			boolean exists_reference_number,
			char *tmp_file_directory );

FILE *insert_database_open_tmp_file(
			char **tmp_file_name,
			char *tmp_file_directory,
			char *session );

void insert_database_remove_tmp_file(
			char *tmp_file_name );

int insert_database_execute_insert_mysql(
			char **message,
			char *application_name,
			char *folder_name,
			char *tmp_file_name,
			LIST *insert_attribute_name_list );

boolean build_insert_tmp_file_each_row(	
			FILE *insert_tmp_file,
			DICTIONARY *row_dictionary,
			char *application_name,
			LIST *primary_key_list,
			LIST *insert_required_attribute_name_list,
			LIST *attribute_name_list,
			LIST *ignore_attribute_name_list,
			LIST *mto1_related_folder_list,
			LIST *common_non_primary_key_list,
			LIST *attribute_list,
			boolean exists_reference_number );

boolean build_insert_tmp_file_row_zero(	
			FILE *insert_tmp_file,
			DICTIONARY *row_dictionary,
			char *application_name,
			LIST *primary_key_list,
			LIST *attribute_name_list,
			LIST *ignore_attribute_name_list,
			LIST *mto1_related_folder_list,
			LIST *common_non_primary_key_list,
			LIST *attribute_list,
			boolean exists_reference_number );

void insert_database_populate_missing_attribute_name_list(
			LIST **missing_attribute_name_list,
			int row,
			DICTIONARY *query_dictionary,
			LIST *ignore_attribute_name_list,
			LIST *check_attribute_name_list );

LIST *insert_database_get_missing_attribute_name_list(
			int row,
			DICTIONARY *row_dictionary,
			LIST *ignore_attribute_name_list,
			LIST *primary_key_list,
			LIST *insert_required_attribute_name_list );

boolean build_insert_data_string( 	
			char *destination,
			DICTIONARY *row_dictionary,
		     	int row,
			LIST *ignore_attribute_name_list,
			LIST *attribute_name_list,
			LIST *mto1_related_folder_list,
			LIST *common_non_primary_key_list,
			char *application_name,
			LIST *attribute_list );

void insert_database_dont_remove_tmp_file(
			INSERT_DATABASE *insert_database );

int insert_database_ignore_button_pressed(
			LIST *ignore_attribute_name_list,
			char *attribute_name );

void insert_database_set_post_insert_process(
			INSERT_DATABASE *insert_database,
			char *post_insert_process );

void insert_database_set_login_name(
			INSERT_DATABASE *insert_database,
			char *s );

INSERT_DATABASE *insert_database_calloc(
			char *application_name,
			char *session,
			char *folder_name );

void insert_database_execute_post_change_process_row_zero(
			char **message,
			char *application_name,
			char *session,
			DICTIONARY *row_dictionary,
			LIST *ignore_attribute_name_list,
			LIST *primary_key_list,
			LIST *insert_required_attribute_name_list,
			PROCESS *post_change_process,
			char *login_name,
			char *folder_name,
			char *role_name );

void insert_database_execute_post_change_process_each_row(
			char **message,
			char *application_name,
			char *session,
			DICTIONARY *row_dictionary,
			LIST *ignore_attribute_name_list,
			LIST *primary_key_list,
			LIST *insert_required_attribute_name_list,
			PROCESS *post_change_process,
			char *login_name,
			char *folder_name,
			char *role_name,
			LIST *mto1_related_folder_list,
			boolean exists_reference_number,
			LIST *attribute_name_list );

char *insert_database_get_common_data(
			DICTIONARY *dictionary,
			char *application_name,
			LIST *mto1_related_folder_list,
			char *attribute_name,
			int row );

RELATED_FOLDER *insert_database_get_common_data_related_folder(
			char *attribute_name,
			LIST *mto1_related_folder_list );

char *insert_database_get_fetch_common_data_where_clause_string( 	
			LIST *primary_key_list,
			LIST *primary_data_list );

char *insert_database_fetch_common_data(
			char *application_name,
			char *folder_name,
			char *attribute_name,
			LIST *primary_key_list,
			LIST *primary_data_list );

boolean insert_database_non_primary_attribute_populated(
			int row,
			DICTIONARY *dictionary,
			LIST *non_primary_key_list );

void insert_database_set_reference_number(
			DICTIONARY *dictionary,
			char *application_name,
			LIST *attribute_list,
			int row );

LIST *insert_database_get_trim_indices_dictionary_key_list(
			DICTIONARY *dictionary );

LIST *insert_database_set_ignore_primary_key_to_null(
			DICTIONARY *row_dictionary,
			LIST *ignore_attribute_name_list,
			LIST *primary_key_list );

LIST *insert_database_attribute_name_list(
			LIST *attribute_data_list );

LIST *insert_database_attribute_data_list( 	
			DICTIONARY *row_dictionary,
		     	int row,
			LIST *ignore_attribute_name_list,
			LIST *attribute_name_list,
			LIST *mto1_related_folder_list,
			LIST *common_non_primary_key_list,
			char *application_name,
			LIST *attribute_list );

void insert_database_direct_attribute_data_list(
			LIST *attribute_data_list,
			LIST *done_attribute_name_list,
			LIST *attribute_name_list,
			DICTIONARY *row_dictionary,
			int row,
			LIST *attribute_list );

void insert_database_set_attribute_data_list(
			LIST *attribute_data_list,
			LIST *done_attribute_name_list,
			char *attribute_name_string,
			char *data_string,
			int length,
			LIST *attribute_list );

char *insert_database_attribute_data_list_display(
			LIST *attribute_data_list );

#endif
