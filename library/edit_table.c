/* $APPASERVER_HOME/library/edit_table.c				*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include "String.h"
#include "environ.h"
#include "appaserver_library.h"
#include "appaserver_parameter.h"
#include "row_security.h"
#include "application.h"
#include "application_constants.h"
#include "appaserver.h"
#include "post_edit_table.h"
#include "role_operation.h"
#include "form.h"
#include "edit_table.h"

EDIT_TABLE *edit_table_calloc( void )
{
	EDIT_TABLE *edit_table;

	if ( ! ( edit_table =
			calloc( 1, sizeof( EDIT_TABLE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return edit_table;
}

EDIT_TABLE *edit_table_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *target_frame,
			boolean menu_horizontal_boolean,
			DICTIONARY *original_post_dictionary )
{
	EDIT_TABLE *edit_table = edit_table_calloc();

	/* Used for convenience */
	/* -------------------- */
	ROW_SECURITY_ROLE *row_security_role;
	LIST *viewonly_element_list;

	if ( ! ( edit_table->role =
			role_fetch(
				role_name,
				1 /* fetch_attribute_exclude_list */ ) ) )
 	{
		fprintf(stderr,
		"Warning in %s/%s()/%d: role_fetch(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			role_name );
		return (EDIT_TABLE *)0;
	}

	if ( ! ( edit_table->folder =
			folder_fetch(
				folder_name,
				/* ------------------------- */
				/* Fetching role_folder_list */
				/* ------------------------- */
				role_name,
				role_exclude_lookup_attribute_name_list(
					edit_table->
						role->
						attribute_exclude_list ),
				/* -------------------------- */
				/* Also sets primary_key_list */
				/* -------------------------- */
				1 /* fetch_folder_attribute_list */,
				1 /* fetch_relation_mto1_non_isa_list */,
				/* ------------------------------------------ */
				/* Also sets folder_attribute_append_isa_list */
				/* ------------------------------------------ */
				1 /* fetch_relation_mto1_isa_list */,
				0 /* not fetch_relation_one2m_list */,
				1 /* fetch_relation_one2m_recursive_list */,
				0 /* not fetch_process */,
				1 /* fetch_role_folder_list */,
				1 /* fetch_row_level_restriction */,
				1 /* fetch_role_operation_list */ ) ) )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: folder_fetch(%s) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				folder_name );
		return (EDIT_TABLE *)0;
	}

	edit_table->dictionary_separate =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		dictionary_separate_edit_table_new(
			original_post_dictionary,
			application_name,
			login_name,
			folder_attribute_date_name_list(
				edit_table->
					folder->
					folder_attribute_append_isa_list ),
			edit_table->
				folder->
				folder_attribute_append_isa_list );

	edit_table->folder_attribute_date_name_list_length =
		list_length(
			folder_attribute_date_name_list(
				edit_table->
					folder->
					folder_attribute_append_isa_list ) );

	edit_table->folder->relation_join_one2m_list =
		relation_join_one2m_list(
			edit_table->folder->relation_one2m_recursive_list,
			edit_table->
				dictionary_separate->
				no_display_dictionary );

	if ( menu_horizontal_boolean )
	{
		edit_table->folder_menu =
			folder_menu_new(
				application_name,
				session_key,
				role_name,
				appaserver_parameter_data_directory() );

		edit_table->menu =
			menu_new(
				application_name,
				session_key,
				login_name,
				role_name,
				1 /* frameset_menu_horizontal */,
				edit_table->folder_menu->count_list );
	}

	edit_table->security_entity =
		/* -------------- */
		/* Always returns */
		/* -------------- */
		security_entity_new(
			login_name,
			edit_table->folder->non_owner_forbid,
			edit_table->role->override_row_restrictions );

	edit_table->security_entity_where =
		security_entity_where(
			edit_table->security_entity,
			edit_table->
				folder->
				folder_attribute_list );

	edit_table->state =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		edit_table_state(
			edit_table->
				folder->
				role_folder_list );

	edit_table->primary_keys_non_edit =
		edit_table_primary_keys_non_edit(
			list_length(
				edit_table->
					folder->
					relation_mto1_isa_list ) );

	edit_table->row_security =
		row_security_new(
			folder_name,
			edit_table->folder->folder_attribute_append_isa_list,
			edit_table->folder->relation_mto1_non_isa_list,
			edit_table->folder->relation_join_one2m_list,
			edit_table->folder->post_change_javascript,
			edit_table->dictionary_separate->
				drillthru_dictionary,
			edit_table->primary_keys_non_edit,
			edit_table->
				dictionary_separate->
				no_display_name_list,
			edit_table->state,
			role_exclude_update_attribute_name_list(
				edit_table->
					role->
					attribute_exclude_list ),
			role_exclude_lookup_attribute_name_list(
				edit_table->
					role->
					attribute_exclude_list ),
			edit_table->role->override_row_restrictions,
			login_name,
			edit_table->security_entity_where );

	if ( !edit_table->row_security )
	{
		fprintf(stderr,
		"Warning in %s/%s()/%d: row_security_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (EDIT_TABLE *)0;
	}

	if ( !edit_table->row_security->row_security_element_list )
	{
		fprintf(stderr,
		"Warning in %s/%s()/%d: row_security_element_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (EDIT_TABLE *)0;
	}

	row_security_role =
		edit_table->
			row_security->
			row_security_role;

	edit_table->query_edit_table =
		query_edit_table_new(
			application_name,
			login_name,
			folder_name,
			edit_table->security_entity_where,
			edit_table->folder->relation_join_one2m_list,
			edit_table->
				dictionary_separate->
				no_display_name_list,
			role_exclude_lookup_attribute_name_list(
				edit_table->
					role->
					attribute_exclude_list ),
			edit_table->folder->folder_attribute_append_isa_list,
			edit_table->folder->relation_mto1_non_isa_list,
			edit_table->folder->relation_mto1_isa_list,
			edit_table->
				dictionary_separate->
				query_dictionary,
			edit_table->
				dictionary_separate->
				sort_dictionary,
			(row_security_role)
				? row_security_role->folder_name
				: (char *)0,
			(row_security_role)
				? row_security_role->relation
				: (RELATION *)0,
			(row_security_role)
				? row_security_role->attribute_not_null
				: (char *)0 );

	if ( !edit_table->query_edit_table )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: query_edit_table_new(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			folder_name );
		return (EDIT_TABLE *)0;
	}

	edit_table->spool_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		edit_table_spool_filename(
			appaserver_parameter_data_directory(),
			application_name,
			folder_name,
			session_key );

	edit_table->dictionary_list_length =
		list_length(
			edit_table->
				query_edit_table->
				row_dictionary_list );

	edit_table->row_insert_count =
		edit_table_row_insert_count(
			ROWS_INSERTED_COUNT_KEY,
			edit_table->
				dictionary_separate->
				non_prefixed_dictionary );

	edit_table->cell_update_count =
		edit_table_cell_update_count(
			COLUMNS_UPDATED_KEY,
			edit_table->
				dictionary_separate->
				non_prefixed_dictionary );

	edit_table->cell_update_folder_list_string =
		edit_table_cell_update_folder_list_string(
			COLUMNS_UPDATED_CHANGED_FOLDER_KEY,
			edit_table->
				dictionary_separate->
				non_prefixed_dictionary );

	edit_table->results_string =
		edit_table_results_string(
			RESULTS_STRING_KEY,
			edit_table->
				dictionary_separate->
				non_prefixed_dictionary );

	edit_table->post_edit_table_action_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		post_edit_table_action_string(
			POST_EDIT_TABLE_EXECUTABLE,
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			target_frame,
			(char *)0 /* detail_base_folder_name */ );

	if ( !edit_table->post_edit_table_action_string )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: post_edit_table_action_string() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}


	if ( edit_table->row_security->row_security_element_list->viewonly )
	{
		viewonly_element_list =
			edit_table->
				row_security->
				row_security_element_list->
				viewonly->
				element_list;
	}
	else
	{
		viewonly_element_list = (LIST *)0;
	}

	edit_table->heading_name_list =
		/* --------------------------- */
		/* Returns list of heap memory */
		/* --------------------------- */
		edit_table_heading_name_list(
			edit_table->
				row_security->
				row_security_element_list->
				regular->
				element_list,
			viewonly_element_list );

	edit_table->title_html =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		edit_table_title_html(
			folder_name,
			edit_table->state );

	edit_table->message_html =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		edit_table_message_html(
			edit_table->row_insert_count,
			edit_table->cell_update_count,
			edit_table->results_string );

	edit_table->document =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		document_new(
			application_name,
			application_title_string( application_name ),
			edit_table->title_html,
			edit_table->message_html,
			(char *)0 /* subsubtitle_html */,
			(char *)0 /* javascript_replace */,
			edit_table->menu_boolean,
			edit_table->menu,
			document_head_menu_setup_string(
				menu_horizontal_boolean ),
			document_head_calendar_setup_string(
				edit_table->
				      folder_attribute_date_name_list_length ),
			document_head_javascript_include_string(),
			(char *)0 /* input_onload_string */ );

	edit_table->form_edit_table =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		form_edit_table_new(
			folder_name,
			edit_table->folder->post_change_javascript,
			edit_table->dictionary_list_length,
			edit_table->post_edit_table_action_string,
			edit_table->folder->role_operation_list,
			edit_table->heading_name_list,
			target_frame,
			edit_table->
				dictionary_separate->
				query_dictionary,
			edit_table->
				dictionary_separate->
				sort_dictionary,
			edit_table->
				dictionary_separate->
				drillthru_dictionary,
			edit_table->
				dictionary_separate->
				no_display_dictionary  );

	edit_table->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		edit_table_html(
			edit_table->document->html,
			edit_table->document->document_head->html,
			document_head_close_html(),
			edit_table->document->document_body->html );

	edit_table->trailer_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		edit_table_trailer_html(
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_body_close_html(),
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_close_html() );

	return edit_table;
}

char *edit_table_state( LIST *role_folder_list )
{
	if ( role_folder_update( role_folder_list ) )
		return APPASERVER_UPDATE_STATE;

	if ( role_folder_lookup( role_folder_list ) )
		return APPASERVER_VIEWONLY_STATE;

	return (char *)0;
}

boolean edit_table_primary_keys_non_edit(
			int relation_mto1_isa_list_length )
{
	return ( relation_mto1_isa_list_length >= 1 );
}

int edit_table_row_insert_count(
			char *rows_inserted_count_key,
			DICTIONARY *non_prefixed_dictionary )
{
	char *row_insert_count;

	if ( dictionary_length( non_prefixed_dictionary ) )
		return 0;

	if ( ( row_insert_count =
		dictionary_get(
			rows_inserted_count_key,
			non_prefixed_dictionary ) ) )
	{
		return atoi( row_insert_count );
	}
	else
	{
		return 0;
	}
}

int edit_table_cell_update_count(
			char *columns_updated_key,
			DICTIONARY *non_prefixed_dictionary )
{
	char *cell_update_count;

	if ( dictionary_length( non_prefixed_dictionary ) )
		return 0;

	if ( ( cell_update_count =
		dictionary_get(
			columns_updated_key,
			non_prefixed_dictionary ) ) )
	{
		return atoi( cell_update_count );
	}
	else
	{
		return 0;
	}
}

char *edit_table_cell_update_folder_list_string(
			char *columns_updated_changed_folder_key,
			DICTIONARY *non_prefixed_dictionary )
{
	if ( dictionary_length( non_prefixed_dictionary ) )
		return (char *)0;

	return
		dictionary_get(
			columns_updated_changed_folder_key,
			non_prefixed_dictionary );
}

char *edit_table_results_string(
			char *results_string_key,
			DICTIONARY *non_prefixed_dictionary )
{
	if ( dictionary_length( non_prefixed_dictionary ) )
		return (char *)0;

	return
		dictionary_get(
			results_string_key,
			non_prefixed_dictionary );
}

char *edit_table_html(	char *document_html,
			char *document_head_html,
			char *document_head_close_html,
			char *document_body_html )
{
	char html[ STRING_64K ];

	if ( !document_html
	||   !document_head_html
	||   !document_head_close_html
	||   !document_body_html )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(html,
		"%s\n%s\n%s\n%s",
		document_html,
		document_head_html,
		document_head_close_html,
		document_body_html );

	return strdup( html );
}

char *edit_table_trailer_html(
			char *document_body_close_html,
			char *document_close_html )
{
	char trailer_html[ 1024 ];

	if ( !document_body_close_html
	||   !document_close_html )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}


	sprintf(trailer_html,
		"%s\n%s",
		document_body_close_html,
		document_close_html );

	return strdup( trailer_html );
}

LIST *edit_table_apply_element_list(
			LIST *regular_element_list,
			LIST *viewonly_element_list,
			DICTIONARY *row_dictionary,
			ROW_SECURITY_ROLE *row_security_role )
{
	if ( !row_security_role )
	{
		if ( regular_element_list )
			return regular_element_list;
		else
			return viewonly_element_list;
	}
	else
	if ( edit_table_viewonly(
			row_dictionary,
			row_security_role->attribute_not_null ) )
	{
		return viewonly_element_list;
	}
	else
	{
		return regular_element_list;
	}
}

char *edit_table_row_html(
			LIST *apply_element_list,
			LIST *role_operation_list,
			char *application_name,
			char *background_color,
			ROW_SECURITY_ROLE *row_security_role,
			char *state,
			int row_number,
			DICTIONARY *row_dictionary )
{
	ROLE_OPERATION *role_operation;
	char row_html[ STRING_ONE_MEG ];
	char *ptr = row_html;
	char *html;

	if ( list_rewind( role_operation_list ) )
	{
		do {
			role_operation =
				list_get(
					role_operation_list );

			if ( !role_operation->operation )
			{
				fprintf(stderr,
			"ERROR in %s/%s()/%d: operation is empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}

			if ( !role_operation->operation->appaserver_element )
			{
				fprintf(stderr,
			"ERROR in %s/%s()/%d: appaserver_element is empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}

			if ( !role_operation->
				operation->
				appaserver_element->
				checkbox )
			{
				fprintf(stderr,
			"ERROR in %s/%s()/%d: checkbox is empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}

			html =
			    operation_html(
				    role_operation->
					    operation->
					    appaserver_element->
					    checkbox,
				    state,
				    row_number,
				    background_color,
				    edit_table_viewonly(
					    row_dictionary,
					    (row_security_role)
						? row_security_role->
						     attribute_not_null
						: (char *)0 )
						/* delete_mask_boolean */ );

			if ( !html )
			{
				fprintf(stderr,
		"ERROR in %s/%s()/%d: operation_html(%s) returned empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__,
					role_operation->
						operation->
						operation_name );
				exit( 1 );
			}

			ptr += sprintf(
				ptr,
				"%s%s\n",
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				element_table_data_html(
					0 /* not align_right */,
					1 /* column_span */ ),
				html );

			free( html );

		} while ( list_next( role_operation_list ) );
	}

	html =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		appaserver_element_list_row_dictionary_html(
			apply_element_list /* in/out */,
			application_name,
			background_color,
			state,
			row_number,
			row_dictionary );

	if ( !html )
	{
		fprintf(stderr,
"Warning in %s/%s()/%d: appaserver_element_list_row_dictionary_html() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
	}
	else
	{
		ptr += sprintf( ptr, "%s\n", html );
		free( html );
	}

	if ( ptr == row_html )
		return (char *)0;
	else
		return strdup( row_html );
}

char *edit_table_message_html(
			int row_insert_count,
			int cell_update_count,
			char *results_string )
{
	char html[ 1024 ];
	char *ptr = html;

	if ( row_insert_count )
	{
		if ( row_insert_count == 1 )
		{
			ptr += sprintf(
				ptr,
				"<h2>1 row inserted</h2>" );
		}
		else
		{
			ptr += sprintf(
				ptr,
				"<h2>%d rows inserted</h2>",
				row_insert_count );
		}
	}

	if ( cell_update_count )
	{
		if ( cell_update_count == 1 )
		{
			ptr += sprintf(
				ptr,
				"<h2>1 cell changed</h2>" );
		}
		else
		{
			ptr += sprintf(
				ptr,
				"<h2>%d cells changed</h2>",
				cell_update_count );
		}
	}

	if ( results_string && *results_string )
	{
		if ( ptr != html ) ptr += sprintf( ptr, "\n" );

		ptr += sprintf( ptr, "<h2>%s</h2>", results_string );
	}

	if ( ptr == html )
		return (char *)0;
	else
		return strdup( html );
}

LIST *edit_table_heading_name_list(
			LIST *regular_element_list,
			LIST *viewonly_element_list )
{
	LIST *apply_element_list;

	if ( regular_element_list )
	{
		apply_element_list = regular_element_list;
	}
	else
	{
		apply_element_list = viewonly_element_list;
	}

	if ( !list_length( apply_element_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: both parameters are empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	/* --------------------------- */
	/* Returns list of heap memory */
	/* --------------------------- */
	appaserver_element_heading_name_list( apply_element_list );
}

void edit_table_output(	FILE *output_stream,
			char *application_name,
			char *edit_table_html,
			char *form_edit_table_html,
			LIST *role_operation_list,
			LIST *row_dictionary_list,
			LIST *regular_element_list,
			LIST *viewonly_element_list,
			ROW_SECURITY_ROLE *row_security_role,
			char *edit_table_state,
			char *form_edit_table_trailer_html,
			char *edit_table_trailer_html )
{
	if ( !edit_table_html
	||   !form_edit_table_html
	||   !edit_table_state
	||   !form_edit_table_trailer_html
	||   !edit_table_trailer_html )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return;
	}

	fprintf(output_stream,
		"%s\n%s\n",
		edit_table_html,
		form_edit_table_html );

	edit_table_apply_output(
		output_stream,
		application_name,
		role_operation_list,
		row_dictionary_list,
		regular_element_list,
		viewonly_element_list,
		row_security_role,
		edit_table_state );

	fprintf(output_stream,
		"%s\n",
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		element_table_close_html() );

	edit_table_hidden_output(
		output_stream,
		row_dictionary_list,
		regular_element_list,
		viewonly_element_list );

	fprintf(output_stream,
		"%s\n%s\n",
		form_edit_table_trailer_html,
		edit_table_trailer_html );
}

void edit_table_apply_output(
			FILE *output_stream,
			char *application_name,
			LIST *role_operation_list,
			LIST *row_dictionary_list,
			LIST *regular_element_list,
			LIST *viewonly_element_list,
			ROW_SECURITY_ROLE *row_security_role,
			char *state )
{
	DICTIONARY *row_dictionary;
	LIST *apply_element_list;
	char *html;
	int row_number;
	char *background_color;
	int list_len = list_length( row_dictionary_list );

	if ( !list_len ) return;

	for(	row_number = 1, list_rewind( row_dictionary_list );
		row_number <= list_len;
		row_number++, list_next( row_dictionary_list ) )
	{
		row_dictionary = list_get( row_dictionary_list );

		apply_element_list =
			edit_table_apply_element_list(
				regular_element_list,
				viewonly_element_list,
				row_dictionary,
				row_security_role );

		if ( !apply_element_list )
		{
			fprintf(stderr,
"ERROR in %s/%s()/%d: edit_table_apply_element_list() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		background_color = form_background_color();

		if ( ! ( html =
				/* --------------------------- */
				/* Returns heap memory or null */
				/* --------------------------- */
				edit_table_row_html(
					apply_element_list /* in/out */,
					role_operation_list,
					application_name,
					background_color,
					row_security_role,
					state,
					row_number,
					row_dictionary ) ) )
		{
			fprintf(stderr,
	"Warning in %s/%s()/%d: edit_table_row_html() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			continue;
		}

		fprintf(output_stream,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			element_table_row_html(
				background_color ) );

		fprintf(output_stream,
			"%s\n",
			html );

		free( html );
	}
}

void edit_table_hidden_output(
			FILE *output_stream,
			LIST *row_dictionary_list,
			LIST *regular_element_list,
			LIST *viewonly_element_list )
{
	DICTIONARY *row_dictionary;
	char *html;
	int row_number;
	LIST *apply_element_list;
	int list_len = list_length( row_dictionary_list );

	if ( !list_len ) return;

	if ( regular_element_list )
		apply_element_list = regular_element_list;
	else
		apply_element_list = viewonly_element_list;

	if ( !apply_element_list )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: apply_element_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	for(	list_rewind( row_dictionary_list ), row_number = 1;
		row_number <= list_len;
		list_next( row_dictionary_list ), row_number++ )
	{
		row_dictionary = list_get( row_dictionary_list );

		if ( ( html =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			appaserver_element_list_hidden_html(
				apply_element_list /* in/out */,
				row_number,
				row_dictionary ) ) )
		{
			fprintf(output_stream,
				"%s\n",
				html );

			free( html );
		}
	}
}

char *edit_table_output_system_string(
			char *executable,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *target_frame,
			char *subsub_title,
			char *dictionary_separate_send_string,
			char *appaserver_error_filename )
{
	char system_string[ STRING_8K ];

	if ( !executable
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !target_frame
	||   !appaserver_error_filename )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"%s %s %s %s %s %s \"%s\" \"%s\" 2>>%s",
		executable,
		session_key,
		login_name,
		role_name,
		folder_name,
		target_frame,
		(subsub_title )
			? subsub_title
			: "",
		(dictionary_separate_send_string)
			? dictionary_separate_send_string
			: "",
		appaserver_error_filename );

	return strdup( system_string );
}

char *edit_table_title_html(
			char *folder_name,
			char *state )
{
	static char html[ 128 ];
	char buffer1[ 64 ];
	char buffer2[ 64 ];

	if ( !folder_name || !state )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(html,
		"<h1>%s %s</h1>",
		string_initial_capital(
			buffer1,
			state ),
		string_initial_capital(
			buffer2,
			folder_name ) );

	return html;
}

boolean edit_table_viewonly(
			DICTIONARY *row_dictionary,
			char *attribute_not_null )
{
	char *data;

	if ( !attribute_not_null ) return 0;

	data =
		dictionary_get(
			attribute_not_null,
			row_dictionary );

	if ( data && *data )
		return 1;
	else
		return 0;
}

char *edit_table_spool_filename(
			char *appaserver_data_directory,
			char *application_name,
			char *folder_name,
			char *session_key )
{
	char filename[ 128 ];

	if ( !appaserver_data_directory
	||   !application_name
	||   !folder_name
	||   !session_key )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(filename,
		"%s/update_%s_%s_%s.dat",
		appaserver_data_directory,
		application_name,
		folder_name,
		session_key );

	return strdup( filename );
}

void edit_table_spool_file(
			char *spool_filename,
			LIST *folder_attribute_name_list,
			LIST *row_dictionary_list,
			char sql_delimiter )
{
	FILE *output_file;
	DICTIONARY *row_dictionary;
	char *string;
	int row_number;
	int list_len = list_length( row_dictionary_list );

	if ( !list_len ) return;

	if ( ! ( output_file = fopen( spool_filename, "w" ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: fopen(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			spool_filename );
		exit( 1 );
	}

	for(	row_number = 1, list_rewind( row_dictionary_list );
		row_number <= list_len;
		row_number++, list_next( row_dictionary_list ) )
	{
		row_dictionary = list_get( row_dictionary_list );

		string =
			/* ---------------------------- */
			/* Returns heap memory or null. */
			/* Note: has trailing CR.	*/
			/* ---------------------------- */
			dictionary_attribute_name_list_string(
				row_dictionary,
				folder_attribute_name_list,
				sql_delimiter,
				row_number );

		if ( !string )
		{
			fprintf(stderr,
"Warning in %s/%s()/%d: dictionary_attribute_name_list_string(%s) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				dictionary_display_delimiter(
					row_dictionary,
					sql_delimiter ) );
			continue;
		}

		fprintf(output_file,
			"%s",
			string );

		free( string );
	}

	fclose( output_file );
}

