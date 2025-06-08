/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/row_security.c		  		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "String.h"
#include "timlib.h"
#include "list.h"
#include "sql.h"
#include "piece.h"
#include "dictionary.h"
#include "role_folder.h"
#include "folder_attribute.h"
#include "appaserver_error.h"
#include "application.h"
#include "relation_mto1.h"
#include "relation_one2m.h"
#include "environ.h"
#include "attribute.h"
#include "query.h"
#include "appaserver.h"
#include "table_edit.h"
#include "ajax.h"
#include "row_security.h"

ROW_SECURITY *row_security_calloc( void )
{
	ROW_SECURITY *row_security;

	if ( ! ( row_security = calloc( 1, sizeof ( ROW_SECURITY ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return row_security;
}

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
		int query_row_list_length )
{
	ROW_SECURITY *row_security;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !folder_name
	||   !list_length( folder_attribute_append_isa_list )
	||   !list_length( query_select_name_list )
	||   !table_edit_state )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	row_security = row_security_calloc();

	row_security->regular_boolean =
		row_security_regular_boolean(
			APPASERVER_UPDATE_STATE,
			table_edit_state );

	row_security->viewonly_relation_boolean =
		row_security_viewonly_relation_boolean(
			non_owner_viewonly,
			row_security_role_update_list,
			row_security->regular_boolean );

	row_security->viewonly_no_relation_boolean =
		row_security_viewonly_no_relation_boolean(
			row_security->regular_boolean );

	if ( row_security->regular_boolean )
	{
		row_security->application =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			application_fetch(
				application_name );

		row_security->regular_widget_list =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			row_security_regular_widget_list_new(
				application_name,
				session_key,
				login_name,
				role_name,
				folder_name,
				folder_attribute_non_primary_name_list,
				relation_mto1_list,
				relation_mto1_isa_list,
				relation_one2m_join_list,
				folder_attribute_append_isa_list,
				post_change_javascript,
				drop_down_dictionary,
				no_display_name_list,
				query_select_name_list,
				viewonly_attribute_name_list,
				row_security_role_update_list,
				query_row_list_length,
				row_security->
					application->
					max_query_rows_for_drop_downs,
				row_security->
					application->
					max_drop_down_size );
	}

	if ( row_security->viewonly_relation_boolean )
	{
		row_security->viewonly_widget_list =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			row_security_viewonly_widget_list_new(
				application_name,
				session_key,
				login_name,
				relation_mto1_list,
				relation_mto1_isa_list,
				relation_one2m_join_list,
				folder_attribute_append_isa_list,
				no_display_name_list,
				query_select_name_list,
				viewonly_attribute_name_list,
				row_security_role_update_list );
	}
	else
	if ( row_security->viewonly_no_relation_boolean )
	{
		row_security->viewonly_widget_list =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			row_security_viewonly_widget_list_new(
				application_name,
				session_key,
				login_name,
				(LIST *)0 /* relation_mto1_list */,
				(LIST *)0 /* relation_mto1_isa_list */,
				relation_one2m_join_list,
				folder_attribute_append_isa_list,
				no_display_name_list,
				query_select_name_list,
				viewonly_attribute_name_list,
				row_security_role_update_list );
	}

	return row_security;
}

ROW_SECURITY_REGULAR_WIDGET_LIST *
	row_security_regular_widget_list_calloc(
		void )
{
	ROW_SECURITY_REGULAR_WIDGET_LIST *regular_widget_list;

	if ( ! ( regular_widget_list =
		    calloc( 1,
			    sizeof ( ROW_SECURITY_REGULAR_WIDGET_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return regular_widget_list;
}

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
		int max_drop_down_size )
{
	ROW_SECURITY_REGULAR_WIDGET_LIST *row_security_regular_widget_list;
	FOLDER_ATTRIBUTE *folder_attribute;
	ROW_SECURITY_RELATION *row_security_relation;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !list_rewind( folder_attribute_append_isa_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	row_security_regular_widget_list =
		 row_security_regular_widget_list_calloc();

	row_security_regular_widget_list->widget_container_list = list_new();

	do {
		folder_attribute =
			list_get(
				folder_attribute_append_isa_list );

		if ( !folder_attribute->attribute )
		{
			char message[ 128 ];

			sprintf(message,
				"folder_attribute->attribute is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( !list_string_exists(
			folder_attribute->attribute_name,
			query_select_name_list ) )
		{
			continue;
		}

		if ( list_string_exists(
			folder_attribute->attribute_name,
			no_display_name_list ) )
		{
			if ( folder_attribute->primary_key_index )
			{
				list_set(
					row_security_regular_widget_list->
						widget_container_list,
					widget_container_new(
						hidden,
						folder_attribute->
							attribute_name ) );
			}

			continue;
		}

		if ( !list_string_exists(
			folder_attribute->attribute_name,
			viewonly_attribute_name_list )
		&&   ( row_security_relation =
			row_security_relation_new(
				application_name,
				session_key,
				login_name,
				role_name,
				folder_name
					/* many_folder_name */,
				folder_attribute_non_primary_name_list,
				relation_mto1_list
					/* many_folder_relation_mto1_list */,
				relation_mto1_isa_list,
				folder_attribute->attribute_name,
				folder_attribute->primary_key_index,
				post_change_javascript,
				drop_down_dictionary,
				0 /* not viewonly_boolean */,
				query_row_list_length,
				max_query_rows_for_drop_downs,
				max_drop_down_size,
				row_security_regular_widget_list->
					row_security_relation_list ) ) )
		{
			if ( !row_security_regular_widget_list->
				row_security_relation_list )
				   row_security_regular_widget_list->
					row_security_relation_list =
						list_new();

			list_set(
				row_security_regular_widget_list->
					row_security_relation_list,
				row_security_relation );

			list_set(
				row_security_regular_widget_list->
					widget_container_list,
				widget_container_new(
					table_data,
					(char *)0 /* widget_name */ ) );

			list_set(
				row_security_regular_widget_list->
					widget_container_list,
				row_security_relation->
					widget_container );

			if ( row_security_relation->viewonly_boolean
			&&   folder_attribute->primary_key_index )
			{
				list_set(
					row_security_regular_widget_list->
						widget_container_list,
					widget_container_new(
						hidden,
						row_security_relation->
							relation_mto1->
							relation_name
							/* widget_name */ ) );
			}

			continue;
		}

		if ( ( row_security_regular_widget_list->
			row_security_attribute =
			row_security_attribute_new(
				application_name,
				session_key,
				login_name,
				viewonly_attribute_name_list,
				folder_attribute->attribute_name,
				folder_attribute->primary_key_index,
				folder_attribute->prompt,
				folder_attribute->
					attribute->
					datatype_name,
				folder_attribute->
					attribute->
					width,
				post_change_javascript,
				row_security_regular_widget_list->
					row_security_relation_list ) ) )
		{
			list_set_list(
				row_security_regular_widget_list->
					widget_container_list,
				row_security_regular_widget_list->
					row_security_attribute->
					widget_container_list );
		}

	} while ( list_next( folder_attribute_append_isa_list ) );

	if (	row_security_role_update_list
	&&	row_security_role_update_list->
			row_security_role_update_relation )
	{
		list_set_list(
			row_security_regular_widget_list->widget_container_list,
			row_security_role_update_container_list(
				row_security_role_update_list->
					attribute_not_null ) );
	}

	if ( list_length( relation_one2m_join_list ) )
	{
		list_set_list(
			row_security_regular_widget_list->widget_container_list,
			row_security_join_container_list(
				relation_one2m_join_list ) );
	}

	return row_security_regular_widget_list;
}

ROW_SECURITY_RELATION *row_security_relation_calloc( void )
{
	ROW_SECURITY_RELATION *row_security_relation;

	if ( ! ( row_security_relation =
			calloc( 1, sizeof ( ROW_SECURITY_RELATION ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return row_security_relation;
}

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
		LIST *row_security_relation_list )
{
	ROW_SECURITY_RELATION *row_security_relation;

	if ( !attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "attribute_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_length( many_folder_relation_mto1_list )
	&&   !list_length( relation_mto1_isa_list ) )
	{
		return NULL;
	}

	if ( row_security_relation_attribute_name_exists(
		attribute_name,
		row_security_relation_list ) )
	{
		return NULL;
	}

	row_security_relation = row_security_relation_calloc();

	row_security_relation->relation_mto1 =
		relation_mto1_consumes(
			attribute_name,
			many_folder_relation_mto1_list );

	if ( !row_security_relation->relation_mto1 )
	{
		row_security_relation->relation_mto1 =
			relation_mto1_isa_consumes(
				attribute_name,
				relation_mto1_isa_list );

		if ( !row_security_relation->relation_mto1 )
		{
			free( row_security_relation );
			return NULL;
		}
	}

	if ( primary_key_index
	&&   row_security_relation->
		relation_mto1->
		relation->
		relation_type_isa )
	{
		free( row_security_relation );
		return NULL;
	}

	if ( !row_security_relation->relation_mto1->one_folder_name
	||   !row_security_relation->relation_mto1->one_folder
	||   !list_length(
		row_security_relation->
			relation_mto1->
			one_folder->
			folder_attribute_primary_key_list )
	||   !list_length(
		row_security_relation->
			relation_mto1->
			relation_foreign_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "relation_mto1 is incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( viewonly_boolean )
	{
		row_security_relation->widget_container =
			widget_container_new(
				non_edit_text,
				row_security_relation->
					relation_mto1->
					relation_name );

		goto row_security_relation_heading_set;
	}

	row_security_relation->relation_mto1_to_one_fetch_list =
		relation_mto1_to_one_fetch_list(
			role_name,
			row_security_relation->
				relation_mto1->
				one_folder_name,
			row_security_relation->
				relation_mto1->
				one_folder->
				folder_attribute_primary_key_list );

	if ( row_security_relation->
		relation_mto1->
		relation->
		ajax_fill_drop_down )
	{
		row_security_relation->ajax_client =
			/* --------------------------------- */
			/* Returns null if not participating */
			/* --------------------------------- */
			ajax_client_relation_mto1_new(
				row_security_relation->relation_mto1,
				row_security_relation->
					relation_mto1_to_one_fetch_list,
				0 /* not top_select_boolean */ );
	}
 
	if ( row_security_relation->ajax_client )
	{
		row_security_relation->widget_container =
			row_security_relation_ajax_widget_container(
				/* --------------------------- */
				/* Returns heap memory or null */
				/* --------------------------- */
				relation_mnemonic(
				    row_security_relation->
					relation_mto1->
					relation_foreign_key_list )
						/* widget_name */,
				row_security_relation->
					ajax_client->
					widget_container_list );

		goto row_security_relation_heading_set;
	}

	row_security_relation->relation_mto1_common_name_list =
		relation_mto1_common_name_list(
			folder_attribute_non_primary_name_list,
			row_security_relation->
				relation_mto1->
				relation->
				copy_common_columns,
			row_security_relation->
				relation_mto1->
				one_folder->
				folder_attribute_name_list );

	row_security_relation->query_drop_down =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		query_drop_down_new(
			application_name,
			session_key,
			login_name,
			role_name,
			APPASERVER_UPDATE_STATE,
			many_folder_name,
			row_security_relation->
				relation_mto1->
				one_folder_name,
			row_security_relation->
				relation_mto1->
				one_folder->
				folder_attribute_list,
			row_security_relation->
				relation_mto1->
				one_folder->
				populate_drop_down_process_name,
			row_security_relation->
				relation_mto1_to_one_fetch_list,
			drop_down_dictionary,
			(SECURITY_ENTITY *)0 /* security_entity */,
			row_security_relation->
				relation_mto1_common_name_list );

	row_security_relation->viewonly_boolean =
		row_security_relation_viewonly_boolean(
			TABLE_EDIT_FORCE_DROP_DOWN_ROW_COUNT,
			query_row_list_length,
			max_query_rows_for_drop_downs,
			max_drop_down_size,
			list_length(
				row_security_relation->
				query_drop_down->delimited_list )
				/* delimited_list_length */ );

	if ( row_security_relation->viewonly_boolean )
	{
		row_security_relation->widget_container =
			widget_container_new(
				non_edit_text,
				row_security_relation->
					relation_mto1->
					relation_name );
	}
	else
	{
		row_security_relation->widget_container =
			row_security_relation_drop_down_widget_new(
				application_name,
				login_name,
				post_change_javascript,
				row_security_relation->relation_mto1,
				row_security_relation->
					query_drop_down->
					delimited_list );
	}

row_security_relation_heading_set:

	row_security_relation->widget_container->heading_string =
		row_security_relation->relation_mto1->relation_prompt;

	return row_security_relation;
}

boolean row_security_relation_attribute_name_exists(
			char *attribute_name,
			LIST *row_security_relation_list )
{
	ROW_SECURITY_RELATION *row_security_relation;

	if ( list_rewind( row_security_relation_list ) )
	do {
		row_security_relation =
			list_get(
				row_security_relation_list );

		if ( !row_security_relation->relation_mto1 )
		{
			char message[ 128 ];

			sprintf(message,
			"row_security_relation->relation_mto1 is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( !list_length(
			row_security_relation->
				relation_mto1->
				relation_foreign_key_list ) )
		{
			char message[ 128 ];

			sprintf(message,
		"relation_mto1->relation_foreign_key_list is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( list_string_exists(
			attribute_name,
			row_security_relation->
				relation_mto1->
				relation_foreign_key_list ) )
		{
			return 1;
		}

	} while ( list_next( row_security_relation_list ) );

	return 0;
}

ROW_SECURITY_ATTRIBUTE *row_security_attribute_calloc( void )
{
	ROW_SECURITY_ATTRIBUTE *row_security_attribute;

	if ( ! ( row_security_attribute =
			calloc( 1, sizeof ( ROW_SECURITY_ATTRIBUTE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return row_security_attribute;
}

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
		LIST *row_security_relation_list )
{
	ROW_SECURITY_ATTRIBUTE *row_security_attribute;
	WIDGET_CONTAINER *widget_container;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !attribute_name
	||   !folder_attribute_prompt
	||   !datatype_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( row_security_relation_attribute_name_exists(
		attribute_name,
		row_security_relation_list ) )
	{
		return NULL;
	}

	row_security_attribute = row_security_attribute_calloc();
	row_security_attribute->widget_container_list = list_new();

	list_set(
		row_security_attribute->widget_container_list,
		( widget_container = widget_container_new(
			table_data,
			(char *)0 /* widget_name */ ) ) );

	if ( attribute_is_number( datatype_name ) )
	{
		widget_container->table_data->align_right = 1;
	}

	row_security_attribute->non_edit_boolean =
		row_security_attribute_non_edit_boolean(
			viewonly_attribute_name_list,
			attribute_name,
			datatype_name );

	if ( attribute_is_password( datatype_name ) )
	{
		row_security_attribute->row_security_password =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			row_security_password_new(
				attribute_name,
				attribute_width,
				folder_attribute_prompt,
				row_security_attribute->
					non_edit_boolean
					/* viewonly_boolean */ );

		list_set_list(
			row_security_attribute->widget_container_list,
			row_security_attribute->
				row_security_password->
				widget_container_list );

		return row_security_attribute;
	}

	if ( row_security_attribute->non_edit_boolean )
	{
		row_security_attribute->widget_container =
			widget_container_new(
				non_edit_text,
				attribute_name );

		row_security_attribute->
			widget_container->
			non_edit_text->
			no_initial_capital_boolean = 1;

		if ( primary_key_index )
		{
			list_set(
				row_security_attribute->widget_container_list,
				widget_container_new(
					hidden,
					attribute_name ) );
		}
	}
	else
	if ( attribute_is_yes_no( attribute_name ) )
	{
		row_security_attribute->widget_container =
			widget_container_new(
				yes_no,
				attribute_name );

		row_security_attribute->
			widget_container->
			yes_no->
			post_change_javascript =
				post_change_javascript;
	}
	else
	if ( attribute_is_notepad(
		ATTRIBUTE_DATATYPE_NOTEPAD,
		datatype_name ) )
	{
		row_security_attribute->widget_container =
			widget_container_new(
				notepad,
				attribute_name );

		row_security_attribute->
			widget_container->
			notepad->
			post_change_javascript =
				post_change_javascript;

		row_security_attribute->
			widget_container->
			notepad->
			attribute_size =
				attribute_width;

		row_security_attribute->
			widget_container->
			notepad->
			null_to_slash = 1;
	}
	else
	if ( attribute_is_encrypt( datatype_name ) )
	{
		row_security_attribute->widget_container =
			widget_container_new(
				encrypt,
				attribute_name );

		row_security_attribute->
			widget_container->
			encrypt->
			attribute_width_max_length =
				attribute_width;
	}
	else
	if ( attribute_is_upload( datatype_name ) )
	{
		row_security_attribute->widget_container =
			widget_container_new(
				upload,
				attribute_name );

		row_security_attribute->
			widget_container->
			upload->
			application_name = application_name;

		row_security_attribute->
			widget_container->
			upload->
			session_key = session_key;
	}
	else
	if ( attribute_is_date( datatype_name )
	||   attribute_is_date_time( datatype_name )
	||   attribute_is_current_date_time( datatype_name ) )
	{
		row_security_attribute->widget_container =
			widget_container_new(
				widget_date,
				attribute_name );

		row_security_attribute->
			widget_container->
			date->
			datatype_name = datatype_name;

		row_security_attribute->
			widget_container->
			date->
			application_name = application_name;

		row_security_attribute->
			widget_container->
			date->
			login_name = login_name;

		row_security_attribute->
			widget_container->
			date->
			post_change_javascript = post_change_javascript;

		row_security_attribute->
			widget_container->
			date->
			attribute_width_max_length = attribute_width;

		row_security_attribute->
			widget_container->
			date->
			display_size =
				widget_text_display_size(
					WIDGET_TEXT_DEFAULT_MAX_LENGTH,
					attribute_width );

		row_security_attribute->
			widget_container->
			date->
			null_to_slash = 1;
	}
	else
	{
		row_security_attribute->widget_container =
			widget_container_new(
				character,
				attribute_name );

		row_security_attribute->
			widget_container->
			text->
			post_change_javascript =
				post_change_javascript;

		row_security_attribute->
			widget_container->
			text->
			datatype_name =
				datatype_name;

		row_security_attribute->
			widget_container->
			text->
			attribute_width_max_length =
				attribute_width;

		row_security_attribute->
			widget_container->
			text->
			widget_text_display_size =
				widget_text_display_size(
					WIDGET_TEXT_DEFAULT_MAX_LENGTH,
					attribute_width );

		row_security_attribute->
			widget_container->
			text->
			null_to_slash = 1;

		row_security_attribute->
			widget_container->
			text->
			prevent_carrot_boolean =
				(boolean)primary_key_index;
	}

	row_security_attribute->widget_container->heading_string =
		folder_attribute_prompt;

	list_set(
		row_security_attribute->widget_container_list,
		row_security_attribute->widget_container );

	return row_security_attribute;
}

ROW_SECURITY_VIEWONLY_WIDGET_LIST *
	row_security_viewonly_widget_list_calloc( void )
{
	ROW_SECURITY_VIEWONLY_WIDGET_LIST *
		row_security_viewonly_widget_list;

	if ( ! ( row_security_viewonly_widget_list =
		   calloc(
			1,
			sizeof ( ROW_SECURITY_VIEWONLY_WIDGET_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return row_security_viewonly_widget_list;
}

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
			row_security_role_update_list )
{
	ROW_SECURITY_VIEWONLY_WIDGET_LIST *
		row_security_viewonly_widget_list;
	FOLDER_ATTRIBUTE *folder_attribute;
	ROW_SECURITY_RELATION *row_security_relation;

	if ( !application_name
	||   !session_key
	||   !login_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( folder_attribute_append_isa_list ) ) return NULL;

	row_security_viewonly_widget_list =
		row_security_viewonly_widget_list_calloc();

	row_security_viewonly_widget_list->
		viewonly_attribute_name_list =
			list_new();

	row_security_viewonly_widget_list->
		widget_container_list =
			list_new();

	do {
		folder_attribute =
			list_get(
				folder_attribute_append_isa_list );

		if ( !folder_attribute->attribute )
		{
			char message[ 128 ];

			sprintf(message,
				"folder_attribute->attribute is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( folder_attribute->primary_key_index )
		{
			list_set(
				row_security_viewonly_widget_list->
					widget_container_list,
				widget_container_new(
					hidden,
					folder_attribute->
					     attribute_name ) );
		}

		if ( !list_string_exists(
			folder_attribute->attribute_name,
			query_select_name_list ) )
		{
			continue;
		}

		if ( list_string_exists(
			folder_attribute->attribute_name,
			no_display_name_list ) )
		{
			continue;
		}

		if ( !list_string_exists(
			folder_attribute->attribute_name,
			viewonly_attribute_name_list )
		&&   ( row_security_relation =
			row_security_relation_new(
				(char *)0 /* application_name */,
				(char *)0 /* session_key */,
				(char *)0 /* login_name */,
				(char *)0 /* role_name */,
				(char *)0 /* many_folder_name */,
				(LIST *)0 /* non_primary_name_list */,
				relation_mto1_list
					/* many_folder_relation_mto1_list */,
				relation_mto1_isa_list,
				folder_attribute->attribute_name,
				folder_attribute->primary_key_index,
				(char *)0 /* post_change_javascript */,
				(DICTIONARY *)0 /* drop_down_dictionary */,
				1 /* viewonly_boolean */,
				0 /* query_row_list_length */,
				0 /* max_query_rows_for_drop_downs */,
				0 /* max_drop_down_size */,
				row_security_viewonly_widget_list->
					row_security_relation_list ) ) )
		{
			if ( !row_security_viewonly_widget_list->
				row_security_relation_list )
					row_security_viewonly_widget_list->
						row_security_relation_list =
							list_new();

			list_set(
				row_security_viewonly_widget_list->
					row_security_relation_list,
				row_security_relation );

			list_set(
				row_security_viewonly_widget_list->
					widget_container_list,
				widget_container_new(
					table_data, (char *)0 ) );

			list_set(
				row_security_viewonly_widget_list->
					widget_container_list,
				row_security_relation->
					widget_container );
	
			continue;
		}

		list_set(
			row_security_viewonly_widget_list->
				viewonly_attribute_name_list,
			folder_attribute->attribute_name );

		if ( ( row_security_viewonly_widget_list->
			row_security_attribute =
			row_security_attribute_new(
				application_name,
				session_key,
				login_name,
				row_security_viewonly_widget_list->
					viewonly_attribute_name_list,
				folder_attribute->attribute_name,
				folder_attribute->primary_key_index,
				folder_attribute->prompt,
				folder_attribute->
					attribute->
					datatype_name,
				folder_attribute->
					attribute->
					width,
				(char *)0 /* post_change_javascript */,
				row_security_viewonly_widget_list->
					row_security_relation_list ) ) )
		{
			list_set_list(
				row_security_viewonly_widget_list->
					widget_container_list,
				row_security_viewonly_widget_list->
					row_security_attribute->
					widget_container_list );
		}

	} while ( list_next( folder_attribute_append_isa_list ) );

	if (	row_security_role_update_list
	&&	row_security_role_update_list->
			row_security_role_update_relation )
	{
		list_set_list(
			row_security_viewonly_widget_list->
				widget_container_list,
			row_security_role_update_container_list(
				row_security_role_update_list->
					attribute_not_null ) );
	}

	if ( list_length( relation_one2m_join_list ) )
	{
		list_set_list(
			row_security_viewonly_widget_list->
				widget_container_list,
			row_security_join_container_list(
				relation_one2m_join_list ) );
	}

	return row_security_viewonly_widget_list;
}

boolean row_security_attribute_non_edit_boolean(
		LIST *viewonly_attribute_name_list,
		char *attribute_name,
		char *datatype_name )
{
	if ( list_string_exists(
		attribute_name,
		viewonly_attribute_name_list ) )
	{
		return 1;
	}

	if ( attribute_is_timestamp( datatype_name ) )
	{
		return 1;
	}

	return 0;
}

WIDGET_CONTAINER *row_security_relation_drop_down_widget_new(
		char *application_name,
		char *login_name,
		char *post_change_javascript,
		RELATION_MTO1 *relation_mto1,
		LIST *delimited_list )
{
	WIDGET_CONTAINER *widget_container;

	if ( !application_name
	||   !login_name
	||   !relation_mto1
	||   !relation_mto1->relation_name
	||   !relation_mto1->one_folder
	||   !list_length( relation_mto1->one_folder->folder_attribute_list )
	||   !list_length( relation_mto1->relation_foreign_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	widget_container =
		widget_container_new(
			drop_down,
			relation_mto1->relation_name
				/* widget_name */ );

	widget_container->
		drop_down->
		null_boolean = 1;

	widget_container->
		drop_down->
		top_select_boolean = 1;

	widget_container->
		drop_down->
		display_size = 1;

	widget_container->
		drop_down->
		post_change_javascript =
			post_change_javascript;

	widget_container->
		drop_down->
		foreign_key_list =
			relation_mto1->
				relation_foreign_key_list;

	widget_container->
		drop_down->
		widget_drop_down_option_list =
			widget_drop_down_option_list(
				ATTRIBUTE_MULTI_KEY_DELIMITER,
				WIDGET_DROP_DOWN_LABEL_DELIMITER,
				WIDGET_DROP_DOWN_EXTRA_DELIMITER,
				WIDGET_DROP_DOWN_DASH_DELIMITER,
				delimited_list,
				relation_mto1->
					one_folder->
					no_initial_capital );

	return widget_container;
}

LIST *row_security_join_container_list(
		LIST *relation_one2m_join_list )
{
	LIST *join_widget_container_list;
	RELATION_ONE2M *relation_one2m;
	WIDGET_CONTAINER *drop_down_widget_container;

	if ( !list_rewind( relation_one2m_join_list ) ) return (LIST *)0;

	join_widget_container_list = list_new();

	do {
		relation_one2m = list_get( relation_one2m_join_list );

		if ( !relation_one2m->many_folder
		||   !list_length(
			relation_one2m->
				many_folder->
				folder_attribute_list ) )
		{
			char message[ 128 ];

			sprintf(message,
			"relation_one2m->many_folder is empty or incomplete." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		list_set(
			join_widget_container_list,
			widget_container_new(
				table_data, (char *)0 ) );

		list_set(
			join_widget_container_list,
			( drop_down_widget_container =
				widget_container_new(
					drop_down,
				relation_one2m->many_folder_name ) ) );

		drop_down_widget_container->
			drop_down->
			display_size = 3;

		drop_down_widget_container->
			drop_down->
			multi_select_boolean = 1;

		drop_down_widget_container->
			drop_down->
			foreign_key_list =
				folder_attribute_non_primary_name_list(
					relation_one2m->
						many_folder->
						folder_attribute_list );

		drop_down_widget_container->
			heading_string =
				relation_one2m->many_folder_name;

	} while ( list_next( relation_one2m_join_list ) );

	return join_widget_container_list;
}

LIST *row_security_role_update_container_list(
		char *attribute_not_null )
{
	LIST *list;
	WIDGET_CONTAINER *widget_container;

	if ( !attribute_not_null )
	{
		char message[ 128 ];

		sprintf(message, "attribute_not_null is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	list = list_new();

	list_set(
		list,
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		widget_container_new(
			table_data, (char *)0 ) );

	widget_container =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		widget_container_new(
			character,
			attribute_not_null );

	widget_container->heading_string = attribute_not_null;
	widget_container->text->viewonly = 1;

	list_set( list, widget_container );

	return list;
}

int row_security_relation_viewonly_boolean(
		int table_edit_force_drop_down_row_count,
		int query_row_list_length,
		int max_query_rows_for_drop_downs,
		int max_drop_down_size,
		int delimited_list_length )
{
	if ( query_row_list_length <= table_edit_force_drop_down_row_count )
	{
		if (	delimited_list_length >
			max_query_rows_for_drop_downs * 10 )
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	if ( query_row_list_length <= max_query_rows_for_drop_downs )
		return 0;

	if ( delimited_list_length <= max_drop_down_size )
		return 0;

	return 1;
}

WIDGET_CONTAINER *row_security_relation_ajax_widget_container(
		char *widget_name,
		LIST *ajax_client_widget_container_list )
{
	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	widget_container_ajax_new(
		widget_name,
		ajax_client_widget_container_list );
}

LIST *row_security_relation_ajax_client_list(
		LIST *row_security_relation_list )
{
	LIST *ajax_client_list = list_new();
	ROW_SECURITY_RELATION *row_security_relation;

	if ( list_rewind( row_security_relation_list ) )
	do {
		row_security_relation =
			list_get(
				row_security_relation_list );

		if ( row_security_relation->ajax_client )
		{
			list_set(
				ajax_client_list,
				row_security_relation->
					ajax_client );
		}

	} while ( list_next( row_security_relation_list ) );

	if ( !list_length( ajax_client_list ) )
	{
		list_free( ajax_client_list );
		ajax_client_list = (LIST *)0;
	}

	return ajax_client_list;
}

LIST *row_security_ajax_client_list( ROW_SECURITY *row_security )
{
	if ( row_security
	&&   row_security->regular_widget_list )
	{
		return
		row_security_relation_ajax_client_list(
			row_security->
				regular_widget_list->
				row_security_relation_list );
	}
	else
	{
		return NULL;
	}
}

ROW_SECURITY_PASSWORD *row_security_password_new(
		char *attribute_name,
		int attribute_width,
		char *folder_attribute_prompt,
		boolean viewonly_boolean )
{
	ROW_SECURITY_PASSWORD *row_security_password;
	WIDGET_CONTAINER *line_break_container;

	if ( !attribute_name
	||   !attribute_width
	||   !folder_attribute_prompt )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	row_security_password = row_security_password_calloc();

	row_security_password->widget_container_list = list_new();

	row_security_password->hidden_container =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		widget_container_new(
			hidden,
			attribute_name );

	list_set(
		row_security_password->widget_container_list,
		row_security_password->hidden_container );

	row_security_password->widget_password_display_name =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		widget_password_display_name(
			attribute_name );

	row_security_password->display_container =
		row_security_password_display_container(
			attribute_name,
			attribute_width,
			folder_attribute_prompt,
			viewonly_boolean,
			row_security_password->
				widget_password_display_name );

	list_set(
		row_security_password->widget_container_list,
		row_security_password->display_container );

	line_break_container =
		widget_container_new(
			line_break,
			(char *)0 /* widget_name */ );

	list_set(
		row_security_password->widget_container_list,
		line_break_container );

	row_security_password->checkbox_container =
		row_security_password_checkbox_container(
			attribute_name,
			row_security_password->
				widget_password_display_name );

	list_set(
		row_security_password->widget_container_list,
		row_security_password->checkbox_container );

	return row_security_password;
}

ROW_SECURITY_PASSWORD *row_security_password_calloc( void )
{
	ROW_SECURITY_PASSWORD *row_security_password;

	if ( ! ( row_security_password =
			calloc( 1,
				sizeof ( ROW_SECURITY_PASSWORD ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return row_security_password;
}

WIDGET_CONTAINER *row_security_password_display_container(
		char *attribute_name,
		int attribute_width,
		char *folder_attribute_prompt,
		boolean viewonly_boolean,
		char *widget_password_display_name )
{
	WIDGET_CONTAINER *widget_container;

	if ( !attribute_name
	||   !attribute_width
	||   !folder_attribute_prompt
	||   !widget_password_display_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	widget_container =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		widget_container_new(
			widget_password,
			widget_password_display_name
				/* widget_name */ );

	widget_container->
		password->
		attribute_name = attribute_name;

	widget_container->
		password->
		text->
		attribute_width_max_length =
			attribute_width;

	widget_container->
		password->
		text->
		widget_text_display_size =
			widget_text_display_size(
				WIDGET_TEXT_DEFAULT_MAX_LENGTH,
				attribute_width );

	widget_container->
		password->
		text->
		null_to_slash = 1;

	widget_container->
		password->
		text->
		post_change_javascript =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			row_security_password_display_javascript(
				attribute_name,
				widget_password_display_name );

	widget_container->
		password->
		text->
		viewonly = viewonly_boolean;

	widget_container->heading_string = folder_attribute_prompt;

	return widget_container;
}

char *row_security_password_display_javascript(
		char *attribute_name,
		char *widget_password_display_name )
{
	char display_javascript[ 1024 ];

	if ( !attribute_name
	||   !widget_password_display_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(display_javascript,
		"timlib_password_set( '%s_$row', '%s_$row' )",
		attribute_name,
		widget_password_display_name );

	return strdup( display_javascript );
}

WIDGET_CONTAINER *row_security_password_checkbox_container(
		char *attribute_name,
		char *widget_password_display_name )
{
	WIDGET_CONTAINER *widget_container;

	if ( !attribute_name
	||   !widget_password_display_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	widget_container =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		widget_container_new(
			checkbox,
			(char *)0 /* widget_name */ );

	widget_container->checkbox->prompt = "view";

	widget_container->
		checkbox->
		onclick =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			row_security_password_checkbox_javascript(
				attribute_name,
				widget_password_display_name );

	return widget_container;
}

char *row_security_password_checkbox_javascript(
		char *attribute_name,
		char *widget_password_display_name )
{
	char checkbox_javascript[ 1024 ];

	if ( !attribute_name
	||   !widget_password_display_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(checkbox_javascript,
		"timlib_password_view(this,'%s_$row','%s_$row' )",
		attribute_name,
		widget_password_display_name );

	return strdup( checkbox_javascript );
}

boolean row_security_regular_boolean(
		const char *appaserver_update_state,
		char *table_edit_state )
{
	return
	( string_strcmp(
		(char *)appaserver_update_state,
		table_edit_state ) == 0 );
}

boolean row_security_viewonly_relation_boolean(
		boolean non_owner_viewonly,
		ROW_SECURITY_ROLE_UPDATE_LIST *
			row_security_role_update_list,
		boolean row_security_regular_boolean )
{
	if ( !row_security_regular_boolean ) return 0;

	if ( non_owner_viewonly
	||   row_security_role_update_list )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

boolean row_security_viewonly_no_relation_boolean(
		boolean row_security_regular_boolean )
{
	return !row_security_regular_boolean;
}

