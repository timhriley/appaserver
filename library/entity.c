/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/entity.c					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "piece.h"
#include "sql.h"
#include "appaserver_error.h"
#include "folder_attribute.h"
#include "entity.h"

ENTITY *entity_calloc( void )
{
	ENTITY *entity;

	if ( ! ( entity = calloc( 1, sizeof ( ENTITY ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return entity;
}

ENTITY *entity_new(
		char *full_name,
		char *street_address )
{
	ENTITY *entity;

	if ( !full_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: full_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}


	entity = entity_calloc();

	entity->full_name = full_name;
	entity->street_address = street_address;

	return entity;
}

ENTITY *entity_full_name_seek(
		char *full_name,
		LIST *entity_list )
{
	ENTITY *entity;

	if ( list_rewind( entity_list ) )
	do {
		entity = list_get( entity_list );

		if ( string_strcmp(	entity->full_name,
					full_name ) == 0 )
		{
			return entity;
		}

	} while( list_next( entity_list ) );

	return NULL;
}

ENTITY *entity_seek(
		char *full_name,
		char *street_address,
		LIST *entity_list )
{
	ENTITY *entity;

	if ( list_rewind( entity_list ) )
	do {
		entity = list_get( entity_list );

		if ( string_strcmp(	entity->full_name,
					full_name ) == 0
		&&   string_strcmp(	entity->street_address,
					street_address ) == 0 )
		{
			return entity;
		}

	} while( list_next( entity_list ) );

	return NULL;
}

ENTITY *entity_getset(
		LIST *entity_list,
		char *full_name,
		char *street_address,
		boolean with_strdup )
{
	ENTITY *entity;

	if ( ! ( entity =
			entity_seek(
				full_name,
				street_address,
				entity_list ) ) )
	{
		if ( with_strdup )
		{
			entity =
				entity_new(
					strdup( full_name ),
					strdup( street_address ) );
		}
		else
		{
			entity = entity_new( full_name, street_address );
		}

		list_set( entity_list, entity );
	}
	return entity;
}

boolean entity_list_exists(
		char *full_name,
		char *street_address,
		LIST *entity_list )
{
	if ( !entity_list )
		return 0;
	else
	if ( entity_seek(
		full_name,
		street_address,
		entity_list ) )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

ENTITY *entity_fetch(
		char *full_name,
		char *street_address )
{
	FILE *input_pipe;
	char input[ 1024 ];
	ENTITY *entity;

	if ( !full_name || !street_address ) return NULL;

	input_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		entity_input_pipe(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			entity_system_string(
				ENTITY_SELECT,
				ENTITY_TABLE,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				entity_primary_where(
					full_name,
					street_address ) ) );

	entity =
		entity_parse(
			/* ----------------------------------------- */
			/* Returns input_buffer or null if all done. */
			/* ----------------------------------------- */
			string_input(
				input,
				input_pipe,
				1024 ) );

	pclose( input_pipe );

	return entity;
}

ENTITY *entity_parse( char *input )
{
	char full_name[ 128 ];
	char street_address[ 128 ];
	char piece_buffer[ 128 ];
	ENTITY *entity;

	if ( !input || !*input ) return NULL;

	/* See ENTITY_SELECT */
	/* ----------------- */
	piece( full_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );

	entity =
		entity_new(
			strdup( full_name ),
			strdup( street_address ) );

	piece( piece_buffer, SQL_DELIMITER, input, 2 );
	if ( *piece_buffer ) entity->city = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	if ( *piece_buffer ) entity->state_code = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	if ( *piece_buffer ) entity->zip_code = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 5 );
	if ( *piece_buffer ) entity->land_phone_number = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 6 );
	if ( *piece_buffer ) entity->email_address = strdup( piece_buffer );

	return entity;
}

char *entity_name_escape( char *full_name )
{
	return entity_escape_full_name( full_name );
}

char *entity_escape_name( char *full_name )
{
	return entity_escape_full_name( full_name );
}

char *entity_escape_full_name( char *full_name )
{
	static char escape_full_name[ 256 ];

	if ( !full_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty full_name.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return string_escape_quote( escape_full_name, full_name );
}

char *entity_primary_where(
		char *full_name,
		char *street_address )
{
	static char where[ 256 ];

	sprintf( where,
		 "full_name = '%s' and	"
		 "street_address = '%s'	",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 entity_escape_full_name( full_name ),
		 street_address );

	return where;
}

char *entity_street_address( char *full_name )
{
	static LIST *entity_list = {0};
	ENTITY *entity;

	if ( !entity_list )
	{
		entity_list =
			entity_system_list(
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				entity_system_string(
					ENTITY_SELECT,
					ENTITY_TABLE,
					"1 = 1" ) );
	}

	if ( ( entity = entity_full_name_seek( full_name, entity_list ) ) )
	{
		return entity->street_address;
	}
	else
	{
		return (char *)0;
	}
}

FILE *entity_insert_open( char *error_filename )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"insert_statement table=%s field=\"%s\" delimiter='%c'	|"
		"sql 2>&1						|"
		"grep -vi duplicate					|"
		"cat >%s 2>&1						 ",
		ENTITY_TABLE,
		ENTITY_INSERT_COLUMNS,
		SQL_DELIMITER,
		error_filename );

	return popen( sys_string, "w" );
}

void entity_insert_pipe(
		FILE *insert_pipe,
		char *full_name,
		char *street_address,
		char *email_address )
{
	fprintf(insert_pipe,
		"%s^%s^%s\n",
		entity_escape_name( full_name ),
		street_address,
		(email_address)
			? email_address
			: "" );
}

char *entity_name_display(
		char *full_name,
		char *street_address )
{
	static char display[ 256 ];

	if ( !full_name || !*full_name ) return "";

	if ( !street_address
	||   !*street_address
	||   string_strcmp( street_address, "unknown" ) == 0
	||   string_strcmp( street_address, "null" ) == 0 )
	{
		strcpy( display, full_name );
	}
	else
	{
		sprintf( display, "%s/%s", full_name, street_address );
	}

	return display;
}

FILE *entity_input_pipe( char *system_string )
{
	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	popen( system_string, "r" );
}

LIST *entity_system_list( char *system_string )
{
	FILE *input_pipe;
	char input[ 1024 ];
	LIST *entity_list;

	if ( !system_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: system_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	entity_list = list_new();

	/* -------------- */
	/* Safely returns */
	/* -------------- */
	input_pipe = entity_input_pipe( system_string );

	/* ----------------------------------------- */
	/* Returns input_buffer or null if all done. */
	/* ----------------------------------------- */
	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set( entity_list, entity_parse( input ) );
	}

	pclose( input_pipe );

	return entity_list;
}

char *entity_system_string(
		const char *select,
		const char *table,
		char *where )
{
	char system_string[ 1024 ];

	if ( !where ) where = "1 = 1";

	sprintf( system_string,
		 "select.sh '%s' %s \"%s\" select",
		 select,
		 table,
		 where );

	return strdup( system_string );
}

LIST *entity_full_street_list(
		LIST *full_name_list,
		LIST *street_address_list )
{
	LIST *entity_list;

	if ( !list_length( full_name_list ) ) return (LIST *)0;

	if (	list_length( full_name_list ) !=
		list_length( street_address_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: list_lengths not same [%d vs. %d]\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			list_length( full_name_list ),
			list_length( street_address_list ) );
		exit( 1 );
	}

	list_rewind( full_name_list );
	list_rewind( street_address_list );
	entity_list = list_new();

	do {
		list_set(
			entity_list,
			entity_new(
				list_get( full_name_list ),
				list_get( street_address_list ) ) );

		list_next( street_address_list );

	} while( list_next( full_name_list ) );

	return entity_list;
}

ENTITY *entity_full_name_entity(
		/* ------------------- */
		/* Expect stack memory */
		/* ------------------- */
		char *full_name )
{
	ENTITY *entity;
	char *street_address;

	if ( !full_name ) return (ENTITY *)0;

	if ( ( street_address =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			entity_street_address(
				full_name ) ) )
	{
		entity =
			entity_new(
				strdup( full_name ),
				street_address );
	}
	else
	{
		entity =
			entity_new(
				strdup( full_name ),
				ENTITY_STREET_ADDRESS_ANY );
	}
	return entity;
}

boolean entity_login_name_boolean(
		const char *entity_table,
		const char *appaserver_user_primary_key )
{
	return
	folder_attribute_exists(
		(char *)entity_table
			/* folder_name */,
		(char *)appaserver_user_primary_key
			/* attribute_name */ );
}

ENTITY *entity_login_name_fetch(
		const char *entity_table,
		const char *appaserver_user_primary_key,
		char *login_name )
{
	char *where;
	char *system_string;
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	ENTITY *entity;

	if ( !login_name )
	{
		char message[ 128 ];

		sprintf(message, "login_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_login_name_where(
			appaserver_user_primary_key,
			login_name );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		entity_system_string(
			ENTITY_SELECT,
			entity_table,
			where );

	input_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		entity_input_pipe(
			system_string );

	entity =
		entity_parse(
			/* ----------------------------------------- */
			/* Returns input_buffer or null if all done. */
			/* ----------------------------------------- */
			string_input(
				input_buffer,
				input_pipe,
				1024 ) );

	pclose( input_pipe );

	return entity;
}

char *entity_login_name_where(
		const char *appaserver_user_primary_key,
		char *login_name )
{
	static char where[ 128 ];

	if ( !login_name )
	{
		char message[ 128 ];

		sprintf(message, "login_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(where,
		"%s = '%s'",
		appaserver_user_primary_key,
		login_name );

	return where;
}

