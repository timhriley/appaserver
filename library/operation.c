/* $APPASERVER_HOME/library/operation.c					*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "String.h"
#include "piece.h"
#include "appaserver_error.h"
#include "appaserver_library.h"
#include "button.h"
#include "role_operation.h"
#include "operation.h"

void operation_row_execute( char *command_line )
{
	if ( system( command_line ) ){};
}

OPERATION_SEMAPHORE *operation_semaphore_calloc( void )
{
	OPERATION_SEMAPHORE *operation_semaphore;

	operation_semaphore =
		calloc( 1, sizeof( OPERATION_SEMAPHORE ) );

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
			char *operation_name,
			char *appaserver_data_directory,
			pid_t parent_process_id,
			int operation_row_total )
{
	OPERATION_SEMAPHORE *operation_semaphore;

	operation_semaphore = operation_semaphore_calloc();

	operation_semaphore->filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		operation_semaphore_filename(
			operation_name,
			appaserver_data_directory,
			parent_process_id );

	if ( ( operation_semaphore->group_first_time =
		operation_semaphore_group_first_time(
			operation_semaphore->filename ) ) )
	{
		operation_semaphore_initialize_file(
			operation_semaphore->filename );

		operation_semaphore->row_current = 1;
	}
	else
	{
		operation_semaphore->row_current =
			operation_semaphore_row_current(
				operation_semaphore->filename );
	}

	if ( ( operation_semaphore->group_last_time =
			operation_semaphore_group_last_time(
				operation_semaphore->row_current,
				operation_row_total ) ) )
	{
		operation_semaphore_remove_file(
			operation_semaphore->filename );
	}
	else
	{
		operation_semaphore_increment(
			operation_semaphore->filename,
			operation_semaphore->row_current );
	}

	return operation_semaphore;
}

void operation_semaphore_remove_file(
			char *filename )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"rm -f %s",
		filename );

	if ( system( system_string ) ){};
}

char *operation_semaphore_filename(
			char *operation_name,
			char *appaserver_data_directory,
			pid_t parent_process_id )
{
	char semaphore_filename[ 256 ];

	if ( !appaserver_data_directory
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
		"/%s/%s_%d.dat",
		appaserver_data_directory,
		operation_name,
		parent_process_id );

	return strdup( semaphore_filename );
}

boolean operation_semaphore_group_first_time( char *filename )
{
	return timlib_file_exists( filename );
}

void operation_semaphore_initialize_file( char *filename )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"echo 1 > %s",
		filename );

	if ( system( system_string ) ){};
}

int operation_semaphore_row_current( char *filename )
{
	char system_string[ 1024 ];

	sprintf( system_string,
		 "cat %s",
		 filename );

	return atoi( string_pipe_fetch( system_string ) );
}

boolean operation_semaphore_group_last_time(
			int row_current,
			int operation_row_total )
{
	return ( row_current == operation_row_total );
}

void operation_semaphore_increment(
			char *filename,
			int row_current )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"echo %d > %s",
		row_current + 1,
		filename );

	if ( system( system_string ) ){};
}

OPERATION_ROW *operation_row_calloc( void )
{
	OPERATION_ROW *operation_row;

	if ( ! ( operation_row = calloc( 1, sizeof( OPERATION_ROW ) ) ) )
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
	char key[ 128 ];

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

		data = (char *)0;

		if ( dictionary_index_data(
				&data,
				row_dictionary,
				attribute_name,
				row_number ) == 1 )
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

boolean operation_detail_boolean( char *operation_name )
{
	return ( string_strcmp( operation_name, "detail" ) == 0 );
}

OPERATION *operation_calloc( void )
{
	OPERATION *operation;

	if ( ! ( operation = calloc( 1, sizeof( OPERATION ) ) ) )
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
			boolean detail_boolean )
{
	char image_source[ 128 ];

	*image_source = '\0';

	if ( delete_boolean )
	{
		sprintf(image_source,
		 	"/%s/trashcan.gif",
		 	BUTTON_IMAGE_RELATIVE_DIRECTORY );
	}
	else
	if ( detail_boolean )
	{
		sprintf(image_source,
			"/%s/magnify_glass.gif",
			BUTTON_IMAGE_RELATIVE_DIRECTORY );
	}

	if ( *image_source )
		return strdup( image_source );
	else
		return (char *)0;
}

char *operation_delete_warning_javascript(
			char *delete_warning_javascript,
			boolean delete_boolean )
{
	if ( delete_boolean )
	{
		return delete_warning_javascript;
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
	if ( !operation_name || !*operation_name )
	{
		return (OPERATION *)0;
	}

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
	OPERATION *operation = operation_calloc();
	char operation_name[ 128 ];
	char output_yn[ 128 ];

	piece( operation_name, SQL_DELIMITER, input, 0 );
	piece( output_yn, SQL_DELIMITER, input, 1 );

	operation->operation_name = strdup( operation_name );

	if ( fetch_process )
	{
		operation->process =
			process_fetch(
				operation_name,
				(char *)0 /* document_root_directory */,
				(char *)0 /* relative_source_directory */,
				1 /* check_executable_inside_filesystem */ );
	
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

	operation->output_boolean =
		operation_output_boolean(
			output_yn );

	operation->delete_boolean =
		operation_delete_boolean(
			operation_name );

	operation->detail_boolean =
		operation_detail_boolean(
			operation_name );

	operation->image_source =
		operation_image_source(
			operation->delete_boolean,
			operation->detail_boolean );

	operation->delete_warning_javascript =
		/* --------------------------------------------------- */
		/* Returns OPERATION_DELETE_WARNING_JAVASCRIPT or null */
		/* --------------------------------------------------- */
		operation_delete_warning_javascript(
			OPERATION_DELETE_WARNING_JAVASCRIPT,
			operation->delete_boolean );

	operation->appaserver_element =
		operation_element(
			operation_name,
			operation->image_source,
			operation->delete_warning_javascript );

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

APPASERVER_ELEMENT *operation_element(
			char *operation_name,
			char *image_source,
			char *delete_warning_javascript )
{
	APPASERVER_ELEMENT *element;

	element = appaserver_element_new( checkbox, (char *)0 /* name */ );

	free( element->checkbox );

	element->checkbox =
		element_checkbox_new(
			(char *)0 /* attribute_name */,
			operation_name /* operation_name */,
			(char *)0 /* prompt_string */,
			delete_warning_javascript /* on_click */,
			-1 /* tab_order */,
			image_source,
			0 /* not recall */ );

	return element;
}

char *operation_html(	ELEMENT_CHECKBOX *checkbox,
			char *state,
			int row_number,
			char *background_color,
			boolean delete_mask_boolean )
{
	char prompt_display[ 128 ];
	char *element_name;

	if ( !checkbox )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: checkbox is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	*prompt_display = '\0';

	if ( checkbox->prompt_string )
	{
		string_initial_capital(
			prompt_display,
			checkbox->prompt_string );
	}
	else
	if ( checkbox->attribute_name  )
	{
		string_initial_capital(
			prompt_display,
			checkbox->attribute_name );
	}
	else
	if ( checkbox->element_name  )
	{
		string_initial_capital(
			prompt_display,
			checkbox->element_name );
	}
		
	if ( delete_mask_boolean ) return strdup( prompt_display );

	if ( checkbox->element_name )
	{
		element_name =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			appaserver_element_name(
				checkbox->element_name,
				row_number );
	}
	else
	if ( checkbox->attribute_name )
	{
		element_name =
			appaserver_element_name(
				checkbox->attribute_name,
				row_number );
	}
	else
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: both element_name and attribute_name are empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}


	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	element_checkbox_html(
		element_name,
		prompt_display,
		0 /* not element_checkbox_checked */,
		javascript_replace(
			checkbox->on_click,
			state,
			row_number ),
		checkbox->tab_order,
		background_color,
		checkbox->image_source );
}

OPERATION_ROW *operation_row_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			LIST *role_operation_list,
			LIST *primary_key_list,
			int dictionary_key_highest_index,
			DICTIONARY *operation_dictionary,
			int row_number,
			DICTIONARY *dictionary_single_row )
{
	ROLE_OPERATION *role_operation;
	OPERATION_ROW *operation_row;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !folder_name
	||   !list_length( primary_key_list )
	||   !dictionary_key_highest_index
	||   !row_number )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !dictionary_length( operation_dictionary )
	||   !dictionary_length( dictionary_single_row ) )
	{
		return (OPERATION_ROW *)0;
	}

	if ( !list_rewind( role_operation_list ) )
		return (OPERATION_ROW *)0;

	operation_row = operation_row_calloc();

	operation_row->checked_list = list_new();

	do {
		role_operation = list_get( role_operation_list );

		if ( !role_operation->operation
		||   !role_operation->operation->process
		||   !role_operation->operation->process->command_line )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: command_line is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		list_set(
			operation_row->checked_list,
			operation_row_checked_new(
				application_name,
				session_key,
				login_name,
				role_name,
				folder_name,
				primary_key_list,
				dictionary_key_highest_index,
				operation_dictionary,
				dictionary_single_row,
				role_operation->operation_name,
				role_operation->
					operation->
					process->
					command_line ) );
	} while ( list_next( role_operation_list ) );

	if ( !list_length( operation_row->checked_list ) )
	{
		list_free( operation_row->checked_list );
		free( operation_row );
		return (OPERATION_ROW *)0;
	}
	else
	{
		return operation_row;
	}
}

OPERATION_ROW_CHECKED *operation_row_checked_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			LIST *primary_key_list,
			int dictionary_key_highest_index,
			DICTIONARY *operation_dictionary,
			DICTIONARY *dictionary_single_row,
			char *operation_name,
			char *command_line )
{
	OPERATION_ROW_CHECKED *operation_row_checked =
		operation_row_checked_calloc();

	operation_row_checked->count =
		operation_row_checked_count(
			dictionary_key_highest_index,
			operation_dictionary,
			operation_name );

	if ( !operation_row_checked->count )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: operation_row_checked_count(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			operation_name );
		exit( 1 );
	}

	operation_row_checked->primary_key_data_list =
		dictionary_data_list(
			primary_key_list,
			dictionary_single_row );

	if ( !list_length( operation_row_checked->primary_key_data_list ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: dictionary_data_list(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			list_display( primary_key_list ) );
		exit( 1 );
	}

	operation_row_checked->command_line =
		operation_row_checked_command_line(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			dictionary_single_row,
			operation_name,
			command_line,
			operation_row_checked->count,
			getpid() /* parent_process_id */,
			operation_row_checked->primary_key_data_list );

	return operation_row_checked;
}

OPERATION_ROW_CHECKED *operation_row_checked_calloc( void )
{
	OPERATION_ROW_CHECKED *operation_row_checked;

	if ( ! ( operation_row_checked =
			calloc( 1, sizeof( OPERATION_ROW_CHECKED ) ) ) )
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

int operation_row_checked_count(
			int highest_index,
			DICTIONARY *operation_dictionary,
			char *operation_name )
{
	int checked_count = 0;
	int row;
	char key[ 128 ];

	if ( !highest_index ) return 0;

	for( row = 1; row <= highest_index; row++ )
	{
		sprintf(key,
			"%s_%d",
			operation_name,
			row );

		if ( dictionary_get( key, operation_dictionary ) )
			checked_count++;
	}
	return checked_count;
}
 
char *operation_row_checked_command_line(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			DICTIONARY *dictionary_single_row,
			char *operation_name,
			char *command_line,
			int operation_row_checked_count,
			pid_t parent_process_id,
			LIST *primary_key_data_list )
{
	/* Returns heap memory */
	/* ------------------- */
	return
	process_operation_command_line(
		application_name,
		session_key,
		login_name,
		role_name,
		folder_name,
		dictionary_single_row,
		operation_name,
		command_line,
		parent_process_id,
		operation_row_checked_count,
		primary_key_data_list );
}

char *operation_row_checked_execute( char *command_line )
{
	return string_pipe_fetch( command_line );
}

OPERATION_ROW_LIST *operation_row_list_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			LIST *role_operation_list,
			LIST *primary_key_list,
			LIST *folder_attribute_name_list,
			DICTIONARY *operation_dictionary,
			DICTIONARY *multi_row_dictionary )
{
	OPERATION_ROW_LIST *operation_row_list;
	int row_number;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !folder_name
	||   list_length( primary_key_list )
	||   !list_length( folder_attribute_name_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !dictionary_length( operation_dictionary )
	||   !dictionary_length( multi_row_dictionary ) )
	{
		return (OPERATION_ROW_LIST *)0;
	}

	operation_row_list = operation_row_list_calloc();

	operation_row_list->dictionary_key_highest_index =
		dictionary_key_highest_index(
			operation_dictionary );

	operation_row_list->list = list_new();

	for(	row_number = 1;
		row_number <= operation_row_list->dictionary_key_highest_index;
		row_number++ )
	{
		operation_row_list->dictionary_single_row =
			dictionary_single_row(
				folder_attribute_name_list,
				row_number,
				multi_row_dictionary );

		if ( !dictionary_length(
			operation_row_list->dictionary_single_row ) )
		{
			continue;
		}

		list_set(
			operation_row_list->list,
			operation_row_new(
				application_name,
				session_key,
				login_name,
				role_name,
				folder_name,
				role_operation_list,
				primary_key_list,
				operation_row_list->
					dictionary_key_highest_index,
				operation_dictionary,
				row_number,
				operation_row_list->dictionary_single_row ) );
	}

	if ( !list_length( operation_row_list->list ) )
	{
		free( operation_row_list );
		return (OPERATION_ROW_LIST *)0;
	}
	else
	{
		return operation_row_list;
	}
}

OPERATION_ROW_LIST *operation_row_list_calloc( void )
{
	OPERATION_ROW_LIST *operation_row_list;

	operation_row_list =
		calloc( 1, sizeof( OPERATION_ROW_LIST ) );

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

		if ( !list_rewind( operation_row->checked_list ) )
			continue;

		do {
			operation_row_checked =
				list_get(
					operation_row->checked_list );

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
				operation_row_checked_execute(
					operation_row_checked->
						command_line );

			if ( operation_error_message
			&&   *operation_error_message )
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

		} while ( list_next( operation_row->checked_list ) );

	} while ( list_next( operation_row_list->list ) );

	if ( ptr == operation_error_message_list_string )
		return (char *)0;
	else
		return strdup( operation_error_message_list_string );
}
