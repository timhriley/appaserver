/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/utility/accumulate.c				*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "timlib.h"
#include "piece.h"

#define DEFAULT_DELIMITER	'^'

double accumulate_running(
		double sum,
		char *input_buffer,
		double amount,
		int piece_offset,
		char delimiter );

int main( int argc, char **argv )
{
	double sum = 0.0;
	char input_buffer[ 1024 ];
	char piece_buffer[ 1024 ];
	char delimiter = DEFAULT_DELIMITER;
	int piece_offset = -1;
	boolean append = 0;
	boolean running = 0;

	if ( argc != 4 )
	{
		fprintf(stderr,
		"Usage: %s piece_offset delimiter replace|append|running\n",
			argv[ 0 ] );
		exit( 1 );
	}

	piece_offset = atoi( argv[ 1 ] );
	delimiter = *argv[ 2 ];

	if ( strcmp( argv[ 3 ], "replace" ) == 0 )
	{
		append = 0;
	}
	else
	if ( strcmp( argv[ 3 ], "append" ) == 0 )
		append = 1;
	else
	if ( strcmp( argv[ 3 ], "running" ) == 0 )
		running = 1;
	else
	{
		fprintf(stderr,
	"ERROR in %s: must be either 'replace' or 'append' or 'running'\n",
			argv[ 0 ] );
		exit( 1 );
	}

	while( string_input( input_buffer, stdin, 1024 ) )
	{
		if ( !*input_buffer ) continue;
		if ( *input_buffer == '#' ) continue;

		if ( piece_offset < 0 )
		{
			printf( "%s\n", input_buffer );
			continue;
		}

		*piece_buffer = '\0';

		if ( !piece(	piece_buffer,
				delimiter,
				input_buffer,
				piece_offset ) )
		{
			fprintf(stderr,
	"Warning in %s/%s()/%d: cannot piece('%c',%d): (%s); continuing.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				delimiter,
				piece_offset,
				input_buffer );
			continue;
		}

		if ( running )
		{
			sum = accumulate_running(
					sum,
					input_buffer,
					atof( piece_buffer ),
					piece_offset,
					delimiter );
			continue;
		}
	
		sum += atof( piece_buffer );
		sprintf( piece_buffer, "%.4lf", sum );

		if ( append )
		{
			printf( "%s\n",
				piece_insert(
					input_buffer, 
					delimiter, 
					piece_buffer, 
					piece_offset + 1 ) );
		}
		else
		{
			printf( "%s\n",
				piece_replace(
					input_buffer, 
					delimiter, 
					piece_buffer, 
					piece_offset ) );
		}
	}

	return 0;

}

/* Returns new sum */
/* --------------- */
double accumulate_running(
		double sum,
		char *input_buffer,
		double amount,
		int piece_offset,
		char delimiter )
{
	char piece_buffer[ 128 ];
	char running_balance_string[ 128 ];
	double running_balance = 0.0;
	static boolean first_time = 1;

	if ( first_time )
	{
		if ( !piece(	piece_buffer,
				delimiter,
				input_buffer,
				piece_offset + 1 ) )
		{
			fprintf(stderr,
	"Warning in %s/%s()/%d: cannot piece('%c',%d): (%s); continuing.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				delimiter,
				piece_offset + 1,
				input_buffer );

			printf( "%s\n", input_buffer );

			return 0.0;
		}

		running_balance = atof( piece_buffer );
		first_time = 0;
		return running_balance;
	}
	else
	{
		running_balance = sum + amount;
	}

	sprintf( running_balance_string, "%.2lf", running_balance );

	printf( "%s\n",
		piece_replace(
			input_buffer,
			delimiter, 
			running_balance_string /* new_data */, 
			piece_offset + 1 ) );

	return running_balance;
}

