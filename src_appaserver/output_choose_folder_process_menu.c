/* --------------------------------------------------		*/
/* src_appaserver/output_choose_folder_process_menu.c		*/
/* --------------------------------------------------		*/
/*								*/
/* Freely available software: see Appaserver.org		*/
/* --------------------------------------------------		*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "list.h"
#include "folder.h"
#include "role.h"
#include "appaserver.h"
#include "appaserver_user.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "role_folder.h"
#include "application.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "horizontal_menu.h"
#include "folder_menu.h"

/* Constants */
/* --------- */
#define CHOOSE_ROLE_DROP_DOWN_FORM_NAME		"refresh_choose_role_drop_down"

/* Structures */
/* ---------- */
enum horizontal_orphan {subschemas_only,
			orphans_only,
			orphans_and_subschemas };

/* Prototypes */
/* ---------- */
LIST *get_process_record_list(
				char *entity,
				char *role,
				char *error_file,
				boolean with_group_name_trimmed );

void output_process_element(	char *apache_cgi_directory,
				char *server_address,
				char *application_name,
				char *login_name,
				char *session,
				char *process_name,
				char *role_name,
				char *target_frame );

void output_folder_element(	char *apache_cgi_directory,
				char *server_address,
				char *application_name,
				char *login_name,
				char *session,
				char *folder_name,
				char *role_name,
				char *state,
				char *target_frame,
				char folder_count_yn,
				long count );

void output_vertical_folders(
			char *login_name,
			char *application_name,
			char *session,
			char *role_name,
			char *target_frame,
			LIST *folder_list,
			char *apache_cgi_directory,
			char *server_address,
			char folder_count_yn );

void output_vertical_processes(
			char *login_name,
			char *application_name,
			char *session,
			char *role_name,
			char *target_frame,
			LIST *process_record_list,
			char *apache_cgi_directory,
			char *server_address );

void output_horizontal_folders(
			char *login_name,
			char *application_name,
			char *session,
			char *role_name,
			char *target_frame,
			char *apache_cgi_directory,
			char *server_address,
			char folder_count_yn,
			char *state,
			LIST *subschema_list,
			LIST *orphan_folder_list );

void output_horizontal_processes(
			char *login_name,
			char *application_name,
			char *session,
			char *role_name,
			char *target_frame,
			char *apache_cgi_directory,
			char *server_address,
			LIST *process_group_list,
			LIST *orphan_process_name_list );

int main( int argc, char **argv )
{
	char *login_name, *application_name, *session, *role_name;
	LIST *process_record_list;
	LIST *folder_list = {0};
	char buffer[ 1024 ];
	ROLE *role;
	LIST *role_folder_insert_list;
	boolean with_group_name_trimmed;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 5 )
	{
		fprintf( stderr, 
	"Usage: %s login_name ignored session role\n", 
			 argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 1 ];
	session = argv[ 3 ];
	role_name = argv[ 4 ];

	add_src_appaserver_to_path();
	environ_set_utc_offset( application_name );

	appaserver_parameter_file = appaserver_parameter_file_new();

	role = role_new_role( application_name, role_name );
	
	if ( appaserver_frameset_menu_horizontal(
						application_name,
						login_name ) )
	{
		with_group_name_trimmed = 0;
	}
	else
	{
		with_group_name_trimmed = 1;
	}

	role_folder_insert_list = role_folder_get_insert_list(
					application_name,
					session,
					role_name );

	folder_list =
		folder_menu_get_choose_folder_list(
		     application_name,
		     appaserver_parameter_file_get_data_directory(),
		     session,
		     role_name,
		     (role->folder_count_yn == 'y' ) );

	process_record_list =
		get_process_record_list(
			application_name,
			role_name,
			appaserver_error_get_filename( application_name ),
			with_group_name_trimmed );
	
	if ( list_length( folder_list ) == 0
	&&   list_length( process_record_list ) == 1 )
	{
		sprintf( buffer,
			 "post_choose_process %s %s %s %s %s",
			 login_name,
			application_name,
			 session,
			 (char *)list_get_first_pointer( process_record_list ),
			 role_name );

		exit( system( buffer ) );
	}

	role_folder_populate_folder_insert_permission(
			folder_list, 
			role_folder_insert_list,
			application_name );

	if ( appaserver_frameset_menu_horizontal(
						application_name,
						login_name ) )
	{
		HORIZONTAL_MENU *horizontal_menu;

		horizontal_menu =
			horizontal_menu_new(
				folder_list,
				process_record_list );

		output_horizontal_folders(
				login_name,
				application_name,
				session,
				role_name,
				PROMPT_FRAME,
				appaserver_parameter_file->apache_cgi_directory,
				appaserver_library_get_server_address(),
				role->folder_count_yn,
				"lookup",
				horizontal_menu->lookup_subschema_list,
			        horizontal_menu->
					lookup_orphan_folder_list );

		output_horizontal_folders(
				login_name,
				application_name,
				session,
				role_name,
				PROMPT_FRAME,
				appaserver_parameter_file->apache_cgi_directory,
				appaserver_library_get_server_address(),
				role->folder_count_yn,
				"insert",
				horizontal_menu->insert_subschema_list,
			        horizontal_menu->
					insert_orphan_folder_list );
	
		output_horizontal_processes(
				login_name,
				application_name,
				session,
				role_name,
				PROMPT_FRAME,
				appaserver_parameter_file->apache_cgi_directory,
				appaserver_library_get_server_address(),
				horizontal_menu->process_group_list,
			        horizontal_menu->orphan_process_name_list );
	}
	else
	{
		output_vertical_folders(
				login_name,
				application_name,
				session,
				role_name,
				PROMPT_FRAME,
				folder_list,
				appaserver_parameter_file->apache_cgi_directory,
				appaserver_library_get_server_address(),
				role->folder_count_yn );
	
		output_vertical_processes(
				login_name,
				application_name,
				session,
				role_name,
				PROMPT_FRAME,
				process_record_list,
				appaserver_parameter_file->apache_cgi_directory,
				appaserver_library_get_server_address() );
	}

	return 0;
} /* main() */

void output_vertical_folders(
			char *login_name,
			char *application_name,
			char *session,
			char *role_name,
			char *target_frame,
			LIST *folder_list,
			char *apache_cgi_directory,
			char *server_address,
			char folder_count_yn )
{
	char buffer[ 256 ];
	FOLDER *folder;
	char *state = "lookup";

	if ( !list_reset( folder_list ) ) return;

	printf(
"	<table border>							\n"
"	<tr>								\n"
"	<th colspan=3><h2 align=center>Data</h2></th>			\n" );

	do {
		folder = list_get_pointer( folder_list );

		printf ( "<tr>\n" );

		/* Output open folder */
		/* ------------------ */
		printf(
"<td><A class=%s HREF=\"%s/post_choose_folder?"
				"%s+%s+%s+%s+%s+%s\""
				" target=%s>%s</A></td>\n",
				VERTICAL_MENU_CLASS,
				appaserver_library_get_http_prompt(
					apache_cgi_directory,
					server_address,
					application_get_ssl_support_yn(
						application_name ),
				       application_get_prepend_http_protocol_yn(
						application_name ) ),
				 login_name,
				 application_name,
				 session,
				 folder->folder_name, 
				 role_name,
				 state,
				 target_frame,
				 format_initial_capital(
						buffer, 
				 		folder->folder_name ) );

		if ( folder_count_yn == 'y' )
		{
			printf(
				"<td align=right>%s</td>\n",
				 place_commas_in_long( folder->row_count ) );
		}

		if ( folder->insert_permission )
		{
			/* Output new folder */
			/* ----------------- */
			printf(
"<td><A class=%s HREF=\"%s/post_choose_folder?"
				"%s+%s+%s+%s+%s+insert\""
				" target=%s>New</A></td>\n",
				VERTICAL_MENU_CLASS,
				appaserver_library_get_http_prompt(
					apache_cgi_directory,
				 	server_address,
					application_get_ssl_support_yn(
						application_name ),
				       application_get_prepend_http_protocol_yn(
					application_name ) ),
				 login_name,
				 application_name,
				 session,
				 folder->folder_name, role_name,
				 target_frame );
		}
	} while( list_next( folder_list ) );

/* See folder_menu.h
	if ( folder_count_yn == 'y' )
	{
		output_recount_submit_button_form(
					application_name,
					login_name,
					session,
					role_name );
	}
*/

	printf( 
"	</table>\n" );

} /* output_vertical_folders() */

void output_vertical_processes(
			char *login_name,
			char *application_name,
			char *session,
			char *role_name,
			char *target_frame,
			LIST *process_record_list,
			char *apache_cgi_directory,
			char *server_address )
{
	char buffer[ 256 ];
	char *process_name;

	if ( list_reset( process_record_list ) )
	{
		printf(
"	<table border>							\n"
"	<tr>								\n"
"	<th><h2 align=center>Processes</h2></th>			\n" );

		do {
			process_name = list_get_pointer( process_record_list );
			printf ( "<tr>\n" );

			printf(
"<td><A class=%s HREF=\"%s/post_choose_process?"
				"%s+%s+%s+%s+%s",
				VERTICAL_MENU_CLASS,
				appaserver_library_get_http_prompt(
					apache_cgi_directory,
				 	server_address,
					application_get_ssl_support_yn(
						application_name ),
				       application_get_prepend_http_protocol_yn(
						application_name ) ),
				 login_name,
				 application_name,
				 session,
				 process_name, role_name );

			printf( "\" target=%s>%s</A>\n",
				 target_frame, 
				 format_initial_capital( buffer,
							 process_name ) );
		} while( list_next( process_record_list ) );
	}

	printf( 
"	</table>\n" );

} /* output_vertical_processes() */

void output_horizontal_folders(
			char *login_name,
			char *application_name,
			char *session,
			char *role_name,
			char *target_frame,
			char *apache_cgi_directory,
			char *server_address,
			char folder_count_yn,
			char *state,
			LIST *subschema_list,
			LIST *orphan_folder_list )
{
	char buffer[ 128 ];
	HORIZONTAL_MENU_SUBSCHEMA *subschema;
	HORIZONTAL_MENU_FOLDER *horizontal_menu_folder;
	enum horizontal_orphan horizontal_orphan;

	if ( !list_length( subschema_list )
	&&   !list_length( orphan_folder_list ) )
	{
		return;
	}
	else
	if ( list_length( subschema_list )
	&&   !list_length( orphan_folder_list ) )
	{
		horizontal_orphan = subschemas_only;
	}
	else
	if ( !list_length( subschema_list )
	&&   list_length( orphan_folder_list ) )
	{
		horizontal_orphan = orphans_only;
	}
	else
	{
		horizontal_orphan = orphans_and_subschemas;
	}

	if ( strcmp( state, "lookup" ) == 0 )
	{
		printf(
"\t<li>\n"
"\t<a><span class=%s>Lookup</span></a>\n"
"\t<ul>\n",
			HORIZONTAL_MENU_CLASS
		);
	}
	else
	if ( strcmp( state, "insert" ) == 0 )
	{
			printf(
"\t<li>\n"
"\t<a><span class=%s>Insert</span></a>\n"
"\t<ul>\n",

			HORIZONTAL_MENU_CLASS
		);
	}
	else
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: invalid state = (%s)\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			state );
		exit( 1 );
	}

	if ( list_rewind( subschema_list ) )
	{
		do {
			subschema = list_get_pointer( subschema_list );
	
			printf(
"\t\t<li>\n"
"\t\t<a><span class=%s>%s</span></a>\n"
"\t\t<ul>\n",
				HORIZONTAL_MENU_CLASS,
				format_initial_capital(
					buffer,
					subschema->subschema_name ) );

			if ( !list_rewind( subschema->
						horizontal_menu_folder_list ) )
			{
				fprintf(stderr,
		"ERROR in %s/%s()/%d: empty horizontal_menu_folder_list.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}

			do {
				horizontal_menu_folder =
					list_get_pointer(
						subschema->
						horizontal_menu_folder_list );

				output_folder_element(
					apache_cgi_directory,
					server_address,
					application_name,
					login_name,
					session,
					horizontal_menu_folder->folder_name,
					role_name,
					state,
					target_frame,
					folder_count_yn,
					horizontal_menu_folder->row_count );

			} while( list_next( subschema->
						horizontal_menu_folder_list ) );
			printf( "\t\t</ul>\n" );
		} while ( list_next( subschema_list ) );

		if ( horizontal_orphan == subschemas_only )
			printf( "\t</ul>\n" );

	} /* if ( list_rewind( subschema_list ) ) */

	if ( list_rewind( orphan_folder_list ) )
	{
		do {
			horizontal_menu_folder =
				list_get_pointer(
					orphan_folder_list );

			output_folder_element(
					apache_cgi_directory,
					server_address,
					application_name,
					login_name,
					session,
					horizontal_menu_folder->folder_name,
					role_name,
					state,
					target_frame,
					folder_count_yn,
					horizontal_menu_folder->row_count );

		} while ( list_next( orphan_folder_list ) );

		printf( "\t</ul>\n" );

	} /* if ( list_rewind( orphan_folder_list ) ) */

} /* output_horizontal_folders() */

void output_horizontal_processes(
			char *login_name,
			char *application_name,
			char *session,
			char *role_name,
			char *target_frame,
			char *apache_cgi_directory,
			char *server_address,
			LIST *process_group_list,
			LIST *orphan_process_name_list )
{
	char buffer[ 256 ];
	char *process_name;
	HORIZONTAL_MENU_PROCESS_GROUP *process_group;

	if ( list_rewind( process_group_list ) )
	{
		do {
			process_group =
				list_get_pointer(
					process_group_list );

			printf(
"\t<li>\n"
"\t<a><span class=%s>%s</span></a>\n"
"\t<ul>\n",
				HORIZONTAL_MENU_CLASS,
				format_initial_capital(
					buffer,
					process_group->process_group_name ) );

			if ( !list_rewind( process_group->process_name_list ) )
			{
				fprintf(stderr,
			"ERROR in %s/%s()/%d: empty process_name_list.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}

			do {
				process_name =
					list_get_pointer(
						process_group->
							process_name_list );

				output_process_element(
					apache_cgi_directory,
					server_address,
					application_name,
					login_name,
					session,
					process_name,
					role_name,
					target_frame );

			} while( list_next( process_group->
						process_name_list ) );
			printf( "\t</ul>\n" );
		} while( list_next( process_group_list ) );

	} /* if ( list_rewind( process_group_list ) ) */

	if ( list_rewind( orphan_process_name_list ) )
	{
		printf(
"\t<li>\n"
"\t<a><span class=%s>Process</span></a>\n"
"\t<ul>\n",
		HORIZONTAL_MENU_CLASS
		);

		do {
			process_name =
				list_get_pointer(
					orphan_process_name_list );

			output_process_element(
					apache_cgi_directory,
					server_address,
					application_name,
					login_name,
					session,
					process_name,
					role_name,
					target_frame );

		} while ( list_next( orphan_process_name_list ) );

		printf( "\t</ul>\n" );

	} /* if ( list_rewind( orphan_process_name_list ) ) */

} /* output_horizontal_processes() */

void output_folder_element(	char *apache_cgi_directory,
				char *server_address,
				char *application_name,
				char *login_name,
				char *session,
				char *folder_name,
				char *role_name,
				char *state,
				char *target_frame,
				char folder_count_yn,
				long count )
{
	char action_string[ 1024 ];
	char buffer[ 256 ];

	sprintf(action_string,
		"%s/post_choose_folder?%s+%s+%s+%s+%s+%s",
		appaserver_library_get_http_prompt(
			apache_cgi_directory,
		 	server_address,
			application_get_ssl_support_yn(
				application_name ),
		       application_get_prepend_http_protocol_yn(
				application_name ) ),
		login_name,
		application_name,
		session,
		folder_name,
		role_name,
		state );
	
		if ( strcmp( state, "lookup" ) == 0
		&&   folder_count_yn == 'y' )
		{
			printf(
	"		<li>\n"
	"        	<a href=\"%s\" target=\"%s\">\n"
	"		<span class=%s>%s %s</span></a>\n",
			action_string,
			target_frame,
			HORIZONTAL_MENU_CLASS,
			format_initial_capital(
				buffer, folder_name ),
			 place_commas_in_long(
				 count ) );
		}
		else
		{
			printf(
	"		<li>\n"
	"        	<a href=\"%s\" target=\"%s\">\n"
	"		<span class=%s>%s</span></a>\n",
			action_string,
			target_frame,
			HORIZONTAL_MENU_CLASS,
			format_initial_capital(
				buffer, folder_name ) );
		}
} /* output_folder_element() */

void output_process_element(	char *apache_cgi_directory,
				char *server_address,
				char *application_name,
				char *login_name,
				char *session,
				char *process_name,
				char *role_name,
				char *target_frame )
{
	char action_string[ 1024 ];
	char buffer[ 256 ];

	sprintf(action_string,
		"%s/post_choose_process?%s+%s+%s+%s+%s",
		appaserver_library_get_http_prompt(
		 	apache_cgi_directory,
		 	server_address,
			application_get_ssl_support_yn(
				application_name ),
		application_get_prepend_http_protocol_yn(
			application_name ) ),
		login_name,
		application_name,
		session,
		process_name,
		role_name );
	
		printf(
"		<li>\n"
"        	<a href=\"%s\" target=\"%s\">\n"
"		<span class=%s>%s</span></a>\n",
			action_string,
			target_frame,
			HORIZONTAL_MENU_CLASS,
			format_initial_capital(
				buffer, process_name ) );
} /* output_process_element() */

LIST *get_process_record_list(
			char *application,
			char *role,
			char *error_file,
			boolean with_group_name_trimmed )
{
	char sys_string[ 4096 ];

	if ( with_group_name_trimmed )
	{
		sprintf( sys_string, 
		"(process_records4role.sh %s %s '%c'			|"
		"piece.e '%c' 0 2>>%s					;"
		"process_sets4role.sh %s %s '%c'			|"
		"piece.e '%c' 0 2>>%s)					|"
		"sort							 ",
			 application, role,
			 FOLDER_DATA_DELIMITER,
			 FOLDER_DATA_DELIMITER, error_file,
			 application, role, 
			 FOLDER_DATA_DELIMITER,
			 FOLDER_DATA_DELIMITER, error_file );
	}
	else
	{
		sprintf( sys_string, 
		"(process_records4role.sh %s %s '%c' 2>>%s		;"
		"process_sets4role.sh %s %s '%c' 2>>%s)			|"
		"sort							 ",
			 application, role,
			 FOLDER_DATA_DELIMITER,
			 error_file,
			 application, role, 
			 FOLDER_DATA_DELIMITER,
			 error_file );
	}

	return pipe2list( sys_string );
} /* get_process_record_list() */

