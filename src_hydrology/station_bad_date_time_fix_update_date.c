/* station_bad_date_time_fix_update_date.c */
/* --------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "piece.h"
#include "date.h"
#include "timlib.h"

int main( void )
{
	char input_buffer[ 1024 ];
	char station[ 128 ];
	char datatype[ 128 ];
	char date_string[ 128 ];
	char time[ 128 ];
	DATE *date;

	/* Looks like C1,stage,2000-12-10,2400 */
	/* ----------------------------------- */
	while( get_line( input_buffer, stdin ) )
	{
		piece( station, ',', input_buffer, 0 );
		piece( datatype, ',', input_buffer, 1 );
		piece( date_string, ',', input_buffer, 2 );
		piece( time, ',', input_buffer, 3 );

		date =
			date_yyyy_mm_dd_new(
				date_string );

		date_increment_days(
			date,
			1.0 );

		printf( 
"update measurement set measurement_date = '%s', measurement_time = '0000' where station = '%s' and datatype = '%s' and measurement_date = '%s' and measurement_time = '%s';\n",
			date_display_yyyy_mm_dd( date ),
			station,
			datatype,
			date_string,
			time );

		date_free( date );
	}
	return 0;
}
