/* frameset.c 								*/
/* -------------------------------------------------------------------- */
/* This is the appaserver frameset ADT.					*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "frameset.h"
#include "application.h"
#include "appaserver_library.h"

FRAMESET *frameset_new( char *frameset_name, char *form_name )
{
	FRAMESET *f = (FRAMESET *)calloc( 1, sizeof( FRAMESET ) );
	f->frameset_name = frameset_name;
	f->form_name = form_name;
	f->dictionary_string = "";
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

char *frameset_get_prompt_destination_filename( FRAMESET *frameset )
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

char *frameset_get_create_destination_filename( FRAMESET *frameset )
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

char *frameset_get_prompt_bottom_filename( FRAMESET *frameset )
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

char *frameset_get_create_bottom_filename( FRAMESET *frameset)
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

void frameset_set_dictionary_string( 	FRAMESET *frameset,
					char *dictionary_string )
{
	frameset->dictionary_string =  dictionary_string;
}

void frameset_set_output_bottom_frame( FRAMESET *frameset )
{
	frameset->output_bottom_frame = 1;
}

void frameset_set_target_frame( FRAMESET *frameset, 
				char *target_frame )
{
	frameset->target_frame = strdup( target_frame );
}


void frameset_set_sys_string(	FRAMESET *frameset,
				char *sys_string )
{
	frameset->sys_string = strdup( sys_string );
}

void frameset_set_application_name(	FRAMESET *frameset,
					char *application_name )
{
	frameset->application_name = strdup( application_name );
}

void frameset_set_session(	FRAMESET *frameset,
				char *session )
{
	frameset->session = strdup( session );
}

void frameset_set_appaserver_mount_point(
				FRAMESET *frameset,
				char *appaserver_mount_point )
{
	frameset->appaserver_mount_point = appaserver_mount_point;
}
