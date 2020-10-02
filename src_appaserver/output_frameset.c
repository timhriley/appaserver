/* ----------------------------------------------------	*/
/* src_appaserver/output_frameset.c			*/
/* ----------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ----------------------------------------------------	*/

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
#include "appaserver_link_file.h"

#define LOCAL_CONTENT_TYPE_YN		"n"

void output_vertical_frameset(		char *application_name,
					char *session,
					char *login_name,
					char *role_name,
					char *menu_frame_prompt_filename,
					char *menu_frame_create_filename,
					char *prompt_frame_prompt_filename,
					char *prompt_frame_create_filename,
					char *edit_frame_prompt_filename,
					char *edit_frame_create_filename,
					char *title,
					boolean content_type );

void output_horizontal_frameset(	char *application_name,
					char *session,
					char *login_name,
					char *role_name,
					char *prompt_frame_prompt_filename,
					char *prompt_frame_create_filename,
					char *edit_frame_prompt_filename,
					char *edit_frame_create_filename,
					char *title,
					boolean content_type );

int main( int argc, char **argv )
{
	char *application_name, *session, *login_name;
	char *role_name;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char title[ 1024 ];
	char *menu_frame_prompt_filename;
	char *menu_frame_create_filename;
	char *prompt_frame_prompt_filename;
	char *prompt_frame_create_filename;
	char *edit_frame_prompt_filename;
	char *edit_frame_create_filename;
	APPASERVER_LINK_FILE *appaserver_link_file;
	boolean content_type = 1;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc < 5 )
	{
		fprintf( stderr, 
"Usage: %s ignored session login_name role [content_type_yn]\n", 
		argv[ 0 ] );
		exit ( 1 );
	}

	session = argv[ 2 ];
	login_name = argv[ 3 ];
	role_name = argv[ 4 ];

	if ( argc == 6 )
	{
		content_type = ( *argv[ 5 ] == 'y' );
	}

	add_src_appaserver_to_path();
	environ_set_utc_offset( application_name );

	environ_prepend_dot_to_path();
	add_utility_to_path();
	add_relative_source_directory_to_path( application_name );

	appaserver_parameter_file = appaserver_parameter_file_new();

	appaserver_link_file =
		appaserver_link_file_new(
			application_get_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_get_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		appaserver_parameter_file->document_root,
			(char *)0 /* filename_stem */,
			application_name,
			0 /* process_id */,
			session,
			"html" );

	appaserver_link_file->filename_stem = MENU_FRAME;

	menu_frame_prompt_filename =
		appaserver_link_get_link_prompt(
			appaserver_link_file->
				link_prompt->
				prepend_http_boolean,
			appaserver_link_file->
				link_prompt->
				http_prefix,
			appaserver_link_file->
				link_prompt->server_address,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	menu_frame_create_filename =
		appaserver_link_get_output_filename(
			appaserver_link_file->
				output_file->
				document_root_directory,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	if ( !zap_file( menu_frame_create_filename ) )
	{
		char msg[ 1024 ];

		sprintf(msg,
			"ERROR in %s/%s()/%d: cannot write to file (%s)",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			menu_frame_create_filename );

		appaserver_output_error_message(
			application_name,
			msg,
			login_name );
		exit( 1 );
	}

	appaserver_link_file->filename_stem = PROMPT_FRAME;

	prompt_frame_prompt_filename =
		appaserver_link_get_link_prompt(
			appaserver_link_file->
				link_prompt->
				prepend_http_boolean,
			appaserver_link_file->
				link_prompt->
				http_prefix,
			appaserver_link_file->
				link_prompt->server_address,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	prompt_frame_create_filename =
		appaserver_link_get_output_filename(
			appaserver_link_file->
				output_file->
				document_root_directory,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	appaserver_link_file->filename_stem = EDIT_FRAME;

	edit_frame_prompt_filename =
		appaserver_link_get_link_prompt(
			appaserver_link_file->
				link_prompt->
				prepend_http_boolean,
			appaserver_link_file->
				link_prompt->
				http_prefix,
			appaserver_link_file->
				link_prompt->server_address,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	edit_frame_create_filename =
		appaserver_link_get_output_filename(
			appaserver_link_file->
				output_file->
				document_root_directory,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	strcpy( title, 
		application_get_application_title(
			application_name ) );

	if ( appaserver_frameset_menu_horizontal(
					application_name,
					login_name ) )
	{
		output_horizontal_frameset(
					application_name,
					session,
					login_name,
					role_name,
					prompt_frame_prompt_filename,
					prompt_frame_create_filename,
					edit_frame_prompt_filename,
					edit_frame_create_filename,
					title,
					content_type );
	}
	else
	{
		output_vertical_frameset(
					application_name,
					session,
					login_name,
					role_name,
					menu_frame_prompt_filename,
					menu_frame_create_filename,
					prompt_frame_prompt_filename,
					prompt_frame_create_filename,
					edit_frame_prompt_filename,
					edit_frame_create_filename,
					title,
					content_type );
	}
	exit( 0 );
}

void output_vertical_frameset(		char *application_name,
					char *session,
					char *login_name,
					char *role_name,
					char *menu_frame_prompt_filename,
					char *menu_frame_create_filename,
					char *prompt_frame_prompt_filename,
					char *prompt_frame_create_filename,
					char *edit_frame_prompt_filename,
					char *edit_frame_create_filename,
					char *title,
					boolean content_type )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
"output_choose_role_folder_process_form '%s' '%s' '%s' '%s' \"%s\" %s >> %s",
			application_name,
			session,
			login_name,
			role_name,
			title,
			LOCAL_CONTENT_TYPE_YN,
			menu_frame_create_filename );

	if ( system( sys_string ) ){};

	sprintf( sys_string,
		 "output_blank_screen.sh \"%s\" \"%s\" n > %s 2>>%s",
		 application_get_background_color( application_name ),
		 title,
		 prompt_frame_create_filename,
		 appaserver_get_error_filename( application_name ) );
	if ( system( sys_string ) ){};

	sprintf( sys_string,
		 "output_blank_screen.sh \"%s\" '' n > %s 2>>%s",
		 application_get_background_color( application_name ),
		 edit_frame_create_filename,
		 appaserver_get_error_filename( application_name ) );
	if ( system( sys_string ) ){};

	if ( content_type )
	{
		printf(
"Content-type: text/html\n\n" );
	}

	printf(
"<HTML>									\n"
"<HEAD>									\n"
"	<TITLE>%s</TITLE>			  			\n"
"</HEAD>								\n"
"									\n"
"<frameset cols=\"%d,*\">						\n"
"<frame name=\"%s\" src=\"%s\">						\n"
"<frameset rows=\"*,*\">						\n"
"<frame name=\"%s\" src=\"%s\">						\n"
"<frame name=\"%s\" src=\"%s\">						\n"
"</frameset>								\n"
"</frameset>								\n"
"</HTML>								\n",
	title,
	MENU_VERTICAL_PIXEL_COLUMNS,
	MENU_FRAME,
	menu_frame_prompt_filename,
	PROMPT_FRAME,
	prompt_frame_prompt_filename,
	EDIT_FRAME,
	edit_frame_prompt_filename );
}

void output_horizontal_frameset(	char *application_name,
					char *session,
					char *login_name,
					char *role_name,
					char *prompt_frame_prompt_filename,
					char *prompt_frame_create_filename,
					char *edit_frame_prompt_filename,
					char *edit_frame_create_filename,
					char *title,
					boolean content_type )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
"output_choose_role_folder_process_form '%s' '%s' '%s' '%s' \"%s\" %s >> %s",
			application_name,
			session,
			login_name,
			role_name,
			title,
			LOCAL_CONTENT_TYPE_YN,
			prompt_frame_create_filename );

	if ( system( sys_string ) ){};

	sprintf( sys_string,
		 "output_blank_screen.sh \"%s\" '' n > %s 2>>%s",
		 application_get_background_color( application_name ),
		 edit_frame_create_filename,
		 appaserver_error_get_filename(
				application_name ) );

	if ( system( sys_string ) ){};

	if ( content_type )
	{
		printf(
"Content-type: text/html\n\n" );
	}

	printf(
"<HTML xmlns=\"http://www.w3.org/1999/xhtml\">\n"
"<HEAD>\n"
"	<TITLE>%s</TITLE>\n"
"</HEAD>\n"
"<frameset rows=\"*,*\">\n"
"<frame name=\"%s\" src=\"%s\">\n"
"<frame name=\"%s\" src=\"%s\">\n"
"</frameset>\n"
"</HTML>\n",
	title,
	PROMPT_FRAME,
	prompt_frame_prompt_filename,
	EDIT_FRAME,
	edit_frame_prompt_filename );

}

