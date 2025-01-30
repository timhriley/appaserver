/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/frameset.c					*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "application.h"
#include "appaserver_error.h"
#include "appaserver_user.h"
#include "timlib.h"
#include "form.h"
#include "role.h"
#include "folder.h"
#include "choose_role.h"
#include "String.h"
#include "execute_system_string.h"
#include "frameset.h"

FRAMESET *frameset_calloc( void )
{
	FRAMESET *frameset;

	if ( ! ( frameset = calloc( 1, sizeof ( FRAMESET ) ) ) )
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

	if ( ! ( frameset_frame = calloc( 1, sizeof ( FRAMESET_FRAME ) ) ) )
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

FRAMESET *frameset_new(
		char *application_name,
		char *session_key,
		char *login_name,
		boolean application_menu_horizontal_boolean )
{
	FRAMESET *frameset;

	if ( !application_name
	||   !session_key
	||   !login_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	frameset = frameset_calloc();

	frameset->appaserver_parameter =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_parameter_new();

	frameset->frameset_document =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		document_basic_new(
			application_name,
			(LIST *)0 /* javascript_filename_list */,
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			application_title_string( application_name ) );

	frameset->role_name =
		frameset_role_name(
			role_appaserver_user_role_name_list(
				login_name ),
			appaserver_user_default_role_name(
				login_name ) );

	if ( frameset->role_name )
	{
		frameset->folder_menu =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			folder_menu_fetch(
				application_name,
				session_key,
				frameset->role_name,
				frameset->
					appaserver_parameter->
					data_directory,
				0 /* not folder_menu_remove_boolean */ );

		frameset->menu =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			menu_new(
				application_name,
				session_key,
				login_name,
				frameset->role_name,
				application_menu_horizontal_boolean,
				frameset->folder_menu->count_list );

		frameset->frame_menu_document =
			document_new(
				application_name,
				/* ------------------------- */
				/* Returns heap memory or "" */
				/* ------------------------- */
				application_title_string( application_name ),
				(char *)0 /* title_string */,
				(char *)0 /* sub_title_string */,
				(char *)0/* sub_sub_title_string */,
				(char *)0/* notepad */,
				(char *)0 /* onload_javascript_string */,
				frameset->menu->html,
				document_head_menu_setup_string(
					application_menu_horizontal_boolean ),
				(char *)0 /* calendar_setup_string */,
				javascript_include_string(
					application_name,
					(LIST *)0
					/* javascript_filename_list */ ) );

		frameset->document_form_html =
			/* Returns heap memory */
			/* ------------------- */
			document_form_html(
				frameset->frame_menu_document->html,
				frameset->
					frame_menu_document->
					document_head->
					html,
				/* ---------------------- */
				/* Returns program memory */
				/* ---------------------- */
				document_head_close_tag(),
				frameset->
					frame_menu_document->
					document_body->
					html,
				(char *)0 /* form_html */,
				/* ---------------------- */
				/* Returns program memory */
				/* ---------------------- */
				document_body_close_tag(),
				/* ---------------------- */
				/* Returns program memory */
				/* ---------------------- */
				document_close_tag() );

	}

	if ( !application_menu_horizontal_boolean )
	{
		frameset->frameset_frame_menu =
			frameset_frame_new(
				application_name,
				frameset->
					appaserver_parameter->
					data_directory,
				session_key,
				FRAMESET_MENU_FRAME,
				frameset->document_form_html );

		frameset->document_form_html = (char *)0;
	}

	frameset->frameset_frame_prompt =
		frameset_frame_new(
			application_name,
			frameset->
				appaserver_parameter->
				data_directory,
			session_key,
			FRAMESET_PROMPT_FRAME,
			frameset->document_form_html );

	frameset->frameset_frame_table =
		frameset_frame_new(
			application_name,
			frameset->
				appaserver_parameter->
				data_directory,
			session_key,
			FRAMESET_TABLE_FRAME,
			(char *)0 /* document_form_html */ );

	if ( !frameset->role_name )
	{
		frameset->filename =
			/* ----------------------------------- */
			/* Returns either of ->output_filename */
			/* ----------------------------------- */
			frameset_filename(
				frameset->frameset_frame_menu,
				frameset->frameset_frame_prompt );

		frameset->execute_system_string_choose_role =
			execute_system_string_choose_role(
				CHOOSE_ROLE_EXECUTABLE,
				session_key,
				login_name,
				application_menu_horizontal_boolean,
				frameset->filename );
	}

	frameset->blank_table_frame_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		frameset_blank_system_string(
			application_name,
			application_background_color(
				application_name ),
			(char *)0 /* application_title_string */,
			frameset->frameset_frame_table->output_filename,
			appaserver_error_filename(
				application_name ) );

	if ( !application_menu_horizontal_boolean )
	{
		frameset->blank_prompt_frame_system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			frameset_blank_system_string(
				application_name,
				application_background_color(
					application_name ),
				/* ------------------------- */
				/* Returns heap memory or "" */
				/* ------------------------- */
				application_title_string(
					application_name ),
				frameset->
					frameset_frame_prompt->
					output_filename,
				appaserver_error_filename(
					application_name ) );
	}

	frameset->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		frameset_html(
			frameset->frameset_document->html,
			frameset->frameset_document->document_head->html,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_head_close_tag(),
			frameset->frameset_frame_menu,
			frameset->frameset_frame_prompt->tag,
			frameset->frameset_frame_table->tag,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_close_tag() );

	return frameset;
}

char *frameset_html(	char *document_html,
			char *document_head_html,
			char *document_head_close_tag,
			FRAMESET_FRAME *frameset_frame_menu,
			char *frameset_frame_prompt_tag,
			char *frameset_frame_table_tag,
			char *document_close_tag )
{
	char html[ STRING_64K ];
	char *ptr = html;

	if ( !document_html
	||   !document_head_html
	||   !document_head_close_tag
	||   !frameset_frame_prompt_tag
	||   !frameset_frame_table_tag
	||   !document_close_tag )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf(
		ptr,
		"%s\n%s\n%s\n",
		document_html,
		document_head_html,
		document_head_close_tag );

	if ( frameset_frame_menu )
	{
		ptr += sprintf(
			ptr,
			"<frameset cols=\"200,*\">\n"
			"%s\n"
			"<frameset rows=\"*,*\">\n"
			"%s\n"
			"%s\n"
			"</frameset>\n"
			"</frameset>\n",
			frameset_frame_menu->tag,
			frameset_frame_prompt_tag,
			frameset_frame_table_tag );
	}
	else
	{
		ptr += sprintf(
			ptr,
			"<frameset rows=\"*,*\">\n"
			"%s\n"
			"%s\n"
			"</frameset>\n",
			frameset_frame_prompt_tag,
			frameset_frame_table_tag );
	}

	ptr += sprintf(
		ptr,
		"%s\n",
		document_close_tag );

	return strdup( html );
}

FRAMESET_FRAME *frameset_frame_new(
		char *application_name,
		char *data_directory,
		char *session_key,
		char *frame_name,
		char *document_form_html )
{
	FRAMESET_FRAME *frameset_frame;

	if ( !application_name
	||   !data_directory
	||   !session_key
	||   !frame_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	frameset_frame = frameset_frame_calloc();

	frameset_frame->appaserver_link =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_link_new(
			application_http_prefix(
				application_ssl_support_boolean(
					application_name ) ),
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			application_server_address(),
	 		data_directory,
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
			appaserver_link_prompt->
			filename;

	frameset_frame->output_filename =
		frameset_frame->
			appaserver_link->
			appaserver_link_output->
			filename;

	if ( document_form_html )
	{
		if ( !string_file_write(
			frameset_frame->output_filename,
			document_form_html /* string */ ) )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: string_write_file(%s) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				frameset_frame->output_filename );
			exit( 1 );
		}
	}
	else
	{
		timlib_zap_file( frameset_frame->output_filename );
	}

	frameset_frame->tag =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		frameset_frame_tag(
			frame_name,
			frameset_frame->prompt_filename );

	return frameset_frame;
}

char *frameset_frame_tag(
		char *frame_name,
		char *prompt_filename )
{
	char tag[ 256 ];

	if ( !frame_name
	||   !prompt_filename )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: a parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		tag,
		sizeof ( tag ),
		"<frame name=\"%s\" src=\"%s\">",
		frame_name,
		prompt_filename );

	return strdup( tag );
}

char *frameset_blank_system_string(
			char *application_name,
			char *application_background_color,
			char *application_title_string,
			char *output_filename,
			char *appaserver_error_filename )
{
	char system_string[ 1024 ];

	if ( !application_name
	||   !application_background_color
	||   !output_filename
	||   !appaserver_error_filename )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"output_blank_screen.sh %s \"%s\" \"%s\" n > %s 2>>%s",
		application_name,
		application_background_color,
		(application_title_string)
			? application_title_string
			: "",
		output_filename,
		appaserver_error_filename );

	return strdup( system_string );
}

char *frameset_role_name(
			LIST *role_name_list,
			char *appaserver_user_default_role_name )
{
	if ( list_length( role_name_list ) == 1 )
		return (char *)list_first( role_name_list );
	else
	if ( appaserver_user_default_role_name )
		return appaserver_user_default_role_name;
	else
		return (char *)0;
}

char *frameset_filename(
			FRAMESET_FRAME *frameset_frame_menu,
			FRAMESET_FRAME *frameset_frame_prompt )
{
	if ( frameset_frame_menu )
		return frameset_frame_menu->output_filename;
	else
	if ( frameset_frame_prompt )
		return frameset_frame_prompt->output_filename;
	else
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: both frameset_frame_menu and frameset_frame_prompt are empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
}

char *frameset_current_frame(
		const char *frameset_prompt_frame,
		const char *frameset_table_frame,
		char *appaserver_form )
{
	if ( folder_form_prompt_boolean(
		FORM_APPASERVER_PROMPT,
		appaserver_form ) )
	{
		return (char *)frameset_prompt_frame;
	}
	else
	{
		return (char *)frameset_table_frame;
	}
}

char *frameset_prompt_target(
		char *datatype_name,
		pid_t process_id )
{
	static char target[ 64 ];

	if ( datatype_name )
	{
		sprintf(target,
			"frame_%s_%d",
			datatype_name,
			process_id );
	}
	else
	{
		sprintf(target,
			"frame_%d",
			process_id );
	}

	return target;
}

