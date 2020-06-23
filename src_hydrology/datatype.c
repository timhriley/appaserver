/* $APPASERVER_HOME/src_hydrology/datatype.c		*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdlib.h>
#include "hydrology.h"
#include "station_datatype.h"
#include "datatype.h"

DATATYPE_ALIAS *datatype_alias_new( void )
{
	DATATYPE_ALIAS *d;

	if ( ! ( d = (DATATYPE_ALIAS *)calloc( 1, sizeof( DATATYPE_ALIAS ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return d;

} /* datatype_alias_new() */

DATATYPE *datatype_calloc( void )
{
	DATATYPE *datatype;

	if ( ! ( datatype = (DATATYPE *)calloc( 1, sizeof( DATATYPE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return datatype;

} /* datatype_calloc() */

void datatype_free( DATATYPE *datatype )
{
	if ( !datatype ) return;

	if ( datatype->units )
	{
		units_free( datatype->units );
	}

	free( datatype );

} /* datatype_free() */

DATATYPE *datatype_new( char *datatype_name )
{
	DATATYPE *datatype = datatype_calloc();

	datatype->datatype_name = datatype_name;
	datatype->column_piece = -1;
	return datatype;

} /* datatype_new() */

DATATYPE *datatype_new_datatype(
			char *datatype_name,
			char *units_name )
{
	DATATYPE *datatype;

	datatype = datatype_new( datatype_name );

	if ( units_name )
	{
		datatype->units = units_new();
		datatype->units->units_name = units_name;
	}

	return datatype;

} /* datatype_new_datatype() */

DATATYPE *datatype_unit_record2datatype( char *record )
{
	DATATYPE *datatype;
	char datatype_name[ 128 ];
	char units[ 128 ];
	char bar_graph_yn[ 16 ];
	char scale_graph_to_zero_yn[ 16 ];
	char aggregation_sum_yn[ 16 ];
	char set_negative_values_to_zero_yn[ 16 ];
	char calibrated_yn[ 16 ];

	if ( character_count( '|', record ) != 7 )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: not 7 delimiters in (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 record );
		exit( 1 );
	}

	piece( units, '|', record, 0 );
	piece( datatype_name, '|', record, 1 );
	/* station is piece( 2 ) */
	piece( bar_graph_yn, '|', record, 3 );
	piece( scale_graph_to_zero_yn, '|', record, 4 );
	piece( aggregation_sum_yn, '|', record, 5 );
	piece( set_negative_values_to_zero_yn, '|', record, 6 );
	piece( calibrated_yn, '|', record, 7 );

	datatype =
		datatype_new_datatype(
			strdup( datatype_name ),
			strdup( units ) );

	datatype->bar_chart =
		( tolower( *bar_graph_yn ) == 'y' );

	datatype->scale_graph_to_zero =
		( tolower( *scale_graph_to_zero_yn ) == 'y' );

	datatype->aggregation_sum =
		( tolower( *aggregation_sum_yn ) == 'y' );

	datatype->set_negative_values_to_zero =
		( tolower( *set_negative_values_to_zero_yn ) == 'y' );

	datatype->calibrated =
		( tolower( *calibrated_yn ) == 'y' );

	return datatype;

} /* datatype_unit_record2datatype() */

DATATYPE *datatype_set_or_get_datatype(
				LIST *datatype_list,
				/* ------------------- */
				/* Assume stack memory */
				/* ------------------- */
				char *datatype_name )
{
	DATATYPE *datatype;

	if ( !datatype_list )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: empty datatype_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 0 );
	} 

	if ( ( datatype =
			datatype_list_seek(
				datatype_list,
				datatype_name ) ) )
	{
		return datatype;
	}

	datatype = datatype_new( strdup( datatype_name ) );
	list_append_pointer( datatype_list, datatype );

	return datatype;

} /* datatype_set_or_get_datatype() */

DATATYPE *datatype_list_seek(	LIST *datatype_list,
				char *datatype_name )
{
	DATATYPE *datatype;

	if ( !list_rewind( datatype_list ) ) return (DATATYPE *)0;

	do {
		datatype = list_get_pointer( datatype_list );

		if ( timlib_strcmp(	datatype_name,
					datatype->datatype_name ) == 0 )
		{
			return datatype;
		}

	} while( list_next( datatype_list ) );

	return (DATATYPE *)0;

} /* datatype_list_seek() */

boolean datatype_list_exists(	LIST *datatype_list,
				char *datatype_name )
{
	if ( datatype_list_seek( datatype_list, datatype_name ) )
		return 1;
	else
		return 0;

} /* datatype_list_exists() */

LIST *datatype_with_station_name_list_get_datatype_bar_graph_list(
			char *application_name,
			LIST *station_name_list )
{
	DATATYPE *datatype;
	char input_buffer[ 256 ];
	char buffer[ 256 ];
	FILE *input_pipe;
	LIST *datatype_list;
	char *station_name;

	if ( !list_rewind( station_name_list ) )
		return (LIST *)0;

	datatype_list = list_new_list();

	do {
		station_name = list_get_pointer( station_name_list );

		sprintf(buffer,
			"datatype_bar_graph4station_list.sh %s %s",
			application_name,
			station_name );

		input_pipe = popen( buffer, "r" );

		while( get_line( input_buffer, input_pipe ) )
		{
			piece( buffer, '^', input_buffer, 0 );

			if ( !datatype_list_exists(
					datatype_list,
					buffer ) )
			{
				datatype =
					datatype_new_datatype(
						strdup( buffer ),
						(char *)0 /* units */ );

				piece( buffer, '^', input_buffer, 1 );
				datatype->bar_chart = ( *buffer == 'y' );

				list_append_pointer( datatype_list, datatype );
			}
		}

		pclose( input_pipe );

	} while( list_next( station_name_list ) );

	return datatype_list;

} /* datatype_with_station_name_list_get_datatype_bar_graph_list() */

LIST *datatype_with_station_name_get_datatype_list(
			char *application_name,
			char *station_name )
{
	DATATYPE *datatype;
	char buffer[ 1024 ];
	FILE *input_pipe;
	LIST *datatype_list;

	datatype_list = list_new_list();

	sprintf(	buffer,
			"station_datatype_record_list.sh %s %s",
			application_name,
			station_name );

	input_pipe = popen( buffer, "r" );

	while( timlib_get_line( buffer, input_pipe, 1024 ) )
	{
		datatype = datatype_parse(
				application_name,
				buffer );

		list_append_pointer( datatype_list, datatype );
	}

	pclose( input_pipe );

	return datatype_list;

} /* datatype_with_station_name_get_datatype_list() */

LIST *datatype_with_station_name_list_get_datatype_list(
			char *application_name,
			LIST *station_name_list )
{
	DATATYPE *datatype;
	char buffer[ 1024 ];
	FILE *input_pipe;
	LIST *datatype_list;
	char *station_name;

	if ( !list_rewind( station_name_list ) )
		return (LIST *)0;

	datatype_list = list_new_list();

	do {
		station_name = list_get_pointer( station_name_list );

		sprintf(	buffer,
				"datatype_unit_record_list.sh %s %s %c %s",
				application_name,
				station_name,
				'n',
				"aggregate_statistic" );

		input_pipe = popen( buffer, "r" );

		while( get_line( buffer, input_pipe ) )
		{
			datatype = datatype_unit_record2datatype( buffer );

			if ( !datatype_list_exists(
					datatype_list,
					datatype->datatype_name ) )
			{
				list_append_pointer( datatype_list, datatype );
			}
		}

		pclose( input_pipe );

	} while( list_next( station_name_list ) );

	return datatype_list;

} /* datatype_with_station_name_list_get_datatype_list() */

LIST *datatype_get_datatype_list(
			char *application_name,
			char *station_name,
			char plot_for_station_check_yn,
			enum aggregate_statistic aggregate_statistic )
{
	DATATYPE *datatype;
	char buffer[ 1024 ];
	FILE *input_pipe;
	LIST *datatype_list = list_new_list();

	if ( plot_for_station_check_yn )
	{
		sprintf(buffer,
			"datatype_unit_record_list.sh %s %s '%c' %s",
			application_name,
			station_name,
			plot_for_station_check_yn,
			aggregate_statistic_get_string(
				aggregate_statistic ) );
	}
	else
	{
		sprintf(buffer,
			"datatype_unit_record_list.sh %s %s '' %s",
			application_name,
			station_name,
			aggregate_statistic_get_string(
				aggregate_statistic ) );
	}

	input_pipe = popen( buffer, "r" );

	while( get_line( buffer, input_pipe ) )
	{
		datatype = datatype_unit_record2datatype( buffer );
		list_append_pointer( datatype_list, datatype );
	}

	pclose( input_pipe );
	return datatype_list;

} /* datatype_get_datatype_list() */

char *datatype_get_units_string(		boolean *bar_graph,
						char *application_name,
						char *datatype_name )
{
	char sys_string[ 1024 ];
	char *table_name;
	char *results;
	char bar_graph_yn[ 16 ];

	table_name = get_table_name( application_name, "datatype" );

	sprintf( sys_string,
		 "echo \"select units,bar_graph_yn			 "
		 "       from %s					 "
		 "	 where datatype = '%s';\"			|"
		 "sql.e '^'						 ",
		 table_name,
		 datatype_name );

	results = pipe2string( sys_string );
	*bar_graph = 0;
	if ( !results )
	{
		return "";
	}
	else
	{
		piece( bar_graph_yn, '^', results, 1 );
		*bar_graph = (*bar_graph_yn == 'y');

		if ( *( results + strlen( results ) - 1 ) == '^' )
			*(results + strlen( results ) - 1) = '\0';
		else
			*(results + strlen( results ) - 2) = '\0';

		return results;
	}

} /* datatype_get_units_string() */

char *datatype_list_display( LIST *datatype_list )
{
	DATATYPE *datatype;
	char buffer[ 65536 ];
	char *ptr = buffer;

	if ( !list_rewind( datatype_list ) ) return "";
	*ptr = '\0';

	do {
		datatype = list_get_pointer( datatype_list );

		if ( !list_at_head( datatype_list ) )
			ptr += sprintf( ptr, ", " );

		if ( datatype->units )
		{
			ptr += sprintf( ptr,
					"%s (%s): %d",
					datatype->datatype_name,
					datatype->units->units_name,
					datatype->column_piece );
		}
		else
		{
			ptr += sprintf( ptr,
					"%s (null)",
					datatype->datatype_name );
		}

	} while( list_next( datatype_list ) );

	return strdup( buffer );

} /* datatype_list_display() */

boolean datatype_bar_chart(
			char *application_name,
			char *datatype_name )
{
	char sys_string[ 1024 ];
	char *results;
	char where[ 256 ];

	sprintf( where, "datatype = '%s'", datatype_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=bar_graph_yn	"
		 "			folder=datatype		"
		 "			where=\"%s\"		",
		 application_name,
		 where );

	results = pipe2string( sys_string );

	if ( !results )
		return 0;
	else
		return ( *results == 'y' );

} /* datatype_bar_chart() */

DATATYPE *datatype_parse(		char *application_name,
					char *record )
{
	DATATYPE *datatype;
	char datatype_name[ 128 ];
	char units[ 64 ];
	char bar_graph_yn[ 16 ];
	char scale_graph_to_zero_yn[ 16 ];
	char aggregation_sum_yn[ 16 ];
	char set_negative_values_to_zero_yn[ 16 ];
	char calibrated_yn[ 16 ];

	if ( character_count( '|', record ) != 8 )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: not 8 delimiters in (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 record );
		exit( 1 );
	}

	piece( datatype_name, '|', record, 0 );
	piece( units, '|', record, 1 );
	piece( bar_graph_yn, '|', record, 2 );
	piece( scale_graph_to_zero_yn, '|', record, 3 );
	piece( aggregation_sum_yn, '|', record, 4 );
/*
	char ysi_load_heading[ 32 ];
	char exo_load_heading[ 32 ];
	piece( ysi_load_heading, '|', record, 5 );
	piece( exo_load_heading, '|', record, 6 );
*/

	piece( set_negative_values_to_zero_yn, '|', record, 7 );
	piece( calibrated_yn, '|', record, 8 );

	datatype =
		datatype_new_datatype(
			strdup( datatype_name ),
			strdup( units ) );

	if ( !datatype->units )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: empty units.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	datatype->units->units_alias_list =
		units_fetch_units_alias_list(
			application_name,
			datatype->units->units_name );

	datatype->bar_chart =
		( tolower( *bar_graph_yn ) == 'y' );

	datatype->scale_graph_to_zero =
		( tolower( *scale_graph_to_zero_yn ) == 'y' );

	datatype->aggregation_sum =
		( tolower( *aggregation_sum_yn ) == 'y' );

	/* datatype->ysi_load_heading = strdup( ysi_load_heading ); */

	/* datatype->exo_load_heading = strdup( exo_load_heading ); */

	datatype->set_negative_values_to_zero =
		( tolower( *set_negative_values_to_zero_yn ) == 'y' );

	datatype->calibrated =
		( tolower( *calibrated_yn ) == 'y' );

	return datatype;

} /* datatype_parse() */

LIST *datatype_fetch_list( char *application_name )
{
	return datatype_list_get( application_name );
}

LIST *datatype_get_list( char *application_name )
{
	return datatype_list_get( application_name );
}

LIST *datatype_list_get( char *application_name )
{
	DATATYPE *datatype;
	char buffer[ 1024 ];
	FILE *input_pipe;
	LIST *datatype_list;

	datatype_list = list_new_list();

	sprintf(	buffer,
			"datatype_record_list.sh %s",
			application_name );

	input_pipe = popen( buffer, "r" );

	while( get_line( buffer, input_pipe ) )
	{
		datatype = datatype_parse( application_name, buffer );
		list_append_pointer( datatype_list, datatype );
	}

	pclose( input_pipe );

	return datatype_list;

} /* datatype_list_get() */

boolean datatype_bypass_data_collection_frequency(
				char *application_name,
				char *station,
				char *datatype )
{
	return datatype_get_bypass_data_collection_frequency(
				application_name,
				station,
				datatype );
}

boolean datatype_get_bypass_data_collection_frequency(
				char *application_name,
				char *station,
				char *datatype )
{
	char *select;
	char *results;
	char where[ 128 ];
	char sys_string[ 512 ];

	select = "bypass_data_collection_frequency_yn";
	sprintf( where,
		 "station = '%s' and datatype = '%s'",
		 station,
		 datatype );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=station_datatype	"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 where );

	results = pipe2string( sys_string );

	if ( !results ) return 0;

	return ( *results == 'y' );

} /* datatype_get_bypass_data_collection_frequency() */

LIST *datatype_list_get_unique_unit_list(
			LIST *datatype_list )
{
	DATATYPE *datatype;
	LIST *unit_list;

	if ( !list_rewind( datatype_list ) ) return (LIST *)0;

	unit_list = list_new();

	do {
		datatype = list_get_pointer( datatype_list );

		if ( !datatype->units ) continue;

		if ( !list_exists_string(
			unit_list,
			datatype->units->units_name ) )
		{
			list_append_pointer(
				unit_list,
				datatype->units->units_name );
		}

	} while( list_next( datatype_list ) );

	return unit_list;

} /* datatype_list_get_unique_unit_list() */

LIST *datatype_get_datatypes_for_unit(
			LIST *datatype_list,
			char *units_name )
{
	DATATYPE *datatype;
	LIST *return_list;

	if ( !list_rewind( datatype_list ) ) return (LIST *)0;

	return_list = list_new();

	do {
		datatype = list_get_pointer( datatype_list );

		if ( timlib_strcmp(
			units_name,
			datatype->units->units_name ) == 0 )
		{
			list_append_pointer( return_list, datatype );
		}

	} while( list_next( datatype_list ) );

	return return_list;

} /* datatype_get_datatypes_for_unit() */

LIST *datatype_fetch_datatype_alias_list(
					char *application_name )
{
	char sys_string[ 1024 ];
	char *folder_name;
	char *select;
	LIST *record_list;
	char *record;
	char piece_buffer[ 128 ];
	DATATYPE_ALIAS *datatype_alias;
	LIST *datatype_alias_list;

	select = "datatype_alias,datatype";
	folder_name = "datatype_alias";

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=%s	"
		 "			folder=%s	",
		 application_name,
		 select,
		 folder_name );

	record_list = pipe2list( sys_string );

	if ( !list_rewind( record_list ) ) return (LIST *)0;

	datatype_alias_list = list_new();

	do {
		record = list_get( record_list );

		datatype_alias = datatype_alias_new();

		datatype_alias->datatype_alias =
			strdup( piece(	piece_buffer,
					FOLDER_DATA_DELIMITER,
					record,
					0 ) );

		datatype_alias->datatype_name =
			strdup( piece(	piece_buffer,
					FOLDER_DATA_DELIMITER,
					record,
					1 ) );

		list_append_pointer( datatype_alias_list, datatype_alias );

	} while ( list_next( record_list ) );

	return datatype_alias_list;

} /* datatype_fetch_datatype_alias_list() */

DATATYPE_ALIAS *datatype_alias_seek(
				LIST *datatype_alias_list,
				char *datatype_alias_name )
{
	DATATYPE_ALIAS *a;

	if ( !list_rewind( datatype_alias_list ) )
		return (DATATYPE_ALIAS *)0;

	do {
		a = list_get( datatype_alias_list );

		if ( timlib_strcmp(	a->datatype_alias,
					datatype_alias_name ) == 0 ) 
		{
			return a;
		}

	} while( list_next( datatype_alias_list ) );

	return (DATATYPE_ALIAS *)0;

} /* datatype_alias_seek() */

LIST *datatype_fetch_alias_list(
				char *application_name,
				char *datatype_name )
{
	LIST *return_list;
	DATATYPE_ALIAS *datatype_alias;
	static LIST *datatype_alias_list = {0};

	if ( !datatype_alias_list )
	{
		datatype_alias_list =
			datatype_fetch_datatype_alias_list(
				application_name );
	}

	if ( !list_rewind( datatype_alias_list ) ) return (LIST *)0;

	return_list = list_new();

	do {
		datatype_alias = list_get( datatype_alias_list );

		if ( timlib_strcmp(	datatype_alias->datatype_name,
					datatype_name ) == 0 )
		{
			list_append_pointer( return_list, datatype_alias );
		}

	} while ( list_next( datatype_alias_list ) );

	return return_list;

} /* datatype_fetch_alias_list() */

char *datatype_alias_display(
			LIST *station_list,
			char *application_name,
			char *station_name,
			char *datatype_alias )
{
	DATATYPE *datatype;
	STATION *station;

	if ( ! ( station =
			station_get_or_set_station(
				station_list,
				application_name,
				station_name ) ) )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: station_get_or_set_station() failed.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( ( datatype =
			datatype_seek_phrase(
				station->station_datatype_list,
				station->station_name,
				datatype_alias ) ) )
	{
		return datatype->datatype_name;
	}

	return "";

} /* datatype_alias_display() */

/* -------------------------------- */
/* Sets units_converted_multiply_by */
/* -------------------------------- */
DATATYPE *datatype_seek_phrase(
				LIST *station_datatype_list,
				char *station_name,
				/* -----------------------	*/
				/* Samples: Salinity (PSU)	*/
				/*	    Salinity		*/
				/* ----------------------- 	*/
				char *datatype_units_seek_phrase )
{
	char *datatype_name;
	STATION_DATATYPE *station_datatype;

	if ( !datatype_units_seek_phrase
	||   !*datatype_units_seek_phrase )
	{
		return (DATATYPE *)0;
	}

	if ( ! ( datatype_name =
			hydrology_translate_datatype_name(
				station_datatype_list,
				datatype_units_seek_phrase ) ) )
	{
		char *decoded_datatype;

		/* Special codes include [mu] and [deg] */
		/* ------------------------------------ */
		decoded_datatype =
			units_search_replace_special_codes(
				datatype_units_seek_phrase );

		if ( ! ( datatype_name =
				hydrology_translate_datatype_name(
					station_datatype_list,
					decoded_datatype ) ) )
		{
			return (DATATYPE *)0;
		}
	}

	if ( ! ( station_datatype =
			station_datatype_seek(
				station_datatype_list,
				station_name,
				datatype_name ) ) )
	{
		return (DATATYPE *)0;
	}

	return station_datatype->datatype;

} /* datatype_seek_phrase() */

char *datatype_name_seek_phrase(
				LIST *station_datatype_list,
				char *station_name,
				/* -----------------------	*/
				/* Samples: Salinity (PSU)	*/
				/*	    Salinity		*/
				/* ----------------------- 	*/
				char *datatype_units_seek_phrase )
{
	DATATYPE *datatype;

	if ( ! ( datatype =
			datatype_seek_phrase(
				station_datatype_list,
				station_name,
				datatype_units_seek_phrase ) ) )
	{
		return (char *)0;
	}

	return datatype->datatype_name;

} /* datatype_name_seek_phrase() */

LIST *datatype_column_piece_datatype_list(
				LIST *datatype_list )
{
	LIST *return_list;
	DATATYPE *datatype;

	if ( !list_rewind( datatype_list ) ) return (LIST *)0;

	return_list = list_new();

	do {
		datatype = list_get( datatype_list );

		if ( datatype->column_piece > -1 )
		{
			list_append_pointer( return_list, datatype );
		}

	} while ( list_next( datatype_list ) );

	return return_list;

} /* datatype_column_piece_datatype_list() */

