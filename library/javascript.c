/* $APPASERVER_HOME/library/javascript.c		   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <ctype.h>
#include "timlib.h"
#include "String.h"
#include "piece.h"
#include "appaserver_library.h"
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
			char *document_root_directory,
			char *application_relative_source_directory )
{
	JAVASCRIPT *javascript;

	if ( !javascript_filename
	||   !document_root_directory
	||   !application_relative_source_directory )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: a parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	javascript = javascript_calloc();

	javascript->javascript_html =
		javascript_html(
			javascript_filename,
			document_root_directory,
			application_relative_source_directory );

	if ( !javascript->javascript_html ) return (JAVASCRIPT *)0;

	javascript->javascript_source =
		javascript_source(
			javascript->javascript_html,
			document_root_directory );

	if ( !javascript->javascript_source ) return (JAVASCRIPT *)0;

	return javascript;
}

char *javascript_html(
			char *javascript_filename,
			char *document_root_directory,
			char *application_relative_source_directory )
{
	char javascript_home_source[ 256 ];
	char javascript_relative_source[ 256 ];
	char html[ 256 ];
	char relative_source_directory[ 128 ];
	int index;

	sprintf(javascript_home_source,
		"%s/appaserver/javascript/%s",
		document_root_directory,
		javascript_filename );

	if ( timlib_file_exists( javascript_home_source ) )
	{
		sprintf(html,
			"/appaserver/javascript/%s",
			javascript_filename );

		return strdup( html );
	}

	for(	index = 0;
		piece(	relative_source_directory,
			PATH_DELIMITER,
			application_relative_source_directory,
			index );
		index++ )
	{
		sprintf(javascript_relative_source,
			"%s/appaserver/%s/%s",
			document_root_directory,
			relative_source_directory,
			javascript_filename );

		if ( timlib_file_exists( javascript_relative_source ) )
		{
			sprintf(html,
				"/appaserver/%s/%s",
				relative_source_directory,
				javascript_filename );

			return strdup( html );
		}
	}
	return (char *)0;
}

char *javascript_source(
			char *javascript_html,
			char *document_root_directory )
{
	char source[ 256 ];

	sprintf(source,
		"%s/%s",
		document_root_directory,
		javascript_html );

	return strdup( source );
}

void javascript_replace_state(
			char *post_change_javascript,
			char *state )
{
	search_replace_string(
		post_change_javascript,
		"$state",
		state );
}

void javascript_replace_row(
			char *post_change_javascript,
			char *row )
{
	search_replace_string(
		post_change_javascript,
		"$row",
		row );
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

