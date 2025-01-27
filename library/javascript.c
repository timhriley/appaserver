/* $APPASERVER_HOME/library/javascript.c		   		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <ctype.h>
#include "String.h"
#include "piece.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "environ.h"
#include "application.h"
#include "document.h"
#include "javascript.h"

JAVASCRIPT *javascript_calloc( void )
{
	JAVASCRIPT *javascript;

	if ( ! ( javascript =
			calloc( 1, sizeof( JAVASCRIPT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return javascript;
}

JAVASCRIPT *javascript_new(
		char *javascript_filename,
		char *document_root,
		char *application_relative_source_directory )
{
	JAVASCRIPT *javascript;

	if ( !javascript_filename
	||   !document_root
	||   !application_relative_source_directory )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	javascript = javascript_calloc();

	javascript->script_tag =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		javascript_script_tag(
			javascript_filename,
			document_root,
			application_relative_source_directory );

	if ( !javascript->script_tag )
	{
		char message[ 1024 ];

		sprintf(message,
			"javascript_script_tag(%s,%s,%s) returned empty.",
			javascript_filename,
			document_root,
			application_relative_source_directory );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return javascript;
}

char *javascript_script_tag(
		char *javascript_filename,
		char *document_root,
		char *application_relative_source_directory )
{
	char *home_source;
	char *script_tag;

	if ( !javascript_filename
	||   !document_root
	||   !application_relative_source_directory )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	home_source =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		javascript_home_source(
			javascript_filename,
			document_root );

	if ( application_file_exists_boolean( home_source ) )
	{
		script_tag =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			javascript_home_script_tag( javascript_filename );
	}
	else
	{
		script_tag =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			javascript_application_relative_script_tag(
				javascript_filename,
				document_root,
				application_relative_source_directory );
	}

	return script_tag;
}

char *javascript_application_relative_script_tag(
		char *javascript_filename,
		char *document_root,
		char *application_relative_source_directory )
{
	int p;
	char relative_source_directory[ 128 ];
	char *relative_source;
	char *script_tag = {0};

	if ( !javascript_filename
	||   !document_root
	||   !application_relative_source_directory )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	for(	p = 0;
		/* ------------------------------------- */
		/* Returns null if not enough delimiters */
		/* ------------------------------------- */
		piece(	relative_source_directory,
			':',
			application_relative_source_directory,
			p );
		p++ )
	{
		relative_source =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			javascript_relative_source(
				javascript_filename,
				document_root,
				relative_source_directory );

		if ( application_file_exists_boolean( relative_source ) )
		{
			script_tag =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				javascript_relative_script_tag(
					javascript_filename,
					relative_source_directory );

			break;
		}
	}

	return script_tag;
}

char *javascript_replace(
		char *post_change_javascript,
		char *state,
		int row_number )
{
	static char javascript[ STRING_4K ];

	if ( !post_change_javascript || !*post_change_javascript )
		return (char *)0;

	if ( strlen( post_change_javascript ) + 16 >= STRING_4K )
	{
		char message[ 128 ];

		sprintf(message,
			STRING_OVERFLOW_TEMPLATE,
			STRING_4K );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	strcpy( javascript, post_change_javascript );

	if ( state && *state )
	{
		javascript_replace_state(
			javascript,
			state );
	}

	javascript_replace_row(
		javascript,
		row_number );

	return javascript;
}

void javascript_replace_state(
		char *post_change_javascript,
		char *state )
{
	if ( post_change_javascript )
	{
		string_search_replace(
			post_change_javascript,
			"$state",
			state );
	}
}

void javascript_replace_row(
		char *post_change_javascript,
		int row_number )
{
	string_search_replace(
		post_change_javascript,
		"$row",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		string_itoa( row_number ) );
}

char *javascript_post_change_append(
		char *original_post_change_javascript,
		char *post_change_javascript,
		boolean place_first )
{
	char new_post_change_javascript[ 65536 ];

	*new_post_change_javascript = '\0';

	if ( original_post_change_javascript
	&&   *original_post_change_javascript
	&&   post_change_javascript
	&&   *post_change_javascript )
	{
		if ( place_first )
		{
			sprintf(	new_post_change_javascript,
					"%s && %s",
					post_change_javascript,
					original_post_change_javascript );
		}
		else
		{
			sprintf(	new_post_change_javascript,
					"%s && %s",
					original_post_change_javascript,
					post_change_javascript );
		}
	}
	else
	if ( post_change_javascript
	&&   *post_change_javascript )
	{
		string_strcpy(	new_post_change_javascript,
				post_change_javascript,
				65536 );
	}
	else
	if ( original_post_change_javascript
	&&   *original_post_change_javascript )
	{
		string_strcpy(	new_post_change_javascript,
				original_post_change_javascript,
				65536 );
	}

	if ( original_post_change_javascript )
	{
		free( original_post_change_javascript );
	}

	return strdup( new_post_change_javascript );
}

char *javascript_include_string(
		char *application_name,
		LIST *javascript_filename_list )
{
	char string[ STRING_64K ];
	char *ptr = string;
	char *document_root;
	char *relative_source_directory;
	JAVASCRIPT *javascript;

	ptr += sprintf(
		ptr,
		"%s",
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		document_head_javascript_string() );

	document_root =
		/* ----------------------------------------- */
		/* Returns component of APPASERVER_PARAMETER */
		/* ----------------------------------------- */
		appaserver_parameter_document_root();

	relative_source_directory =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		application_relative_source_directory(
			application_name );

	if ( list_rewind( javascript_filename_list ) )
	do {
		javascript =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			javascript_new(
				(char *)list_get(
					javascript_filename_list )
					/* javascript_filename */,
				document_root,
				relative_source_directory );

		ptr += sprintf(
			ptr,
			"\n%s",
			javascript->script_tag );

	} while ( list_next( javascript_filename_list ) );

	return strdup( string );
}

char *javascript_home_source(
		char *javascript_filename,
		char *document_root )
{
	static char home_source[ 256 ];

	if ( !javascript_filename
	||   !document_root )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );

		/* Stub */
		/* ---- */
		exit( 1 );
	}

	sprintf(home_source,
		"%s/appaserver_home/javascript/%s",
		document_root,
		javascript_filename );

	return home_source;
}

char *javascript_relative_source(
		char *javascript_filename,
		char *document_root,
		char *relative_source_directory )
{
	static char relative_source[ 256 ];

	if ( !javascript_filename
	||   !document_root
	||   !relative_source_directory )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(relative_source,
		"%s/appaserver_home/%s/%s",
		document_root,
		relative_source_directory,
		javascript_filename );

	return relative_source;
}

char *javascript_open_tag( void )
{
	return "<script language=Javascript1.2";
}

char *javascript_close_tag( void )
{
	return "></script>";
}

char *javascript_relative_script_tag(
		char *javascript_filename,
		char *relative_source_directory )
{
	char script_tag[ 256 ];

	if ( !javascript_filename
	||   !relative_source_directory )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(script_tag,
		"%s src=\"/appaserver_home/%s/%s\"%s",
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		javascript_open_tag(),
		relative_source_directory,
		javascript_filename,
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		javascript_close_tag() );

	return strdup( script_tag );
}

char *javascript_home_script_tag( char *javascript_filename )
{
	char script_tag[ 256 ];

	if ( !javascript_filename )
	{
		char message[ 128 ];

		sprintf(message, "javascript_filename is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(script_tag,
		"%s src=\"/appaserver_home/javascript/%s\"%s",
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		javascript_open_tag(),
		javascript_filename,
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		javascript_close_tag() );

	return strdup( script_tag );
}

