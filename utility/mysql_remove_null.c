/* utility/mysql_remove_null.c */
/* ------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"

int main( int argc, char **argv )
{
	char buffer[ 65536 ];
	char delimiter;
	char search_string[ 1024 ];
	char replace_string[ 3 ];
	int str_len;

	if ( argc != 2 )
	{
		fprintf(stderr,
			"Usage: %s delimiter\n",
			argv[ 0 ] );
		exit( 1 );
	}

	delimiter = *argv[ 1 ];

	while( timlib_get_line( buffer, stdin, 65536 ) )
	{
		if ( !*buffer )
		{
			printf( "\n" );
			continue;
		}

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

		if ( timlib_strict_case_instr(
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

		if ( timlib_strict_case_strcmp(
			timlib_right_string(
				buffer,
				str_len ),
			search_string ) == 0 )
		{
			sprintf( replace_string, "%c", delimiter );

			sprintf( buffer + strlen( buffer ) - str_len,
				 "%s",
				 replace_string );
		}

		/* Output */
		/* ------ */
		printf( "%s\n", buffer );
	}

	return 0;

} /* main() */
