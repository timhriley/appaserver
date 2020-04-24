/* sfwmd_station_datatype.c */
/* ------------------------ */
#include <string.h>
#include <stdlib.h>
#include "sfwmd_station_datatype.h"
#include "hash_table.h"
#include "timlib.h"
#include "piece.h"
#include "appaserver_library.h"

char *sfwmd_month_array[] = { 	"JAN",
				"FEB",
				"MAR",
				"APR",
				"MAY",
				"JUN",
				"JUL",
				"AUG",
				"SEP",
				"OCT",
				"NOV",
				"DEC",
				(char *)0 };

HASH_TABLE *sfwmd_station_datatype_hash_table_load(
			char *application_name )
{
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char *select = "db_key,station,datatype,conversion_factor";
	char input_buffer[ 1024 ];
	char piece_buffer[ 1024 ];
	HASH_TABLE *sfwmd_station_datatype_hash_table;
	SFWMD_STATION_DATATYPE *sfwmd_station_datatype;
	char *where;

	where =
"station is not null and station <> '' and datatype is not null and datatype <> ''";

	sfwmd_station_datatype_hash_table =
		hash_table_new_hash_table(
			hash_table_medium );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			folder=%s			"
		 "			select=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 SFWMD_STATION_DATATYPE_FOLDER,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
		sfwmd_station_datatype = sfwmd_station_datatype_calloc();
		sfwmd_station_datatype->db_key = strdup( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
		sfwmd_station_datatype->station = strdup( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
		sfwmd_station_datatype->datatype = strdup( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
		sfwmd_station_datatype->conversion_factor =
			atof( piece_buffer );

		hash_table_insert(
				sfwmd_station_datatype_hash_table,
				sfwmd_station_datatype->db_key,
				sfwmd_station_datatype );
	}

	pclose( input_pipe );
	return sfwmd_station_datatype_hash_table;

} /* sfwmd_station_datatype_hash_table_load() */

SFWMD_STATION_DATATYPE *sfwmd_station_datatype_calloc( void )
{
	return (SFWMD_STATION_DATATYPE *)
			calloc( 1, sizeof( SFWMD_STATION_DATATYPE ) );
}

SFWMD_STATION_DATATYPE *sfwmd_parse(
				/* ------------------- */
				/* Expect stack memory */
				/* ------------------- */
				char *db_key,
				HASH_TABLE *sfwmd_station_datatype_hash_table,
				char *input_buffer )
{
	char process_buffer[ 1024 ];
	char piece_buffer[ 1024 ];
	SFWMD_STATION_DATATYPE *sfwmd_station_datatype;
	char last_validation_date_string[ 128 ];
	char *measurement_date_international;
	char measurement_date_string[ 128 ];
	char *last_validation_date_international;

	strcpy( process_buffer, input_buffer );
	search_replace_string( process_buffer, "\"", "" );

	/* piece db_key */
	/* ------------ */
	*db_key = '\0';
	if ( !piece( db_key, ',', process_buffer, 1 ) )
	{
		return (SFWMD_STATION_DATATYPE *)0;
	}

	if ( ! ( sfwmd_station_datatype =
			hash_table_fetch(
				sfwmd_station_datatype_hash_table,
				db_key ) ) )
	{
		return (SFWMD_STATION_DATATYPE *)0;
	}

	/* piece measurement_date */
	/* ---------------------- */
	if ( !piece( piece_buffer, ',', process_buffer, 2 ) )
	{
		return (SFWMD_STATION_DATATYPE *)0;
	}

	if ( ! ( measurement_date_international =
			sfwmd_to_yyyy_mm_dd(
				measurement_date_string,
				piece_buffer ) ) )
	{
		return (SFWMD_STATION_DATATYPE *)0;
	}

	strcpy(	sfwmd_station_datatype->measurement_date_string,
		measurement_date_international );

	/* piece measurement_value */
	/* ----------------------- */
	if ( !piece( piece_buffer, ',', process_buffer, 3 ) )
	{
		return (SFWMD_STATION_DATATYPE *)0;
	}

	if ( !*piece_buffer )
	{
		sfwmd_station_datatype->is_null = 1;
	}
	else
	{
		sfwmd_station_datatype->is_null = 0;
		sfwmd_station_datatype->measurement_value =
			atof( piece_buffer );
	}

	if ( sfwmd_station_datatype->conversion_factor )
	{
		sfwmd_station_datatype->measurement_value *=
			sfwmd_station_datatype->conversion_factor;
	}

	/* piece qualifier */
	/* --------------- */
	if ( piece( piece_buffer, ',', process_buffer, 4 ) )
	{
		if ( *piece_buffer == 'M' )
		{
			sfwmd_station_datatype->is_null = 1;
		}
		else
		if ( *piece_buffer == 'P' )
		{
			sfwmd_station_datatype->provisional = 1;
		}
		else
		{
			sfwmd_station_datatype->provisional = 0;
		}
	}

	/* piece last_validation_date */
	/* -------------------------- */
	if ( piece( piece_buffer, ',', process_buffer, 5 ) )
	{
		if ( ( last_validation_date_international =
				sfwmd_to_yyyy_mm_dd(
					last_validation_date_string,
					piece_buffer ) ) )
		{
			strcpy( sfwmd_station_datatype->
					last_validation_date,
				last_validation_date_international );
		}
	}

	return sfwmd_station_datatype;

} /* sfwmd_parse() */

char *sfwmd_to_yyyy_mm_dd( char *d, char *s )
{
	int mm_int;
	char yyyy[ 128 ], mon[ 128 ], dd[ 128 ];

	if ( ! piece( dd, '-', s, 0 ) ) return (char *)0;
	if ( ! piece( mon, '-', s, 1 ) ) return (char *)0;
	if ( ! piece( yyyy, '-', s, 2 ) ) return (char *)0;

	mm_int = sfwmd_ora_month2integer( mon );
	if ( mm_int < 0 ) return (char *)0;

	sprintf( d, 
		 "%s-%02d-%02d",
		 yyyy,
		 mm_int + 1,
		 atoi( dd ) );

	return d;

} /* sfwmd_to_yyyy_mm_dd() */

int sfwmd_ora_month2integer( char *mon )
{
	char **array_ptr = sfwmd_month_array;
	int i = 0;

	while( *array_ptr )
	{
		if ( strcmp( mon, *array_ptr ) == 0 )
		{
			return i;
		}
		else
		{
			i++;
			array_ptr++;
		}
	}
	return -1;
} /* sfwmd_ora_month2integer() */

