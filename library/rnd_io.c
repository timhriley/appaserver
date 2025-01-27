/* rnd_io.c */
/* -------- */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#include "rnd_io.h"

RANDOM_IO *init_random_io( void )
{
        return ( RANDOM_IO *) calloc( 1, sizeof( RANDOM_IO ) );
}

int set_filename_random_io( RANDOM_IO *f, char *filename )
{
        if( f == NULL ) return -1;

        if( ( !filename )
        ||  ( !*filename )
        ||  ( isspace( *filename ) ) )
        {
                set_error_random_io( f, "Invalid file name\n" );
                return -1;
        }
        f->filename = (char *)strdup( filename );
        return 0;
}


int set_rec_len_random_io( RANDOM_IO *f, int rec_len )
{
        if( f == NULL ) return -1;

        if( rec_len == 0 )
        {
                set_error_random_io( f, "Invalid record len\n" );
                return -1;
        }
        f->record_len = rec_len;
        return 0;
}

int open_random_io( RANDOM_IO *f, char *type )
{
        if( f == NULL ) return -1;

        f->file_ptr = fopen( f->filename, type );

        if( f->file_ptr == NULL )
        {
                sprintf( f->error_buffer, "Can't Open file\n" );
                return -1;
        }

        return 0;
}


int get_rec_random_io( RANDOM_IO *f, unsigned long offset )
{
        long  file_offset;

        if( f == NULL ) return -1;
        file_offset = offset * f->record_len;

        return fseek( f->file_ptr, file_offset, SEEK_SET );
}


int  set_previous_rec_random_io( RANDOM_IO *f )
{
        long file_offset;

        if( f == NULL ) return -1;

        file_offset = ftell( f->file_ptr );

        if( file_offset == 0 ) return -1;

        if( fseek( f->file_ptr, file_offset - f->record_len, SEEK_SET ) != 0 )
                return -1;

        return 0;
}

int fetch_rec_random_io( RANDOM_IO *f, void *rec )
{
        if( f == NULL ) return -1;

        return fread( rec, f->record_len, 1, f->file_ptr );
}


unsigned long rnd_io_get_file_size( RANDOM_IO *f)
{
        unsigned long filesize;

        if( f->file_ptr == NULL ) return 0L;

        fseek( f->file_ptr, 0L, SEEK_END );
        filesize = ftell( f->file_ptr );
        rewind( f->file_ptr );
        return filesize;
}


int free_random_io( RANDOM_IO *f )
{
        if( f == NULL ) return -1;

        free( f->filename );
        if( f->file_ptr != NULL )
                fclose( f->file_ptr );
        free( f );
	return 1;
}

int set_error_random_io( RANDOM_IO *f, char *error_msg )
{
        if( f == NULL ) return -1;

        sprintf( f->error_buffer,
                "%s", error_msg );
	return 1;
}

char *get_error_random_io( RANDOM_IO *f )
{
        if( f == NULL ) return NULL;

        return f->error_buffer;
}
