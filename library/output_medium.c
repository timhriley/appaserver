/* $APPASERVER_HOME/library/output_medium.c 				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "output_medium.h"

enum output_medium output_medium_get( char *output_medium_string )
{
	if ( string_strcmp(
		output_medium_string,
		OUTPUT_MEDIUM_STDOUT_STRING ) == 0 )
	{
		return output_medium_stdout;
	}
	else
	if ( string_strcmp( output_medium_string, "text" ) == 0 )
		return text_file;
	else
	if ( string_strcmp( output_medium_string, "table" ) == 0 )
		return table;
	else
	if ( string_strcmp( output_medium_string, "spreadsheet" ) == 0 )
		return spreadsheet;
	else
	if ( string_strcmp(
		output_medium_string,
		OUTPUT_MEDIUM_GRACECHART_STRING ) == 0 )
	{
		return gracechart;
	}
	else
	if ( string_strcmp( output_medium_string, "googlechart" ) == 0 )
		return googlechart;
	else
		return table;
}

enum output_medium output_medium_dictionary_extract( DICTIONARY *dictionary )
{
	return
	output_medium_get(
		dictionary_get(
			"output_medium",
			dictionary ) );
}
