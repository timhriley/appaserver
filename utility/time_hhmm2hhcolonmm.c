/* utility/time_hhmm2hhcolonmm.c       			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "piece.h"
#include "timlib.h"

void time_hhmm2hhcolonmm(	char delimiter,
				int time_offset );

int main( int argc, char **argv )
{
	if ( argc != 3 )
	{
		fprintf( stderr,
			 "Usage: %s delimiter time_offset\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	time_hhmm2hhcolonmm( *argv[ 1 ], atoi( argv[ 2 ] ) );

	return 0;
}

void time_hhmm2hhcolonmm(	char delimiter,
				int time_offset )
{
	char buffer[ 1024 ];
	char first_part[ 1024 ];
	char last_part[ 1024 ];
	char date_string[ 128 ];
	char time_string[ 128 ];
	char hour_string[ 3 ];
	char minute_string[ 4 ];

	if ( time_offset <= 0 )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: time_offset must be positive.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return;
	}

	while( get_line( buffer, stdin ) )
	{
		if ( !piece( date_string, delimiter, buffer, time_offset - 1 ) )
		{
			fprintf( stderr,
			"Warning in %s/%s()/%d: cannot piece( %d ) in %s\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 time_offset,
				 buffer );
			continue;
		}

		if ( !piece( time_string, delimiter, buffer, time_offset ) )
		{
			fprintf( stderr,
			"Warning in %s/%s()/%d: cannot piece( %d ) in %s\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 time_offset,
				 buffer );
			continue;
		}

		if ( strlen( time_string ) > 5 )
		{
			fprintf( stderr,
	"Warning in %s/%s()/%d: expecting time length of 4 or 5 in %s\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 buffer );
			continue;
		}

		timlib_strncpy( hour_string, time_string, 2 );
		timlib_strcpy( minute_string, time_string + 2, 4 );

		trim_after_character(	first_part,
					delimiter,
					buffer,
					time_offset - 1 );

		trim_until_character(	last_part,
					delimiter,
					buffer,
					time_offset + 1 );

		if ( strcmp( time_string, "null" ) == 0 )
		{
			printf( "%s%c%s %s%c%s\n",
				first_part,
				delimiter,
				date_string,
				time_string,
				delimiter,
				last_part );
		}
		else
		{
			printf( "%s%c%s %s:%s%c%s\n",
				first_part,
				delimiter,
				date_string,
				hour_string,
				minute_string,
				delimiter,
				last_part );
		}
	}

} /* time_hhmm2hhcolonmm() */

