/* --------------------------------------------- */
/* $APPASERVER_HOME/library/relation.h		 */
/* --------------------------------------------- */
/*						 */
/* Freely available software: see Appaserver.org */
/* --------------------------------------------- */

#ifndef RELATION_H
#define RELATION_H

#include "boolean.h"
#include "list.h"
#include "folder.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define RELATION_TABLE		"relation"

#define RELATION_PRIMARY_KEY	"program_name"

#define RELATION_SELECT_COLUMNS	"folder,"				\
				"related_folder,"			\
				"related_attribute,"			\
				"pair_1tom_order,"			\
				"omit_1tom_detail_yn,"			\
				"prompt_mto1_recursive_yn,"		\
				"relation_type_isa_yn,"			\
				"copy_common_attributes_yn,"		\
				"automatic_preselection_yn,"		\
				"drop_down_multi_select_yn,"		\
				"join_1tom_each_row_yn,"		\
				"omit_lookup_before_drop_down_yn,"	\
				"ajax_fill_drop_down_yn,"		\
				"hint_message"


/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	char *many_folder_name;
	FOLDER *many_folder;
	char *one_folder_name;
	FOLDER *one_folder;
	char *related_attribute_name;
	int pair_1tom_order;
	boolean omit_1tom_detail;
	boolean prompt_mto1_recursive;
	boolean relation_type_isa;
	boolean copy_common_attributes;
	boolean automatic_preselection;
	boolean drop_down_multi_select;
	boolean join_1tom_each_row;
	boolean omit_lookup_before_drop_down;
	boolean ajax_fill_drop_down;
	char *hint_message;

	/* Process */
	/* ------- */
	boolean is_primary_key_subset;
	LIST *foreign_attribute_name_list;
	boolean ignore;
	LIST *join_where_clause;
} RELATION;

RELATION *relation_calloc(
			void );

RELATION *relation_new(
			char *many_folder_name,
			char *one_folder_name,
			boolean fetch_folder,
			boolean fetch_attribute_list );

RELATION *relation_parse(
			char *input,
			/* ---------------------------- */
			/* Setting both will set	*/
			/* foreign_attribute_name_list	*/
			/* ---------------------------- */
			boolean fetch_folder,
			boolean fetch_attribute_list );

LIST *relation_system_list(
			char *sys_string,
			/* ---------------------------- */
			/* Setting both will set	*/
			/* foreign_attribute_name_list	*/
			/* ---------------------------- */
			boolean fetch_folder,
			boolean fetch_attribute_list );

char *relation_sys_string(
			char *where,
			char *order_clause );

/* Safely returns heap memory */
/* -------------------------- */
char *relation_display(	RELATION *relation );

LIST *relation_foreign_attribute_name_list(
			/* ----------------------------------- */
			/* Send in primary_attribute_name_list */
			/* ----------------------------------- */
			LIST *primary_foreign_attribute_name_list,
			char *related_attribute_name,
			LIST *foreign_attribute_list_folder );

LIST *relation_one2m_recursive_relation_list(
			LIST *relation_list /* in only */,
			char *one_folder_name );

LIST *relation_one2m_relation_list(
			char *one_folder_name );

LIST *relation_mto1_isa_recursive_relation_list(
			LIST *relation_list /* in only */,
			char *many_folder_name );

LIST *relation_mto1_relation_list(
			char *many_folder_name );

/* ---------------------------------- */
/* foreign_attribute_name_list is set */
/* ---------------------------------- */
LIST *relation_one2m_fetch_relation_list(
			char *one_folder_name,
			char *order_clause );

/* ---------------------------------- */
/* foreign_attribute_name_list is set */
/* ---------------------------------- */
LIST *relation_mto1_fetch_relation_list(
			char *many_folder_name );

boolean relation_is_primary_key_subset(
			LIST *foreign_attribute_name_list,
			LIST *many_primary_attribute_name_list );

char *relation_list_display(
			LIST *one2m_relation_list );

LIST *relation_one2m_pair_relation_list(
			LIST *relation_one2m_relation_list );

#endif

