/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/spreadsheet.c		*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "timlib.h"
#include "piece.h"
#include "date.h"
#include "sql.h"
#include "list.h"
#include "boolean.h"
#include "spreadsheet_column.h"
#include "spreadsheet.h"

SPREADSHEET *spreadsheet_new(
			char *spreadsheet_name )
{
	SPREADSHEET *spreadsheet;

	if ( ! ( spreadsheet = calloc( 1, sizeof( SPREADSHEET ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	spreadsheet->spreadsheet_name = spreadsheet_name;
	return spreadsheet;
}

LIST *spreadsheet_column_list( char *spreadsheet_name )
{
	return spreadsheet_column_system_list(
		spreadsheet_column_sys_string(
			spreadsheet_primary_where(
				spreadsheet_name ) ) );
}

SPREADSHEET *spreadsheet_fetch(
			char *spreadsheet_name )
{
	SPREADSHEET *spreadsheet;

	if ( !spreadsheet_name || !*spreadsheet_name )
	{
		return (SPREADSHEET *)0;
	}

	spreadsheet = spreadsheet_new( strdup( spreadsheet_name ) );

	spreadsheet->
		spreadsheet_column_list =
			spreadsheet_column_list(
				spreadsheet_name );

	return spreadsheet;
}

char *spreadsheet_escape_name( char *spreadsheet_name )
{
	static char escape_spreadsheet_name[ 256 ];

	return string_escape_quote(
			escape_spreadsheet_name,
			spreadsheet_name );
}

char *spreadsheet_primary_where(
			char *spreadsheet_name )
{
	char where[ 128 ];

	sprintf(where,
		"spreadsheet = '%s'",
		spreadsheet_escape_name(
			spreadsheet_name ) );

	return strdup( where );
}

char *spreadsheet_minimum_date(
			char **maximum_date,
			char *spreadsheet_filename )
{
if ( maximum_date ){}
if ( spreadsheet_filename ){}

	return (char *)0;
}

