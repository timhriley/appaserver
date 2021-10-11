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
#include "security.h"
#include "relation.h"
#include "folder_attribute.h"
#include "folder.h"

/* Constants */
/* --------- */
#define UPDATE_PREUPDATE_PREFIX		"preupdate_"

/* Structures */
/* ---------- */
typedef struct
{
	/* Attributes */
	/* ---------- */
	char *primary_attribute_name;
	char *datatype_name;
	char *sql_injection_escape_file_data;
} UPDATE_WHERE_ATTRIBUTE;

typedef struct
{
	/* Input */
	/* ----- */
	char *attribute_name;
	char *datatype_name;
	int primary_key_index;
	char *file_data;
	char *sql_injection_escape_file_data;
	int row;

	/* Process */
	/* ------- */
	char *post_data;
	char *sql_injection_escape_post_data;
	boolean attribute_changed;
} UPDATE_ATTRIBUTE_CHANGED;

typedef struct
{
	/* Input */
	/* ----- */
	FOLDER_ATTRIBUTE *folder_attribute;
	int row;

	/* Process */
	/* ------- */
	char *file_data;
	UPDATE_ATTRIBUTE_CHANGED *update_attribute_changed;
	char *update_attribute_preupdate_label;
} UPDATE_ATTRIBUTE;

typedef struct
{
	/* Process */
	/* ------- */
	LIST *update_attribute_list;
	LIST *update_attribute_changed_list;
	LIST *update_where_attribute_list;
	char *update_where_clause;
	char *update_sql_statement;
	char *update_command_line;
} UPDATE_MTO1_ISA;

typedef struct
{
	/* Process */
	/* ------- */
	LIST *update_attribute_list;
	LIST *update_attribute_changed_list;
	LIST *update_where_attribute_list;
	char *update_where_clause;
	char *update_sql_statement;
	char *update_command_line;
} UPDATE_ONE2M;

typedef struct
{
	/* Process */
	/* ------- */
	LIST *update_attribute_list;
	LIST *update_attribute_changed_list;
	LIST *update_where_attribute_list;
	LIST *update_changed_attribute_list;
	char *update_where_clause;
	char *update_root_where_clause;
	char *update_sql_statement;
	char *update_command_line;
	boolean changed_primary_key;
} UPDATE_ROOT;

typedef struct
{
	/* Input */
	/* ----- */
	int row;

	/* Process */
	/* ------- */
	UPDATE_ROOT *update_root;
	LIST *update_mto1_isa_list;
	LIST *update_one2m_list;
	int cell_count;
	LIST *sql_statement_list;
	LIST *command_line_list;
} UPDATE_ROW;

typedef struct
{
	/* Process */
	/* ------- */
	LIST *list;
	int dictionary_highest_row;
	int cell_count;
	LIST *command_line_list;

	/* Output */
	/* ------ */
	char *message_list_string;
} UPDATE_ROW_LIST;

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
	ROLE *role;
	FOLDER *folder;
	SECURITY_ENTITY *security_entity;
	UPDATE_ROW_LIST *update_row_list;
} UPDATE;

/* UPDATE operations */
/* ----------------- */
UPDATE *update_calloc(	void );

UPDATE *update_new(
			/* ------------------------------------------ */
			/* Sets preupdate_$attribute_name for trigger */
			/* ------------------------------------------ */
			DICTIONARY *post_dictionary /* in/out */,
			DICTIONARY *file_dictionary,
			char *login_name,
			char *role_name,
			char *folder_name );

/* Returns heap memory or null */
/* --------------------------- */
char *update_sql_statement(
			char *folder_table_name,
			LIST *update_attribute_changed_list,
			char *update_where_clause );

char *update_command_line(
			PROCESS *post_change_process,
			char *login_name,
			LIST *update_attribute_list );

/* Returns message_string as heap memory */
/* ------------------------------------- */
char *update_execute(	char *sql_statement );

FILE *update_sql_pipe(	char *application_name );

void update_sql_statement_list_execute(
			FILE *update_sql_pipe,
			LIST *sql_statement_list );

/* UPDATE_ROW_LIST operations */
/* -------------------------- */
UPDATE_ROW_LIST *update_row_list_calloc(
			void );

/* Returns null if no updates */
/* -------------------------- */
UPDATE_ROW_LIST *update_row_list_new(
			/* ------------------------------------------ */
			/* Sets preupdate_$attribute_name for trigger */
			/* ------------------------------------------ */
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *login_name,
			char *folder_name,
			LIST *folder_attribute_list,
			LIST *relation_mto1_isa_list,
			LIST *relation_one2m_recursive_list,
			PROCESS *post_change_process,
			SECURITY_ENTITY *security_entity );

int update_row_list_cell_count(
			UPDATE_ROW_LIST *update_row_list );

char *update_row_list_execute(
			UPDATE_ROW_LIST *update_row_list );

LIST *update_row_list_sql_statement_list(
			UPDATE_ROW_LIST *update_row_list );

LIST *update_row_list_command_line_list(
			UPDATE_ROW_LIST *update_row_list );

void update_row_list_command_line_execute(
			LIST *command_line_list );

/* UPDATE_ROW operations */
/* --------------------- */
UPDATE_ROW *update_row_calloc(
			void );

UPDATE_ROW *update_row_new(
			/* ------------------------------------------ */
			/* Sets preupdate_$attribute_name for trigger */
			/* ------------------------------------------ */
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *login_name,
			char *folder_name,
			LIST *folder_attribute_list,
			LIST *relation_mto1_isa_list,
			LIST *relation_one2m_recursive_list,
			PROCESS *post_change_process,
			SECURITY_ENTITY *security_entity,
			int row );

int update_row_cell_count(
			UPDATE_ROOT *update_root,
			LIST *update_mto1_isa_list,
			LIST *update_one2m_list );

LIST *update_row_sql_statement_list(
			UPDATE_ROOT *update_root,
			LIST *update_mto1_isa_list,
			LIST *update_one2m_list );

LIST *update_row_command_line_list(
			UPDATE_ROOT *update_root,
			LIST *update_mto1_isa_list,
			LIST *update_one2m_list );

/* UPDATE_ROOT operations */
/* ---------------------- */
UPDATE_ROOT *update_root_calloc(
			void );

UPDATE_ROOT *update_root_new(
			/* ------------------------------------------ */
			/* Sets preupdate_$attribute_name for trigger */
			/* ------------------------------------------ */
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *login_name,
			char *folder_name,
			LIST *folder_attribute_list,
			PROCESS *post_change_process,
			SECURITY_ENTITY *security_entity,
			int row );

char *update_root_where_clause(
			char *update_where_clause,
			SECURITY_ENTITY *security_entity );

/* UPDATE_MTO1_ISA operations */
/* -------------------------- */
LIST *update_mto1_isa_list(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *login_name,
			LIST *relation_mto1_isa_list,
			int row );

UPDATE_MTO1_ISA *update_mto1_isa_new(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *login_name,
			RELATION *relation_mto1_isa,
			int row );

UPDATE_MTO1_ISA *update_mto1_isa_calloc(
			void );

LIST *update_mto1_isa_sql_statement_list(
			LIST *update_mto1_isa_list );

LIST *update_mto1_isa_command_line_list(
			LIST *update_mto1_isa_list );

/* UPDATE_ONE2M operations */
/* ----------------------- */
UPDATE_ONE2M *update_one2m_calloc(
			void );

LIST *update_one2m_list(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *login_name,
			LIST *relation_one2m_recursive_list,
			int row );

UPDATE_ONE2M *update_one2m_new(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *login_name,
			RELATION *relation_one2m,
			int row );

LIST *update_one2m_sql_statement_list(
			LIST *update_one2m_list );

LIST *update_one2m_command_line_list(
			LIST *update_one2m_list );

/* UPDATE_ATTRIBUTE operations */
/* --------------------------- */
LIST *update_attribute_list(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			LIST *folder_attribute_list,
			int row );

UPDATE_ATTRIBUTE *update_attribute_calloc(
			void );

UPDATE_ATTRIBUTE *update_attribute_new(
			/* ------------------------------------------ */
			/* Sets preupdate_$attribute_name for trigger */
			/* ------------------------------------------ */
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			FOLDER_ATTRIBUTE *folder_attribute,
			int row );

/* Returns heap memory */
/* ------------------- */
char *update_attribute_preupdate_label(
			char *attribute_name,
			int row );

LIST *update_attribute_changed_list(
			LIST *update_attribute_list );

/* UPDATE_ATTRIBUTE_CHANGED operations */
/* ----------------------------------- */
UPDATE_ATTRIBUTE_CHANGED *update_attribute_changed_calloc(
			void );

UPDATE_ATTRIBUTE_CHANGED *update_attribute_changed_new(
			DICTIONARY *post_dictionary,
			char *attribute_name,
			char *datatype_name,
			int primary_key_index,
			char *file_data,
			int row );

boolean update_attribute_changed_primary_key(
			LIST *update_attribute_changed_list );

/* UPDATE_WHERE_ATTRIBUTE operations */
/* --------------------------------- */
LIST *update_where_attribute_list(
			LIST *update_attribute_list );

UPDATE_WHERE_ATTRIBUTE *update_where_attribute_calloc(
			void );

UPDATE_WHERE_ATTRIBUTE *update_where_attribute_new(
			char *primary_attribute_name,
			char *datatype_name,
			char *sql_injection_escape_file_data );

/* Returns heap memory or null */
/* --------------------------- */
char *update_where_clause(
			LIST *update_where_attribute_list );

#endif
