/* ----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/output_edit_table.c			*/
/* ----------------------------------------------------------------	*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include "String.h"
#include "timlib.h"
#include "list.h"
#include "environ.h"
#include "post_dictionary.h"
#include "dictionary_separate.h"
#include "session.h"
#include "edit_table.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *folder_name;
	char *target_frame;
	DICTIONARY_SEPARATE *dictionary_separate;
	EDIT_TABLE *edit_table;

	application_name = environment_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 7 )
	{
		fprintf( stderr, 
"Usage: %s session login_name role folder target_frame post_dictionary\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	session_key = argv[ 1 ];
	login_name = argv[ 2 ];
	role_name = argv[ 3 ];
	folder_name = argv[ 4 ];
	target_frame = argv[ 5 ];

	session_environment_set( application_name );

	if ( ! ( dictionary_separate =
			dictionary_separate_new(
				post_dictionary_string_new( argv[ 6 ],
				application_name,
				(LIST *)0 /* folder_attribute_isa_list */,
				(LIST *)0 /* operation_name_list */ ) ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: dictionary_separate() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	edit_table =
		edit_table_new(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			target_frame,
			frameset_menu_horizontal(
				application_name,
				login_name )
				/* menu_horizontal_boolean */,
			dictionary_separate->query_dictionary,
			dictionary_separate->ignore_dictionary,
			dictionary_separate->non_prefixed_dictionary,
			dictionary_separate->drillthru_dictionary,
			dictionary_separate->sort_dictionary,
			dictionary_separate->
				ignore_select_attribute_name_list );

	if ( !edit_table )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: edit_table_new(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			folder_name );
		exit( 1 );
	}

	edit_table_output( stdout, edit_table );

	return 0;
}
