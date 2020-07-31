/* library/operation.c 							*/
/* -------------------------------------------------------------------- */
/* This is the appaserver operation ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdlib.h>
#include <string.h>
#include "operation.h"
#include "appaserver_error.h"
#include "folder.h"
#include "timlib.h"
#include "appaserver.h"
#include "dictionary_appaserver.h"

/* appaserver_link_file */

OPERATION *operation_new_operation(
				char *application_name,
				char *session,
				char *operation_name,
				boolean empty_placeholder_instead )
{
	OPERATION *operation;
	char sys_string[ 1024 ];
	char *results;
	char *table_name;

	operation = (OPERATION *)calloc( 1, sizeof( OPERATION ) );

	if ( !operation )
	{
		fprintf( stderr,
			 "Memory allocation error in %s/%s()/%d\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	table_name = get_table_name( application_name, "operation" );
	sprintf(sys_string,
		"echo \"select output_yn			 "
		"	 from %s				 "
		"	 where operation = '%s';\"		|"
		"sql						 ",
		table_name,
		operation_name );

	results = pipe2string( sys_string );
	if ( !results )
	{
		char msg[ 1024 ];
		sprintf(msg,
			"ERROR in %s.%s: cannot find operation = %s",
			__FILE__,
			__FUNCTION__,
			operation_name );
		appaserver_output_error_message(
			application_name, msg, (char *)0 );
		exit( 1 );
	}

	operation->output_yn = *results;
	operation->empty_placeholder_instead = empty_placeholder_instead;

	if ( ! (operation->process =
			process_new_process(
				application_name,
				session,
				operation_name,
				(DICTIONARY *)0,
				(char *)0 /* role_name */,
				0 /* not with_check_executable_ok */ ) ) )
	{
		char msg[ 1024 ];
		sprintf( msg,
		"Error in %s/%s() cannot get process for operation = (%s)",
			__FILE__,
			__FUNCTION__,
			operation_name );
		appaserver_output_error_message(
			application_name, msg, (char *)0 );
		exit( 1 );
	}
	return operation;
} /* operation_new_operation() */

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
			LIST *primary_attribute_name_list,
			char *process_name,
			char *executable,
			char output_yn,
			boolean non_owner_forbid_deletion,
			char *target_frame,
			char *operation_row_count_string,
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
						primary_attribute_name_list,
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
				dictionary_appaserver_get_row_dictionary_row(
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
				primary_attribute_name_list,
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

} /* operation_perform() */

char *operation_get_operation_row_count_string(
			DICTIONARY *dictionary,
			char *operation_name,
			int highest_index )
{
	static char row_count_string[ 16 ];
	int operation_row_count = 0;
	int row;
	char *data;

	if ( !highest_index )
	{
		highest_index = get_dictionary_key_highest_index( dictionary );
	}

	if ( !highest_index ) return 0;

	for( row = 1; row <= highest_index; row++ )
	{
		/* -------------------------------------------------- */
		/* Sample dictionary entry			      */
		/* delete_1					      */
		/* -------------------------------------------------- */
		if ( dictionary_get_index_data(
					&data,
					dictionary,
					operation_name,
					row ) != -1 )
		{
			if ( strcmp( data, "yes" ) == 0 )
				operation_row_count++;

		}
	} /* for each row */
	sprintf( row_count_string, "%d", operation_row_count );
	return row_count_string;
} /* operation_get_operation_row_count_string() */

OPERATION_SEMAPHORE *operation_semaphore_new(
			char *application_name,
			char *process_name,
			char *parent_process_id_string,
			char *appaserver_data_directory,
			char *operation_row_count_string )
{
	OPERATION_SEMAPHORE *operation_semaphore;
	char increment_string[ 16];
	char sys_string[ 1024 ];
	char *results;

	operation_semaphore =
		(OPERATION_SEMAPHORE *)
			calloc( 1, sizeof( OPERATION_SEMAPHORE ) );

	if ( !operation_semaphore )
	{
		fprintf( stderr,
			 "Memory allocation error in %s/%s()/%d\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	sprintf(	operation_semaphore->semaphore_filename,
			OPERATION_SEMAPHORE_TEMPLATE,
			appaserver_data_directory,
			application_name,
			process_name,
			parent_process_id_string );

	if ( !timlib_file_exists( operation_semaphore->semaphore_filename ) )
	{
		sprintf(	sys_string,
				"echo 0 > %s",
				operation_semaphore->semaphore_filename );
		fflush( stdout );
		if ( system( sys_string ) ){};
		fflush( stdout );
		operation_semaphore->group_first_time = 1;
	}

	sprintf( sys_string,
		 "cat %s",
		 operation_semaphore->semaphore_filename );
	results = pipe2string( sys_string );
	sprintf( increment_string, "%d", atoi( results ) + 1 );
	sprintf( sys_string,
		 "echo %s > %s",
		 increment_string,
		 operation_semaphore->semaphore_filename );
	fflush( stdout );
	if ( system( sys_string ) ){};
	fflush( stdout );

	operation_semaphore->group_last_time =
		( strcmp(	operation_row_count_string,
				increment_string ) == 0 );

	return operation_semaphore;

} /* operation_semaphore_new() */

void operation_semaphore_remove_file( char *semaphore_filename )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "rm -f %s",
		 semaphore_filename );

}

