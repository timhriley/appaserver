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
	/* Input */
	/* ----- */
	DICTIONARY *post_dictionary;
	DICTIONARY *file_dictionary;
	char *attribute_name;
	char *attribute_datatype;
	int primary_key_index;
	int row;

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
	DICTIONARY *post_dictionary;
	DICTIONARY *file_dictionary;
	FOLDER_ATTRIBUTE *folder_attribute;
	int row;

	/* Process */
	/* ------- */
	UPDATE_CHANGED_ATTRIBUTE_DATA *update_changed_attribute_data;
	char *update_changed_attribute_preupdate_label;
} UPDATE_CHANGED_ATTRIBUTE;

typedef struct
{
	/* Input */
	/* ----- */
	DICTIONARY *post_dictionary;
	DICTIONARY *file_dictionary;
	FOLDER *folder;
	SECURITY_ENTITY *security_entity;
	int row;

	/* Process */
	/* ------- */
	LIST *primary_attribute_name_list;

	LIST *changed_attribute_list;

	/* ---------------------------------------------------- */
	/* For the primary folder,				*/
	/* build where using primary_attribute_name_list.	*/
	/* ---------------------------------------------------- */
	LIST *primary_attribute_name_list;

	/* ------------------------------------------------------------ */
	/* For the secondary folders,					*/
	/* build where using relation_foreign_attribute_name_list.	*/
	/* ------------------------------------------------------------ */
	LIST *relation_foreign_attribute_name_list;

	/* ---------------------------------------------------- */
	/* Where_attribute_name_list is either			*/
	/* primary_attribute_name_list or			*/
	/* relation_foreign_attribute_name_list			*/
	/* ---------------------------------------------------- */
	LIST *where_attribute_name_list;

	LIST *primary_data_list;
	LIST *where_attribute_list;

	char *where_clause;
	char *set_clause;
	boolean changed_primary_key;
	boolean update_failed;
	int count;
} UPDATE_FOLDER;

typedef struct
{
	int row;
	LIST *update_folder_list;
	boolean changed_primary_key;
} UPDATE_ROW;

typedef struct
{
	char *application_name;
	char *session;
	char *login_name;
	char *role_name;
	char *folder_name;
	FOLDER *folder;
	DICTIONARY *post_dictionary;
	DICTIONARY *file_dictionary;
	LIST *update_row_list;
	LIST *attribute_list;
	PROCESS *post_change_process;
	LIST *one2m_recursive_relation_list;
} UPDATE_DATABASE;

UPDATE_ROW *update_database_update_row_new(
			int row );

UPDATE_FOLDER *update_database_update_folder_new(
			char *folder_name );

WHERE_ATTRIBUTE *update_database_where_attribute_new(
			char *where_attribute_name,
			char *data );

CHANGED_ATTRIBUTE *update_changed_attribute_new(
			char *attribute_name,
			char *attribute_datatype,
			char *old_data,
			char *escaped_replaced_new_data );

UPDATE_FOLDER *update_database_update_folder_new(
			char *folder_name );

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

void update_database_set_login_name_each_row(
			DICTIONARY *dictionary,
			char *login_name );

LIST *update_folder_changed_attribute_list(
			boolean *changed_primary_key,
			/* ------------------------------------------ */
			/* Sets preupdate_$attribute_name for trigger */
			/* ------------------------------------------ */
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			LIST *attribute_list,
			int row );

CHANGED_ATTRIBUTE *update_changed_attribute(
			/* ------------------------------------------ */
			/* Sets preupdate_$attribute_name for trigger */
			/* ------------------------------------------ */
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			ATTRIBUTE *attribute,
			int row );

UPDATE_FOLDER *update_database_update_folder(
			/* ------------------------------------------ */
			/* Sets preupdate_$attribute_name for trigger */
			/* ------------------------------------------ */
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *folder_name,
			LIST *attribute_list,
			int row,
			PROCESS *post_change_process );

UPDATE_DATABASE *update_database_calloc(
			void );

UPDATE_FOLDER *update_folder_calloc(
			void );

LIST *update_folder_primary_data_list(
			DICTIONARY *file_dictionary,
			LIST *primary_attribute_name_list,
			int row );

UPDATE_ROW *update_database_update_row(
			/* ------------------------------------------ */
			/* Sets preupdate_$attribute_name for trigger */
			/* ------------------------------------------ */
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *folder_name,
			LIST *attribute_list,
			LIST *one2m_recursive_relation_list,
			PROCESS *post_change_process,
			int row );

UPDATE_ROW *update_database_update_row_calloc(
			void );

LIST *update_database_update_row_list(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *folder_name,
			LIST *attribute_list,
			LIST *one2m_recursive_relation_list,
			LIST *mto1_isa_recursive_relation_list,
			PROCESS *post_change_process );

UPDATE_FOLDER *update_secondary_update_folder(
			char *mto1_folder_name,
			LIST *foreign_attribute_name_list,
			LIST *primary_data_list,
			LIST *primary_changed_attribute_list );

UPDATE_FOLDER *update_folder_set_where_clause(
			UPDATE_FOLDER *update_folder );

LIST *update_folder_where_attribute_list(
			LIST *attribute_name_list,
			LIST *primary_data_list );

UPDATE_FOLDER *update_database_primary_update_folder(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *folder_name,
			LIST *attribute_list,
			PROCESS *post_change_process,
			int row );

UPDATE_DATABASE *update_database_new(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *folder_name,
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary );

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
char *update_changed_attribute_data_key(
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

#endif
