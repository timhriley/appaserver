/* -------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/sql.c			   		*/
/* -------------------------------------------------------------	*/
/* No warranty and Freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------	*/

#include "sql.h"

char *sql_delimiter_string( char sql_delimiter )
{
	static char string[ 2 ] = {0};

	if ( !*string ) *string = sql_delimiter;

	return string;
}
