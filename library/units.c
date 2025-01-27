/* -------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/units.c					*/
/* -------------------------------------------------------------	*/
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "piece.h"
#include "sql.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "units.h"

LIST *units_alias_list( void )
{
	FILE *input_pipe;
	char *tmp;
	LIST *list;
	char input[ 1024 ];
	UNITS_ALIAS *units_alias;

	input_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_pipe(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			( tmp = appaserver_system_string(
				UNITS_ALIAS_SELECT,
				UNITS_ALIAS_TABLE,
				(char *)0 /* where */ ) ) );

	free( tmp );
	list = list_new();

	while ( string_input( input, input_pipe, 1024 ) )
	{
		if ( ! ( units_alias =
				units_alias_parse(
					input ) ) )
		{
			char message[ 2048 ];

			pclose( input_pipe );

			sprintf(message,
				"units_alias_parse(%s) returned empty.",
				input );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		list_set( list, units_alias );
	}

	pclose( input_pipe );
	return list;
}

UNITS_ALIAS *units_alias_parse( char *input )
{
	UNITS_ALIAS *units_alias;
	char units_alias_name[ 128 ];
	char units_name[ 128 ];

	if ( !input )
	{
		char message[ 256 ];

		sprintf(message, "input is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	/* See UNITS_ALIAS_SELECT */
	/* ---------------------- */
	piece( units_alias_name, SQL_DELIMITER, input, 0 );
	piece( units_name, SQL_DELIMITER, input, 1 );

	if ( !*units_name ) return (UNITS_ALIAS *)0;

	units_alias = units_alias_calloc();
	units_alias->units_alias_name = strdup( units_alias_name );
	units_alias->units_name = strdup( units_name );

	return units_alias;
}

UNITS_ALIAS *units_alias_seek(
			char *units_alias_name,
			LIST *units_alias_list )
{
	UNITS_ALIAS *units_alias;

	if ( !units_alias_name )
	{
		char message[ 256 ];

		sprintf(message, "units_alias_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( units_alias_list ) ) return (UNITS_ALIAS *)0;

	do {
		units_alias = list_get( units_alias_list );

		if ( strcmp(
			units_alias_name,
			units_alias->units_alias_name ) == 0 )
		{
			return units_alias;
		}

	} while ( list_next( units_alias_list ) );

	return (UNITS_ALIAS *)0;
}

UNITS_CONVERTED *units_converted_calloc( void )
{
	UNITS_CONVERTED *units_converted;

	if ( ! ( units_converted = calloc( 1, sizeof ( UNITS_CONVERTED ) ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return units_converted;
}

UNITS_ALIAS *units_alias_calloc( void )
{
	UNITS_ALIAS *units_alias;

	if ( ! ( units_alias = calloc( 1, sizeof ( UNITS_ALIAS ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return units_alias;
}

UNITS *units_calloc( void )
{
	UNITS *units;

	if ( ! ( units = calloc( 1, sizeof ( UNITS ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return units;
}

char *units_primary_where( char *units_name )
{
	static char where[ 128 ];

	if ( !units_name )
	{
		char message[ 256 ];

		sprintf(message, "units_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(where,
		"units = '%s'",
		units_name );

	return where;
}

LIST *units_list( boolean fetch_converted_list )
{
	FILE *input_pipe;
	char *tmp;
	LIST *list;
	char input[ 128 ];

	input_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_pipe(
			( tmp =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				appaserver_system_string(
					"units" /* select */,
					UNITS_TABLE,
					(char *)0 /* where */ ) ) );

	free( tmp );
	list = list_new();

	while ( string_input( input, input_pipe, 128 ) )
	{
		list_set(
			list,
			units_new(
				strdup( input ) /* units_name */,
				fetch_converted_list ) );
	}

	pclose( input_pipe );
	return list;
}

UNITS *units_new(	char *units_name,
			boolean fetch_converted_list )
{
	UNITS *units;

	if ( !units_name )
	{
		char message[ 256 ];

		sprintf(message, "units_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	units = units_calloc();
	units->units_name = units_name;

	if ( fetch_converted_list )
	{
		units->units_converted_list =
			units_converted_list(
				units->units_name,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				units_primary_where(
					units->units_name ) );
	}

	return units;
}

char *units_converted_primary_where(
			char *units_name,
			char *units_converted )
{
	static char where[ 128 ];

	if ( !units_name
	||   !units_converted )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(where,
		"units = '%s' and units_converted = '%s'",
		units_name,
		units_converted );

	return where;
}

UNITS_CONVERTED *units_converted_fetch(
			char *units_name,
			char *units_converted )
{
	if ( !units_name
	||   !units_converted )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	units_converted_parse(
		units_name,
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_pipe_input(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			appaserver_system_string(
				UNITS_CONVERTED_SELECT,
				UNITS_CONVERTED_TABLE,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				units_converted_primary_where(
					units_name,
					units_converted ) ) ) );
}

UNITS_CONVERTED *units_converted_parse(
			char *units_name,
			char *input )
{
	UNITS_CONVERTED *units_converted;
	char piece_buffer[ 128 ];

	if ( !units_name
	||   !input )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	units_converted = units_converted_calloc();
	units_converted->units_name = units_name;

	/* See UNITS_CONVERTED_SELECT */
	/* -------------------------- */
	piece( piece_buffer, SQL_DELIMITER, input, 0 );
	units_converted->units_converted_name = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 1 );
	units_converted->multiply_by = atof( piece_buffer );

	return units_converted;
}

LIST *units_converted_list(
			char *units_name,
			char *units_primary_where )
{
	FILE *input_pipe;
	char *tmp;
	LIST *list;
	char input[ 128 ];

	if ( !units_name
	||   !units_primary_where )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	input_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_pipe(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			( tmp =
				appaserver_system_string(
					UNITS_CONVERTED_SELECT,
					UNITS_CONVERTED_TABLE,
					units_primary_where ) ) );

	free( tmp );
	list = list_new();

	while ( string_input( input, input_pipe, 128 ) )
	{
		list_set(
			list,
			units_converted_parse(
				units_name,
				input ) );
	}

	pclose( input_pipe );
	return list;
}

char *units_converted_constant_spelling( char *units_name )
{
	if ( !units_name )
	{
		char message[ 256 ];

		sprintf(message, "units_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( strcmp( units_name, "centigrade" ) == 0 )
	{
		return "celsius";
	}
	else
	if ( strcmp( units_name, "absolute_centigrade" ) == 0
	||   strcmp( units_name, "absolute_celsius" ) == 0
	||   strcmp( units_name, "absolute_c" ) == 0 )
	{
		return "kelvin";
	}
	else
	if ( strcmp( units_name, "absolute_f" ) == 0
	||   strcmp( units_name, "absolute_fahren" ) == 0 )
	{
		return "absolute_fahrenheit";
	}
	else
	{
		return units_name;
	}
}

boolean units_converted_temperature_boolean( char *units_name )
{
	if ( !units_name )
	{
		char message[ 256 ];

		sprintf(message, "units_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( strcmp( units_name, "celsius" ) == 0 )
		return 1;
	else
	if ( strcmp( units_name, "fahrenheit" ) == 0 )
		return 1;
	else
	if ( strcmp( units_name, "kelvin" ) == 0 )
		return 1;
	else
	if ( strcmp( units_name, "absolute_fahrenheit" ) == 0 )
		return 1;
	else
		return 0;
}

boolean units_converted_navd88_boolean( char *units_converted )
{
	if ( !units_converted )
	{
		char message[ 256 ];

		sprintf(message, "units_converted is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return ( strcasecmp( units_converted, "feet_navd88" ) == 0 );
}

UNITS_CONVERTED_ROW *units_converted_row_calloc( void )
{
	UNITS_CONVERTED_ROW *units_converted_row;

	if ( ! ( units_converted_row =
			calloc( 1,
				sizeof ( UNITS_CONVERTED_ROW ) ) ) )
	{
		char message[ 256 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return units_converted_row;
}

UNITS_CONVERTED_ROW *units_converted_row_parse(
			char *input /* in/out */,
			UNITS_CONVERTED_ROW *units_converted_row /* in/out */,
			UNITS_CONVERTED_STDIN *units_converted_stdin )
{
	if ( !input
	||   !units_converted_row
	||   !units_converted_stdin )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	units_converted_row->value_string =
		/* ----------------------------- */
		/* Returns static memory or null */
		/* ----------------------------- */
		units_converted_row_value_string(
			units_converted_stdin->value_piece,
			units_converted_stdin->delimiter,
			input );

	if ( !units_converted_row->value_string )
	{
		units_converted_row->output_string = input;
		return units_converted_row;
	}

	units_converted_row->old_value =
		units_converted_row_old_value(
			units_converted_row->value_string );

	if ( units_converted_stdin->navd88_boolean
	&&   units_converted_stdin->station_piece > -1 )
	{
		units_converted_row->station_string =
			/* ----------------------------- */
			/* Returns static memory or null */
			/* ----------------------------- */
			units_converted_row_station_string(
				units_converted_stdin->station_piece,
				units_converted_stdin->delimiter,
				input );

		if ( !units_converted_row->station_string )
		{
			char message[ 256 ];

			sprintf(message,
		"units_converted_row_station_string(%s) returned empty.",
				input );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		units_converted_row->new_value =
			units_converted_row_navd88_new_value(
				units_converted_stdin->
					station_navd88_dictionary,
				units_converted_row->old_value,
				units_converted_row->station_string );

		units_converted_row->output_string =
			/* ------------- */
			/* Returns input */
			/* ------------- */
			units_converted_row_output_string(
				input /* in/out */,
				units_converted_stdin->value_piece,
				units_converted_stdin->delimiter,
				units_converted_row->new_value );
	}
	else
	if ( units_converted_stdin->temperature_boolean )
	{
		units_converted_row->new_value =
			units_converted_row_temperature_new_value(
				units_converted_stdin->units_name,
				units_converted_stdin->units_converted_name,
				units_converted_row->old_value );

		units_converted_row->output_string =
			/* ------------- */
			/* Returns input */
			/* ------------- */
			units_converted_row_output_string(
				input /* in/out */,
				units_converted_stdin->value_piece,
				units_converted_stdin->delimiter,
				units_converted_row->new_value );
	}
	else
	if ( units_converted_stdin->units_converted )
	{
		units_converted_row->new_value =
			units_converted_row_new_value(
				units_converted_stdin->
					units_converted->
					multiply_by,
				units_converted_row->old_value );

		units_converted_row->output_string =
			/* ------------- */
			/* Returns input */
			/* ------------- */
			units_converted_row_output_string(
				input /* in/out */,
				units_converted_stdin->value_piece,
				units_converted_stdin->delimiter,
				units_converted_row->new_value );
	}
	else
	{
		char message[ 256 ];

		sprintf(message, "Invalid configuration." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return units_converted_row;
}

char *units_converted_row_value_string(
			int value_piece,
			char delimiter,
			char *input )
{
	static char value_string[ 16 ];

	piece(	value_string,
		delimiter,
		input,
		value_piece );

	if ( !*value_string
	||   strcmp( value_string, "null" ) == 0 )
	{
		return (char *)0;
	}

	return value_string;
}

double units_converted_row_old_value( char *value_string )
{
	if ( !value_string )
	{
		char message[ 256 ];

		sprintf(message, "value_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return atof( value_string );
}

char *units_converted_row_station_string(
			int station_piece,
			char delimiter,
			char *input )
{
	static char station_string[ 16 ];

	if ( station_piece == -1
	||   !delimiter
	||   !input )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	piece(	station_string,
		delimiter,
		input,
		station_piece );

	if ( !*station_string ) return (char *)0;

	return station_string;
}

double units_converted_row_temperature_new_value(
			char *units_name,
			char *units_converted_name,
			double old_value )
{
	if ( !units_name
	||   !units_converted_name )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( strcmp( units_name, "celsius" ) == 0 )
	{
		if ( strcmp( units_converted_name, "fahrenheit" ) == 0 )
		{
			return ( old_value * 1.8 ) + 32.0;
		}
		else
		if ( strcmp( units_converted_name, "kelvin" ) == 0 )
		{
			return old_value + 273.16;
		}
		else
		if ( strcmp(	units_converted_name,
				"absolute_fahrenheit" ) == 0 )
		{
			return ( ( old_value * 1.8 ) + 32.0 ) + 459.688;
		}
	}
	else
	if ( strcmp( units_name, "fahrenheit" ) == 0 )
	{
		if ( strcmp( units_converted_name, "celsius" ) == 0 )
		{
			return ( old_value - 32.0 ) / 1.8;
		}
		else
		if ( strcmp( units_converted_name, "kelvin" ) == 0 )
		{
			return ( ( old_value - 32.0 ) / 1.8 ) + 273.16;
		}
		else
		if ( strcmp(
			units_converted_name,
			"absolute_fahrenheit" ) == 0 )
		{
			return old_value + 459.688;
		}
	}
	else
	if ( strcmp( units_name, "kelvin" ) == 0 )
	{
		if ( strcmp( units_converted_name, "fahrenheit" ) == 0 )
		{
			return ( ( old_value - 273.16 ) * 1.8 ) + 32.0;
		}
		else
		if ( strcmp( units_converted_name, "celsius" ) == 0 )
		{
			return old_value - 273.16;
		}
		else
		if ( strcmp(
			units_converted_name,
			"absolute_fahrenheit" ) == 0 )
		{
			return
			( ( ( old_value - 273.16 ) * 1.8 ) + 32 ) + 459.688;
		}
	}
	else
	if ( strcmp( units_name, "absolute_fahrenheit" ) == 0 )
	{
		if ( strcmp( units_converted_name, "fahrenheit" ) == 0 )
		{
			return old_value - 459.688;
		}
		else
		if ( strcmp( units_converted_name, "celsius" ) == 0 )
		{
			return ( ( old_value - 459.688 ) - 32.0 ) / 1.8;
		}
		else
		if ( strcmp( units_converted_name, "kelvin" ) == 0 )
		{
			return
			( ( ( old_value - 459.688 ) - 32 ) / 1.8 ) + 273.16;
		}
	}

	return 0.0;
}

double units_converted_row_new_value(
			double multiply_by,
			double old_value )
{
	return multiply_by * old_value;
}

double units_converted_row_navd88_new_value(
			DICTIONARY *station_navd88_dictionary,
			double old_value,
			char *station_string )
{
	char *value_increment_string;

	if ( !station_navd88_dictionary
	||   !station_string )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( ( value_increment_string =
		dictionary_get(
			station_string,
			station_navd88_dictionary ) ) )
	{
		old_value += atof( value_increment_string );
	}

	return old_value;
}

char *units_converted_row_output_string(
			char *input /* in/out */,
			int value_piece,
			char delimiter,
			double new_value )
{
	piece_replace(
		input /* in/out */,
		delimiter,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		string_ftoa(	new_value,
				4 /* decimal_places */ ),
		value_piece );

	return input;
}

UNITS_CONVERTED_STDIN *units_converted_stdin_new(
			char *units_name,
			char *units_converted_name,
			int value_piece,
			char delimiter,
			int station_piece )
{
	UNITS_CONVERTED_STDIN *units_converted_stdin;

	if ( !units_name
	||   !units_converted_name
	||   !delimiter )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	units_converted_stdin = units_converted_stdin_calloc();
	units_converted_stdin->value_piece = value_piece;
	units_converted_stdin->delimiter = delimiter;
	units_converted_stdin->station_piece = station_piece;

	units_converted_stdin->units_name =
		/* ------------------------------------ */
		/* Returns program memory or units_name */
		/* ------------------------------------ */
		units_converted_stdin_constant_spelling(
			units_name );

	units_converted_stdin->units_converted_name =
		units_converted_stdin_constant_spelling(
			units_converted_name );

	units_converted_stdin->temperature_boolean =
		units_converted_stdin_temperature_boolean(
			units_converted_stdin->units_name );

	units_converted_stdin->navd88_boolean =
		units_converted_stdin_navd88_boolean(
			UNITS_CONVERTED_STDIN_NAVD88_NAME,
			units_converted_stdin->units_converted_name );

	if ( units_converted_stdin->navd88_boolean
	&&   station_piece == -1 )
	{
		char message[ 256 ];

		sprintf(message, "navd88 unit needs station_piece set." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( units_converted_stdin->navd88_boolean )
	{
		units_converted_stdin->station_navd88_dictionary =
			units_converted_stdin_station_navd88_dictionary(
				APPASERVER_STATION_TABLE,
				APPASERVER_NAVD88_ATTRIBUTE );
	}

	if ( !units_converted_stdin->temperature_boolean
	&&   !units_converted_stdin->navd88_boolean )
	{
		units_converted_stdin->units_converted =
			units_converted_fetch(
				units_converted_stdin->units_name,
				units_converted_stdin->units_converted_name );
	}

	return units_converted_stdin;
}

UNITS_CONVERTED_STDIN *units_converted_stdin_calloc( void )
{
	UNITS_CONVERTED_STDIN *units_converted_stdin;

	if ( ! ( units_converted_stdin =
			calloc( 1,
				sizeof ( UNITS_CONVERTED_STDIN ) ) ) )
	{
		char message[ 256 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return units_converted_stdin;
}

char *units_converted_stdin_constant_spelling( char *units_name )
{
	if ( !units_name )
	{
		char message[ 256 ];

		sprintf(message, "units_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( strcmp( units_name, "centigrade" ) == 0 )
	{
		return "celsius";
	}
	else
	if ( strcmp( units_name, "absolute_centigrade" ) == 0
	||   strcmp( units_name, "absolute_celsius" ) == 0
	||   strcmp( units_name, "absolute_c" ) == 0 )
	{
		return "kelvin";
	}
	else
	if ( strcmp( units_name, "absolute_f" ) == 0
	||   strcmp( units_name, "absolute_fahren" ) == 0 )
	{
		return "absolute_fahrenheit";
	}
	else
	{
		return units_name;
	}
}

boolean units_converted_stdin_temperature_boolean( char *units_name )
{
	if ( strcmp( units_name, "celsius" ) == 0 )
		return 1;
	else
	if ( strcmp( units_name, "fahrenheit" ) == 0 )
		return 1;
	else
	if ( strcmp( units_name, "kelvin" ) == 0 )
		return 1;
	else
	if ( strcmp( units_name, "absolute_fahrenheit" ) == 0 )
		return 1;
	else
		return 0;
}

boolean units_converted_stdin_navd88_boolean(
			char *units_converted_stdin_navd88_name,
			char *units_converted_name )
{
	if ( !units_converted_stdin_navd88_name
	||   !units_converted_name )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	( strcmp(
		units_converted_stdin_navd88_name,
		units_converted_name ) == 0 );
}

DICTIONARY *units_converted_stdin_station_navd88_dictionary(
			char *appaserver_station_table,
			char *appaserver_navd88_attribute )
{
	char system_string[ 1024 ];
	char select[ 128 ];
	char where[ 128 ];

	sprintf(select,
		"station,%s",
		appaserver_navd88_attribute );

	sprintf(where,
		"%s is not null",
		appaserver_navd88_attribute );

	sprintf(system_string,
		"select.sh %s %s \"%s\"",
		select,
		appaserver_station_table,
		where );

	return
	dictionary_pipe(
		system_string,
		SQL_DELIMITER );
}

void units_converted_stdin_execute(
			UNITS_CONVERTED_STDIN *units_converted_stdin )
{
	char input[ 1024 ];
	UNITS_CONVERTED_ROW *units_converted_row;

	if ( !units_converted_stdin )
	{
		char message[ 256 ];

		sprintf(message, "units_converted_stdin is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	units_converted_row = units_converted_row_calloc();

	while ( string_input( input, stdin, 1024 ) )
	{
		units_converted_row =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			units_converted_row_parse(
				input /* in/out */,
				units_converted_row /* in/out */,
				units_converted_stdin );

		printf( "%s\n",
			units_converted_row->output_string );
	}
}

char *units_converted_dictionary_extract( DICTIONARY *dictionary )
{
	return
	dictionary_get(
		UNITS_CONVERTED_LABEL,
		dictionary );
}

