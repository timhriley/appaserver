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
			char *login_name,
			char *session_key,
			char *folder_name,
			char *role_name,
			char *target_frame,
			boolean menu_boolean,
			DICTIONARY *query_dictionary,
			DICTIONARY *ignore_dictionary,
			DICTIONARY *non_prefixed_dictionary,
			DICTIONARY *drillthru_dictionary,
			DICTIONARY *sort_dictionary,
			LIST *ignore_select_attribute_name_list )
{
	EDIT_TABLE *edit_table = edit_table_calloc();

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

		return (EDIT_TABLE_FORM *)0;
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

	edit_table->folder->join_one2m_relation_list =
		relation_join_one2m_list(
			edit_table->folder->relation_one2m_recursive_list,
			ignore_dictionary );

	if ( menu_boolean )
	{
		edit_table->folder_menu =
			folder_menu_fetch(
				application_name,
				session_key,
				appaserver_parameter_data_directory(),
				role_name );

		edit_table->menu =
			menu_fetch(
				application_name,
				login_name,
				session_key,
				role_name,
				target_frame,
				folder_menu->lookup_count_list );
	}

	edit_table->security_entity =
		/* -------------- */
		/* Always returns */
		/* -------------- */
		security_entity_new(
			login_name,
			edit_table->folder->non_owner_forbid,
			edit_table->role->override_row_restrictions );

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
			drillthru_dictionary,
			edit_table->primary_keys_non_edit,
			edit_table->folder->role_operation_list,
			ignore_select_attribute_name_list,
			edit_table->state,
			edit_table->role->exclude_update_attribute_name_list,
			edit_table->role->exclude_lookup_attribute_name_list,
			edit_table->folder->non_owner_forbid,
			edit_table->role->override_row_restrictions,
			login_name,
			security_entity_where( security_entity ) );

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

	edit_table->query_edit_table =
		query_edit_table_new(
			application_name,
			folder_name,
			login_name,
			edit_table->security_entity_where,
			edit_table->folder->relation_join_one2m_list,
			ignore_select_attribute_name_list,
			edit_table->role->exclude_lookup_attribute_name_list,
			edit_table->folder->folder_attribute_append_isa_list,
			edit_table->folder->relation_mto1_non_isa_list,
			edit_table->folder->relation_mto1_isa_list,
			query_dictionary,
			sort_dictionary,
			edit_table->
				row_security_edit_table->
				row_security_role );

	if ( !edit_table->query_edit_table )
	{
		fprintf(stderr,
		"Warning in %s/%s()/%d: query_edit_table(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			folder_name );

		return (EDIT_TABLE *)0;
	}

	edit_table->row_insert_count =
		edit_table_row_insert_count(
			non_prefixed_dictionary );

	edit_table->cell_update_count =
		edit_table_cell_update_count(
			non_prefixed_dictionary );

	edit_table->cell_update_folder_list_string =
		edit_table_cell_update_folder_list_string(
			non_prefixed_dictionary );

	edit_table->results_string =
		edit_table_results_string(
			non_prefixed_dictionary );

	edit_table->submit_action_string =
		edit_table_submit_action_string(
			application_name,
			login_name,
			session_key,
			folder_name,
			role_name,
			target_frame,
			(char *)0 /* detail_base_folder_name */ );

	edit_table->heading_name_list =
		/* --------------------------- */
		/* Returns list of heap memory */
		/* --------------------------- */
		edit_table_heading_name_list(
			edit_table->
				row_security_edit_table->
				row_security_element_list->
				regular_element_list,
			edit_table->
				row_security_edit_table->
				row_security_element_list->
				viewonly_element_list );

	edit_table->key_list =
		edit_table_key_list(
			edit_table->
				row_security_edit_table->
				row_security_element_list->
				regular_element_list,
			edit_table->
				row_security_edit_table->
				row_security_element_list->
				viewonly_element_list );

	edit_table->title =
		edit_table_title(
			folder_name,
			edit_table->state );

	edit_table->message_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		edit_table_message_html(
			edit_table->title,
			edit_table->row_insert_count,
			edit_table->cell_update_count,
			edit_table->results_string );

	edit_table->document_edit_table =
		document_edit_table_new(
			application_name,
			folder_name,
			edit_table->title,
			edit_table->menu,
			edit_table->menu_boolean,
			edit_table->folder->folder_attribute_append_isa_list,
			edit_table->folder->relation_mto1_non_isa_list,
			list_length(
				edit_table->query_edit_table->dictionary_list ),
			edit_table->submit_action_string,
			edit_table->heading_name_list,
			javascript_replace(
				folder->post_change_javascript,
				edit_table->state,
				0 /* row_number */ ),
			edit_table->security_entity );

	if ( !edit_table->document_edit_table )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: document_edit_table_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (EDIT_TABLE *)0;
	}

	edit_table->html =
		/* --------------------------------- */
		/* Returns document_edit_table->html */
		/* --------------------------------- */
		edit_table_html(
			edit_table->
				document_edit_table->
				html );

	edit_table->trailer_html =
		/* ----------------------------------------- */
		/* Returns document_edit_table->trailer_html */
		/* ----------------------------------------- */
		edit_table_trailer_html(
			edit_table->
				document_edit_table->
				trailer_html );

	return edit_table;
}

char *edit_table_state( LIST *role_folder_list )
{
	if ( role_folder_update( role_folder_list ) )
		return APPASERVER_UPDATE_STATE;

	if ( role_folder_lookup( role_folder_list ) )
		return APPASERVER_VIEW_ONLY_STATE;

	return (char *)0;
}

boolean edit_table_primary_keys_non_edit(
			int relation_mto1_isa_list_length )
{
	return ( relation_mto1_isa_list_length >= 1 );
}

int edit_table_row_insert_count(
			DICTIONARY *non_prefixed_dictionary )
{
	char *row_insert_count;

	if ( ( row_insert_count =
		dictionary_get(
			ROWS_INSERTED_COUNT_KEY
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
			DICTIONARY *non_prefixed_dictionary )
{
	char *cell_update_count;

	if ( ( cell_update_count =
		dictionary_get(
			COLUMNS_UPDATED_KEY,
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
			DICTIONARY *non_prefixed_dictionary )
{
	return
		dictionary_get(
			COLUMNS_UPDATED_CHANGED_FOLDER_KEY,
			non_prefixed_dictionary );
}

char *edit_table_results_string(
			DICTIONARY *non_prefixed_dictionary )
{
	return
		dictionary_get(
			non_prefixed_dictionary,
			RESULTS_STRING_KEY );
}

char *edit_table_html( char *document_edit_table_html )
{
	return document_edit_table_html;
}

char *edit_table_trailer_html( char *document_edit_table_trailer_html )
{
	return document_edit_table_tailer_html;
}

void edit_table_output(
			FILE *output_stream,
			char *edit_table_html,
			LIST *row_dictionary_list,
			LIST *edit_table_key_list,
			LIST *regular_element_list,
			LIST *viewonly_element_list,
			ROW_SECURITY_ROLE *row_security_role,
			char *edit_table_trailer_html )
{
	DICTIONARY *row_dictionary;
	LIST *apply_element_list;
	char *html;

	if ( !edit_table_html
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
		"%s\n",
		edit_table_html );

	if ( !list_rewind( row_dictionary_list ) )
	{
		fprintf(output_stream,
			"%s\n",
			edit_table_trailer_html );
		return;
	}

	do {
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
"Warning in %s/%s()/%d: edit_table_apply_element_list() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( ! ( html =
				/* --------------------------- */
				/* Returns heap memory or null */
				/* --------------------------- */
				edit_table_row_html(
					row_dictionary,
					apply_element_list,
					edit_table_key_list,
					edit_table_background_color() ) ) )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: edit_table_row_html() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		fprintf(output_stream,
			"%s\n",
			html );

		free( html );

	} while ( list_next( row_dictionary_list ) );

	fprintf(output_stream,
		"%s\n",
		edit_table_trailer_html );
}

LIST *edit_table_apply_element_list(
			LIST *regular_element_list,
			LIST *viewonly_element_list,
			DICTIONARY *row_dictionary,
			ROW_SECURITY_ROLE *row_security_role )
{
	if ( !row_security_role ) return regular_element_list;

	return row_security_row_apply_element_list(
			regular_element_list,
			viewonly_element_list,
			row_dictionary,
			row_security_role );
}

char *edit_table_row_html(
			DICTIONARY *row_dictionary,
			LIST *apply_element_list,
			LIST *key_list,
			char *background_color )
{
	char html[ STRING_64K ];
	char *ptr = html;
	ELEMENT_APPASERVER *element;
	char current_value[ 4096 ];

	if ( !background_color )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: background_color is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( apply_element_list ) ) return (char *)0;

	ptr += sprintf( ptr, "<tr bgcolor=%s>\n", background_color );

/* Returns heap memory or null */
/* --------------------------- */
char *appaserver_element_list_html(
			char *background_color,
			char *state,
			int row_number,
			LIST *appaserver_element_list )
	do {
		element = list_get( apply_element_list );

		ptr += sprintf(
			ptr,
			"\n%s",
			appaserver_element_html(

			element->drop_down->folder_name,
			non_edit_folder_name_list ) )
		{
			element->drop_down->readonly = 1;
		}

		set_data_flag = 1;

		if ( row_level_non_owner_view_only
		&&   element->element_type == password
		&&   dictionary_login_name
		&&   strcasecmp(
			dictionary_login_name,
			login_name ) != 0 )
		{
			non_edit_element->name = element->name;

			element = non_edit_element;

			element_set_data( element, "hidden" );

			set_data_flag = 0;
		}
		else
		if ( row_level_non_owner_view_only
		&&   element->element_type != toggle_button
		&&   element->element_type != hidden
		&&   dictionary_login_name
		&&   strcasecmp(
			dictionary_login_name,
			login_name ) != 0 )
		{
			non_edit_element->name = element->name;
			element = non_edit_element;
		}
		else
		if ( row_level_non_owner_view_only
		&&   element->element_type == toggle_button
		&&   element->name
		&&   strcmp( element->name, "delete" ) == 0
		&&   dictionary_login_name
		&&   strcasecmp(
			dictionary_login_name,
			login_name ) != 0 )
		{
			non_edit_element->name = "";
			element = non_edit_element;
			set_data_flag = 0;
		}
		else
		if ( element->element_type == reference_number )
		{
			element_set_data(	
				element,
				form_get_next_reference_number(
					form_current_reference_number ) );

			set_data_flag = 0;
		}

		if ( set_data_flag )
		{
			*current_value = '\0';

			if ( dictionary
			&&   element->name
			&&   dictionary_get_index_data_multi(
				current_value,
				dictionary,
				element->name,
				row,
				MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER ) != -1 )
			{
				element_set_data(
					element,
					strdup( current_value ) );
			}
		}

		element_output(
			hidden_name_dictionary,
			element,
			row,
			with_push_buttons,
			stdout,
			background_color,
			application_name,
			login_name );

		fflush( stdout );

		if ( spool_file )
		{
			element_output_as_dictionary(
				spool_file,
				element,
				row );
		}
	} while( list_next( apply_element_list ) );

	return strdup( html );
}

char *edit_table_background_color( void )
{
	static int cycle_count = 0;
	static char **background_color_array = {0};
	static int background_color_array_length = 0;
	char *background_color;

	if ( !background_color_array )
	{
		background_color_array =
			edit_table_background_color_array(
				&background_color_array_length,
				application_name );
	}

	if ( !background_color_array_length )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty background_color_array.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	background_color = background_color_array[ cycle_count ];

	if ( ++cycle_count == background_color_array_length )
		cycle_count = 0;

	return background_color;
}

static char **edit_table_background_color_array(
			int *background_color_array_length,
			char *application_name )
{
	char **background_color_array;
	char *application_constants_color;
	APPLICATION_CONSTANTS *application_constants;

	background_color_array =
		(char **)calloc(FORM_MAX_BACKGROUND_COLOR_ARRAY,
				sizeof( char * ) );

	application_constants = application_constants_new();
	application_constants->dictionary =
		application_constants_get_dictionary(
			application_name );

	if ( ( application_constants_color =
		application_constants_fetch(
			application_constants->dictionary,
			"color1" ) ) )
	{
		background_color_array[ 0 ] = application_constants_color;
		*background_color_array_length = 1;

		if ( ( application_constants_color =
			application_constants_fetch(
				application_constants->dictionary,
				"color2" ) ) )
		{
			background_color_array[ 1 ] =
				application_constants_color;
			*background_color_array_length = 2;
		}
		else
		{
			background_color_array[ 1 ] = "white";
		}

		if ( ( application_constants_color =
			application_constants_fetch(
				application_constants->dictionary,
				"color3" ) ) )
		{
			background_color_array[ 2 ] =
				application_constants_color;
			*background_color_array_length = 3;
		}
		else
		{
			background_color_array[ 2 ] = "white";
		}


		if ( ( application_constants_color =
			application_constants_fetch(
				application_constants->dictionary,
				"color4" ) ) )
		{
			background_color_array[ 3 ] =
				application_constants_color;
			*background_color_array_length = 4;
		}
		else
		{
			background_color_array[ 3 ] = "white";
		}

		if ( ( application_constants_color =
			application_constants_fetch(
				application_constants->dictionary,
				"color5" ) ) )
		{
			background_color_array[ 4 ] =
				application_constants_color;
			*background_color_array_length = 5;
		}
		else
		{
			background_color_array[ 4 ] = "white";
		}


		if ( ( application_constants =
			application_constants_fetch(
				application_constants->dictionary,
				"color6" ) ) )
		{
			background_color_array[ 5 ] =
				application_constants_color;
			*background_color_array_length = 6;
		}
		else
		{
			background_color_array[ 5 ] = "white";
		}


		if ( ( application_constants_color =
			application_constants_fetch(
				application_constants->dictionary,
				"color7" ) ) )
		{
			background_color_array[ 6 ] =
				application_constants_color;
			*background_color_array_length = 7;
		}
		else
		{
			background_color_array[ 6 ] = "white";
		}


		if ( ( application_constants_color =
			application_constants_fetch(
				application_constants->dictionary,
				"color8" ) ) )
		{
			background_color_array[ 7 ] =
				application_constants_color;
			*background_color_array_length = 8;
		}
		else
		{
			background_color_array[ 7 ] = "white";
		}


		if ( ( application_constants_color =
			application_constants_fetch(
				application_constants->dictionary,
				"color9" ) ) )
		{
			background_color_array[ 8 ] =
				application_constants_color;
			*background_color_array_length = 9;
		}
		else
		{
			background_color_array[ 8 ] = "white";
		}


		if ( ( application_constants_color =
			application_constants_fetch(
				application_constants->dictionary,
				"color10" ) ) )
		{
			background_color_array[ 9 ] =
				application_constants_color;
			*background_color_array_length = 10;
		}
		else
		{
			background_color_array[ 9 ] = "white";
		}
	}
	else
	{
		background_color_array[ 0 ] = FORM_COLOR1;
		background_color_array[ 1 ] = FORM_COLOR2;
		background_color_array[ 2 ] = FORM_COLOR3;
		background_color_array[ 3 ] = FORM_COLOR4;
		background_color_array[ 4 ] = FORM_COLOR5;
		*background_color_array_length = 5;
	}
	return background_color_array;
}

char *edit_table_submit_action_string(
			char *application_name,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *role_name,
			char *target_frame,
			char *detail_base_folder_name )
{
	char action_string[ 1024 ];

	sprintf(action_string,
		" action=\"%s/%s?%s+%s+%s+%s+%s+%s+%s\"",
		appaserver_library_http_prompt(
			appaserver_parameter_cgi_directory(),
			appaserver_library_server_address(),
			application_ssl_support_yn(
				application_name ),
			application_prepend_http_protocol_yn(
				application_name ) ),
		"post_edit_table",
		application_name,
		login_name,
		session_key,
		folder_name,
		role_name,
		target_frame,
		(detail_base_folder_name)
			? detail_base_folder_name
			: "" );

	return strdup( action_string );
}

EDIT_TABLE_POST *edit_table_post_calloc( void )
{
	EDIT_TABLE_POST *edit_table_post;

	if ( ! ( edit_table_post = calloc( 1, sizeof( EDIT_TABLE_POST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return edit_table_post;
}

EDIT_TABLE_POST *edit_table_post_new(
			int argc,
			char *argv,
			char *application_name,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *role_name,
			char *target_frame,
			char *detail_base_folder_name )
{
	EDIT_TABLE_POST *edit_table_post = edit_table_post_calloc();

	edit_table_post->target_frame = target_frame;
	edit_table_post->detail_base_folder_name = detail_base_folder_name;

	edit_table_post->session =
		/* --------------------------------------------- */
		/* Sets appaserver environment and outputs argv. */
		/* Each parameter is security inspected.	 */
		/* --------------------------------------------- */
		session_folder_integrity_exit(
			argc,
			argv,
			application_name,
			login_name,
			session_key,
			folder_name,
			role_name,
			state );

	return edit_table_post;
}

char *edit_table_message_html(
			char *edit_table_title,
			int edit_table_row_insert_count,
			int edit_table_cell_update_count,
			char *edit_table_results_string )
{
	char html[ 1024 ];
	char *ptr = html;

	*ptr = '\0';

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

