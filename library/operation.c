/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/operation.c					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "application.h"
#include "appaserver.h"
#include "piece.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "button.h"
#include "sql.h"
#include "folder_operation.h"
#include "dictionary_separate.h"
#include "security.h"
#include "operation.h"

void operation_row_execute( char *command_line )
{
	if ( system( command_line ) ){};
}

OPERATION_SEMAPHORE *operation_semaphore_calloc( void )
{
	OPERATION_SEMAPHORE *operation_semaphore;

	operation_semaphore =
		calloc( 1, sizeof ( OPERATION_SEMAPHORE ) );

	if ( !operation_semaphore )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return operation_semaphore;
}

OPERATION_SEMAPHORE *operation_semaphore_new(
		char *application_name,
		char *operation_name,
		char *appaserver_data_directory,
		pid_t parent_process_id,
		int operation_row_checked_count )
{
	OPERATION_SEMAPHORE *operation_semaphore;

	operation_semaphore = operation_semaphore_calloc();

	operation_semaphore->filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		operation_semaphore_filename(
			application_name,
			operation_name,
			appaserver_data_directory,
			parent_process_id );

	if ( ( operation_semaphore->group_first_time =
		operation_semaphore_group_first_time(
			operation_semaphore->filename ) ) )
	{
		operation_semaphore->row_current = 1;
	}
	else
	{
		operation_semaphore->row_current =
			operation_semaphore_row_current(
				operation_semaphore->filename );

		operation_semaphore->row_current++;
	}

	if ( ( operation_semaphore->group_last_time =
			operation_semaphore_group_last_time(
				operation_semaphore->row_current,
				operation_row_checked_count ) ) )
	{
		if ( !operation_semaphore->group_first_time )
		{
			operation_semaphore_remove_file(
				operation_semaphore->filename );
		}
	}
	else
	{
		operation_semaphore_write(
			operation_semaphore->filename,
			operation_semaphore->row_current );
	}

	return operation_semaphore;
}

void operation_semaphore_remove_file( char *filename )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"rm -f %s",
		filename );

	if ( system( system_string ) ){};
}

char *operation_semaphore_filename(
		char *application_name,
		char *operation_name,
		char *appaserver_data_directory,
		pid_t parent_process_id )
{
	char semaphore_filename[ 512 ];

	if ( !application_name
	||   !appaserver_data_directory
	||   !operation_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(semaphore_filename,
		"%s/%s/%s_%d.dat",
		appaserver_data_directory,
		application_name,
		operation_name,
		parent_process_id );

	return strdup( semaphore_filename );
}

boolean operation_semaphore_group_first_time( char *filename )
{
	return
	!application_file_exists_boolean(
		filename );
}

void operation_semaphore_initialize_file( char *filename )
{
	operation_semaphore_write( filename, 1 /* current_row */ );
}

int operation_semaphore_row_current( char *filename )
{
	FILE *input_file;
	char buffer[ 128 ];

	/* Safely returns */
	/* -------------- */
	input_file = appaserver_input_file( filename );

	string_input( buffer, input_file, sizeof ( buffer ) );
	fclose( input_file );

	if ( !*buffer )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"buffer is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return atoi( buffer );
}

boolean operation_semaphore_group_last_time(
		int row_current,
		int operation_row_checked_count )
{
	return ( row_current == operation_row_checked_count );
}

void operation_semaphore_write(
		char *filename,
		int row_current )
{
	FILE *output_file;

	/* Safely returns */
	/* -------------- */
	output_file = appaserver_output_file( filename );

	fprintf( output_file, "%d\n", row_current );
	fclose( output_file );
}

OPERATION_ROW *operation_row_calloc( void )
{
	OPERATION_ROW *operation_row;

	if ( ! ( operation_row = calloc( 1, sizeof ( OPERATION_ROW ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return operation_row;
}

boolean operation_row_checked(
		DICTIONARY *operation_dictionary,
		int row_number,
		char *operation_name )
{
	char key[ 256 ];

	if ( !operation_dictionary
	||   row_number < 1
	||   !operation_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(key, 
	 	"%s_%d",
	 	operation_name,
		row_number );

	if ( dictionary_get( key, operation_dictionary ) )
		return 1;
	else
		return 0;
}

DICTIONARY *operation_row_single_dictionary(
		DICTIONARY *row_dictionary,
		LIST *attribute_name_list,
		int row_number )
{
	DICTIONARY *single_row_dictionary = dictionary_small();
	char key[ 128 ];
	char *attribute_name;
	char *data;

	if ( !list_rewind( attribute_name_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: list_rewind() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		attribute_name = list_get( attribute_name_list );

		sprintf(key,
			"%s_%d",
			attribute_name,
			row_number );

		if ( ( data =
			dictionary_get(
				key,
				row_dictionary ) ) )
		{
			dictionary_set(
				single_row_dictionary,
				attribute_name,
				data );
		}
	} while ( list_next( attribute_name_list ) );

	sprintf( key, "execute_yn_%d", row_number );
	dictionary_set( single_row_dictionary, strdup( key ), "y" );

	return single_row_dictionary;
}

boolean operation_delete_boolean( char *operation_name )
{
	return ( string_strncmp( operation_name, "delete" ) == 0 );
}

boolean operation_drilldown_boolean( char *operation_name )
{
	if ( string_strcmp( operation_name, "drilldown" ) == 0 )
		return 1;
	else
	if ( string_strcmp( operation_name, "detail" ) == 0 )
		return 1;
	else
		return 0;
}

OPERATION *operation_calloc( void )
{
	OPERATION *operation;

	if ( ! ( operation = calloc( 1, sizeof ( OPERATION ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return operation;
}

char *operation_image_source(
		boolean delete_boolean,
		boolean drilldown_boolean )
{
	char image_source[ 128 ];

	*image_source = '\0';

	if ( delete_boolean )
	{
		sprintf(image_source,
		 	"/%s/trashcan.png",
		 	BUTTON_IMAGE_RELATIVE_DIRECTORY );
	}
	else
	if ( drilldown_boolean )
	{
		sprintf(image_source,
			"/%s/drill.png",
			BUTTON_IMAGE_RELATIVE_DIRECTORY );
	}

	if ( *image_source )
		return strdup( image_source );
	else
		return (char *)0;
}

char *operation_delete_warning_javascript(
		const char *delete_warning_javascript,
		boolean delete_boolean )
{
	if ( delete_boolean )
	{
		return (char *)delete_warning_javascript;
	}
	else
	{
		return (char *)0;
	}
}

OPERATION *operation_fetch(
		char *operation_name,
		boolean fetch_process )
{
	if ( !operation_name || !*operation_name ) return NULL;

	return
	operation_parse(
		string_pipe_input(
			operation_system_string(
				OPERATION_SELECT,
				OPERATION_TABLE,
				operation_primary_where(
					operation_name ) ) ),
		fetch_process );
}

char *operation_primary_where( char *operation_name )
{
	static char where[ 128 ];

	if ( !operation_name || !*operation_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: operation_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf( where, "operation = '%s'", operation_name );

	return where;
}

char *operation_system_string(
		char *select,
		char *table,
		char *primary_where )
{
	static char system_string[ 512 ];

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		select,
		table,
		primary_where );

	return system_string;
}

OPERATION *operation_parse(
		char *input,
		boolean fetch_process )
{
	OPERATION *operation;
	char operation_name[ 128 ];
	char output_yn[ 128 ];

	if ( !input || !*input ) return (OPERATION *)0;

	operation = operation_calloc();

	/* See OPERATION_SELECT */
	/* -------------------- */
	piece( operation_name, SQL_DELIMITER, input, 0 );
	piece( output_yn, SQL_DELIMITER, input, 1 );

	operation->operation_name = strdup( operation_name );

	operation->output_boolean =
		operation_output_boolean(
			output_yn );

	operation->delete_boolean =
		operation_delete_boolean(
			operation_name );

	operation->drilldown_boolean =
		operation_drilldown_boolean(
			operation_name );

	operation->image_source =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		operation_image_source(
			operation->delete_boolean,
			operation->drilldown_boolean );

	operation->delete_warning_javascript =
		/* --------------------------------------------------- */
		/* Returns OPERATION_DELETE_WARNING_JAVASCRIPT or null */
		/* --------------------------------------------------- */
		operation_delete_warning_javascript(
			OPERATION_DELETE_WARNING_JAVASCRIPT,
			operation->delete_boolean );

	operation->widget_container =
		operation_widget_container(
			operation->operation_name,
			operation->image_source,
			operation->delete_warning_javascript );

	if ( fetch_process )
	{
		char *mount_point = appaserver_parameter_mount_point();

		operation->process =
			process_fetch(
				operation_name /* process_name */,
				/* ----------------------- */
				/* Not fetching javascript */
				/* ----------------------- */
				(char *)0 /* document_root_directory */,
				(char *)0 /* relative_source_directory */,
				/* -------------------------------------- */
				/* Should move to opertion_row_list_new() */
				/* -------------------------------------- */
				1 /* check_executable_inside_filesystem */,
				mount_point );
	
		if ( !operation->process )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: process_fetch(%s) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				operation_name );
			exit( 1 );
		}
	}

	return operation;
}

boolean operation_output_boolean( char *output_yn )
{
	if ( !output_yn )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: output_yn is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return (*output_yn == 'y');
}

WIDGET_CONTAINER *operation_widget_container(
		char *operation_name,
		char *image_source,
		char *delete_warning_javascript )
{
	WIDGET_CONTAINER *widget_container;

	widget_container =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		widget_container_new(
			checkbox, operation_name /* widget_name */ );

	widget_container->checkbox->prompt = operation_name;
	widget_container->checkbox->onclick = delete_warning_javascript;
	widget_container->checkbox->image_source = image_source;

	return widget_container;
}

char *operation_html(
		WIDGET_CONTAINER *widget_container,
		char *state,
		int row_number,
		char *background_color,
		boolean delete_mask_boolean )
{
	char *html;

	if ( !widget_container )
	{
		char message[ 128 ];

		sprintf(message, "widget_container is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( delete_mask_boolean )
	{
		char prompt_display[ 128 ];

		if ( !widget_container->checkbox )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
				"widget_container->checkbox is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		string_initial_capital(
			prompt_display,
			widget_container->checkbox->prompt );

		return strdup( prompt_display );
	}

	html =
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	widget_container_html(
		state,
		row_number /* -1 will omit index suffix */,
		(LIST *)0 /* query_row_cell_list */,
		(RELATION_JOIN_ROW *)0 /* relation_join_row */,
		background_color,
		widget_container );

	return html;
}

OPERATION_ROW *operation_row_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *target_frame,
		char *state,
		LIST *folder_operation_list,
		LIST *primary_key_list,
		char *update_results_string,
		char *update_error_string,
		int dictionary_highest_index,
		DICTIONARY *operation_row_list_dictionary,
		int row_number,
		DICTIONARY *dictionary_single_row )
{
	FOLDER_OPERATION *folder_operation;
	OPERATION_ROW *operation_row;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !folder_name
	||   !target_frame
	||   !state
	||   !list_length( primary_key_list )
	||   !dictionary_highest_index
	||   !row_number )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !dictionary_length( operation_row_list_dictionary )
	||   !dictionary_length( dictionary_single_row ) )
	{
		return NULL;
	}

	if ( !list_rewind( folder_operation_list ) ) return NULL;

	operation_row = operation_row_calloc();

	operation_row->operation_row_checked_list = list_new();

	operation_row->appaserver_error_filename =
		appaserver_error_filename(
			application_name );

	do {
		folder_operation = list_get( folder_operation_list );

		if ( !folder_operation->operation )
		{
			char message[ 128 ];

			sprintf(message,
				"folder_operation->operation is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( !folder_operation->operation->process )
		{
			char message[ 128 ];

			sprintf(message, "operation->process is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( !folder_operation->operation->process->command_line )
		{
			char message[ 128 ];

			sprintf(message, "process->command_line is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		list_set(
			operation_row->operation_row_checked_list,
			operation_row_checked_new(
				session_key,
				login_name,
				role_name,
				folder_name,
				target_frame,
				state,
				primary_key_list,
				update_results_string,
				update_error_string,
				dictionary_highest_index,
				operation_row_list_dictionary,
				row_number,
				dictionary_single_row,
				folder_operation->operation_name,
				folder_operation->
					operation->
					process->
					command_line,
				folder_operation->
					operation->
					output_boolean,
				operation_row->appaserver_error_filename ) );

	} while ( list_next( folder_operation_list ) );

	if ( !list_length( operation_row->operation_row_checked_list ) )
	{
		list_free( operation_row->operation_row_checked_list );
		free( operation_row );
		return (OPERATION_ROW *)0;
	}
	else
	{
		return operation_row;
	}
}

OPERATION_ROW_CHECKED *operation_row_checked_new(
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *target_frame,
		char *state,
		LIST *primary_key_list,
		char *update_results_string,
		char *update_error_string,
		int dictionary_highest_index,
		DICTIONARY *operation_row_list_dictionary,
		int row_number,
		DICTIONARY *dictionary_single_row,
		char *operation_name,
		char *process_command_line,
		boolean output_boolean,
		char *appaserver_error_filename )
{
	OPERATION_ROW_CHECKED *operation_row_checked;

	if ( !operation_row_checked_boolean(
		operation_row_list_dictionary,
		row_number,
		operation_name ) )
	{
		return NULL;
	}

	operation_row_checked =
		operation_row_checked_calloc();

	operation_row_checked->count =
		operation_row_checked_count(
			dictionary_highest_index,
			operation_row_list_dictionary,
			operation_name );

	if ( !operation_row_checked->count )
	{
		char message[ 128 ];

		sprintf(message,
			"operation_row_checked_count() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	operation_row_checked->output_boolean = output_boolean;

	operation_row_checked->primary_key_data_list =
		dictionary_data_list(
			primary_key_list,
			dictionary_single_row );

	if ( !list_length( operation_row_checked->primary_key_data_list ) )
	{
		char message[ 1024 ];

		sprintf(message,
			"dictionary_data_list(%s) returned empty.",
			list_display( primary_key_list ) );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	operation_row_checked->command_line =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		operation_row_checked_command_line(
			ATTRIBUTE_MULTI_KEY_DELIMITER,
			session_key,
			login_name,
			role_name,
			folder_name,
			target_frame,
			state,
			update_results_string,
			update_error_string,
			operation_row_list_dictionary,
			dictionary_single_row,
			operation_name,
			process_command_line,
			appaserver_error_filename,
			operation_row_checked->count,
			getpid() /* parent_process_id */,
			operation_row_checked->primary_key_data_list );

	return operation_row_checked;
}

OPERATION_ROW_CHECKED *operation_row_checked_calloc( void )
{
	OPERATION_ROW_CHECKED *operation_row_checked;

	if ( ! ( operation_row_checked =
			calloc( 1, sizeof ( OPERATION_ROW_CHECKED ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return operation_row_checked;
}

boolean operation_row_checked_boolean(
		DICTIONARY *operation_row_list_dictionary,
		int row_number,
		char *operation_name )
{
	return
	dictionary_key_exists(
		operation_row_list_dictionary,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		operation_row_checked_key(
			row_number,
			operation_name ) );
}

int operation_row_checked_count(
		int dictionary_highest_index,
		DICTIONARY *operation_row_list_dictionary,
		char *operation_name )
{
	int count = 0;
	int row_number;

	if ( !dictionary_highest_index ) return 0;

	for(	row_number = 1;
		row_number <= dictionary_highest_index;
		row_number++ )
	{
		if ( operation_row_checked_boolean(
			operation_row_list_dictionary,
			row_number,
			operation_name ) )
		{
			count++;
		}
	}

	return count;
}
 
char *operation_row_checked_command_line(
		const char attribute_multi_key_delimiter,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *target_frame,
		char *state,
		char *update_results_string,
		char *update_error_string,
		DICTIONARY *operation_row_list_dictionary,
		DICTIONARY *dictionary_single_row,
		char *operation_name,
		char *process_command_line,
		char *appaserver_error_filename,
		int operation_row_checked_count,
		pid_t parent_process_id,
		LIST *primary_key_data_list )
{
	char command_line[ STRING_16K ];
	char *tmp;

	if ( !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !target_frame
	||   !state
	||   !dictionary_length( dictionary_single_row )
	||   !operation_name
	||   !process_command_line
	||   !operation_row_checked_count
	||   !parent_process_id
	||   !list_length( primary_key_data_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	string_strcpy(
		command_line,
		process_command_line,
		STRING_16K );

	string_replace_command_line(
		command_line,
		session_key,
		PROCESS_SESSION_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		login_name,
		PROCESS_LOGIN_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		role_name,
		PROCESS_ROLE_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		folder_name,
		PROCESS_FOLDER_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		folder_name,
		PROCESS_TABLE_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		target_frame,
		PROCESS_TARGET_FRAME_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		state,
		PROCESS_STATE_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		update_results_string,
		PROCESS_UPDATE_RESULTS_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		update_error_string,
		PROCESS_UPDATE_ERROR_PLACEHOLDER );

	/* Must preceed PROCESS_NAME_PLACEHOLDER */
	/* ------------------------------------- */
	process_replace_pid_command_line(
		command_line,
		parent_process_id,
		PROCESS_PID_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		operation_name,
		PROCESS_NAME_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		(tmp = list_display_delimited(
			primary_key_data_list,
			(char)attribute_multi_key_delimiter ) ),
		PROCESS_PRIMARY_PLACEHOLDER );

	free( tmp );

	process_replace_row_count_command_line(
		command_line,
		operation_row_checked_count,
		PROCESS_ROW_COUNT_PLACEHOLDER );

	process_replace_dictionary_command_line(
		command_line,
		operation_row_list_dictionary,
		DICTIONARY_ATTRIBUTE_DATUM_DELIMITER,
		DICTIONARY_ELEMENT_DELIMITER,
		PROCESS_DICTIONARY_PLACEHOLDER );

	dictionary_replace_command_line(
		command_line,
		dictionary_single_row );

	sprintf(command_line + strlen( command_line ),
		" 2>>%s",
		appaserver_error_filename );

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	string_escape_dollar( command_line );
}

char *operation_row_checked_execute(
		char *command_line,
		boolean operation_row_list_output_boolean )
{
	if ( !command_line )
	{
		char message[ 128 ];

		sprintf(message, "command_line is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	if ( operation_row_list_output_boolean )
	{
		security_system(
			SECURITY_FORK_CHARACTER,
			SECURITY_FORK_STRING,
			command_line );

		return (char *)0;
	}
	else
	{
		/* Returns heap memory or null */
		/* --------------------------- */
		return string_pipe_fetch( command_line );
	}
}

OPERATION_ROW_LIST *operation_row_list_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *target_frame,
		char *state,
		LIST *folder_operation_list,
		LIST *primary_key_list,
		LIST *folder_attribute_name_list,
		DICTIONARY *operation_dictionary,
		DICTIONARY *no_display_dictionary,
		DICTIONARY *multi_row_dictionary,
		DICTIONARY *drillthru_dictionary,
		char *update_results_string,
		char *update_error_string )
{
	OPERATION_ROW_LIST *operation_row_list;
	int row_number;
	DICTIONARY *single_row;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !folder_name
	||   !target_frame
	||   !state
	||   !list_length( primary_key_list )
	||   !list_length( folder_attribute_name_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !dictionary_length( operation_dictionary )
	||   !dictionary_length( multi_row_dictionary ) )
	{
		return NULL;
	}

	operation_row_list = operation_row_list_calloc();

	operation_row_list->dictionary_highest_index =
		/* ------------ */
		/* Returns >= 0 */
		/* ------------ */
		dictionary_highest_index(
			operation_dictionary );

	if ( !operation_row_list->dictionary_highest_index ) return NULL;

	operation_row_list->dictionary =
		/* ----------------------------------------- */
		/* Returns a combination of all dictionaries */
		/* ----------------------------------------- */
		operation_row_list_dictionary(
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			operation_dictionary,
			no_display_dictionary,
			drillthru_dictionary );

	operation_row_list->list = list_new();

	for(	row_number = 1;
		row_number <= operation_row_list->dictionary_highest_index;
		row_number++ )
	{
		single_row =
			dictionary_single_row(
				folder_attribute_name_list,
				row_number,
				multi_row_dictionary );

		if ( !dictionary_length( single_row ) ) continue;

		list_set(
			operation_row_list->list,
			operation_row_new(
				application_name,
				session_key,
				login_name,
				role_name,
				folder_name,
				target_frame,
				state,
				folder_operation_list,
				primary_key_list,
				update_results_string,
				update_error_string,
				operation_row_list->dictionary_highest_index,
				operation_row_list->dictionary,
				row_number,
				single_row ) );
	}

	if ( !list_length( operation_row_list->list ) )
	{
		free( operation_row_list );
		return (OPERATION_ROW_LIST *)0;
	}

	operation_row_list->output_boolean =
		operation_row_list_output_boolean(
			operation_row_list->list
				/* operation_row_list */ );

	return operation_row_list;
}

OPERATION_ROW_LIST *operation_row_list_calloc( void )
{
	OPERATION_ROW_LIST *operation_row_list;

	operation_row_list =
		calloc( 1, sizeof ( OPERATION_ROW_LIST ) );

	if ( !operation_row_list )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return operation_row_list;
}

char *operation_row_list_execute( OPERATION_ROW_LIST *operation_row_list )
{
	char operation_error_message_list_string[ STRING_16K ];
	char *ptr = operation_error_message_list_string;
	char *operation_error_message;
	OPERATION_ROW *operation_row;
	OPERATION_ROW_CHECKED *operation_row_checked;

	if ( !operation_row_list ) return (char *)0;

	if ( !list_rewind( operation_row_list->list ) ) return (char *)0;

	do {
		operation_row =
			list_get(
				operation_row_list->list );

		if ( !list_rewind( operation_row->operation_row_checked_list ) )
			continue;

		do {
			operation_row_checked =
				list_get(
					operation_row->
						operation_row_checked_list );

			if ( !operation_row_checked->command_line )
			{
				fprintf(stderr,
			"ERROR in %s/%s()/%d: command_line is empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}

			operation_error_message =
				/* --------------------------------------- */
				/* Returns operation_error_message or null */
				/* --------------------------------------- */
				operation_row_checked_execute(
					operation_row_checked->command_line,
					operation_row_list->output_boolean );

			if ( operation_error_message )
			{
				if ( ptr !=
				     operation_error_message_list_string )
				{
					ptr += sprintf( ptr, "\n<br>" );

					ptr += sprintf(
						ptr,
						"%s",
						operation_error_message );
				}
			}

		} while ( list_next(
				operation_row->
					operation_row_checked_list ) );

	} while ( list_next( operation_row_list->list ) );

	if ( ptr == operation_error_message_list_string )
		return (char *)0;
	else
		return strdup( operation_error_message_list_string );
}

boolean operation_row_list_output_boolean( LIST *operation_row_list )
{
	OPERATION_ROW *operation_row;
	OPERATION_ROW_CHECKED *operation_row_checked;

	if ( !list_rewind( operation_row_list ) ) return 0;

	do {
		operation_row =
			list_get(
				operation_row_list );

		if ( !list_rewind(
				operation_row->
					operation_row_checked_list ) )
		{
			continue;
		}

		do {
			operation_row_checked =
				list_get(
					operation_row->
						operation_row_checked_list );

			if ( operation_row_checked->output_boolean ) return 1;

		} while ( list_next(
				operation_row->
					operation_row_checked_list ) );

	} while ( list_next( operation_row_list ) );

	return 0;
}

DICTIONARY *operation_row_list_dictionary(
		char *dictionary_separate_no_display_prefix,
		char *dictionary_separate_drillthru_prefix,
		DICTIONARY *operation_dictionary,
		DICTIONARY *no_display_dictionary,
		DICTIONARY *drillthru_dictionary )
{
	DICTIONARY *dictionary;
	LIST *key_list;
	char *key;
	char dictionary_key[ 128 ];

	dictionary = dictionary_medium_copy( operation_dictionary );

	if ( dictionary_length( no_display_dictionary ) )
	{
		key_list = dictionary_key_list( no_display_dictionary );

		list_rewind( key_list );

		do {
			key = list_get( key_list );

			sprintf(dictionary_key,
				"%s%s",
				dictionary_separate_no_display_prefix,
				key );

			dictionary_set(
				dictionary,
				strdup( dictionary_key ),
				/* --------------------------------------- */
				/* Returns component of dictionary or null */
				/* --------------------------------------- */
				dictionary_get(
					key,
					no_display_dictionary ) );

		} while( list_next( key_list ) );

		list_free_container( key_list );
	}

	if ( dictionary_length( drillthru_dictionary ) )
	{
		key_list = dictionary_key_list( drillthru_dictionary );

		list_rewind( key_list );

		do {
			key = list_get( key_list );

			sprintf(dictionary_key,
				"%s%s",
				dictionary_separate_drillthru_prefix,
				key );

			dictionary_set(
				dictionary,
				strdup( dictionary_key ),
				/* --------------------------------------- */
				/* Returns component of dictionary or null */
				/* --------------------------------------- */
				dictionary_get(
					key,
					drillthru_dictionary ) );

		} while( list_next( key_list ) );

		list_free_container( key_list );
	}

	return dictionary;
}

char *operation_row_checked_key(
		int row_number,
		char *operation_name )
{
	static char key[ 256 ];

	if ( !row_number
	||   !operation_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(key, "%s_%d",
		(operation_name) ? operation_name : "",
		row_number );

	return key;
}

OPERATION *operation_blank_new( void )
{
	OPERATION *operation;

	operation = operation_calloc();

	operation->widget_container =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		widget_container_new(
			blank /* widget_type */,
			(char *)0 /* widget_name */ );

	return operation;
}

