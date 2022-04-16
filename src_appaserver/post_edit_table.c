/* ------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/post_edit_table.c		*/
/* ------------------------------------------------------	*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "post_edit_table.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *folder_name;
	char *target_frame;
	char *detail_base_folder_name;
	POST_EDIT_TABLE *post_edit_table;
	char *sql_error_message = {0};
	char *operation_error_message = {0};
	char *subsub_title;
	char *system_string;

	if ( argc != 8 )
	{
		fprintf( stderr,
"Usage: %s application session login_name role folder target_frame detail_base\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	session_key = argv[ 2 ];
	login_name = argv[ 3 ];
	role_name = argv[ 4 ];
	folder_name = argv[ 5 ];
	target_frame = argv[ 6 ];
	detail_base_folder_name = argv[ 7 ];

	if ( ! ( post_edit_table =
			post_edit_table_new(
				argc,
				argv,
				application_name,
				session_key,
				login_name,
				role_name,
				folder_name,
				target_frame,
				detail_base_folder_name ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: post_edit_table_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( post_edit_table->update )
	{
		sql_error_message =
			update_row_list_execute(
				post_edit_table->update->update_row_list,
				appaserver_error_filename(
					application_name ) );
	}

	if ( list_length( post_edit_table->operation_row_list ) )
	{
		operation_error_message =
			operation_row_list_execute(
				post_edit_table->operation_row_list );
	}

	subsub_title =
		string_append_string(
			sql_error_message,
			operation_error_message,
			“<br>” /* string_delimiter */ );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		edit_table_output_system_string(
			EDIT_TABLE_OUTPUT_EXECUTABLE,
			session_key,
			login_name,
			role_name,
			folder_name,
			target_frame,
			subsub_title,
			dictionary_separate_send_dictionary(),
			appaserver_error_filename(
				application_name ) );

	if ( system( system_string ) ){}

	exit( 0 );
}

