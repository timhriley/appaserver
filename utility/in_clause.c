/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/utility/in_clause.c			   		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "String.h"

void output_in_clause_stdin( void );
void output_in_clause_parameter( char *argv_1 );

int main( int argc, char **argv )
{
	if ( argc == 2 )
	{
		output_in_clause_parameter( argv[ 1 ] );
	}
	else
	{
		output_in_clause_stdin();
	}

	return 0;
}

void output_in_clause_stdin( void )
{
	char input_buffer[ 1024 ];
	LIST *data_list;
	data_list = list_new();

	while( string_input( input_buffer, stdin, 1024 ) )
		list_set(
			data_list,
			strdup( input_buffer ) );

	printf( "in (%s)\n", string_in_clause( data_list ) );
}

void output_in_clause_parameter( char *argv_1 )
{
	printf( "in (%s)\n",
		string_in_clause( 
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			list_string_list(
				argv_1 /* string */,
				string_delimiter(
					argv_1 /* string */ ) ) ) );
}
