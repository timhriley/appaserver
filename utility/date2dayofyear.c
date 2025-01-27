/* date2dayofyear.c                             */
/* -------------------------------------------- */
/* Input command line: piece offset of date     */
/*                     field.			*/
/* Input stream: pipe delimited rows            */
/* Output:       pipe delimited rows with the   */
/*               field replaced with 2 		*/
/*		 fields -- the year and day     */
/*               of year.		        */
/*                                              */
/* Tim Riley                                    */
/* -------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "environ.h"
#include "hash_table.h"
#include "piece.h"
#include "timlib.h"

/* Constants */
/* --------- */
#define MAX_INPUT_BUFFER        65536
#define MAX_DAYS_IN_YEAR        366
#define CAL2LINEAR_EXECUTABLE   "cal2linear"

/* ---------------------------------------------------- */
/* Note: date_string_array looks like:  "1999-01-01"    */
/*                                      "1999-01-02"    */
/*                                      "1999-01-03"    */
/* ---------------------------------------------------- */
char **load_date_string_array( char *year );

char **load_date_string_array( char *year )
{
        FILE *p;
        char buffer[ 128 ];
        char **date_array_ptr;
        char **date_string_array = (char **)calloc( 
                                MAX_DAYS_IN_YEAR,
                                sizeof( char *) );
        if ( !date_string_array ) return (char **)0;
        date_array_ptr = date_string_array;
        sprintf( buffer, "%s %s", CAL2LINEAR_EXECUTABLE, year );
        p = popen( buffer, "r" );
        if ( !p ) return (char **)0;
        while( get_line( buffer, p ) ) *date_array_ptr++ = strdup( buffer );
        pclose( p );
        return date_string_array;
} /* load_date_string_array() */

/* Main */
/* ---- */
int main( int argc, char **argv )
{
        char **date_string_array;
        int full_date_piece;
        int dayofyear_int;
        char full_date_string[ 64 ];
        char year_string[ 64 ];
        char input_buffer[ MAX_INPUT_BUFFER ];
	char replace_string[ MAX_INPUT_BUFFER ];
        char delimiter;
        HASH_TABLE *date_string_array_hash_table;

        if ( argc != 3 )
        {
                fprintf(stderr,
                        "Usage: %s delimiter full_date_piece\n",
                        argv[ 0 ] );
                exit( 1 );
        }

        delimiter = *argv[ 1 ];
        full_date_piece = atoi( argv[ 2 ] );

        date_string_array_hash_table = hash_table_new( hash_table_medium );

        while( get_line( input_buffer, stdin ) )
        {
                if ( !piece( full_date_string, 
                             delimiter, 
                             input_buffer, 
                             full_date_piece ) )
                {
                        fprintf(stderr,
                        "Ignoring: not enough fields in: %s to piece %d\n",
                                input_buffer, full_date_piece );
                        continue;
                }

		piece( year_string, '-', full_date_string, 0 );

                /* See if the year exists in the hash table */
                /* ---------------------------------------- */
                date_string_array = hash_table_get(
					date_string_array_hash_table,
                                        year_string
					);

                if ( !date_string_array )
                {
                        date_string_array = 
				load_date_string_array( year_string );
                        hash_table_insert(	date_string_array_hash_table, 
                                                year_string, 
                                                date_string_array );
                }

                /* Note: the array is zero based */
                /* ----------------------------- */
		if ( ( dayofyear_int = string_array_search_offset(
					date_string_array,
					full_date_string ) ) == -1 )
		{
                        fprintf(stderr,
                        "Ignoring: (%s) cannot find day of year in (%s)\n",
                                input_buffer, 
                                full_date_string );
                        continue;
		}

		sprintf( replace_string, "%s%c%d",
			 year_string,
			 delimiter,
			 dayofyear_int + 1 );

                replace_piece(  input_buffer, 
                                delimiter,
                                replace_string,
                                full_date_piece );
                printf( "%s\n", input_buffer );
        } /* while(get_line()) */
	return 0;
} /* main() */

