/* $APPASERVER_HOME/utility/delete_statement.c		*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */
/* -------------------------------------------------------
	Input:	Table name 			argv[ 1 ]
		Piped delimited rows		stream
	Output:	Delete statements

------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "piece.h"
#include "name_arg.h"
#include "timlib.h"

/* Constants */
/* --------- */
#define MAX_BUFFER		4096
#define DEFAULT_DELIMITER	"|"
/* #define IGNORE_CLAUSE		"ignore" */

/* Prototypes */
/* ---------- */
void output_row(		char *row,
				char *field_names,
				char delimiter );
void output_delete_stmts( 	char *table_name, 
				char *field_names,
				char delimiter );
void fix_any_quotes(		char *d );

int main( int argc, char **argv )
{
        NAME_ARG *arg = init_arg( argv[ 0 ] );
        char *table_name;
	char *field_name_list_string;
	char *delimiter_string;
        int ticket;

	char legacy_usage_string[ 128 ];

	sprintf(legacy_usage_string,
		"Legacy usage: %s table_name field_1,...,field_n [delimiter]",
		argv[ 0 ] );

        set_comment( arg, legacy_usage_string );

        ticket = add_valid_option( arg, "table_name");

        ticket = add_valid_option( arg, "field_list");

        ticket = add_valid_option( arg, "delimiter");
        set_default_value( arg, ticket, DEFAULT_DELIMITER );

        name_arg_insert_argv( arg, argc, argv );

	if ( !arg->did_any && argc > 2 )
	{
		table_name = argv[ 1 ];
		field_name_list_string = argv[ 2 ];

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
	}

	output_delete_stmts(	table_name,
				field_name_list_string,
				*delimiter_string );

	return 0;

} /* main() */


void output_delete_stmts( 	char *table_name, 
				char *field_names, 
				char delimiter )
{
	char buffer[ MAX_BUFFER + 1 ];

	while( get_line( buffer, stdin ) )
	{
		if ( !*buffer ) continue;
		search_replace_string( buffer, "\\", "" );
		fix_any_quotes( buffer );
		escape_dollar_signs( buffer );

#ifdef IGNORE_CLAUSE
		printf( "delete %s from %s", IGNORE_CLAUSE, table_name );
#else
		printf( "delete from %s", table_name );
#endif

		printf( " where 1 = 1" );
		output_row( buffer, field_names, delimiter );
		printf( ";\n" );
		fflush( stdout );
	}

} /* output_delete_stmts() */


void output_row( char *row, char *field_names, char delimiter )
{
	int p;
	char field[ MAX_BUFFER + 1 ];
	char column[ MAX_BUFFER + 1 ];
	char *trim_field;

	for( p = 0; piece( field, delimiter, row, p ); p++ )
	{
		/* If out of columns then all done */
		/* ------------------------------- */
		if ( !piece( column, ',', field_names, p ) ) return;

		trim_field = trim( field );

		if ( strcmp( trim_field, "null" ) == 0
		||   strcmp( trim_field, "is_null" ) == 0 )
		{
			trim_field = "";
		}

		if ( *trim_field )
		{
			printf( " and %s = \'%s\'", 
			 	column, trim_field );
		}
		else
		{
			printf(
			" and (%s = \'\' or %s = \'null\' or %s is null)",
			 	column, column, column );
		}
	}
} /* output_row() */

void fix_any_quotes( char *d )
{
	char tmp[ MAX_BUFFER + 1 ];
	char *tmp_ptr = tmp;

	strcpy( tmp, d );

	while ( *tmp_ptr )
	{
		if ( *tmp_ptr == '\'' )
		{
			*d++ = '\'';
			*d++ = *tmp_ptr++;
		}
		else
			*d++ = *tmp_ptr++;
	}

	*d = '\0';

} /* fix_any_quotes() */

