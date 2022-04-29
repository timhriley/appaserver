/* $APPASERVER_HOME/library/row_security.h		   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#ifndef ROW_SECURITY_H
#define ROW_SECURITY_H

#include "list.h"
#include "folder.h"
#include "element.h"
#include "relation.h"
#include "query.h"
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
	/* Attributes */
	/* ---------- */
	char *folder_name;
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

typedef struct
{
	APPASERVER_ELEMENT *table_data_appaserver_element;
	APPASERVER_ELEMENT *attribute_appaserver_element;
	ELEMENT_TEXT *element_text;
	ELEMENT_NOTEPAD *element_notepad;
	ELEMENT_CHECKBOX *element_checkbox;
	ELEMENT_PASSWORD *element_password;
	ELEMENT_NON_EDIT_TEXT *element_non_edit_text;
	ELEMENT_UPLOAD *element_upload;
	LIST *element_list;
} ROW_SECURITY_ATTRIBUTE;

/* ROW_SECURITY_ATTRIBUTE operations */
/* --------------------------------- */
ROW_SECURITY_ATTRIBUTE *row_security_attribute_calloc(
			void );

ROW_SECURITY_ATTRIBUTE *row_security_attribute_new(
			char *attribute_name,
			boolean primary_keys_non_edit,
			int primary_key_index,
			char *datatype_name,
			int attribute_width,
			LIST *exclude_update_attribute_name_list,
			char *post_change_javascript,
			LIST *row_security_relation_list );

typedef struct
{
	/* Process */
	/* ------- */
	RELATION *relation;
	LIST *attribute_name_list;
	QUERY_WIDGET *query_widget;
	APPASERVER_ELEMENT *table_data_appaserver_element;
	char *element_name;
	APPASERVER_ELEMENT *drop_down_appaserver_element;
	LIST *element_list;
} ROW_SECURITY_RELATION;

/* ROW_SECURITY_RELATION operations */
/* -------------------------------- */
ROW_SECURITY_RELATION *row_security_relation_calloc(
			void );

ROW_SECURITY_RELATION *row_security_relation_new(
			char *attribute_name,
			LIST *relation_mto1_non_isa_list,
			char *post_change_javascript,
			DICTIONARY *drillthru_dictionary,
			char *login_name,
			char *security_entity_where,
			boolean viewonly,
			LIST *row_security_relation_list );

boolean row_security_relation_attribute_name_exists(
			char *attribute_name,
			LIST *row_security_relation_list );

LIST *row_security_relation_attribute_name_list(
			LIST *foreign_key_list );

/* Returns heap memory */
/* ------------------- */
char *row_security_relation_element_name(
			char *one_folder_name,
			char *related_attribute_name,
			LIST *foreign_key_list );

typedef struct
{
	LIST *row_security_relation_list;
	LIST *row_security_attribute_list;
	LIST *element_list;
	LIST *attribute_name_list;
} ROW_SECURITY_ELEMENT_LIST_REGULAR;

/* ROW_SECURITY_ELEMENT_LIST_REGULAR operations */
/* -------------------------------------------- */
ROW_SECURITY_ELEMENT_LIST_REGULAR *
	row_security_element_list_regular_calloc(
			void );

ROW_SECURITY_ELEMENT_LIST_REGULAR *
	row_security_element_list_regular_new(
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_join_one2m_list,
			char *post_change_javascript,
			DICTIONARY *drillthru_dictionary,
			boolean primary_keys_non_edit,
			LIST *ignore_select_attribute_name_list,
			char *login_name,
			char *security_entity_where,
			LIST *exclude_update_attribute_name_list,
			LIST *exclude_lookup_attribute_name_list,
			/* ------------------------- */
			/* Null if not participating */
			/* ------------------------- */
			ROW_SECURITY_ROLE *row_security_role );

/* Process */
/* ------- */

typedef struct
{
	LIST *element_list;
	LIST *attribute_name_list;
} ROW_SECURITY_ELEMENT_LIST_VIEWONLY;

/* ROW_SECURITY_ELEMENT_LIST_VIEWONLY operations */
/* --------------------------------------------- */
ROW_SECURITY_ELEMENT_LIST_VIEWONLY *
	row_security_element_list_viewonly_calloc(
			void );

ROW_SECURITY_ELEMENT_LIST_VIEWONLY *
	row_security_element_list_viewonly_new(
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_join_one2m_list,
			LIST *ignore_select_attribute_name_list,
			char *security_entity_where,
			LIST *exclude_lookup_attribute_name_list );

typedef struct
{
	ROW_SECURITY_ELEMENT_LIST_REGULAR *regular;
	ROW_SECURITY_ELEMENT_LIST_VIEWONLY *viewonly;
} ROW_SECURITY_ELEMENT_LIST;

/* ROW_SECURITY_ELEMENT_LIST operations */
/* ------------------------------------ */
ROW_SECURITY_ELEMENT_LIST *row_security_element_list_calloc(
			void );

ROW_SECURITY_ELEMENT_LIST *row_security_element_list_new(
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_join_one2m_list,
			char *post_change_javascript,
			DICTIONARY *drillthru_dictionary,
			boolean primary_keys_non_edit,
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

typedef struct
{
	ROW_SECURITY_ROLE *row_security_role;
	ROW_SECURITY_ELEMENT_LIST *row_security_element_list;
} ROW_SECURITY;

/* ROW_SECURITY operations */
/* ----------------------- */
ROW_SECURITY *row_security_calloc(
			void );

/* Always succeeds */
/* --------------- */
ROW_SECURITY *row_security_new(
			char *folder_name,
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_join_one2m_list,
			char *post_change_javascript,
			DICTIONARY *drillthru_dictionary,
			boolean edit_table_primary_keys_non_edit,
			LIST *no_display_name_list,
			char *state,
			LIST *role_exclude_update_attribute_name_list,
			LIST *role_exclude_lookup_attribute_name_list,
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
			LIST *no_display_name_list,
			LIST *role_exclude_update_attribute_name_list,
			LIST *role_exclude_lookup_attribute_name_list,
			ROW_SECURITY_ROLE_UPDATE *role_update );

LIST *row_security_update_state_viewonly_element_list(
			LIST *folder_attribute_append_isa_list,
			LIST *relation_mto1_non_isa_list,
			LIST *relation_join_one2m_list,
			LIST *role_operation_list,
			LIST *no_display_name_list,
			LIST *role_exclude_lookup_attribute_name_list,
			ROW_SECURITY_ROLE_UPDATE *role_update );

#endif
