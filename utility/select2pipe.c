/* utility/select_to_pipe.c					 */
/* ------------------------------------------------------------- */
/* This program converts a standard select statement to one that */
/* will separate each field with the pipe '|' character. 	 */
/*								 */
/* Freely available software: see Appaserver.org		 */
/* ------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"
#include "timlib.h"

/* Function prototypes */
/* ------------------- */
void build_sql_stmt(	char *sql_stmt, 
			char *tok_ptr );

/*
void replace_asterisk_with_all_fields( 
			char *tok_ptr, 
			char *from_table,
			char *schema );
*/

/* Global variables */
/* ---------------- */
char from_table[ 128 ];

int main( int argc, char **argv )
{
	char buffer[ 2048 ];
	char sql_stmt[ 4096 ];
	/* char *schema; */
	char *buf_ptr;
	char *tok_ptr;		/* Note: tok_ptr is the 4K static buffer */
				/*       inside of get_token().		 */

/*
	if ( argc == 2 )
		schema = argv[ 1 ];
	else
		schema = "";
*/

	printf( "set linesize 9999;\n" );

	while( get_line( buffer, stdin ) )
	{
		buf_ptr = buffer;

        	while ( 1 )
        	{
                	tok_ptr = get_token( &buf_ptr );
                	if ( !*tok_ptr ) break;

			if ( strcasecmp( tok_ptr, "rem" ) == 0 ) break;

			build_sql_stmt( sql_stmt, tok_ptr );
		}
	}

/*
	replace_asterisk_with_all_fields(	sql_stmt, 
						from_table,
						schema );
*/

	printf( "%s\n", sql_stmt );

	return 0;

} /* main() */


#define beginning		0
#define select_state		1
#define from_state		2
#define ok_to_end		3
#define literal			4

void build_sql_stmt(	char *d, 
			char *tok_ptr )
{
	static int state = beginning;
	static int before_literal_state;

	if ( *tok_ptr == ';' )
	{
		sprintf( d + strlen( d ), "%s ", tok_ptr );
		return;
	}

	if ( state == literal )
	{
		sprintf( d + strlen( d ), "%s ", tok_ptr );
		state = before_literal_state;
		return;
	}

	if ( *tok_ptr == '\\' )
	{
		before_literal_state = state;
		state = literal;
		return;
	}

	if ( state == beginning && strcasecmp( tok_ptr, "select" ) == 0 )
	{
		strcpy( tok_ptr, "select '^' ||" );
		sprintf( d + strlen( d ), "%s ", tok_ptr );
		state = select_state;
		return;
	}
	else
	if ( strcasecmp( tok_ptr, "from" ) == 0 )
		state = from_state;
	else
	if ( strcasecmp( tok_ptr, "where" ) == 0 )
		state = ok_to_end;

	/* This must be first to capture the table */
	/* --------------------------------------- */
	if ( state == from_state )
	{
		strcpy( from_table, tok_ptr );
	}

	if ( state == select_state && *tok_ptr == ',' )
	{
		strcpy( tok_ptr, "||'|'||" ); 		/* 4K static buffer */
	}

	sprintf( d + strlen( d ), "%s ", tok_ptr );

} /* build_sql_stmt() */


void replace_asterisk_with_all_fields(	char *sql_stmt, 
					char *from_table,
					char *schema )
{
	FILE *p;
	char buffer[ 128 ];
	char all_fields[ 1024 ];
	int first_time = 1;

	if ( instr( "(*)", sql_stmt, 1 ) >= 0 ) return;
	if ( instr( "( * )", sql_stmt, 1 ) >= 0 ) return;
	if ( instr( "(* )", sql_stmt, 1 ) >= 0 ) return;
	if ( instr( "(*)", sql_stmt, 1 ) >= 0 ) return;
	if ( instr( "*", sql_stmt, 1 ) == -1 ) return;

	sprintf( buffer, 
		 "column4table.sh %s %s | piece.e '|' 0",
		 schema, low_string( from_table ) );

/* fprintf( stderr, "(%s)\n", buffer ); */

	p = popen( buffer, "r" );

	while( fgets( buffer, 127, p ) )
	{
		buffer[ strlen( buffer ) - 1 ] = '\0';

		if ( first_time )
		{
			strcpy( all_fields, buffer );
			first_time = 0;
		}
		else
		{
			sprintf( all_fields + strlen( all_fields ),
				 "||'|'||%s",
				 buffer );
		}
	}
	pclose( p );

	search_replace( "*", all_fields, sql_stmt );

} /* replace_asterisk_with_all_fields() */

