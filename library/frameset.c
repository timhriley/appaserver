/* $APPASERVER_HOME/library/frameset.c			*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "application.h"
#include "appaserver_library.h"
#include "appaserver_user.h"
#include "timlib.h"
#include "frameset.h"

FRAMESET *frameset_calloc( void )
{
	FRAMESET *frameset;

	if ( ! ( frameset = calloc( 1, sizeof( FRAMESET ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return frameset;
}

FRAMESET_FRAME *frameset_frame_calloc( void )
{
	FRAMESET_FRAME *frameset_frame;

	if ( ! ( frameset_frame = calloc( 1, sizeof( FRAMESET_FRAME ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return frameset_frame;
}

FRAMESET *frameset_new(	char *application_name,
			char *session_key,
			boolean frameset_menu_horizontal )
{
	FRAMESET *frameset = frameset_calloc();

	frameset->appaserver_parameter =
		appaserver_parameter_new();

	if ( !frameset_menu_horizontal )
	{
		frameset->frameset_frame_menu =
			frameset_frame_new(
				application_name,
				frameset->
					appaserver_parameter->
					document_root_directory,
				session_key,
				FRAMESET_MENU_FRAME );
	}

	frameset->frameset_frame_prompt =
		frameset_frame_new(
			application_name,
			frameset->
				appaserver_parameter->
				document_root_directory,
			session_key,
			FRAMESET_PROMPT_FRAME );

	frameset->frameset_frame_edit =
		frameset_frame_new(
			application_name,
			frameset->
				appaserver_parameter->
				document_root_directory,
			session_key,
			FRAMESET_EDIT_FRAME );

	frameset->html =
		/* -------------------------- */
		/* Safely returns heap memory */
		/* -------------------------- */
		frameset_html(
			frameset->frameset_frame_menu->html,
			frameset->frameset_frame_prompt->html,
			frameset->frameset_frame_edit->html,
			frameset_menu_horizontal );

	return frameset;
}

char *frameset_html(	char *frameset_frame_menu_html,
			char *frameset_frame_prompt_html,
			char *frameset_frame_edit_html,
			boolean frameset_menu_horizontal )
{
	char html[ 2048 ];

	if ( !frameset_frame_menu_html
	||   !*frameset_frame_menu_html
	||   !frameset_frame_prompt_html
	||   !*frameset_frame_prompt_html
	||   !frameset_frame_edit_html
	||   !*frameset_frame_edit_html )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: a parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !frameset_menu_horizontal )
	{
		sprintf(html,
			"<frameset cols=\"200,*\">\n"
			"%s\n"
			"<frameset rows=\"*,*\">\n"
			"%s\n"
			"%s\n"
			"</frameset>\n"
			"</frameset>\n",
			frameset_frame_menu_html,
			frameset_frame_prompt_html,
			frameset_frame_edit_html );
	}
	else
	{
		sprintf(html,
			"<frameset rows=\"*,*\">\n"
			"%s\n"
			"%s\n"
			"</frameset>\n",
			frameset_frame_prompt_html,
			frameset_frame_edit_html );
	}

	return strdup( html );
}

FRAMESET_FRAME *frameset_frame_new(
			char *application_name,
			char *document_root_directory,
			char *session_key,
			char *frame_name )
{
	FRAMESET_FRAME *frameset_frame = frameset_frame_calloc();

	frameset_frame->appaserver_link =
		appaserver_link_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		document_root_directory,
			frame_name /* filename_stem */,
			application_name,
			0 /* process_id */,
			session_key,
			(char *)0 /* begin_date_string */,
			(char *)0 /* end_date_string */,
			"html" );

	frameset_frame->prompt_filename =
		frameset_frame->
			appaserver_link->
			prompt->
			filename;

	frameset_frame->output_filename =
		frameset_frame->
			appaserver_link->
			output->
			filename;

	if ( !zap_file( frameset_frame->output_filename ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: zap_file(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			frameset_frame->output_filename );
		exit( 1 );
	}

	frameset_frame->html =
		/* -------------------------- */
		/* Safely returns heap memory */
		/* -------------------------- */
		frameset_frame_html(
			frame_name,
			frameset_frame->prompt_filename );

	return frameset_frame;
}

char *frameset_frame_html(
			char *frame_name,
			char *prompt_filename )
{
	char html[ 1024 ];

	if ( !frame_name
	||   !*frame_name
	||   !prompt_filename
	||   !*prompt_filename )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: a parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(html,
		"<frame name=\"%s\" src=\"%s\">",
		frame_name,
		prompt_filename );

	return strdup( html );
}

boolean frameset_menu_horizonatal(
			FRAMESET_FRAME *frameset_frame_menu )
{
	if ( frameset_frame_menu )
		return 0;
	else
		return 1;
}

boolean frameset_menu_horizontal(
			char *application_name,
			char *login_name )
{
	char frameset_menu_horizontal_yn;

	if ( appaserver_user_frameset_menu_horizontal( login_name ) )
	{
		return 1;
	}

	frameset_menu_horizontal_yn =
		application_frameset_menu_horizontal_yn(
			application_name );

	return (frameset_menu_horizontal_yn == 'y');
}

char *frameset_output_system_string(
			char *executable,
			char *session_key,
			char *login_name,
			char *post_login_output_pipe_string )
{
	char system_string[ 1024 ];

	if ( !executable
	||   !session_key
	||   !login_name
	||   !post_login_output_pipe_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"%s %s %s %s",
		executable,
		session_key,
		login_name,
		post_login_output_pipe_string );

	return strdup( system_string );

}

