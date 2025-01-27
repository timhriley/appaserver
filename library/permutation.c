/* permutation.c */
/* ------------- */
/* ==================================================== */
/* Function code for PERMUTATION ADT			*/
/*                                                      */
/* Freely available software: see Appaserver.org	*/
/* ==================================================== */

#include <stdio.h>
#include <stdlib.h>
#include "permutation.h"

PERMUTATION *permutation_new( int N )
{
	PERMUTATION *permutation;

	permutation = (PERMUTATION *)calloc( 1, sizeof( PERMUTATION ) );
	permutation->N = N;
	permutation->n_factoral = permutation_get_factoral( N );
	permutation->value_array = (int *)calloc( N, sizeof( int ) );
	permutation->permutation_array =
		permutation_allocate_permutation_array(
			N, permutation->n_factoral );
	permutation->level = -1;
	permutation_visit(	permutation->permutation_array,
				permutation->value_array,
				&permutation->level,
				N,
				0 );
	return permutation;
} /* permutation_new() */

int *permutation_allocate_permutation_array( int N, unsigned int n_factoral )
{
	int *permutation_array;

	if ( ! ( permutation_array =
			(int *)calloc( n_factoral * N, sizeof( int ) ) ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot allocate memory\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return permutation_array;

} /* permutation_allocate_permutation_array() */

void permutation_visit(	int *permutation_array,
			int *value_array,
			int *level,
			int N,
			int k )
{
	int i;

	(*level)++;
	value_array[ k ] = *level;

	if ( *level == N )
	{
		permutation_set_permutation_array(	permutation_array,
							value_array,
							N );
	}
	else
	{
		for( i = 0; i < N; i++ )
		{
			if ( value_array[ i ] == 0 )
				permutation_visit(
					permutation_array,
					value_array,
					level,
					N,
					i );
		}
	}
	(*level)--;
	value_array[ k ] = 0;
} /* permutation_visit() */

void permutation_output_permutation_array(
					int *permutation_array,
					unsigned int n_factoral,
					int N )
{
	int x, y;
	int row;

	for( y = 0; y < n_factoral; y++ )
	{
		row = N * y;
		for( x = 0; x < N; x++ )
		{
			printf( "%d ", permutation_array[ row + x ] );
		}
		printf( "\n" );
	}
}

unsigned int permutation_get_factoral( int N )
{
	unsigned int factoral = 1;

	while( N ) factoral *= N--;
	return factoral;
} /* permutation_get_factoral() */

void permutation_set_permutation_array(	int *permutation_array,
					int *value_array,
					int N )
{
	int x;
	static int y = 0;
	int row;

	row = N * y;
	for( x = 0; x < N; x++ )
	{
		permutation_array[ row + x ] = value_array[ x ];
	}
	y++;
}

int permutation_get_single_offset(
					int x,
					int y,
					int y_width )
{
	return (y * y_width) + x;
} /* permutation_get_single_offset() */
