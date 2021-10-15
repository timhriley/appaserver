/* $APPASERVER_HOME/library/delete.h		 */
/* --------------------------------------------- */
/* This is the appaserver delete_database ADT.	 */
/*						 */
/* Freely available software: see Appaserver.org */
/* --------------------------------------------- */

#ifndef DELETE_H
#define DELETE_H

#include <stdio.h>
#include "list.h"
#include "dictionary.h"
#include "process.h"
#include "folder.h"
#include "role.h"
#include "security.h"
#include "relation.h"

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	char *application_name;
	RELATION *relation_mto1_isa;
	LIST *primary_data_list;

	/* Process */
	/* ------- */
	char *delete_sql_statement;
	char *delete_command_line;
	char *delete_pre_command_line;
	LIST *one2m_list;
} DELETE_MTO1_ISA;

typedef struct
{
	/* Input */
	/* ----- */
	char *application_name;
	RELATION *relation_one2m;
	LIST *foreign_data_list;

	/* Process */
	/* ------- */
	LIST *primary_delimited_list;
	LIST *update_sql_statement_list;
	LIST *delete_sql_statement_list;
	LIST *pre_command_line_list;
	LIST *command_line_list;
} DELETE_ONE2M;

typedef struct
{
	/* Input */
	/* ----- */
	char *application_name;
	char *folder_name;
	char *login_name;
	LIST *primary_key_list;
	LIST *primary_data_list;
	PROCESS *post_change_process;
	char *security_entity_where;

	/* Process */
	/* ------- */
	char *delete_sql_statement;
	char *delete_pre_command_line;
	char *delete_command_line;
} DELETE_ROOT;

typedef struct
{
	/* Input */
	/* ----- */
	char *application_name;
	char *session_key;
	char *login_name;
	char *folder_name;
	char *role_name;
	LIST *primary_data_list;
	boolean dont_delete_mto1_isa;

	/* Process */
	/* ------- */
	FOLDER *folder;
	ROLE *role;
	SECURITY_ENTITY *security_entity;
	DELETE_ROOT *delete_root;
	LIST *delete_one2m_list;
	LIST *delete_mto1_isa_list;
} DELETE;

/* DELETE operations */
/* ----------------- */
DELETE *delete_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *folder_name,
			char *role_name,
			LIST *primary_data_list,
			boolean dont_delete_mto1_isa );

DELETE *delete_calloc(	void );

char *delete_sql_statement(
			char *folder_table_name,
			LIST *primary_key_list,
			LIST *primary_data_list,
			char *security_entity_where );

/* --------------------------- */
/* Returns heap memory or null */
/* --------------------------- */
char *delete_command_line(
			char *command_line,
			char *login_name,
			LIST *primary_key_list,
			LIST *primary_data_list,
			char *state );

LIST *delete_sql_statement_list(
			DELETE_ROOT *delete_root,
			LIST *delete_one2m_list,
			LIST *delete_mto1_isa_list );

LIST *delete_pre_command_line_list(
			DELETE_ROOT *delete_root,
			LIST *delete_one2m_list,
			LIST *delete_mto1_isa_list );

LIST *delete_command_line_list(
			DELETE_ROOT *delete_root,
			LIST *delete_one2m_list,
			LIST *delete_mto1_isa_list );

LIST *delete_distinct_folder_name_list(
			DELETE_ROOT *delete_root,
			LIST *delete_one2m_list,
			LIST *delete_mto1_isa_list );

/* DELETE_ROOT operations */
/* ---------------------- */
DELETE_ROOT *delete_root_new(
			char *application_name,
			char *folder_name,
			char *login_name,
			LIST *primary_key_list,
			LIST *primary_data_list,
			PROCESS *post_change_process,
			char *security_entity_where );

DELETE_ROOT *delete_root_calloc(
			void );

/* DELETE_ONE2M operations */
/* ----------------------- */
LIST *delete_one2m_list(
			char *application_name,
			char *login_name,
			LIST *relation_one2m_recursive_list,
			LIST *foreign_data_list );

DELETE_ONE2M *delete_one2m_new(
			char *application_name,
			char *login_name,
			RELATION *relation_one2m,
			LIST *foreign_data_list );

DELETE_ONE2M *delete_one2m_calloc(
			void );

LIST *delete_one2m_primary_delimited_list(
			char *folder_table_name,
			LIST *primary_key_list,
			LIST *foreign_key_list,
			LIST *foreign_data_list );

/* Returns heap memory */
/* ------------------- */
char *delete_one2m_sql_statement(
			char *folder_table_name,
			LIST *primary_key_list,
			LIST *primary_delimited_list );

/* Returns heap memory */
/* ------------------- */
char *delete_one2m_update_sql_statement(
			char *folder_table_name,
			LIST *foreign_key_list,
			LIST *primary_key_list,
			LIST *primary_data_list );

/* Returns heap memory or null */
/* --------------------------- */
char *delete_one2m_update_set_null_statement(
			LIST *foreign_key_list );

LIST *delete_one2m_sql_statement_list(
			char *folder_table_name,
			LIST *primary_key_list,
			LIST *primary_data_list );

LIST *delete_one2m_update_sql_statement_list(
			char *folder_table_name,
			LIST *foreign_key_list,
			LIST *primary_key_list,
			LIST *primary_delimited_list );

LIST *delete_one2m_command_line_list(
			char *command_line,
			char *login_name,
			LIST *foreign_key_list,
			LIST *primary_delimited_list,
			char *state );

LIST *delete_one2m_list_sql_statement_list(
			LIST *delete_one2m_list );

LIST *delete_one2m_list_pre_command_line_list(
			LIST *delete_one2m_list );

LIST *delete_one2m_list_command_line_list(
			LIST *delete_one2m_list );

/* DELETE_MTO1_ISA operations */
/* -------------------------- */
LIST *delete_mto1_isa_list(
			char *application_name,
			char *login_name,
			LIST *relation_mto1_isa_list,
			LIST *primary_data_list );

DELETE_MTO1_ISA *delete_mto1_isa_new(
			char *application_name,
			char *login_name,
			RELATION *relation_mto1_isa,
			LIST *primary_data_list );

DELETE_MTO1_ISA *delete_mto1_isa_calloc(
			void );

LIST *delete_mto1_isa_one2m_list(
			char *application_name,
			char *login_name,
			char *folder_name,
			LIST *primary_data_list );

LIST *delete_mto1_isa_sql_statement_list(
			LIST *delete_mto1_isa_list );

LIST *delete_mto1_isa_pre_command_line_list(
			LIST *delete_mto1_isa_list );

LIST *delete_mto1_isa_command_line_list(
			LIST *delete_mto1_isa_list );

LIST *delete_mto1_isa_one2m_sql_statement_list(
			LIST *one2m_list );

LIST *delete_mto1_isa_one2m_pre_command_line_list(
			LIST *one2m_list );

LIST *delete_mto1_isa_one2m_command_line_list(
			LIST *one2m_list );

LIST *delete_mto1_isa_distinct_folder_name_list(
			LIST *delete_mto1_isa_list );

LIST *delete_mto1_isa_one2m_distinct_folder_name_list(
			LIST *one2m_list );

#endif
