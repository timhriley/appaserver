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

typedef struct
{
	/* Input */
	/* ----- */
	char *check_folder_name;
	LIST *primary_key_list;

	/* Attributes */
	/* ---------- */
	char *folder_name;
	char *attribute_not_null;

	/* Process */
	/* ------- */
	LIST *relation_one2m_recursive_list;
	boolean participating;
	char *join;
} ROW_SECURITY_ROLE_UPDATE;

/* ROW_SECURITY_ROLE_UPDATE operations */
/* ----------------------------------- */
ROW_SECURITY_ROLE_UPDATE *row_security_role_update_calloc(
			void );

ROW_SECURITY_ROLE_UPDATE *row_security_role_update_fetch(
			char *check_folder_name,
			LIST *primary_key_list );

LIST *row_security_role_update_list(
			LIST *primary_key_list );

ROW_SECURITY_ROLE_UPDATE *row_security_role_update_parse(
			char *input );

boolean row_security_role_update_participating(
			char *check_folder_name,
			char *folder_name,
			LIST *relation_one2m_recursive_list );

/* Returns heap memory */
/* ------------------- */
char *row_security_role_update_join(
			char *folder_name,
			LIST *primary_key_list,
			LIST *relation_one2m_recursive_list );

/* Returns static memory */
/* --------------------- */
char *row_security_role_update_system_string(
			void );

typedef struct
{
	/* Process */
	/* ------- */
	ROW_SECURITY_ROLE_UPDATE *row_security_role_update;
} ROW_SECURITY_ROLE;

/* ROW_SECURITY_ROLE operations */
/* ---------------------------- */
ROW_SECURITY_ROLE *row_security_role_calloc(
			void );

/* Returns null if not participating */
/* --------------------------------- */
ROW_SECURITY_ROLE *row_security_role_new(
			char *folder_name,
			LIST *primary_key_list,
			boolean role_override_row_restrictions );

boolean row_security_role_viewonly(
			char *delimited_string,
			ROW_SECURITY_ROLE *row_security_row );

typedef struct
{
	LIST *regular_element_list;
	LIST *viewonly_element_list;
	LIST *apply_element_list;
} ROW_SECURITY_ELEMENT_LIST;

/* ROW_SECURITY_ELEMENT_LIST operations */
/* ------------------------------------ */
ROW_SECURITY_ELEMENT_LIST *row_security_element_list_calloc(
			void );

ROW_SECURITY_ELEMENT_LIST *row_security_element_list_new(
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_join_one2m_list,
			DICTIONARY *drillthru_dictionary,
			boolean primary_keys_non_edit,
			LIST *role_operation_list,
			LIST *ignore_select_attribute_name_list,
			char *state,
			char *login_name,
			SECURITY_ENTITY *security_entity,
			LIST *role_exclude_update_attribute_name_list,
			LIST *role_exclude_lookup_attribute_name_list,
			/* ------------------------- */
			/* Null if not participating */
			/* ------------------------- */
			ROW_SECURITY_ROLE *row_security_role );

LIST *row_security_regular_element_list(
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_join_one2m_list,
			DICTIONARY *drillthru_dictionary,
			boolean primary_keys_non_edit,
			LIST *role_operation_list,
			LIST *ignore_select_attribute_name_list,
			char *login_name,
			SECURITY_ENTITY *security_entity,
			LIST *role_exclude_update_attribute_name_list,
			LIST *role_exclude_lookup_attribute_name_list,
			ROW_SECURITY_ROLE *row_security_role );

LIST *row_security_viewonly_element_list(
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_join_one2m_list,
			LIST *role_operation_list,
			LIST *ignore_select_attribute_name_list,
			LIST *role_exclude_lookup_attribute_name_list,
			ROW_SECURITY_ROLE *row_security_role );

/* Always returns */
/* -------------- */
LIST *row_security_operation_element_list(
			LIST *role_operation_list );

LIST *row_security_apply_element_list(
			LIST *regular_element_list,
			LIST *viewonly_element_list,
			char *delimited_string,
			ROW_SECURITY_ROLE *row_security_role );

typedef struct
{
	ROW_SECURITY_ELEMENT_LIST *element_list;
	ROW_SECURITY_ROLE_UPDATE *role_update;
} ROW_SECURITY;

/* ROW_SECURITY operations */
/* ----------------------- */
ROW_SECURITY *row_security_calloc(
			void );

ROW_SECURITY *row_security_edit_table(
			char *folder_name,
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_join_one2m_list,
			DICTIONARY *drillthru_dictionary,
			boolean edit_table_primary_keys_non_edit,
			LIST *role_operation_list,
			LIST *ignore_select_attribute_name_list,
			char *state,
			LIST *role_exclude_update_attribute_name_list,
			LIST *role_exclude_lookup_attribute_name_list,
			boolean folder_non_owner_forbid,
			boolean role_override_row_restrictions,
			char *login_name );

LIST *row_security_update_state_regular_element_list(
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_join_one2m_list,
			DICTIONARY *drillthru_dictionary,
			boolean primary_keys_non_edit,
			LIST *role_operation_list,
			LIST *ignore_select_attribute_name_list,
			LIST *role_exclude_update_attribute_name_list,
			LIST *role_exclude_lookup_attribute_name_list,
			ROW_SECURITY_ROLE_UPDATE *role_update );

LIST *row_security_update_state_viewonly_element_list(
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_join_one2m_list,
			LIST *role_operation_list,
			LIST *ignore_select_attribute_name_list,
			LIST *role_exclude_lookup_attribute_name_list,
			ROW_SECURITY_ROLE_UPDATE *role_update );

#endif
