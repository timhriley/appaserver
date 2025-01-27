/* $APPASERVER_HOME/utilility/dayofyear2date.c         	*/
/* -------------------------------------------- 	*/
/* Input command line: piece offset of day of   	*/
/*                     year field and piece     	*/
/*                     offset of year.          	*/
/* Input stream: pipe delimited rows            	*/
/* Output:       pipe delimited rows with the   	*/
/*               field replaced with the full   	*/
/*               date in YYYY-MM-DD format.     	*/
/*                                              	*/
/* ---------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "hash_table.h"
#include "date.h"

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
        char *date_string;
        int dayofyear_piece;
        int dayofyear_int;
        int year_piece;
        char dayofyear_string[ 1024 ];
        char year_string[ 1024 ];
        char input_buffer[ MAX_INPUT_BUFFER ];
        char delimiter;
        HASH_TABLE *date_string_array_hash_table;
	int exclude_future_dates;
	DATE *today = {0};

        if ( argc != 5 )
        {
                fprintf(stderr,
"Usage: %s delimiter year_piece dayofyear_piece exclude_future_dates_yn\n",
                        argv[ 0 ] );
                exit( 1 );
        }

        delimiter = *argv[ 1 ];
        year_piece = atoi( argv[ 2 ] );
        dayofyear_piece = atoi( argv[ 3 ] );
	exclude_future_dates = ( *argv[ 4 ] == 'y' );

        date_string_array_hash_table = hash_table_new( hash_table_medium );

	if ( exclude_future_dates )
	{
		today = date_today_new( date_utc_offset() );
	}

        while( get_line( input_buffer, stdin ) )
        {
                if ( !piece( year_string, 
                             delimiter, 
                             input_buffer, 
                             year_piece ) )
                {
                        fprintf(stderr, "%s\n", input_buffer );
                        continue;
                }

		if ( exclude_future_dates
		&&   date_year_in_future( today, atoi( year_string ) ) )
		{
                        fprintf(stderr, "%s\n", input_buffer );
                        continue;
		}

                if ( !piece( dayofyear_string, 
                             delimiter, 
                             input_buffer, 
                             dayofyear_piece ) )
                {
                        fprintf(stderr, "%s\n", input_buffer );
                        continue;
                }

                dayofyear_int = atoi( dayofyear_string );
                if ( dayofyear_int <= 0 || dayofyear_int > MAX_DAYS_IN_YEAR)
                {
                        fprintf(stderr, "%s\n", input_buffer );
                        continue;
                }

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
                date_string = date_string_array[ dayofyear_int - 1 ];
                if ( !date_string )
                {
                        fprintf(stderr, "%s\n", input_buffer );
                        continue;
                }

                replace_piece(  input_buffer, 
                                delimiter,
                                date_string,
                                dayofyear_piece );
                printf( "%s\n", input_buffer );
        } /* while(get_line()) */
	return 0;
} /* main() */

