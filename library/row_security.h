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
#include "relation.h"
#include "role.h"

typedef struct
{
	/* Attributes */
	/* ---------- */
	char *folder_name;
	char *attribute_not_null;

	/* Process */
	/* ------- */
	char *system_string;
	LIST *relation_one2m_recursive_list;
} ROW_SECURITY_ROLE_UPDATE;

/* ROW_SECURITY_ROLE_UPDATE operations */
/* ----------------------------------- */
ROW_SECURITY_ROLE_UPDATE *row_security_role_update_calloc(
			void );

LIST *row_security_role_update_list(
			void );

/* Returns static memory */
/* --------------------- */
char *row_security_role_update_system_string(
			void );

ROW_SECURITY_ROLE_UPDATE *row_security_role_update_parse(
			char *input );

/* Always succeeds */
/* --------------- */
char *row_security_role_update_attribute_not_null(
			char *row_security_role_root_folder_name,
			RELATION *row_security_role_relation,
			LIST *row_security_role_update_list );

typedef struct
{
	/* Attribute */
	/* --------- */
	boolean role_override_row_restrictions;

	/* Process */
	/* ------- */
	LIST *update_list;
	char *root_folder_name;
	RELATION *relation;
	boolean participating;
	char *attribute_not_null;
	char *join;
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

char *row_security_role_root_folder_name(
			char *folder_name,
			LIST *row_security_role_update_list );

RELATION *row_security_role_relation(
			char *folder_name,
			LIST *row_security_role_update_list );

boolean row_security_role_participating(
			char *row_security_role_root_folder_name,
			RELATION *row_security_role_relation );

/* Returns heap memory */
/* ------------------- */
char *row_security_role_join(
			char *folder_name,
			LIST *primary_key_list,
			RELATION *row_security_role_relation );

/* Public */
/* ------ */
LIST *row_security_role_apply_element_list(
			LIST *regular_element_list,
			LIST *viewonly_element_list,
			DICTIONARY *row_dictionary,
			ROW_SECURITY_ROLE *row_security_role );

/* Private */
/* ------- */
boolean row_security_role_viewonly(
			DICTIONARY *row_dictionary,
			char *row_security_role_update_attribute_not_null );

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
			char *security_entity_where,
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
			LIST *row_security_operation_element_list,
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
			LIST *row_security_operation_element_list,
			LIST *ignore_select_attribute_name_list,
			LIST *role_exclude_lookup_attribute_name_list,
			ROW_SECURITY_ROLE *row_security_role );

/* Always returns */
/* -------------- */
LIST *row_security_operation_element_list(
			LIST *role_operation_list,
			boolean viewonly );

typedef struct
{
	ROW_SECURITY_ROLE *row_security_role;
	ROW_SECURITY_ELEMENT_LIST *row_security_element_list;
} ROW_SECURITY;

/* ROW_SECURITY operations */
/* ----------------------- */
ROW_SECURITY *row_security_calloc(
			void );

ROW_SECURITY *row_security_new(
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
			char *login_name,
			char *security_entity_where );

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
