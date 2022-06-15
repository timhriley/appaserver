/* ------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/post_table_edit.c		*/
/* ------------------------------------------------------	*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_error.h"
#include "String.h"
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
	char *detail_base_folder_name;
	POST_TABLE_EDIT *post_table_edit;
	char *sql_error_message_list_string = {0};
	char *operation_error_message_list_string = {0};
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

	if ( ! ( post_table_edit =
			post_table_edit_new(
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
		"ERROR in %s/%s()/%d: post_table_edit_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( post_table_edit->update )
	{
		sql_error_message_list_string =
			/* --------------------------------------------- */
			/* Returns sql_error_message_list_string or null */
			/* --------------------------------------------- */
			update_row_list_execute(
				post_table_edit->update->update_row_list,
				appaserver_error_filename(
					application_name ) );
	}

	if ( post_table_edit->operation_row_list )
	{
		operation_error_message_list_string =
			/* ---------------------------------------------------*/
			/* Returns operation_error_message_list_string or null*/
			/* ---------------------------------------------------*/
			operation_row_list_execute(
				post_table_edit->operation_row_list );
	}

	subsub_title =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		string_append(
			sql_error_message_list_string,
			operation_error_message_list_string,
			"<br>" /* string_delimiter */ );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		table_edit_output_system_string(
			TABLE_EDIT_OUTPUT_EXECUTABLE,
			session_key,
			login_name,
			role_name,
			folder_name,
			target_frame,
			subsub_title,
			dictionary_separate_send_string(
				dictionary_separate_send_dictionary(
					post_table_edit->
						dictionary_separate->
						sort_dictionary,
					DICTIONARY_SEPARATE_SORT_PREFIX,
					post_table_edit->
						dictionary_separate->
						query_dictionary,
					DICTIONARY_SEPARATE_QUERY_PREFIX,
					post_table_edit->
						dictionary_separate->
						drillthru_dictionary,
					DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
					(DICTIONARY *)0 /* ignore_dictionary */,
					DICTIONARY_SEPARATE_IGNORE_PREFIX,
					post_table_edit->
						dictionary_separate->
						no_display_dictionary,
					DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
					post_table_edit->
						dictionary_separate->
						pair_one2m_dictionary,
					DICTIONARY_SEPARATE_PAIR_PREFIX,
					post_table_edit->
						dictionary_separate->
						non_prefixed_dictionary ) ),
			appaserver_error_filename( application_name ) );

	if ( system( system_string ) ){}

	return 0;
}

