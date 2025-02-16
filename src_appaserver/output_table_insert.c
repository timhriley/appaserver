/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/output_insert_table.c		*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include "appaserver_error.h"
#include "environ.h"
#include "post_dictionary.h"
#include "table_insert.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *folder_name;
	char *target_frame;
	char *results_string;
	char *error_string;
	POST_DICTIONARY *post_dictionary;
	TABLE_INSERT *table_insert;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 8 )
	{
		fprintf(stderr, 
	"Usage: %s session login role folder target_frame results error\n",
			argv[ 0 ] );
		exit ( 1 );
	}

	session_key = argv[ 1 ];
	login_name = argv[ 2 ];
	role_name = argv[ 3 ];
	folder_name = argv[ 4 ];
	target_frame = argv[ 5 ];
	results_string = argv[ 6 ];
	error_string = argv[ 7 ];

	post_dictionary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_dictionary_stdin_new(
			/* ---------------------------------- */
			/* Used when expecting a spooled file */
			/* ---------------------------------- */
			(char *)0 /* application_name */,
			(char *)0 /* upload_directory */,
			(LIST *)0 /* upload_filename_list */ );

	table_insert =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		table_insert_new(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			target_frame,
			results_string,
			error_string,
			post_dictionary->original_post_dictionary );

	if ( table_insert->
		vertical_new_table )
	{
		vertical_new_table_blank_prompt_frame(
			table_insert->
				vertical_new_table->
				vertical_new_filename->
				output_filename,
			table_insert->
				vertical_new_table->
				document );
	}

	if ( table_insert->entire_primary_key_ignored )
	{
		document_process_output(
			application_name,
			(LIST *)0 /* javascript_filename_list */,
			table_insert->
				table_insert_input->
				title_string );

		printf(
			"<h3>%s</h3>\n",
			TABLE_INSERT_PRIMARY_IGNORED_MESSAGE );

		document_close();
	}
	else
	{
		printf( "%s\n", table_insert->document_form_html );
	}

	return 0;
}

