/* $APPASERVER_HOME/library/operation.c					*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "String.h"
#include "appaserver_error.h"
#include "appaserver_library.h"
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

OPERATION_SEMAPHORE *operation_semaphore_fetch(
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
		 	IMAGE_RELATIVE_DIRECTORY );
	}
	else
	if ( detail_boolean )
	{
		sprintf(image_source,
			"/%s/magnify_glass.gif",
			IMAGE_RELATIVE_DIRECTORY );
	}

	if ( *image_source )
		return strdup( image_source );
	else
		return (char *)0;
}

char *operation_delete_warning_javascript( boolean delete_boolean )
{
	if ( delete_boolean )
	{
		return "timlib_delete_button_warning()";
	}
	else
	{
		return (char *)0;
	}
}

OPERATION_LIST *operation_list_calloc( void )
{
	OPERATION_LIST *operation_list;

	if ( ! ( operation_list = calloc( 1, sizeof( OPERATION_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return operation_list;
}

OPERATION_LIST *operation_list_new(
			char *folder_name,
			char *role_name )
{
	OPERATION_LIST *operation_list = operation_list_calloc();
	FILE *input_pipe;
	char input[ 128 ];

	operation_list->select =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		operation_list_select();

	operation_list->where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		operation_list_where(
			folder_name,
			role_name );

	operation_list->system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		operation_list_system_string(
			operation_list->select,
			operation_list->where );

	input_pipe = popen( operation_list->system_string, "r" );
	operation_list->operation_list = list_new();

	while ( string_input( input, input_pipe, 128 ) )
	{
		list_set(
			operation_list->operation_list,
			operation_parse( input ) );
	}

	pclose( input_pipe );
	return operation_list;
}

char *operation_list_select( void )
{
	char select[ 128 ];

	sprintf(select,
		"%s.operation,output_yn",
		OPERATION_TABLE );

	return select;
}

char *operation_list_where(
			char *folder_name,
			char *role_name )
{
	static char where[ 256 ];

	sprintf(where,
		"folder = '%s' and role = '%s' and"
		"%s.operation = %s.operation",
		folder_name,
		role_name,
		ROLE_OPERATION_TABLE,
		OPERATION_TABLE );

	return where;
}

char *operation_list_system_string(
			char *select,
			char *where )
{
	static char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh \"%s\" %s,%s \"%s\"",
		select,
		ROLE_OPERATION_TABLE,
		OPERATION_TABLE,
		where );

	return system_string;
}

OPERATION *operation_parse( char *input )
{
	OPERATION *operation = operation_calloc();
	char operation_name[ 128 ];
	char output_yn[ 128 ];

	piece( operation_name, SQL_DELIMITER, input, 0 );
	piece( output_yn, SQL_DELIMITER, input, 1 );

	operation->operation_name = strdup( operation_name );
	operation->output = (*output_yn == 'y');

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
		operation_delete_warning_javascript(
			operation->delete_boolean );

	operation->operation_element =
		operation_element(
			operation_name,
			operation->imagage_source,
			operation->delete_warning_javascript );

	return operation;
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
			operation_name /* attribute_name */,
			operation_name /* prompt_string */,
			0 /* not checked */,
}

