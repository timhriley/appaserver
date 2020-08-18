/* ---------------------------------------------------------	*/
/* src_hydrology/measurement_date_within_range.c		*/
/* ---------------------------------------------------------	*/
/* Freely available software: see Appaserver.org		*/
/* ---------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "query.h"
#include "date.h"
#include "basename.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "timlib.h"
#include "column.h"
#include "piece.h"
#include "list.h"
#include "environ.h"
#include "julian.h"
#include "boolean.h"
#include "process.h"
#include "dictionary.h"
#include "session.h"
#include "application.h"
#include "hydrology_library.h"
#include "station.h"
#include "datatype.h"
#include "units.h"
#include "hydrology.h"
#include "julian.h"
#include "name_arg.h"

/* Structures */
/* ---------- */

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void execute(		JULIAN *begin_date_julian,
			JULIAN *end_date_julian,
			char *delimiter );

void setup_arg(		NAME_ARG *arg, int argc, char **argv );

void fetch_parameters(	char **begin_date,
			char **begin_time,
			char **end_date,
			char **end_time,
			char **delimiter,
			NAME_ARG *arg );

int main( int argc, char **argv )
{
	char *begin_date;
	char *begin_time;
	char *end_date;
	char *end_time;
	char *delimiter;
	NAME_ARG *arg;
	JULIAN *begin_date_julian;
	JULIAN *end_date_julian;

	arg = name_arg_new( argv[ 0 ] );

	setup_arg( arg, argc, argv );

	fetch_parameters(
		&begin_date,
		&begin_time,
		&end_date,
		&end_time,
		&delimiter,
		arg );

	if ( strcmp( begin_time, "begin_time" ) == 0 )
	{
		begin_time = "0000";
	}

	if ( strcmp( end_time, "end_time" ) == 0 )
	{
		end_time = "2359";
	}

	begin_date_julian = 
		julian_yyyy_mm_dd_hhmm_new(
			begin_date,
			begin_time );

	end_date_julian = 
		julian_yyyy_mm_dd_hhmm_new(
			end_date,
			end_time );

	execute(	begin_date_julian,
			end_date_julian,
			delimiter );

	return 0;

} /* main() */

void execute(		JULIAN *begin_date_julian,
			JULIAN *end_date_julian,
			char *delimiter )
{
	char input_buffer[ 1024 ];
	MEASUREMENT *measurement;
	JULIAN *measurement_date_time_julian;

	if ( !begin_date_julian
	||   !end_date_julian )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty julian.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	measurement_date_time_julian = julian_new_julian( 0.0 );

	while ( timlib_get_line( input_buffer, stdin, 1024 ) )
	{
		if ( ! ( measurement = measurement_parse(
						input_buffer,
						*delimiter ) ) )
		{
			fprintf( stderr,
				 "Error in %s/%s()/%d: cannot parse [%s].\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 input_buffer );
			exit( 1 );
		}

		julian_set_yyyy_mm_dd_hhmm(
			measurement_date_time_julian,
			measurement->measurement_date,
			measurement->measurement_time);

		if (	measurement_date_time_julian->current <
			begin_date_julian->current )
		{
			fprintf( stderr,
				 "Ignored too early: %s\n",
				 input_buffer );
			continue;
		}

		if (	measurement_date_time_julian->current >
			end_date_julian->current )
		{
			fprintf( stderr,
				 "Ignored too late: %s\n",
				 input_buffer );
			continue;
		}

		measurement_text_output(
			measurement,
			*delimiter,
			0 /* not insert_null_values */ );
	}
}

void fetch_parameters(	char **begin_date,
			char **begin_time,
			char **end_date,
			char **end_time,
			char **delimiter,
			NAME_ARG *arg )
{
	*begin_date = fetch_arg( arg, "begin_date" );
	*begin_time = fetch_arg( arg, "begin_time" );
	*end_date = fetch_arg( arg, "end_date" );
	*end_time = fetch_arg( arg, "end_time" );
	*delimiter = fetch_arg( arg, "delimiter" );

}

void setup_arg( NAME_ARG *arg, int argc, char **argv )
{
        int ticket;

        ticket = add_valid_option( arg, "begin_date" );
        ticket = add_valid_option( arg, "begin_time" );

        ticket = add_valid_option( arg, "end_date" );
        ticket = add_valid_option( arg, "end_time" );

        ticket = add_valid_option( arg, "delimiter" );
        set_default_value( arg, ticket, "^" );

        ins_all( arg, argc, argv );

}

