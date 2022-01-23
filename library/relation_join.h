/* --------------------------------------------- */
/* $APPASERVER_HOME/library/relation_join.h	 */
/* --------------------------------------------- */
/*						 */
/* Freely available software: see Appaserver.org */
/* --------------------------------------------- */

#ifndef RELATION_JOIN_H
#define RELATION_JOIN_H

#include "boolean.h"
#include "list.h"
#include "dictionary.h"
#include "relation.h"
#include "folder.h"

/* Structures */
/* ---------- */
typedef struct
{
	/* Process */
	/* ------- */
	LIST *dictionary_data_list;
	LIST *relation_foreign_key_list;
	char *string;
} RELATION_JOIN_FOLDER_WHERE;

/* RELATION_JOIN_FOLDER_WHERE operations */
/* ------------------------------------- */
RELATION_JOIN_FOLDER_WHERE *relation_join_folder_where_calloc(
			void );

/* Always succeeds */
/* --------------- */
RELATION_JOIN_FOLDER_WHERE *relation_join_folder_where_new(
			DICTIONARY *row_dictionary,
			RELATION *relation_join_one2m,
			LIST *primary_key_list );

/* Returns heap memory or null */
/* --------------------------- */
char *relation_join_folder_where_string(
			LIST *relation_foreign_key_list,
			LIST *dictionary_data_list );

typedef struct
{
	/* Process */
	/* ------- */
	RELATION_JOIN_FOLDER_WHERE *where;
	char *system_string;
	char *delimited_string;
} RELATION_JOIN_FOLDER;

RELATION_JOIN_FOLDER *relation_join_folder_calloc(
			void );

/* Always succeeds */
/* --------------- */
RELATION_JOIN_FOLDER *relation_join_folder_new(
			DICTIONARY *row_dictionary /* in */,
			char *application_name,
			RELATION *relation_join_one2m,
			LIST *primary_key_list );

/* Returns heap memory */
/* ------------------- */
char *relation_join_folder_system_string(
			char *application_name,
			char *select_name_list_string,
			char *folder_name,
			char *relation_join_folder_where_string,
			int multi_attribute_data_label_delimiter );

char *relation_join_folder_delimited_string(
			char *relation_join_folder_system_string );

typedef struct

{
	/* Process */
	/* ------- */
	RELATION_JOIN_FOLDER *relation_join_folder;
} RELATION_JOIN;

/* Operations */
/* ---------- */
RELATION_JOIN *relation_join_calloc(
			void );

RELATION_JOIN *relation_join_new(
			DICTIONARY *row_dictionary /* in/out */,
			char *application_name,
			LIST *relation_join_one2m_list,
			LIST *primary_key_list );
#endif
