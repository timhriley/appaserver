/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/utility/mysql_transform.c				*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "timlib.h"

void mysql_remove_spaces(
		char *buffer /* in/out */ );

void mysql_escape_delimiter(
		char delimiter,
		char *buffer /* in/out */ );

void mysql_remove_null(
		char delimiter,
		char *buffer /* in/out */ );

void mysql_tab_to_delimiter(
		char delimiter,
		char *buffer /* in/out */ );

int main( int argc, char **argv )
{
	char buffer[ STRING_128K ];
	char delimiter;

	if ( argc != 2 )
	{
		fprintf(stderr,
			"Usage: %s delimiter\n",
			argv[ 0 ] );
		exit( 1 );
	}

	delimiter = *argv[ 1 ];

	while( string_input( buffer, stdin, sizeof ( buffer ) ) )
	{
		if ( !*buffer )
		{
			printf( "\n" );
			continue;
		}

#ifdef NOT_DEFINED
		mysql_remove_spaces(
			buffer /* in/out */ );
#endif

		mysql_escape_delimiter(
			delimiter,
			buffer /* in/out */ );

		mysql_tab_to_delimiter(
			delimiter,
			buffer /* in/out */ );

		mysql_remove_null(
			delimiter,
			buffer /* in/out */ );

		printf( "%s\n", buffer );
	}

	return 0;
}

void mysql_escape_delimiter(
		char delimiter,
		char *buffer /* in/out */ )
{
	if ( !string_exists_character(
		buffer /* source */,
		delimiter ) )
	{
		return;
	}

	{
		char destination[ STRING_64K ];

		(void)string_escape_character(
			destination,
			buffer /* data */,
			delimiter );

		strcpy( buffer, destination );
	}
}

void mysql_remove_null(
		char delimiter,
		char *buffer /* in/out */ )
{
	char search_string[ 1024 ];
	char replace_string[ 3 ];
	int str_len;

	/* If NULL */
	/* ------- */
	if ( strcmp( buffer, "NULL" ) == 0 )
	{
		*buffer = '\0';
	}

	/* If begins with NULL^ */
	/* -------------------- */
	sprintf( search_string,
		 "NULL%c",
		 delimiter );

	if ( string_strict_case_instr(
		search_string,
		buffer,
		1 ) == 0 )
	{
		sprintf( replace_string, "%c", delimiter );

		search_replace_strict_case_once(
			buffer,
			(boolean *)0 /* made_replace */,
			search_string,
			replace_string );
	}

	/* If ^NULL^ in the middle */
	/* ----------------------- */
	sprintf( search_string,
		 "%cNULL%c",
		 delimiter,
		 delimiter );

	sprintf( replace_string, "%c%c", delimiter, delimiter );

	search_replace_strict_case_string(
		buffer,
		search_string,
		replace_string );

	/* If ends with ^NULL */
	/* ------------------ */
	sprintf( search_string,
		 "%cNULL",
		 delimiter );

	str_len = strlen( search_string );

	if ( string_strict_case_strcmp(
		string_right(
			buffer,
			str_len ),
		search_string ) == 0 )
	{
		sprintf( replace_string, "%c", delimiter );

		sprintf( buffer + strlen( buffer ) - str_len,
			 "%s",
			 replace_string );
	}
}

void mysql_tab_to_delimiter(
		char delimiter,
		char *buffer /* in/out */ )
{
	(void)string_search_replace_character(
		buffer /* source_destination */,
		STRING_TAB /* search_character */,
		delimiter /* replace_character */ );
}

void mysql_remove_spaces( char *buffer /* in/out */ )
{
	char substring[ 3 ];
	char tabstring[ 2 ];

	/* Trims leading and trailing spaces */
	/* --------------------------------- */
	(void)string_trim( buffer );

	sprintf(tabstring,
		"%c",
		STRING_TAB );

	/* Trim leading spaces for each field. */
	/* ----------------------------------- */
	sprintf(substring,
		" %c",
		STRING_TAB );

	while ( string_exists( buffer, substring ) )
	{
		(void)string_search_replace(
			buffer /* source_destination */,
			substring /* search_string */,
			tabstring /* replace_string */ );
	}

	/* Trim trailing spaces for each field. */
	/* ------------------------------------ */
	sprintf(substring,
		"%c ",
		STRING_TAB );

	while ( string_exists( buffer, substring ) )
	{
		(void)string_search_replace(
			buffer /* source_destination */,
			substring /* search_string */,
			tabstring /* replace_string */ );
	}
}
