/* ----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/output_menu.c			*/
/* ----------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ----------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "application.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "session.h"
#include "frameset.h"
#include "document.h"
#include "folder_menu.h"
#include "menu.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	boolean application_menu_horizontal_boolean;
	boolean content_type_boolean;
	DOCUMENT *document;
	FOLDER_MENU *folder_menu;
	MENU *menu;

	if ( argc != 7 )
	{
		fprintf(stderr,
"Usage: %s application session login_name role application_menu_horizontal_yn content_type_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];

	session_environment_set( application_name );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	session_key = argv[ 2 ];
	login_name = argv[ 3 ];
	role_name = argv[ 4 ];
	application_menu_horizontal_boolean = (*argv[ 5 ] == 'y');
	content_type_boolean = (*argv[ 6 ] == 'y');

	if ( content_type_boolean )
	{
		document_content_type_output();
	}

	session_access_or_exit(
		application_name,
		session_key,
		login_name );

	folder_menu =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_menu_fetch(
			application_name,
			session_key,
			role_name,
			appaserver_parameter_data_directory(),
			1 /* folder_menu_remove_boolean */ );

	menu =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		menu_new(
			application_name,
			session_key,
			login_name,
			role_name,
			application_menu_horizontal_boolean,
			folder_menu->count_list );

	if ( !menu->html )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: menu->html is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	document =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		document_new(
			application_name,
			application_title_string(
				application_name ),
			(char *)0 /* title_html */,
			(char *)0 /* sub_title_html */,
			(char *)0 /* sub_sub_title_html */,
			(char *)0 /* notepad */,
			(char *)0 /* javascript_replace */,
			menu->html,
			/* ------------------------------ */
			/* Returns program memory or null */
			/* ------------------------------ */
			document_head_menu_setup_string( 1 /* menu_boolean */ ),
			(char *)0 /* document_head_calendar_setup_string */,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			javascript_include_string(
				application_name,
				(LIST *)0 /* javascript_filename_list */ ) );

	printf( "%s\n%s\n%s\n%s\n%s\n%s\n",
		document->html,
		document->document_head->html,
		document_head_close_tag(),
		document->document_body->html,
		document_body_close_tag(),
		document_close_tag() );

	return 0;
}
