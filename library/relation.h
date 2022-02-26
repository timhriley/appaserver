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

#define RELATION_PRIMARY_KEY	"folder,"				\
				"related_folder,"			\
				"related_attribute"

#define RELATION_SELECT_COLUMNS	"folder,"				\
				"related_folder,"			\
				"related_attribute,"			\
				"pair_1tom_order,"			\
				"omit_1tom_detail_yn,"			\
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
	int pair_one2m_order;
	boolean omit_one2m_detail;
	boolean relation_type_isa;
	boolean copy_common_attributes;
	boolean automatic_preselection;
	boolean drop_down_multi_select;
	boolean join_one2m_each_row;
	boolean omit_lookup_before_drop_down;
	boolean ajax_fill_drop_down;
	char *hint_message;

	/* Process */
	/* ------- */
	LIST *foreign_key_list;

	/* Private */
	/* ------- */
	boolean is_primary_key_subset;
	boolean consumes_taken;
} RELATION;

/* RELATION Operations */
/* ------------------- */

/* Usage */
/* ----- */
LIST *relation_mto1_non_isa_list(
			char *many_folder_name );

/* folder_attribute_list is set */
/* ---------------------------- */
LIST *relation_mto1_isa_list(
			LIST *relation_list /* in/out */,
			char *many_folder_name,
			boolean fetch_process );

LIST *relation_one2m_list(
			char *one_folder_name );

LIST *relation_one2m_recursive_list(
			LIST *relation_list /* in/out */,
			char *one_folder_name );

/* ----------------------- */
/* foreign_key_list is set */
/* ----------------------- */
LIST *relation_one2m_pair_list(
			char *one_folder_name );

LIST *relation_mto1_drillthru_list(
			LIST *relation_list,
			char *base_folder_name,
			LIST *fulfilled_folder_name_list );

/* Process */
/* ------- */
RELATION *relation_calloc(
			void );

char *relation_system_string(
			char *where,
			char *order_clause );

LIST *relation_system_list(
			char *system_string,
			/* -------------------------------------- */
			/* Setting both will set foreign_key_list */
			/* -------------------------------------- */
			boolean fetch_folder,
			boolean fetch_attribute_list,
			boolean fetch_process );

RELATION *relation_parse(
			char *input,
			/* -------------------------------------- */
			/* Setting both will set foreign_key_list */
			/* -------------------------------------- */
			boolean fetch_folder,
			boolean fetch_attribute_list,
			/* ---------------------- */
			/* If fetch_folder is set */
			/* ---------------------- */
			boolean fetch_process );

LIST *relation_foreign_key_list(
			/* ------------------------------------------- */
			/* Send in folder_attribute_primary_key_list() */
			/* ------------------------------------------- */
			LIST *primary_foreign_key_list,
			char *related_attribute_name,
			LIST *foreign_attribute_name_list );

/* Public */
/* ------ */
LIST *relation_pair_one2m_list(
			LIST *relation_one2m_list );

LIST *relation_join_one2m_list(
			LIST *relation_one2m_recursive_list,
			DICTIONARY *ignore_dictionary );

LIST *relation_mto1_non_isa_list(
			char *many_folder_name );

RELATION *relation_consumes(
			char *many_attribute_name,
			LIST *relation_mto1_list );

RELATION *relation_one2m_seek(
			char *folder_name,
			LIST *relation_one2m_list );

RELATION *relation_mto1_seek(
			char *folder_name,
			LIST *relation_mto1_list );

LIST *relation_mto1_folder_name_list(
			LIST *relation_mto1_list );

LIST *relation_one2m_folder_name_list(
			LIST *relation_one2m_list );

char *relation_list_display(
			LIST *relation_list );

void relation_set_one_folder_primary_delimited_list(
			LIST *relation_mto1_list );

/* Private */
/* ------- */
RELATION *relation_new(
			char *many_folder_name,
			char *one_folder_name );

boolean relation_is_primary_key_subset(
			LIST *foreign_key_list,
			LIST *many_primary_key_list );

/* Returns heap memory */
/* ------------------- */
char *relation_display(	RELATION *relation );

#endif

