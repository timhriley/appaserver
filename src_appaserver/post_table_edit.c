/* -------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/post_table_edit.c			*/
/* -------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "appaserver_error.h"
#include "sql.h"
#include "execute_system_string.h"
#include "dictionary_separate.h"
#include "attribute.h"
#include "security.h"
#include "table_edit.h"
#include "post_table_edit.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *folder_name;
	char *target_frame;
	pid_t process_id;
	char *drilldown_base_folder_name;
	char *drilldown_primary_data_list_string;
	POST_TABLE_EDIT *post_table_edit;
	char *update_results_string = {0};
	char *update_error_string = {0};
	char *operation_row_list_output_string = {0};
	char *system_string = {0};

	if ( argc != 10 )
	{
		fprintf( stderr,
"Usage: %s application session login_name role folder target_frame process_id drilldown_base drilldown_primary\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	session_key = argv[ 2 ];
	login_name = argv[ 3 ];
	role_name = argv[ 4 ];
	folder_name = argv[ 5 ];
	target_frame = argv[ 6 ];
	process_id = (pid_t)atoi( argv[ 7 ] );
	drilldown_base_folder_name = argv[ 8 ];
	drilldown_primary_data_list_string = argv[ 9 ];

	document_content_type_output();

	post_table_edit =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_table_edit_new(
			argc,
			argv,
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			target_frame,
			process_id );

	if ( !post_table_edit->post_table_edit_input )
	{
		document_process_output(
			application_name,
			(LIST *)0 /* javascript_filename_list */,
			"Expired Form" /* title_string */ );

		printf( "%s\n",
			POST_TABLE_EDIT_FORM_EXPIRED );

		document_close();
		exit( 0 );
	}

	if ( post_table_edit->update )
	{
		if ( !post_table_edit->update->update_row_list
		||   !list_length(
			post_table_edit->
				update->
				update_row_list->
				list ) )
		{
			char message[ 128 ];

			sprintf(message,
	"post_table_edit->update->update_row_list is empty or incomplete." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

#ifdef NOT_DEFINED

		update_error_string =
		post_table_edit->update->error_string =
			/* ------------------------------------------- */
			/* Returns error_row_list_error_string or null */
			/* ------------------------------------------- */
			update_row_list_execute(
				SQL_EXECUTABLE,
				application_name,
				post_table_edit->update->update_row_list,
				post_table_edit->
					post_table_edit_input->
					appaserver_error_filename );

		if ( !post_table_edit->update->error_string )
		{
			/* Execute any post_change_process */
			/* ------------------------------- */
			update_row_list_command_line_execute(
				post_table_edit->
					update->
					update_row_list );
		}

		update_results_string =
			post_table_edit->
				update->
				results_string;
#endif

/* #ifdef NOT_DEFINED */
		update_row_list_display(
			post_table_edit->
				update->
				update_row_list );

		update_error_string = "test only";
/* #endif */
	}

	if ( !update_error_string
	&&   post_table_edit->operation_row_list )
	{
		operation_row_list_output_string =
			/* -------------------------------------------------*/
			/* Returns operation_row_list_output_string or null */
			/* -------------------------------------------------*/
			operation_row_list_execute(
				post_table_edit->operation_row_list );

		if ( post_table_edit->
			operation_row_list->
			output_boolean )
		{
			exit( 0 );
		}
	}

	if ( !operation_row_list_output_string )
	{
		if ( *drilldown_base_folder_name )
		{
			char drilldown_primary_data_list_unescape[ 2048 ];

			system_string =
			   execute_system_string_drilldown(
				POST_TABLE_EDIT_DRILLDOWN_EXECUTABLE,
				session_key,
				login_name,
				role_name,
				target_frame,
				drilldown_base_folder_name,
				/* ------------------- */
				/* Returns destination */
				/* ------------------- */
				string_unescape_character(
					drilldown_primary_data_list_unescape
						/* destination */,
					drilldown_primary_data_list_string
						/* datum */,
					ATTRIBUTE_MULTI_KEY_DELIMITER ),
				update_results_string,
				update_error_string,
				post_table_edit->
					dictionary_separate_send_string,
				post_table_edit->
					post_table_edit_input->
					appaserver_error_filename );
		}
		else
		{
			system_string =
				execute_system_string_table_edit(
					TABLE_EDIT_EXECUTABLE,
					session_key,
					login_name,
					role_name,
					folder_name,
					target_frame,
					update_results_string,
					update_error_string,
					post_table_edit->
						dictionary_separate_send_string,
					post_table_edit->
						post_table_edit_input->
						appaserver_error_filename );
		}
	}

	if ( operation_row_list_output_string )
	{
		printf( "%s\n", operation_row_list_output_string );
	}
	else
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

		security_system(
			SECURITY_FORK_CHARACTER,
			SECURITY_FORK_STRING,
			system_string );
	}

	return 0;
}
