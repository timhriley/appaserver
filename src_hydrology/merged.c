/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_hydrology/merged.c		*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ----------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"
#include "date.h"
#include "appaserver_library.h"
#include "hydrology_library.h"
#include "merged.h"

MERGED *merged_new( MERGED_INPUT *merged_input )
{
	MERGED *m;

	if ( ! ( m = (MERGED *)
			calloc( 1, sizeof( MERGED ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	m->merged_input = merged_input;

	m->waterquality_station_datatype_list =
		merged_waterquality_station_datatype_list(
			merged_input->waterquality_station_name_list,
			merged_input->waterquality_parameter_name_list,
			merged_input->waterquality_units_name_list );

	m->hydrology_station_datatype_list =
		merged_hydrology_station_datatype_list(
			merged_input->hydrology_station_name_list,
			merged_input->hydrology_datatype_name_list );

	return m;

} /* merged_new() */

MERGED_STATION_DATATYPE *merged_station_datatype_new( void )
{
	MERGED_STATION_DATATYPE *m;

	if ( ! ( m = (MERGED_STATION_DATATYPE *)
			calloc( 1, sizeof( MERGED_STATION_DATATYPE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return m;

} /* merged_station_datatype_new() */

MERGED_INPUT *merged_input_new( DICTIONARY *parameter_dictionary )
{
	MERGED_INPUT *m;
	LIST *record_list;

	if ( ! ( m = (MERGED_INPUT *)calloc( 1, sizeof( MERGED_INPUT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	record_list =
		dictionary_get_index_list(
			parameter_dictionary,
			"waterquality_station_parameter_units" );

	m->waterquality_station_name_list =
		piece_list( record_list, '^', 0 );

	m->waterquality_parameter_name_list =
		piece_list( record_list, '^', 1 );

	m->waterquality_units_name_list =
		piece_list( record_list, '^', 2 );

	record_list =
		dictionary_get_index_list(
			parameter_dictionary,
			"station^datatype" );

	m->hydrology_station_name_list =
		piece_list( record_list, '^', 0 );

	m->hydrology_datatype_name_list =
		piece_list( record_list, '^', 1 );

	return m;

} /* merged_input_new() */

LIST *merged_waterquality_station_datatype_list(
				LIST *waterquality_station_name_list,
				LIST *waterquality_parameter_name_list,
				LIST *waterquality_units_name_list )
{
	char *station;
	char *parameter;
	char *units;
	LIST *return_list;
	MERGED_STATION_DATATYPE *m;

	if ( !list_rewind( waterquality_station_name_list )
	||   !list_rewind( waterquality_parameter_name_list )
	||   !list_rewind( waterquality_units_name_list ) )
	{
		return (LIST *)0;
	}

	return_list = list_new();

	do {
		station = list_get( waterquality_station_name_list );
		parameter = list_get( waterquality_parameter_name_list );
		units = list_get( waterquality_units_name_list );

		m = merged_station_datatype_new();

		m->station = station;
		m->datatype = parameter;
		m->units = units;

		list_append_pointer( return_list, m );

		if ( !list_next( waterquality_parameter_name_list ) )
			break;
	
		if ( !list_next( waterquality_units_name_list ) )
		{
			fprintf( stderr,
	"ERROR in %s/%s()/%d: waterquality_units_name_list is short.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

	} while ( list_next( waterquality_station_name_list ) );

	return return_list;

} /* merged_waterquality_station_datatype_list() */
 
LIST *merged_hydrology_station_datatype_list(
				LIST *hydrology_station_name_list,
				LIST *hydrology_datatype_name_list )
{
	char *station;
	char *datatype;
	LIST *return_list;
	MERGED_STATION_DATATYPE *m;

	if ( !list_rewind( hydrology_station_name_list )
	||   !list_rewind( hydrology_datatype_name_list ) )
	{
		return (LIST *)0;
	}

	return_list = list_new();

	do {
		station = list_get( hydrology_station_name_list );
		datatype = list_get( hydrology_datatype_name_list );

		m = merged_station_datatype_new();

		m->station = station;
		m->datatype = datatype;

		list_append_pointer( return_list, m );

		if ( !list_next( hydrology_datatype_name_list ) )
			break;

	} while ( list_next( hydrology_station_name_list ) );

	return return_list;

} /* merged_hydrology_station_datatype_list() */

/* ------------------- */
/* Returns heap memory */
/* ------------------- */
char *merged_station_datatype_list_display(
				LIST *station_datatype_list )
{
	MERGED_STATION_DATATYPE *m;
	char buffer[ 65536 ];
	char *ptr = buffer;

	if ( !list_rewind( station_datatype_list ) ) return strdup( "" );

	*ptr = '\0';

	do {
		m = list_get( station_datatype_list );

		if ( m->units )
		{
			ptr += sprintf(
				ptr,
		"station = %s; datatype = %s; units = %s; rows = %d |\n",
				m->station,
				m->datatype,
				m->units,
				hash_table_length(
					m->measurement_hash_table ) );
		}
		else
		{
			ptr += sprintf(
				ptr,
				"station = %s; datatype = %s; rows = %d |\n",
				m->station,
				m->datatype,
				hash_table_length(
					m->measurement_hash_table ) );
		}

	} while ( list_next( station_datatype_list ) );

	return strdup( buffer );

} /* merged_station_datatype_list_display() */

char *merged_hydrology_sys_string(
				char *application_name,
				char *begin_date,
				char *end_date,
				char *station_name,
				char *datatype_name )
{
	char sys_string[ 1024 ];
	char where_clause[ 512 ];
	char intermediate_process[ 512 ];
	enum aggregate_statistic aggregate_statistic;

	aggregate_statistic =
		based_on_datatype_get_aggregate_statistic(
			application_name,
			datatype_name,
			aggregate_statistic_none );

	sprintf( where_clause,
 	"station = '%s' and 				      	"
 	"datatype = '%s' and 				      	"
 	"measurement_date >= '%s' and measurement_date <= '%s' 	",
		station_name,
		datatype_name,
		begin_date,
		end_date );

	sprintf( intermediate_process, 
	 "real_time2aggregate_value.e %s %d %d %d '%c' daily n %s",
	 		aggregate_statistic_get_string(
				aggregate_statistic ),
	 		MERGED_DATE_PIECE,
	 		MERGED_TIME_PIECE,
	 		MERGED_VALUE_PIECE,
			FOLDER_DATA_DELIMITER,
			end_date );

	sprintf( sys_string,
	"get_folder_data	application=%s		    "
	"			folder=measurement	    "
	"			select=\"%s\"		    "
	"			where=\"%s\"		    "
	"			quick=yes		   |"
	"sort	 					   |"
	"%s	 					   |"
	"cat						    ",
		application_name,
		MERGED_HYDROLOGY_SELECT_LIST,
		where_clause,
		intermediate_process );

	return strdup( sys_string );

} /* merged_hydrology_sys_string() */

char *merged_waterquality_sys_string(
				char *begin_date,
				char *end_date,
				char *station,
				char *parameter,
				char *units )
{
	char sys_string[ 1024 ];
	char where_clause[ 512 ];

	sprintf( where_clause,
 	"station = '%s' and 				      	"
 	"parameter = '%s' and 				      	"
 	"units = '%s' and 				      	"
 	"collection_date >= '%s' and collection_date <= '%s'	",
		station,
		parameter,
		units,
		begin_date,
		end_date );

	sprintf( sys_string,
	"get_folder_data	application=waterquality    "
	"			folder=results		    "
	"			select=\"%s\"		    "
	"			where=\"%s\"		    "
	"			quick=yes		   |"
	"cat						    ",
		MERGED_WATERQUALITY_SELECT_LIST,
		where_clause );

	return strdup( sys_string );

} /* merged_waterquality_sys_string() */

void merged_measurement_hash_table(
				char *application_name,
				char *begin_date,
				char *end_date,
				LIST *hydrology_station_datatype_list )
{
	MERGED_STATION_DATATYPE *m;
	char *sys_string;

	if ( !list_rewind( hydrology_station_datatype_list ) ) return;

	do {
		m = list_get( hydrology_station_datatype_list );

		sys_string =
			merged_hydrology_sys_string(
				application_name,
				begin_date,
				end_date,
				m->station,
				m->datatype );

		m->measurement_hash_table =
			merged_hash_table(
				sys_string );

	} while ( list_next( hydrology_station_datatype_list ) );

} /* merged_measurement_hash_table() */

void merged_results_hash_table(
				char *begin_date,
				char *end_date,
				LIST *waterquality_station_datatype_list )
{
	MERGED_STATION_DATATYPE *m;
	char *sys_string;

	if ( !list_rewind( waterquality_station_datatype_list ) ) return;

	do {
		m = list_get( waterquality_station_datatype_list );

		sys_string =
			merged_waterquality_sys_string(
				begin_date,
				end_date,
				m->station,
				m->datatype,
				m->units );

		m->measurement_hash_table =
			merged_hash_table(
				sys_string );

	} while ( list_next( waterquality_station_datatype_list ) );

} /* merged_results_hash_table() */

HASH_TABLE *merged_hash_table( char *sys_string )
{
	MERGED_MEASUREMENT *measurement;
	char input_buffer[ 1024 ];
	FILE *input_pipe;
	char date_buffer[ 16 ];
	char measurement_buffer[ 16 ];
	HASH_TABLE *hash_table;

	input_pipe = popen( sys_string, "r" );

	hash_table =
		hash_table_new_hash_table(
			hash_table_large );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( date_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );

		piece(	measurement_buffer,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		measurement =
			merged_measurement_new(
				strdup( date_buffer )
					/* measurement_date_string */ );

		if ( *measurement_buffer
		&&   timlib_strncmp( measurement_buffer, "null" ) != 0 )
		{
			measurement->measurement_value =
				atof( measurement_buffer );
		}
		else
		{
			measurement->is_null = 1;
		}

		hash_table_set_pointer(
			hash_table,
			measurement->measurement_date_string,
			measurement );
	}

	pclose( input_pipe );

	return hash_table;

} /* merged_hash_table() */

MERGED_MEASUREMENT *merged_measurement_new( char *measurement_date_string )
{
	MERGED_MEASUREMENT *m;

	if ( ! ( m = (MERGED_MEASUREMENT *)
			calloc( 1, sizeof( MERGED_MEASUREMENT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	m->measurement_date_string = measurement_date_string;
	return m;

} /* merged_measurement_new() */

HASH_TABLE *merged_key_hash_table(
				LIST *station_datatype_list )
{
	MERGED_STATION_DATATYPE *station_datatype;
	LIST *date_space_time_key_list;
	char *date_space_time_key;
	HASH_TABLE *key_hash_table;

	if ( !list_rewind( station_datatype_list ) ) return (HASH_TABLE *)0;

	key_hash_table =
		hash_table_new_hash_table(
			hash_table_large );

	do {
		station_datatype =
			list_get_pointer(
				station_datatype_list );

		date_space_time_key_list =
			hash_table_get_key_list(
				station_datatype->measurement_hash_table );

		if ( list_rewind( date_space_time_key_list ) )
		{
			do {
				date_space_time_key =
					list_get_pointer(
						date_space_time_key_list );

				hash_table_set_pointer(
					key_hash_table,
					date_space_time_key,
					(char *)0 );

			} while( list_next( date_space_time_key_list ) );
		}
	} while( list_next( station_datatype_list ) );

	return key_hash_table;

} /* merged_key_hash_table() */

LIST *merged_heading_list(	LIST *waterquality_station_datatype_list,
				LIST *hydrology_station_datatype_list )
{
	MERGED_STATION_DATATYPE *station_datatype;
	char buffer[ 128 ];
	char initial_capital_buffer[ 128 ];
	LIST *heading_list = list_new();

	list_append_pointer( heading_list, "Date" );

	/* Set the waterquality headings */
	/* ----------------------------- */
	if ( list_rewind( waterquality_station_datatype_list ) )
	{
		do {
			station_datatype =
				list_get_pointer(
					waterquality_station_datatype_list );

			sprintf(buffer,
				"%s %s %s",
				station_datatype->station,
				format_initial_capital(
					initial_capital_buffer,
					station_datatype->
						datatype ),
				station_datatype->units );

			list_append_pointer(
				heading_list,
				strdup( buffer ) );

		} while( list_next( waterquality_station_datatype_list ) );

	} /* If */

	/* Set the hydrology headings */
	/* -------------------------- */
	if ( list_rewind( hydrology_station_datatype_list ) )
	{
		do {
			station_datatype =
				list_get_pointer(
					hydrology_station_datatype_list );

			sprintf(buffer,
				"%s %s",
				station_datatype->station,
				format_initial_capital(
					initial_capital_buffer,
					station_datatype->
						datatype ) );

			list_append_pointer(
				heading_list,
				strdup( buffer ) );

		} while( list_next( hydrology_station_datatype_list ) );

	} /* If */

	return heading_list;

} /* merged_heading_list() */

/* Returns heap memory */
/* ------------------- */
char *merged_subtitle(	char *begin_date,
			char *end_date )
{
	char sub_title[ 128 ];

	sprintf(sub_title,
		"Merged from %s to %s",
		begin_date,
		end_date );

	return strdup( sub_title );

}  /* merged_subtitle() */

/* Returns static memory */
/* --------------------- */
char *merged_measurement_buffer( MERGED_MEASUREMENT *measurement )
{
	static char measurement_buffer[ 128 ];

	if ( measurement && !measurement->is_null )
	{
		sprintf(measurement_buffer,
			"%.3lf",
			measurement->
				measurement_value );
	}
	else
	if ( measurement )
	{
		strcpy( measurement_buffer, "null" );
	}
	else
	{
		strcpy( measurement_buffer, "missing" );
	}

	return measurement_buffer;

} /* merged_measurement_buffer() */

