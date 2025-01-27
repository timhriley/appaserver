/* date_convert_dvr.c */
/* ------------------ */

#include <stdio.h>
#include <stdlib.h>
#include "date_convert.h"

void convert_static();
void convert_database();
void convert_american_sans_slashes( void );

int main()
{
	convert_static();
	/* convert_database(); */
	/* convert_american_sans_slashes(); */
	exit( 0 );
}

void convert_database()
{
	DATE_CONVERT *date_convert;

	date_convert = date_convert_new_user_format_date_convert(
		"hydrology", "tim", "06/01/01" );

	if ( !date_convert )
	{
		fprintf( stderr,
			 "Error in %s/%s(): cannot fetch from database\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	printf(
"If tim's user_date_format is american, then should be (06/01/2001) is (%s)\n",
		date_convert->return_date );

	date_convert_populate_return_date(
		date_convert->return_date,
		date_convert->source_format,
		date_convert->destination_format,
		"06/30/01" );

	printf(
"If tim's user_date_format is american, then should be (06/30/2001) is (%s)\n",
		date_convert->return_date );

	date_convert_free( date_convert );

	date_convert = date_convert_new_database_format_date_convert(
		"hydrology", "06/01/01" );

	if ( !date_convert )
	{
		fprintf( stderr,
			 "Error in %s/%s(): cannot fetch from database\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	printf(
"If database_date_format is international, then should be (2001-06-01) is (%s)\n", date_convert->return_date );

	date_convert_free( date_convert );

}

void convert_static()
{
	DATE_CONVERT *date;

	/* Source American */
	/* --------------- */
	date = date_convert_new_date_convert( 
				international,
				"06/01/01" );

	printf( "got international date = %s\n", date->return_date );
	date_convert_free( date );

	date = date_convert_new_date_convert( 
				military,
				"06/01/01" );

	printf( "got military date = %s\n", date->return_date );
	date_convert_free( date );

	/* Source Military */
	/* --------------- */
	date = date_convert_new_date_convert( 
				american,
				"01-JUN-01" );

	printf( "got american date = %s\n", date->return_date );
	date_convert_free( date );

	date = date_convert_new_date_convert( 
				international,
				"01-JUN-2001" );

	printf( "got international date = %s\n", date->return_date );
	date_convert_free( date );

	/* Source International */
	/* -------------------- */
	date = date_convert_new_date_convert( 
				american,
				"01-06-01" );

	printf( "got american date = %s\n", date->return_date );
	date_convert_free( date );

	date = date_convert_new_date_convert( 
				military,
				"01-06-01" );

	printf( "got military date = %s\n", date->return_date );
	date_convert_free( date );
}

void convert_american_sans_slashes( void )
{
	char *date_string = "10190";

	printf( "%s: should be (01/01/1990) is = (%s)\n",
		__FUNCTION__,
		date_convert_american_sans_slashes( date_string ) );
} /* convert_american_sans_slashes() */
