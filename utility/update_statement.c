/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/utility/update_statement.c				*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

/* ----------------------------------------------------------------------
Input:	Table name 					argv[ 1 ]
	Primary key comma list				argv[ 2 ]
	primary_key[,primary_key...]|attribute=data|...	stream
	or
	primary_key[^primary_key...]^attribute^data	stream
Output:	Update statements
------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "name_arg.h"
#include "boolean.h"

#define BIG_BUFFER		65536
#define SMALL_BUFFER		4096
#define DEFAULT_DELIMITER	"|"

void output_update_statements(	char *table_name,
				LIST *primary_key_comma_list,
				char *argv_0,
				boolean update_single_carrot_delimiter,
				char delimiter );

boolean exists_single_carrot_delimiters( char *input_buffer );

char *get_carrot_new_data( char *input_buffer );

char *get_carrot_data_fieldname( char *input_buffer );

void output_update_single_carrot_delimiters(
				char *input_buffer,
				char *table_name,
				LIST *primary_key_comma_list,
				char *argv_0 );

void output_original_format(	char *input_buffer,
				char delimiter,
				char *table_name,
				LIST *primary_key_comma_list );

char *fix_any_quotes( char *d );

char get_primary_key_data_delimiter( char *primary_key_data_buffer );

int main( int argc, char **argv )
{
	char *table_name;
	LIST *primary_key_comma_list;
	char *primary_key_comma_list_string;
	boolean update_single_carrot_delimiter = 0;
        NAME_ARG *arg = init_arg( argv[ 0 ] );
	char *delimiter_string;
	char *carrot_throughout_yn;
        int ticket;

/*
	char legacy_usage_string[ 128 ];
	sprintf(legacy_usage_string,
	"Legacy usage: %s table_name key_field_1,...,key_field_n [delimiter]",
		argv[ 0 ] );

        set_comment( arg, legacy_usage_string );
        set_comment( arg,
"In the input stream, the delimiter between each primary datum is assumed to" );
	set_comment( arg,
"be a comma. Moreover, the delimiter between the last primary datum and the" );
	set_comment( arg,
"update column name is assumed to be the vertical bar, and the delimiter" );
	set_comment( arg,
"between the update column name and the new data is assumed to be the" );
	set_comment( arg,
"equal sign." );
        set_comment( arg,
"However, you may have each of these elements separated by the carrot if" );
        set_comment( arg,
"carrot_throughout_yn is set to 'y'." );
        set_comment( arg,
"You may also have each of these elements separated by the carrot if" );
	set_comment( arg,
"you're using the legacy usage and use double carrots (^^) as the delimiter." );
*/

        ticket = add_valid_option( arg, "table_name");

        ticket = add_valid_option( arg, "key_field_list");

        ticket = add_valid_option( arg, "delimiter");
        set_default_value( arg, ticket, DEFAULT_DELIMITER );

        ticket = add_valid_option( arg, "carrot_throughout_yn");
        set_default_value( arg, ticket, "n" );

        name_arg_insert_argv( arg, argc, argv );

	if ( !arg->did_any && argc > 2 )
	{
		table_name = argv[ 1 ];

		primary_key_comma_list_string = argv[ 2 ];

		if ( argc == 4 )
			delimiter_string = argv[ 3 ];
		else
			delimiter_string = DEFAULT_DELIMITER;

		if ( argc == 4 )
		{
			if ( strcmp( argv[ 3 ], "^^" ) == 0 )
				update_single_carrot_delimiter = 1;
			else
				delimiter_string = argv[ 3 ];
		}
		else
		{
			delimiter_string = DEFAULT_DELIMITER;
		}
	}
	else
	{
        	table_name = fetch_arg(arg, "table_name" );
		primary_key_comma_list_string =
			fetch_arg(arg, "key_field_list");
        	delimiter_string = fetch_arg(arg, "delimiter" );
        	carrot_throughout_yn = fetch_arg(arg, "carrot_throughout_yn" );
		update_single_carrot_delimiter =
			(*carrot_throughout_yn == 'y' );
	}

	primary_key_comma_list =
		string2list( primary_key_comma_list_string, ',' );

	output_update_statements(
				table_name,
				primary_key_comma_list,
				argv[ 0 ],
				update_single_carrot_delimiter,
				*delimiter_string );
	return 0;
}

void output_update_statements(	char *table_name,
				LIST *primary_key_comma_list,
				char *argv_0,
				boolean update_single_carrot_delimiter,
				char delimiter )
{
	char input_buffer[ BIG_BUFFER ];

	while( timlib_get_line( input_buffer, stdin, BIG_BUFFER ) )
	{
		if ( !*input_buffer ) continue;
		if ( *input_buffer == '#' ) continue;

		if ( update_single_carrot_delimiter )
		{
			output_update_single_carrot_delimiters(
						input_buffer,
						table_name,
						primary_key_comma_list,
						argv_0 );
		}
		else
		{
			output_original_format(	input_buffer,
						delimiter,
						table_name,
						primary_key_comma_list );
		}
	} /* while( get_line() */
}

char *fix_any_quotes( char *d )
{
	char tmp[ SMALL_BUFFER + 1 ];
	char *tmp_ptr = tmp;
	char *anchor = d;

	strcpy( tmp, d );

	while ( *tmp_ptr )
	{
		if ( *tmp_ptr == '\'' 
		&&   *(tmp_ptr + 1) != '\'' )
		{
			*d++ = '\'';
			*d++ = *tmp_ptr++;
		}
		else
		{
			*d++ = *tmp_ptr++;
		}
	}

	*d = '\0';
	return anchor;
}

char get_primary_key_data_delimiter( char *primary_key_data_buffer )
{
	if ( timlib_exists_character( primary_key_data_buffer, '^' ) )
		return '^';
	else
		return ',';
}

/* ------------------------------------------------------------- */
/* primary_key[,primary_key...]|attribute=data[|...]	stream 	 */
/* ------------------------------------------------------------- */
/* ^^				^^	  ^    ^		 */
/* ||				||	  |    |		 */
/* ||				||	  |    Optional multiset */
/* ||				||	  |			 */
/* ||				||	  |			 */
/* ||				||	  new_data		 */
/* ||				|data_fieldname			 */
/* ||				data_buffer			 */
/* |primary key column						 */
/* primary_key_data_buffer					 */
/* ------------------------------------------------------------- */

void output_original_format(	char *input_buffer,
				char delimiter,
				char *table_name,
				LIST *primary_key_comma_list )
{
	char primary_key_data_buffer[ SMALL_BUFFER ];
	char primary_key_data_delimiter;
	char data_buffer[ BIG_BUFFER ];
	char new_data[ BIG_BUFFER ];
	char primary_key_data[ SMALL_BUFFER ];
	char data_fieldname[ SMALL_BUFFER ];
	int i;
	int p;
	char *key;
	char *output_data;

	piece( primary_key_data_buffer, delimiter, input_buffer, 0 );

	primary_key_data_delimiter =
		get_primary_key_data_delimiter(
			primary_key_data_buffer );

	printf( "update %s set ", table_name );

	for(	p = 1;
		piece( data_buffer, delimiter, input_buffer, p );
		p++ )
	{
		piece( data_fieldname, '=', data_buffer, 0 );

		if ( !piece( new_data, '=', data_buffer, 1 ) )
		{
			fprintf(stderr,
"Ignoring: update_statement.e for field = %s, cannot get new data in (%s)\n",
				data_fieldname,
				input_buffer );
			return;
		}

		if ( p > 1 ) printf( "," );

		if ( *new_data 
		&&   strcmp( new_data, "NULL" ) != 0
		&&   strcmp( new_data, "/" ) != 0
		&&   strcmp( new_data, "is_null" ) != 0 )
		{
			output_data =
				fix_any_quotes(
					trim( new_data ) );

			printf( "%s = '%s'",
				data_fieldname,
				output_data );
		}
		else
		{
			printf( "%s = null",
				data_fieldname );
		}
	}

	printf( " where 1 = 1" );

	/* --------------------------------------------------- */
	/* primary_key[,primary_key...]|attribute=data	stream */
	/* --------------------------------------------------- */
	list_reset( primary_key_comma_list );

	i = 0;

	do {
		key = list_get_pointer( primary_key_comma_list );

		if ( !piece( 	primary_key_data, 
				primary_key_data_delimiter, 
				primary_key_data_buffer, 
				i ) )
		{
			fprintf(stderr,
"Ignoring 3: update_statement.e is missing \"...|attribute=data\" in (%s)\n",
				input_buffer );
			return;
		}

		if ( !*key )
		{
			printf( " and (%s = '' or %s is null)",
				key, key );
		}
		else
		{
			unescape_string( primary_key_data );
			output_data = fix_any_quotes( primary_key_data );

			printf( " and %s = '%s'",
				key,
				output_data );
		}
		i++;
	} while( list_next( primary_key_comma_list ) );

	printf( ";\n" );

	fflush( stdout );

}

void output_update_single_carrot_delimiters(	char *input_buffer,
						char *table_name,
						LIST *primary_key_comma_list,
						char *argv_0 )
{
	char primary_key_data[ SMALL_BUFFER ];
	char *data_fieldname;
	char *new_data;
	int i;
	char *key;

	if ( 	character_count( '^', input_buffer ) !=
		list_length( primary_key_comma_list ) + 1 )
	{
		fprintf(	stderr,
				"%s Ignoring: %s\n",
				argv_0,
				input_buffer );
		return;
	}

	data_fieldname = get_carrot_data_fieldname( input_buffer );
	new_data = get_carrot_new_data( input_buffer );

	if ( *new_data 
	&&   strcmp( new_data, "NULL" ) != 0
	&&   strcmp( new_data, "/" ) != 0
	&&   strcmp( new_data, "is_null" ) != 0 )
	{
		printf( "update %s set %s = '%s' where ",
			table_name,
			data_fieldname,
			trim( new_data ) );
	}
	else
	{
		printf( "update %s set %s = null where ",
			table_name,
			data_fieldname );
	}

	list_reset( primary_key_comma_list );

	i = 0;

	do {
		key = list_get_pointer( primary_key_comma_list );

		piece( 	primary_key_data, '^', input_buffer, i );

		if ( i ) printf( " and " );

		if ( !*key )
		{
			printf( "(%s = '' or %s is null)",
				key, key );
		}
		else
		{
			unescape_string( primary_key_data );
			fix_any_quotes( primary_key_data );
			printf( "%s = '%s'",
				key,
				primary_key_data );
		}

		i++;

	} while( list_next( primary_key_comma_list ) );

	printf( ";\n" );

	fflush( stdout );

}

boolean exists_single_carrot_delimiters( char *input_buffer )
{
	while( *input_buffer )
	{
		if ( *input_buffer == '=' ) return 0;
		input_buffer++;
	}
	return 1;
}

char *get_carrot_data_fieldname( char *input_buffer )
{
	static char data_fieldname[ SMALL_BUFFER ] = {0};
	char *end_pointer = input_buffer + strlen( input_buffer );
	int first_time = 1;

	while( end_pointer > input_buffer )
	{
		if ( *end_pointer == '^' )
		{
			if ( first_time )
			{
				first_time = 0;
			}
			else
			{
				piece(	data_fieldname,
					'^',
					end_pointer + 1,
					0 );
				break;
			}
		}
		end_pointer--;
	}
	return data_fieldname;
}

char *get_carrot_new_data( char *input_buffer )
{
	static char new_data[ SMALL_BUFFER ] = {0};
	char *end_pointer = input_buffer + strlen( input_buffer );

	while( end_pointer > input_buffer )
	{
		if ( *end_pointer == '^' )
		{
			strcpy( new_data, end_pointer + 1 );
			break;
		}
		end_pointer--;
	}
	return new_data;
}

