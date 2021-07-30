/* $APPASERVER_HOME/library/folder_attribute.h				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef FOLDER_ATTRIBUTE_H
#define FOLDER_ATTRIBUTE_H

/* Includes */
/* -------- */
#include "list.h"
#include "boolean.h"
#include "relation.h"
#include "attribute.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define FOLDER_ATTRIBUTE_TABLE	"folder_attribute"

#define FOLDER_ATTRIBUTE_SELECT	"folder,"			\
				"attribute,"			\
				"primary_key_index,"		\
				"display_order,"		\
				"omit_insert_yn,"		\
				"omit_insert_prompt_yn,"	\
				"omit_update_yn,"		\
				"additional_unique_index_yn,"	\
				"additional_index_yn,"		\
				"insert_required_yn,"		\
				"lookup_required_yn"

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	char *folder_name;
	char *attribute_name;
	ATTRIBUTE *attribute;
	int primary_key_index;
	int display_order;
	boolean omit_insert;
	boolean omit_insert_prompt;
	boolean omit_update;
	boolean additional_unique_index;
	boolean additional_index;
	boolean insert_required;
	boolean lookup_required;

	/* Process */
	/* ------- */

} FOLDER_ATTRIBUTE;

/* Operations */
/* ---------- */
FOLDER_ATTRIBUTE *folder_attribute_new(
			char *folder_name,
			char *attribute_name );

LIST *folder_attribute_list(
			char *escaped_replaced_folder_name,
			LIST *exclude_attribute_name_list );

/* Returns heap memory */
/* ------------------- */
char *folder_attribute_system_string(
			char *where );

LIST *folder_attribute_system_list(
			char *system_string );

FOLDER_ATTRIBUTE *folder_attribute_parse(
			char *input );

/* Returns heap memory */
/* ------------------- */
char *folder_attribute_where(
			char *escaped_replaced_folder_name,
			LIST *exclude_attribute_name_list );

#endif
