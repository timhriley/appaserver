/* --------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_appaserver/post_login.c	       	*/
/* --------------------------------------------------- 	*/
/* 						       	*/
/* This program is attached to the submit button on the */
/* password form. 				       	*/
/* 							*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "folder.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver.h"
#include "environ.h"
#include "basename.h"
#include "boolean.h"
#include "dictionary.h"
#include "post2dictionary.h"
#include "appaserver_parameter_file.h"
#include "application.h"
#include "security.h"
#include "post_login.h"
#include "frameset.h"

/* Prototypes */
/* ---------- */
void post_login_frameset_output(
			char *application_name,
			char *login_name,
			char *session );

void post_login_horizontal_frameset(
			char *title,
			FRAMESET *frameset );

void post_login_vertical_frameset(
			char *title,
			FRAMESET *frameset );

int main( int argc, char **argv )
{
	POST_LOGIN *post_login;

	if ( ! ( post_login =
			/* -------------------------------- */
			/* Inserts into APPASERVER_SESSIONS */
			/* -------------------------------- */
			post_login_new(
				argc,
				argv ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: post_login_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( post_login->post_login_missing_login_name )
	{
		char msg[ 1024 ];

		sprintf(msg,
			"Missing login_name from %s",
			environment_get( "REMOTE_ADDR" ) );

		appaserver_output_error_message(
			post_login->post_login_application_name,
			msg,
			(char *)0 /* login_name */ );

		if ( !appaserver_library_from_php(
			post_login->post_login_dictionary ) )
		{
			printf( 
"Content-type: text/html					\n"
"\n"
"		<HTML>						\n"
"		<HEAD>						\n"
"			<TITLE>%s Please Try Again</TITLE>   	\n"
"		</HEAD>						\n"
"								\n"
"		<p>Please try again.				\n"
"		</HTML>						\n",
			application_name );
		}
		else
		{
			post_login_redraw_index_screen(
				post_login->post_login_application_name,
				(char *)0 /* location */,
				"invalid_login_yn=y" );
		}

		sleep( POST_LOGIN_SECONDS_TO_SLEEP );
		exit ( 1 );
	}

	if ( post_login->post_login_missing_database_password
	||   post_login->post_login_match_return == password_fail
	/* ------------------------------ */
	/* Email login is not implemented */
	/* ------------------------------ */
	||   post_login->post_login_match_return == email_login )
	{
		char msg[ 1024 ];

		if ( post_login->post_login_missing_database_password )
		{
			sprintf(msg,
				"Nonexisting appaserver user %s from %s",
				login_name,
				environment_get( "REMOTE_ADDR" ) );
		}
		else
		{
			sprintf(msg,
				"Password missmatch for %s from %s",
				login_name,
				environment_get( "REMOTE_ADDR" ) );
		}

		appaserver_output_error_message(
			post_login->post_login_application_name,
			msg,
			login_name );

		if ( !appaserver_library_from_php( post_dictionary ) )
		{
			printf( 
"Content-type: text/html					\n"
"\n"
"		<HTML>						\n"
"		<HEAD>						\n"
"			<TITLE>%s Please Try Again</TITLE>   	\n"
"		</HEAD>						\n"
"								\n"
"		<p>Please try again.				\n"
"		</HTML>						\n",
			application_name );
		}
		else
		{
			post_login_redraw_index_screen(
				application_name,
				(char *)0 /* location */,
				"invalid_login_yn=y" );
		}

		sleep( POST_LOGIN_SECONDS_TO_SLEEP );
		exit ( 1 );
	}

	environ_set_environment(
		APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
		post_login->post_login_application_name );

	environ_set_environment(
		"DATABASE",
		post_login->post_login_application_name );

	add_utility_to_path();
	add_src_appaserver_to_path();
	environ_appaserver_home();

	add_relative_source_directory_to_path(
		post_login->post_login_application_name );

	appaserver_error_login_name_append_file(
		argc,
		argv,
		post_login->post_login_application_name,
		post_login->sql_injection_login_name );

	post_login_frameset_output(
		post_login->post_login_application_name,
		post_login->sql_injection_login_name );
		post_login->post_login_session );

	return 0;
}

void post_login_frameset_output(
			char *application_name,
			char *login_name,
			char *session )
{
	FRAMESET *frameset;
	char title[ 1024 ];

	if ( ! ( frameset =
			frameset_new(
				application_name,
				login_name,
				session ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: frameset_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	strcpy( title,
		application_title(
			application_name ) );

	if ( appaserver_frameset_menu_horizontal(
		application_name,
		login_name ) )
	{
		post_login_horizontal_frameset(
			title,
			frameset );
	}
	else
	{
		post_login_vertical_frameset(
			title,
			frameset );
	}
}

void post_login_horizontal_frameset(
			char *title,
			FRAMESET *frameset )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
"output_choose_role_folder_process_form '%s' '%s' \"%s\" %s >> %s",
			frameset->session,
			frameset->login_name,
			title,
			'n' /* content_type_yn */,
			frameset->
				frameset_link_file->
				prompt_frame_create_filename );

	if ( system( sys_string ) ){};

	sprintf( sys_string,
		 "output_blank_screen.sh \"%s\" '' n > %s 2>>%s",
		 application_background_color( application_name ),
		 frameset->
			frameset_link_file->
			edit_frame_create_filename,
		 appaserver_error_filename( application_name ) );

	if ( system( sys_string ) ){};

	if ( system( "content_type_cgi.sh" ) ){};

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
	frameset->
		frameset_link_file->
		prompt_frame_prompt_filename,
	EDIT_FRAME,
	frameset->
		frameset_link_file->
		edit_frame_prompt_filename );
}

void post_login_vertical_frameset(
			char *title,
			FRAMESET *frameset )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
"output_choose_role_folder_process_form '%s' '%s' \"%s\" %s >> %s",
			session,
			login_name,
			title,
			'n' /* content_type_yn */,
			frameset->
				frameset_link_file->
				menu_frame_create_filename );

	if ( system( sys_string ) ){};

	sprintf( sys_string,
		 "output_blank_screen.sh \"%s\" \"%s\" n > %s 2>>%s",
		 application_background_color( application_name ),
		 title,
		 frameset->
			frameset_link_file->
			prompt_frame_create_filename,
		 appaserver_error_filename( application_name ) );

	if ( system( sys_string ) ){};

	sprintf( sys_string,
		 "output_blank_screen.sh \"%s\" '' n > %s 2>>%s",
		 application_background_color( application_name ),
		 frameset->
			frameset_link_file->
			edit_frame_create_filename,
		 appaserver_error_filename( application_name ) );

	if ( system( sys_string ) ){};

	if ( system( "content_type_cgi.sh" ) ){};

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
	frameset->
		frameset_link_file->
		menu_frame_prompt_filename,
	PROMPT_FRAME,
	frameset->
		frameset_link_file->
		prompt_frame_prompt_filename,
	EDIT_FRAME,
	frameset->
		frameset_link_file->
		edit_frame_prompt_filename );
}
