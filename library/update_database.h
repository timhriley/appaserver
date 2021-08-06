/* $APPASERVER_HOME/library/update_database.h				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef UPDATE_DATABASE_H
#define UPDATE_DATABASE_H

#include <stdio.h>
#include "list.h"
#include "dictionary.h"
#include "security_entity.h"
#include "related_folder.h"
#include "folder_attribute.h"
#include "folder.h"

/* Constants */
/* --------- */
/* #define UPDATE_DATABASE_DEBUG_MODE		1 */
#define UPDATE_DATABASE_PREUPDATE_PREFIX	"preupdate_"
#define UPDATE_DATABASE_NULL_TOKEN		"/"

/* Structures */
/* ---------- */
typedef struct
{
	char *attribute_name;
	char *data;
} WHERE_ATTRIBUTE;

typedef struct
{
	/* Process */
	/* ------- */
	char *key;
	char *old_data;
	char *new_data;
	boolean changed_attribute;
	char *escaped_replaced_new_data;
} UPDATE_CHANGED_ATTRIBUTE_DATA;

typedef struct
{
	/* Input */
	/* ----- */
	FOLDER_ATTRIBUTE *folder_attribute;

	/* Process */
	/* ------- */
	UPDATE_CHANGED_ATTRIBUTE_DATA *update_changed_attribute_data;
	char *update_changed_attribute_preupdate_label;
} UPDATE_CHANGED_ATTRIBUTE;

typedef struct
{
	LIST *related_changed_attribute_list;
	LIST *update_related_delimited_list;
} UPDATE_FOLDER_RELATED;

typedef struct
{
	/* Process */
	/* ------- */
	LIST *changed_attribute_list;
	LIST *where_attribute_list;
	char *where_clause;
	char *security_entity_where_clause;
	char *set_clause;
	char *folder_delimited_fetch;
} UPDATE_FOLDER_PRIMARY;

typedef struct
{
	/* Input */
	/* ----- */
	int row;

	/* Process */
	/* ------- */
	UPDATE_FOLDER_PRIMARY *update_folder_primary;
	LIST *primary_changed_attribute_list;
	LIST *folder_related_list;
	int update_row_count;
} UPDATE_ROW;

typedef struct
{
	/* Input */
	/* ----- */
	char *login_name;
	char *role_name;
	char *folder_name;
	DICTIONARY *post_dictionary;
	DICTIONARY *file_dictionary;
	char *application_name;

	/* Process */
	/* ------- */
	FOLDER *folder;
	LIST *update_row_list;
	LIST *attribute_list;
	PROCESS *post_change_process;
	LIST *one2m_recursive_relation_list;
} UPDATE_DATABASE;

WHERE_ATTRIBUTE *update_where_attribute(
			char *attribute_name,
			char *data );

UPDATE_FOLDER_PRIMARY *update_folder_primary(
			/* ------------------------------------------ */
			/* Sets preupdate_$attribute_name for trigger */
			/* ------------------------------------------ */
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			FOLDER *folder,
			SECURITY_ENTITY *security_entity,
			int row );

UPDATE_FOLDER_PRIMARY *update_folder_primary_calloc(
			void );

boolean update_database_data_if_changed(
			char **old_data,
			char **new_data,
			/* ------------------------------------------------- */
			/* Set the preupdate_$attribute_name for the trigger */
			/* ------------------------------------------------- */
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *attribute_name, 
			int row );

void update_database_execute_row(
			char *error_messages,
			char *application_name,
			char *session,
			DICTIONARY *post_dictionary,
			char *login_name,
			LIST *update_folder_list,
			int row,
			char *role_name,
			LIST *additional_update_attribute_name_list,
			LIST *additional_update_data_list,
			boolean abort_if_first_update_failed );

char *update_database_execute_folder(
			char *application_name,
			char *login_name,
			char *folder_name,
			char *set_clause,
			char *where_clause,
			LIST *additional_update_attribute_name_list,
			LIST *additional_update_data_list );

void update_database_build_update_clause(
			char *destination,
			char *application_name,
			LIST *changed_attribute_list,
			LIST *primary_attribute_name_list );

char *update_database_execute(
			char *application_name,
			char *session,
			LIST *update_row_list,
			DICTIONARY *post_dictionary,
			char *login_name,
			char *role_name,
			LIST *additional_update_attribute_name_list,
			LIST *additional_update_data_list,
			boolean abort_if_first_update_failed );

boolean update_database_dictionary_index_data(
			char **destination,
			DICTIONARY *dictionary,
			char *key,
			int index );

void update_database_set_each_mto1_isa_one2m_related_folder_list(
			LIST *mto1_isa_related_folder_list,
			char *application_name,
			char *session,
			char *role_name );

void update_database_set_one2m_related_folder_list(
			LIST *update_folder_list,
			LIST *one2m_related_folder_list,
			int row,
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			LIST *exclude_attribute_name_list,
			DICTIONARY *foreign_attribute_dictionary );

void update_row_free(	UPDATE_ROW *update_row );

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

CHANGED_ATTRIBUTE *update_changed_attribute(
			/* ------------------------------------------ */
			/* Sets preupdate_$attribute_name for trigger */
			/* ------------------------------------------ */
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			FOLDER_ATTRIBUTE *folder_attribute,
			int row );

UPDATE_DATABASE *update_database_calloc(
			void );

UPDATE_FOLDER *update_folder_calloc(
			void );

LIST *update_folder_primary_data_list(
			DICTIONARY *file_dictionary,
			LIST *primary_attribute_name_list,
			int row );

UPDATE_ROW *update_row(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			FOLDER *folder,
			SECURITY_ENTITY *security_entity,
			int row );

UPDATE_ROW *update_row_calloc(
			void );

LIST *update_row_list(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			FOLDER *folder,
			SECURITY_ENTITY *security_entity );

UPDATE_FOLDER *update_secondary_update_folder(
			char *mto1_folder_name,
			LIST *foreign_attribute_name_list,
			LIST *primary_data_list,
			LIST *primary_changed_attribute_list );

UPDATE_FOLDER *update_folder_set_where_clause(
			UPDATE_FOLDER *update_folder );

LIST *update_where_attribute_list(
			DICTIONARY *file_dictionary,
			LIST *primary_attribute_name_list,
			int row );

UPDATE_FOLDER *update_database_primary_update_folder(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *folder_name,
			LIST *attribute_list,
			PROCESS *post_change_process,
			int row );

UPDATE_DATABASE *update_database(
			/* ------------------------------------------ */
			/* Sets preupdate_$attribute_name for trigger */
			/* ------------------------------------------ */
			DICTIONARY *post_dictionary /* in/out */,
			DICTIONARY *file_dictionary,
			char *login_name,
			char *role_name,
			char *folder_name );

LIST *update_database_changed_folder_name_list(
			LIST *update_row_list );

LIST *update_database_where_attribute_name_list(
			LIST *primary_attribute_name_list,
			LIST *relation_foreign_attribute_name_list );

LIST *update_database_update_folder_list(
			LIST *primary_data_list,
			LIST *primary_changed_attribute_list,
			boolean changed_primary_key,
			LIST *one2m_recursive_relation_list,
			UPDATE_FOLDER *primary_update_folder );

char *update_folder_set_clause(
			LIST *changed_attribute_list );

int update_folder_count(
			char *folder_name,
			char *where_clause );

int update_database_cells_updated(
			LIST *update_row_list );

int update_folder_columns_updated(
			LIST *changed_attribute_list );

CHANGED_ATTRIBUTE *update_database_changed_attribute(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			ATTRIBUTE *attribute,
			int row );

boolean update_database_attribute_exists(
			char *attribute_name,
			LIST *update_row_list );

UPDATE_CHANGED_ATTRIBUTE_DATA *update_changed_attribute_data_calloc(
			void );

UPDATE_CHANGED_ATTRIBUTE_DATA *update_changed_attribute_data(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *attribute_name,
			char *attribute_datatype,
			int primary_key_index,
			int row );

/* Returns static memory */
/* --------------------- */
char *update_dictionary_key(
			char *attribute_name,
			int row );

UPDATE_CHANGED_ATTRIBUTE *update_changed_attribute_calloc(
			void );

UPDATE_CHANGED_ATTRIBUTE *update_changed_attribute(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			FOLDER_ATTRIBUTE *folder_attribute,
			int row );

/* Returns heap memory */
/* ------------------- */
char *update_changed_attribute_preupdate_label(
			char *attribute_name,
			int row );

LIST *update_folder_related_list(
			LIST *primary_attribute_name_list,
			LIST *primary_changed_attribute_list,
			LIST *where_attribute_list,
			LIST *relation_one2m_recursive_list );

UPDATE_FOLDER_RELATED *update_folder_related_calloc(
			void );

UPDATE_FOLDER_RELATED *update_folder_related(
			LIST *primary_attribute_name_list,
			LIST *primary_changed_attribute_list,
			LIST *where_attribute_list,
			RELATION *relation_one2m );

#endif
