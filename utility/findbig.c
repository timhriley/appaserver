/* $APPASERVER_HOME/utility/findbig.c       			   	*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"

/* #define MAX_FILE_ARRAY	65536 */
#define MAX_FILE_ARRAY	131072
#define FIND_COMMAND	 "find `pwd` -xdev -type f -print 2>/dev/null | stat.e | trim_until_character.e ' '"

typedef struct {
	char *filename;
	char *owner;
	char *group;
	char *modified;
	unsigned long size;
} FILE_STRUCT;

void output_file_array(
		FILE_STRUCT *file_array[],
		int num_file_array,
		int max_fetch );

int populate_file_array(
		FILE_STRUCT *file_array[] );

void sort_file_array_size_descending(
		FILE_STRUCT *file_array[],
		int num_file_array );

void add_file_array(
		FILE_STRUCT *file_array[],
		int add_here,
		char *filename,
		char *owner,
		char *group,
		char *modified,
		char *size );

int main( int argc, char **argv )
{
	FILE_STRUCT *file_array[ MAX_FILE_ARRAY ];
	int num_file_array;
	char *max_fetch;

	if ( argc == 2 )
		max_fetch = argv[ 1 ];
	else
		max_fetch = "10";

	num_file_array = populate_file_array( file_array );
	sort_file_array_size_descending( file_array, num_file_array );
	output_file_array( file_array, num_file_array, atoi( max_fetch ) );

	return 0;
}


void output_file_array(
		FILE_STRUCT *file_array[],
		int num_file_array,
		int max_fetch )
{
	int i;

	if ( max_fetch > num_file_array ) max_fetch = num_file_array;

	for( i = 0; i < max_fetch; i++ )
	{
		printf( "%s\n", file_array[ i ]->filename );
		printf( "               %10s   %20s      %20s\n",
			file_array[ i ]->owner, 
			file_array[ i ]->modified, 
			/* --------------------------- */
			/* Returns static memory or "" */
			/* --------------------------- */
			string_commas_unsigned_long(
				file_array[ i ]->size ) );
	}
}


void sort_file_array_size_descending(
		FILE_STRUCT *file_array[],
		int num_file_array )
{
	int all_done = 0;
	int i;
	FILE_STRUCT *tmp;

	while( !all_done )
	{
		all_done = 1;

		for( i = 0; i < num_file_array - 1; i++ )
		{
			if ( file_array[ i ]->size < file_array[ i + 1 ]->size )
			{
				tmp = file_array[ i ];
				file_array[ i ] = file_array[ i + 1 ];
				file_array[ i + 1 ] = tmp;
				all_done = 0;
			}
		}
	}
}


int populate_file_array( FILE_STRUCT *file_array[] )
{
	FILE *p;
	char in_buffer[ 512 ];
	char filename[ 512 ];
	char owner[ 512 ];
	char group[ 512 ];
	char modified[ 512 ];
	char size[ 512 ];
	int iterator = 1;
	int num_file_array = 0;

	p = popen( FIND_COMMAND, "r" );

	while( string_input( in_buffer, p, sizeof ( in_buffer ) ) )
	{
		switch( iterator )
		{
			case 1:		strcpy( filename, in_buffer );
					break;

			case 2:		strcpy( owner, in_buffer );
					break;

			case 3:		strcpy( group, in_buffer );
					break;

			case 4:		strcpy( modified, in_buffer );
					break;

			case 5:		strcpy( size, in_buffer );

					add_file_array(
						file_array,
						num_file_array++,
						filename,
						owner,
						group,
						modified,
						size );

					break;
		}
		if ( ++iterator == 6 ) iterator = 1;
	}

	pclose( p );
	return num_file_array;
}

void add_file_array(
		FILE_STRUCT *file_array[],
		int add_here,
		char *filename,
		char *owner,
		char *group,
		char *modified,
		char *size )
{
	FILE_STRUCT *new = (FILE_STRUCT *)calloc( 1, sizeof ( FILE_STRUCT ) );

	if ( !new )
	{
		fprintf(stderr, 
			"add_file_array() memory allocation error!\n" );
		exit( 1 );
	}

	if ( add_here == MAX_FILE_ARRAY )
	{
		fprintf( stderr, 
			 "findbig: exceeded max files of %d\n",
			 MAX_FILE_ARRAY );
		exit( 1 );
	}

	new->filename = strdup( filename );
	new->owner = strdup( owner );
	new->group = strdup( group );
	new->modified = strdup( modified );
	new->size = strtoul( size, NULL, 10 );

	file_array[ add_here ] = new;
}

