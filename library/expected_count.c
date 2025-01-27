/* $APPASERVER_HOME/library/expected_count.c		   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "expected_count.h"
#include "julian.h"
#include "date.h"
#include "timlib.h"
#include "float.h"
#include "piece.h"
#include "folder.h"
#include "aggregate_level.h"

LIST *expected_count_with_string_get_expected_count_list(
				char *expected_count_list_string,
				enum aggregate_level aggregate_level )
{
	char piece_buffer[ 128 ];
	char begin_measurement_date[ 16 ];
	char begin_measurement_time[ 16 ];
	char expected_count_string[ 16 ];
	int i;
	int expected_count_int;
	EXPECTED_COUNT *expected_count;
	LIST *expected_count_list = list_new_list();

	if ( aggregate_level == aggregate_level_none
	||   aggregate_level == real_time )
	{
		for(	i = 0;
			piece(	piece_buffer,
				'|',
				expected_count_list_string,
				i );
			i++ )
		{
			if ( i == 0 )
			{
				strcpy(
				begin_measurement_date,
				EXPECTED_COUNT_DEFAULT_MINIMUM_BEGIN_DATE );
			}
			else
			{
				piece(	begin_measurement_date,
					'^',
					piece_buffer,
					0 );
			}

			if ( !piece(	begin_measurement_time,
					'^',
					piece_buffer,
					1 ) )
			{
				return (LIST *)0;
			}

			if ( !piece(	expected_count_string,
					'^',
					piece_buffer,
					2 ) )
			{
				return (LIST *)0;
			}

			expected_count_int =
				expected_count_get_int(
					expected_count_string );

			expected_count =
				expected_count_new_expected_count(
				strdup( begin_measurement_date ),
				strdup( begin_measurement_time ),
				expected_count_int );

			list_append_pointer(
				expected_count_list,
				expected_count );
		}
	}
	else
	{
		if ( aggregate_level == half_hour )
		{
			expected_count_int = 48;
		}
		else
		if ( aggregate_level == hourly )
		{
			expected_count_int = 24;
		}
		else
		if ( aggregate_level == daily )
		{
			expected_count_int = 1;
		}
		else
		{
			return expected_count_list;
		}
		expected_count =
			expected_count_new_expected_count(
				EXPECTED_COUNT_DEFAULT_MINIMUM_BEGIN_DATE,
				"0000",
				expected_count_int );

		list_append_pointer(
			expected_count_list,
			expected_count );
	}
	return expected_count_list;
} /* expected_count_with_string_get_expected_count_list() */

EXPECTED_COUNT *expected_count_new_expected_count(
			char *begin_measurement_date,
			char *begin_measurement_time,
			int expected_count_int )
{
	EXPECTED_COUNT *expected_count;

	expected_count =
		(EXPECTED_COUNT *)
			calloc( 1, sizeof( EXPECTED_COUNT ) );

	if ( !expected_count )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: memory allocation error\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !begin_measurement_time
	||   !*begin_measurement_time )
	{
		begin_measurement_time = "0000";
	}

	expected_count->begin_measurement_date = begin_measurement_date;
	expected_count->begin_measurement_time = begin_measurement_time;
	expected_count->expected_count = expected_count_int;

	expected_count->expected_count_julian =
		julian_yyyy_mm_dd_time_hhmm_to_julian(
			begin_measurement_date,
			begin_measurement_time );

	expected_count->expected_count_change_time_t =
		date_yyyy_mm_dd_time_hhmm_to_time_t(
			begin_measurement_date,
			begin_measurement_time );

	return expected_count;

} /* expected_count_new_expected_count() */

LIST *expected_count_get_expected_count_list(	char *application_name,
						char *station,
						char *datatype )
{
	char buffer[ 2048 ];
	char *table_name;
	LIST *expected_count_list = list_new_list();
	char begin_measurement_date[ 16 ];
	char begin_measurement_time[ 16 ];
	char *order = "begin_measurement_date,begin_measurement_time";
	char expected_count_string[ 16 ];
	int expected_count_int;
	FILE *input_pipe;
	EXPECTED_COUNT *expected_count;
	boolean first_time = 1;

	table_name =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		folder_table_name(
			application_name,
			"data_collection_frequency" );

	sprintf( buffer, "echo \"	select	begin_measurement_date,	    "
			 "			begin_measurement_time,	    "
			 "			expected_count		    "
			 "		from %s				    "
			 "		where station = '%s'		    "
			 "		  and datatype = '%s'		    "
			 "		order by %s;\"			   |"
			 "sql.e '^'					    ",
		 table_name,
		 station,
		 datatype,
		 order );

	input_pipe = popen( buffer, "r" );

	while( get_line( buffer, input_pipe ) )
	{
		piece( begin_measurement_date, '^', buffer, 0 );

		if ( first_time )
		{
			strcpy( begin_measurement_date,
				EXPECTED_COUNT_DEFAULT_MINIMUM_BEGIN_DATE );

			first_time = 0;
		}

		piece( begin_measurement_time, '^', buffer, 1 );
		piece( expected_count_string, '^', buffer, 2 );

		expected_count_int =
			expected_count_get_int(
				expected_count_string );

		expected_count =
			expected_count_new_expected_count(
				strdup( begin_measurement_date ),
				strdup( begin_measurement_time ),
				expected_count_int );

		list_append_pointer(
			expected_count_list,
			expected_count );
	}

	pclose( input_pipe );

	return expected_count_list;
} /* expected_count_get_expected_count_list() */

char *expected_count_list_display( LIST *expected_count_list )
{
	char return_string[ 2048 ];
	EXPECTED_COUNT *expected_count;
	char *ptr = return_string;

	*ptr = '\0';
	if ( list_rewind( expected_count_list ) )
	{
		list_push( expected_count_list );
		do {
			expected_count =
				list_get_pointer(
					expected_count_list );

/*
			if ( !list_at_head( expected_count_list ) )
				ptr += sprintf( ptr, "," );
*/

			ptr += sprintf( ptr,
					"%s",
					expected_count_display(
						expected_count ) );

		} while( list_next( expected_count_list ) );
		list_pop( expected_count_list );
	}

	return strdup( return_string );
} /* expected_count_list_display() */

char *expected_count_display( EXPECTED_COUNT *expected_count )
{
	static char return_string[ 2048 ];

	sprintf(return_string,
		"\n%s:%s (julian: %.5lf or unix:%ld) expected_count: %d",
			expected_count->begin_measurement_date,
			expected_count->begin_measurement_time,
			expected_count->
				expected_count_julian,
			expected_count->
				expected_count_change_time_t,
			expected_count->expected_count );

	return return_string;
} /* expected_count_display() */

boolean expected_count_invalid_collection_frequency_date(
			LIST *expected_count_list,
			char *begin_date )
{
	EXPECTED_COUNT *expected_count;
	if ( !list_rewind( expected_count_list ) ) return 1;

	expected_count = list_get_pointer( expected_count_list );

	if ( strcmp(	begin_date,
			expected_count->begin_measurement_date ) < 0 )
	{
		return 1;
	}
	else
	{
		return 0;
	}

} /* expected_count_invalid_collection_frequency_date() */

boolean expected_count_synchronized(
				char *application_name,
				LIST *station_name_list,
				LIST *datatype_name_list,
				char *begin_date_string,
				char *end_date_string )
{
	LIST *expected_count_list;
	char *station_name;
	char *datatype_name;
	int anchor_expected_count = 0;
	int compare_expected_count;
	boolean first_time = 1;

	if ( !list_rewind( station_name_list ) ) return 0;
	if ( !list_rewind( datatype_name_list ) ) return 0;

	if (	list_length( station_name_list ) !=
		list_length( datatype_name_list ) )
	{
		return 0;
	}
	else
	if ( list_length( station_name_list ) == 1 )
	{
		return 1;
	}

	do {
		station_name = list_get_pointer( station_name_list );
		datatype_name = list_get_pointer( datatype_name_list );

		expected_count_list =
			expected_count_get_expected_count_list(
					application_name,
					station_name,
					datatype_name );
		if ( first_time )
		{
			if ( !( anchor_expected_count =
				expected_count_get_date_range_count(
					expected_count_list,
					begin_date_string,
					end_date_string ) ) )
			{
				return 0;
			}
			first_time = 0;
			list_next( datatype_name_list );
			continue;
		}

		compare_expected_count =
			expected_count_get_date_range_count(
				expected_count_list,
				begin_date_string,
				end_date_string );

		if ( compare_expected_count != anchor_expected_count )
		{
			return 0;
		}

		list_next( datatype_name_list );

	} while( list_next( station_name_list ) );
	return 1;
} /* expected_count_synchronized() */

int expected_count_get_date_range_count(
				LIST *expected_count_list,
				char *begin_date_string,
				char *end_date_string )
{
	EXPECTED_COUNT *expected_count;
	int return_count = 0;

	if ( !list_rewind( expected_count_list ) )
	{
	       	return 0;
	}

	do {
		expected_count =
			list_get_pointer( expected_count_list );

		/* ------------------------------- */
		/* If return_count never populated */
		/* ------------------------------- */
		if ( !return_count )
		{
			/* ------------------------------ */
			/* If at or before the begin date */
			/* ------------------------------ */
			if ( strcmp(	expected_count->begin_measurement_date,
					begin_date_string ) <= 0 )
			{
				return_count = expected_count->expected_count;
			}
		}
		else
		/* ------------------------------------ */
		/* If return_count previously populated */
		/* ------------------------------------ */
		{
			/* ---------------------- */
			/* If passed the end date */
			/* ---------------------- */
			if ( strcmp(	expected_count->begin_measurement_date,
					end_date_string ) > 0 )
			{
				return return_count;
			}
			else
			/* --------------------------- */
			/* If didn't pass the end date */
			/* --------------------------- */
			{
				return_count = expected_count->expected_count;
			}
		}
	
	} while( list_next( expected_count_list ) );

	/* -------------------------------------------- */
	/* If the end_date is later than the		*/
	/* last record in DATA_COLLECTION_FREQUENCY	*/
	/* -------------------------------------------- */
	return return_count;

} /* expected_count_get_date_range_count() */

EXPECTED_COUNT *expected_count_fetch(
				LIST *expected_count_list,
				double current )
{
	EXPECTED_COUNT *expected_count;

	if ( !list_rewind( expected_count_list ) )
		return (EXPECTED_COUNT *)0;

/*
fprintf( stderr, "%s()/%d: got current = %s\n",
__FUNCTION__,
__LINE__,
julian_display_yyyy_mm_dd_hhmm( current ) );
*/

	do {
		expected_count = list_get_pointer( expected_count_list );

		if ( double_virtually_same(
			current,
			expected_count->expected_count_julian ) )
		{
			break;
		}

		if ( expected_count->expected_count_julian > current )
		{
			list_previous( expected_count_list );
			expected_count =
				list_get_pointer( expected_count_list );
			break;
		}

	} while( list_next( expected_count_list ) );

/*
fprintf( stderr, "%s()/%d: returning expected_count = %d\n",
__FUNCTION__,
__LINE__,
expected_count->expected_count );
*/

	return expected_count; 
} /* expected_count_fetch() */

int expected_count_get_int( char *expected_count_string )
{
	int i;

	i = atoi( expected_count_string );

	if ( !i ) i = DEFAULT_EXPECTED_COUNT_PER_DAY;

	return i;
} /* expected_count_get_int() */

