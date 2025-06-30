/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/relation_one2m.h	 			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef RELATION_ONE2M_H
#define RELATION_ONE2M_H

#include "boolean.h"
#include "list.h"
#include "folder.h"
#include "relation.h"

typedef struct
{
	char *one_folder_name;
	char *many_folder_name;
	LIST *one_folder_primary_key_list;
	RELATION *relation;
	FOLDER *many_folder;
	LIST *foreign_attribute_list;
	LIST *foreign_attribute_name_list;
	LIST *relation_foreign_key_list;
	LIST *relation_translate_list;
	char *relation_name;
	boolean relation_foreign_key_exists_primary;
	char *relation_prompt;
	boolean primary_key_subset;
	boolean foreign_key_none_primary;
} RELATION_ONE2M;

/* Usage */
/* ----- */
LIST *relation_one2m_list(
		/* ----------------------------------- */
		/* Set to cache all folders for a role */
		/* ----------------------------------- */
		char *role_name,
		char *one_folder_name,
		LIST *one_folder_primary_key_list,
		boolean include_isa_boolean );

/* Usage */
/* ----- */
RELATION_ONE2M *relation_one2m_new(
		char *role_name,
		char *one_folder_name,
		LIST *one_folder_primary_key_list,
		RELATION *relation );

/* Process */
/* ------- */
RELATION_ONE2M *relation_one2m_calloc(
		void );

/* Usage */
/* ----- */
LIST *relation_one2m_recursive_list(
		LIST *one2m_list /* Pass in NULL */,
		char *role_name,
		char *one_folder_name,
		LIST *one_folder_primary_key_list );

/* Usage */
/* ----- */
boolean relation_one2m_primary_key_subset(
		LIST *relation_foreign_key_list,
		LIST *many_folder_primary_key_list );

/* Usage */
/* ----- */
boolean relation_one2m_foreign_key_none_primary(
		LIST *relation_foreign_key_list,
		LIST *many_folder_primary_key_list );

/* Usage */
/* ----- */
LIST *relation_one2m_pair_list(
		char *role_name,
		char *one_folder_name,
		LIST *one_folder_primary_key_list );

/* Usage */
/* ----- */
LIST *relation_one2m_folder_name_list(
		LIST *relation_one2m_list );

/* Usage */
/* ----- */
LIST *relation_one2m_join_list(
		/* ----------------------------------- */
		/* Set to cache all folders for a role */
		/* ----------------------------------- */
		char *role_name,
		char *one_folder_name,
		LIST *one_folder_primary_key_list,
		DICTIONARY *no_display_dictionary );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *relation_one2m_where_string(
		const char *relation_foreign_key
			/* probably related_table */,
		char *one_folder_name );

/* Usage */
/* ----- */

/* Returns heap memory or program memory */
/* ------------------------------------- */
char *relation_one2m_list_display(
		LIST *relation_one2m_list );

/* Usage */
/* ----- */
LIST *relation_one2m_omit_update_list(
		LIST *relation_mto1_recursive_list );

#endif

