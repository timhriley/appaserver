/* $APPASERVER_HOME/library/appaserver_link_file.c			*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "application.h"
#include "appaserver_library.h"
#include "appaserver_link_file.h"

APPASERVER_LINK_FILE *appaserver_link_file_new(
			char *http_prefix,
			char *server_address,
			boolean prepend_http_boolean,
			char *document_root_directory,
			char *filename_stem,
			char *application_name,
			pid_t process_id,
			char *session,
			char *extension )
{
	APPASERVER_LINK_FILE *h;

	h = (APPASERVER_LINK_FILE *)calloc( 1, sizeof( APPASERVER_LINK_FILE ) );
	if ( !h )
	{
		fprintf( stderr, 
			 "%s/%s(): Memory allocation error.\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	h->filename_stem = filename_stem;
	h->application_name = application_name;
	h->process_id = process_id;
	h->session = session;
	h->extension = extension;

	h->link_prompt =
		appaserver_link_prompt_new(
			http_prefix,
			server_address,
			prepend_http_boolean );

	h->output_file =
		appaserver_link_output_file_new(
			document_root_directory );

	return h;

} /* appaserver_link_file_new() */

LINK_PROMPT *appaserver_link_prompt_new(
			char *http_prefix,
			char *server_address,
			boolean prepend_http_boolean )
{
	LINK_PROMPT *h;

	h = (LINK_PROMPT *)calloc( 1, sizeof( LINK_PROMPT ) );
	if ( !h )
	{
		fprintf( stderr, 
			 "%s/%s(): Memory allocation error.\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	h->http_prefix = http_prefix;
	h->server_address = server_address;
	h->prepend_http_boolean = prepend_http_boolean;
	return h;

} /* appaserver_link_prompt_new() */

LINK_OUTPUT_FILE *appaserver_link_output_file_new(
			char *document_root_directory )
{
	LINK_OUTPUT_FILE *h;

	h = (LINK_OUTPUT_FILE *)calloc( 1, sizeof( LINK_OUTPUT_FILE ) );
	if ( !h )
	{
		fprintf( stderr, 
			 "%s/%s(): Memory allocation error.\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	h->document_root_directory = document_root_directory;
	return h;

} /* appaserver_link_output_file_new() */

char *appaserver_link_get_abbreviated_output_filename(
		char *filename_stem,
		char *begin_date_string,
		char *end_date_string,
		pid_t process_id,
		char *session,
		char *extension )
{
	char output_filename[ 512 ];

	if ( !filename_stem || !*filename_stem )
	{
		fprintf( stderr, 
			 "%s/%s(): empty filename_stem.\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	strcpy(  output_filename,
		 appaserver_link_get_tail_half(
			(char *)0 /* application_name */,
			filename_stem,
			begin_date_string,
			end_date_string,
			process_id,
			session,
			extension ) );

	return strdup( output_filename );

} /* appaserver_link_get_abbreviated_output_filename() */

char *appaserver_link_get_output_filename(
		char *document_root_directory,
		char *application_name,
		char *filename_stem,
		char *begin_date_string,
		char *end_date_string,
		pid_t process_id,
		char *session,
		char *extension )
{
	char output_filename[ 512 ];

	if ( !filename_stem || !*filename_stem )
	{
		fprintf( stderr, 
			 "%s/%s(): empty filename_stem.\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	sprintf( output_filename,
		 "%s%s",
		 document_root_directory,
		 appaserver_link_get_tail_half(
			application_name,
			filename_stem,
			begin_date_string,
			end_date_string,
			process_id,
			session,
			extension ) );

	return strdup( output_filename );

} /* appaserver_link_get_output_filename() */

char *appaserver_link_get_link_prompt(
		boolean prepend_http_boolean,
		char *http_prefix,
		char *server_address,
		char *application_name,
		char *filename_stem,
		char *begin_date_string,
		char *end_date_string,
		pid_t process_id,
		char *session,
		char *extension )
{
	char link_prompt[ 512 ];
	char link_half[ 128 ];

	*link_half = '\0';

	if ( prepend_http_boolean )
	{
		if ( server_address && *server_address )
		{
			sprintf(link_half,
			 	"%s://%s",
			 	http_prefix,
			 	server_address );
		}
		else
		{
			sprintf(link_half,
			 	"%s://",
			 	http_prefix );
		}
	}

	sprintf( link_prompt,
		 "%s%s",
		 link_half,
		 appaserver_link_get_tail_half(
			application_name,
			filename_stem,
			begin_date_string,
			end_date_string,
			process_id,
			session,
			extension ) );

	return strdup( link_prompt );

} /* appaserver_link_get_link_prompt() */

char *appaserver_link_get_tail_half(
		char *application_name,
		char *filename_stem,
		char *begin_date_string,
		char *end_date_string,
		pid_t process_id,
		char *session,
		char *extension )
{
	static char tail_half[ 512 ];
	char application_part[ 128 ];
	char date_part[ 128 ];
	char key_part[ 128 ];

	*date_part = '\0';

	if ( begin_date_string && *begin_date_string )
	{
		sprintf( date_part,
			 "_%s",
			 begin_date_string );
	}

	if ( end_date_string && *end_date_string )
	{
		sprintf( date_part + strlen( date_part ),
		 	"_%s",
		 	end_date_string );
	}

	if ( session && *session )
	{
		sprintf( key_part,
			 "_%s",
			 session );
	}
	else
	if ( process_id )
	{
		sprintf( key_part,
			 "_%d",
			 process_id );
	}
	else
	{
		*key_part = '\0';
	}

	*application_part = '\0';

	if ( application_name )
	{
		sprintf( application_part,
		 	"/%s/%s/%s/",
			 	APPASERVER_KEY,
		 	application_name,
		 	APPASERVER_DATA_KEY );
	}

	sprintf( tail_half,
		 "%s%s%s%s.%s",
		 application_part,
		 filename_stem,
		 (*date_part) ? date_part : "",
		 key_part,
		 (extension) ? extension : "null" );

	return tail_half;

} /* appaserver_link_get_tail_half() */

char *appaserver_link_source_directory(
		char *document_root_directory,
		char *application_name )
{
	char source_directory[ 128 ];

	sprintf( source_directory,
		 "%s/%s/%s/data",
		 document_root_directory,
		 APPASERVER_KEY,
		 application_name );

	return strdup( source_directory );

} /* appaserver_link_get_source_directory() */

void appaserver_link_get_pid_filename(
				char **output_filename,
				char **prompt_filename,
				char *application_name,
				char *document_root_directory,
				pid_t pid,
				char *process_name,
				char *extension )
{
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
			application_get_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_get_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			process_name /* FILENAME_STEM */,
			application_name,
			pid,
			(char *)0 /* session */,
			extension );

	*output_filename =
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

	*prompt_filename =
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

} /* appaserver_link_get_pid_filename() */

