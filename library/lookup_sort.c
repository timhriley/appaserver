/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/lookup_sort.c	   		   	*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "application.h"
#include "appaserver_parameter.h"
#include "appaserver_error.h"
#include "relation_mto1.h"
#include "appaserver.h"
#include "lookup_sort.h"

LOOKUP_SORT *lookup_sort_new(
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		DICTIONARY *original_post_dictionary )
{
	LOOKUP_SORT *lookup_sort;

	if ( !application_name
	||   !login_name
	||   !role_name
	||   !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	lookup_sort = lookup_sort_calloc();

	lookup_sort->lookup_sort_input =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		lookup_sort_input_new(
			application_name,
			login_name,
			role_name,
			folder_name,
			original_post_dictionary );

	lookup_sort->query_primary_key =
		/* ----------------------- */
		/* Note: frees each where. */
		/* ----------------------- */
		query_primary_key_fetch(
			application_name,
			login_name,
			folder_name,
			lookup_sort->
				lookup_sort_input->
				security_entity->
				where,
			lookup_sort->
				lookup_sort_input->
				relation_mto1_list,
			lookup_sort->
				lookup_sort_input->
				relation_mto1_isa_list,
			lookup_sort->
				lookup_sort_input->
				folder_attribute_append_isa_list,
			lookup_sort->
				lookup_sort_input->
				dictionary_separate->
				non_prefixed_dictionary
					/* query_dictionary */,
			lookup_sort->
				lookup_sort_input->
				attribute_name
				/* additional_attribute_name */ );

	lookup_sort->row_count =
		lookup_sort_row_count(
			list_length(
				lookup_sort->
					query_primary_key->
					query_fetch->
					row_list )
						/* row_list_length */ );

	return lookup_sort;
}

LOOKUP_SORT *lookup_sort_calloc( void )
{
	LOOKUP_SORT *lookup_sort;

	if ( ! ( lookup_sort = calloc( 1, sizeof ( LOOKUP_SORT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return lookup_sort;
}

char *lookup_sort_input_attribute_name(
		char *attribute_name_sort_order,
		char *attribute_name_display_order,
		char *attribute_name_sequence_number,
		LIST *folder_attribute_name_list )
{
	char *return_attribute = {0};

	if ( list_exists_string(
		attribute_name_sort_order,
		folder_attribute_name_list ) )
	{
		return_attribute = attribute_name_sort_order;
	}
	else
	if ( list_exists_string(
		attribute_name_display_order,
		folder_attribute_name_list ) )
	{
		return_attribute = attribute_name_display_order;
	}
	else
	if ( list_exists_string(
		attribute_name_sequence_number,
		folder_attribute_name_list ) )
	{
		return_attribute = attribute_name_sequence_number;
	}

	return return_attribute;
}

int lookup_sort_row_count( int row_list_length )
{
	return row_list_length;
}

LOOKUP_SORT_FORM *lookup_sort_form_new(
		char *lookup_sort_executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		DICTIONARY *original_post_dictionary )
{
	LOOKUP_SORT_FORM *lookup_sort_form;

	if ( !lookup_sort_executable
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	lookup_sort_form = lookup_sort_form_calloc();

	lookup_sort_form->lookup_sort =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		lookup_sort_new(
			application_name,
			login_name,
			role_name,
			folder_name,
			original_post_dictionary );

	if ( lookup_sort_form->lookup_sort->row_count == 0 )
		return lookup_sort_form;

	lookup_sort_form->javascript_filename_list =
		lookup_sort_form_javascript_filename_list();

	lookup_sort_form->action_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		lookup_sort_form_action_string(
			LOOKUP_SORT_EXECUTABLE,
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name );

	lookup_sort_form->form_lookup_sort =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		form_lookup_sort_new(
			lookup_sort_form->
				lookup_sort->
				lookup_sort_input->
				folder->
				folder_attribute_primary_key_list,
			lookup_sort_form->
				lookup_sort->
				lookup_sort_input->
				attribute_name,
			lookup_sort_form->
				lookup_sort->
				query_primary_key->
				query_fetch->
				row_list,
			lookup_sort_form->action_string );

	return lookup_sort_form;
}

LOOKUP_SORT_FORM *lookup_sort_form_calloc( void )
{
	LOOKUP_SORT_FORM *lookup_sort_form;

	if ( ! ( lookup_sort_form =
			calloc( 1,
				sizeof ( LOOKUP_SORT_FORM ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return lookup_sort_form;
}

LIST *lookup_sort_form_javascript_filename_list( void )
{
	LIST *list = list_new();

	list_set( list, "trim.js" );
	list_set( list, "form.js" );
	list_set( list, "sort_order.js" );

	return list;
}

char *lookup_sort_form_action_string(
		char *lookup_sort_executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name )
{
	char action_string[ 1024 ];

	if ( !lookup_sort_executable
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(action_string,
		"%s/%s?%s+%s+%s+%s+%s+two",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		application_http_prompt(
			appaserver_parameter_cgi_directory(),
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			application_server_address(),
			application_ssl_support_boolean( application_name ) ),
		lookup_sort_executable,
		application_name,
		session_key,
		login_name,
		role_name,
		folder_name );

	return strdup( action_string );
}

LOOKUP_SORT_EXECUTE *lookup_sort_execute_new(
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		DICTIONARY *original_post_dictionary,
		char *appaserver_error_filename )
{
	LOOKUP_SORT_EXECUTE *lookup_sort_execute;

	if ( !application_name
	||   !login_name
	||   !role_name
	||   !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	lookup_sort_execute = lookup_sort_execute_calloc();

	lookup_sort_execute->lookup_sort_input =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		lookup_sort_input_new(
			application_name,
			login_name,
			role_name,
			folder_name,
			original_post_dictionary );

	lookup_sort_execute->query_fetch =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		lookup_sort_execute_query_fetch(
			lookup_sort_execute->
				lookup_sort_input->
				folder->
				folder_attribute_primary_key_list,
			lookup_sort_execute->
				lookup_sort_input->
				attribute_name,
			original_post_dictionary );

	lookup_sort_execute->appaserver_table_name =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		appaserver_table_name(
			folder_name );

	lookup_sort_execute->update_statement_list =
		lookup_sort_execute_update_statement_list(
			lookup_sort_execute->
				lookup_sort_input->
				folder->
				folder_attribute_primary_key_list,
			lookup_sort_execute->
				lookup_sort_input->
				attribute_name,
			lookup_sort_execute->
				query_fetch->
				row_list,
			lookup_sort_execute->appaserver_table_name );

	lookup_sort_execute->system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		lookup_sort_execute_system_string(
			appaserver_error_filename );

	return lookup_sort_execute;
}

LOOKUP_SORT_EXECUTE *lookup_sort_execute_calloc( void )
{
	LOOKUP_SORT_EXECUTE *lookup_sort_execute;

	if ( ! ( lookup_sort_execute =
			calloc( 1,
				sizeof ( LOOKUP_SORT_EXECUTE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return lookup_sort_execute;
}

LIST *lookup_sort_execute_update_statement_list(
		LIST *primary_key_list,
		char *lookup_sort_input_attribute_name,
		LIST *query_fetch_row_list,
		char *appaserver_table_name )
{
	LIST *list;
	QUERY_ROW *query_row;

	if ( !list_length( primary_key_list )
	||   !lookup_sort_input_attribute_name
	||   !list_rewind( query_fetch_row_list )
	||   !appaserver_table_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	list = list_new();

	do {
		query_row = list_get( query_fetch_row_list );

		list_set(
			list,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			lookup_sort_execute_update_statement(
				primary_key_list,
				lookup_sort_input_attribute_name,
				appaserver_table_name,
				query_row->cell_list ) );

	} while ( list_next( query_fetch_row_list ) );

	return list;
}

char *lookup_sort_execute_update_statement(
		LIST *primary_key_list,
		char *lookup_sort_input_attribute_name,
		char *appaserver_table_name,
		LIST *query_row_cell_list )
{
	char update_statement[ 1024 ];
	char *ptr = update_statement;
	QUERY_CELL *query_cell;
	char *primary_key;

	if ( !list_length( primary_key_list )
	||   !lookup_sort_input_attribute_name
	||   !appaserver_table_name
	||   !list_length( query_row_cell_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	query_cell =
		list_last(
			query_row_cell_list );

	ptr += sprintf(
		ptr,
		"update %s set %s = %s where ",
		appaserver_table_name,
		lookup_sort_input_attribute_name,
		query_cell->select_datum );

	list_rewind( query_row_cell_list );
	list_rewind( primary_key_list );

	do {
		primary_key = list_get( primary_key_list );

		if ( !list_at_first( primary_key_list ) )
			ptr += sprintf( ptr, " and " );

		query_cell = list_get( query_row_cell_list );

		ptr += sprintf(
			ptr,
			"%s = '%s'",
			primary_key,
			query_cell->select_datum );

		list_next( query_row_cell_list );

	} while ( list_next( primary_key_list ) );

	ptr += sprintf( ptr, ";" );

	return strdup( update_statement );
}

void lookup_sort_execute_update(
		LIST *update_statement_list,
		char *system_string )
{
	FILE *output_pipe;

	if ( !list_rewind( update_statement_list ) ) return;

	/* Safely returns */
	/* -------------- */
	output_pipe = appaserver_output_pipe( system_string );

	do {
		fprintf(output_pipe,
			"%s\n",
			(char *)list_get( update_statement_list ) );

	} while ( list_next( update_statement_list ) );

	pclose( output_pipe );
}

LOOKUP_SORT_INPUT *lookup_sort_input_new(
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		DICTIONARY *original_post_dictionary )
{
	LOOKUP_SORT_INPUT *lookup_sort_input;

	if ( !application_name
	||   !login_name
	||   !role_name
	||   !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	lookup_sort_input = lookup_sort_input_calloc();

	lookup_sort_input->folder =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_fetch(
			folder_name,
			/* ----------------------------------- */
			/* Set to cache all folders for a role */
			/* ----------------------------------- */
			role_name,
			(LIST *)0 /* exclude_attribute_name_list */,
			/* -------------------------------------- */
			/* Sets folder_attribute_primary_list	  */
			/* Sets folder_attribute_primary_key_list */
			/* Sets folder_attribute_name_list	  */
			/* -------------------------------------- */
			1 /* fetch_folder_attribute_list */,
			1 /* fetch_attribute */ );

	lookup_sort_input->relation_mto1_list =
		relation_mto1_list(
			/* ----------------------------------- */
			/* Set to cache all folders for a role */
			/* ----------------------------------- */
			role_name,
			folder_name
				/* many_folder_name */,
			lookup_sort_input->
				folder->
				folder_attribute_primary_key_list
				/* many_folder_primary_key_list */ );

	lookup_sort_input->relation_mto1_isa_list =
		relation_mto1_isa_list(
			(LIST *)0 /* mto1_isa_list */,
			/* ----------------------------------- */
			/* Set to cache all folders for a role */
			/* ----------------------------------- */
			role_name,
			folder_name
				/* many_folder_name */,
			lookup_sort_input->
				folder->
				folder_attribute_primary_key_list
				/* many_folder_primary_key_list */,
			0 /* not fetch_relation_one2m_list */,
			0 /* not fetch_relation_mto1_list */ );

	lookup_sort_input->folder_attribute_append_isa_list =
		folder_attribute_append_isa_list(
			lookup_sort_input->
				folder->
				folder_attribute_list
				/* append_isa_list */,
			lookup_sort_input->relation_mto1_isa_list );

	lookup_sort_input->folder_attribute_name_list =
		folder_attribute_name_list(
			(char *)0 /* folder_name */,
			lookup_sort_input->folder_attribute_append_isa_list );

	lookup_sort_input->folder_attribute_date_name_list =
		folder_attribute_date_name_list(
			lookup_sort_input->folder_attribute_append_isa_list );

	lookup_sort_input->dictionary_separate =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_post_prompt_lookup_new(
			original_post_dictionary,
			application_name,
			login_name,
			lookup_sort_input->relation_mto1_list,
			lookup_sort_input->folder_attribute_name_list,
			lookup_sort_input->folder_attribute_date_name_list,
			lookup_sort_input->folder_attribute_append_isa_list );

	lookup_sort_input->role =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		role_fetch(
			role_name,
			0 /* not fetch_role_attribute_exclude_list */ );

	lookup_sort_input->folder_row_level_restriction =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_row_level_restriction_fetch(
			folder_name );

	lookup_sort_input->security_entity =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		security_entity_new(
			login_name,
			lookup_sort_input->
				folder_row_level_restriction->
				non_owner_forbid ||
			lookup_sort_input->
				folder_row_level_restriction->
				non_owner_viewonly,
			lookup_sort_input->role->override_row_restrictions );

	lookup_sort_input->attribute_name =
		/* ------------------------------------------------- */
		/* Returns one of the first three parameters or null */
		/* ------------------------------------------------- */
		lookup_sort_input_attribute_name(
			ATTRIBUTE_NAME_SORT_ORDER,
			ATTRIBUTE_NAME_DISPLAY_ORDER,
			ATTRIBUTE_NAME_SEQUENCE_NUMBER,
			lookup_sort_input->folder->folder_attribute_name_list );

	return lookup_sort_input;
}

LOOKUP_SORT_INPUT *lookup_sort_input_calloc( void )
{
	LOOKUP_SORT_INPUT *lookup_sort_input;

	if ( ! ( lookup_sort_input =
			calloc( 1,
				sizeof ( LOOKUP_SORT_INPUT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return lookup_sort_input;
}

char *lookup_sort_execute_system_string( char *appaserver_error_filename )
{
	static char system_string[ 128 ];

	if ( !appaserver_error_filename )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"appaserver_error_filename is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"tee_appaserver.sh |"
		"sql.e 2>>%s",
		appaserver_error_filename );

	return system_string;
}

QUERY_CELL *lookup_sort_execute_query_cell(
		char *attribute_name,
		DICTIONARY *original_post_dictionary,
		int index )
{
	QUERY_CELL *query_cell;
	char *index_get;

	if ( !attribute_name
	||   !dictionary_length( original_post_dictionary )
	||   !index )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	query_cell = query_cell_calloc();

	if ( ! ( index_get =
			/* --------------------------------------- */
			/* Returns component of dictionary or null */
			/* --------------------------------------- */
			dictionary_index_get(
				attribute_name /* key */,
				original_post_dictionary,
				index ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"dictionary_index_get(%s,%d) returned empty.",
			attribute_name,
			index );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	query_cell->select_datum = index_get;

	return query_cell;
}

QUERY_ROW *lookup_sort_execute_query_row(
		LIST *folder_attribute_primary_key_list,
		char *attribute_name,
		DICTIONARY *original_post_dictionary,
		int index )
{
	QUERY_ROW *query_row;
	char *primary_key;
	QUERY_CELL *query_cell;

	if ( !list_rewind( folder_attribute_primary_key_list )
	||   !attribute_name
	||   !dictionary_length( original_post_dictionary )
	||   !index )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	query_row = query_row_calloc();
	query_row->cell_list = list_new();

	do {
		primary_key =
			list_get(
				folder_attribute_primary_key_list );

		query_cell =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			lookup_sort_execute_query_cell(
				primary_key /* attribute_name */,
				original_post_dictionary,
				index );

		list_set( query_row->cell_list, query_cell );

	} while ( list_next( folder_attribute_primary_key_list ) );

	query_cell =
		lookup_sort_execute_query_cell(
			attribute_name,
			original_post_dictionary,
			index );

	list_set( query_row->cell_list, query_cell );

	return query_row;
}

QUERY_FETCH *lookup_sort_execute_query_fetch(
		LIST *folder_attribute_primary_key_list,
		char *attribute_name,
		DICTIONARY *original_post_dictionary )
{
	QUERY_FETCH *query_fetch;
	int highest_index;
	int index;
	QUERY_ROW *query_row;

	if ( !list_length( folder_attribute_primary_key_list )
	||   !attribute_name
	||   !dictionary_length( original_post_dictionary ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( ! ( highest_index =
			/* ------------ */
			/* Returns >= 0 */
			/* ------------ */
			dictionary_highest_index(
				original_post_dictionary ) ) )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"dictionary_highest_index(%s) returned empty.",
			dictionary_display( original_post_dictionary ) );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	query_fetch = query_fetch_calloc();
	query_fetch->row_list = list_new();

	for ( index = 1; index <= highest_index; index++ )
	{
		query_row =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			lookup_sort_execute_query_row(
				folder_attribute_primary_key_list,
				attribute_name,
				original_post_dictionary,
				index );

		list_set(
			query_fetch->row_list,
			query_row );
	}

	return query_fetch;
}

