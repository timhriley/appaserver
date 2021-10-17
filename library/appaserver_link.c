/* $APPASERVER_HOME/library/appaserver_link.c				*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "application.h"
#include "appaserver_library.h"
#include "appaserver_link.h"

APPASERVER_LINK *appaserver_link_calloc( void )
{
	APPASERVER_LINK *appaserver_link;

	if ( ! ( appaserver_link =
			calloc( 1, sizeof( APPASERVER_LINK ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return appaserver_link;
}

APPASERVER_LINK *appaserver_link_new(
			char *http_prefix,
			char *server_address,
			boolean prepend_http,
			char *document_root_directory,
			char *filename_stem,
			char *application_name,
			pid_t process_id,
			char *session_key,
			char *begin_date_string,
			char *end_date_string,
			char *extension )
{
	APPASERVER_LINK *appaserver_link = appaserver_link_calloc();

	if ( !filename_stem || !*filename_stem )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: filename_stem is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	appaserver_link->http_prefix = http_prefix;
	appaserver_link->server_address = server_address;
	appaserver_link->prepend_http = prepend_http;
	appaserver_link->document_root_directory = document_root_directory;
	appaserver_link->filename_stem = filename_stem;
	appaserver_link->application_name = application_name;
	appaserver_link->process_id = process_id;
	appaserver_link->session_key = session_key;
	appaserver_link->begin_date_string = begin_date_string;
	appaserver_link->end_date_string = end_date_string;
	appaserver_link->extension = extension;

	appaserver_link->prompt =
		appaserver_link_prompt_new(
			http_prefix,
			server_address,
			prepend_http,
			application_name,
			filename_stem,
			begin_date_string,
			end_date_string,
			process_id,
			session_key,
			extension );

	appaserver_link->tail_half =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_output_tail_half(
			application_name,
			filename_stem,
			begin_date_string,
			end_date_string,
			process_id,
			session_key,
			extension );

	appaserver_link->output =
		appaserver_link_output_new(
			document_root_directory,
			appaserver_link->tail_half );

	return appaserver_link;
}

APPASERVER_LINK_PROMPT *appaserver_link_prompt_calloc( void )
{
	APPASERVER_LINK_PROMPT *appaserver_link_prompt;

	if ( ! ( appaserver_link_prompt =
			calloc( 1, sizeof( APPASERVER_LINK_PROMPT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return appaserver_link_prompt;
}

APPASERVER_LINK_PROMPT *appaserver_link_prompt_new(
			char *http_prefix,
			char *server_address,
			boolean prepend_http,
			char *application_name,
			char *filename_stem,
			char *begin_date_string,
			char *end_date_string,
			pid_t process_id,
			char *session_key,
			char *extension )
{
	APPASERVER_LINK_PROMPT *appaserver_link_prompt =
		appaserver_link_prompt_calloc();

	appaserver_link_prompt->link_half =
		appaserver_link_prompt_link_half(
			prepend_http,
			http_prefix,
			server_address );

	appaserver_link_prompt->filename =
		appaserver_link_prompt_filename(
			appaserver_link_prompt->link_half,
			application_name,
			filename_stem,
			begin_date_string,
			end_date_string,
			process_id,
			session_key,
			extension );

	return appaserver_link_prompt;
}

APPASERVER_LINK_OUTPUT *appaserver_link_output_calloc( void )
{
	APPASERVER_LINK_OUTPUT *appaserver_link_output;

	if ( ! ( appaserver_link_output =
			calloc( 1, sizeof( APPASERVER_LINK_OUTPUT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return appaserver_link_output;
}

APPASERVER_LINK_OUTPUT *appaserver_link_output_new(
			char *document_root_directory,
			char *tail_half )
{
	APPASERVER_LINK_OUTPUT *appaserver_link_output =
		appaserver_link_output_calloc();

	appaserver_link_output->filename =
		appaserver_link_output_filename(
			document_root_directory,
			tail_half );

	return appaserver_link_output;
}

char *appaserver_link_output_abbreviated_filename(
			char *filename_stem,
			char *begin_date_string,
			char *end_date_string,
			pid_t process_id,
			char *session_key,
			char *extension )
{
	if ( !filename_stem || !*filename_stem )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: filename_stem is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}


	return
	/* --------------------- */
	/* Returns heap memory */
	/* --------------------- */
	appaserver_link_output_tail_half(
		(char *)0 /* application_name */,
		filename_stem,
		begin_date_string,
		end_date_string,
		process_id,
		session_key,
		extension );
}

char *appaserver_link_output_filename(
			char *document_root_directory,
			char *tail_half )
{
	char output_filename[ 512 ];

	if ( !document_root_directory || !*document_root_directory )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: document_root_directory is  empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(output_filename,
		"%s%s",
		document_root_directory,
		tail_half );

	return strdup( output_filename );
}

char *appaserver_link_prompt_filename(
			char *link_half,
			char *application_name,
			char *filename_stem,
			char *begin_date_string,
			char *end_date_string,
			pid_t process_id,
			char *session_key,
			char *extension )
{
	char filename[ 512 ];
	char *tail_half;

	sprintf(filename,
		"%s%s",
		link_half,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		( tail_half =
			appaserver_link_output_tail_half(
				application_name,
				filename_stem,
				begin_date_string,
				end_date_string,
				process_id,
				session_key,
				extension ) ) );

	free( tail_half );
	return strdup( filename );
}

char *appaserver_link_output_tail_half(
			char *application_name,
			char *filename_stem,
			char *begin_date_string,
			char *end_date_string,
			pid_t process_id,
			char *session_key,
			char *extension )
{
	char tail_half[ 512 ];
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

	if ( session_key && *session_key )
	{
		sprintf( key_part,
			 "_%s",
			 session_key );
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

	return strdup( tail_half );
}

char *appaserver_link_working_directory(
		char *document_root_directory,
		char *application_name )
{
	char working_directory[ 128 ];

	sprintf( working_directory,
		 "%s/%s/%s/data",
		 document_root_directory,
		 APPASERVER_KEY,
		 application_name );

	return strdup( working_directory );
}

void appaserver_link_pid_filename(
			char **prompt_filename,
			char **output_filename,
			char *application_name,
			char *document_root_directory,
			pid_t process_id,
			char *process_name,
			char *extension )
{
	APPASERVER_LINK *appaserver_link;

	appaserver_link =
		appaserver_link_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			process_name /* FILENAME_STEM */,
			application_name,
			process_id,
			(char *)0 /* session_key */,
			(char *)0 /* begin_date_string */,
			(char *)0 /* end_date_string */,
			extension );

	*output_filename = appaserver_link->output->filename;

	*prompt_filename = appaserver_link->prompt->filename;
}

char *appaserver_link_prompt_link_half(
			boolean prepend_http,
			char *http_prefix,
			char *server_address )
{
	char link_half[ 128 ];

	if ( prepend_http )
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
	else
	{
		*link_half = '\0';
	}

	return strdup( link_half );
}
