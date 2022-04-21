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

#define UPDATE_PREUPDATE_PREFIX	"preupdate_"

typedef struct
{
	/* Attribute */
	/* --------- */
	FOLDER_ATTRIBUTE *folder_attribute;

	/* Process */
	/* ------- */
	char *post_data;
	char *sql_injection_escape_post_data;
	char *file_data;
	char *sql_injection_escape_file_data;
} UPDATE_ATTRIBUTE;

/* UPDATE_ATTRIBUTE operations */
/* --------------------------- */

/* Usage */
/* ----- */
LIST *update_attribute_list(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			LIST *folder_attribute_append_isa_list,
			int row );

UPDATE_ATTRIBUTE *update_attribute_new(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			FOLDER_ATTRIBUTE *folder_attribute,
			int row );

/* Process */
/* ------- */
UPDATE_ATTRIBUTE *update_attribute_calloc(
			void );

/* Public */
/* ------ */
UPDATE_ATTRIBUTE *update_attribute_seek(
			char *attribute_name,
			LIST *update_attribute_list );

LIST *update_attribute_data_list(
			LIST *update_attribute_list );

typedef struct
{
	UPDATE_ATTRIBUTE *update_attribute;
	char *clause;
} UPDATE_WHERE;

/* UPDATE_WHERE operations */
/* ----------------------- */

/* Usage */
/* ----- */
LIST *update_where_list(
			LIST *folder_attribute_primary_key_list,
			LIST *update_attribute_list );

UPDATE_WHERE *update_where_new(
			char *primary_key,
			LIST *update_attribute_list );

/* Process */
/* ------- */
UPDATE_WHERE *update_where_calloc(
			void );

/* Returns heap memory */
/* ------------------- */
char *update_where_clause(
			char *key,
			char *datatype_name,
			char *sql_injection_escape_file_data );

/* Public */
/* ------ */

/* Returns heap memory.			  */
/* Executes free( update_where->clause ). */
/* -------------------------------------- */
char *update_where_list_clause(
			LIST *update_where_list );

/* Returns heap memory */
/* ------------------- */
char *update_where_primary_data_list_string(
			LIST *update_where_list,
			char sql_delimiter );

typedef struct
{
	UPDATE_ATTRIBUTE *update_attribute;
	boolean update_changed_boolean;
	char *set_clause;
} UPDATE_CHANGED;

/* UPDATE_CHANGED operations */
/* ------------------------- */

/* Usage */
/* ----- */

LIST *update_changed_list(
			LIST *folder_attribute_append_isa_name_list,
			LIST *update_attribute_list );

UPDATE_CHANGED *update_changed_new(
			char *attribute_name,
			LIST *update_attribute_list );

/* Process */
/* ------- */
UPDATE_CHANGED *update_changed_calloc(
			void );

boolean update_changed_boolean(
			char *sql_injection_escape_file_data,
			char *sql_injection_escape_post_data );

/* Returns heap memory */
/* ------------------- */
char *update_changed_set_clause(
			char *attribute_name,
			char *datatype_name,
			char *sql_injection_escape_post_data );

/* Public */
/* ------ */

/* Returns heap memory or null.		 	*/
/* Executes free( update_changed->set_clause ). */
/* -------------------------------------------- */
char *update_changed_list_set_clause(
			LIST *update_changed_list );

boolean update_changed_primary_key(
			LIST *update_changed_list );

LIST *update_changed_primary_post_data_list(
			LIST *update_changed_list );

LIST *update_changed_primary_file_data_list(
			LIST *update_changed_list );

UPDATE_CHANGED *update_changed_seek(
			char *attribute_name,
			LIST *update_changed_list );

typedef struct
{
	LIST *update_attribute_list;
} UPDATE_ONE2M_ROW;

/* UPDATE_ONE2M_ROW operations */
/* --------------------------- */

/* Usage */
/* ----- */
UPDATE_ONE2M_ROW *update_one2m_row_new(
			char *many_folder_name,
			LIST *primary_key_list,
			LIST *update_one2m_changed_list,
			LIST *primary_data_list );

/* Process */
/* ------- */
UPDATE_ONE2M_ROW *update_one2m_row_calloc(
			void );

/* Public */
/* ------ */

/* Returns heap memory */
/* ------------------- */
char *update_one2m_row_where(
			UPDATE_ONE2M_ROW *update_one2m_row );

typedef struct
{
	LIST *fetch_list;
	LIST *list;
} UPDATE_ONE2M_ROW_LIST;

/* UPDATE_ONE2M_ROW_LIST operations */
/* -------------------------------- */

/* Usage */
/* ----- */
UPDATE_ONE2M_ROW_LIST *update_one2m_row_list_new(
			char *application_name,
			char *many_folder_name,
			LIST *primary_key_list,
			LIST *update_one2m_changed_list,
			char *update_where_list_clause );

/* Process */
/* ------- */
UPDATE_ONE2M_ROW_LIST *update_one2m_row_list_calloc(
			void );

LIST *update_one2m_row_list_fetch_list(
			char *folder_table_name,
			LIST *primary_key_list,
			char *update_where_list_clause );

typedef struct
{
	LIST *changed_list;
	LIST *where_list;
	char *update_where_list_clause;
	UPDATE_ONE2M_ROW_LIST *update_one2m_row_list;
	LIST *sql_statement_list;
	LIST *command_line_list;
} UPDATE_ONE2M;

/* UPDATE_ONE2M operations */
/* ----------------------- */

/* Usage */
/* ----- */
LIST *update_one2m_list(
			char *application_name,
			char *login_name,
			char *appaserver_error_filename,
			LIST *update_changed_list,
			LIST *update_where_list,
			LIST *relation_one2m_recursive_list );

UPDATE_ONE2M *update_one2m_new(
			char *application_name,
			char *login_name,
			char *appaserver_error_filename,
			LIST *update_changed_list,
			LIST *update_where_list,
			RELATION *relation_one2m );

/* Process */
/* ------- */
UPDATE_ONE2M *update_one2m_calloc(
			void );

LIST *update_one2m_changed_list(
			char *many_folder_name,
			LIST *update_changed_list,
			LIST *foreign_key_list );

LIST *update_one2m_where_list(
			char *many_folder_name,
			LIST *update_where_list,
			LIST *foreign_key_list );

LIST *update_one2m_sql_statement_list(
			char *folder_table_name,
			char *update_changed_list_set_clause,
			UPDATE_ONE2M_ROW_LIST *update_one2m_row_list );

LIST *update_one2m_command_line_list(
			char *command_line,
			char *login_name,
			char *appaserver_error_filename,
			UPDATE_ONE2M_ROW_LIST *update_one2m_row_list );

/* Private */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *update_one2m_sql_statement(
			char *folder_table_name,
			char *update_changed_list_set_clause,
			char *update_one2m_row_where );

/* Returns heap memory */
/* ------------------- */
char *update_one2m_command_line(
			char *command_line,
			char *login_name,
			char *appaserver_error_filename,
			UPDATE_ONE2M_ROW *update_one2m_row,
			char *appaserver_update_state,
			char *update_preupdate_prefix );

/* Returns list_get( foreign_key_list ) */
/* ------------------------------------ */
char *update_one2m_changed_attribute_name(
			int primary_key_index,
			LIST *foreign_key_list );

typedef struct
{
	LIST *update_changed_list;
	LIST *update_where_list;
	char *update_sql_statement;
	char *update_command_line;
	LIST *update_one2m_list;
} UPDATE_MTO1_ISA;

/* UPDATE_MTO1_ISA operations */
/* -------------------------- */
LIST *update_mto1_isa_list(
			char *application_name,
			char *login_name,
			char *appaserver_error_filename,
			LIST *update_attribute_list,
			boolean update_changed_primary_key,
			LIST *relation_mto1_isa_list );

UPDATE_MTO1_ISA *update_mto1_isa_new(
			char *application_name,
			char *login_name,
			char *appaserver_error_filename,
			LIST *update_attribute_list,
			boolean update_changed_primary_key,
			RELATION *relation_mto1_isa );

UPDATE_MTO1_ISA *update_mto1_isa_calloc(
			void );

typedef struct
{
	LIST *folder_attribute_append_isa_name_list;
	LIST *update_attribute_list;
	LIST *update_changed_list;
	boolean update_changed_primary_key;
	LIST *folder_attribute_primary_key_list;
	LIST *update_where_list;
	char *where_clause;
	char *update_sql_statement;
	char *update_command_line;
} UPDATE_ROOT;

/* UPDATE_ROOT operations */
/* ---------------------- */

/* Usage */
/* ----- */
UPDATE_ROOT *update_root_new(
			char *application_name,
			char *login_name,
			char *folder_name,
			LIST *folder_attribute_append_isa_list,
			PROCESS *post_change_process,
			SECURITY_ENTITY *security_entity,
			char *appaserver_error_filename,
			LIST *update_attribute_list );

/* Process */
/* ------- */
UPDATE_ROOT *update_root_calloc(
			void );

/* Returns heap memory */
/* ------------------- */
char *update_root_where_clause(
			char *update_where_list_clause,
			SECURITY_ENTITY *security_entity,
			LIST *folder_attribute_append_isa_list );

typedef struct
{
	/* Attribute */
	/* --------- */
	int row;

	/* Process */
	/* ------- */
	LIST *update_attribute_list;
	UPDATE_ROOT *update_root;
	LIST *update_one2m_list;
	LIST *update_mto1_isa_list;
	int cell_count;
} UPDATE_ROW;

/* UPDATE_ROW operations */
/* --------------------- */

/* Usage */
/* ----- */
UPDATE_ROW *update_row_new(
			char *application_name,
			char *login_name,
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *folder_name,
			LIST *folder_attribute_append_isa_list,
			LIST *relation_one2m_recursive_list,
			LIST *relation_mto1_isa_list,
			PROCESS *post_change_process,
			SECURITY_ENTITY *security_entity,
			char *appaserver_error_filename,
			int row );

/* Process */
/* ------- */
UPDATE_ROW *update_row_calloc(
			void );
int update_row_cell_count(
			UPDATE_ROOT *update_root,
			LIST *update_one2m_list,
			LIST *update_mto1_isa_list );

/* Public */
/* ------ */

/* Returns char *sql_error_message or null */
/* --------------------------------------- */
char *update_row_root_sql_statement_execute(
			char *update_sql_statement );

void update_row_sql_statement_list_execute(
			LIST *update_one2m_list,
			LIST *update_mto1_isa_list,
			char *appaserver_error_filename );

void update_row_command_line_execute(
			UPDATE_ROOT *update_root,
			LIST *update_one2m_list,
			LIST *update_mto1_isa_list );

/* Private */
/* ------- */
void update_row_one2m_sql_statement_execute(
			FILE *output_pipe,
			LIST *update_one2m_list );

void update_row_mto1_isa_sql_statement_execute(
			FILE *output_pipe,
			LIST *update_mto1_isa_list );

void update_row_one2m_command_line_execute(
			LIST *update_one2m_list );

void update_row_mto1_isa_command_line_execute(
			LIST *update_mto1_isa_list );

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

/* Returns null if no updates */
/* -------------------------- */
UPDATE_ROW_LIST *update_row_list_new(
			char *application_name,
			char *login_name,
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *folder_name,
			LIST *folder_attribute_append_isa_list,
			LIST *relation_one2m_recursive_list,
			LIST *relation_mto1_isa_list,
			PROCESS *post_change_process,
			SECURITY_ENTITY *security_entity,
			char *appaserver_error_filename );

/* Process */
/* ------- */
UPDATE_ROW_LIST *update_row_list_calloc(
			void );

int update_row_list_cell_count(
			UPDATE_ROW_LIST *update_row_list );

/* Public */
/* ------ */

/* Returns char *sql_error_message_list_string or null */
/* --------------------------------------------------- */
char *update_row_list_execute(
			UPDATE_ROW_LIST *update_row_list,
			char *appaserver_error_filename );

typedef struct
{
	LIST *list;
} UPDATE_SQL_STATEMENT_LIST;

/* UPDATE_SQL_STATEMENT_LIST operations */
/* ------------------------------------ */

/* Usage */
/* ----- */
UPDATE_SQL_STATEMENT_LIST *update_sql_statement_list_new(
			UPDATE_ROW_LIST *update_row_list );

/* Process */
/* ------- */
UPDATE_SQL_STATEMENT_LIST *update_sql_statement_list_calloc(
			void );

typedef struct
{
	char *root;
	char *command_line_root;
	LIST *one2m_list;
	LIST *command_line_one2m_list;
	LIST *mto1_isa_list;
	LIST *command_line_mto1_isa_list;
} UPDATE_SQL_STATEMENT;

/* UPDATE_SQL_STATEMENT operations */
/* ------------------------------- */

/* Usage */
/* ----- */
UPDATE_SQL_STATEMENT *update_sql_statement_new(
			UPDATE_ROOT *update_root,
			LIST *update_one2m_list,
			LIST *update_mto1_isa_list );

/* Process */
/* ------- */
UPDATE_SQL_STATEMENT *update_sql_statement_calloc(
			void );

char *update_sql_statement_root(
			UPDATE_ROOT *update_root );

char *update_sql_statement_command_line_root(
			UPDATE_ROOT *update_root );

LIST *update_sql_statement_one2m_list(
			LIST *update_one2m_list );

LIST *update_sql_statement_command_line_one2m_list(
			LIST *update_one2m_list );

LIST *update_sql_statement_mto1_isa_list(
			LIST *update_mto1_isa_list );

LIST *update_sql_statement_command_line_mto1_isa_list(
			LIST *update_mto1_isa_list );

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

/* Returns heap memory */
/* ------------------- */
char *update_sql_statement(
			char *folder_table_name,
			char *update_changed_list_set_clause,
			char *update_where_clause );

/* Returns heap memory */
/* ------------------- */
char *update_command_line(
			char *command_line,
			char *login_name,
			char *appaserver_error_filename,
			char *update_where_primary_data_list_string,
			LIST *update_attribute_list,
			char *appaserver_update_state,
			char *update_preupdate_prefix );

#endif
