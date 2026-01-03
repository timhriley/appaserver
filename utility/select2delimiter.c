/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/utility/select2delimiter.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

/* ------------------------------------------------------------- */
/* This program converts a standard ORACLE select statement to	 */
/* one that will separate each field with a delimiter.		 */
/* ------------------------------------------------------------- */
/* Note: need to enhance to use "set colsep '^'"		 */
/* ------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"
#include "timlib.h"

int build_sql_statement(	char *sql_stmt, 
				char *token_ptr,
				char delimiter );

/* Global variables */
/* ---------------- */
char from_table[ 128 ];

int main( int argc, char **argv )
{
	char buffer[ 2048 ];
	char sql_stmt[ 4096 ];
	char delimiter = '^';
	char *buf_ptr;
	char *token_ptr;	/* Note: token_ptr is the 4K static buffer */
				/*       inside of token_get().		 */

	if ( argc == 2 ) delimiter = *argv[ 1 ];

	*sql_stmt = '\0';

	printf( "set linesize 9999;\n" );

	while( get_line( buffer, stdin ) )
	{
		buf_ptr = buffer;

		/* Returns static memory or null */
		/* ----------------------------- */
        	while ( ( token_ptr = token_get( &buf_ptr ) ) )
        	{
			if ( strcasecmp( token_ptr, "rem" ) == 0 ) break;

			if ( !build_sql_statement(	sql_stmt,
							token_ptr,
							delimiter ) )
			{
				break;
			}
		}
		printf( "%s\n", sql_stmt );
		*sql_stmt = '\0';
	}

	return 0;
}


#define beginning		0
#define select_state		1
#define from_state		2
#define ok_to_end		3
#define literal			4

int build_sql_statement(	char *d, 
				char *token_ptr,
				char delimiter )
{
	static int state = beginning;
	static int before_literal_state;

	if ( *token_ptr == ';' )
	{
		strcat( d, ";" );
		state = beginning;
		return 0;
	}

	if ( state == literal )
	{
		sprintf( d + strlen( d ), "%s ", token_ptr );
		state = before_literal_state;
		return 1;
	}

	if ( *token_ptr == '\\' )
	{
		before_literal_state = state;
		state = literal;
		return 1;
	}

	if ( state == beginning && strcasecmp( token_ptr, "select" ) != 0 )
	{
		state = ok_to_end;
	}
	else
	if ( state == beginning && strcasecmp( token_ptr, "select" ) == 0 )
	{
		strcpy( token_ptr, "select '^' ||" );
		sprintf( d + strlen( d ), "%s ", token_ptr );
		state = select_state;
		return 1;
	}
	else
	if ( strcasecmp( token_ptr, "from" ) == 0 )
		state = from_state;
	else
	if ( strcasecmp( token_ptr, "where" ) == 0 )
		state = ok_to_end;

	/* This must be first to capture the table */
	/* --------------------------------------- */
	if ( state == from_state )
	{
		strcpy( from_table, token_ptr );
	}

	if ( state == select_state )
	{
		if ( *token_ptr == ',' )
		{
			sprintf( token_ptr, "||'%c'||", delimiter );
			sprintf( d + strlen( d ), "%s ", token_ptr );
		}
		else
		{
			sprintf( d + strlen( d ), "%s ", token_ptr );
		}
	}
	else
	{
		sprintf( d + strlen( d ), "%s ", token_ptr );
	}
	return 1;

} /* build_sql_statement() */

