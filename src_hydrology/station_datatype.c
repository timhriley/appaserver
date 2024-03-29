/* $APPASERVER_HOME/src_hydrology/station_datatype.c */
/* ------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "datatype.h"
#include "units.h"
#include "shef_datatype_code.h"
#include "station_datatype.h"
#include "timlib.h"
#include "piece.h"
#include "hydrology.h"
#include "appaserver_library.h"

LIST *station_datatype_get_station_datatype_list(
				char *application_name,
				LIST *station_list,
				LIST *datatype_list )
{
	char *station, *datatype;
	STATION_DATATYPE *station_datatype;
	LIST *station_datatype_list;

	if (!list_length( station_list )
	||  list_length( station_list ) != list_length( datatype_list ) )
	{
		return (LIST *)0;
	}

	station_datatype_list = list_new();

	list_rewind( station_list );
	list_rewind( datatype_list );

	do {
		station = list_get_string( station_list );
		datatype = list_get_string( datatype_list );

		if ( ! ( station_datatype =
				station_datatype_get_station_datatype(
					application_name,
					station,
					datatype ) ) )
		{
			return (LIST *)0;
		}

		list_append_pointer(	station_datatype_list,
					station_datatype );

		list_next( datatype_list );
	} while( list_next( station_list ) );

	return station_datatype_list;

} /* station_datatype_get_station_datatype_list() */

STATION_DATATYPE *station_datatype_get_station_datatype(
				char *application_name,
				char *station_name,
				char *datatype_name )
{
	return station_datatype_fetch_new(
				application_name,
				station_name,
				datatype_name );
}

STATION_DATATYPE *station_datatype_fetch_new(
				char *application_name,
				char *station_name,
				char *datatype_name )
{
	char *station_datatype_string;
	char sys_string[ 1024 ];
	char buffer[ 256 ];
	char *datatype_table, *station_datatype_table;
	STATION_DATATYPE *station_datatype;

	station_datatype_table =
		get_table_name( application_name, "station_datatype" );

	datatype_table = get_table_name( application_name, "datatype" );

	if ( station_name && *station_name )
	{
		sprintf( sys_string,
	"echo \"select 							   "
"units,aggregation_sum_yn,bar_graph_yn,scale_graph_to_zero_yn		   "
	"	from  %s,%s						   "
	"	where %s.datatype = %s.datatype				   "
	"	  and %s.datatype = '%s'				   "
	"	  and %s.station = '%s';\"				  |"
	"sql.e '%c'							   ",
		datatype_table, station_datatype_table,
		datatype_table, station_datatype_table,
		datatype_table, datatype_name,
		station_datatype_table, station_name,
		FOLDER_DATA_DELIMITER );
	}
	else
	{
		sprintf( sys_string,
	"echo \"select 							   "
"units,aggregation_sum_yn,bar_graph_yn,scale_graph_to_zero_yn,''	   "
	"	from  %s						   "
	"	where datatype = '%s';\"				  |"
	"sql.e '%c'							   ",
		datatype_table,
		datatype_name,
		FOLDER_DATA_DELIMITER );
	}

	station_datatype_string = pipe2string( sys_string );

	if ( !station_datatype_string )
	{
		return (STATION_DATATYPE *)0;
	}

	station_datatype = station_datatype_new();

	piece(	buffer,
		FOLDER_DATA_DELIMITER,
		station_datatype_string,
		0 );
	station_datatype->units = strdup( buffer );

	piece(	buffer,
		FOLDER_DATA_DELIMITER,
		station_datatype_string,
		1 );
	station_datatype->aggregation_sum_yn = *buffer;

	piece(	buffer,
		FOLDER_DATA_DELIMITER,
		station_datatype_string,
		2 );
	station_datatype->bar_graph_yn = *buffer;

	piece( 	buffer,
		FOLDER_DATA_DELIMITER,
		station_datatype_string,
		3 );
	station_datatype->scale_graph_zero_yn = *buffer;

	station_datatype->station_name = station_name;

	station_datatype->datatype = datatype_new( datatype_name );

	station_datatype->alias_name_list =
		station_datatype_alias_name_list(
			application_name,
			station_datatype->station_name,
			station_datatype->datatype->datatype_name );

	return station_datatype;

} /* station_datatype_fetch_new() */

STATION_DATATYPE *station_datatype_seek(
				LIST *station_datatype_list,
				char *station_name,
				char *datatype_name )
{
	STATION_DATATYPE *station_datatype;

	if ( !list_rewind( station_datatype_list ) )
		return (STATION_DATATYPE *)0;

	do {
		station_datatype = list_get_pointer( station_datatype_list );

		if ( station_name )
		{
			if ( timlib_strcmp(	station_datatype->
							station_name,
						station_name ) == 0 )
			{
				if ( !station_datatype->datatype ) continue;

				if ( timlib_strcmp(
					station_datatype->
						datatype->
						datatype_name,
					datatype_name ) == 0
				||   list_exists_string(
					datatype_name
						/* datatype_alias_name */,
					station_datatype->alias_name_list ) )
				{
					return station_datatype;
				}
			}
		}
		else
		/* --------------- */
		/* No station_name */
		/* --------------- */
		{
			if ( !station_datatype->datatype ) continue;

			if ( timlib_strcmp(
				station_datatype->
					datatype->
					datatype_name,
				datatype_name ) == 0
			||   list_exists_string(
				datatype_name /* datatype_alias_name */,
				station_datatype->alias_name_list ) )
			{
				return station_datatype;
			}
		}

	} while( list_next( station_datatype_list ) );

	return (STATION_DATATYPE *)0;

} /* station_datatype_seek() */

void station_datatype_free( STATION_DATATYPE *station_datatype )
{
	if ( !station_datatype ) return;

	if ( station_datatype->datatype )
	{
		datatype_free( station_datatype->datatype );
	}

	free( station_datatype );

} /* station_datatype_free() */

STATION_DATATYPE *station_datatype_new( void )
{
	STATION_DATATYPE *station_datatype;

	if ( ! ( station_datatype =
			(STATION_DATATYPE *)
				calloc( 1, sizeof( STATION_DATATYPE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s/%d: cannot allocate memory.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return station_datatype;

} /* station_datatype_new() */

/* Returns program memory */
/* ---------------------- */
char *station_datatype_where(	char *station,
				char *datatype )
{
	static char where[ 256 ];

	sprintf(	where,
			"station = '%s' and datatype = '%s'",
			station,
			datatype );

	return where;

} /* station_datatype_where() */

char *station_datatype_get_manipulate_agency(
				char *application_name,
				char *station,
				char *datatype )
{
	char sys_string[ 1024 ];
	char *where;

	where =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		station_datatype_where(
			station,
			datatype );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=manipulate_agency	"
		 "			folder=station_datatype		"
		 "			where=\"%s\"			",
		 application_name,
		 where );

	return pipe2string( sys_string );

} /* station_datatype_get_manipulate_agency() */

char *station_datatype_translate_datatype_name(
				DATATYPE *datatype,
				char *shef_download_code,
				/* -----------------------	*/
				/* Samples: Salinity (PSU)	*/
				/*	    Salinity		*/
				/* ----------------------- 	*/
				char *datatype_units_seek_phrase )
{
	DATATYPE_ALIAS *datatype_alias;
	char datatype_phrase[ 128 ];
	char units_phrase[ 128 ];

	if ( !datatype ) return (char *)0;

	if ( timlib_strcmp(	datatype->datatype_name,
				datatype_units_seek_phrase ) == 0 )
	{
		return datatype->datatype_name;
	}

	*datatype_phrase = '\0';
	*units_phrase = '\0';

	hydrology_parse_datatype_units_phrase(
		datatype_phrase /* in/out */,
		units_phrase	/* in/out */,
		datatype_units_seek_phrase );

	if ( timlib_strcmp(	datatype->datatype_name,
				datatype_phrase ) == 0 )
	{
		return datatype->datatype_name;
	}

	/* Shef takes precedence over alias. */
	/* --------------------------------- */
	if ( timlib_strcmp(	shef_download_code,
				datatype_phrase ) == 0 )
	{
		return datatype->datatype_name;
	}

	if ( ( datatype_alias =
			datatype_alias_seek(
				datatype->datatype_alias_list,
				datatype_phrase
					/* datatype_alias_name */ ) ) )
	{
		return datatype_alias->datatype_name;
	}

	return (char *)0;

} /* station_datatype_translate_datatype_name() */

UNITS *station_datatype_list_seek_units(
				LIST *station_datatype_list,
				/* -----------------------	*/
				/* Samples: Salinity (PSU)	*/
				/* ----------------------- 	*/
				char *datatype_units_seek_phrase )
{
	STATION_DATATYPE *station_datatype;
	DATATYPE *datatype;
	char datatype_phrase[ 128 ];
	char units_phrase[ 128 ];

	*datatype_phrase = '\0';
	*units_phrase = '\0';

	hydrology_parse_datatype_units_phrase(
		datatype_phrase /* in/out */,
		units_phrase	/* in/out */,
		datatype_units_seek_phrase );

	/* If not $DATATYPE ($UNIT) */
	/* ------------------------ */
	if ( !*units_phrase )
	{
		strcpy( units_phrase, datatype_units_seek_phrase );
	}

	if ( !list_rewind( station_datatype_list ) )
		return (UNITS *)0;

	do {
		station_datatype = list_get_pointer( station_datatype_list );

		if ( !station_datatype->datatype )
		{
			fprintf( stderr,
			"Warning in %s/%s()/%d: empty datatype for %s\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 station_datatype->station_name );
			continue;
		}

		datatype = station_datatype->datatype;

		if ( !datatype->units )
		{
			fprintf( stderr,
			"Warning in %s/%s()/%d: empty units for %s/%s\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 station_datatype->station_name,
				 datatype->datatype_name );
			continue;
		}

		if ( !datatype->units->units_name )
		{
			fprintf( stderr,
			"Warning in %s/%s()/%d: empty units_name for %s/%s\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 station_datatype->station_name,
				 datatype->datatype_name );
			continue;
		}

		if ( units_translate_units_name(
					datatype->units->units_alias_list,
					datatype->units->units_name,
					units_phrase ) )
		{
			return datatype->units;
		}

	} while( list_next( station_datatype_list ) );

	return (UNITS *)0;

} /* station_datatype_list_seek_units() */

LIST *station_datatype_fetch_list(
			char *application_name,
			char *station_name,
			/* -------------------------------------------- */
			/* Only shef_upload_datatpe_list for a station. */
			/* -------------------------------------------- */
			LIST *shef_upload_datatype_list )
{
	STATION_DATATYPE *station_datatype;
	LIST *station_datatype_list;
	char sys_string[ 128 ];
	char input_buffer[ 1024 ];
	FILE *input_pipe;

	/* Note: delimiter is '|' */
	/* ---------------------- */
	sprintf(sys_string,
	 	"station_datatype_list_all.sh ignored %s",
		station_name );

	input_pipe = popen( sys_string, "r" );

	station_datatype_list = list_new();

	while ( get_line( input_buffer, input_pipe ) )
	{
		station_datatype =
			station_datatype_parse(
				application_name,
		   		shef_upload_datatype_list,
				input_buffer );

		list_append_pointer(
			station_datatype_list,
			station_datatype );
	}

	pclose( input_pipe );
	return station_datatype_list;

} /* station_datatype_fetch_list() */

#ifdef NOT_DEFINED
/* ---------------------- */
/* Note: delimiter is '|' */
/* ---------------------- */
select="	$station_datatype.station,				\
		$datatype.datatype,					\
		$datatype.units,					\
		$datatype.bar_graph_yn,					\
		$datatype.scale_graph_to_zero_yn,			\
		$datatype.aggregation_sum_yn,				\
		$datatype.ysi_load_heading,				\
		$datatype.exo_load_heading,				\
		$datatype.set_negative_values_to_zero_yn,		\
		$datatype.calibrated_yn"
#endif

STATION_DATATYPE *station_datatype_parse(
			char *application_name,
			/* -------------------------------------------- */
			/* Only shef_upload_datatpe_list for a station. */
			/* -------------------------------------------- */
		   	LIST *shef_upload_datatype_list,
			char *input_buffer )
{
	char piece_buffer[ 128 ];
	STATION_DATATYPE *station_datatype;

	station_datatype = station_datatype_new();

	piece( piece_buffer, '|', input_buffer, 0 );
	station_datatype->station_name = strdup( piece_buffer );

	piece( piece_buffer, '|', input_buffer, 1 );
	station_datatype->datatype = datatype_new( strdup( piece_buffer ) );

	station_datatype->datatype->datatype_alias_list =
		datatype_name_alias_list(
			application_name,
			station_datatype->
				datatype->
				datatype_name );

	station_datatype->alias_name_list =
		station_datatype_alias_name_list(
			application_name,
			station_datatype->station_name,
			station_datatype->datatype->datatype_name );

	station_datatype->shef_upload_code =
		shef_datatype_code_seek_upload_code(
	   	     /* -------------------------------------------- */
	   	     /* Only shef_upload_datatpe_list for a station. */
	   	     /* -------------------------------------------- */
	   	     shef_upload_datatype_list,
	   	     station_datatype->datatype->datatype_name );

	station_datatype->shef_download_code =
		shef_datatype_code_seek_download_code(
	   	     /* -------------------------------------------- */
	   	     /* Only shef_upload_datatpe_list for a station. */
	   	     /* -------------------------------------------- */
	   	     shef_upload_datatype_list,
	   	     station_datatype->datatype->datatype_name );

	piece( piece_buffer, '|', input_buffer, 2 );
	station_datatype->datatype->units = units_new();
	station_datatype->datatype->units->units_name = strdup( piece_buffer );

	piece( piece_buffer, '|', input_buffer, 3 );
	station_datatype->datatype->bar_chart = (*piece_buffer == 'y');

	piece( piece_buffer, '|', input_buffer, 4 );
	station_datatype->datatype->scale_graph_to_zero =
		(*piece_buffer == 'y');

	piece( piece_buffer, '|', input_buffer, 5 );
	station_datatype->datatype->aggregation_sum =
		(*piece_buffer == 'y');

/* Retired.
	piece( piece_buffer, '|', input_buffer, 6 );
	station_datatype->datatype->ysi_load_heading = strdup( piece_buffer );

	piece( piece_buffer, '|', input_buffer, 7 );
	station_datatype->datatype->exo_load_heading = strdup( piece_buffer );
*/

	piece( piece_buffer, '|', input_buffer, 8 );
	station_datatype->datatype->set_negative_values_to_zero =
		(*piece_buffer == 'y');

	piece( piece_buffer, '|', input_buffer, 9 );
	station_datatype->datatype->calibrated = (*piece_buffer == 'y');

	return station_datatype;

} /* station_datatype_parse() */

LIST *station_datatype_fetch_measurement_list(
					char *application_name,
					char *station_name,
					char *datatype_name,
					char *begin_measurement_date,
					char *end_measurement_date )
{
	FILE *input_pipe;
	char where_clause[ 256 ];
	LIST *measurement_list;

	sprintf( where_clause,
		 "station = '%s' and				"
		 "datatype = '%s' and				"
		 "measurement_date between '%s' and '%s'	",
		 station_name,
		 datatype_name,
		 begin_measurement_date,
		 end_measurement_date );

	input_pipe = measurement_open_input_pipe(
			application_name,
			where_clause,
			FOLDER_DATA_DELIMITER );

	measurement_list = 
		measurement_fetch_list(
			input_pipe,
			FOLDER_DATA_DELIMITER );

	pclose( input_pipe );

	return measurement_list;

} /* station_datatype_fetch_measurement_list() */

STATION_DATATYPE *station_datatype_getset(
			LIST *station_datatype_list,
			char *application_name,
			char *station_name,
			char *datatype_name )
{
	STATION_DATATYPE *station_datatype;

	if ( ( station_datatype =
		station_datatype_seek(
			station_datatype_list,
			station_name,
			datatype_name ) ) )
	{
		return station_datatype;
	}

	station_datatype =
		station_datatype_fetch_new(
			application_name,
			strdup( station_name ),
			strdup( datatype_name ) );

	list_append_pointer( station_datatype_list, station_datatype );

	return station_datatype;
}

void station_datatype_html_display(
			LIST *station_datatype_list )
{
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *heading;
	int total = 0;

	heading = "Station,Datatype,Count";

	sprintf( sys_string,
		 "html_table.e '^^Datatype Summary' '%s' '^' 'left,left,right'",
		 heading );

	output_pipe = popen( sys_string, "w" );

	if ( list_rewind( station_datatype_list ) )
	{
		STATION_DATATYPE *station_datatype;

		do {
			station_datatype =
				list_get_pointer(
					station_datatype_list );

			fprintf(output_pipe,
				"%s^%s^%d\n",
				station_datatype->station_name,
				station_datatype->datatype->datatype_name,
				station_datatype->measurement_count );

			total += station_datatype->measurement_count;

		} while ( list_next( station_datatype_list ) );
	}

	fprintf(output_pipe,
		"Total^^%d\n",
		total );

	pclose( output_pipe );

} /* station_datatype_html_display() */

DATA_COLLECTION_FREQUENCY *station_datatype_frequency_new(
				char *begin_measurement_date,
				char *begin_measurement_time,
				int expected_count_per_day )
{
	DATA_COLLECTION_FREQUENCY *a;

	if ( ! ( a =
		(DATA_COLLECTION_FREQUENCY *)
			calloc( 1, sizeof( DATA_COLLECTION_FREQUENCY ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s/%d: cannot allocate memory.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	a->begin_measurement_date = begin_measurement_date;
	a->begin_measurement_time = begin_measurement_time;
	a->expected_count_per_day = expected_count_per_day;

	return a;

} /* station_datatype_frequency_new() */

LIST *station_datatype_frequency_list(	char *application_name,
					char *station_name,
					char *datatype_name )
{
	LIST *l;
	FILE *input_pipe;
	char *select;
	char where[ 256 ];
	char sys_string[ 1024 ];
	DATA_COLLECTION_FREQUENCY *d;
	char input_buffer[ 1024 ];
	char begin_measurement_date[ 16 ];
	char begin_measurement_time[ 16 ];
	char expected_count_per_day[ 16 ];

	select = "begin_measurement_date,begin_measurement_time,expected_count";

	sprintf( where,
		 "station = '%s' and datatype = '%s'",
		 station_name,
		 datatype_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=%s		"
		 "			where=\"%s\"		"
		 "			order=select		",
		 application_name,
		 select,
		 "data_collection_frequency",
		 where );

	input_pipe = popen( sys_string, "r" );
	l = list_new();

	while ( get_line( input_buffer, input_pipe ) )
	{
		piece(	begin_measurement_date,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	begin_measurement_time,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		piece(	expected_count_per_day,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		d = station_datatype_frequency_new(
			strdup( begin_measurement_date ),
			strdup( begin_measurement_time ),
			atoi( expected_count_per_day ) );

		list_append_pointer( l, d );
	}

	pclose( input_pipe );
	return l;

} /* station_datatype_frequency_list() */

DATA_COLLECTION_FREQUENCY *station_datatype_frequency(
			LIST *data_collection_frequency_list,
			char *measurement_date,
			char *measurement_time )
{
	DATA_COLLECTION_FREQUENCY *d;
	LIST *l = data_collection_frequency_list;

	if ( !list_rewind( l ) ) return (DATA_COLLECTION_FREQUENCY *)0;

	do {
		d = list_get( l );

		/* ------------------------------------------------------ */
		/* Likely the first row will have 0000-00-00^0000^144^... */
		/* ------------------------------------------------------ */
		if ( timlib_strcmp(
			measurement_date,
			d->begin_measurement_date ) > 0 )
		{
			continue;
		}

		if ( timlib_strcmp(
			d->begin_measurement_date,
			measurement_date ) == 0
		&&   timlib_strcmp(
			measurement_time,
			d->begin_measurement_time ) >= 0 )
		{
			continue;
		}

		if ( !list_at_first( l ) )
		{
			list_prior( l );
			d = list_get( l );
		}

		return d;

	} while ( list_next( l ) );

	return d;

} /* station_datatype_frequency() */

boolean station_datatype_frequency_reject(
				LIST *data_collection_frequency_list,
				char *measurement_date,
				char *measurement_time )
{
	DATA_COLLECTION_FREQUENCY *d;

	if ( !list_length( data_collection_frequency_list ) )
	{
		/* Don't reject */
		/* ------------ */
		return 0;
	}

	if ( ! ( d = station_datatype_frequency(
			data_collection_frequency_list,
			measurement_date,
			measurement_time ) ) )
	{
		/* Don't reject */
		/* ------------ */
		return 0;
	}

	return station_datatype_expected_count_reject(
			measurement_time,
			d->expected_count_per_day );

} /* station_datatype_frequency_reject() */

boolean station_datatype_expected_count_reject(
				char *measurement_time,
				int expected_count_per_day )
{
	int minutes;
	int remainder;

	minutes = atoi( measurement_time + 2 );

	if ( !expected_count_per_day )
	{
		/* Don't reject */
		/* ------------ */
		return 0;
	}

	if ( expected_count_per_day == 1
	||   expected_count_per_day == 8
	||   expected_count_per_day == 12
	||   expected_count_per_day == 24 )
	{
		if ( minutes ) return 1;
	}

	if ( expected_count_per_day == 48 )
	{
		remainder = minutes % 30;

		if ( minutes > 30 || remainder )
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	if ( expected_count_per_day == 96 )
	{
		remainder = minutes % 15;

		if ( minutes > 45 || remainder )
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	if ( expected_count_per_day == 144 )
	{
		remainder = minutes % 10;

		if ( minutes > 50 || remainder )
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	if ( expected_count_per_day == 240 )
	{
		remainder = minutes % 6;

		if ( minutes > 54 || remainder )
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	if ( expected_count_per_day == 288 )
	{
		remainder = minutes % 5;

		if ( minutes > 55 || remainder )
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	/* Don't reject */
	/* ------------ */
	return 0;

} /* station_datatype_expected_count_reject() */

char *station_datatype_frequency_display(
			LIST *data_collection_frequency_list )
{
	DATA_COLLECTION_FREQUENCY *d;
	char return_buffer[ 65536 ];
	char *ptr = return_buffer;

	if ( !list_rewind( data_collection_frequency_list ) ) return "";

	*ptr = '\0';

	do {
		d = list_get( data_collection_frequency_list );

		ptr += sprintf( ptr,
				"begin_measurement_date = %s; "
				"begin_measurement_time = %s; "
				"expected_count_per_day = %d\n",
				d->begin_measurement_date,
				d->begin_measurement_time,
				d->expected_count_per_day );

	} while ( list_next( data_collection_frequency_list ) );

	return strdup( return_buffer );

} /* station_datatype_frequency_display() */

#define STATION_DATATYPE_ALIAS	"station_datatype_alias"

LIST *station_datatype_alias_name_list(
				char *application_name,
				char *station_name,
				char *datatype_name )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=datatype_alias		"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 STATION_DATATYPE_ALIAS,
		 /* ---------------------- */
		 /* Returns program memory */
		 /* ---------------------- */
		 station_datatype_where(
			station_name,
			datatype_name ) );

	return pipe2list( sys_string );

} /* station_datatype_alias_name_list() */

