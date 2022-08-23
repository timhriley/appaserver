/* ------------------------------------------------------------	*/
/* $APPASERVER_HOME/utility/measurement_convert_units.c		*/
/* ----------------------------------------------------------	*/
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ----------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver_library.h"
#include "timlib.h"
#include "environ.h"
#include "piece.h"

/* Constants */
/* --------- */
#define DEFAULT_MULTIPLY_BY		0.0

/* Prototypes */
/* ---------- */
DICTIONARY *get_station_dictionary(
				char *application_name );

double get_navd88_value(	char *application_name,
				double value,
				char *station );

char *get_constant_units_spelling(
				char *units );

double get_temperature_value(	double value,
				char *units,
				char *units_converted );

boolean is_navd88(		char *units_converted );

boolean is_temperature(		char *units );

double get_multiply_by(
			char *units,
			char *units_converted );

int main( int argc, char **argv )
{
	char *application_name;
	char *units;
	char *units_converted;
	int value_piece;
	int station_piece;
	char delimiter;
	char input_buffer[ 1024 ];
	char value_buffer[ 128 ];
	char station_buffer[ 128 ];
	double multiply_by = DEFAULT_MULTIPLY_BY;
	char new_value_buffer[ 128 ];
	char *delimiter_string;
	boolean is_temperature_boolean;
	boolean is_navd88_boolean;

	output_starting_argv_stderr( argc, argv );

	if ( argc < 5 )
	{
		fprintf(stderr,
"Usage: %s units units_converted value_piece delimiter [station_piece]\n",
			argv[ 0 ] );
		exit( 1 );
	}

	application_name = environment_application_name();

	units = argv[ 1 ];
	units_converted = argv[ 2 ];
	value_piece = atoi( argv[ 3 ] );
	delimiter_string = argv[ 4 ];

	if ( argc == 6 )
		station_piece = atoi( argv[ 5 ] );
	else
		station_piece = -1;

	timlib_trim_double_quotes( units );
	timlib_trim_double_quotes( units_converted );
	timlib_trim_double_quotes( delimiter_string );

	delimiter = *delimiter_string;

	units = get_constant_units_spelling( units );
	units_converted = get_constant_units_spelling( units_converted );

	is_temperature_boolean = is_temperature( units );
	is_navd88_boolean = is_navd88( units_converted );

	if ( is_temperature_boolean )
	{
		multiply_by = DEFAULT_MULTIPLY_BY;
	}
	else
	{
		multiply_by =
			get_multiply_by(
				units,
				units_converted );
	}

	while( get_line( input_buffer, stdin ) )
	{
		if ( piece(	value_buffer,
				delimiter,
				input_buffer,
				value_piece ) )
		{
			if ( *value_buffer
			&&   timlib_strcmp( value_buffer, "null" ) != 0 )
			{
				if ( is_navd88_boolean && station_piece >= 0 )
				{
					piece(	station_buffer,
						delimiter,
						input_buffer,
						station_piece );

					sprintf(new_value_buffer,
						"%lf",
						get_navd88_value(
							application_name,
							atof( value_buffer ),
							station_buffer ) );
				}
				else
				if ( is_temperature_boolean )
				{
					sprintf(new_value_buffer,
						"%lf",
						get_temperature_value(
							atof( value_buffer ),
							units,
							units_converted ) );
				}
				else
				{
					sprintf(new_value_buffer,
						"%lf",
						atof( value_buffer ) *
						multiply_by );
				}

				piece_replace(	input_buffer,
						delimiter,
						new_value_buffer,
						value_piece );
			}

		}
		printf( "%s\n", input_buffer );
	}

	exit( 0 );
}

double get_multiply_by(
			char *units,
			char *units_converted )
{
	char sys_string[ 1024 ];
	char *results;

	sprintf( sys_string,
		 "echo \"	select multiply_by		 "
		 "		from %s				 "
		 "		where units = '%s'		 "
		 "		  and units_converted = '%s';\"	|"
		 "sql						 ",
		 "units_converted",
		 units,
		 units_converted );

	results = pipe2string( sys_string );
	if ( !results || !*results )
		return DEFAULT_MULTIPLY_BY;
	else
		return atof( results );
}

boolean is_navd88( char *units_converted )
{
	return ( strcasecmp( units_converted, "feet_navd88" ) == 0 );
}

boolean is_temperature( char *units )
{
	if ( strcmp( units, "celsius" ) == 0 )
		return 1;
	else
	if ( strcmp( units, "fahrenheit" ) == 0 )
		return 1;
	else
	if ( strcmp( units, "kelvin" ) == 0 )
		return 1;
	else
	if ( strcmp( units, "absolute_fahrenheit" ) == 0 )
		return 1;
	else
		return 0;
}

char *get_constant_units_spelling( char *units )
{
	if ( strcmp( units, "centigrade" ) == 0 )
	{
		return "celsius";
	}
	else
	if ( strcmp( units, "absolute_centigrade" ) == 0
	||   strcmp( units, "absolute_celsius" ) == 0
	||   strcmp( units, "absolute_c" ) == 0 )
	{
		return "kelvin";
	}
	else
	if ( strcmp( units, "absolute_f" ) == 0
	||   strcmp( units, "absolute_fahren" ) == 0 )
	{
		return "absolute_fahrenheit";
	}
	else
	{
		return units;
	}
}

double get_navd88_value(	char *application_name,
				double value,
				char *station )
{
	static DICTIONARY *station_dictionary = {0};
	char *value_offset_string;

	if ( !station_dictionary )
	{
		station_dictionary = get_station_dictionary( application_name );
	}

	if ( ( value_offset_string =
			dictionary_fetch( station_dictionary, station ) ) )
	{
		return value + atof( value_offset_string );
	}
	return value;
}

DICTIONARY *get_station_dictionary( char *application_name )
{
	char sys_string[ 1024 ];
	char *where;
	char *select;

	select = "station,NGVD29_NAVD88";
	where = "NGVD29_NAVD88 is not null";

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=station		"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 where );

	return pipe2dictionary( sys_string, FOLDER_DATA_DELIMITER );

}

double get_temperature_value(	double value,
				char *units,
				char *units_converted )
{
	if ( strcmp( units, "celsius" ) == 0 )
	{
		if ( strcmp( units_converted, "fahrenheit" ) == 0 )
		{
			return ( value * 1.8 ) + 32.0;
		}
		else
		if ( strcmp( units_converted, "kelvin" ) == 0 )
		{
			return value + 273.16;
		}
		else
		if ( strcmp( units_converted, "absolute_fahrenheit" ) == 0 )
		{
			return ( ( value * 1.8 ) + 32.0 ) + 459.688;
		}
	}
	else
	if ( strcmp( units, "fahrenheit" ) == 0 )
	{
		if ( strcmp( units_converted, "celsius" ) == 0 )
		{
			return ( value - 32.0 ) / 1.8;
		}
		else
		if ( strcmp( units_converted, "kelvin" ) == 0 )
		{
			return ( ( value - 32.0 ) / 1.8 ) + 273.16;
		}
		else
		if ( strcmp( units_converted, "absolute_fahrenheit" ) == 0 )
		{
			return value + 459.688;
		}
	}
	else
	if ( strcmp( units, "kelvin" ) == 0 )
	{
		if ( strcmp( units_converted, "fahrenheit" ) == 0 )
		{
			return ( ( value - 273.16 ) * 1.8 ) + 32.0;
		}
		else
		if ( strcmp( units_converted, "celsius" ) == 0 )
		{
			return value - 273.16;
		}
		else
		if ( strcmp( units_converted, "absolute_fahrenheit" ) == 0 )
		{
			return ( ( ( value - 273.16 ) * 1.8 ) + 32 ) + 459.688;
		}
	}
	else
	if ( strcmp( units, "absolute_fahrenheit" ) == 0 )
	{
		if ( strcmp( units_converted, "fahrenheit" ) == 0 )
		{
			return value - 459.688;
		}
		else
		if ( strcmp( units_converted, "celsius" ) == 0 )
		{
			return ( ( value - 459.688 ) - 32.0 ) / 1.8;
		}
		else
		if ( strcmp( units_converted, "kelvin" ) == 0 )
		{
			return ( ( ( value - 459.688 ) - 32 ) / 1.8 ) + 273.16;
		}
	}
	return 0.0;
}

