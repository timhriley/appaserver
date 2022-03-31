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
#define FOLDER_ATTRIBUTE_TABLE			"folder_attribute"
#define SORT_ORDER_ATTRIBUTE_NAME		"sort_order"
#define DISPLAY_ORDER_ATTRIBUTE_NAME		"display_order"
#define SEQUENCE_NUMBER_ATTRIBUTE_NAME		"sequence_number"

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
	/* Attributes */
	/* ---------- */
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
} FOLDER_ATTRIBUTE;

/* FOLDER_ATTRIBUTE operations */
/* --------------------------- */

/* Usage */
/* ----- */
LIST *folder_attribute_list(
			char *escaped_replaced_folder_name,
			LIST *exclude_attribute_name_list,
			boolean fetch_attribute );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *folder_attribute_where(
			char *escaped_replaced_folder_name,
			LIST *exclude_attribute_name_list );

/* Returns heap memory */
/* ------------------- */
char *folder_attribute_system_string(
			char *folder_attribute_where,
			char *folder_attribute_select,
			char *folder_attribute_table );

LIST *folder_attribute_system_list(
			char *folder_attribute_system_string,
			boolean fetch_attribute );

FOLDER_ATTRIBUTE *folder_attribute_parse(
			char *input,
			boolean fetch_attribute );

FOLDER_ATTRIBUTE *folder_attribute_new(
			char *folder_name,
			char *attribute_name );

FOLDER_ATTRIBUTE *folder_attribute_calloc(
			void );

/* Public */
/* ------ */
LIST *folder_attribute_append_isa_list(
			LIST *append_isa_list /* in/out */,
			LIST *relation_mto1_isa_list );

LIST *folder_attribute_name_list(
			LIST *folder_attribute_list );

LIST *folder_attribute_prefixed_name_list(
			LIST *folder_attribute_list );

LIST *folder_attribute_primary_list(
			LIST *folder_attribute_list );

LIST *folder_attribute_non_primary_list(
			LIST *folder_attribute_list );

LIST *folder_attribute_primary_key_list(
			LIST *folder_attribute_list );

LIST *folder_attribute_non_primary_key_list(
			LIST *folder_attribute_list );

LIST *folder_attribute_number_list(
			LIST *folder_attribute_list );

LIST *folder_attribute_date_name_list(
			LIST *folder_attribute_list );

LIST *folder_attribute_time_name_list(
			LIST *folder_attribute_list );

LIST *folder_attribute_date_time_name_list(
			LIST *folder_attribute_list );

boolean folder_attribute_exists(
			char *folder_name,
			char *attribute_name );

FOLDER_ATTRIBUTE *folder_attribute_seek(
			char *attribute_name,
			LIST *folder_attribute_list );

FOLDER_ATTRIBUTE *folder_attribute_list_seek(
			char *attribute_name,
			LIST *folder_attribute_list );

/* Returns heap memory */
/* ------------------- */
char *folder_attribute_sort_attribute_name(
			LIST *folder_attribute_list );

/* Returns heap memory */
/* ------------------- */
char *folder_attribute_list_display(
			LIST *folder_attribute_list );

/* Private */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *folder_attribute_primary_where(
			char *folder_name,
			char *attribute_name );

#endif
