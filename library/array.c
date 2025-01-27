/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/array.h			   		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "array.h"

ARRAY *array_new( size_t length )
{
	ARRAY *array;

	if ( ! ( array = (ARRAY *)calloc( 1, sizeof ( ARRAY ) ) ) )
	{
		fprintf( stderr,
			"Error in %s/%s()/%d: memory allocation error.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	array->base = (void **)calloc( length, sizeof ( void * ) );
	array->length = length;
	return array;
}

int array_set(	void **base,
		size_t length,
		void *item,
		int position )
{
	if ( (size_t)position > length ) return 0;
	if ( position < 0 ) return 0;
	base[ position ] = item;
	return 1;
}

void *array_get(	void **base,
			size_t length,
			int position )
{
	if ( (size_t)position > length ) return (void *)0;
	if ( position < 0 ) return (void *)0;
	return base[ position ];
}

int array_sort_string(	void **base,
			size_t length )
{
	qsort( (void **)base, length, sizeof (void *), array_string_compare );
	return 1;
}

int array_string_compare(
			const void *s1,
			const void *s2 )
{
/*
	const char **s1_indirect = (const char **)s1;
	const char **s2_indirect = (const char **)s2;

fprintf( stderr,
	 "%s/%s returns: %d\n",
	 *s1_indirect,
	 *s2_indirect,
	 strcmp( *s1_indirect, *s2_indirect ) );
*/

	return strcmp( *(char **)s1, *(char **)s2 );

}

double *array_list_to_double_array(
			int *length,
			LIST *double_list )
{
	double *d_ptr;
	double *double_array;
	int index;

	*length = list_length( double_list );

	if ( !*length ) return (double *)0;

	if ( ! ( double_array =
			(double *)calloc(
				*length,
				sizeof ( double ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	list_rewind( double_list );
	index = 0;

	do {
		d_ptr = (double *)list_get_pointer( double_list );
		double_array[ index++ ] = *d_ptr;

	} while( list_next( double_list ) );

	return double_array;

}

