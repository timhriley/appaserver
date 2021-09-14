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
#include "related_folder.h"

/* Constants */
/* --------- */

/* Data structures */
/* --------------- */
typedef struct
{
	FOLDER *one_folder;
} PROMPT_RECURSIVE_MTO1_FOLDER;

typedef struct
{
	FOLDER *one_folder;
	LIST *relation_mto1_primary_key_subset_list;
	LIST *prompt_recursive_mto1_folder_list;
} PROMPT_RECURSIVE_FOLDER;

typedef struct
{
	char *folder_name;
	DICTIONARY *drillthru_dictionary;
	LIST *prompt_recursive_folder_list;
	LIST *element_list;
} PROMPT_RECURSIVE;

/* PROMPT_RECURSIVE operations */
/* --------------------------- */
PROMPT_RECURSIVE *prompt_recursive_new(
			char *folder_name,
			LIST *relation_mto1_non_isa_list,
			DICTIONARY *drillthru_dictionary,
			boolean drillthru_skipped );

/* PROMPT_RECURSIVE_FOLDER operations */
/* ---------------------------------- */
PROMPT_RECURSIVE_FOLDER *prompt_recursive_folder_calloc(
			void );

LIST *prompt_recursive_folder_list(
			LIST *relation_mto1_non_isa_list,
			DICTIONARY *drillthru_dictionary );

LIST *prompt_recursive_element_list(
			LIST *prompt_recursive_folder_list );

PROMPT_RECURSIVE_FOLDER *prompt_recursive_folder_new(
			FOLDER *one_folder,
			/* ------------------------------ */
			/* Exclude drop_down_multi_select */
			/* ------------------------------ */
			boolean drop_down_multi_select );

LIST *prompt_recursive_mto1_folder_list(
			char *folder_name,
			LIST *primary_key_list );

PROMPT_RECURSIVE_MTO1_FOLDER *prompt_recursive_mto1_folder_new(
			FOLDER *one_folder );

PROMPT_RECURSIVE_MTO1_FOLDER *prompt_recursive_mto1_folder_calloc(
			void );

#endif
