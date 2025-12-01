/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/keys_match_sum.c		   		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <string.h>
#include <strings.h>
#include <math.h>
#include <time.h>
#include "timlib.h"
#include "float.h"
#include "piece.h"
#include "date.h"
#include "array.h"
#include "keys_match_sum.h"

LIST *keys_match_sum_get_match_sum_key_list(
		LIST *key_list,
		LIST *value_double_list,
		double match_sum )
{
	double *double_array;
	int length = 0;
	static LIST *binary_count_list = {0};
	char *binary_count_string;
	LIST *match_sum_key_list;
	double p;
	int offset;

	/* Check if single match. */
	/* ---------------------- */
	offset = list_double_list_match( value_double_list, match_sum );

	if ( offset != -1 )
	{
		char *return_key;

		if ( ! ( return_key =
				list_offset_seek(
					offset,
					key_list ) ) )
		{
			fprintf( stderr,
"ERROR in %s/%s()/%d: cannot get offset=%d from list of length=%d.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 offset,
				 list_length( key_list ) );
			exit( 1 );
		}

		match_sum_key_list = list_new();
		list_append_pointer( match_sum_key_list, return_key );
		return match_sum_key_list;

	} /* if single match. */

	double_array = 
		array_list_to_double_array(
			&length,
			value_double_list );

	if ( !length ) return (LIST *)0;

	p = pow( 2.0, (double)length );

	if ( !binary_count_list )
	{
		char sys_string[ 1024 ];

		sprintf( sys_string,
		 	"binary_count.e %.0lf",
		 	p );

		binary_count_list = pipe2list( sys_string );
	}

	if ( !list_rewind( binary_count_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty binary_count_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	do {
		binary_count_string = list_get_pointer( binary_count_list );

		match_sum_key_list =
			keys_match_sum_get_double_array_match_sum_key_list(
				binary_count_string,
				match_sum,
				double_array,
				length,
				key_list );

		if ( list_length( match_sum_key_list ) )
			return match_sum_key_list;

	} while( list_next( binary_count_list ) );

	return (LIST *)0;
}

/* ------------------------------------------------------------- */
/* Sample binary_count_reverse: 11000000000000000000000000000000 */
/* ------------------------------------------------------------- */
LIST *keys_match_sum_get_binary_reverse_array_index_list(
		char *binary_count_reverse,
		double match_sum,
		double *double_array,
		int length )
{
	LIST *array_index_list;
	char bit;
	double d;
	int i;
	double sum;
	char index_string[ 16 ];

	sum = 0.0;

	for (	i = 0;
		i < length;
		i++ )
	{
		bit = binary_count_reverse[ i ];

		if ( bit == '1' )
		{
			d = double_array[ i ];
			sum += d;
		}
	}

	if ( !float_dollar_virtually_same( sum, match_sum ) )
		return (LIST *)0;

	/* Found a match! */
	/* -------------- */
	array_index_list = list_new();

	for (	i = 0;
		i < length;
		i++ )
	{
		bit = binary_count_reverse[ i ];

		if ( bit == '1' )
		{
			sprintf( index_string,
				 "%d",
				 i );

			list_append_pointer(
				array_index_list,
				strdup( index_string ) );
		}
	}

	return array_index_list;
}

LIST *keys_match_sum_dereference_array_index_list(
		LIST *array_index_list,
		LIST *key_list )
{
	LIST *return_list;
	char *return_key;
	int offset;
	char *index_string;

	if ( !list_length( key_list ) ) return (LIST *)0;
	if ( !list_rewind( array_index_list ) ) return (LIST *)0;

	return_list = list_new();

	do {
		index_string = list_get_pointer( array_index_list );

		/* Both index and offset are zero based. */
		/* ------------------------------------- */
		offset = atoi( index_string );

		if ( ! ( return_key =
				list_offset_seek(
					offset,
					key_list ) ) )
		{
			fprintf( stderr,
"ERROR in %s/%s()/%d: cannot get offset=%d from list of length=%d.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 offset,
				 list_length( key_list ) );
			exit( 1 );
		}

		list_append_pointer( return_list, return_key );

	} while( list_next( array_index_list ) );

	return return_list;
}

/* ------------------------------------------------------------ */
/* Sample binary_count_string: 00000000000000000000000000000011 */
/* ------------------------------------------------------------ */
LIST *keys_match_sum_get_double_array_match_sum_key_list(
		char *binary_count_string,
		double match_sum,
		double *double_array,
		int length,
		LIST *key_list )
{
	char binary_count_reverse[ 256 ];
	LIST *array_index_list;

	/* ------------------------------------------------------------- */
	/* Sample binary_count_reverse: 11000000000000000000000000000000 */
	/* ------------------------------------------------------------- */
	timlib_reverse_string(
		binary_count_reverse,
		binary_count_string );

	array_index_list =
		keys_match_sum_get_binary_reverse_array_index_list(
			binary_count_reverse,
			match_sum,
			double_array,
			length );

	if ( list_length( array_index_list ) )
	{
		return keys_match_sum_dereference_array_index_list(
			array_index_list,
			key_list );
	}
	else
	{
		return (LIST *)0;
	}
}

