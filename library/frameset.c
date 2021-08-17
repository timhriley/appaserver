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

FRAMESET *frameset_new(	char *application_name,
			char *login_name,
			char *session )
{
	FRAMESET *f = frameset_calloc();

	f->frameset_name = frameset_name;
	f->form_name = form_name;
	f->application_name = application_name;
	f->login_name = login_name;
	f->session = session;

	f->appaserver_parameter_file =
		appaserver_parameter_file_new();

	f->frameset_link_file =
		frameset_link_file_new(
			application_name,
			appaserver_parameter_file->
				document_root,
			login_name,
			session );
	return f;
}

void frameset_set_parameters( 	FRAMESET *frameset,
				char *login_name, 
				char *application_name,
				char *session, 
				char *folder_or_process,
				char *role,
				char *state, 
				char *appaserver_mount_point, 
				char *error_file )
{
	frameset->login_name = login_name;
	frameset->application_name = application_name;
	frameset->session = session;
	frameset->folder_or_process = folder_or_process;
	frameset->role = role;
	frameset->state = state;
	frameset->appaserver_mount_point = appaserver_mount_point;
	frameset->error_file = error_file;
}

void frameset_remove_files( FRAMESET *frameset )
{
	char sys_string[ 1024 ];
	char *filename;

	filename = frameset_get_create_destination_filename( frameset );

	sprintf( sys_string, "/bin/rm -f %s", filename );
	if ( system( sys_string ) ){};

	if ( frameset->output_bottom_frame )
	{
		filename = frameset_get_create_bottom_filename( frameset );

		sprintf( sys_string, "/bin/rm -f %s", filename );
		if ( system( sys_string ) ){};
	}
}

void frameset_output_by_frameset( FRAMESET *frameset )
{
	printf(
"<frameset rows=\"*, *\">						\n"
"<frame name=\"%s\" src=\"%s\">						\n"
"<frame name=\"%s\" src=\"%s\">						\n"
"</frameset>								\n",
		TOP_FRAME_NAME,
		frameset_get_prompt_destination_filename( frameset ),
		frameset->target_frame,
		frameset_get_prompt_bottom_filename( frameset ) );
}

void frameset_output_by_cat( FRAMESET *frameset )
{
	char sys_string[ 1024 ];

	sprintf( sys_string, 
		 "cat %s",
		 frameset_get_create_destination_filename( frameset ) );
	if ( system( sys_string ) ){};

}

char *frameset_prompt_destination_filename( FRAMESET *frameset )
{
	if ( !frameset->prompt_destination_filename )
	{
		char filename[ 1024 ];

		sprintf( filename, 
		 	"/%s/%s_%s.html",
		 	frameset->application_name, 
		 	frameset->frameset_name, 
		 	frameset->session );
		frameset->prompt_destination_filename = strdup( filename );
	}
	return frameset->prompt_destination_filename;
}

char *frameset_create_destination_filename( FRAMESET *frameset )
{
	if ( !frameset->create_destination_filename )
	{
		char filename[ 1024 ];

		sprintf( filename, 
		 	"%s/%s/%s_%s.html",
		 	frameset->appaserver_mount_point, 
		 	frameset->application_name, 
		 	frameset->frameset_name, 
		 	frameset->session );
		frameset->create_destination_filename = strdup( filename );
	}
	return frameset->create_destination_filename;
}

char *frameset_prompt_bottom_filename( FRAMESET *frameset )
{
	char filename[ 1024 ];

	if ( !frameset->prompt_bottom_filename )
	{
		sprintf( filename, 
		 	"/%s/bottom_%s.html",
		 	frameset->application_name, 
		 	frameset->session );
		frameset->prompt_bottom_filename = strdup( filename );
	}
	return frameset->prompt_bottom_filename;
}

char *frameset_create_bottom_filename( FRAMESET *frameset )
{
	char filename[ 1024 ];

	if ( !frameset->create_bottom_filename )
	{
		sprintf( filename, 
		 	"%s/%s/bottom_%s.html",
		 	frameset->appaserver_mount_point, 
		 	frameset->application_name, 
		 	frameset->session );

		frameset->create_bottom_filename = strdup( filename );
	}
	return frameset->create_bottom_filename;
}

FRAMESET_LINK_FILE *frameset_link_file_calloc( void )
{
	FRAMESET_LINK_FILE *frameset_link_file;

	if ( ! ( frameset_link_file =
			calloc( 1, sizeof( FRAMESET_LINK_FILE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return frameset_link_file;
}

FRAMESET_LINK_FILE *frameset_link_file_new(
			char *application_name,
			char *document_root_directory,
			char *login_name,
			char *session )
{
	APPASERVER_LINK_FILE *appaserver_link_file;

	FRAMESET_LINK_FILE *frameset_link_file =
		frameset_link_file_calloc();

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		document_root_directory,
			(char *)0 /* filename_stem */,
			application_name,
			0 /* process_id */,
			session,
			"html" );

	appaserver_link_file->filename_stem = MENU_FRAME;

	frameset_link_file->menu_frame_prompt_filename =
		appaserver_link_prompt_filename(
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

	frameset_link_file->menu_frame_create_filename =
		appaserver_link_output_filename(
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

	if ( !zap_file( frameset_link_file->menu_frame_create_filename ) )
	{
		char msg[ 1024 ];

		sprintf(msg,
			"ERROR in %s/%s()/%d: cannot write to file (%s)",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			frameset_link_file->menu_frame_create_filename );

		appaserver_output_error_message(
			application_name,
			msg,
			login_name );
		exit( 1 );
	}

	appaserver_link_file->filename_stem = PROMPT_FRAME;

	frameset_link_file->prompt_frame_prompt_filename =
		appaserver_link_prompt_filename(
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

	frameset_link_file->prompt_frame_create_filename =
		appaserver_link_output_filename(
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

	frameset_link_file->edit_frame_prompt_filename =
		appaserver_link_prompt_filename
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

	frameset_link_file->edit_frame_create_filename =
		appaserver_link_output_filename(
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

	return frameset_link_file;
}
