/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/sql.c			   		*/
/* ---------------------------------------------------------------	*/
/* No warranty and Freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdlib.h>
#include "String.h"
#include "timlib.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "sql.h"

char *sql_delimiter_string( char sql_delimiter )
{
	static char string[ 2 ];

	*string = sql_delimiter;

	return string;
}

char *sql_execute(
		char *appaserver_error_filename,
		LIST *sql_list,
		char *sql_statement )
{
	char system_string[ 128 ];
	FILE *output_pipe;
	char *temp_filename;
	char *error_string;

	if ( !appaserver_error_filename )
	{
		char message[ 128 ];

		sprintf(message, "appaserver_error_filename is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( sql_list )
	&&   !sql_statement )
	{
		return (char *)0;
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"tee -a %s | sql.e 2>%s",
		appaserver_error_filename,
		( temp_filename =
			timlib_temp_filename( "insert" /* key */ ) ) );

	output_pipe = appaserver_output_pipe( system_string );

	if ( sql_statement )
	{
		fprintf(output_pipe,
			"%s\n",
			sql_statement );
	}
	else
	do {
		fprintf(output_pipe,
			"%s\n",
			(char *)list_get( sql_list ) );

	} while ( list_next( sql_list ) );

	pclose( output_pipe );

	error_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_file_fetch(
			temp_filename,
			"\n<br>" /* delimiter */ );

	timlib_remove_file( temp_filename );

	return
	string_search_replace(
		error_string,
		"ERROR 1062 (23000) at",
		"At" );
}

