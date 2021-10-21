/* $APPASERVER_HOME/library/prompt_recursive.h				*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef PROMPT_RECURSIVE_H
#define PROMPT_RECURSIVE_H

/* Includes */
/* -------- */
#include "list.h"
#include "folder.h"
#include "relation.h"
#include "security.h"

/* Constants */
/* --------- */

/* Data structures */
/* --------------- */
typedef struct
{
	/* Input */
	/* ----- */
	FOLDER *one_folder;
	boolean drop_down_multi_select;
} PROMPT_RECURSIVE_MTO1_FOLDER;

typedef struct
{
	/* Input */
	/* ----- */
	FOLDER *one_folder;
	boolean drop_down_multi_select;

	/* Process */
	/* ------- */
	LIST *relation_mto1_primary_key_subset_list;
	LIST *mto1_folder_list;
	char *javascript;
	LIST *element_list;
} PROMPT_RECURSIVE_FOLDER;

typedef struct
{
	/* Input */
	/* ----- */
	char *folder_name;

	/* Process */
	/* ------- */
	SECURITY_ENTITY *security_entity;
	LIST *prompt_recursive_folder_list;
	char *javascript;
	LIST *element_list;
} PROMPT_RECURSIVE;

/* PROMPT_RECURSIVE operations */
/* --------------------------- */
PROMPT_RECURSIVE *prompt_recursive_new(
			char *folder_name,
			LIST *relation_mto1_non_isa_list,
			DICTIONARY *drillthru_dictionary,
			char *login_name,
			boolean non_owner_forbid,
			boolean override_row_restrictions,
			boolean drillthru_skipped );

LIST *prompt_recursive_element_list(
			LIST *prompt_recursive_folder_list );

/* PROMPT_RECURSIVE_FOLDER operations */
/* ---------------------------------- */
PROMPT_RECURSIVE_FOLDER *prompt_recursive_folder_calloc(
			void );

LIST *prompt_recursive_folder_list(
			LIST *relation_mto1_non_isa_list,
			DICTIONARY *drillthru_dictionary,
			char *login_name,
			SECURITY_ENTITY *security_entity );

PROMPT_RECURSIVE_FOLDER *prompt_recursive_folder_new(
			FOLDER *one_folder,
			boolean drop_down_multi_select,
			DICTIONARY *drillthru_dictionary,
			char *login_name,
			SECURITY_ENTITY *security_entity );

void prompt_recursive_folder_list_set_javascript(
			LIST *prompt_recursive_folder_list );

LIST *prompt_recursive_folder_list_element_list(
			LIST *prompt_recursive_folder_list );

/* Safely returns heap memory */
/* -------------------------- */
char *prompt_recursive_folder_javascript(
			LIST *primary_key_list,
			boolean drop_down_multi_select,
			LIST *relation_mto1_primary_key_subset_list );

LIST *prompt_recursive_folder_element_list(
			FOLDER *one_folder,
			boolean drop_down_multi_select,
			char *javascript,
			LIST *prompt_recursive_mto1_folder_list );

LIST *prompt_recursive_one_folder_element_list(
			char *folder_name,
			LIST *primary_key_list,
			boolean drop_down_multi_select,
			char *javascript );

/* PROMPT_RECURSIVE_MTO1_FOLDER operations */
/* --------------------------------------- */
LIST *prompt_recursive_mto1_folder_list(
			LIST *relation_mto1_primary_key_subset_list,
			DICTIONARY *drillthru_dictionary,
			char *login_name,
			SECURITY_ENTITY *security_entity );

PROMPT_RECURSIVE_MTO1_FOLDER *prompt_recursive_mto1_folder_new(
			FOLDER *one_folder,
			boolean drop_down_multi_select,
			DICTIONARY *drillthru_dictionary,
			char *login_name,
			SECURITY_ENTITY *security_entity );

PROMPT_RECURSIVE_MTO1_FOLDER *prompt_recursive_mto1_folder_calloc(
			void );

LIST *prompt_recursive_mto1_folder_element_list(
			/* -------------------- */
			/* Returns element_list */
			/* -------------------- */
			LIST *element_list,
			char *javascript,
			LIST *mto1_folder_list );

#endif
