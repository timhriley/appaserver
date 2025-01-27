/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/libary/folder_row_level_restriction.c		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "appaserver_error.h"
#include "appaserver.h"
#include "folder.h"
#include "folder_row_level_restriction.h"

char *folder_row_level_restriction_string( char *folder_name )
{
	char system_string[ 1024 ];

	if ( !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(system_string,
		"select.sh %s %s \"%s\"",
		"row_level_restriction",
		FOLDER_ROW_LEVEL_RESTRICTION_TABLE,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		folder_primary_where_string(
			FOLDER_PRIMARY_KEY,
			folder_name ) );

	return string_fetch_pipe( system_string );
}

FOLDER_ROW_LEVEL_RESTRICTION *
	folder_row_level_restriction_fetch(
		char *folder_name )
{
	if ( !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	folder_row_level_restriction_parse(
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_pipe_input(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			appaserver_system_string(
				FOLDER_ROW_LEVEL_RESTRICTION_SELECT,
				FOLDER_ROW_LEVEL_RESTRICTION_TABLE,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				folder_primary_where_string(
					FOLDER_PRIMARY_KEY,
					folder_name ) ) ) );
}

FOLDER_ROW_LEVEL_RESTRICTION *
	folder_row_level_restriction_parse(
		char *input )
{
	FOLDER_ROW_LEVEL_RESTRICTION *folder_row_level_restriction;

	folder_row_level_restriction =
		folder_row_level_restriction_calloc();

	if ( !input || !*input ) return folder_row_level_restriction;

	/* See FOLDER_ROW_LEVEL_RESTRICTION_SELECT */
	/* --------------------------------------- */
	folder_row_level_restriction->non_owner_viewonly =
		folder_row_level_restriction_non_owner_viewonly(
			input );

	folder_row_level_restriction->non_owner_forbid =
		folder_row_level_restriction_non_owner_forbid(
			input );

	return folder_row_level_restriction;
}

FOLDER_ROW_LEVEL_RESTRICTION *
	folder_row_level_restriction_calloc(
		void )
{
	FOLDER_ROW_LEVEL_RESTRICTION *folder_row_level_restriction;

	if ( ! ( folder_row_level_restriction =
			calloc(	1,
				sizeof ( FOLDER_ROW_LEVEL_RESTRICTION ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return folder_row_level_restriction;
}

boolean folder_row_level_restriction_non_owner_viewonly( char *input )
{
	if ( !input ) return 0;

	return
	(string_strcmp(
		input,
		"row_level_non_owner_view_only" ) == 0 );
}

boolean folder_row_level_restriction_non_owner_forbid( char *input )
{
	if ( !input ) return 0;

	return
	(string_strcmp(
		input,
		"row_level_non_owner_forbid" ) == 0 );
}

