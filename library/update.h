/* $APPASERVER_HOME/library/update.h					*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef UPDATE_H
#define UPDATE_H

#include <stdio.h>
#include "list.h"
#include "dictionary.h"
#include "security_entity.h"
#include "related_folder.h"
#include "folder_attribute.h"
#include "folder.h"

/* Constants */
/* --------- */
/* #define UPDATE_DEBUG_MODE		1 */
#define UPDATE_PREUPDATE_PREFIX	"preupdate_"
#define UPDATE_NULL_TOKEN		"/"

/* Structures */
/* ---------- */
typedef struct
{
	char *attribute_name;
	char *data;
} UPDATE_WHERE_ATTRIBUTE;

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
	UPDATE_CHANGED_ATTRIBUTE_DATA *data;
	char *update_changed_attribute_preupdate_label;
} UPDATE_CHANGED_ATTRIBUTE;

typedef struct
{
	/* Input */
	/* ----- */
	LIST *primary_changed_attribute_list;
	LIST *primary_where_attribute_list;
	RELATION *relation_one2m;

	/* Process */
	/* ------- */
	LIST *foreign_changed_attribute_list;
	LIST *foreign_where_attribute_list;
	LIST *folder_delimited_list;
} UPDATE_ONE2M;

typedef struct
{
	/* Input */
	/* ----- */
	LIST *primary_changed_attribute_list;
	LIST *primary_where_attribute_list;
	RELATION *relation_mto1;

	/* Process */
	/* ------- */
	LIST *foreign_changed_attribute_list;
	LIST *foreign_where_attribute_list;
	LIST *folder_delimited_list;
} UPDATE_MTO1;

typedef struct
{
	/* Process */
	/* ------- */
	LIST *primary_changed_attribute_list;
	LIST *primary_where_attribute_list;
	char *security_entity_where_clause;
	char *folder_delimited;
	LIST *primary_key_changed_attribute_list;
	char *primary_set_clause;
} UPDATE_PRIMARY;

typedef struct
{
	/* Input */
	/* ----- */
	int row;

	/* Process */
	/* ------- */
	UPDATE_PRIMARY *update_primary;
	LIST *one2m_list;
	LIST *mto1_list;
	int update_row_cell_count;
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
} UPDATE;

/* UPDATE operations */
/* ----------------- */
UPDATE *update_calloc(
			void );

UPDATE *update(
			/* ------------------------------------------ */
			/* Sets preupdate_$attribute_name for trigger */
			/* ------------------------------------------ */
			DICTIONARY *post_dictionary /* in/out */,
			DICTIONARY *file_dictionary,
			char *login_name,
			char *role_name,
			char *folder_name );

int update_cell_count(
			LIST *update_row_list );

/* Returns mysql_message_list_string as heap memory */
/* ------------------------------------------------ */
char *update_row_list_sql(
			char *login_name,
			SECURITY_ENTITY *security_entity,
			LIST *update_row_list );

char *update_set_clause(
			LIST *changed_attribute_list );

char *update_where_clause(
			LIST *where_attribute_list );

/* Returns mysql_message as heap memory */
/* ------------------------------------ */
char *update_folder_sql(
			char *login_name,
			char *table_name,
			char *update_set_clause,
			char *update_security_where_clause,
			char *process_update_command_line );

char *update_post_change_process(
			char *command_line );

LIST *update_data_list( LIST *changed_attribute_list );

/* UPDATE_ROW operations */
/* --------------------- */
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

/* Returns mysql_message_list_string as heap memory */
/* ------------------------------------------------ */
char *update_row_sql(
			char *login_name,
			SECURITY_ENTITY *security_entity,
			UPDATE_ROW *update_row );

/* UPDATE_PRIMARY operations */
/* ------------------------- */
UPDATE_PRIMARY *update_primary_calloc(
			void );

UPDATE_PRIMARY *update_primary(
			/* ------------------------------------------ */
			/* Sets preupdate_$attribute_name for trigger */
			/* ------------------------------------------ */
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			FOLDER *folder,
			SECURITY_ENTITY *security_entity,
			int row );

LIST *update_primary_changed_attribute_list(
			/* ------------------------------------------ */
			/* Sets preupdate_$attribute_name for trigger */
			/* ------------------------------------------ */
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			LIST *folder_attribute_list,
			int row );

LIST *update_primary_where_attribute_list(
			DICTIONARY *file_dictionary,
			LIST *primary_key_list,
			int row );

LIST *update_primary_key_changed_attribute_list(
			LIST *primary_changed_attribute_list );

char *update_primary_set_clause(
			LIST *primary_changed_attribute_list );

/* UPDATE_FOREIGN operations */
/* ------------------------- */
LIST *update_foreign_changed_attribute_list(
			LIST *primary_key_changed_attribute_list,
			LIST *foreign_key_list );

LIST *update_foreign_where_attribute_list(
			LIST *primary_key_changed_attribute_list,
			LIST *foreign_key_list );

/* UPDATE_ONE2M operations */
/* ----------------------- */
UPDATE_ONE2M *update_one2m_calloc(
			void );

UPDATE_ONE2M *update_one2m(
			LIST *primary_key_changed_attribute_list,
			LIST *primary_where_attribute_list,
			RELATION *relation_one2m );

/* UPDATE_MTO1 operations */
/* ----------------------- */
UPDATE_MTO1 *update_mto1_calloc(
			void );

UPDATE_MTO1 *update_one2m(
			LIST *primary_key_changed_attribute_list,
			LIST *primary_where_attribute_list,
			RELATION *relation_mto1 );

LIST *update_mto1_changed_attribute_list(
			LIST *primary_key_changed_attribute_list,
			char *one_folder_name,
			LIST *foreign_attribute_name_list );

/* UPDATE_CHANGED_ATTRIBUTE operations */
/* ----------------------------------- */
UPDATE_CHANGED_ATTRIBUTE *update_changed_attribute(
			/* ------------------------------------------ */
			/* Sets preupdate_$attribute_name for trigger */
			/* ------------------------------------------ */
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			FOLDER_ATTRIBUTE *folder_attribute,
			int row );

UPDATE_CHANGED_ATTRIBUTE *update_changed_attribute_new(
			char *folder_name,
			char *attribute_name );

UPDATE_CHANGED_ATTRIBUTE *update_changed_attribute_calloc(
			void );

/* Returns heap memory */
/* ------------------- */
char *update_changed_attribute_preupdate_label(
			char *attribute_name,
			int row );

LIST *update_primary_changed_attribute_list(
			LIST *changed_attribute_list );

/* UPDATE_CHANGED_ATTRIBUTE_DATA operations */
/* ---------------------------------------- */
UPDATE_CHANGED_ATTRIBUTE_DATA *update_changed_attribute_data_calloc(
			void );

UPDATE_CHANGED_ATTRIBUTE_DATA *update_changed_attribute_data_new(
			char *key,
			char *old_data,
			char *escaped_replaced_new_data );

UPDATE_CHANGED_ATTRIBUTE_DATA *update_changed_attribute_data(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *attribute_name,
			char *attribute_datatype,
			int primary_key_index,
			int row );


/* UPDATE_WHERE_ATTRIBUTE operations */
/* --------------------------------- */
UPDATE_WHERE_ATTRIBUTE *update_where_attribute_calloc(
			void );

UPDATE_WHERE_ATTRIBUTE *update_where_attribute(
			DICTIONARY *file_dictionary,
			char *primary_key,
			int row );

LIST *update_foreign_where_attribute_list(
			LIST *primary_key_changed_attribute_list,
			LIST *foreign_key_list );

char *update_where_clause(
			LIST *where_attribute_list );

#endif
