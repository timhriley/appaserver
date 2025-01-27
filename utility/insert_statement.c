/* ------------------------------------------- */
/* $APPASERVER_HOME/utility/insert_statement.c */
/* ------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "piece.h"
#include "name_arg.h"

#define MAX_BUFFER		65535
#define DEFAULT_DELIMITER	"|"
#define NULL_STRING		"null"

void output_row( char *row, char delimiter, int max_piece );

void output_insert_statements(
		char *table_name,
		char *field_names,
		char delimiter,
		char replace_yn );

void output_compress_insert_statements(
		char *table_name,
		char *field_names,
		char delimiter,
		char replace_yn );

void fix_dollar_signs( char *d );
void fix_quotes( char *d );

int main( int argc, char **argv )
{
        NAME_ARG *arg = init_arg( argv[ 0 ] );
        char *table_name;
	char *field_name_list_string;
	char *delimiter_string;
	char *replace_yn = "n";
	char *compress_yn = "n";
        int ticket;

	char legacy_usage_string[ 128 ];
	sprintf(legacy_usage_string,
		"Legacy usage: %s table_name [field_1,...,field_n] [delimiter]",
		argv[ 0 ] );

        set_comment( arg, legacy_usage_string );

        ticket = add_valid_option( arg, "table_name");

        ticket = add_valid_option( arg, "field_list");
        set_default_value( arg, ticket, "" );

        ticket = add_valid_option( arg, "delimiter");
        set_default_value( arg, ticket, DEFAULT_DELIMITER );

        ticket = add_valid_option( arg, "replace");
        add_valid_value( arg, ticket, "y" );
        add_valid_value( arg, ticket, "n" );
        set_default_value( arg, ticket, "n" );

        ticket = add_valid_option( arg, "compress");
        add_valid_value( arg, ticket, "y" );
        add_valid_value( arg, ticket, "n" );
        set_default_value( arg, ticket, "n" );

        name_arg_insert_argv( arg, argc, argv );

	if ( !arg->did_any && argc > 1 )
	{
		table_name = argv[ 1 ];

		if ( argc >= 3 )
			field_name_list_string = argv[ 2 ];
		else
			field_name_list_string = "";

		if ( argc == 4 )
			delimiter_string = argv[ 3 ];
		else
			delimiter_string = DEFAULT_DELIMITER;
	}
	else
	{
        	table_name = fetch_arg(arg, "table_name");
        	field_name_list_string = fetch_arg(arg, "field_list");
        	delimiter_string = fetch_arg(arg, "delimiter");
        	replace_yn = fetch_arg(arg, "replace");
        	compress_yn = fetch_arg(arg, "compress");
	}

	if ( *compress_yn == 'y' )
	{
		output_compress_insert_statements(
				table_name,
				field_name_list_string,
				*delimiter_string,
				*replace_yn );
	}
	else
	{
		output_insert_statements(
				table_name,
				field_name_list_string,
				*delimiter_string,
				*replace_yn );
	}

	return 0;
}

void output_compress_insert_statements(
		char *table_name,
		char *field_names,
		char delimiter,
		char replace_yn )
{
	char buffer[ MAX_BUFFER + 1 ];
	int max_piece = 0;
	boolean first_time = 1;

	if ( *field_names )
	{
		max_piece = string_character_count( ',', field_names );
	}

	if ( replace_yn == 'y' )
	{
		printf( "replace into %s", table_name );
	}
	else
	{
		printf( "insert into %s", table_name );
	}

	if ( *field_names ) printf( " (%s)", field_names );

	printf( " values " );

	while( string_input( buffer, stdin, sizeof ( buffer ) ) )
	{
		if ( !*buffer ) continue;

		fix_quotes( buffer );

		if ( first_time )
			first_time = 0;
		else
			printf( "," );

		printf( "(" );
		output_row( buffer, delimiter, max_piece );
		printf( ")" );
	}

	printf(";\n" );
	fflush( stdout );
}

void output_insert_statements(
		char *table_name,
			char *field_names,
			char delimiter,
			char replace_yn )
{
	char buffer[ MAX_BUFFER + 1 ];
	int max_piece = 0;

	if ( *field_names )
	{
		max_piece = string_character_count( ',', field_names );
	}

	while( string_input( buffer, stdin, sizeof ( buffer ) ) )
	{
		if ( !*buffer ) continue;

		fix_quotes( buffer );

		if ( replace_yn == 'y' )
		{
			printf( "replace into %s", table_name );
		}
		else
		{
			printf( "insert into %s", table_name );
		}

		if ( *field_names )
		{
			printf( " (%s)",
				string_remove_character(
					field_names,
					STRING_LF ) );
		}

		printf( " values (" );
		output_row( buffer, delimiter, max_piece );
		printf(");\n" );

		fflush( stdout );
	}
}

void output_row( char *row, char delimiter, int max_piece )
{
	int p;
	char field[ MAX_BUFFER + 1 ];

	for( p = 0; piece( field, delimiter, row, p ); p++ )
	{
		if ( max_piece && p > max_piece ) break;

		fix_dollar_signs( field );
		string_trim( field );

		if ( p ) printf( "," );

		if ( !*field
		||   strcmp( field, "is_null" ) == 0 
		||   strcmp( field, "/" ) == 0 )
		{
			printf( "%s", NULL_STRING );
		}
		else
		{
			printf( "\'%s\'", field );
		}
	}
}

void fix_dollar_signs( char *d )
{
	static char tmp[ MAX_BUFFER + 1 ];
	char *tmp_ptr = tmp;

	strcpy( tmp, d );

	while ( *tmp_ptr )
	{
		if ( *tmp_ptr == '$' )
		{
			*d++ = '\\';
			*d++ = *tmp_ptr++;
		}
		else
			*d++ = *tmp_ptr++;
	}

	*d = '\0';
}

void fix_quotes( char *d )
{
	static char tmp[ MAX_BUFFER + 1 ];
	char *tmp_ptr = tmp;

	strcpy( tmp, d );

	while ( *tmp_ptr )
	{
		/* If escaped, then leave alone */
		/* ---------------------------- */
		if ( *tmp_ptr == '\\' && *(tmp_ptr + 1) == '\'' )
		{
			*d++ = *tmp_ptr++;
			*d++ = *tmp_ptr++;
			continue;
		}

		if ( *tmp_ptr == '\'' )
		{
			/* If at beginning */
			/* --------------- */
			if ( tmp_ptr == tmp )
			{
				if ( *(tmp_ptr + 1) && *(tmp_ptr + 1) != '\'' )
				{
					*d++ = '\'';
					*d++ = *tmp_ptr++;
				}
				else
				{
					*d++ = *tmp_ptr++;
				}
			}
			else
			/* ------------ */
			/* If at ending */
			/* ------------ */
			if ( !*(tmp_ptr + 1 ) )
			{
				if ( *(tmp_ptr - 1) && *(tmp_ptr - 1) != '\'' )
				{
					*d++ = '\'';
					*d++ = *tmp_ptr++;
				}
				else
				{
					*d++ = *tmp_ptr++;
				}
			}
			else
			/* ----------------- */
			/* Must be in middle */
			/* ----------------- */
			if ( *(tmp_ptr + 1) && *(tmp_ptr + 1) != '\''
			&&   *(tmp_ptr - 1) && *(tmp_ptr - 1) != '\'' )
			{
				*d++ = '\'';
				*d++ = *tmp_ptr++;
			}
			else
			{
				*d++ = *tmp_ptr++;
			}
		}
		else
		{
			*d++ = *tmp_ptr++;
		}
	}

	*d = '\0';
}

