/* $APPASERVER_HOME/library/row_security.h		   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#ifndef ROW_SECURITY_H
#define ROW_SECURITY_H

#include "list.h"
#include "folder.h"
#include "attribute.h"
#include "element.h"
#include "role.h"
#include "operation_list.h"

typedef struct
{
	/* Attributes */
	/* ---------- */
	char *one_folder_name;
	char *attribute_not_null;

	/* Process */
	/* ------- */
	LIST *relation_one2m_recursive_list;
	boolean participating;
	char *join;
} ROW_SECURITY_ROLE_UPDATE_FOLDER;

/* ROW_SECURITY_ROLE_UPDATE_FOLDER operations */
/* ------------------------------------------ */
ROW_SECURITY_ROLE_UPDATE_FOLDER *row_security_role_update_folder_calloc(
			void );

ROW_SECURITY_ROLE_UPDATE_FOLDER *row_security_role_update_folder_fetch(
			char *folder_name,
			LIST *primary_key_list );

ROW_SECURITY_ROLE_UPDATE_FOLDER *row_security_role_update_folder_parse(
			char *input );

boolean row_security_role_update_folder_participating(
			char *folder_name,
			char *one_folder_name,
			LIST *relation_one2m_recursive_list );

/* Returns heap memory */
/* ------------------- */
char *row_security_role_update_folder_join(
			char *folder_name,
			char *one_folder_name,
			LIST *primary_key_list );

typedef struct
{
	/* Input */
	/* ----- */
	char *folder_name;
	LIST *primary_key_list;
	boolean role_override_restrictions;

	/* Process */
	/* ------- */
	ROW_SECURITY_ROLE_UPDATE_FOLDER *row_security_role_update_folder;
	boolean viewonly;
} ROW_SECURITY_ROLE_UPDATE;

/* ROW_SECURITY_ROLE_UPDATE operations */
/* ----------------------------------- */
ROW_SECURITY_ROLE_UPDATE *row_security_role_update_calloc(
			void );

ROW_SECURITY_ROLE_UPDATE *row_security_role_update_new(
			char *folder_name,
			LIST *primary_key_list,
			boolean role_override_row_restrictions );

boolean row_security_role_update_viewonly(
			boolean role_override_restrictions );

/* Returns static memory */
/* --------------------- */
char *row_security_role_update_system_string(
			char *folder_name );

typedef struct
{
	FOLDER *folder;
	ROLE *role;
	char *login_name;
	LIST *role_update_list;
	char *state;
	DICTIONARY *preprompt_dictionary;
	DICTIONARY *query_dictionary;
	DICTIONARY *sort_dictionary;
	LIST *ignore_attribute_name_list;
	FOLDER *foreign_login_name_folder;
	char *attribute_not_null_join;
	FOLDER *attribute_not_null_folder;
	char *attribute_not_null_string;
	boolean row_security_is_participating;
	LIST *regular_element_list;
	LIST *viewonly_element_list;
	LIST *apply_element_list;
	char query_folder_name[ 256 ];
	char *isa_where_join;
	boolean add_attribute_not_null_to_list;
	LIST *row_dictionary_list;
	RELATION *relation_ajax_fill_drop_down;
} ROW_SECURITY;

/* ROW_SECURITY operations */
/* ----------------------- */
ROW_SECURITY *row_security_calloc(
			void );

ROW_SECURITY *row_security_edit_table(
			char *folder_name,
			LIST *folder_attribute_append_isa_list,
			DICTIONARY *drillthru_dictionary,
			boolean edit_table_primary_keys_non_edit,
			LIST *role_operation_list,
			LIST *ignore_select_attribute_name_list,
			char *state,
			LIST *role_exclude_lookup_attribute_name_list,
			LIST *role_exclude_lookup_attribute_name_list );

#endif
