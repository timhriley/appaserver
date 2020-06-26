/* $APPASERVER_HOME/src_hydrology/shef_datatype_code.c */
/* --------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "list.h"
#include "shef_datatype_code.h"
#include "appaserver_library.h"
#include "piece.h"
#include "hydrology.h"

SHEF_UPLOAD_AGGREGATE_MEASUREMENT *
		shef_upload_aggregate_measurement_new(
			char *station,
			char *datatype,
			char *measurement_date,
			char *measurement_time,
			double measurement_value )
{
	SHEF_UPLOAD_AGGREGATE_MEASUREMENT *s;

	s = (SHEF_UPLOAD_AGGREGATE_MEASUREMENT *)
		calloc( 1, sizeof( SHEF_UPLOAD_AGGREGATE_MEASUREMENT ) );

	if ( !s )
	{
		fprintf( stderr, 
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	s->station = station;
	s->datatype = datatype;
	s->measurement_date = measurement_date;
	s->measurement_time = measurement_time;
	s->measurement_value = measurement_value;

	return s;

} /* shef_upload_aggregate_measurement_new() */

SHEF_UPLOAD_DATATYPE *shef_upload_datatype_new( void )
{
	SHEF_UPLOAD_DATATYPE *s;

	if ( ! ( s = (SHEF_UPLOAD_DATATYPE *)
			calloc( 1, sizeof( SHEF_UPLOAD_DATATYPE ) ) ) )
	{
		fprintf( stderr, 
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return s;

} /* shef_upload_datatype_new() */

SHEF_DATATYPE_CODE *shef_datatype_code_new( char *application_name )
{
	SHEF_DATATYPE_CODE *s;

	s = (SHEF_DATATYPE_CODE *)calloc( 1, sizeof( SHEF_DATATYPE_CODE ) );

	s->shef_upload_datatype_list = 
		shef_fetch_upload_datatype_list(
			application_name );

	if ( !list_length( s->shef_upload_datatype_list ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot fetch from SHEF_UPLOAD_DATATYPE\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	s->shef_download_datatype_list =
		shef_datatype_fetch_download_datatype_list(
			application_name );

	return s;

} /* shef_datatype_code_new() */

char *shef_datatype_code_get_shef_download_code(
			char *station, 
			char *datatype_string,
			LIST *shef_download_datatype_list )
{
	SHEF_DOWNLOAD_DATATYPE *datatype;
	char datatype_string_upper_case[ 128 ];

	strcpy( datatype_string_upper_case, datatype_string );
	up_string( datatype_string_upper_case );

	if ( list_rewind( shef_download_datatype_list ) )
	{
		do {
			datatype =
				list_get_pointer(
					shef_download_datatype_list );

			if ( timlib_strcmp(
				datatype_string_upper_case, 
				datatype->datatype_name ) == 0 
			&&   timlib_strcmp(
				station,
				datatype->station_name ) == 0 )
			{
				return datatype->shef_download_code;
			}
		} while( list_next( shef_download_datatype_list ) );
	}

	return (char *)0;

} /* shef_datatype_code_get_shef_download_code() */

char *shef_upload_datatype_get_key(	char *datatype_name,
					char *measurement_date,
					char *measurement_time )
{
	static char key[ 128 ];
	char hour[ 3 ] = {0};

	strncpy( hour, measurement_time, 2 );

	sprintf(	key,
			"%s^%s^%s",
			datatype_name,
			measurement_date,
			hour );

	return key;

} /* shef_upload_datatype_get_key() */

/* Are the last two characters in the shef code MM? */
/* ------------------------------------------------ */
boolean shef_is_min_max(	int *str_len,
				char *shef_code_upper_case )
{
	*str_len = strlen( shef_code_upper_case );

	if ( *str_len <= 2 ) return 0;

	return ( *( shef_code_upper_case + *str_len - 2 ) == 'M'
		&&   *( shef_code_upper_case + *str_len - 1 ) == 'M' );

} /* shef_is_min_max() */

SHEF_UPLOAD_DATATYPE *shef_get_upload_datatype(
				char *station_name,
				char *shef_upload_code,
				LIST *shef_upload_datatype_list )
{
	SHEF_UPLOAD_DATATYPE *datatype;

	if ( !list_rewind( shef_upload_datatype_list ) )
		return (SHEF_UPLOAD_DATATYPE *)0;

	do {
		datatype = list_get( shef_upload_datatype_list );

		if ( !station_name )
		{
			if ( timlib_strcmp(
				shef_upload_code,
				datatype->shef_upload_code ) == 0 )
			{
				return datatype;
			}
		}
		else
		{
			if ( timlib_strcmp(
					shef_upload_code,
					datatype->shef_upload_code ) == 0
			&&   timlib_strcmp(
					station_name,
					datatype->station_name ) == 0 )
			{
				return datatype;
			}
		}

	} while( list_next( shef_upload_datatype_list ) );

	return (SHEF_UPLOAD_DATATYPE *)0;

} /* shef_get_upload_datatype() */

LIST *shef_station_fetch_upload_datatype_list(
				char *application_name,
				char *station_name )
{
	LIST *return_list;
	SHEF_UPLOAD_DATATYPE *shef_upload_datatype;
	static SHEF_DATATYPE_CODE *shef_datatype_code = {0};

	if ( !shef_datatype_code )
	{
		shef_datatype_code =
			shef_datatype_code_new(
				application_name );
	}

	if ( !list_rewind( shef_datatype_code->shef_upload_datatype_list ) )
		return (LIST *)0;

	return_list = list_new();

	do {
		shef_upload_datatype =
			list_get(
				shef_datatype_code->
					shef_upload_datatype_list );

		if ( timlib_strcmp(	shef_upload_datatype->station_name,
					station_name ) == 0 )
		{
			list_append_pointer(
				return_list,
				shef_upload_datatype );
		}
				
	} while ( list_next( shef_datatype_code->shef_upload_datatype_list ) );

	return return_list;

} /* shef_station_fetch_upload_datatype_list() */

LIST *shef_fetch_upload_datatype_list( char *application_name )
{
	char input_line[ 128 ];
	char station_string[ 128 ];
	char datatype_string[ 128 ];
	char  shef_upload_code[ 128 ];
	char sys_string[ 1024 ];
	SHEF_UPLOAD_DATATYPE *datatype;
	LIST *datatype_list;
	FILE *p;

	datatype_list = list_new();

	sprintf( sys_string,
		 "shef_upload_list.sh %s",
		 application_name );

	p = popen( sys_string, "r" );
	while( get_line( input_line, p ) )
	{
		piece( station_string, '|', input_line, 0 );
		piece( datatype_string, '|', input_line, 1 );
		piece( shef_upload_code, '|', input_line, 2 );

		datatype = (SHEF_UPLOAD_DATATYPE *)
				calloc( 1, sizeof( SHEF_UPLOAD_DATATYPE ) );

		datatype->station_name = strdup( station_string );
		datatype->datatype_name = strdup( datatype_string );
		datatype->shef_upload_code = strdup( shef_upload_code );

		list_append_pointer( datatype_list, datatype );
	}
	pclose( p );
	return datatype_list;
} /* shef_fetch_upload_datatype_list() */

LIST *shef_datatype_fetch_download_datatype_list(
				char *application_name )
{
	char input_line[ 128 ];
	char station[ 128 ], datatype_string[ 128 ], shef_download_code[ 128 ];
	char sys_string[ 1024 ];
	SHEF_DOWNLOAD_DATATYPE *datatype;
	LIST *datatype_list;
	FILE *p;

	datatype_list = list_new();

	sprintf( sys_string,
		 "shef_download_list.sh %s", application_name );

	p = popen( sys_string, "r" );
	while( get_line( input_line, p ) )
	{
		piece( station, '|', input_line, 0 );
		piece( datatype_string, '|', input_line, 1 );
		piece( shef_download_code, '|', input_line, 2 );

		datatype = (SHEF_DOWNLOAD_DATATYPE *)
				calloc( 1, sizeof( SHEF_DOWNLOAD_DATATYPE ) );
		datatype->station_name = strdup( station );
		datatype->datatype_name = strdup( datatype_string );
		datatype->shef_download_code = strdup( shef_download_code );

		list_append_pointer( datatype_list, datatype );
	}
	pclose( p );

	return datatype_list;

} /* shef_datatype_fetch_download_datatype_list() */

char *shef_datatype_code_get_upload_min_max_datatype(
				SHEF_UPLOAD_AGGREGATE_MEASUREMENT **
					shef_upload_aggregate_measurement,
				HASH_TABLE *shef_upload_hash_table,
				char *station,
				char *datatype_name,
				char *measurement_date,
				char *measurement_time,
				double measurement_value )
{
	char *key;
	char other_datatype[ 128 ];
	static char return_datatype[ 128 ];
	char datatype_lower_case[ 128 ];
	SHEF_UPLOAD_AGGREGATE_MEASUREMENT *
			local_shef_upload_aggregate_measurement;

	*shef_upload_aggregate_measurement =
		(SHEF_UPLOAD_AGGREGATE_MEASUREMENT *)0;

	timlib_strcpy( datatype_lower_case, datatype_name, 128 );
	low_string( datatype_lower_case );

	key = shef_upload_datatype_get_key(
					datatype_lower_case,
					measurement_date,
					measurement_time );

	if ( ( local_shef_upload_aggregate_measurement =
			hash_table_fetch_pointer(
				shef_upload_hash_table,
				key ) ) )
	{
		if (	measurement_value <=
			local_shef_upload_aggregate_measurement->
				measurement_value )
		{
			sprintf( return_datatype,
				 "%s_min",
				 datatype_lower_case );

			sprintf( other_datatype,
				 "%s_max",
				 datatype_lower_case );
		}
		else
		{
			sprintf( return_datatype,
				 "%s_max",
				 datatype_lower_case );

			sprintf( other_datatype,
				 "%s_min",
				 datatype_lower_case );
		}

		local_shef_upload_aggregate_measurement->datatype =
			strdup( other_datatype );

		*shef_upload_aggregate_measurement =
			local_shef_upload_aggregate_measurement;

		return return_datatype;
	}

	local_shef_upload_aggregate_measurement =
		shef_upload_aggregate_measurement_new(
			strdup( station ),
			strdup( datatype_lower_case ),
			strdup( measurement_date ),
			strdup( measurement_time ),
			measurement_value );

	hash_table_set_pointer(
		shef_upload_hash_table,
		strdup( key ),
		local_shef_upload_aggregate_measurement );

	return SHEF_AGGREGATE_STUB;

} /* shef_datatype_code_get_upload_min_max_datatype() */

LIST *shef_upload_datatype_fetch_list(
				char *application_name )
{
	char sys_string[ 1024 ];
	char *folder_name;
	char *select;
	LIST *record_list;
	char *record;
	char piece_buffer[ 128 ];
	SHEF_UPLOAD_DATATYPE *a;
	LIST *return_list;

	select = "station,shef_upload_code,datatype";
	folder_name = "shef_upload_datatype";

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=%s	"
		 "			folder=%s	",
		 application_name,
		 select,
		 folder_name );

	record_list = pipe2list( sys_string );

	if ( !list_rewind( record_list ) ) return (LIST *)0;

	return_list = list_new();

	do {
		record = list_get( record_list );

		a = shef_upload_datatype_new();

		a->station_name =
			strdup( piece(	piece_buffer,
					FOLDER_DATA_DELIMITER,
					record,
					0 ) );

		a->shef_upload_code =
			strdup( piece(	piece_buffer,
					FOLDER_DATA_DELIMITER,
					record,
					1 ) );

		a->datatype_name =
			strdup( piece(	piece_buffer,
					FOLDER_DATA_DELIMITER,
					record,
					2 ) );

		list_append_pointer( return_list, a );

	} while ( list_next( record_list ) );

	return return_list;

} /* shef_upload_datatype_fetch_list() */

char *shef_datatype_code_seek_download_code(
			/* ---------------------------------------------- */
			/* Only shef_download_datatpe_list for a station. */
			/* ---------------------------------------------- */
			LIST *shef_download_datatype_list,
			char *datatype_name )
{
	SHEF_DOWNLOAD_DATATYPE *s;

	if ( !list_rewind( shef_download_datatype_list ) )
		return ( char *)0;

	do {
		s = list_get( shef_download_datatype_list );

		if ( timlib_strcmp( s->datatype_name, datatype_name ) == 0 )
			return s->shef_download_code;

	} while ( list_next( shef_download_datatype_list ) );

	return (char *)0;
}

char *shef_datatype_code_seek_upload_code(
			/* -------------------------------------------- */
			/* Only shef_upload_datatpe_list for a station. */
			/* -------------------------------------------- */
			LIST *shef_upload_datatype_list,
			char *datatype_name )
{
	SHEF_UPLOAD_DATATYPE *s;

	if ( !list_rewind( shef_upload_datatype_list ) )
		return ( char *)0;

	do {
		s = list_get( shef_upload_datatype_list );

		if ( timlib_strcmp( s->datatype_name, datatype_name ) == 0 )
			return s->shef_upload_code;

	} while ( list_next( shef_upload_datatype_list ) );

	return (char *)0;

} /* shef_datatype_code_seek_upload_code() */

SHEF_UPLOAD_DATATYPE *shef_datatype_code_seek_upload_datatype(
				LIST *shef_upload_datatype_list,
				char *shef_upload_code )
{
	return shef_upload_datatype_seek(
				shef_upload_datatype_list,
				(char *)0 /* station_name */,
				shef_upload_code );
}

SHEF_UPLOAD_DATATYPE *shef_upload_datatype_seek(
				LIST *shef_upload_datatype_list,
				char *station_name,
				char *shef_upload_code )
{
	SHEF_UPLOAD_DATATYPE *a;

	if ( !list_rewind( shef_upload_datatype_list ) )
		return (SHEF_UPLOAD_DATATYPE *)0;

	do {
		a = list_get( shef_upload_datatype_list );

		if ( station_name )
		{
			if ( timlib_strcmp(	a->station_name,
						station_name ) == 0
			&&   timlib_strcmp(	a->shef_upload_code,
						shef_upload_code ) == 0 )
			{
				return a;
			}
		}
		else
		/* --------------- */
		/* No station_name */
		/* --------------- */
		{
			if ( timlib_strcmp(	a->shef_upload_code,
						shef_upload_code ) == 0 )
			{
				return a;
			}
		}

	} while( list_next( shef_upload_datatype_list ) );

	return (SHEF_UPLOAD_DATATYPE *)0;

} /* shef_upload_datatype_seek() */

LIST *shef_datatype_code_station_get_upload_datatype_list(
				LIST *shef_upload_datatype_list,
				char *station_name,
				char *datatype_name )
{
	LIST *return_list;
	SHEF_UPLOAD_DATATYPE *s;

	if ( !list_rewind( shef_upload_datatype_list ) )
		return (LIST *)0;

	return_list = list_new();

	do {
		s = list_get( shef_upload_datatype_list );

		if ( timlib_strcmp( s->station_name, station_name ) == 0 )
		{
			list_append_pointer( return_list, s );
		}
		else
		if ( timlib_strcmp( s->datatype_name, datatype_name ) == 0 )
		{
			list_append_pointer( return_list, s );
		}

	} while ( list_next( shef_upload_datatype_list ) );

	return return_list;

} /* shef_datatype_code_station_get_upload_datatype_list() */

LIST *shef_datatype_code_fetch_upload_datatype_list(
				char *application_name,
				char *station_name,
				char *datatype_name )
{
	static SHEF_DATATYPE_CODE *shef_datatype_code = {0};

	if ( !shef_datatype_code )
	{
		shef_datatype_code =
			shef_datatype_code_new(
				application_name );
	}

	return shef_datatype_code_station_get_upload_datatype_list(
			shef_datatype_code->shef_upload_datatype_list,
			station_name,
			datatype_name );

} /* shef_datatype_code_fetch_upload_datatype_list() */

char *shef_datatype_code_translate_datatype_name(
				LIST *shef_upload_datatype_list,
				char *shef_upload_code )
{
	SHEF_UPLOAD_DATATYPE *shef_upload_datatype;

	if ( ( shef_upload_datatype =
		shef_get_upload_datatype(
			(char *)0 /* station_name */,
			shef_upload_code,
			shef_upload_datatype_list ) ) )
	{
		return shef_upload_datatype->datatype_name;
	}
	else
	{
		return (char *)0;
	}

} /* shef_datatype_code_translate_datatype_name() */

/* -------------------------------------------- */
/* If aggregate measurement, then this returns	*/
/* SHEF_AGGREGATE_STUB.				*/
/* -------------------------------------------- */
char *shef_datatype_code_get_upload_datatype(
				SHEF_UPLOAD_AGGREGATE_MEASUREMENT **
					shef_upload_aggregate_measurement,
				char *application_name,
				char *station_name,
				char *shef_code,
				char *measurement_date,
				char *measurement_time,
				double measurement_value )
{
	char *datatype_name;
	char local_shef_code[ 128 ];
	boolean is_aggregate_measurement = 0;
	int str_len;
	static HYDROLOGY *hydrology = {0};
	STATION *station;
	static HASH_TABLE *shef_upload_hash_table = {0};

	if ( !hydrology )
	{
		hydrology = hydrology_new();
	}

	if ( !shef_upload_hash_table )
	{
		shef_upload_hash_table = hash_table_new( HASH_TABLE_MEDIUM );
	}

	strcpy( local_shef_code, shef_code );

	if ( measurement_date )
	{
		if ( ( is_aggregate_measurement =
				shef_is_min_max(
					&str_len,
					local_shef_code ) ) )
		{
			/* Trim off the MM */
			/* --------------- */
			*( local_shef_code + str_len - 2 ) = '\0';
		}
	}

	station =
		station_get_or_set_station(
			hydrology->input.station_list,
			application_name,
			station_name );

	if ( ! ( datatype_name =
			shef_datatype_code_translate_datatype_name(
				station->shef_upload_datatype_list,
				local_shef_code
					/* shef_upload_code */ ) ) )
	{
		datatype_name =
			hydrology_translate_datatype_name(
				station->station_datatype_list,
				local_shef_code /* datatype_name */ );
	}

	if ( is_aggregate_measurement )
	{
		return shef_datatype_code_get_upload_min_max_datatype(
				shef_upload_aggregate_measurement,
				shef_upload_hash_table,
				station->station_name,
				datatype_name,
				measurement_date,
				measurement_time,
				measurement_value );
	}

	return datatype_name;

} /* shef_datatype_code_get_upload_datatype() */

