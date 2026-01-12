/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/relation_mto1.h	 			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef RELATION_MTO1_H
#define RELATION_MTO1_H

#include "boolean.h"
#include "list.h"
#include "folder.h"
#include "relation.h"

typedef struct
{
	char *many_folder_name;
	char *one_folder_name;
	LIST *many_folder_primary_key_list;
	RELATION *relation;
	FOLDER *one_folder;
	LIST *foreign_attribute_list;
	LIST *foreign_attribute_name_list;
	LIST *relation_foreign_key_list;
	LIST *relation_translate_list;
	char *relation_name;
	boolean relation_foreign_key_exists_primary;
	char *relation_prompt;
	LIST *relation_one2m_list;
	LIST *relation_mto1_list;
} RELATION_MTO1;

/* Usage */
/* ----- */
LIST *relation_mto1_list(
		char *many_folder_name,
		LIST *many_folder_primary_key_list );

/* Usage */
/* ----- */
RELATION_MTO1 *relation_mto1_new(
		char *many_folder_name,
		LIST *many_folder_primary_key_list,
		RELATION *relation );

/* Process */
/* ------- */
RELATION_MTO1 *relation_mto1_calloc(
		void );

/* Usage */
/* ----- */
RELATION_MTO1 *relation_mto1_consumes(
		char *many_attribute_name,
		LIST *relation_mto1_list );

/* Usage */
/* ----- */
RELATION_MTO1 *relation_mto1_isa_consumes(
		char *many_attribute_name,
		LIST *relation_mto1_isa_list );

/* Usage */
/* ----- */
RELATION_MTO1 *relation_mto1_attribute_consumes(
		char *many_attribute_name,
		LIST *relation_mto1_list );

/* Usage */
/* ----- */
LIST *relation_mto1_status_skipped_list(
		int query_drop_down_fetch_max_rows,
		int drillthru_skipped_max_foreign_length,
		LIST *relation_mto1_list,
		boolean drillthru_status_skipped_boolean );

/* Usage */
/* ----- */
RELATION_MTO1 *relation_mto1_seek(
		char *one_folder_name,
		LIST *relation_mto1_list );

/* Usage */
/* ----- */
RELATION_MTO1 *relation_mto1_ajax_fill_seek(
		LIST *relation_mto1_to_one_list );

/* Usage */
/* ----- */
LIST *relation_mto1_folder_name_list(
		LIST *relation_mto1_list );

/* Usage */
/* ----- */
boolean relation_mto1_exists_multi_select(
		LIST *relation_mto1_list );

/* Usage */
/* ----- */
RELATION_MTO1 *relation_mto1_automatic_preselection(
		LIST *relation_mto1_list );

/* Usage */
/* ----- */
void relation_mto1_list_set_one_to_many_list(
		LIST *relation_mto1_list /* in/out */ );

/* Usage */
/* ----- */
LIST *relation_mto1_foreign_key_less_equal_list(
		unsigned long query_drop_down_fetch_max_rows,
		int drillthru_skipped_max_foreign_length,
		LIST *relation_mto1_list );

/* Usage */
/* ----- */
LIST *relation_mto1_drillthru_list(
		LIST *relation_mto1_list /* Pass in null */,
		char *base_folder_name,
		LIST *base_folder_primary_key_list,
		LIST *fulfilled_folder_name_list );

/* Usage */
/* ----- */
LIST *relation_mto1_recursive_list(
		LIST *relation_mto1_list /* Pass in null */,
		char *many_folder_name,
		LIST *many_folder_primary_key_list,
		boolean single_foreign_key_only );

/* Usage */
/* ----- */
LIST *relation_mto1_isa_list(
		LIST *mto1_isa_list /* Pass in null */,
		char *many_folder_name,
		LIST *many_folder_primary_key_list,
		boolean fetch_relation_one2m_list,
		boolean fetch_relation_mto1_list );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *relation_mto1_where_string(
		const char *folder_primary_key,
		char *many_folder_name );

/* Usage */
/* ----- */

/* Returns heap or program memory */
/* ------------------------------ */
char *relation_mto1_list_display(
		LIST *relation_mto1_list );

/* Usage */
/* ----- */
LIST *relation_mto1_to_one_fetch_list(
		char *one_folder_name,
		LIST *one_folder_primary_key_list );

/* Usage */
/* ----- */
LIST *relation_mto1_without_omit_drillthru_list(
		LIST *relation_mto1_list );

/* Usage */
/* ----- */
LIST *relation_mto1_without_omit_update_list(
		LIST *relation_mto1_list );

/* Usage */
/* ----- */
LIST *relation_mto1_to_one_list(
		LIST *relation_mto1_list,
		LIST *relation_mto1_isa_list );

/* Usage */
/* ----- */
LIST *relation_mto1_common_name_list(
		LIST *folder_attribute_non_primary_name_list,
		boolean copy_common_columns,
		LIST *one_folder_attribute_name_list );
#endif

