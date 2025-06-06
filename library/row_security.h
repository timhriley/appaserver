/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/row_security.h		   		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef ROW_SECURITY_H
#define ROW_SECURITY_H

#include "list.h"
#include "boolean.h"
#include "folder.h"
#include "widget.h"
#include "role.h"
#include "dictionary.h"
#include "query.h"
#include "security_entity.h"
#include "relation_mto1.h"
#include "row_security_role_update.h"
#include "ajax.h"
#include "application.h"

#define ROW_SECURITY_ASTERISK	"**********"

typedef struct
{
	char *attribute_name;
	LIST *widget_container_list;
	WIDGET_CONTAINER *hidden_container;
	char *widget_password_display_name;
	WIDGET_CONTAINER *display_container;
	WIDGET_CONTAINER *checkbox_container;
} ROW_SECURITY_PASSWORD;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
ROW_SECURITY_PASSWORD *row_security_password_new(
		char *attribute_name,
		int attribute_width,
		char *folder_attribute_prompt,
		boolean viewonly_boolean );

/* Process */
/* ------- */
ROW_SECURITY_PASSWORD *row_security_password_calloc(
		void );

/* Usage */
/* ----- */
WIDGET_CONTAINER *row_security_password_display_container(
		char *attribute_name,
		int attribute_width,
		char *folder_attribute_prompt,
		boolean viewonly_boolean,
		char *widget_password_display_name );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *row_security_password_display_javascript(
		char *attribute_name,
		char *widget_password_display_name );

/* Usage */
/* ----- */
WIDGET_CONTAINER *
	row_security_password_checkbox_container(
		char *attribute_name,
		char *widget_password_display_name );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *row_security_password_checkbox_javascript(
		char *attribute_name,
		char *widget_password_display_name );

typedef struct
{
	LIST *widget_container_list;
	boolean non_edit_boolean;
	ROW_SECURITY_PASSWORD *row_security_password;
	WIDGET_CONTAINER *widget_container;
} ROW_SECURITY_ATTRIBUTE;

/* Usage */
/* ----- */
ROW_SECURITY_ATTRIBUTE *row_security_attribute_new(
		char *application_name,
		char *session_key,
		char *login_name,
		LIST *viewonly_attribute_name_list,
		char *attribute_name,
		int primary_key_index,
		char *folder_attribute_prompt,
		char *datatype_name,
		int attribute_width,
		char *post_change_javascript,
		LIST *row_security_relation_list );

/* Process */
/* ------- */
ROW_SECURITY_ATTRIBUTE *row_security_attribute_calloc(
		void );

/* Usage */
/* ----- */
boolean row_security_attribute_non_edit_boolean(
		LIST *viewonly_attribute_name_list,
		char *attribute_name,
		char *datatype_name );

typedef struct
{
	RELATION_MTO1 *relation_mto1;
	LIST *relation_mto1_to_one_fetch_list;
	LIST *relation_mto1_common_name_list;
	QUERY_DROP_DOWN *query_drop_down;
	boolean viewonly_boolean;
	AJAX_CLIENT *ajax_client;
	WIDGET_CONTAINER *widget_container;
} ROW_SECURITY_RELATION;

/* Usage */
/* ----- */
ROW_SECURITY_RELATION *row_security_relation_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *many_folder_name,
		LIST *folder_attribute_non_primary_name_list,
		LIST *many_folder_relation_mto1_list,
		LIST *relation_mto1_isa_list,
		char *attribute_name,
		int primary_key_index,
		char *post_change_javascript,
		DICTIONARY *drop_down_dictionary,
		boolean viewonly_boolean,
		int query_row_list_length,
		int max_query_rows_for_drop_downs,
		int max_drop_down_size,
		LIST *row_security_relation_list );

/* Process */
/* ------- */
ROW_SECURITY_RELATION *row_security_relation_calloc(
		void );

boolean row_security_relation_attribute_name_exists(
		char *attribute_name,
		LIST *row_security_relation_list );

/* Usage */
/* ----- */
WIDGET_CONTAINER *row_security_relation_drop_down_widget_new(
		char *application_name,
		char *login_name,
		char *post_change_javascript,
		RELATION_MTO1 *relation_mto1,
		LIST *delimited_list );

/* Process */
/* ------- */
int row_security_relation_viewonly_boolean(
		int table_edit_force_drop_down_row_count,
		int query_row_list_length,
		int max_query_rows_for_drop_downs,
		int max_drop_down_size,
		int delimited_list_length );

/* Usage */
/* ----- */
WIDGET_CONTAINER *row_security_relation_ajax_widget_container(
		char *widget_name,
		LIST *ajax_client_widget_container_list );

/* Usage */
/* ----- */
LIST *row_security_relation_ajax_client_list(
		LIST *row_security_relation_list );

typedef struct
{
	LIST *row_security_relation_list;
	ROW_SECURITY_ATTRIBUTE *row_security_attribute;
	LIST *widget_container_list;
} ROW_SECURITY_REGULAR_WIDGET_LIST;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
ROW_SECURITY_REGULAR_WIDGET_LIST *
	row_security_regular_widget_list_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		LIST *folder_attribute_non_primary_name_list,
		LIST *relation_mto1_list,
		LIST *relation_mto1_isa_list,
		LIST *relation_one2m_join_list,
		LIST *folder_attribute_append_isa_list,
		char *post_change_javascript,
		DICTIONARY *drop_down_dictionary,
		LIST *no_display_name_list,
		LIST *query_select_name_list,
		LIST *viewonly_attribute_name_list,
		/* ------------------------- */
		/* Null if not participating */
		/* ------------------------- */
		ROW_SECURITY_ROLE_UPDATE_LIST *
			row_security_role_update_list,
		int query_row_list_length,
		int max_query_rows_for_drop_downs,
		int max_drop_down_size );

/* Process */
/* ------- */
ROW_SECURITY_REGULAR_WIDGET_LIST *
	row_security_regular_widget_list_calloc(
		void );

typedef struct
{
	LIST *viewonly_attribute_name_list;
	LIST *row_security_relation_list;
	ROW_SECURITY_ATTRIBUTE *row_security_attribute;
	LIST *widget_container_list;
} ROW_SECURITY_VIEWONLY_WIDGET_LIST;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
ROW_SECURITY_VIEWONLY_WIDGET_LIST *
	row_security_viewonly_widget_list_new(
		char *application_name,
		char *session_key,
		char *login_name,
		LIST *relation_mto1_list,
		LIST *relation_mto1_isa_list,
		LIST *relation_one2m_join_list,
		LIST *folder_attribute_append_isa_list,
		LIST *no_display_name_list,
		LIST *query_select_name_list,
		LIST *viewonly_attribute_name_list,
		/* ------------------------- */
		/* Null if not participating */
		/* ------------------------- */
		ROW_SECURITY_ROLE_UPDATE_LIST *
			row_security_role_update_list );


/* Process */
/* ------- */
ROW_SECURITY_VIEWONLY_WIDGET_LIST *
	row_security_viewonly_widget_list_calloc(
		void );

typedef struct
{
	boolean regular_boolean;
	boolean viewonly_relation_boolean;
	boolean viewonly_no_relation_boolean;
	APPLICATION *application;
	ROW_SECURITY_REGULAR_WIDGET_LIST *regular_widget_list;
	ROW_SECURITY_VIEWONLY_WIDGET_LIST *viewonly_widget_list;
} ROW_SECURITY;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
ROW_SECURITY *row_security_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		LIST *folder_attribute_non_primary_name_list,
		LIST *relation_mto1_list,
		LIST *relation_mto1_isa_list,
		LIST *relation_one2m_join_list,
		LIST *folder_attribute_append_isa_list,
		char *post_change_javascript,
		boolean non_owner_viewonly,
		DICTIONARY *drop_down_dictionary,
		LIST *no_display_name_list,
		LIST *query_select_name_list,
		LIST *viewonly_attribute_name_list,
		char *table_edit_state,
		/* ------------------------- */
		/* Null if not participating */
		/* ------------------------- */
		ROW_SECURITY_ROLE_UPDATE_LIST *
			row_security_role_update_list,
		int query_row_list_length );

/* Process */
/* ------- */
ROW_SECURITY *row_security_calloc(
		void );

boolean row_security_regular_boolean(
		const char *appaserver_update_state,
		char *table_edit_state );

boolean row_security_viewonly_relation_boolean(
		boolean non_owner_viewonly,
		ROW_SECURITY_ROLE_UPDATE_LIST *
			row_security_role_update_list,
		boolean row_security_regular_boolean );

boolean row_security_viewonly_no_relation_boolean(
		boolean row_security_regular_boolean );

/* Usage */
/* ----- */
LIST *row_security_join_container_list(
		LIST *relation_one2m_join_list );

/* Usage */
/* ----- */
LIST *row_security_role_update_container_list(
		char *attribute_not_null );

/* Usage */
/* ----- */
LIST *row_security_ajax_client_list(
		ROW_SECURITY *row_security );

#endif
