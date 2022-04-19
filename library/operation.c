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
#include "operation.h"

void operation_row_execute( char *command_line )
{
	if ( system( command_line ) ){};
}

int operation_row_total(
			DICTIONARY *dictionary,
			char *operation_name,
			int highest_index )
{
	int row_total = 0;
	int row;
	char *data;

	if ( !highest_index ) return 0;

	for( row = 1; row <= highest_index; row++ )
	{
		/* ---------------------------- */
		/* Sample dictionary entry:	*/
		/* delete_1=yes			*/
		/* ---------------------------- */
		if ( dictionary_index_data(
			&data,
			dictionary,
			operation_name,
			row ) == 1 )
		{
			if ( strcmp( data, "yes" ) == 0 ) row_total++;
		}
	}
	return row_total;
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

OPERATION_ROW *operation_row_fetch(
			DICTIONARY *row_dictionary,
			char *operation_name,
			LIST *primary_key_list,
			LIST *attribute_name_list,
			int row_number )
{
	boolean checked;
	OPERATION_ROW *operation_row;

	if ( ! ( checked =
			operation_row_checked(
				row_dictionary,
				operation_name,
				row_number ) ) )
	{
		return (OPERATION_ROW *)0;
	}

	operation_row = operation_row_calloc();

	operation_row->primary_data_list =
		operation_row_primary_data_list(
			row_dictionary,
			primary_key_list,
			row_number );

	if ( !list_length( operation_row->primary_data_list ) )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: operation_row_primary_data_list(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			list_display( primary_key_list ) );
		exit( 1 );
	}

	operation_row->single_dictionary =
		operation_row_single_dictionary(
			row_dictionary,
			attribute_name_list,
			row_number );

	return operation_row;
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
			DICTIONARY *row_dictionary,
			char *operation_name,
			int row_number )
{
	char *data = {0};

	if ( dictionary_index_data(
			&data,
			row_dictionary,
			operation_name,
			row_number ) == 1 )
	{
		if ( strcmp( data, "yes" ) == 0 ) return 1;
	}
	return 0;
}

LIST *operation_row_primary_data_list(
			DICTIONARY *row_dictionary,
			LIST *primary_key_list,
			int row_number )
{
	return
	dictionary_key_list_index_data_list( 
		row_dictionary,
		primary_key_list,
		row_number );
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

char *operation_row_command_line(
			char *command_line,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *operation_name,
			pid_t parent_process_id,
			int operation_row_total,
			LIST *operation_row_primary_data_list,
			DICTIONARY *dictionary_single_row )
{
	/* Returns heap memory */
	/* ------------------- */
	return
	process_operation_command_line(
		command_line,
		application_name,
		operation_name,
		login_name,
		role_name,
		folder_name,
		pid_t parent_process_id,
		session_key,
		int operation_row_total,
		primary_data_list,
		dictionary_single_row );
}


LIST *operation_row_list(
			DICTIONARY *multi_row_dictionary,
			char *operation_name,
			LIST *primary_key_list,
			LIST *attribute_name_list,
			int operation_row_total )
{
}

LIST *operation_row_key_list(
			char *operation_name,
			LIST *attribute_name_list )
{
}

OPERATION_ROW *operation_row_new(
			DICTIONARY *dictionary_single_row,
			char *operation_name,
			LIST *primary_key_list,
			int operation_row_total )
{
	operation_row->command_line =
		operation_row_command_line(
			char *command_line,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *operation_name,
			pid_t parent_process_id,
			int operation_row_total,
			LIST *operation_row_primary_data_list,
			DICTIONARY *dictionary_single_row );
}

OPERATION_ROW *operation_row_calloc( void )
{
}

LIST *operation_row_primary_data_list(
			DICTIONARY *dictionary_single_row,
			LIST *primary_key_list )
{
}

DICTIONARY *operation_row_single_dictionary(
			DICTIONARY *row_dictionary,
			LIST *attribute_name_list,
			int row_number )
{
}

boolean operation_row_checked(
			DICTIONARY *single_row_dictionary,
			char *operation_name )
{
}


