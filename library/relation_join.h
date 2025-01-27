/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/relation_join.h	 			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef RELATION_JOIN_H
#define RELATION_JOIN_H

#include "boolean.h"
#include "list.h"
#include "relation_one2m.h"
#include "folder.h"

typedef struct
{
	LIST *query_cell_attribute_data_list;
	char *string;
} RELATION_JOIN_FOLDER_WHERE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
RELATION_JOIN_FOLDER_WHERE *relation_join_folder_where_new(
		LIST *one_folder_primary_key_list,
		LIST *relation_foreign_key_list,
		LIST *query_row_cell_list );

/* Process */
/* ------- */
RELATION_JOIN_FOLDER_WHERE *relation_join_folder_where_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *relation_join_folder_where_string(
		LIST *relation_foreign_key_list,
		LIST *query_cell_attribute_data_list );

typedef struct
{
	RELATION_ONE2M *relation_one2m_join;
	LIST *select_name_list;
	char *select_name_list_string;
	RELATION_JOIN_FOLDER_WHERE *relation_join_folder_where;
	char *system_string;
	LIST *delimited_list;
} RELATION_JOIN_FOLDER;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
RELATION_JOIN_FOLDER *relation_join_folder_new(
		LIST *one_folder_primary_key_list,
		RELATION_ONE2M *relation_one2m_join,
		LIST *query_row_cell_list );

/* Process */
/* ------- */
RELATION_JOIN_FOLDER *relation_join_folder_calloc(
		void );

/* Usage */
/* ----- */
LIST *relation_join_folder_select_name_list(
		LIST *many_folder_primary_key_list,
		LIST *relation_foreign_key_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *relation_join_folder_select_name_list_string(
		LIST *relation_join_folder_select_name_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *relation_join_folder_system_string(
		char *relation_join_folder_select_name_list_string,
		char *many_folder_name,
		char *relation_join_folder_where_string );

/* Usage */
/* ----- */
LIST *relation_join_folder_delimited_list(
		char *relation_join_folder_system_string );

/* Usage */
/* ----- */
RELATION_JOIN_FOLDER *relation_join_folder_seek(
		char *folder_name,
		LIST *relation_join_folder_list );

typedef struct
{
	LIST *relation_join_folder_list;
} RELATION_JOIN_ROW;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
RELATION_JOIN_ROW *relation_join_row_new(
		LIST *one_folder_primary_key_list,
		LIST *relation_one2m_join_list,
		LIST *query_row_cell_list );

/* Process */
/* ------- */
RELATION_JOIN_ROW *relation_join_row_calloc(
		void );

/* Usage */
/* ----- */

/* Returns static LIST * or null */
/* ----------------------------- */
LIST *relation_join_row_folder_name_list(
		RELATION_JOIN_ROW *relation_join_row );

typedef struct

{
	LIST *relation_join_row_list;
} RELATION_JOIN;

/* Usage */
/* ----- */
RELATION_JOIN *relation_join_new(
		LIST *one_folder_primary_key_list,
		LIST *relation_one2m_join_list,
		LIST *query_fetch_row_list );

/* Process */
/* ------- */
RELATION_JOIN *relation_join_calloc(
		void );

#endif
