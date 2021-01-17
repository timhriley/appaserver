/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_program/program.c		*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "piece.h"
#include "timlib.h"
#include "sql.h"
#include "boolean.h"
#include "list.h"
#include "program.h"

char *program_primary_where( char *program_name )
{
	char static where[ 128 ];

	sprintf( where,
		 "program_name = '%s'",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 program_name_escape( program_name ) );

	return where;
}

char *program_name_escape( char *program_name )
{
	static char name[ 256 ];

	return string_escape_quote( name, program_name );
}

PROGRAM *program_calloc( void )
{
	PROGRAM *program;

	if ( ! ( program = calloc( 1, sizeof( PROGRAM ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return program;
}

PROGRAM *program_new( char *program_name )
{
	PROGRAM *program = program_calloc();

	program->program_name = program_name;
	return program;
}

PROGRAM *program_fetch( char *program_name,
			boolean fetch_alias_list )
{
	char sys_string[ 1024 ];

	if ( !program_name || !*program_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty program_name.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf( sys_string,
		 "select.sh '*' %s \"%s\" select",
		 PROGRAM_TABLE,
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 program_primary_where( program_name ) );

	return
		program_parse(
			pipe2string( sys_string ),
			fetch_alias_list );
}

PROGRAM *program_parse(
			char *input,
			boolean fetch_alias_list )
{
	char program_name[ 128 ];
	char revenue_account[ 128 ];
	PROGRAM *program;

	if ( !input || !*input ) return (PROGRAM *)0;

	/* See: attribute_list */
	/* ------------------- */
	piece( program_name, SQL_DELIMITER, input, 0 );

	program =
		program_new(
			strdup( program_name ) );

	piece( revenue_account, SQL_DELIMITER, input, 1 );
	program->revenue_account = strdup( revenue_account );

	if ( fetch_alias_list )
	{
		program->program_alias_list =
			program_alias_list(
				program_name );
	}

	return program;
}

PROGRAM_ALIAS *program_alias_parse( char *input )
{
	char program_name[ 128 ];
	char alias_name[ 128 ];
	PROGRAM_ALIAS *program_alias;

	if ( !input || !*input ) return (PROGRAM_ALIAS *)0;

	/* See: attribute_list */
	/* ------------------- */
	piece( program_name, SQL_DELIMITER, input, 0 );
	piece( alias_name, SQL_DELIMITER, input, 1 );

	program_alias =
		program_alias_new(
			strdup( program_name ),
			strdup( alias_name ) );

	return program_alias;
}

LIST *program_list( boolean fetch_alias_list )
{
	return program_system_list(
			program_sys_string(
				"1 = 1" /* where */ ),
				fetch_alias_list );
}

PROGRAM_ALIAS *program_alias_new(
			char *program_name,
			char *alias_name )
{
	PROGRAM_ALIAS *program_alias;

	if ( ! ( program_alias = calloc( 1, sizeof( PROGRAM_ALIAS ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	program_alias->program_name = program_name;
	program_alias->alias_name = alias_name;

	return program_alias;
}

PROGRAM *program_seek(
			char *program_name,
			LIST *program_list )
{
	return
		program_name_seek(
			program_name,
			program_list );
}

PROGRAM *program_list_seek(
			char *program_name,
			LIST *program_list )
{
	return
		program_name_seek(
			program_name,
			program_list );
}

LIST *program_alias_list( char *program_name )
{
	char where[ 256 ];

	sprintf(where,
		"program_name = '%s'",
		program_name ); 

	return program_alias_system_list(
			program_alias_sys_string(
				where ) );
}

LIST *program_system_list(
			char *sys_string,
			boolean fetch_alias_list )
{
	char input[ 1024 ];
	FILE *input_pipe;
	LIST *program_list = list_new();

	input_pipe = popen( sys_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			program_list,
			program_parse(
				input,
				fetch_alias_list ) );
	}

	pclose( input_pipe );
	return program_list;
}

LIST *program_alias_system_list(
			char *sys_string )
{
	char input[ 1024 ];
	FILE *input_pipe;
	LIST *program_alias_list = list_new();

	input_pipe = popen( sys_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			program_alias_list,
			program_alias_parse(
				input ) );
	}

	pclose( input_pipe );
	return program_alias_list;
}

char *program_sys_string( char *where )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "select.sh '*' %s \"%s\" select",
		 PROGRAM_TABLE,
		 where );

	return strdup( sys_string );
}

char *program_alias_sys_string( char *where )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "select.sh '*' %s \"%s\" select",
		 PROGRAM_ALIAS_TABLE,
		 where );

	return strdup( sys_string );
}

PROGRAM *program_name_seek(
			char *program_name,
			LIST *program_list )
{
	PROGRAM *program;

	if ( !list_rewind( program_list ) ) return (PROGRAM *)0;

	do {
		program = list_get( program_list );

		if ( string_strcmp(	program->program_name,
					program_name ) == 0 )
		{
			return program;
		}

		if ( program_alias_seek(
			program_name
				/* program_alias_name */,
			program->program_alias_list ) )
		{
			return program;
		}
	} while ( list_next( program_list ) );

	return (PROGRAM *)0;
}

PROGRAM *program_seek_name(
			char *program_name,
			LIST *program_list )
{
	return
		program_list_seek(
			program_name,
			program_list );
}

PROGRAM_ALIAS *program_alias_seek(
			char *program_alias_name,
			LIST *program_alias_list )
{
	
	PROGRAM_ALIAS *program_alias;

	if ( !list_rewind( program_alias_list ) )
		return (PROGRAM_ALIAS *)0;

	do {
		program_alias = list_get( program_alias_list );

		if ( string_strcmp(
			program_alias->alias_name,
			program_alias_name ) == 0 )
		{
			return program_alias;
		}

	} while ( list_next( program_alias_list ) );

	return (PROGRAM_ALIAS *)0;
}

LIST *program_name_list( LIST *program_list )
{
	PROGRAM *program;
	LIST *name_list;

	if ( !list_rewind( program_list ) ) return (LIST *)0;

	name_list = list_new();

	do {
		program = list_get( program_list );

		list_set( name_list, program->program_name );

	} while ( list_next( program_list ) );

	return name_list;
}

LIST *program_alias_name_list(
			LIST *program_list )
{
	PROGRAM *program;
	PROGRAM_ALIAS *program_alias;
	LIST *name_list;

	if ( !list_rewind( program_list ) ) return (LIST *)0;

	name_list = list_new();

	do {
		program = list_get( program_list );

		if ( !list_rewind( program->program_alias_list ) )
			continue;

		do {
			program_alias =
				list_get(
					program->program_alias_list );

			list_set(	name_list,
					program_alias->alias_name );

		} while ( list_next( program->program_alias_list ) );

	} while ( list_next( program_list ) );

	return name_list;
}

void program_donation_total(
			char *program_name )
{
	char sys_string[ 1024 ];

	printf(	sys_string,
		"program_donation_total.sh \"%s\"",
		program_name_escape( program_name ) );

	if ( system( sys_string ) ){};
}
