/* $APPASERVER_HOME/utility/colapse_repeating.c			*/
/* ------------------------------------------------------ 	*/
/* This process assumes as input rows that repeat.		*/
/* It will display the row then a count of the remaining.	*/
/* ------------------------------------------------------ 	*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------ 	*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"

/* Structures */
/* ---------- */
typedef struct
{
	char *key;
	char *row;
	int count;
} RECORD;

/* Prototypes */
/* ---------- */
void record_list_display(	LIST *record_list,
				char delimiter );

char *colapse_repeating_get_key(
				char *row,
				int key_column_count,
				char delimiter );

RECORD *record_new(		void );

LIST *colapse_repeating(	int key_column_count,
				char delimiter );

RECORD *record_get_or_set(	LIST *list,
				char *key,
				char *row );

RECORD *record_list_seek(	LIST *list,
				char *key );

RECORD *record_new( void )
{
	RECORD *r;

	if ( ! ( r = (RECORD *)calloc( 1, sizeof( RECORD ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return r;

} /* record_new() */

RECORD *record_list_seek(	LIST *list,
				char *key )
{
	RECORD *r;

	if ( !list_rewind( list ) ) return (RECORD *)0;

	do {
		r = list_get_pointer( list );

		if ( timlib_strcmp( r->key, key ) == 0 ) return r;
	} while ( list_next( list ) );

	return (RECORD *)0;

} /* record_list_seek() */

RECORD *record_get_or_set(	LIST *list,
				char *key,
				char *row )
{
	RECORD *r;

	if ( !list )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: null list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( ( r = record_list_seek( list, key ) ) )
		return r;

	r = record_new();
	r->key = strdup( key );
	r->row = strdup( row );
	list_append_pointer( list, r );

	return r;

} /* record_get_or_set() */

char *colapse_repeating_get_key(
				char *row,
				int key_column_count,
				char delimiter )
{
	static char key[ 512 ];
	char *key_ptr;
	char piece_buffer[ 128 ];
	int i;

	key_ptr = key;

	for (	i = 0;
		i < key_column_count;
		i++ )
	{
		piece( piece_buffer, delimiter, row, i );
		key_ptr += sprintf(	key_ptr,
					"%s%c",
					piece_buffer,
					delimiter );
	}

	*key_ptr = '\0';
	return key;

} /* colapse_repeating_get_key() */

LIST *colapse_repeating(	int key_column_count,
				char delimiter )
{
	char row[ 1024 ];
	LIST *record_list;
	RECORD *r;
	char *key;

	record_list = list_new();

	while ( timlib_get_line( row, stdin, 1024 ) )
	{
		key = colapse_repeating_get_key(
				row,
				key_column_count,
				delimiter );

		r = record_get_or_set(
			record_list,
			key,
			row );

		r->count++;
	}

	return record_list;

} /* colapse_repeating() */

void record_list_display(	LIST *record_list,
				char delimiter )
{
	RECORD *r;

	if ( !list_rewind( record_list ) ) return;

	do {
		r = list_get_pointer( record_list );

		printf( "%s\n", r->row );

		if ( r->count > 1 )
		{
			printf( "...%c%d%cmore\n",
				delimiter,
				r->count - 1,
				delimiter );
		}

	} while ( list_next( record_list ) );

} /* record_list_display() */

int main( int argc, char **argv )
{
	int key_column_count;
	char delimiter;
	LIST *record_list;

	if ( argc != 3 )
	{
		fprintf(stderr,
			"Usage: %s key_column_count delimiter\n",
			argv[ 0 ] );
		exit( 1 );
	}

	key_column_count = atoi( argv[ 1 ] );
	delimiter = *argv[ 2 ];

	record_list =
		colapse_repeating(
			key_column_count,
			delimiter );

	record_list_display(
			record_list,
			delimiter );

	return 0;

} /* main() */

