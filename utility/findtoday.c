/* utility/findtoday.c		       			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

/* -------- */
/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"

/* --------- */
/* Constants */
/* --------- */
#define FIND_COMMAND	 "find `pwd` -xdev -type f -print 2>/dev/null | statymd.e | trim_until_character.e ' '"

/* ---------------- */
/* Type definitions */
/* ---------------- */
typedef struct {
	char *filename;
	char *owner;
	char *modified;
	unsigned long size;
} FILE_STRUCT;

void output_file_struct( FILE_STRUCT file_struct );

void add_file_struct(	FILE_STRUCT *file_struct,
			char *filename,
			char *owner,
			char *modified,
			char *size );

void findtoday( int days_before );

int main( int argc, char **argv )
{
	int days_before = 0;

	if ( argc == 2 ) days_before = atoi( argv[ 1 ] );

	findtoday( days_before );
	return 0;
} /* main() */


void output_file_struct( FILE_STRUCT file_struct )
{
	printf( "%s\n", file_struct.filename );
	printf( "               %10s   %20s      %20s\n",
		file_struct.owner, 
		file_struct.modified, 
		place_commas_in_unsigned_long(
			file_struct.size ) );

} /* output_file_struct() */


void findtoday( int days_before )
{
	FILE *p;
	char in_buffer[ 512 ];
	char filename[ 512 ];
	char owner[ 512 ];
	char modified[ 512 ];
	char size[ 512 ];
	int iterator = 1;
	FILE_STRUCT file_struct = {0};
	char *now_string;

	if ( days_before )
	{
		char sys_string[ 32 ];

		sprintf( sys_string, "now.sh ymd -%d", days_before );
		now_string = pipe2string( sys_string );
	}
	else
	{
		now_string = pipe2string( "now.sh ymd" );
	}

	p = popen( FIND_COMMAND, "r" );

	while( get_line( in_buffer, p ) )
	{
		switch( iterator )
		{
			case 1:		strcpy( filename, in_buffer );
					break;

			case 2:		strcpy( owner, in_buffer );
					break;

			case 3:		strcpy( modified, in_buffer );
					break;

			case 4:		strcpy( size, in_buffer );

					if ( timlib_strncmp(
							modified,
							now_string ) == 0 )
					{
						add_file_struct(
							&file_struct,
							filename,
							owner,
							modified,
							size );

						output_file_struct(
							file_struct );
					}

					break;
		}
		if ( ++iterator == 5 ) iterator = 1;
	}

	pclose( p );

} /* findtoday() */


void add_file_struct(	FILE_STRUCT *file_struct,
			char *filename,
			char *owner,
			char *modified,
			char *size )
{
	file_struct->filename = filename;
	file_struct->owner = owner;
	file_struct->modified = modified;
	file_struct->size = strtoul( size, NULL, 10 );

} /* add_file_array() */

