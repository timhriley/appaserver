/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/appaserver_link.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "String.h"
#include "application.h"
#include "column.h"
#include "appaserver_error.h"
#include "appaserver_link.h"

APPASERVER_LINK *appaserver_link_calloc( void )
{
	APPASERVER_LINK *appaserver_link;

	if ( ! ( appaserver_link =
			calloc( 1, sizeof ( APPASERVER_LINK ) ) ) )
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
		char *data_directory,
		char *filename_stem,
		char *application_name,
		pid_t process_id,
		char *session_key,
		char *begin_date_string,
		char *end_date_string,
		char *extension )
{
	APPASERVER_LINK *appaserver_link;

	if ( !data_directory
	||   !application_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !process_id
	&&   !session_key )
	{
		char message[ 128 ];

		sprintf(message, "both process_id and session_key are empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	appaserver_link = appaserver_link_calloc();

	if ( !filename_stem ) filename_stem = "stem";

	end_date_string =
		/* -------------------------------------- */
		/* Returns end_date_string or heap memory */
		/* -------------------------------------- */
		appaserver_link_end_date_string(
			end_date_string );

	appaserver_link->http_prefix = http_prefix;
	appaserver_link->server_address = server_address;
	appaserver_link->appaserver_parameter_data_directory = data_directory;
	appaserver_link->server_address = server_address;
	appaserver_link->filename_stem = filename_stem;
	appaserver_link->application_name = application_name;
	appaserver_link->process_id = process_id;
	appaserver_link->session_key = session_key;
	appaserver_link->begin_date_string = begin_date_string;
	appaserver_link->end_date_string = end_date_string;
	appaserver_link->extension = extension;

	appaserver_link->appaserver_link_prompt =
		appaserver_link_prompt_new(
			APPASERVER_LINK_PROMPT_DIRECTORY,
			http_prefix,
			server_address,
			application_name,
			filename_stem,
			begin_date_string,
			end_date_string,
			process_id,
			session_key,
			extension );

	appaserver_link->filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_filename(
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
		appaserver_link_tail_half(
			application_name,
			appaserver_link->filename );

	appaserver_link->appaserver_link_output =
		appaserver_link_output_new(
			data_directory,
			appaserver_link->tail_half );

	return appaserver_link;
}

APPASERVER_LINK_PROMPT *appaserver_link_prompt_calloc( void )
{
	APPASERVER_LINK_PROMPT *appaserver_link_prompt;

	if ( ! ( appaserver_link_prompt =
			calloc( 1, sizeof ( APPASERVER_LINK_PROMPT ) ) ) )
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
		char *appaserver_link_prompt_directory,
		char *http_prefix,
		char *server_address,
		char *application_name,
		char *filename_stem,
		char *begin_date_string,
		char *end_date_string,
		pid_t process_id,
		char *session_key,
		char *extension )
{
	APPASERVER_LINK_PROMPT *appaserver_link_prompt;

	if ( !application_name
	||   !filename_stem )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !process_id
	&&   !session_key )
	{
		char message[ 128 ];

		sprintf(message, "both process_id and session_key are empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	appaserver_link_prompt = appaserver_link_prompt_calloc();

	appaserver_link_prompt->link_half =
		appaserver_link_prompt_link_half(
			appaserver_link_prompt_directory,
			http_prefix,
			server_address );

	appaserver_link_prompt->appaserver_link_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_filename(
			filename_stem,
			begin_date_string,
			end_date_string,
			process_id,
			session_key,
			extension );

	appaserver_link_prompt->appaserver_link_tail_half =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_tail_half(
			application_name,
			appaserver_link_prompt->
				appaserver_link_filename );

	appaserver_link_prompt->filename =
		appaserver_link_prompt_filename(
			appaserver_link_prompt->link_half,
			appaserver_link_prompt->appaserver_link_tail_half );

	return appaserver_link_prompt;
}

APPASERVER_LINK_OUTPUT *appaserver_link_output_calloc( void )
{
	APPASERVER_LINK_OUTPUT *appaserver_link_output;

	if ( ! ( appaserver_link_output =
			calloc( 1, sizeof ( APPASERVER_LINK_OUTPUT ) ) ) )
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
		char *data_directory,
		char *tail_half )
{
	APPASERVER_LINK_OUTPUT *appaserver_link_output;

	if ( !data_directory
	||   !tail_half )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	appaserver_link_output = appaserver_link_output_calloc();

	appaserver_link_output->filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_output_filename(
			data_directory,
			tail_half );

	return appaserver_link_output;
}

char *appaserver_link_output_filename(
		char *data_directory,
		char *tail_half )
{
	char filename[ 512 ];

	if ( !data_directory
	||   !tail_half )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(filename,
		"%s%s",
		data_directory,
		tail_half );

	return strdup( filename );
}

char *appaserver_link_prompt_filename(
			char *link_half,
			char *tail_half )
{
	char filename[ 512 ];

	if ( !link_half
	||   !tail_half )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(filename,
		"%s%s",
		link_half,
		tail_half );

	return strdup( filename );
}

char *appaserver_link_filename(
		char *filename_stem,
		char *begin_date_string,
		char *end_date_string,
		pid_t process_id,
		char *session_key,
		char *extension )
{
	char date_part[ 128 ];
	char key_part[ 128 ];
	char filename[ 512 ];

	if ( !filename_stem )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !process_id
	&&   !session_key )
	{
		char message[ 128 ];

		sprintf(message, "both process_id and session_key are empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

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

	if ( session_key )
	{
		sprintf( key_part,
			 "_%s",
			 session_key );
	}
	else
	{
		sprintf( key_part,
			 "_%d",
			 process_id );
	}

	sprintf( filename,
		 "%s%s%s.%s",
		 filename_stem,
		 (*date_part) ? date_part : "",
		 key_part,
		 (extension) ? extension : "null" );

	return strdup( filename );
}

char *appaserver_link_tail_half(
		char *application_name,
		char *appaserver_link_filename )
{
	char tail_half[ 1024 ];

	if ( !application_name
	||   !appaserver_link_filename )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(tail_half,
		"/%s/%s",
		application_name,
		appaserver_link_filename );

	return strdup( tail_half );
}

char *appaserver_link_working_directory(
		char *data_directory,
		char *application_name )
{
	char working_directory[ 128 ];

	sprintf( working_directory,
		 "%s/%s",
		 data_directory,
		 application_name );

	return strdup( working_directory );
}

void appaserver_link_pid_filename(
		char **prompt_filename,
		char **output_filename,
		char *application_name,
		char *data_directory,
		pid_t process_id,
		char *process_name,
		char *extension )
{
	APPASERVER_LINK *appaserver_link;

	if ( !application_name
	||   !data_directory )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	appaserver_link =
		appaserver_link_new(
			application_http_prefix(
				application_ssl_support_boolean(
					application_name ) ),
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			application_server_address(),
			data_directory,
			process_name /* FILENAME_STEM */,
			application_name,
			process_id,
			(char *)0 /* session_key */,
			(char *)0 /* begin_date_string */,
			(char *)0 /* end_date_string */,
			extension );

	*prompt_filename = appaserver_link->appaserver_link_prompt->filename;
	*output_filename = appaserver_link->appaserver_link_output->filename;
}

char *appaserver_link_prompt_link_half(
		char *appaserver_link_prompt_directory,
		char *http_prefix,
		char *server_address )
{
	char link_half[ 128 ];

	if ( http_prefix
	&&   server_address )
	{
		sprintf(link_half,
		 	"%s://%s/%s",
		 	http_prefix,
		 	server_address,
			appaserver_link_prompt_directory );
	}
	else
	if ( http_prefix )
	{
		sprintf(link_half,
		 	"%s://%s",
		 	http_prefix,
			appaserver_link_prompt_directory );
	}
	else
	{
		strcpy( link_half,
			appaserver_link_prompt_directory );
	}

	return strdup( link_half );
}

char *appaserver_link_anchor_html(
		char *prompt_filename,
		char *target_window,
		char *prompt_message )
{
	char html[ 256 ];

	if ( !prompt_filename
	||   !prompt_message )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(html,
		"<br><a href=\"%s\" target=\"%s\">%s</a>\n",
		prompt_filename,
		(target_window) ? target_window : "_new",
		prompt_message );

	return strdup( html );
}

char *appaserver_link_end_date_string( char *end_date_string )
{
	if ( string_character_exists(
		end_date_string /* datum */,
		' ' ) )
	{
		char buffer[ 128 ];

		column( buffer, 0, end_date_string );
		return strdup( buffer );
	}
	else
	{
		return end_date_string;
	}
}

