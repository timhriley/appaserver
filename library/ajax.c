/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/ajax.c					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "sql.h"
#include "process.h"
#include "query.h"
#include "spool.h"
#include "ajax.h"

AJAX_CLIENT *ajax_client_new(
		char *many_folder_name,
		char *form_folder_name,
		LIST *form_foreign_key_list,
		LIST *form_primary_key_list,
		char *ajax_folder_name,
		LIST *ajax_primary_key_list,
		LIST *ajax_foreign_key_list,
		boolean top_select_boolean )
{
	AJAX_CLIENT *ajax_client;

	if ( !many_folder_name
	||   !form_folder_name
	||   !list_length( form_foreign_key_list )
	||   !list_length( form_primary_key_list )
	||   !ajax_folder_name
	||   !list_length( ajax_primary_key_list )
	||   !list_length( ajax_foreign_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ajax_client = ajax_client_calloc();

	ajax_client->many_folder_name = many_folder_name;
	ajax_client->form_folder_name = form_folder_name;
	ajax_client->ajax_folder_name = ajax_folder_name;

	ajax_client->form_widget_name =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		ajax_client_form_widget_name(
			SQL_DELIMITER,
			form_foreign_key_list );

	ajax_client->drop_down_widget_container =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
	  	widget_container_new(
			drop_down,
			ajax_client->form_widget_name );

	ajax_client->
		drop_down_widget_container->
		drop_down->
		foreign_key_list =
			form_foreign_key_list;

	ajax_client->
		drop_down_widget_container->
		drop_down->
		top_select_boolean =
			top_select_boolean;

	ajax_client->
		drop_down_widget_container->
		drop_down->
		many_folder_name =
			many_folder_name;

	ajax_client->
		drop_down_widget_container->
		heading_string =
			form_folder_name;

	ajax_client->fill_button_widget_container =
		widget_container_new(
			button,
			AJAX_CLIENT_FILL_LABEL
				/* widget_name */ );

	ajax_client->form_widget_id =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		ajax_client_form_widget_id(
			SQL_DELIMITER,
			many_folder_name,
			form_foreign_key_list );

	ajax_client->fill_button_action_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		ajax_client_fill_button_action_string(
			ajax_folder_name,
			ajax_client->form_widget_id );

	ajax_client->
		fill_button_widget_container->
		widget_button->
		button->
		action_string =
			ajax_client->fill_button_action_string;

	ajax_client->select_primary_key_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		ajax_client_select_primary_key_string(
			ajax_primary_key_list );

	ajax_client->popup_widget_name =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		ajax_client_popup_widget_name(
			SQL_DELIMITER,
			ajax_foreign_key_list );

	ajax_client->widget_container_list = list_new();

	list_set(
		ajax_client->widget_container_list,
		ajax_client->drop_down_widget_container );

	list_set(
		ajax_client->widget_container_list,
		ajax_client->fill_button_widget_container );

	return ajax_client;
}

AJAX_CLIENT *ajax_client_calloc( void )
{
	AJAX_CLIENT *ajax_client;

	if ( ! ( ajax_client = calloc( 1, sizeof ( AJAX_CLIENT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return ajax_client;
}

char *ajax_client_fill_button_action_string(
		char *ajax_folder_name,
		char *form_widget_id )
{
	char action_string[ 256 ];

	if ( !ajax_folder_name
	||   !form_widget_id )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		action_string,
		sizeof ( action_string ),
		"fork_%s_ajax_window( '%s_$row' )",
		ajax_folder_name,
		form_widget_id );

	return strdup( action_string );
}

char *ajax_client_execute_window_system_string(
		const char *ajax_client_window_executable,
		const char *ajax_server_post_executable,
		char *session_key,
		char *login_name,
		char *role_name,
		char *form_folder_name,
		char *ajax_folder_name,
		char *popup_widget_name,
		char *select_primary_key_string )
{
	char system_string[ 1024 ];

	if ( !session_key
	||   !login_name
	||   !role_name
	||   !form_folder_name
	||   !ajax_folder_name
	||   !popup_widget_name
	||   !select_primary_key_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"%s %s %s %s %s %s %s %s '%s'",
		ajax_client_window_executable,
		ajax_server_post_executable,
		session_key,
		login_name,
		role_name,
		form_folder_name,
		ajax_folder_name,
		popup_widget_name,
		select_primary_key_string );

	return strdup( system_string );
}

char *ajax_client_list_javascript(
		char *session_key,
		char *login_name,
		char *role_name,
		LIST *ajax_client_list )
{
	char javascript[ STRING_128K ];
	char *ptr = javascript;
	AJAX_CLIENT *ajax_client;
	char *system_string;
	SPOOL *spool;
	LIST *list;
	char *spool_line;

	if ( !session_key
	||   !login_name
	||   !role_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	*javascript = '\0';

	if ( list_rewind( ajax_client_list ) )
	do {
		ajax_client = list_get( ajax_client_list );

		system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			ajax_client_execute_window_system_string(
				AJAX_CLIENT_WINDOW_EXECUTABLE,
				AJAX_SERVER_POST_EXECUTABLE,
				session_key,
				login_name,
				role_name,
				ajax_client->form_folder_name,
				ajax_client->ajax_folder_name,
				ajax_client->popup_widget_name,
				ajax_client->select_primary_key_string );

		/* Safely returns */
		/* -------------- */
		spool =
			spool_new(
				system_string,
				0 /* not capture_stderr_boolean */ );

		pclose( spool->output_pipe );
		list = spool_list( spool->output_filename );
		free( system_string );

		if ( list_rewind( list ) )
		do {
			spool_line = list_get( list );

			if (	strlen( javascript ) +
				strlen( spool_line ) + 1
				>= STRING_128K )
			{
				char message[ 128 ];

				sprintf(message,
					STRING_OVERFLOW_TEMPLATE,
					STRING_128K );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			ptr += sprintf(
				ptr,
				"%s\n",
				spool_line );

		} while ( list_next( list ) );

		if ( list ) list_free( list );

	} while ( list_next( ajax_client_list ) );

	if ( !*javascript )
		return NULL;
	else
		return strdup( javascript );
}

char *ajax_client_select_primary_key_string(
		LIST *ajax_primary_key_list )
{
	if ( !list_length( ajax_primary_key_list ) )
	{
		char message[ 128 ];

		sprintf(message,
			"ajax_primary_key_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	/* ------------------------- */
	/* Returns heap memory or "" */
	/* ------------------------- */
	list_delimited(
		ajax_primary_key_list,
		',' );
}

AJAX_SERVER *ajax_server_new(
		char *application_name,
		char *form_folder_name,
		char *role_name,
		char *ajax_folder_name,
		char *value )
{
	AJAX_SERVER *ajax_server;

	if ( !application_name
	||   !form_folder_name
	||   !role_name
	||   !ajax_folder_name
	||   !value )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ajax_server = ajax_server_calloc();

	ajax_server->form_folder =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_fetch(
			form_folder_name,
			(LIST *)0 /* exclude_attribute_name_list */,
			1 /* fetch_folder_attribute_list */,
			0 /* not fetch_attribute */,
			1 /* cache_boolean */ );

	ajax_server->relation_mto1_list =
		relation_mto1_list(
			form_folder_name /* many_folder_name */,
			ajax_server->
				form_folder->
				folder_attribute_primary_key_list
				/* many_folder_primary_key_list */ );
	
	ajax_server->relation_mto1_to_one =
		relation_mto1_seek(
			ajax_folder_name,
			ajax_server->
				relation_mto1_list );

	if ( !ajax_server->relation_mto1_to_one
	||   !list_length(
		ajax_server->
			relation_mto1_to_one->
			relation_foreign_key_list ) )
	{
		char message[ 128 ];

		sprintf(message,
			"relation_mto1_seek(%s) returned empty or incomplete.",
			ajax_folder_name );

		fprintf(stderr,
			"ERROR in %s/%s()/%d: %s\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		printf( "ERROR\n" );
		exit( 1 );
	}

	if ( ajax_server->form_folder->populate_drop_down_process_name )
	{
		ajax_server->populate_drop_down_process =
			process_fetch(
				ajax_server->
					form_folder->
					populate_drop_down_process_name,
				(char *)0 /* document_root */,
				(char *)0 /* relative_source_directory */,
				1 /* check_executable_inside_filesystem */,
				appaserver_parameter_mount_point() );
	}

	if ( ajax_server->populate_drop_down_process )
	{
		ajax_server->foreign_key_list_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			ajax_server_foreign_key_list_string(
				SQL_DELIMITER,
				ajax_server->
					relation_mto1_to_one->
					relation_foreign_key_list );

		ajax_server->system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			ajax_server_process_system_string(
				value,
				ajax_server->populate_drop_down_process,
				ajax_server->foreign_key_list_string );
	}
	else
	{
		ajax_server->attribute_data_list =
			ajax_server_attribute_data_list(
				SQL_DELIMITER,
				value );

		if ( !list_length( ajax_server->attribute_data_list ) )
		{
			char message[ 128 ];

			sprintf(message,
"ajax_server_attribute_data_list(%s) with delimiter=[%c] returned empty.",
				value,
				SQL_DELIMITER );

			fprintf(stderr,
				"ERROR in %s/%s()/%d: %s\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );

			exit( 1 );
		}

		ajax_server->query_data_where =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			query_data_where(
				(char *)0 /* folder_table_name */,
				ajax_server->
					relation_mto1_to_one->
					relation_foreign_key_list
					/* where_attribute_name_list */,
				ajax_server->attribute_data_list
					/* where_attribute_data_list */,
				ajax_server->
					form_folder->
					folder_attribute_list );

		ajax_server->select_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			ajax_server_select_string(
				ajax_server->
					form_folder->
					folder_attribute_primary_key_list );

		ajax_server->system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			ajax_server_folder_system_string(
				application_name,
				form_folder_name,
				ajax_server->query_data_where,
				ajax_server->select_string );
	}

	ajax_server->result_list =
		ajax_server_result_list(
			ajax_server->system_string );

	if ( list_length( ajax_server->result_list ) )
	{
		ajax_server->return_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			ajax_server_return_string(
				SQL_DELIMITER,
				WIDGET_DROP_DOWN_DASH_DELIMITER,
				AJAX_SERVER_DATA_DELIMITER
					/* probably "@&" */,
				AJAX_SERVER_FORMAT_DELIMITER
					/* probably '|' */,
				ajax_server->result_list );
	}

	return ajax_server;
}

AJAX_SERVER *ajax_server_calloc( void )
{
	AJAX_SERVER *ajax_server;

	if ( ! ( ajax_server = calloc( 1, sizeof ( AJAX_SERVER ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		fprintf(stderr,
			"ERROR in %s/%s()/%d: %s\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		printf( "ERROR\n" );
		exit( 1 );
	}

	return ajax_server;
}

char *ajax_server_process_system_string(
		char *value,
		PROCESS *populate_drop_down_process,
		char *ajax_server_foreign_key_list_string )
{
	DICTIONARY *dictionary;

	if ( !value
	||   !populate_drop_down_process
	||   !populate_drop_down_process->command_line
	||   !ajax_server_foreign_key_list_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty or incomplete." );

		fprintf(stderr,
			"ERROR in %s/%s()/%d: %s\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		printf( "ERROR\n" );
		exit( 1 );
	}

	dictionary = dictionary_small();

	dictionary_set(
		dictionary,
		ajax_server_foreign_key_list_string /* key */,
		value /* datum */ );

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	query_drop_down_process_command_line(
		(char *)0 /* application_name */,
		(char *)0 /* session_key */,
		(char *)0 /* login_name */,
		(char *)0 /* role_name */,
		(char *)0 /* state */,
		(char *)0 /* many_folder_name */,
		(char *)0 /* related_column */,
		populate_drop_down_process->process_name,
		dictionary,
		(char *)0 /* query_drop_down_where_string */,
		populate_drop_down_process->command_line,
		(char *)0 /* appaserver_error_filename */ );
}

char *ajax_server_folder_system_string(
		char *application_name,
		char *form_folder_name,
		char *query_data_where,
		char *ajax_server_select_string )
{
	char system_string[ 1024 ];

	if ( !application_name
	||   !form_folder_name
	||   !query_data_where
	||   !ajax_server_select_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		fprintf(stderr,
			"ERROR in %s/%s()/%d: %s\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		printf( "ERROR\n" );
		exit( 1 );
	}

	sprintf(system_string,
		"select	application=%s	"
		"	select=%s 	"
		"	folder=%s 	"
		"	where=\"%s\" 	"
		"	order=select    ",
		application_name,
		ajax_server_select_string,
		form_folder_name,
		query_data_where );

	return strdup( system_string );
}

LIST *ajax_server_result_list( char *system_string )
{
	SPOOL *spool;

	if ( !system_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		fprintf(stderr,
			"ERROR in %s/%s()/%d: %s\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		printf( "ERROR\n" );
		exit( 1 );
	}

	/* Safely returns */
	/* -------------- */
	spool =
		spool_new(
			system_string,
			0 /* not capture_stderr_boolean */ );

	pclose( spool->output_pipe );

	return spool_list( spool->output_filename );
}

char *ajax_server_return_string(
		const char sql_delimiter,
		const char *widget_drop_down_dash_delimiter,
		const char *ajax_server_data_delimiter
			/* probably "@&" */,
		const char ajax_server_format_delimiter
			/* probably '|' */,
		LIST *ajax_server_result_list )
{
	char *format_return_string;
	char *first_result;
	char *data_string;
	char *format_string;

	if ( !list_length( ajax_server_result_list ) ) return (char *)0;

	first_result = list_first( ajax_server_result_list );

	if ( string_exists_character(
		first_result,
		ajax_server_format_delimiter ) )
	{
		data_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			ajax_server_delimited_data_string(
				ajax_server_data_delimiter,
				ajax_server_format_delimiter,
				ajax_server_result_list );

		format_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			ajax_server_delimited_format_string(
				ajax_server_data_delimiter,
				ajax_server_format_delimiter,
				ajax_server_result_list );
	}
	else
	{
		data_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			ajax_server_data_string(
				ajax_server_data_delimiter,
				ajax_server_result_list );

		format_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			ajax_server_list_format_string(
				sql_delimiter,
				ajax_server_data_delimiter,
				widget_drop_down_dash_delimiter,
				ajax_server_result_list );
	}

	format_return_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		ajax_server_format_return_string(
			ajax_server_format_delimiter
				/* probably | */,
			data_string,
			format_string );

	free( data_string );
	free( format_string );

	return format_return_string;
}

char *ajax_server_delimited_data_string(
		const char *ajax_server_data_delimiter
			/* probably "@&" */,
		const char ajax_server_format_delimiter
			/* probably '|' */,
		LIST *ajax_server_result_list )
{
	char data_string[ STRING_64K ];
	char *ptr = data_string;
	char *result;
	char piece_buffer[ 1024 ];

	if ( !list_rewind( ajax_server_result_list ) )
	{
		char message[ 128 ];

		sprintf(message, "ajax_server_result_list is empty." );

		fprintf(stderr,
			"ERROR in %s/%s()/%d: %s\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		printf( "ERROR\n" );
		exit( 1 );
	}

	do {
		result = list_get( ajax_server_result_list );

		if ( !list_first_boolean( ajax_server_result_list ) )
		{
			ptr += sprintf(
				ptr,
				"%s",
				ajax_server_data_delimiter );
		}

		piece(	piece_buffer,
			ajax_server_format_delimiter,
			result,
			0 );

		if (	strlen( data_string ) +
			strlen( piece_buffer ) >= STRING_64K )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_64K );

			fprintf(stderr,
				"ERROR in %s/%s()/%d: %s\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );

			printf( "ERROR\n" );
			exit( 1 );
		}

		ptr += sprintf( ptr, "%s", piece_buffer );

	} while ( list_next( ajax_server_result_list ) );

	return strdup( data_string );
}

char *ajax_server_delimited_format_string(
		const char *ajax_server_data_delimiter
			/* probably "@&" */,
		const char ajax_server_format_delimiter
			/* probably '|' */,
		LIST *ajax_server_result_list )
{
	char format_string[ STRING_64K ];
	char *ptr = format_string;
	char *result;
	char piece_buffer[ 1024 ];

	if ( !list_rewind( ajax_server_result_list ) )
	{
		char message[ 128 ];

		sprintf(message, "ajax_server_result_list is empty." );

		fprintf(stderr,
			"ERROR in %s/%s()/%d: %s\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		printf( "ERROR\n" );
		exit( 1 );
	}

	do {
		result = list_get( ajax_server_result_list );

		piece(	piece_buffer,
			ajax_server_format_delimiter,
			result,
			1 );

		if (	strlen( format_string ) +
			strlen( piece_buffer ) +
			1 >= STRING_64K )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_64K );

			fprintf(stderr,
				"ERROR in %s/%s()/%d: %s\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );

			printf( "ERROR\n" );
			exit( 1 );
		}

		if ( !list_first_boolean( ajax_server_result_list ) )
		{
			ptr += sprintf(
				ptr,
				"%s",
				ajax_server_data_delimiter );
		}

		ptr += sprintf( ptr, "%s", piece_buffer );

	} while ( list_next( ajax_server_result_list ) );

	return strdup( format_string );
}

char *ajax_server_list_format_string(
		const char sql_delimiter,
		const char *ajax_server_data_delimiter
			/* probably "@&" */,
		const char *widget_drop_down_dash_delimiter,
		LIST *ajax_server_result_list )
{
	char format_string[ STRING_64K ];
	char *ptr = format_string;
	char *delimiter_string;
	char *string;

	if ( !list_rewind( ajax_server_result_list ) )
	{
		char message[ 128 ];

		sprintf(message, "ajax_server_result_list is empty." );

		fprintf(stderr,
			"ERROR in %s/%s()/%d: %s\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		printf( "ERROR\n" );
		exit( 1 );
	}

	delimiter_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		sql_delimiter_string(
			sql_delimiter );

	*format_string = '\0';

	do {
		if ( !list_first_boolean( ajax_server_result_list ) )
		{
			ptr += sprintf(
				ptr,
				"%s",
				ajax_server_data_delimiter );
		}

		string =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			ajax_server_format_string(
				widget_drop_down_dash_delimiter,
				delimiter_string,
				(char *)list_get(
					ajax_server_result_list ) );

		if (	strlen( format_string ) +
			strlen( string ) >= STRING_64K )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_64K );

			fprintf(stderr,
				"ERROR in %s/%s()/%d: %s\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );

			printf( "ERROR\n" );
			exit( 1 );
		}

		ptr += sprintf( ptr, "%s", string );

	} while ( list_next( ajax_server_result_list ) );

	return strdup( format_string );
}

char *ajax_server_format_string(
		const char *widget_drop_down_dash_delimiter,
		char *sql_delimiter_string,
		char *result )
{
	static char format_string[ 1024 ];

	if ( !sql_delimiter_string
	||   !result )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		fprintf(stderr,
			"ERROR in %s/%s()/%d: %s\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		printf( "ERROR\n" );
		exit( 1 );
	}

	string_initial_capital( format_string, result );

	string_search_replace(
		format_string /* source_destination */,
		sql_delimiter_string /* search_string */,
		(char *)widget_drop_down_dash_delimiter /* replace_string */ );

	return format_string;
}

char *ajax_server_format_return_string(
		const char ajax_server_format_delimiter
			/* probably '|' */,
		char *data_string,
		char *format_string )
{
	char format_return_string[ STRING_64K ];

	if ( !data_string
	||   !format_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		fprintf(stderr,
			"ERROR in %s/%s()/%d: %s\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		printf( "ERROR\n" );
		exit( 1 );
	}

	if (	strlen( data_string ) +
		strlen( format_string ) +
		1 >= STRING_64K )
	{
		char message[ 128 ];

		sprintf(message,
			STRING_OVERFLOW_TEMPLATE,
			STRING_64K );

		fprintf(stderr,
			"ERROR in %s/%s()/%d: %s\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		printf( "ERROR\n" );
		exit( 1 );
	}

	sprintf(format_return_string,
		"%s%c%s",
		data_string,
		ajax_server_format_delimiter,
		format_string );

	return strdup( format_return_string );
}

LIST *ajax_server_attribute_data_list(
		const char sql_delimiter,
		char *value )
{
	if ( !value )
	{
		char message[ 128 ];

		sprintf(message, "value is empty." );

		fprintf(stderr,
			"ERROR in %s/%s()/%d: %s\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		printf( "ERROR\n" );
		exit( 1 );
	}

	return
	list_string_list(
		value,
		sql_delimiter );
}

char *ajax_server_foreign_key_list_string(
		const char sql_delimiter,
		LIST *foreign_key_list )
{
	if ( !list_length( foreign_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "foreign_key_list is empty." );

		fprintf(stderr,
			"ERROR in %s/%s()/%d: %s\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		printf( "ERROR\n" );
		exit( 1 );
	}

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	list_delimited(
		foreign_key_list,
		sql_delimiter );
}

char *ajax_server_data_string(
		const char *ajax_server_data_delimiter
			/* probably "@&" */,
		LIST *ajax_server_result_list )
{
	char data_string[ STRING_1K ];
	char *ptr = data_string;
	char *result;

	if ( !ajax_server_data_delimiter
	||   !list_rewind( ajax_server_result_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		fprintf(stderr,
			"ERROR in %s/%s()/%d: %s\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		printf( "ERROR\n" );
		exit( 1 );
	}

	do {
		result = list_get( ajax_server_result_list );

		if (	strlen( data_string ) +
			strlen( result ) +
			1 >= STRING_1K )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_1K );

			fprintf(stderr,
				"ERROR in %s/%s()/%d: %s\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );

			printf( "ERROR\n" );
			exit( 1 );
		}

		if ( !list_first_boolean( ajax_server_result_list ) )
		{
			ptr += sprintf(
				ptr,
				"%s",
				ajax_server_data_delimiter );
		}

		ptr += sprintf( ptr, "%s", result );

	} while ( list_next( ajax_server_result_list ) );

	return strdup( data_string );
}

char *ajax_server_select_string( LIST *primary_key_list )
{
	if ( !list_length( primary_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "primary_key_list is empty." );

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
		primary_key_list,
		',' /* delimiter */ );
}

char *ajax_client_popup_widget_name(
		const char sql_delimiter,
		LIST *ajax_foreign_key_list )
{
	return
	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	list_delimited_string(
		ajax_foreign_key_list,
		sql_delimiter );
}

char *ajax_client_form_widget_name(
		const char sql_delimiter,
		LIST *form_foreign_key_list )
{
	if ( !list_length( form_foreign_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "form_foreign_key_list is empty." );

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
		form_foreign_key_list,
		sql_delimiter );
}

char *ajax_client_form_widget_id(
		const char sql_delimiter,
		char *many_folder_name,
		LIST *form_foreign_key_list )
{
	char *delimited_string;
	char form_widget_id[ 128 ];

	if ( !many_folder_name
	||   !list_length( form_foreign_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	delimited_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		list_delimited_string(
			form_foreign_key_list,
			sql_delimiter );

	snprintf(
		form_widget_id,
		sizeof ( form_widget_id ),
		"%s_%s",
		many_folder_name,
		delimited_string );

	free( delimited_string );

	return strdup( form_widget_id );
}

AJAX_CLIENT *ajax_client_relation_mto1_new(
		RELATION_MTO1 *relation_mto1,
		LIST *relation_mto1_to_one_list,
		boolean top_select_boolean )
{
	RELATION_MTO1 *relation_mto1_ajax;

	if ( !relation_mto1
	||   !relation_mto1->relation )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"relation_mto1 is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !relation_mto1->relation->ajax_fill_drop_down )
	{
		return NULL;
	}

	relation_mto1_ajax =
		relation_mto1_ajax_fill_seek(
			relation_mto1_to_one_list );

	if ( !relation_mto1_ajax ) return NULL;

	if ( !relation_mto1_ajax->
		one_folder
	||   !relation_mto1_ajax->
		one_folder->
			folder_attribute_primary_key_list )
	{
		char message[ 128 ];

		sprintf(message, "relation_mto1 is incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	ajax_client_new(
		relation_mto1->
			many_folder_name,
		relation_mto1->
			one_folder_name
			/* form_folder_name */,
		relation_mto1->
			relation_foreign_key_list
			/* form_foreign_key_list */,
		relation_mto1->
			one_folder->
			folder_attribute_primary_key_list
			/* form_primary_key_list */,
		relation_mto1_ajax->
			one_folder_name
			/* ajax_folder_name */,
		relation_mto1_ajax->
			one_folder->
			folder_attribute_primary_key_list
			/* ajax_primary_key_list */,
		relation_mto1_ajax->
			relation_foreign_key_list
			/* ajax_foreign_key_list */,
		top_select_boolean );
}

