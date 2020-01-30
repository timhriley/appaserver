/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_hydrology/hydrology.c		*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ----------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hydrology.h"
#include "timlib.h"
#include "piece.h"
#include "column.h"
#include "date.h"
#include "date_convert.h"
#include "appaserver_library.h"

HYDROLOGY *hydrology_new( void )
{
	HYDROLOGY *h;

	if ( ! ( h = (HYDROLOGY *)calloc( 1, sizeof( HYDROLOGY ) ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	h->input.station_list = list_new();
	return h;

} /* hydrology_new() */

char *hydrology_units_name_seek_phrase(
				LIST *station_datatype_list,
				/* -----------------------	*/
				/* Samples: Salinity (PSU)	*/
				/*	    Salinity		*/
				/* ----------------------- 	*/
				char *datatype_units_seek_phrase )
{
	UNITS *units;

	if ( ! ( units =
			station_datatype_list_seek_units(
				station_datatype_list,
				datatype_units_seek_phrase ) ) )
	{
		return (char *)0;
	}

	return units->units_name;

} /* hydrology_units_name_seek_phrase() */

char *hydrology_translate_datatype_name(
				LIST *station_datatype_list,
				char *datatype_seek_phrase )
{
	STATION_DATATYPE *station_datatype;
	char *datatype_name;

	if ( !list_rewind( station_datatype_list ) )
		return (char *)0;

	if ( !datatype_seek_phrase
	||   !*datatype_seek_phrase )
	{
		return (char *)0;
	}

	do {
		station_datatype = list_get( station_datatype_list );

		if ( !station_datatype->datatype )
		{
			fprintf( stderr,
				 "Error in %s/%s()/%d: empty datatype.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		if ( ( datatype_name =
			station_datatype_translate_datatype_name(
				station_datatype->datatype,
				station_datatype->
					shef_upload_code,
				datatype_seek_phrase ) ) )
		{
			return datatype_name;
		}

	} while ( list_next( station_datatype_list ) );

	return (char *)0;

} /* hydrology_translate_datatype_name() */

/* --------------------------- */
/* Sets datatype->column_piece */
/* --------------------------- */
LIST *hydrology_get_header_column_datatype_list(
				LIST *station_datatype_list,
				char *station_name,
				char *input_filename,
				int first_column_piece )
{
	DATATYPE *datatype;
	LIST *return_list;
	char header_row[ 65536 ];
	char column_heading[ 256 ];
	int column_piece;

	if ( !timlib_get_first_line(
			header_row,
			input_filename,
			65536 ) )
	{
		fprintf( stderr,
	"Warning in %s/%s()/%d: cannot get header_row in input_filename=%s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 input_filename );

		return (LIST *)0;
	}

	return_list = list_new();

	for(	column_piece = first_column_piece;
		piece_quote_comma(
			column_heading,
			header_row,
			column_piece );
		column_piece++ )
	{
		if ( ( datatype =
			datatype_seek_phrase(
				station_datatype_list,
				station_name,
				column_heading
					/* datatype_units_seek_phrase */ ) ) )
		{
			datatype->column_piece = column_piece;

			list_append_pointer(
				return_list,
				datatype );
		}
	}

	return return_list;

} /* hydrology_get_header_column_datatype_list() */

void hydrology_parse_datatype_units_phrase(
				char *datatype_phrase	/* in/out */,
				char *units_phrase	/* in/out */,
				/* -----------------------	*/
				/* Samples: Salinity (PSU)	*/
				/*	    Salinity		*/
				/* ----------------------- 	*/
				char *datatype_seek_phrase )
{
	int str_len;

	if ( !character_exists( datatype_seek_phrase, '(' ) )
	{
		strcpy( datatype_phrase, datatype_seek_phrase );
		return;
	}

	/* Last character must be the closing paren ')'. */
	/* --------------------------------------------- */
	str_len = strlen( datatype_seek_phrase );

	if ( * ( datatype_seek_phrase + (str_len - 1) ) != ')' )
	{
		return;
	}

	piece( datatype_phrase, '(', datatype_seek_phrase, 0 );

	str_len = strlen( datatype_phrase );

	right_trim( datatype_phrase );

	piece( units_phrase, ')', datatype_seek_phrase + str_len + 2, 0 );
								/* ^ */
								/* | */
					/* Plus the space plus the paren. */
					/* ------------------------------ */

} /* hydrology_parse_datatype_units_phrase() */

MEASUREMENT *hydrology_extract_measurement(
				char *input_string,
				int column_piece )
{
	MEASUREMENT *measurement;
	char measurement_value[ 32 ];

	if ( !column_piece ) return (MEASUREMENT *)0;

	if ( !piece_quote_comma(
		measurement_value,
		input_string,
		column_piece ) )
	{
		return (MEASUREMENT *)0;
	}

	if ( !*measurement_value ) return (MEASUREMENT *)0;

	measurement = measurement_calloc();
	measurement->measurement_value = atof( measurement_value );

	return measurement;

} /* hydrology_extract_measurement() */

/* Sets station_datatype->measurement_list */
/* --------------------------------------- */
void hydrology_parse_file(
			LIST *station_datatype_list,
			FILE *error_file,
			char *application_name,
			char *input_filename,
			int date_time_piece )
{
	FILE *input_file;
	char input_string[ 4096 ];
	int line_number = 0;
	STATION_DATATYPE *station_datatype;
	MEASUREMENT *measurement;
	char american_date_time[ 128 ];
	char measurement_date[ 32 ];
	char measurement_time[ 32 ];
	char *error_message = {0};

	if ( !list_length( station_datatype_list ) ) return;

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot open %s for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 input_filename );
		exit( 1 );
	}

	while( timlib_get_line( input_string, input_file, 4096 ) )
	{
		line_number++;

		if ( line_number == 1 ) continue;

		trim( input_string );
		if ( !*input_string ) continue;

		if ( !piece_quote_comma(
			american_date_time,
			input_string,
			date_time_piece ) )
		{
			continue;
		}

		if ( !date_parse_american_date_time(
				measurement_date /* date_international */,
				measurement_time /* time_hhmm */,
				&error_message,
				american_date_time ) )
		{
			continue;
		}

		list_rewind( station_datatype_list );

		do {
			station_datatype = list_get( station_datatype_list );

			if ( !station_datatype->datatype
			||   (station_datatype->datatype->column_piece == -1 ) )
			{
				continue;
			}

			if ( !station_datatype->data_collection_frequency_list )
			{
				station_datatype->
					data_collection_frequency_list =
					   station_datatype_frequency_list(
						application_name,
						station_datatype->
							station_name,
						station_datatype->
							datatype->
							datatype_name );
			}

			if ( station_datatype_frequency_reject(
				station_datatype->
					data_collection_frequency_list,
				measurement_date,
				measurement_time ) )
			{
				fprintf(error_file,
			"Violates DATA_COLLECTION_FREQUENCY in row %d: %s\n",
					line_number,
			 	 	input_string );
				continue;
			}

			if ( ! ( measurement =
					hydrology_extract_measurement(
						input_string,
						station_datatype->
							datatype->
							column_piece ) ) )
			{
				continue;
			}

			measurement->measurement_date =
				strdup( measurement_date );

			measurement->measurement_time =
				strdup( measurement_time );

			if ( !station_datatype->measurement_list )
			{
				station_datatype->measurement_list = list_new();
			}

			list_append_pointer(
				station_datatype->measurement_list,
				measurement );

		} while ( list_next( station_datatype_list ) );
	}

	fclose( input_file );

	if ( error_message )
	{
		printf( "<h3>Errors</h3>\n" );
		printf( "<p>%s\n", error_message );
	}

} /* hydrology_parse_file() */

void hydrology_summary_table_display(
				char *station_name,
				LIST *station_datatype_list )
{
	STATION_DATATYPE *station_datatype;
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *heading_string;
	char *justify_string;
	LIST *measurement_list;

	if ( !list_rewind( station_datatype_list ) ) return;

	heading_string = "datatype,units,column,measurement_count";

	justify_string = "left,left,right";

	sprintf( sys_string,
		 "html_table.e 'Summary for station %s' '%s' '^' '%s'	 ",
		 station_name,
		 heading_string,
		 justify_string );

	output_pipe = popen( sys_string, "w" );

	do {
		station_datatype = list_get( station_datatype_list );

		if ( !station_datatype->datatype
		||   ( station_datatype->datatype->column_piece == -1 ) )
		{
			continue;
		}

		measurement_list = station_datatype->measurement_list;

		fprintf( output_pipe,
			 "%s^%s^%d^%d\n",
			 station_datatype->datatype->datatype_name,
			 (station_datatype->datatype->units)
			 	? station_datatype->datatype->units->units_name
				: "unknown",
			 station_datatype->datatype->column_piece + 1,
			 list_length( measurement_list ) );

	} while ( list_next( station_datatype_list ) );

	pclose( output_pipe );

} /* hydrology_summary_table_display() */

int hydrology_measurement_insert(
				char *station_name,
				LIST *station_datatype_list )
{
	STATION_DATATYPE *station_datatype;
	MEASUREMENT *measurement;
	char sys_string[ 1024 ];
	FILE *output_pipe;
	LIST *measurement_list;
	char *field;
	char *table;
	int load_count = 0;

	if ( !list_rewind( station_datatype_list ) ) return 0;

	table = "measurement";

	field =
"station,datatype,measurement_date,measurement_time,measurement_value";

	/* Can't have both replace=y and compress=y */
	/* ---------------------------------------- */
	sprintf( sys_string,
	"insert_statement.e table=%s field=%s del='^' replace=y	compress=n |"
	"sql.e 2>&1							   |"
	"html_paragraph_wrapper.e					    ",
		 table,
		 field );

	output_pipe = popen( sys_string, "w" );

	do {
		station_datatype = list_get( station_datatype_list );

		if ( !station_datatype->datatype ) continue;

		measurement_list = station_datatype->measurement_list;

		if ( !list_rewind( measurement_list ) ) continue;

		do {
			measurement = list_get( measurement_list );

			fprintf( output_pipe,
				 "%s^%s^%s^%s^%.3lf\n",
				 station_name,
				 station_datatype->datatype->datatype_name,
				 measurement->measurement_date,
				 measurement->measurement_time,
				 measurement->measurement_value );

			load_count++;

		} while ( list_next( measurement_list ) );

	} while ( list_next( station_datatype_list ) );

	pclose( output_pipe );
	return load_count;

} /* hydrology_measurement_insert() */

int hydrology_measurement_table_display(
				char *station_name,
				LIST *station_datatype_list )
{
	STATION_DATATYPE *station_datatype;
	MEASUREMENT *measurement;
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *heading_string;
	char *justify_string;
	LIST *measurement_list;
	int load_count;

	if ( !list_rewind( station_datatype_list ) ) return 0;

	heading_string =
"datatype,measurement_date,measurement_time,measurement_value";

	justify_string = "left,left,left,right";

	sprintf( sys_string,
	"queue_top_bottom_lines.e 500					|"
	"html_table.e 'Measurements for station %s' '%s' '^' '%s'	 ",
		 station_name,
		 heading_string,
		 justify_string );

	output_pipe = popen( sys_string, "w" );

	load_count = 0;

	do {
		station_datatype = list_get( station_datatype_list );

		if ( !station_datatype->datatype ) continue;

		measurement_list = station_datatype->measurement_list;

		if ( !list_rewind( measurement_list ) ) continue;

		do {
			measurement = list_get( measurement_list );

			fprintf( output_pipe,
				 "%s^%s^%s^%.3lf\n",
				 station_datatype->datatype->datatype_name,
				 measurement->measurement_date,
				 measurement->measurement_time,
				 measurement->measurement_value );

			load_count++;

		} while ( list_next( measurement_list ) );

	} while ( list_next( station_datatype_list ) );

	pclose( output_pipe );
	return load_count;

} /* hydrology_measurement_table_display() */

void hydrology_parse_begin_end_dates(
					char **begin_measurement_date,
					char **end_measurement_date,
					char *input_filespecification,
					char *date_heading_label,
					int date_piece )
{
	FILE *input_file;
	char input_buffer[ 2048 ];
	char measurement_date_string[ 128 ];
	char measurement_time_string[ 128 ];
	boolean got_heading = 0;
	static char local_begin_measurement_date[ 32 ];
	static char local_end_measurement_date[ 32 ];
	DATE *measurement_date = date_calloc();

	*begin_measurement_date = local_begin_measurement_date;
	*end_measurement_date = local_end_measurement_date;
	*local_begin_measurement_date = '\0';

	if ( ! ( input_file = fopen( input_filespecification, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 input_filespecification );
		exit( 1 );
	}

	timlib_reset_get_line_check_utf_16();

	while( timlib_get_line( input_buffer, input_file, 1024 ) )
	{
		/* Measurement Date */
		/* ---------------- */
		piece_quoted(	measurement_date_string,
				',',
				input_buffer,
				date_piece,
				'"' );

		if ( date_heading_label && !got_heading )
		{
			if ( hydrology_got_heading_label(
				date_heading_label,
				measurement_date_string ) )
			{
				got_heading = 1;
			}
			continue;
		}

		if ( !isdigit( *measurement_date_string ) )
			continue;

		strcpy(	measurement_date_string,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			hydrology_format_measurement_date(
				/* ------------------------ */
				/* Out: assume stack memory */
				/* ------------------------ */
				measurement_time_string,
				/* -- */
				/* In */
				/* -- */
				measurement_date_string ) );

		if ( !date_set_yyyy_mm_dd(
				measurement_date,
				measurement_date_string ) )
		{
			continue;
		}

		/* If first time. */
		/* -------------- */
		if ( !*local_begin_measurement_date )
		{
			strcpy( local_begin_measurement_date,
				date_display_yyyymmdd(
					measurement_date ) );

			strcpy( local_end_measurement_date,
				date_display_yyyymmdd(
					measurement_date ) );
			continue;
		}

		if ( strcmp(	date_display_yyyymmdd( measurement_date ),
				local_begin_measurement_date ) < 0 )
		{
			strcpy( local_begin_measurement_date,
				date_display_yyyymmdd(
					measurement_date ) );
		}
		else
		if ( strcmp(	date_display_yyyymmdd( measurement_date ),
				local_end_measurement_date ) > 0 )
		{
			strcpy( local_end_measurement_date,
				date_display_yyyymmdd(
					measurement_date ) );
		}
	}

	fclose( input_file );
	timlib_reset_get_line_check_utf_16();

} /* hydrology_parse_begin_end_dates() */

/* Returns static memory */
/* --------------------- */
char *hydrology_format_measurement_date(
				/* ------------------------ */
				/* Out: assume stack memory */
				/* ------------------------ */
				char *measurement_time_string,
				/* -- */
				/* In */
				/* -- */
				char *measurement_date_time_string )
{
	static char measurement_date_string[ 128 ];

	*measurement_date_string = '\0';

	/* -------------------------------- */
	/* Looks like: 2018-08-31T14:30:22Z */
	/* -------------------------------- */
	if ( timlib_character_exists( measurement_date_time_string, 'T' ) )
	{
		if ( hydrology_extract_zulu_date_time(
			/* --- */
			/* Out */
			/* --- */
			measurement_date_string,
			/* --- */
			/* Out */
			/* --- */
			measurement_time_string,
			/* ----------------------------------- */
			/* In: looks like 2018-08-31T14:30:22Z */
			/* ----------------------------------- */
			measurement_date_time_string ) )
		{
			return measurement_date_string;
		}
	}

	if ( timlib_character_exists( measurement_date_time_string, ' ' ) )
	{
		column(	measurement_date_string,
			0,
			measurement_date_time_string );

		column(	measurement_time_string,
			1,
			measurement_date_time_string );
	}
	else
	{
		strcpy(	measurement_date_string,
			measurement_date_time_string );
	}

	if ( timlib_character_exists( measurement_date_string, '/' ) )
	{
		char buffer[ 128 ];

		date_convert_source_american(
			buffer,
			international,
			measurement_date_string );

		strcpy( measurement_date_string, buffer );
	}

	return measurement_date_string;

} /* hydrology_format_measurement_date() */

boolean hydrology_extract_zulu_date_time(
			/* --- */
			/* Out */
			/* --- */
			char *measurement_date_string,
			/* --- */
			/* Out */
			/* --- */
			char *measurement_time_string,
			/* ----------------------------------- */
			/* In: looks like 2018-08-31T14:30:22Z */
			/* ----------------------------------- */
			char *measurement_date_time_string )
{
	char date_buffer[ 64 ];
	char time_buffer[ 64 ];
	char *hour_buffer;
	char *minute_buffer;
	static DATE *date = {0};

	if ( !timlib_character_exists( measurement_date_time_string, 'T' ) )
		return 0;

	if ( !date )
	{
		date = date_calloc();
	}

	piece( date_buffer, 'T', measurement_date_time_string, 0 );

	if ( !date_set_yyyy_mm_dd(
			date,
			date_buffer ) )
	{
		return 0;
	}

	/* --------------------- */
	/* Looks like: 14:30:22Z */
	/* --------------------- */
	piece( time_buffer, 'T', measurement_date_time_string, 1 );
	hydrology_trim_time( time_buffer );

	/* Returns static */
	/* -------------- */
	hour_buffer = hydrology_extract_hour( time_buffer );

	/* Returns static */
	/* -------------- */
	minute_buffer = hydrology_extract_minute( time_buffer );

	sprintf( time_buffer, "%s%s", hour_buffer, minute_buffer );

	if ( !date_set_time_hhmm(
			date,
			time_buffer /* hhmm */ ) )
	{
		return 0;
	}

	/* Convert from zulu */
	/* ----------------- */
	date_increment_hours( date, date_get_utc_offset() );

	date_get_yyyy_mm_dd( measurement_date_string, date );
	date_get_hhmm( measurement_time_string, date );

	return 1;

} /* hydrology_extract_zulu_date_time() */

char *hydrology_trim_time( char *measurement_time_string )
{
	char buffer[ 128 ];

	strcpy( buffer, measurement_time_string );

	trim_character( measurement_time_string, ':', buffer );

	if ( strlen( measurement_time_string ) == 3 )
	{
		strcpy( buffer, measurement_time_string );

		sprintf(	measurement_time_string,
				"0%s",
				measurement_time_string );
	}

	*( measurement_time_string + 4 ) = '\0';

	return measurement_time_string;

} /* hydrology_trim_time() */

/* Returns static */
/* -------------- */
char *hydrology_extract_hour( char *measurement_time_string )
{
	static char hour[ 3 ];

	*hour = '\0';

	if ( strlen( measurement_time_string ) != 4 )
		return hour;

	*hour = *measurement_time_string;
	*(hour + 1) = *( measurement_time_string + 1 );

	*(hour + 2) = '\0';

	return hour;

} /* hydrology_extract_hour() */

/* Returns static */
/* -------------- */
char *hydrology_extract_minute( char *measurement_time_string )
{
	static char minute[ 3 ];

	*minute = '\0';

	if ( strlen( measurement_time_string ) != 4 )
		return minute;

	*minute = *( measurement_time_string + 2 );
	*(minute + 1) = *( measurement_time_string + 3 );

	*(minute + 2) = '\0';

	return minute;

} /* hydrology_extract_minute() */

boolean hydrology_got_heading_label(
				char *date_heading_label,
				char *heading_buffer )
{
	if ( instr( date_heading_label, heading_buffer, 1 ) > -1 )
		return 1;
	else
		return 0;

} /* hydrology_got_heading_label() */

