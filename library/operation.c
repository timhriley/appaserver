/* $APPASERVER_HOME/library/operation.c					*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdlib.h>
#include <string.h>
#include "appaserver_error.h"
#include "timlib.h"
#include "String.h"
#include "operation.h"

OPERATION *operation_new(
			char *operation_name,
			boolean role_update_view_only )
{
	char system_string[ 1024 ];
	char where[ 128 ];
	char *results;
	OPERATION *operation;

	sprintf(where,
		"operation = '%s'",
		operation_name );

	sprintf(system_string,
		"select.sh output_yn %s \"%s\"",
		OPERATION_TABLE,
		where );

	if ( ! ( results = string_pipe_fetch( system_string ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: string_pipe_fetch(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			system_string );
		exit( 1 );
	}

	operation = operation_calloc();
	operation->operation_name = operation_name;
	operation->output = (*results == 'y');

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

	operation->delete_name =
		operation_delete_name(
			operation_name );

	operation->delete_placeholder =
		operation_delete_placeholder(
			role_update_view_only,
			operation->delete_name );

	operation->label =
		operation_label(
			operation_name,
			operation->delete_placeholder );

	return operation;
}

boolean operation_delete_placeholder(
			boolean role_update_view_only,
			boolean delete_name )
{
	return ( role_update_view_only && delete_name );
}

char *operation_label(	char *operation_name,
			boolean delete_placeholder )
{
	if ( !operation_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: operation_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( delete_placeholder )
	{
		char label[ 128 ];

		format_initial_capital(
			label,
			operation_name );

		return strdup( label );
	}
	else
	{
		return (char *)0;
	}
}

void operation_perform( char *command_line )
{
	if ( system( command_line ) ){};
}

#ifdef NOT_DEFINED
/* Returns 1 if performed any output */
/* --------------------------------- */
boolean operation_perform(
			DICTIONARY *send_dictionary,
			DICTIONARY *row_dictionary,
			char *application_name,
			char *session,
			char *login_name,
			char *folder_name,
			char *role_name,
			LIST *primary_key_list,
			char *process_name,
			char *executable,
			char output_yn,
			boolean non_owner_forbid_deletion,
			char *target_frame,
			int operation_row_total,
			char *state )
{
	int row, highest_index;
	LIST *primary_data_list;
	char *data;
	char *dictionary_login_name;
	char *local_executable;
	DICTIONARY *row_dictionary_row;
	DICTIONARY *local_send_dictionary;
	char key[ 128 ];
	int operation_row_iteration = 0;
	char operation_row_iteration_string[ 16 ];
	FOLDER *folder;

	/* Reentrant */
	/* --------- */
	static boolean performed_any_output = 0;
	static boolean first_time = 1;

	folder = folder_calloc();

	folder->mto1_isa_related_folder_list =
		folder_mto1_isa_related_folder_list(
			list_new() /* recursive */,
			application_name,
			folder_name,
			role_name,
			0 /* recursive_level */ );

	folder->append_isa_attribute_list =
		folder_append_isa_attribute_list(
			application_name,
			folder_name,
			folder->mto1_isa_related_folder_list,
			role_name );

	folder->append_isa_attribute_name_list =
		folder_append_isa_attribute_name_list(
			folder->append_isa_attribute_list );

	highest_index = get_dictionary_key_highest_index( row_dictionary );

	for( row = 1; row <= highest_index; row++ )
	{
		/* -------------------------------------------------- */
		/* Sample dictionary entry			      */
		/* delete_1					      */
		/* -------------------------------------------------- */
		if ( dictionary_get_index_data(
					&data,
					row_dictionary,
					process_name,
					row ) != -1 )
		{
			if ( strcmp( data, "yes" ) != 0 ) continue;

			local_send_dictionary =
				dictionary_copy( send_dictionary );

			if ( strcmp( target_frame, PROMPT_FRAME ) != 0 )
			{
				dictionary_set_pointer(
					local_send_dictionary,
					CONTENT_TYPE_YN,
					"y" );
			}

			if ( dictionary_get_index_data(
						&dictionary_login_name,
						row_dictionary,
						"login_name",
						row ) < 0 )
			{
				dictionary_login_name = (char*)0;
			}

			if ( non_owner_forbid_deletion
			&&   strcmp( process_name, "delete" ) == 0
			&&   dictionary_login_name
			&&   timlib_strcmp(
					dictionary_login_name,
					login_name ) != 0 )
			{
				char msg[ 1024 ];

				sprintf( msg,
"\n\nWARNING in %s/%s()/%d: %s tried to delete a record from %s which was forbidden.\n\n",
					__FILE__,
					__FUNCTION__,
					__LINE__,
					login_name,
					folder_name );

				appaserver_output_error_message(
					application_name, msg, login_name );

				continue;
			}

			primary_data_list = 
				dictionary_using_list_get_index_data_list( 
					row_dictionary,
					primary_key_list,
					row );

			sprintf( key, "really_yn_%d", row );
			dictionary_add_string(	local_send_dictionary,
						strdup( key ),
						"y" );

			sprintf( key, "execute_yn_%d", row );
			dictionary_add_string(	local_send_dictionary,
						strdup( key ),
						"y" );

			if ( performed_any_output )
			{
				dictionary_add_string(
						local_send_dictionary,
						CONTENT_TYPE_YN,
						"n" );
			}

			if ( first_time && output_yn == 'y' )
			{
				if ( appaserver_frameset_menu_horizontal(
					application_name,
					login_name )
				&&   strcmp( target_frame, PROMPT_FRAME ) == 0 )
				{
					char sys_string[ 1024 ];

					sprintf(sys_string,
		"output_choose_role_folder_process_form %s %s %s %s '%s' %s",
					application_name,
					session,
					login_name,
					role_name,
					"" /* title */,
					"y" /* content_type_yn */ );

					if ( system( sys_string ) ){};
					fflush( stdout );

					dictionary_add_string(
						row_dictionary,
						CONTENT_TYPE_YN,
						"n" );
				}

				first_time = 0;
			}

			local_executable = strdup( executable );

			row_dictionary_row =
				dictionary_separate_row_dictionary(
					row_dictionary,
					folder->
					append_isa_attribute_name_list,
					row );

			dictionary_append_dictionary(
				local_send_dictionary,
				row_dictionary_row );

			sprintf( operation_row_iteration_string,
				 "%d",
				 ++operation_row_iteration );

			dictionary_set_pointer(
				local_send_dictionary,
				OPERATION_ROW_ITERATION_LABEL,
				strdup( operation_row_iteration_string ) );

			process_operation_convert(
				&local_executable,
				application_name,
				session,
				state,
				login_name,
				folder_name,
				role_name,
				target_frame,
				local_send_dictionary
					/* parameter_dictionary */,
				local_send_dictionary
					/* where_clause_dictionary */,
				folder->append_isa_attribute_list,
				primary_key_list,
				primary_data_list,
				row,
				process_name,
				operation_row_count_string );

			fflush( stdout );

			if ( system( local_executable ) ){};
			fflush( stdout );
			free( local_executable );

			if ( output_yn == 'y' ) performed_any_output = 1;

			/* dictionary_free( local_send_dictionary ); */

		} /* if checked a process */
	} /* for each row */

	return performed_any_output;
}
#endif

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

	operation_row->dictionary =
		operation_single_row_dictionary(
			row_dictionary,
			attribute_name_list,
			row_number );

	return operation_row;
}

OPERATION_ROW *operation_row_calloc( void )
{
	OPERATION_ROW *operation_row;

	if ( ! ( operation_row =
			calloc( 1, sizeof( OPERATION_ROW ) ) ) )
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

DICTIONARY *operation_single_row_dictionary(
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

boolean operation_delete_name( char *operation_name )
{
	return ( string_strncmp( operation_name, "delete" ) == 0 );
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
