/* $APPASERVER_HOME/src_appaserver/post_change_sort_order.c		*/
/* -------------------------------------------------------------	*/
/* This process is triggered if you select the sort order radio		*/
/* button on the lookup forms. The folder must have one of		*/
/* these attribute names:						*/
/* sort_order, display_order, or sequence_number.			*/
/*									*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_error.h"
#include "post_dictionary.h"
#include "environ.h"
#include "session.h"
#include "appaserver.h"
#include "document.h"
#include "appaserver_error.h"
#include "lookup_sort.h"

void state_one(	char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		DICTIONARY *original_post_dictionary );

void state_two(	char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		DICTIONARY *original_post_dictionary );

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *folder_name;
	char *sort_state;
	SESSION_FOLDER *session_folder;
	POST_DICTIONARY *post_dictionary;

	if ( argc != 7 )
	{
		fprintf(stderr, 
		"Usage: %s application session login role folder sort_state\n",
			argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	session_key = argv[ 2 ];
	login_name = argv[ 3 ];
	role_name = argv[ 4 ];
	folder_name = argv[ 5 ];
	sort_state = argv[ 6 ];

	session_folder =
		/* --------------------------------------------- */
		/* Sets appaserver environment and outputs argv. */
		/* Each parameter is security inspected.	 */
		/* Any error will exit(1).			 */
		/* --------------------------------------------- */
		session_folder_integrity_exit(
			argc,
			argv,
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			APPASERVER_UPDATE_STATE );

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

	if ( strcmp( sort_state, "one" ) == 0 )
	{
		state_one(
			session_folder->application_name,
			session_folder->session->session_key,
			session_folder->login_name,
			session_folder->role_name,
			session_folder->folder_name,
			post_dictionary->original_post_dictionary );
	}
	else
	if ( strcmp( sort_state, "two" ) == 0 )
	{
		document_content_type_output();

		state_two(
			session_folder->application_name,
			session_folder->login_name,
			session_folder->role_name,
			session_folder->folder_name,
			post_dictionary->original_post_dictionary );
	}

	return 0;
}

void state_one(	char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		DICTIONARY *original_post_dictionary )
{
	LOOKUP_SORT_FORM *lookup_sort_form;

	lookup_sort_form =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		lookup_sort_form_new(
			LOOKUP_SORT_EXECUTABLE,
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			original_post_dictionary );

	document_process_output(
		application_name,
		lookup_sort_form->javascript_filename_list,
		"sort" /* process_name */ );

	if ( !lookup_sort_form->lookup_sort->row_count )
	{
		printf( "<h3>%s</h3>\n",
			LOOKUP_SORT_FORM_NO_ROWS_MESSAGE );
	}
	else
	{
		printf(	"%s\n",
			lookup_sort_form->
				form_lookup_sort->
				form_html );
	}

	document_close();
}

void state_two(	char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		DICTIONARY *original_post_dictionary )
{
	LOOKUP_SORT_EXECUTE *lookup_sort_execute;

	lookup_sort_execute =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		lookup_sort_execute_new(
			application_name,
			login_name,
			role_name,
			folder_name,
			original_post_dictionary,
			appaserver_error_filename(
				application_name ) );

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		"sort" /* process_name */ );

	lookup_sort_execute_update(
		lookup_sort_execute->
			update_statement_list,
		lookup_sort_execute->
			system_string );

	printf(	"<h3>%s</h3>\n",
		LOOKUP_SORT_EXECUTE_COMPLETE_MESSAGE );

	document_close();
}

