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

typedef struct
{
	char *primary_attribute_name;
	char *datatype_name;
	char *sql_injection_escape_file_data;
} UPDATE_WHERE_ATTRIBUTE;

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

typedef struct
{
	/* Input */
	/* ----- */
	char *attribute_name;
	char *datatype_name;
	int primary_key_index;
	int row;
	char *file_data;

	/* Process */
	/* ------- */
	char *post_data;
	char *sql_injection_escape_post_data;
	boolean attribute_changed;
} UPDATE_ATTRIBUTE_CHANGED;

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

UPDATE_ATTRIBUTE_CHANGED *update_attribute_changed_seek(
			char *attribute_name,
			LIST *update_attribute_changed_list );

typedef struct
{
	/* Input */
	/* ----- */
	FOLDER_ATTRIBUTE *folder_attribute;
	int row;

	/* Process */
	/* ------- */
	char *file_data;
	char *sql_injection_escape_file_data;
	UPDATE_ATTRIBUTE_CHANGED *update_attribute_changed;
	char *update_attribute_preupdate_label;
} UPDATE_ATTRIBUTE;

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
	LIST *one2m_list;
} UPDATE_MTO1_ISA;

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

LIST *update_mto1_isa_one2m_list(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *login_name,
			char *folder_name,
			int row );

LIST *update_mto1_isa_sql_statement_list(
			LIST *update_mto1_isa_list );

LIST *update_mto1_isa_command_line_list(
			LIST *update_mto1_isa_list );

LIST *update_mto1_isa_one2m_sql_statement_list(
			LIST *update_one2m_list );

LIST *update_mto1_isa_one2m_command_line_list(
			LIST *update_one2m_list );

typedef struct
{
	/* Process */
	/* ------- */
	LIST *update_attribute_list;
	LIST *update_where_attribute_list;
	LIST *primary_delimited_list;
	LIST *update_attribute_changed_list;
	LIST *sql_statement_list;
	LIST *command_line_list;
} UPDATE_ONE2M;

/* UPDATE_ONE2M operations */
/* ----------------------- */

/* Usage */
/* ----- */
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

/* Process */
/* ------- */
UPDATE_ONE2M *update_one2m_calloc(
			void );

LIST *update_one2m_primary_delimited_list(
			char *folder_table_name,
			LIST *primary_key_list,
			char *update_where_clause );

LIST *update_one2m_sql_statement_list(
			char *folder_table_name,
			LIST *update_attribute_changed_list,
			LIST *primary_key_list,
			LIST *primary_delimited_list );

LIST *update_one2m_command_line_list(
			char *command_line,
			char *login_name,
			LIST *primary_key_list,
			LIST *primary_delimited_list,
			LIST *update_attribute_changed_list );

/* Safely returns heap memory */
/* -------------------------- */
char *update_one2m_sql_statement(
			char *folder_table_name,
			LIST *update_attribute_changed_list,
			LIST *primary_key_list,
			LIST *primary_data_list );

/* Safely returns heap memory */
/* -------------------------- */
char *update_one2m_command_line(
			char *command_line,
			char *login_name,
			LIST *primary_key_list,
			LIST *primary_data_list,
			LIST *update_attribute_changed_list );

LIST *update_one2m_list_sql_statement_list(
			LIST *update_one2m_list );

LIST *update_one2m_list_command_line_list(
			LIST *update_one2m_list );

typedef struct
{
	LIST *update_attribute_list;
	LIST *update_attribute_changed_list;
	boolean changed_primary_key;
	LIST *update_where_attribute_list;
	LIST *update_changed_attribute_list;
	char *where_clause;
	char *update_sql_statement;
	char *update_command_line;
} UPDATE_ROOT;

/* UPDATE_ROOT operations */
/* ---------------------- */

/* Usage */
/* ----- */
UPDATE_ROOT *update_root_new(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *login_name,
			char *folder_name,
			LIST *folder_attribute_list,
			PROCESS *post_change_process,
			SECURITY_ENTITY *security_entity,
			int row );

/* Process */
/* ------- */
UPDATE_ROOT *update_root_calloc(
			void );

boolean update_root_changed_primary_key(
			LIST *update_attribute_changed_list );

char *update_root_where_clause(
			char *update_where_clause,
			SECURITY_ENTITY *security_entity,
			LIST *folder_attribute_list );

typedef struct
{
	/* Input */
	/* ----- */
	int row;

	/* Process */
	/* ------- */
	UPDATE_ROOT *update_root;
	LIST *update_one2m_list;
	LIST *update_mto1_isa_list;
	int cell_count;
} UPDATE_ROW;

/* UPDATE_ROW operations */
/* --------------------- */
UPDATE_ROW *update_row_calloc(
			void );

UPDATE_ROW *update_row_new(
			char *application_name,
			char *login_name,
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *folder_name,
			LIST *primary_key_list,
			LIST *folder_attribute_list,
			LIST *relation_one2m_recursive_list,
			LIST *relation_mto1_isa_list,
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

typedef struct
{
	LIST *list;
	int dictionary_highest_row;
	int cell_count;
} UPDATE_ROW_LIST;

/* UPDATE_ROW_LIST operations */
/* -------------------------- */

/* Usage */
/* ----- */
UPDATE_ROW_LIST *update_row_list_new(
			char *application_name,
			char *login_name,
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *folder_name,
			LIST *primary_key_list,
			LIST *folder_attribute_list,
			LIST *relation_mto1_isa_list,
			LIST *relation_one2m_recursive_list,
			PROCESS *post_change_process,
			SECURITY_ENTITY *security_entity );

/* Process */
/* ------- */
UPDATE_ROW_LIST *update_row_list_calloc(
			void );

/* Returns null if no updates */
/* -------------------------- */
UPDATE_ROW_LIST *update_row_list_new(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *login_name,
			char *folder_name,
			LIST *folder_attribute_list,
			LIST *relation_one2m_recursive_list,
			LIST *relation_mto1_isa_list,
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

typedef struct
{
	SECURITY_ENTITY *security_entity;
	UPDATE_ROW_LIST *update_row_list;
	UPDATE_SQL_STATEMENT_LIST *update_sql_statement_list;
} UPDATE;

/* UPDATE operations */
/* ----------------- */

/* Usage */
/* ----- */
UPDATE *update_new(	char *application_name,
			char *login_name,
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			ROLE *role,
			FOLDER *folder );

/* Process */
/* ------- */
UPDATE *update_calloc(	void );

/* Public */
/* ------ */

/* Returns heap memory or null */
/* --------------------------- */
char *update_sql_statement(
			char *folder_table_name,
			LIST *update_attribute_changed_list,
			char *update_where_clause );

char *update_command_line(
			char *command_line,
			char *login_name,
			LIST *update_attribute_list );

/* Returns message_string as heap memory */
/* ------------------------------------- */
char *update_root_execute(
			char *sql_statement );

FILE *update_non_root_sql_pipe(
			char *appaserver_error_filename );

#endif
