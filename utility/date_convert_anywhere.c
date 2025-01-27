/* $APPASERVER_HOME/utility/date_convert_anywhere.c	*/
/* ---------------------------------------------------- */
/* This process differs from date_convert.c in that	*/
/* there is no delimiter.				*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "piece.h"
#include "list.h"
#include "boolean.h"
#include "sed.h"
#include "date_convert.h"

void date_convert_anywhere(
		char *source_format,
		char *destination_format,
		boolean to_date_function );

int main( int argc, char **argv )
{
	char *source_format;
	char *destination_format;
	boolean to_date_function = 0;

	if ( argc < 3 )
	{
		fprintf( stderr,
"Usage: %s source_format destination_format [to_date_function_yn]\n",
			 argv[ 0 ] );
		fprintf( stderr,
"Note: format may be either \"military,international,american\"\n" );

		exit( 1 );
	}

	source_format = argv[ 1 ];
	destination_format = argv[ 2 ];

	if ( argc == 4 )
	{
		to_date_function = ( *argv[ 3 ] == 'y' );
	}

	date_convert_anywhere(
		source_format,
		destination_format,
		to_date_function );

	return 0;
}

void date_convert_anywhere(
		char *source_format,
		char *destination_format,
		boolean to_date_function )
{
	char input_buffer[ 65536 ];
	DATE_CONVERT *date_convert = {0};
	char source_date[ 16 ];
	int all_done;
	SED *sed;
	char *regular_expression;
	char date_convert_string[ 128 ];
	int source_length;
	enum date_convert_format_enum destination_date_convert_format;

	if ( strcmp( source_format, "international" ) == 0 )
	{
		regular_expression =
			"\'[12][0-9][0-9][0-9]-[01][0-9]-[0123][0-9]\'";
		source_length = 10;
	}
	else
	if ( strcmp( source_format, "military" ) == 0 )
	{
		/* ----------------------------------------------- */
		/* JAN,FEB,MAR,APR,MAY,JUN,JUL,AUG,SEP,OCT,NOV,DEC */
		/* ----------------------------------------------- */
		regular_expression =
	"\'[0-9][1-9]-[JBMASOND][AEPUCO][NBRYLGPTVC]-[12][0-9][0-9][0-9]\'";
		source_length = 11;
	}
	else
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: invalid source format = (%s).\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			source_format );
		exit( 1 );
	}

	if ( strcmp( destination_format, "international" ) == 0 )
	{
		destination_date_convert_format = date_convert_international;
	}
	else
	if ( strcmp( destination_format, "military" ) == 0 )
	{
		destination_date_convert_format = date_convert_military;
	}
	else
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: invalid destination format = (%s).\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			destination_format );
		exit( 1 );
	}

	sed = sed_new( regular_expression, (char *)0 );

	while( string_input( input_buffer, stdin, 65536 ) )
	{
		all_done = 0;

		while( !all_done )
		{
			all_done = 1;

			if ( sed_will_replace( input_buffer, sed ) )
			{
				strncpy(
					source_date,
					input_buffer + sed->begin + 1,
					source_length );

				*(source_date + source_length) = '\0';

				if ( !date_convert )
				{
					date_convert =
					date_convert_new( 
						date_convert_unknown
							/* source_enum */,
						destination_date_convert_format,
						source_date );
				}
				else
				{
					date_convert->return_date_string =
			/* ----------------------------------------------- */
			/* Returns heap memory, source_date_string or null */
			/* ----------------------------------------------- */
					date_convert_return_date_string(
						date_convert->source_enum,
						date_convert->destination_enum,
						source_date );
				}

				if ( to_date_function )
				{
					sprintf(date_convert_string,
						"to_date('%s','DD-MON-YYYY')",
						date_convert->
							return_date_string );
				}
				else
				{
					sprintf(date_convert_string,
						"'%s'",
						date_convert->
							return_date_string );
				}

				sed->replace = date_convert_string;
				sed_search_replace( input_buffer, sed );
				all_done = 0;
			}

		}
		printf( "%s\n", input_buffer );
	}
}

