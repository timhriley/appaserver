/* $APPASERVER_HOME/utility/group_date_time.c			*/
/* ------------------------------------------------------------ */
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "name_arg.h"
#include "piece.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void setup_arg( NAME_ARG *arg, int argc, char **argv );

void fetch_parameters(	NAME_ARG *arg,
			char **value_offset_string,
			char **function_string,
			char **delimiter_string );

void output(	char **block,
		char delimiter );

int main( int argc, char **argv )
{
	char buffer[ 4096 ];
	char *block[ 1024 ];
	int block_count;
	char *value_offset_string;
	int value_offset;
	char *function_string;
	char *delimiter_string;
	char value_string[ 32 ];
	double value;
	double change;
	double percent_change;
	double base = 0.0;
	int i;
        NAME_ARG *arg = init_arg( argv[ 0 ] );

	setup_arg( arg, argc, argv );
	fetch_parameters(	arg, 
				&value_offset_string,
				&function_string,
				&delimiter_string );

	value_offset = atoi( value_offset_string );

	while( ( block_count = timlib_get_block_delimiter(
			block,
			*delimiter_string,
			2 /* columns_in_group */,
			stdin,
			1024 /* max_lines */ ) ) )
	{
		for( i = 0; i < block_count; i++ )
		{
			if ( !piece(	value_string, 
					*delimiter_string, 
					block[ i ],
					value_offset ) )
			{
				fprintf(stderr,
	"%s ERROR: cannot extract value using delimiter = (%c) in (%s)\n",
					argv[ 0 ],
					*delimiter_string,
					buffer );
				exit( 1 );
			}

        		if ( strcmp(	function_string,
					"percent_change_base_first" ) == 0 )
			{
				value = atof( value_string );
				if ( i == 0 )
				{
					base = value;
					printf( "%s%c%c\n",
						block[ i ],
						*delimiter_string,
						*delimiter_string );
					free( block[ i ] );
				}
				else
				{
					if ( base )
					{
						change = value - base;
						percent_change =
							change / base * 100.0;
					}
					else
					{
						change = 0.0;
						percent_change = 0.0;
					}

					printf( "%s%c%.4lf%c%.2lf\n",
						block[ i ],
						*delimiter_string,
						change,
						*delimiter_string,
						percent_change );
					free( block[ i ] );
				}
			}
		}
	}

	return 0;

} /* main() */

void fetch_parameters(	NAME_ARG *arg,
			char **value_offset_string,
			char **function_string,
			char **delimiter_string )
{
	*value_offset_string = fetch_arg( arg, "value_offset" );
	*function_string = fetch_arg( arg, "function" );
	*delimiter_string = fetch_arg( arg, "delimiter" );

} /* fetch_parameters() */


void setup_arg( NAME_ARG *arg, int argc, char **argv )
{
        int ticket;

        ticket = add_valid_option( arg, "value_offset" );

        ticket = add_valid_option( arg, "delimiter" );
        set_default_value( arg, ticket, "|" );

        ticket = add_valid_option( arg, "function" );
	/* add_valid_value( arg, ticket, "percent_change_base_first" ); */
        set_default_value( arg, ticket, "percent_change_base_first" );

        ins_all( arg, argc, argv );

} /* setup_arg() */

