/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/relation.h		 			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef RELATION_H
#define RELATION_H

#include "boolean.h"
#include "list.h"
#include "dictionary.h"
#include "folder.h"

#define RELATION_TABLE			"relation"

#define RELATION_PRIMARY_KEY		"table_name,"			\
					"related_table,"		\
					"related_column"

#define RELATION_FOREIGN_KEY		"related_table"

#define RELATION_COLUMN_FOREIGN_KEY	"related_column"

#define RELATION_SELECT			"table_name,"			\
					"related_table,"		\
					"related_column,"		\
					"pair_one2m_order,"		\
					"omit_drillthru_yn,"		\
					"omit_drilldown_yn,"		\
					"relation_type_isa_yn,"		\
					"copy_common_columns_yn,"	\
					"automatic_preselection_yn,"	\
					"drop_down_multi_select_yn,"	\
					"join_one2m_each_row_yn,"	\
					"ajax_fill_drop_down_yn,"	\
					"hint_message"

typedef struct
{
	char *many_folder_name;
	char *one_folder_name;
	char *related_attribute_name;
	int pair_one2m_order;
	boolean omit_drillthru;
	boolean omit_drilldown;
	boolean relation_type_isa;
	boolean copy_common_attributes;
	boolean automatic_preselection;
	boolean drop_down_multi_select;
	boolean join_one2m_each_row;
	boolean ajax_fill_drop_down;
	char *hint_message;
} RELATION;

/* Usage */
/* ----- */

/* Returns static LIST * */
/* --------------------- */
LIST *relation_cache_list(
		void );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
RELATION *relation_parse(
		char *input );

/* Usage */
/* ----- */
RELATION *relation_new(
		char *many_folder_name,
		char *one_folder_name,
		char *related_attribute_name );

/* Process */
/* ------- */
RELATION *relation_calloc(
		void );

/* Usage */
/* ----- */
LIST *relation_seek_mto1_list(
		char *many_folder_name );

/* Usage */
/* ----- */
LIST *relation_seek_one2m_list(
		char *one_folder_name );

/* Usage */
/* ----- */
LIST *relation_foreign_key_list(
		LIST *one_folder_primary_key_list,
		char *related_attribute_name,
		LIST *foreign_attribute_name_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *relation_name(
		const char attribute_multi_key_delimiter,
		LIST *relation_foreign_key_list );

/* Usage */
/* ----- */
boolean relation_foreign_key_exists_primary(
		LIST *many_folder_primary_key_list,
		LIST *relation_foreign_key_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *relation_prompt(
		char *folder_name,
		char *related_attribute_name,
		LIST *foreign_attribute_name_list,
		boolean relation_foreign_key_exists_primary );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *relation_mnemonic(
		LIST *relation_foreign_key_list );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *relation_insert_statement(
		char *relation_table,
		char *relation_primary_key,
		char *many_folder_name,
		char *one_folder_name,
		char *related_attribute_name );

typedef struct
{
	char *primary_key;
	char *foreign_key;
} RELATION_TRANSLATE;

/* Usage */
/* ----- */
LIST *relation_translate_list(
		LIST *primary_key_list,
		LIST *relation_foreign_key_list );

/* Usage */
/* ----- */
RELATION_TRANSLATE *relation_translate_new(
		char *primary_key,
		char *foreign_key );

/* Process */
/* ------- */
RELATION_TRANSLATE *relation_translate_calloc(
		void );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
char *relation_translate_primary_key(
		char *foreign_key,
		LIST *relation_translate_list );

/* Process */
/* ------- */
RELATION_TRANSLATE *relation_translate_foreign_seek(
		char *foreign_key,
		LIST *relation_translate_list );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
char *relation_translate_foreign_key(
		char *primary_key,
		LIST *relation_translate_list );

/* Process */
/* ------- */
RELATION_TRANSLATE *relation_translate_primary_seek(
		char *primary_key,
		LIST *relation_translate_list );

/* Usage */
/* ----- */

/* Returns heap or program memory */
/* ------------------------------ */
char *relation_translate_list_display(
		LIST *relation_translate_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *relation_list_display(
		LIST *relation_list );

#endif
