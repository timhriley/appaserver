/* ------------------------------------------------------------------------*/
/* $APPASERVER_HOME/src_appaserver/output_choose_role_folder_process_form.c*/
/* ------------------------------------------------------------------------*/
/* Freely available software: see Appaserver.org 			   */
/* ------------------------------------------------------------------------*/

/* -------------------------------------------------------------------- */
/* Note: For horizontal menus, the stylesheets must be:			*/
/*       $DOCUMENT_ROOT/zmenu/src/style-template.css and		*/
/*       $DOCUMENT_ROOT/zmenu/src/skin-template.css			*/
/*			AND						*/
/*       $APPASERVER_HOME/$application/style-template.css and		*/
/*       $APPASERVER_HOME/$application/skin-template.css		*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "appaserver_parameter_file.h"
#include "application.h"
#include "appaserver_user.h"
#include "appaserver.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "environ.h"
#include "document.h"
#include "role.h"
#include "folder_menu.h"

int main( int argc, char **argv )
{
	char *application_name, *session, *login_name;
	char *role_name;
	char *title;
	char sys_string[ 1024 ];
	LIST *role_list;
	char login_name_buffer[ 128 ];
	char *content_type_yn;
	boolean omit_html_head = 0;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc < 7 )
	{
		fprintf( stderr, 
"Usage: %s ignored session login_name role title content_type_yn [omit_html_head_yn]\n", 
		argv[ 0 ] );
		exit ( 1 );
	}

	session = argv[ 2 ];
	login_name = argv[ 3 ];
	role_name = argv[ 4 ];
	title = argv[ 5 ];
	content_type_yn = argv[ 6 ];

	if ( argc == 8 )
	{
		omit_html_head = (*argv[ 7 ] == 'y');
	}

	if ( !appaserver_frameset_menu_horizontal(
		application_name,
		login_name ) )
	{
		sprintf(sys_string,
		"output_open_html_document %s %s \"%s\" %s",
			application_name,
			session,
			title,
			content_type_yn );

		fflush( stdout );
		system( sys_string );
		fflush( stdout );
	}
	else
	if ( !omit_html_head )
	{
		document_output_dynarch_non_frame_html_head_body(
				application_name,
				(*content_type_yn == 'y'),
				DOCUMENT_DYNARCH_MENU_ONLOAD_CONTROL_STRING,
				(char *)0 /* additional_control_string */ );

		printf( "<ul id=menu>\n" );
	}

	role_list = get_role_list(	application_name,
					login_name,
					appaserver_error_get_filename(
						application_name ) );

	if ( list_length( role_list ) == 0 )
	{
		char msg[ 1024 ];

		sprintf( msg, 
		"ERROR in %s/%s()/%d: cannot get roles for login_name = %s",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 login_name );

		appaserver_output_error_message(
				application_name, msg, login_name );

		printf( "</ul><p>%s\n", msg );
		document_output_closing();

		exit ( 0 );
	}
	else
	if ( list_length( role_list ) == 1 )
	{
		role_name = list_get_first_string( role_list );
	}

	if ( !appaserver_frameset_menu_horizontal(
		application_name,
		login_name ) )
	{
		if ( list_length( role_list ) == 1 )
		{
			printf( "<h2>%s</h2>\n",
				format_initial_capital(
					login_name_buffer, login_name ) );
			fflush( stdout );
		}
		else
		{
			sprintf(sys_string,
		"output_choose_role_drop_down %s %s %s \"%s\" \"%s\" %s 2>>%s",
				application_name,
				session,
				login_name,
				role_name,
				title,
				list_display_delimited( role_list, ',' ),
				appaserver_error_get_filename(
					application_name ) );
	
			fflush( stdout );
			system( sys_string );
			fflush( stdout );
	
			printf( "<h2>%s</h2>\n",
				format_initial_capital(
					login_name_buffer,
					login_name ) );
		}
	}

	/* If don't know the role, but has a default role, then fetch it. */
	/* -------------------------------------------------------------- */
	if ( !*role_name || strcmp( role_name, "role" ) == 0 )
	{
		role_name =
			appaserver_library_get_default_role_name(
				application_name,
				login_name );
	}

	/* If know the role, then output the other menus. */
	/* ---------------------------------------------- */
	if ( *role_name && strcmp( role_name, "role" ) != 0 )
	{
		sprintf(sys_string,
		"output_choose_folder_process_menu %s %s %s \"%s\" 2>>%s",
			login_name,
			application_name,
			session,
			role_name,
			appaserver_error_get_filename(
					application_name ) );

		fflush( stdout );
		system( sys_string );
		fflush( stdout );
	}

	if ( !appaserver_frameset_menu_horizontal(
		application_name,
		login_name ) )
	{
		sprintf(sys_string,
			"output_close_html_document %s %s",
			application_name,
			session );

		fflush( stdout );
		system( sys_string );
		fflush( stdout );
	}
	else
	{
		if ( list_length( role_list ) > 1 )
		{
			sprintf(sys_string,
		"output_choose_role_drop_down %s %s %s \"%s\" \"%s\" %s 2>>%s",
				application_name,
				session,
				login_name,
				role_name,
				title,
				list_display_delimited( role_list, ',' ),
				appaserver_error_get_filename(
					application_name ) );

			fflush( stdout );
			system( sys_string );
		}

		if ( timlib_strcmp( role_name, login_name ) != 0 )
		{
			char role_buffer[ 128 ];

			printf(
"<li><a><label style=\"color:black\">%s</label></a>\n",
			format_initial_capital(
				role_buffer, role_name ) );
		}

		printf(
"<li><a><label style=\"color:black\">%s</label></a>\n",
			format_initial_capital(
				login_name_buffer, login_name ) );

		if ( !omit_html_head )
		{
			printf(
"</ul>\n"
"</body>\n"
"</html>\n"
			);
		}
	} /* if horizontal menu */

	return 0;
} /* main() */

