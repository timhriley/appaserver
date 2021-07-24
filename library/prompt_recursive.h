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
	FOLDER *folder;
	int recursive_level;
} PROMPT_RECURSIVE_MTO1_FOLDER;

typedef struct
{
	FOLDER *folder;
	LIST *prompt_recursive_mto1_folder_list;
} PROMPT_RECURSIVE_FOLDER;

typedef struct
{
	char *folder_name;
	LIST *prompt_recursive_folder_list;
} PROMPT_RECURSIVE;

/* Prototypes */
/* ---------- */
PROMPT_RECURSIVE_FOLDER *prompt_recursive_folder_calloc(
			void );

PROMPT_RECURSIVE *prompt_recursive(
			char *folder_name,
			LIST *mto1_related_folder_list );

LIST *prompt_recursive_folder_list(
			LIST *mto1_related_folder_list );

PROMPT_RECURSIVE_FOLDER *prompt_recursive_folder(
			FOLDER *folder );

LIST *prompt_recursive_mto1_folder_list(
			char *folder_name,
			LIST *primary_attribute_name_list );

PROMPT_RECURSIVE_MTO1_FOLDER *prompt_recursive_mto1_folder(
			FOLDER *folder,
			int recursive_level );

char *prompt_recursive_display(
			PROMPT_RECURSIVE *prompt_recursive );

PROMPT_RECURSIVE_MTO1_FOLDER *prompt_recursive_mto1_folder_calloc(
			void );

#endif
