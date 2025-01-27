/* $APPASERVER_HOME/utility/keys_match_sum.c	*/
/* ----------------------------------------- 	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "appaserver_error.h"
#include "keys_match_sum.h"

/* Constants */
/* --------- */
#define DEFAULT_DELIMITER	'|'

/* Prototypes */
/* ---------- */
void keys_match_sum(	double match_sum,
			int key_piece_offset,
			int value_piece_offset,
			char delimiter,
			LIST *input_list );

int main( int argc, char **argv )
{
	char delimiter = DEFAULT_DELIMITER;
	int key_piece_offset = 0;
	int value_piece_offset = 1;
	double match_sum;
	LIST *input_list;

	/* appaserver_error_stderr( argc, argv ); */

	if ( argc < 2 )
	{
		fprintf(stderr,
"Usage: %s match_sum [key_piece_offset] [value_piece_offset] [delimiter]\n",
			argv[ 0 ] );
		exit( 1 );
	}

	match_sum = atof( argv[ 1 ] );
	if ( argc >= 3 ) key_piece_offset = atoi( argv[ 2 ] );
	if ( argc >= 4 ) value_piece_offset = atoi( argv[ 3 ] );
	if ( argc >= 5 ) delimiter = *argv[ 4 ];

	input_list = pipe2list( "cat" );

	if ( list_length( input_list ) )
	{
		keys_match_sum(
			match_sum,
			key_piece_offset,
			value_piece_offset,
			delimiter,
			input_list );
	}

	return 0;

} /* main() */

void keys_match_sum(	double match_sum,
			int key_piece_offset,
			int value_piece_offset,
			char delimiter,
			LIST *input_list )
{
	LIST *key_list;
	LIST *value_string_list;
	LIST *value_double_list;
	LIST *match_sum_key_list;

	if ( !list_length( input_list ) ) return;

	key_list =
		list_delimiter_list_piece_list(
			input_list,
			delimiter,
			key_piece_offset );

	if ( !list_length( key_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty key_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	value_string_list =
		list_delimiter_list_piece_list(
			input_list,
			delimiter,
			value_piece_offset );

	if ( !list_length( value_string_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty value_string_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	value_double_list =
		list_string_to_double_list(
			value_string_list );

	if ( !list_length( value_double_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty value_double_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	match_sum_key_list =
		keys_match_sum_get_match_sum_key_list(
			key_list,
			value_double_list,
			match_sum );

	if ( list_length( match_sum_key_list ) )
	{
		printf( "%s\n",
			list_display_delimited(
				match_sum_key_list,
				delimiter ) );
	}

} /* keys_match_sum() */
