/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/sql.c			   		*/
/* ---------------------------------------------------------------	*/
/* No warranty and Freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdlib.h>
#include "String.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "file.h"
#include "sql.h"

char *sql_delimiter_string( char sql_delimiter )
{
	static char string[ 2 ];

	*string = sql_delimiter;

	return string;
}

char *sql_execute(
		const char *sql_executable,
		char *appaserver_error_filename,
		LIST *sql_list,
		char *sql_statement )
{
	char system_string[ 128 ];
	FILE *output_pipe;
	char *temp_filespecification;
	char *error_string;

	if ( !list_rewind( sql_list )
	&&   !sql_statement )
	{
		return (char *)0;
	}

	if ( appaserver_error_filename )
	{
		snprintf(
			system_string,
			sizeof ( system_string ),
			"tee -a %s | %s 2>%s",
			appaserver_error_filename,
			sql_executable,
			( temp_filespecification =
		  		/* ------------------- */
		  		/* Returns heap memory */
		  		/* ------------------- */
				file_temp_filespecification(
					"insert" /* key */ ) ) );
	}
	else
	{
		snprintf(
			system_string,
			sizeof ( system_string ),
			"%s 2>%s",
			sql_executable,
			( temp_filespecification =
		  		/* ------------------- */
		  		/* Returns heap memory */
		  		/* ------------------- */
				file_temp_filespecification(
					"insert" /* key */ ) ) );
	}

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
			temp_filespecification,
			"\n<br>" /* delimiter */ );

	file_remove( temp_filespecification );
	free( temp_filespecification );

	return
	string_search_replace(
		error_string,
		"ERROR 1062 (23000) at",
		"At" );
}

