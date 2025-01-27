/* $APPASERVER_HOME/utility/date2dayofweek.c		*/
/* ---------------------------------------------------- */
/* Input command line: piece offset of date		*/
/*                     field and piece offset		*/
/*		       of the output field.		*/
/* Input stream: pipe delimited rows			*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "environ.h"
#include "piece.h"
#include "timlib.h"
#include "date.h"

/* Constants */
/* --------- */
#define MAX_INPUT_BUFFER        65536

int main( int argc, char **argv )
{
        int full_date_piece;
	int output_piece;
        char full_date_string[ 64 ];
        char input_buffer[ MAX_INPUT_BUFFER ];
	char replace_string[ MAX_INPUT_BUFFER ];
        char delimiter;
	DATE *date;

        if ( argc != 4 )
        {
                fprintf(stderr,
                        "Usage: %s delimiter full_date_piece output_piece\n",
                        argv[ 0 ] );
                exit( 1 );
        }

        delimiter = *argv[ 1 ];
        full_date_piece = atoi( argv[ 2 ] );
        output_piece = atoi( argv[ 3 ] );

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

		if ( ! ( date =
				date_yyyy_mm_dd_new(
					full_date_string ) ) )
		{
                        fprintf(stderr,
                        "Ignoring: invalid date format in: %s of piece %d\n",
                                input_buffer, full_date_piece );
                        continue;
		}

		sprintf(replace_string,
			"%d",
			date_day_of_week( date ) );

                replace_piece(  input_buffer, 
                                delimiter,
                                replace_string,
                                output_piece );

                printf( "%s\n", input_buffer );

		free( date );

        }
	return 0;
}

