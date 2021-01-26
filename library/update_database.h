/* $APPASERVER_HOME/library/update_database.h				*/
/* -------------------------------------------------------------------- */
/* This is the appaserver update_database ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef UPDATE_DATABASE_H
#define UPDATE_DATABASE_H

#include <stdio.h>
#include "list.h"
#include "dictionary.h"
#include "process.h"
#include "related_folder.h"

/* Constants */
/* --------- */
/* #define UPDATE_DATABASE_DEBUG_MODE		1 */
#define UPDATE_DATABASE_PREUPDATE_PREFIX	"preupdate_"
#define UPDATE_DATABASE_NULL_TOKEN		"/"

/* Objects */
/* ------- */
typedef struct
{
	char *attribute_name;
	char *data;
} WHERE_ATTRIBUTE;

typedef struct
{
	char *folder_name;
	char *attribute_name;
	char *old_data;
	char *new_data;
} CHANGED_ATTRIBUTE;

typedef struct
{
	char *folder_name;
	LIST *where_attribute_list;
	LIST *changed_attribute_list;
	PROCESS *post_change_process;
	LIST *primary_attribute_name_list;
	boolean update_failed;
	LIST *one2m_related_folder_list;
	LIST *foreign_attribute_data_list;
} UPDATE_FOLDER;

typedef struct
{
	int row;
	LIST *update_folder_list;
	boolean changed_key;
} UPDATE_ROW;

typedef struct
{
	char *application_name;
	char *session;
	char *login_name;
	char *role_name;
	DICTIONARY *row_dictionary;
	DICTIONARY *file_dictionary;
	LIST *exclude_attribute_name_list;
	LIST *update_row_list;
	FOLDER *folder;
	LIST *mto1_isa_related_folder_list;
	DICTIONARY *foreign_attribute_dictionary;
	LIST *foreign_attribute_list;
	/* LIST *mto1_common_non_primary_attribute_related_folder_list; */
} UPDATE_DATABASE;

UPDATE_DATABASE *update_database_new(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *folder_name,
			DICTIONARY *row_dictionary,
			DICTIONARY *file_dictionary );

UPDATE_FOLDER *update_database_update_folder(
			boolean *changed_key,
			int row,
			DICTIONARY *row_dictionary,
			DICTIONARY *file_dictionary,
			FOLDER *folder,
			LIST *foreign_attribute_name_list,
			LIST *include_attribute_name_list,
			LIST *exclude_attribute_name_list,
			DICTIONARY *foreign_attribute_dictionary,
			LIST *additional_unique_index_attribute_name_list );

UPDATE_FOLDER *update_database_folder_foreign_update_folder(
			int row,
			DICTIONARY *row_dictionary,
			DICTIONARY *file_dictionary,
			FOLDER *folder,
			LIST *primary_attribute_name_list,
			LIST *folder_foreign_attribute_name_list );

LIST *update_database_update_row_list(
			DICTIONARY *row_dictionary,
			DICTIONARY *file_dictionary,
			FOLDER *folder,
			LIST *exclude_attribute_name_list,
			DICTIONARY *foreign_attribute_dictionary,
			LIST *foreign_attribute_list );

UPDATE_ROW *update_database_update_row_new(
			int row );

UPDATE_FOLDER *update_database_update_folder_new(
			char *folder_name );

WHERE_ATTRIBUTE *update_database_where_attribute_new(
			char *where_attribute_name,
			char *data );

CHANGED_ATTRIBUTE *update_database_changed_attribute_new(
			char *folder_name,
			char *attribute_name,
			char *attribute_datatype,
			char *old_data,
			char *new_data );

boolean update_database_exists_changed_key(
			LIST *primary_attribute_name_list,
			LIST *changed_attribute_list );

void update_database_build_where_clause(
			char *destination,
			LIST *where_attribute_list );

boolean update_database_get_dictionary_index_data(
			char **destination,
			DICTIONARY *dictionary,
			char *key,
			int index );

boolean update_database_get_dictionary_data(
			char **destination,
			DICTIONARY *dictionary,
			char *key );

void update_database_populate_common_non_primary_attributes(
			DICTIONARY *row_dictionary,
			char *application_name,
			DICTIONARY *file_dictionary,
			LIST *one2m_common_non_primary_related_folder_list,
			int row,
			char *login_name,
			LIST *exclude_attribute_name_list );

LIST *update_database_get_changed_attribute_name_list(
			LIST *changed_attribute_list );

LIST *update_database_get_changed_attribute_data_list(
			LIST *changed_attribute_list,
			LIST *changed_attribute_name_list );

LIST *update_database_get_new_changed_primary_data_list(
			LIST *primary_attribute_name_list,
			LIST *primary_changed_attribute_name_list,
			DICTIONARY *row_dictionary,
			DICTIONARY *file_dictionary,
			int row );
void update_database_populate_common_non_primary_data(
			DICTIONARY *row_dictionary,
			char *application_name,
			LIST *common_non_primary_attribute_name_list,
			char *folder_name,
			LIST *primary_attribute_name_list,
			LIST *new_changed_primary_data_list,
			int row );

UPDATE_FOLDER *update_database_update_folder_new(
			char *folder_name );

boolean update_database_get_index_data_if_changed(
			char **old_data,
			char **new_data,
			DICTIONARY *row_dictionary,
			DICTIONARY *file_dictionary,
			char *attribute_name, 
			int row );

LIST *update_database_where_attribute_list(
			DICTIONARY *file_dictionary,
			LIST *attribute_list,
			int row,
			DICTIONARY *foreign_attribute_dictionary );

void update_database_execute_for_row(
			char *error_messages,
			char *application_name,
			char *session,
			DICTIONARY *row_dictionary,
			char *login_name,
			LIST *update_folder_list,
			int row,
			char *role_name,
			LIST *additional_update_attribute_name_list,
			LIST *additional_update_data_list,
			boolean abort_if_first_update_failed );

char *update_database_execute_for_folder(
			char *application_name,
			char *login_name,
			char *folder_name,
			LIST *where_attribute_list,
			LIST *changed_attribute_list,
			LIST *primary_attribute_name_list,
			LIST *additional_update_attribute_name_list,
			LIST *additional_update_data_list );

void update_database_build_update_clause(
			char *destination,
			char *application_name,
			LIST *changed_attribute_list,
			LIST *primary_attribute_name_list );

void update_database_build_where_clause(
			char *destination,
			LIST *where_attribute_list );

UPDATE_ROW *update_database_update_row(
			int row,
			DICTIONARY *row_dictionary,
			DICTIONARY *file_dictionary,
			FOLDER *folder,
			LIST *exclude_attribute_name_list,
			DICTIONARY *foreign_attribute_dictionary,
			LIST *foreign_attribute_list );

LIST *update_database_folder_foreign_changed_attribute_list(
			DICTIONARY *row_dictionary,
			DICTIONARY *file_dictionary,
			int row,
			char *folder_name,
			LIST *primary_attribute_name_list,
			LIST *folder_foreign_attribute_name_list );

LIST *update_database_changed_attribute_list(
			boolean *changed_key,
			DICTIONARY *row_dictionary,
			DICTIONARY *file_dictionary,
			char *folder_name,
			LIST *attribute_list,
			int row,
			DICTIONARY *foreign_attribute_dictionary,
			LIST *include_attribute_name_list,
			LIST *exclude_attribute_name_list,
			LIST *additional_unique_index_attribute_name_list );

char *update_database_execute(
			char *application_name,
			char *session,
			LIST *update_row_list,
			DICTIONARY *row_dictionary,
			char *login_name,
			char *role_name,
			LIST *additional_update_attribute_name_list,
			LIST *additional_update_data_list,
			boolean abort_if_first_update_failed );

boolean update_database_get_dictionary_index_data(
			char **destination,
			DICTIONARY *dictionary,
			char *key,
			int index );

void update_database_set_each_mto1_isa_one2m_related_folder_list(
			LIST *mto1_isa_related_folder_list,
			char *application_name,
			char *session,
			char *role_name );

LIST *update_database_set_one2m_related_folder_list(
			LIST *one2m_related_folder_list,
			int row,
			DICTIONARY *row_dictionary,
			DICTIONARY *file_dictionary,
			LIST *exclude_attribute_name_list,
			DICTIONARY *foreign_attribute_dictionary );

void update_row_free(	UPDATE_ROW *update_row );

boolean update_database_changed_attribute(
			char *attribute_name,
			LIST *update_row_list );

char *update_database_update_row_list_display(
			LIST *update_row_list );

char *update_database_folder_display(
			UPDATE_FOLDER *update_folder );

char *update_database_folder_list_display(
			LIST *update_folder_list );

int update_database_changed_display(
			char *buffer_pointer,
			LIST *changed_attribute_list );

int update_database_where_display(
			char *buffer_pointer,
			LIST *where_attribute_list );

void update_database_set_login_name_each_row(
			DICTIONARY *dictionary,
			char *login_name );

int update_database_get_columns_updated(
			char *application_name,
			LIST *update_row_list );

LIST *update_database_get_changed_folder_name_list(
			LIST *update_row_list );

LIST *update_database_get_changed_attribute_name_list(
			LIST *changed_attribute_list );

CHANGED_ATTRIBUTE *update_database_get_folder_foreign(
			char *folder_name,
			char *attribute_name,
			char *datatype,
			char *old_data,
			char *new_data,
			LIST *folder_foreign_attribute_name_list,
			DICTIONARY *foreign_attribute_dictionary,
			LIST *foreign_attribute_list );

LIST *update_database_folder_foreign_where_attribute_list(
			DICTIONARY *file_dictionary,
			LIST *primary_attribute_name_list,
			int row,
			char *application_name,
			char *folder_name,
			LIST *folder_foreign_attribute_name_list );

#endif
