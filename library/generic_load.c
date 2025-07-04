/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/generic_load.c				*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "list.h"
#include "sql.h"
#include "String.h"
#include "piece.h"
#include "document.h"
#include "dictionary.h"
#include "folder.h"
#include "relation_mto1.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "role.h"
#include "sql.h"
#include "role_folder.h"
#include "form.h"
#include "frameset.h"
#include "widget.h"
#include "security.h"
#include "application.h"
#include "attribute.h"
#include "appaserver.h"
#include "environ.h"
#include "html.h"
#include "javascript.h"
#include "generic_load.h"

GENERIC_LOAD_CHOOSE *generic_load_choose_new(
		int argc,
		char **argv,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_name )
{
	GENERIC_LOAD_CHOOSE *generic_load_choose;

	if ( !argc
	||   !argv
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !process_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	generic_load_choose = generic_load_choose_calloc();

	generic_load_choose->session_process =
		/* --------------------------------------------- */
		/* Sets appaserver environment and outputs argv. */
		/* Each parameter is security inspected.	 */
		/* Any error will exit( 1 ).			 */
		/* --------------------------------------------- */
		session_process_integrity_exit(
			argc,
			argv,
			application_name,
			session_key,
			login_name,
			role_name,
			process_name );

	generic_load_choose->application_menu_horizontal_boolean =
		application_menu_horizontal_boolean(
			application_name );

	if ( generic_load_choose->application_menu_horizontal_boolean )
	{
		generic_load_choose->folder_menu =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			folder_menu_fetch(
				application_name,
				session_key,
				role_name,
				appaserver_parameter_data_directory(),
				0 /* not folder_menu_remove_boolean */ );

		generic_load_choose->menu =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			menu_new(
				application_name,
				session_key,
				login_name,
				role_name,
				1 /* application_menu_horizontal_boolean */,
				generic_load_choose->
					folder_menu->
					count_list );
	}

	if ( generic_load_choose->menu )
	{
		generic_load_choose->role_folder_insert_list =
			generic_load_choose->menu->role_folder_insert_list;
	}
	else
	{
		generic_load_choose->role_folder_insert_list =
			role_folder_insert_list( role_name );
	}

	generic_load_choose->title_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		generic_load_choose_title_string(
			process_name );

	generic_load_choose->document =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		document_new(
			application_name,
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			application_title_string( application_name ),
			generic_load_choose->title_string,
			(char *)0 /* sub_title_string */,
			(char *)0 /* sub_sub_title_string */,
			(char *)0 /* notepad */,
			(char *)0 /* onload_javascript_string */,
			(generic_load_choose->menu)
				? generic_load_choose->menu->html
				: (char *)0,
			/* ------------------------------ */
			/* Returns program memory or null */
			/* ------------------------------ */
			document_head_menu_setup_string(
				(generic_load_choose->menu)
					? 1
					: 0 /* menu_boolean */ ),
			(char *)0 /* calendar_setup_string */,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			javascript_include_string(
				(char *)0 /* appliction_name */,
				(LIST *)0 /* javascript_module_list */ ) );

	generic_load_choose->widget_container_list =
		generic_load_choose_widget_container_list(
			application_name,
			session_key,
			login_name,
			role_name,
			process_name,
			generic_load_choose->role_folder_insert_list );

	generic_load_choose->widget_container_list_html =
		widget_container_list_html(
			(char *)0 /* state */,
			-1 /* row_number */,
			(char *)0 /* background_color */,
			generic_load_choose->widget_container_list );

	generic_load_choose->document_form_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		document_form_html(
			generic_load_choose->document->html,
			generic_load_choose->document->document_head->html,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_head_close_tag(),
			generic_load_choose->document->document_body->html,
			generic_load_choose->widget_container_list_html,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_body_close_tag(),
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_close_tag() );

	free( generic_load_choose->widget_container_list_html );

	return generic_load_choose;
}

GENERIC_LOAD_CHOOSE *generic_load_choose_calloc( void )
{
	GENERIC_LOAD_CHOOSE *generic_load_choose;

	if ( ! ( generic_load_choose =
			calloc( 1, sizeof ( GENERIC_LOAD_CHOOSE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return generic_load_choose;
}

char *generic_load_choose_title_string( char *process_name )
{
	static char title_string[ 128 ];
	char buffer[ 64 ];

	sprintf(title_string,
		"%s Choose Table",
		string_initial_capital(
			buffer,
			process_name ) );

	return title_string;
}

LIST *generic_load_choose_widget_container_list(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_name,
		LIST *role_folder_insert_list )
{
	LIST *widget_container_list;
	ROLE_FOLDER *role_folder;
	int counter;
	WIDGET_CONTAINER *widget_container;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !process_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( role_folder_insert_list ) ) return (LIST *)0;

	widget_container_list = list_new();
	counter = 2;

	list_set(
		widget_container_list,
		( widget_container =
			widget_container_new(
				table_open, (char *)0 /* widget_name */ ) ) );

		widget_container->table_open->border_boolean = 1;

	do {
		role_folder = list_get( role_folder_insert_list );

		if ( ++counter == 3 )
		{
			list_set(
				widget_container_list,
				widget_container_new(
					table_row, (char *)0 ) );

			counter = 0;
		}

		list_set(
			widget_container_list,
			widget_container_new(
				table_data, (char *)0 ) );

		list_set(
			widget_container_list,
			generic_load_choose_widget_container(
				application_name,
				session_key,
				login_name,
				role_name,
				process_name,
				role_folder->folder_name ) );

	} while ( list_next( role_folder_insert_list ) );

	list_set(
		widget_container_list,
		widget_container_new(
			table_close, (char *)0 /* widget_name */ ) );

	return widget_container_list;
}

WIDGET_CONTAINER *generic_load_choose_widget_container(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_name,
		char *folder_name )
{
	WIDGET_CONTAINER *widget_container;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !process_name
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

	widget_container =
		widget_container_new(
			anchor, (char *)0 /* widget_name */ );

	widget_container->anchor->hypertext_reference =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		generic_load_hypertext_reference(
			GENERIC_LOAD_FOLDER_EXECUTABLE,
			application_name,
			session_key,
			login_name,
			role_name,
			process_name,
			folder_name );

	widget_container->anchor->target_frame = FRAMESET_PROMPT_FRAME;

	widget_container->anchor->prompt =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		generic_load_choose_prompt(
			folder_name );

	return widget_container;
}

char *generic_load_hypertext_reference(
		char *executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_name,
		char *folder_name )
{
	char hypertext_reference[ 1024 ];

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !process_name
	||   !folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(hypertext_reference,
		"%s/%s?%s+%s+%s+%s+%s+%s",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		application_http_prompt(
			/* ------------------------------------------------ */
			/* Returns component of global_appaserver_parameter */
			/* ------------------------------------------------ */
			appaserver_parameter_cgi_directory(),
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			application_server_address(),
			application_ssl_support_boolean(
				application_name ) ),
		executable,
		application_name,
		session_key,
		login_name,
		role_name,
		process_name,
		folder_name );

	return strdup( hypertext_reference );
}

char *generic_load_choose_prompt( char *folder_name )
{
	char prompt[ 128 ];

	if ( !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	strdup(
		string_initial_capital(
			prompt,
			folder_name ) );
}

GENERIC_LOAD_INPUT *generic_load_input_new(
		char *generic_load_filename_label,
		char *generic_load_skip_header_rows_label,
		char *generic_load_execute_yn_label,
		char *generic_load_position_prefix,
		char *generic_load_constant_prefix,
		DICTIONARY *non_prefixed_dictionary )
{
	GENERIC_LOAD_INPUT *generic_load_input;
	char *get;

	if ( !generic_load_filename_label
	||   !generic_load_skip_header_rows_label
	||   !generic_load_execute_yn_label
	||   !generic_load_position_prefix
	||   !generic_load_constant_prefix )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	generic_load_input = generic_load_input_calloc();

	generic_load_input->load_filename =
		dictionary_get(
			generic_load_filename_label,
			non_prefixed_dictionary );

	if ( !generic_load_input->load_filename )
	{
		return (GENERIC_LOAD_INPUT *)0;
	}

	generic_load_input->skip_header_rows_integer =
		string_atoi(
			dictionary_get(
				generic_load_skip_header_rows_label,
				non_prefixed_dictionary ) );

	get =
		dictionary_get(
			generic_load_execute_yn_label,
			non_prefixed_dictionary );

	if ( get ) generic_load_input->execute = ( *get == 'y' );

	generic_load_input->position_dictionary =
		dictionary_prefix_extract(
			generic_load_position_prefix,
			non_prefixed_dictionary
				/* source_dictionary */ );

	generic_load_input->constant_dictionary =
		dictionary_prefix_extract(
			generic_load_constant_prefix,
			non_prefixed_dictionary
				/* source_dictionary*/ );

	return generic_load_input;
}

GENERIC_LOAD_INPUT *generic_load_input_calloc( void )
{
	GENERIC_LOAD_INPUT *generic_load_input;

	if ( ! ( generic_load_input =
			calloc( 1,
				sizeof ( GENERIC_LOAD_INPUT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return generic_load_input;
}

GENERIC_LOAD_ATTRIBUTE *generic_load_attribute_new(
		DICTIONARY *position_dictionary,
		DICTIONARY *constant_dictionary,
		char *input,
		FOLDER_ATTRIBUTE *folder_attribute )
{
	GENERIC_LOAD_ATTRIBUTE *generic_load_attribute;
	char piece_buffer[ 1024 ];
	char *position_string;
	int position_integer;

	if ( !dictionary_length( position_dictionary )
	||   !dictionary_length( constant_dictionary )
	||   !input
	||   !folder_attribute )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	generic_load_attribute = generic_load_attribute_calloc();
	generic_load_attribute->folder_attribute = folder_attribute;

	if ( ( generic_load_attribute->value =
		dictionary_get(
			folder_attribute->attribute_name,
			constant_dictionary ) ) )
	{
		return generic_load_attribute;
	}

	if ( ! ( position_string =
			dictionary_get(
				folder_attribute->attribute_name,
				position_dictionary ) ) )
	{
		free( generic_load_attribute );
		return (GENERIC_LOAD_ATTRIBUTE *)0;
	}

	position_integer = atoi( position_string );

	if ( position_integer < 1 )
	{
		free( generic_load_attribute );
		return (GENERIC_LOAD_ATTRIBUTE *)0;
	}

	if ( !piece(
		piece_buffer,
		string_delimiter( input ),
		input,
		position_integer - 1 ) )
	{
		free( generic_load_attribute );
		return (GENERIC_LOAD_ATTRIBUTE *)0;
	}

	generic_load_attribute->value = strdup( piece_buffer ); 

	return generic_load_attribute;
}

GENERIC_LOAD_ATTRIBUTE *generic_load_attribute_calloc( void )
{
	GENERIC_LOAD_ATTRIBUTE *generic_load_attribute;

	if ( ! ( generic_load_attribute =
			calloc( 1,
				sizeof ( GENERIC_LOAD_ATTRIBUTE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return generic_load_attribute;
}

char *generic_load_attribute_name_list_string(
		LIST *generic_load_attribute_list )
{
	char list_string[ 1024 ];
	char *ptr = list_string;
	GENERIC_LOAD_ATTRIBUTE *generic_load_attribute;

	if ( !list_rewind( generic_load_attribute_list ) )
		return (char *)0;

	do {
		generic_load_attribute =
			list_get(
				generic_load_attribute_list );

		if ( !generic_load_attribute->folder_attribute )
		{
			char message[ 128 ];

			sprintf(message,
			"generic_load_attribute->folder_attribute is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( ptr != list_string ) ptr += sprintf( ptr, "," );

		ptr +=
			sprintf(
				ptr,
				"%s",
				generic_load_attribute->
					folder_attribute->
					attribute_name );

	} while ( list_next( generic_load_attribute_list ) );

	return strdup( list_string );
}

char *generic_load_attribute_value_list_string(
		LIST *generic_load_attribute_list,
		boolean include_quotes )
{
	char list_string[ 1024 ];
	char *ptr = list_string;
	GENERIC_LOAD_ATTRIBUTE *generic_load_attribute;

	if ( !list_rewind( generic_load_attribute_list ) )
		return (char *)0;

	do {
		generic_load_attribute =
			list_get(
				generic_load_attribute_list );

		if ( !generic_load_attribute->
			folder_attribute
		||   !generic_load_attribute->
			folder_attribute->
			attribute )
		{
			char message[ 128 ];

			sprintf(message,
				"generic_load_attribute is incomplete." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( ptr != list_string ) ptr += sprintf( ptr, "," );

		if (	!include_quotes
		||	attribute_is_number(
				generic_load_attribute->
					folder_attribute->
					attribute->
					datatype_name ) )
		{
			ptr +=
				sprintf(
					ptr,
					"%s",
					generic_load_attribute->value );
		}
		else
		{
			ptr +=
				sprintf(
					ptr,
					"'%s'",
					generic_load_attribute->value );
		}

	} while ( list_next( generic_load_attribute_list ) );

	return strdup( list_string );
}

FILE *generic_load_row_input_file( char *load_filename )
{
	FILE *input_file;

	if ( !load_filename )
	{
		char message[ 128 ];

		sprintf(message, "load_filename is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( ! ( input_file = fopen( load_filename, "r" ) ) )
	{
		char message[ 128 ];

		sprintf(message, "fopen(%s) returned empty.", load_filename );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return input_file;
}

LIST *generic_load_row_list(
		char *load_filename,
		int skip_header_rows_integer,
		DICTIONARY *position_dictionary,
		DICTIONARY *constant_dictionary,
		LIST *folder_attribute_list,
		char *folder_table_name )
{
	LIST *list;
	int row_number = 0;
	char input[ 2048 ];
	FILE *input_file;

	if ( !load_filename
	||   !dictionary_length( position_dictionary )
	||   !dictionary_length( constant_dictionary )
	||   !list_length( folder_attribute_list )
	||   !folder_table_name )
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

	input_file =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		generic_load_row_input_file(
			load_filename );

	/* Returns input or null if all done. */
	/* ---------------------------------- */
	while ( string_input( input, input_file, 2048 ) )
	{
		if ( ++row_number <= skip_header_rows_integer )
			continue;

		list_set(
			list,
			generic_load_row_new(
				position_dictionary,
				constant_dictionary,
				folder_attribute_list,
				folder_table_name,
				input ) );
	}

	fclose( input_file );

	return list;
}

GENERIC_LOAD_ROW *generic_load_row_new(
		DICTIONARY *position_dictionary,
		DICTIONARY *constant_dictionary,
		LIST *folder_attribute_list,
		char *folder_table_name,
		char *input )
{
	GENERIC_LOAD_ROW *generic_load_row;
	char *attribute_name_list_string;
	char *attribute_value_list_string;

	if ( !dictionary_length( position_dictionary )
	||   !dictionary_length( constant_dictionary )
	||   !list_length( folder_attribute_list )
	||   !folder_table_name
	||   !input )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	generic_load_row = generic_load_row_calloc();

	generic_load_row->generic_load_attribute_list = list_new();

	generic_load_row->generic_load_attribute_list =
		generic_load_attribute_list(
			position_dictionary,
			constant_dictionary,
			folder_attribute_list,
			input );

	if ( !list_length(
		generic_load_row->
			generic_load_attribute_list ) )
	{
		return (GENERIC_LOAD_ROW *)0;
	}

	attribute_name_list_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		generic_load_attribute_name_list_string(
			generic_load_row->
				generic_load_attribute_list );

	attribute_value_list_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		generic_load_attribute_value_list_string(
			generic_load_row->
				generic_load_attribute_list,
			1 /* include_quotes */ );

	generic_load_row->insert_statement =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		generic_load_row_insert_statement(
			folder_table_name,
			attribute_name_list_string,
			attribute_value_list_string );

	free( attribute_name_list_string );
	free( attribute_value_list_string );

	return generic_load_row;
}

GENERIC_LOAD_ROW *generic_load_row_calloc( void )
{
	GENERIC_LOAD_ROW *generic_load_row;

	if ( ! ( generic_load_row =
			calloc( 1,
				sizeof ( GENERIC_LOAD_ROW ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return generic_load_row;
}

char *generic_load_row_insert_statement(
		char *folder_table_name,
		char *attribute_name_list_string,
		char *attribute_value_list_string )
{
	char statement[ 2048 ];

	if ( !folder_table_name
	||   !attribute_name_list_string
	||   !attribute_value_list_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(statement,
		"insert into %s (%s) values (%s)",
		folder_table_name,
		attribute_name_list_string,
		attribute_value_list_string );

	return strdup( statement );
}

GENERIC_LOAD_INSERT *generic_load_insert_new(
		int argc,
		char **argv,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_name,
		char *folder_name )
{
	GENERIC_LOAD_INSERT *generic_load_insert;

	if ( !argc
	||   !argv
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !process_name
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

	generic_load_insert = generic_load_insert_calloc();

	generic_load_insert->session_process =
		/* Sets appaserver environment and outputs argv. */
		/* Each parameter is security inspected. */
		/* Any error will exit( 1 ). */
		session_process_integrity_exit(
			argc,
			argv,
			application_name,
			session_key,
			login_name,
			role_name,
			process_name );

	generic_load_insert->folder_name =
		/* ---------------------------- */
		/* Returns heap memory or datum */
		/* ---------------------------- */
		security_sql_injection_escape(
			SECURITY_ESCAPE_CHARACTER_STRING,
			folder_name /* datum */ );

	generic_load_insert->role_folder_list =
		role_folder_list(
			generic_load_insert->session_process->role_name,
			generic_load_insert->folder_name );

	if ( !session_folder_valid(
		APPASERVER_INSERT_STATE,
		generic_load_insert->folder_name,
		generic_load_insert->role_folder_list ) )
	{
		session_access_failed_message_exit(
			application_name,
			login_name,
			session_current_ip_address(
				environment_remote_ip_address(
					ENVIRONMENT_REMOTE_ADDR_KEY ) ) );
	}

	generic_load_insert->sub_title_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		generic_load_insert_sub_title_string(
			generic_load_insert->folder_name );

	generic_load_insert->post_dictionary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_dictionary_stdin_new(
			/* ---------------------------------- */
			/* Used when expecting a spooled file */
			/* ---------------------------------- */
			application_name,
			appaserver_parameter_upload_directory(),
			generic_load_insert_upload_filename_list() );

	generic_load_insert->non_prefixed_dictionary =
		dictionary_separate_non_prefixed(
			DICTIONARY_SEPARATE_SORT_PREFIX,
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			generic_load_insert->
				post_dictionary->
				original_post_dictionary );

	generic_load_insert->generic_load_input =
		generic_load_input_new(
			GENERIC_LOAD_FILENAME_LABEL,
			GENERIC_LOAD_SKIP_HEADER_ROWS_LABEL,
			GENERIC_LOAD_EXECUTE_YN_LABEL,
			GENERIC_LOAD_POSITION_PREFIX,
			GENERIC_LOAD_CONSTANT_PREFIX,
			generic_load_insert->non_prefixed_dictionary );

	if ( !generic_load_insert->generic_load_input ) return NULL;

	generic_load_insert->role =
		role_fetch(
			role_name,
			1 /* fetch_attribute_exclude_list */ );

	generic_load_insert->role_attribute_exclude_name_list =
		role_attribute_exclude_name_list(
			ROLE_PERMISSION_INSERT,
			generic_load_insert->
				role->
				role_attribute_exclude_list );

	generic_load_insert->folder =
		folder_fetch(
			generic_load_insert->folder_name,
			generic_load_insert->
				role_attribute_exclude_name_list,
			1 /* fetch_folder_attribute_list */,
			1 /* fetch_attribute */,
			1 /* cache_boolean */ );

	generic_load_insert->relation_mto1_list =
		relation_mto1_list(
			generic_load_insert->folder_name
				/* many_folder_name */,
			generic_load_insert->
				folder->
				folder_attribute_primary_key_list
				/* many_folder_primary_key_list */ );

	generic_load_insert->folder_table_name =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		folder_table_name(
			application_name,
			folder_name );

	if ( list_length(
		generic_load_insert->
			relation_mto1_list ) )
	{
		generic_load_insert->generic_load_relation_list =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			generic_load_relation_list_new(
				application_name,
				generic_load_insert->relation_mto1_list );

		generic_load_relation_list_set(
			generic_load_insert->
				generic_load_relation_list /* in/out */,
			generic_load_insert->
				generic_load_input->
				load_filename,
			generic_load_insert->
				generic_load_input->
				skip_header_rows_integer,
			generic_load_insert->
				generic_load_input->
				position_dictionary,
			generic_load_insert->
				generic_load_input->
				constant_dictionary,
			generic_load_insert->
				folder->
				folder_attribute_list );

		generic_load_insert->generic_load_orphan_list =
			generic_load_orphan_list_new(
				generic_load_insert->
					generic_load_relation_list );

		if ( generic_load_insert->
			generic_load_orphan_list )
		{
			return generic_load_insert;
		}
	}

	generic_load_insert->generic_load_row_list =
		generic_load_row_list(
			generic_load_insert->
				generic_load_input->
				load_filename,
			generic_load_insert->
				generic_load_input->
				skip_header_rows_integer,
			generic_load_insert->
				generic_load_input->
				position_dictionary,
			generic_load_insert->
				generic_load_input->
				constant_dictionary,
			generic_load_insert->
				folder->
				folder_attribute_list,
			generic_load_insert->folder_table_name );

	return generic_load_insert;
}

GENERIC_LOAD_INSERT *generic_load_insert_calloc( void )
{
	GENERIC_LOAD_INSERT *generic_load_insert;

	if ( ! ( generic_load_insert =
			calloc( 1,
				sizeof ( GENERIC_LOAD_INSERT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return generic_load_insert;
}

GENERIC_LOAD_FOLDER *generic_load_folder_new(
		int argc,
		char **argv,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_name,
		char *folder_name )
{
	GENERIC_LOAD_FOLDER *generic_load_folder;

	if ( !argc
	||   !argv
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !process_name
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

	generic_load_folder = generic_load_folder_calloc();

	generic_load_folder->session_process =
		/* --------------------------------------------- */
		/* Sets appaserver environment and outputs argv. */
		/* Each parameter is security inspected.	 */
		/* Any error will exit( 1 ).			 */
		/* --------------------------------------------- */
		session_process_integrity_exit(
			argc,
			argv,
			application_name,
			session_key,
			login_name,
			role_name,
			process_name );

	generic_load_folder->folder_name =
		/* ---------------------------- */
		/* Returns heap memory or datum */
		/* ---------------------------- */
		security_sql_injection_escape(
			SECURITY_ESCAPE_CHARACTER_STRING,
			folder_name /* datum */ );

	generic_load_folder->application_menu_horizontal_boolean =
		application_menu_horizontal_boolean(
			application_name );

	generic_load_folder->appaserver_parameter = appaserver_parameter_new();

	if ( generic_load_folder->application_menu_horizontal_boolean )
	{
		generic_load_folder->folder_menu =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			folder_menu_fetch(
				application_name,
				session_key,
				role_name,
				generic_load_folder->
					appaserver_parameter->
					data_directory,
				0 /* not folder_menu_remove_boolean */ );

		generic_load_folder->menu =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			menu_new(
				application_name,
				session_key,
				login_name,
				role_name,
				1 /* application_menu_horizontal_boolean */,
				generic_load_folder->
					folder_menu->
					count_list );
	}

	if ( generic_load_folder->menu )
	{
		generic_load_folder->role_folder_insert_list =
			generic_load_folder->menu->role_folder_insert_list;
	}
	else
	{
		generic_load_folder->role_folder_insert_list =
			role_folder_insert_list(
				role_name );
	}

	if ( !role_folder_insert_boolean(
		ROLE_PERMISSION_INSERT,
		generic_load_folder->folder_name,
		generic_load_folder->role_folder_insert_list ) )
	{
		char message[ 128 ];

		sprintf(message,
		"role_folder_insert_boolean(%s,%s,%s) returned false.",
			login_name,
			role_name,
			generic_load_folder->folder_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	generic_load_folder->generic_load_hypertext_reference =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		generic_load_hypertext_reference(
			GENERIC_LOAD_INSERT_EXECUTABLE,
			application_name,
			session_key,
			login_name,
			role_name,
			process_name,
			generic_load_folder->folder_name );

	generic_load_folder->role =
		role_fetch(
			role_name,
			1 /* fetch_attribute_exclude_list */ );

	generic_load_folder->role_attribute_exclude_name_list =
		role_attribute_exclude_name_list(
			ROLE_PERMISSION_INSERT,
			generic_load_folder->
				role->
				role_attribute_exclude_list );

	generic_load_folder->folder_attribute_list =
		folder_attribute_list(
			folder_name,
			generic_load_folder->role_attribute_exclude_name_list,
			1 /* fetch_attribute */,
			1 /* cache_boolean */ );

	generic_load_folder->folder_attribute_primary_key_list =
		folder_attribute_primary_key_list(
			folder_name,
			generic_load_folder->folder_attribute_list );

	generic_load_folder->relation_mto1_recursive_list =
		relation_mto1_recursive_list(
			(LIST *)0 /* relation_mto1_list Pass in null */,
			folder_name
				/* many_folder_name */,
			generic_load_folder->
				folder_attribute_primary_key_list
				/* many_primary_key_list */,
			1 /* single_foreign_key_only */ );

	generic_load_folder->folder_row_level_restriction =
		folder_row_level_restriction_fetch(
			folder_name );

	generic_load_folder->security_entity =
		security_entity_new(
			login_name,
			generic_load_folder->
				folder_row_level_restriction->
				non_owner_forbid,
			generic_load_folder->
				role->
				override_row_restrictions );

	generic_load_folder->form_generic_load =
		form_generic_load_new(
			GENERIC_LOAD_FILENAME_LABEL,
			GENERIC_LOAD_SKIP_HEADER_ROWS_LABEL,
			GENERIC_LOAD_EXECUTE_YN_LABEL,
			GENERIC_LOAD_POSITION_PREFIX,
			GENERIC_LOAD_CONSTANT_PREFIX,
			GENERIC_LOAD_IGNORE_PREFIX,
			application_name,
			login_name,
			role_name,
			folder_name,
			generic_load_folder->
				generic_load_hypertext_reference,
			generic_load_folder->
				folder_attribute_list,
			generic_load_folder->
				relation_mto1_recursive_list,
			generic_load_folder->security_entity );

	generic_load_folder->application_title_string =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		application_title_string(
			application_name );

	generic_load_folder->title_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		generic_load_folder_title_string(
			generic_load_folder->application_title_string,
			generic_load_folder->folder_name );

	generic_load_folder->javascript_module_list =
		generic_load_folder_javascript_module_list();

	generic_load_folder->javascript_include_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		javascript_include_string(
			application_name,
			generic_load_folder->javascript_module_list );

	generic_load_folder->folder_attribute_calendar_date_name_list_length =
		folder_attribute_calendar_date_name_list_length(
				generic_load_folder->
					folder_attribute_list );

	generic_load_folder->document_head_calendar_setup_string =
		document_head_calendar_setup_string(
			generic_load_folder->
			    folder_attribute_calendar_date_name_list_length );

	generic_load_folder->folder_notepad =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		folder_notepad( folder_name );

	generic_load_folder->document =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		document_new(
			application_name,
			generic_load_folder->application_title_string,
			generic_load_folder->title_string,
			(char *)0 /* subtitle_string */,
			(char *)0 /* sub_sub_title_string */,
			generic_load_folder->folder_notepad,
			(char *)0 /* onload_javascript_string */,
			(generic_load_folder->menu)
				? generic_load_folder->menu->html
				: (char *)0,
			document_head_menu_setup_string(
				(generic_load_folder->menu)
					? 1
					: 0 /* menu_boolean */ ),
			generic_load_folder->
				document_head_calendar_setup_string,
			generic_load_folder->
				javascript_include_string );

	generic_load_folder->document_form_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		document_form_html(
			generic_load_folder->document->html,
			generic_load_folder->document->document_head->html,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_head_close_tag(),
			generic_load_folder->document->document_body->html,
			generic_load_folder->form_generic_load->form_html,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_body_close_tag(),
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_close_tag() );

	free( generic_load_folder->
		form_generic_load->
		widget_container_list_html );

	free( generic_load_folder->
		form_generic_load->
		form_html );

	return generic_load_folder;
}

GENERIC_LOAD_FOLDER *generic_load_folder_calloc( void )
{
	GENERIC_LOAD_FOLDER *generic_load_folder;

	if ( ! ( generic_load_folder =
			calloc( 1,
				sizeof ( GENERIC_LOAD_FOLDER ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return generic_load_folder;
}

char *generic_load_folder_title_string(
		char *application_title_string,
		char *folder_name )
{
	static char title_string[ 128 ];
	char buffer[ 64 ];

	if ( !application_title_string
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

	sprintf(title_string,
		"%s Load %s",
		application_title_string,
		string_initial_capital(
			buffer,
			folder_name ) );

	return title_string;
}

LIST *generic_load_folder_javascript_module_list( void )
{
	LIST *javascript_module_list = list_new();;

	list_set(
		javascript_module_list,
		"post_generic_load" );

	return javascript_module_list;
}

void generic_load_row_insert(
		LIST *generic_load_row_list,
		char *system_string )
{
	FILE *output_pipe;
	GENERIC_LOAD_ROW *generic_load_row;

	if ( !system_string )
	{
		char message[ 128 ];

		sprintf(message, "system_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	if ( !list_rewind( generic_load_row_list ) ) return;

	output_pipe =
		generic_load_row_output_pipe(
			system_string );

	do {
		generic_load_row =
			list_get(
				generic_load_row_list );

		if ( !generic_load_row->insert_statement )
		{
			char message[ 128 ];

			sprintf(message,
			"generic_load_row->insert_statement is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );

			pclose( output_pipe );
		}

		fprintf(output_pipe,
			"%s\n",
			generic_load_row->insert_statement );

	} while ( list_next( generic_load_row_list ) );

	pclose( output_pipe );
}

char *generic_load_insert_system_string( void )
{
	return "sql.e 2>&1 | html_paragraph_wrapper.e";
}

FILE *generic_load_row_output_pipe( char *system_string )
{
	if ( !system_string )
	{
		char message[ 128 ];

		sprintf(message, "system_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return popen( system_string, "w" );
}

LIST *generic_load_attribute_list(
		DICTIONARY *position_dictionary,
		DICTIONARY *constant_dictionary,
		LIST *folder_attribute_list,
		char *input )
{
	LIST *list;
	FOLDER_ATTRIBUTE *folder_attribute;

	if ( !position_dictionary
	||   !constant_dictionary
	||   !list_rewind( folder_attribute_list )
	||   !input )
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
		folder_attribute = list_get( folder_attribute_list );

		list_set(
			list,
			generic_load_attribute_new(
				position_dictionary,
				constant_dictionary,
				input,
				folder_attribute ) );

	} while ( list_next( folder_attribute_list ) );

	return list;
}

GENERIC_LOAD_ATTRIBUTE *generic_load_attribute_seek(
		LIST *generic_load_attribute_list,
		char *attribute_name )
{
	GENERIC_LOAD_ATTRIBUTE *generic_load_attribute;

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

	if ( !list_rewind( generic_load_attribute_list ) )
		return (GENERIC_LOAD_ATTRIBUTE *)0;

	do {
		generic_load_attribute =
			list_get(
				generic_load_attribute_list );

		if ( !generic_load_attribute->folder_attribute )
		{
			char message[ 128 ];

			sprintf(message,
		"generic_load_attribute->folder_attribute is empty.\n" );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( strcmp(
			generic_load_attribute->
				folder_attribute->
				attribute_name,
			attribute_name ) == 0 )
		{
			return generic_load_attribute;
		}

	} while ( list_next( generic_load_attribute_list ) );

	return (GENERIC_LOAD_ATTRIBUTE *)0;
}

GENERIC_LOAD_RELATION_LIST *generic_load_relation_list_new(
		char *application_name,
		LIST *relation_mto1_list )
{
	GENERIC_LOAD_RELATION_LIST *generic_load_relation_list;
	RELATION_MTO1 *relation_mto1;

	if ( !application_name )
	{
		char message[ 128 ];

		sprintf(message, "application_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( relation_mto1_list ) ) return NULL;

	generic_load_relation_list = generic_load_relation_list_calloc();

	generic_load_relation_list->list = list_new();

	do {
		relation_mto1 =
			list_get(
				relation_mto1_list );

		if ( !relation_mto1->one_folder_name
		||   !list_length(
			relation_mto1->
				relation_foreign_key_list ) )
		{
			char message[ 128 ];

			sprintf(message,
				"relation_mto1 is incomplete." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		list_set(
			generic_load_relation_list->list,
			generic_load_relation_new(
				application_name,
				relation_mto1->one_folder_name,
				relation_mto1->relation_foreign_key_list ) );

	} while ( list_next( relation_mto1_list ) );

	return generic_load_relation_list;
}

GENERIC_LOAD_RELATION_LIST *generic_load_relation_list_calloc( void )
{
	GENERIC_LOAD_RELATION_LIST *generic_load_relation_list;

	if ( ! ( generic_load_relation_list =
			calloc( 1,
				sizeof ( GENERIC_LOAD_RELATION_LIST ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return generic_load_relation_list;
}

void generic_load_relation_list_set(
		GENERIC_LOAD_RELATION_LIST *
			generic_load_relation_list /* in/out */,
		char *load_filename,
		int skip_header_rows_integer,
		DICTIONARY *position_dictionary,
		DICTIONARY *constant_dictionary,
		LIST *folder_attribute_list )
{
	int row_number = 0;
	GENERIC_LOAD_RELATION *generic_load_relation;
	FILE *input_file;
	char input[ 1024 ];

	if ( !generic_load_relation_list
	||   !list_length( generic_load_relation_list->list )
	||   !load_filename
	||   !position_dictionary
	||   !constant_dictionary
	||   !list_length( folder_attribute_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	input_file =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		generic_load_row_input_file(
			load_filename );

	while ( string_input( input, input_file, 1024 ) )
	{
		if ( ++row_number <= skip_header_rows_integer )
			continue;

		list_rewind( generic_load_relation_list->list );

		do {
			generic_load_relation =
				list_get(
					generic_load_relation_list->list );

			generic_load_relation_set(
				generic_load_relation /* in/out */,
				position_dictionary,
				constant_dictionary,
				folder_attribute_list,
				input );

		} while ( list_next( generic_load_relation_list->list ) );
	}

	fclose( input_file );
}

GENERIC_LOAD_RELATION *generic_load_relation_new(
		char *application_name,
		char *one_folder_name,
		LIST *relation_foreign_key_list )
{
	GENERIC_LOAD_RELATION *generic_load_relation;

	if ( !application_name
	||   !one_folder_name
	||   !list_length( relation_foreign_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	generic_load_relation = generic_load_relation_calloc();

	generic_load_relation->one_folder_name = one_folder_name;

	generic_load_relation->relation_foreign_key_list =
		relation_foreign_key_list;

	generic_load_relation->folder_table_name =
		strdup(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			folder_table_name(
				application_name,
				one_folder_name ) );

	generic_load_relation->foreign_delimited_list = list_new();

	return generic_load_relation;
}

GENERIC_LOAD_RELATION *generic_load_relation_calloc( void )
{
	GENERIC_LOAD_RELATION *generic_load_relation;

	if ( ! ( generic_load_relation =
			calloc( 1,
				sizeof ( GENERIC_LOAD_RELATION ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return generic_load_relation;
}

void generic_load_relation_set(
		GENERIC_LOAD_RELATION *generic_load_relation,
		DICTIONARY *position_dictionary,
		DICTIONARY *constant_dictionary,
		LIST *folder_attribute_list,
		char *input )
{
	LIST *foreign_data_list;
	LIST *attribute_list;
	char *foreign_key;
	GENERIC_LOAD_ATTRIBUTE *generic_load_attribute;
	char *relation_data_delimited;

	if ( !generic_load_relation
	||   !list_length( generic_load_relation->relation_foreign_key_list )
	||   !position_dictionary
	||   !constant_dictionary
	||   !list_length( folder_attribute_list )
	||   !input )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	foreign_data_list = list_new();

	attribute_list =
		generic_load_attribute_list(
			position_dictionary,
			constant_dictionary,
			folder_attribute_list,
			input );

	if ( !list_length( attribute_list ) ) return;

	list_rewind( generic_load_relation->relation_foreign_key_list );

	do {
		foreign_key =
			list_get(
				generic_load_relation->
					relation_foreign_key_list );

		generic_load_attribute =
			generic_load_attribute_seek(
				attribute_list,
				foreign_key /* attribute_name */ );

		if ( generic_load_attribute )
		{
			list_set(
				foreign_data_list,
				generic_load_attribute->value );
		}

	} while (
		list_next(
			generic_load_relation->
				relation_foreign_key_list ) );

	if ( !list_length( foreign_data_list ) )
	{
		list_free_container( foreign_data_list );
		return;
	}

	relation_data_delimited =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		generic_load_relation_data_delimited(
			SQL_DELIMITER,
			foreign_data_list );

	if ( list_exists_string(
		relation_data_delimited,
		generic_load_relation->foreign_delimited_list ) )
	{
		free( relation_data_delimited );
		return;
	}

	list_set(
		generic_load_relation->foreign_delimited_list,
		relation_data_delimited );
}

char *generic_load_relation_data_delimited(
		char sql_delimiter,
		LIST *foreign_data_list )
{
	if ( !list_length( foreign_data_list ) )
	{
		char message[ 128 ];

		sprintf(message, "foreign_data_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	list_delimited_string(
		foreign_data_list,
		sql_delimiter );
}

GENERIC_LOAD_ORPHAN *generic_load_orphan_new(
		GENERIC_LOAD_RELATION *generic_load_relation )
{
	GENERIC_LOAD_ORPHAN *generic_load_orphan;
	char *foreign_delimited_string;
	LIST *foreign_data_list;
	char *select_statement;
	char *input_system;
	boolean result;

	if ( !generic_load_relation
	||   !generic_load_relation->one_folder_name
	||   !list_length( generic_load_relation->relation_foreign_key_list )
	||   !generic_load_relation->folder_table_name
	||   !list_rewind( generic_load_relation->foreign_delimited_list ) )
	{
		char message[ 128 ];

		sprintf(message, "generic_load_relation is incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	generic_load_orphan = generic_load_orphan_calloc();

	generic_load_orphan->foreign_delimited_list = list_new();

	do {
		foreign_delimited_string =
			list_get(
				generic_load_relation->
					foreign_delimited_list );

		foreign_data_list =
			list_string_list(
				foreign_delimited_string /* list_string */,
				SQL_DELIMITER );

		if (	list_length( foreign_data_list ) !=
			list_length( 
				generic_load_relation->
					relation_foreign_key_list ) )
		{
			char message[ 128 ];

			sprintf(message,
	"length(foreign_data_list)=%d <> length(relation_foreign_key_list)=%d",
				list_length( foreign_data_list ),
				list_length( 
					generic_load_relation->
						relation_foreign_key_list ) );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		select_statement =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			generic_load_orphan_select_statement(
				generic_load_relation->folder_table_name,
				generic_load_relation->
					relation_foreign_key_list,
				foreign_data_list );

		input_system =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			generic_load_orphan_input_system(
				select_statement );

		result =
			(boolean)string_atoi(
				string_pipe_fetch(
					input_system ) );

		if ( !result )
		{
			list_set(
				generic_load_orphan->foreign_delimited_list,
				foreign_delimited_string );
		}

	} while ( list_next( generic_load_relation->
				foreign_delimited_list ) );

	if ( list_length( generic_load_orphan->foreign_delimited_list ) )
		return generic_load_orphan;
	else
		return (GENERIC_LOAD_ORPHAN *)0;
}

GENERIC_LOAD_ORPHAN *generic_load_orphan_calloc( void )
{
	GENERIC_LOAD_ORPHAN *generic_load_orphan;

	if ( ! ( generic_load_orphan =
			calloc( 1,
				sizeof ( GENERIC_LOAD_ORPHAN ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return generic_load_orphan;
}

char *generic_load_orphan_input_system( char *select_statement )
{
	static char input_system[ 512 ];

	if ( !select_statement )
	{
		char message[ 128 ];

		sprintf(message, "select_statement is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(input_system,
		"echo \"%s\" | sql.e",
		select_statement );

	return input_system;
}

char *generic_load_orphan_select_statement(
		char *folder_table_name,
		LIST *relation_foreign_key_list,
		LIST *foreign_data_list )
{
	static char select_statement[ 512 ];
	char *ptr = select_statement;
	boolean first_time = 1;
	char *foreign_key;
	char *foreign_data;

	if ( !folder_table_name
	||   !list_length( relation_foreign_key_list )
	||   !list_length( foreign_data_list )
	||   list_length( relation_foreign_key_list ) !=
		list_length( foreign_data_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr +=
		sprintf(
			ptr,
			"select 1 from %s where ",
			folder_table_name );

	list_rewind( foreign_data_list );
	list_rewind( relation_foreign_key_list );

	do {
		foreign_key =
			list_get(
				relation_foreign_key_list );

		if ( first_time )
			first_time = 0;
		else
			ptr += sprintf( ptr, " and " );

		foreign_data = list_get( foreign_data_list );

		ptr +=
			sprintf(
				ptr,
				"%s = '%s'",
				foreign_key,
				foreign_data );

		list_next( foreign_data_list );

	} while ( list_next( relation_foreign_key_list ) );

	ptr += sprintf( ptr, ";" );

	return select_statement;
}

void generic_load_orphan_output( GENERIC_LOAD_ORPHAN *generic_load_orphan )
{
	char *title_string;
	char *heading_string;
	char *output_system;
	FILE *output_pipe;
	char *foreign_delimited_string;

	if ( !generic_load_orphan
	||   !generic_load_orphan->generic_load_relation
	||   !generic_load_orphan->generic_load_relation->one_folder_name
	||   !list_length(
		generic_load_orphan->
			generic_load_relation->
			relation_foreign_key_list )
	||   !list_rewind(
		generic_load_orphan->
			generic_load_relation->
			foreign_delimited_list ) )
	{
		char message[ 128 ];

		sprintf(message, "generic_load_orphan is incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	title_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		generic_load_orphan_title_string(
			generic_load_orphan->
				generic_load_relation->
				one_folder_name );

	heading_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		generic_load_orphan_heading_string(
			generic_load_orphan->
				generic_load_relation->
				relation_foreign_key_list );

	output_system =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		generic_load_orphan_output_system(
			SQL_DELIMITER,
			title_string,
			heading_string );

	output_pipe =
		generic_load_orphan_output_pipe(
			output_system );

	do {
		foreign_delimited_string =
			list_get(
				generic_load_orphan->
					foreign_delimited_list );

		fprintf(output_pipe,
			"%s\n",
			foreign_delimited_string );

	} while ( list_next( generic_load_orphan->
				generic_load_relation->
				foreign_delimited_list ) );

	pclose( output_pipe );
}

char *generic_load_orphan_title_string( char *one_folder_name )
{
	static char title_string[ 128 ];
	char buffer[ 64 ];

	if ( !one_folder_name )
	{
		char message[ 128 ];

		sprintf(message, "one_folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(title_string,
		"Load will generate orphans in: %s",
		string_initial_capital(
			buffer,
			one_folder_name ) );

	return title_string;
}

char *generic_load_orphan_heading_string(
		LIST *relation_foreign_key_list )
{
	static char heading_string[ 256 ];
	char *tmp;

	if ( !list_length( relation_foreign_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "relation_foreign_key_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	strcpy(
		heading_string,
		( tmp =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			list_delimited_string(
				relation_foreign_key_list,
				',' /* delimiter */ ) ) );

	free( tmp );

	return heading_string;
}

char *generic_load_orphan_output_system(
		char sql_delimiter,
		char *title_string,
		char *heading_string )
{
	static char output_system[ 1024 ];

	if ( !sql_delimiter
	||   !title_string
	||   !heading_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(output_system,
		"html_table.e '%s' '%s' '%c'",
		title_string,
		heading_string,
		sql_delimiter );

	return output_system;
}

FILE *generic_load_orphan_output_pipe( char *output_system )
{
	if ( !output_system )
	{
		char message[ 128 ];

		sprintf(message, "output_system is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return popen( output_system, "w" );
}

GENERIC_LOAD_ORPHAN_LIST *generic_load_orphan_list_new(
		GENERIC_LOAD_RELATION_LIST *
			generic_load_relation_list )
{
	GENERIC_LOAD_ORPHAN_LIST *generic_load_orphan_list;
	GENERIC_LOAD_RELATION *generic_load_relation;

	if ( !generic_load_relation_list
	||   !list_rewind( generic_load_relation_list->list ) )
	{
		char message[ 128 ];

		sprintf(message, "generic_load_relation_list is incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	generic_load_orphan_list = generic_load_orphan_list_calloc();

	generic_load_orphan_list->list = list_new();

	do {
		generic_load_relation =
			list_get(
				generic_load_relation_list->list );

		list_set(
			generic_load_orphan_list->list,
			generic_load_orphan_new(
				generic_load_relation ) );

	} while ( list_next( generic_load_relation_list->list ) );

	if ( !list_length( generic_load_orphan_list->list ) )
		return (GENERIC_LOAD_ORPHAN_LIST *)0;
	else
		return generic_load_orphan_list;
}

GENERIC_LOAD_ORPHAN_LIST *generic_load_orphan_list_calloc( void )
{
	GENERIC_LOAD_ORPHAN_LIST *generic_load_orphan_list;

	if ( ! ( generic_load_orphan_list =
			calloc( 1,
				sizeof ( GENERIC_LOAD_ORPHAN_LIST ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return generic_load_orphan_list;
}

void generic_load_orphan_list_output(
		GENERIC_LOAD_ORPHAN_LIST *
			generic_load_orphan_list )
{
	GENERIC_LOAD_ORPHAN *generic_load_orphan;

	if ( !generic_load_orphan_list
	||   !list_rewind( generic_load_orphan_list->list ) )
	{
		char message[ 128 ];

		sprintf(message, "generic_load_orphan_list is incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	do {
		generic_load_orphan =
			list_get(
				generic_load_orphan_list->list );

		generic_load_orphan_output(
			generic_load_orphan );

	} while ( list_next( generic_load_orphan_list->list ) );
}

char *generic_load_attribute_heading_string(
		LIST *generic_load_attribute_list )
{
	static char heading_string[ 256 ];
	char *ptr = heading_string;
	GENERIC_LOAD_ATTRIBUTE *generic_load_attribute;

	if ( !list_rewind( generic_load_attribute_list ) )
	{
		char message[ 128 ];

		sprintf(message, "generic_load_attribute_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	do {
		generic_load_attribute =
			list_get(
				generic_load_attribute_list );

		if ( !generic_load_attribute->folder_attribute )
		{
			char message[ 128 ];

			sprintf(message,
				"generic_load_attribute is incomplete." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( ptr != heading_string ) ptr += sprintf( ptr, "," );

		ptr += sprintf(
			ptr,
			"%s",
			generic_load_attribute->
				folder_attribute->
				attribute_name );

	} while ( list_next( generic_load_attribute_list ) );

	return heading_string;
}

void generic_load_row_display(
		char *folder_name,
		LIST *generic_load_row_list )
{
	char *title_string;
	GENERIC_LOAD_ROW *generic_load_row_first;
	char *heading_string;
	char *output_system;
	FILE *output_pipe;
	GENERIC_LOAD_ROW *generic_load_row;
	char *value_list_string;

	if ( !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	title_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		generic_load_row_title_string(
			folder_name );

	if ( !list_length( generic_load_row_list ) )
	{
		printf( "<h1>%s</h1>\n", title_string );
		printf( "<h3>No rows to load.</h3>\n" );
		return;
	}

	generic_load_row_first = list_first( generic_load_row_list );

	heading_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		generic_load_attribute_heading_string(
			generic_load_row_first->
				generic_load_attribute_list );

	output_system =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		generic_load_row_output_system(
			',' /* delimiter */,
			HTML_TABLE_QUEUE_TOP_BOTTOM,
			title_string,
			heading_string );

	output_pipe =
		generic_load_row_output_pipe(
			output_system );

	list_rewind( generic_load_row_list );

	do {
		generic_load_row =
			list_get(
				generic_load_row_list );

		value_list_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			generic_load_attribute_value_list_string(
				generic_load_row->generic_load_attribute_list,
				0 /* not include_quotes */ );

		fprintf(
			output_pipe,
			"%s\n",
			value_list_string );

		free( value_list_string );

	} while ( list_next( generic_load_row_list ) );

	pclose( output_pipe );
}

char *generic_load_row_title_string( char *folder_name )
{
	static char title_string[ 128 ];
	char buffer[ 64 ];

	sprintf(title_string,
		"Load %s",
		string_initial_capital(
			buffer,
			folder_name ) );

	return title_string;
}

char *generic_load_row_output_system(
		char delimiter,
		int html_table_queue_top_bottom,
		char *title_string,
		char *heading_string )
{
	char output_system[ 1024 ];

	if ( !delimiter
	||   !title_string
	||   !heading_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(output_system,
		"queue_top_bottom_rows.e %d |"
		"html_table.e '%s' '%s' '%c' ",
		html_table_queue_top_bottom,
		title_string,
		heading_string,
		delimiter );

	return strdup( output_system );
}

char *generic_load_insert_sub_title_string( char *folder_name )
{
	static char string[ 128 ];

	if ( !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(string,
		"Load %s",
		folder_name );

	return
	string_initial_capital(
		string,
		string );
}

LIST *generic_load_insert_upload_filename_list( void )
{
	LIST *upload_filename_list = list_new();

	list_set( upload_filename_list, "filename" );

	return upload_filename_list;
}

