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
#include "appaserver.h"
#include "appaserver_error.h"
#include "security.h"
#include "entity_key.h"
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

ENTITY *entity_new( char *full_name )
{
	ENTITY *entity;

	if ( !full_name
	||   !*full_name
	||   strcmp( full_name, "full_name" ) == 0 )
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

		if ( string_strcmp(
			entity->full_name,
			full_name ) == 0 )
		{
			return entity;
		}

	} while( list_next( entity_list ) );

	return NULL;
}

ENTITY *entity_seek(
		char *full_name,
		char *contact_key,
		LIST *entity_list )
{
	ENTITY *entity;

	if ( list_rewind( entity_list ) )
	do {
		entity = list_get( entity_list );

		if ( contact_key )
		{
			if ( string_strcmp(
				entity->contact_key,
				contact_key ) != 0 )
			{
				continue;
			}
		}

		if ( string_strcmp(
			entity->full_name,
			full_name ) == 0 )
		{
			return entity;
		}

	} while( list_next( entity_list ) );

	return NULL;
}

ENTITY *entity_getset(
		LIST *entity_list,
		char *full_name,
		char *contact_key,
		boolean strdup_boolean )
{
	ENTITY *entity;

	if ( ! ( entity =
			entity_seek(
				full_name,
				contact_key,
				entity_list ) ) )
	{
		if ( strdup_boolean )
		{
			entity =
				entity_new(
					strdup( full_name ) );

			if ( contact_key )
			{
				entity->contact_key = strdup( contact_key );
			}
		}
		else
		{
			entity = entity_new( full_name );
			entity->contact_key = contact_key;
		}

		list_set( entity_list, entity );
	}

	return entity;
}

boolean entity_list_exist_boolean(
		char *full_name,
		char *contact_key,
		LIST *entity_list )
{
	boolean exist_boolean = 0;

	if ( entity_seek(
			full_name,
			contact_key,
			entity_list ) )
	{
		exist_boolean = 1;
	}

	return exist_boolean;
}

ENTITY *entity_fetch(
		char *full_name,
		char *contact_key )
{
	boolean contact_key_boolean;
	char *primary_where;
	char *select_string;
	char *system_string;
	char *input;

	if ( !full_name || !*full_name ) return NULL;

	contact_key_boolean =
		entity_contact_key_boolean(
			ENTITY_TABLE,
			ENTITY_CONTACT_KEY_COLUMN );

	primary_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_primary_where(
			contact_key_boolean,
			full_name,
			contact_key );

	select_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_select_string(
			ENTITY_SELECT,
			ENTITY_CONTACT_KEY_COLUMN,
			contact_key_boolean );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			select_string,
			ENTITY_TABLE,
			primary_where );

	input =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_system_string_input(
			system_string );

	return
	entity_parse(
		contact_key_boolean,
		input );
}

ENTITY *entity_parse(
		boolean contact_key_boolean,
		char *input )
{
	char full_name[ 128 ];
	char piece_buffer[ 128 ];
	ENTITY *entity;

	if ( !input || !*input ) return NULL;

	/* See entity_select_string() */
	/* -------------------------- */
	piece( full_name, SQL_DELIMITER, input, 0 );

	/* Safely returns */
	/* -------------- */
	entity = entity_new( strdup( full_name ) );

	piece( piece_buffer, SQL_DELIMITER, input, 1 );
	if ( *piece_buffer ) entity->street_address = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 2 );
	if ( *piece_buffer ) entity->city = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	if ( *piece_buffer ) entity->state_code = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	if ( *piece_buffer ) entity->zip_code = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 5 );
	if ( *piece_buffer ) entity->land_phone_number = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 6 );
	if ( *piece_buffer ) entity->cell_phone_number = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 7 );
	if ( *piece_buffer ) entity->email_address = strdup( piece_buffer );

	if ( contact_key_boolean )
	{
		piece( piece_buffer, SQL_DELIMITER, input, 8 );
		if ( *piece_buffer )
			entity->contact_key =
				strdup( piece_buffer );

		entity->contact_key_boolean = contact_key_boolean;

	}

	return entity;
}

char *entity_escape_full_name( char *full_name )
{
	if ( !full_name || !*full_name )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"full_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	security_sql_injection_escape(
		SECURITY_ESCAPE_CHARACTER_STRING,
		full_name );
}

char *entity_escape_contact_key( char *contact_key )
{
	if ( !contact_key || !*contact_key )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"contact_key is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	security_sql_injection_escape(
		SECURITY_ESCAPE_CHARACTER_STRING,
		contact_key );
}

char *entity_primary_where(
		boolean contact_key_boolean,
		char *full_name,
		char *contact_key )
{
	static char where[ 256 ];
	char *escape_full_name;
	char *contact_key_where;

	escape_full_name =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		entity_escape_full_name(
			full_name );

	contact_key_where =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		entity_contact_key_where(
			contact_key_boolean,
			contact_key );

	snprintf(
		where,
		sizeof ( where ),
		"full_name = '%s' and "
		"%s",
		escape_full_name,
		contact_key_where );

	free( escape_full_name );
	free( contact_key_where );

	return where;
}

char *entity_fetch_contact_key( char *full_name )
{
	char *primary_where;
	char *system_string;
	char *contact_key;

	if ( !full_name || !*full_name )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"full_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	primary_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_primary_where(
			entity_contact_key_boolean(
				ENTITY_TABLE /* table_name */,
				ENTITY_CONTACT_KEY_COLUMN /* column_name */ ),
			full_name,
			(char *)0 /* contact_key */ );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			ENTITY_CONTACT_KEY_COLUMN /* select */,
			ENTITY_TABLE,
			primary_where );

	contact_key =
		string_system_string_input(
			system_string );

	free( system_string );

	return contact_key;
}

char *entity_insert_system_string(
		const char *entity_table,
		const char *entity_full_name_column,
		const char *entity_contact_key_column,
		const char sql_delimiter,
		boolean entity_contact_key_boolean )
{
	char system_string[ 1024 ];
	char *field_string;

	field_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		entity_insert_field_string(
			entity_full_name_column,
			entity_contact_key_column,
			entity_contact_key_boolean );

	snprintf(
		system_string,
		sizeof ( system_string ),
		"insert_statement table=%s field=%s delimiter='%c' | "
		"sql",
		entity_table,
		field_string,
		sql_delimiter );

	free( field_string );

	return strdup( system_string );
}

char *entity_insert_data_string(
		const char sql_delimiter,
		boolean contact_key_boolean,
		char *full_name,
		char *contact_key )
{
	char data_string[ 1024 ];
	char *ptr = data_string;
	char *tmp;

	ptr += sprintf( ptr,
		"%s",
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		( tmp = entity_escape_full_name( full_name ) ) );

	free( tmp );

	if ( contact_key_boolean )
	{
		ptr += sprintf( ptr,
			"%c%s",
			sql_delimiter,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			( tmp = entity_escape_contact_key( contact_key ) ) );
	
		free( tmp );
	}

	ptr += sprintf( ptr, "\n" );

	return strdup( data_string );
}

char *entity_name_display(
		char *full_name,
		char *contact_key )
{
	static char display[ 256 ];

	*display = '\0';

	if ( full_name )
	{
		if ( !contact_key
		||   !*contact_key
		||   strcmp(
			contact_key,
			ENTITY_CONTACT_KEY_UNKNOWN ) == 0
		||   strcmp(
			contact_key,
			ENTITY_CONTACT_KEY_NULL ) == 0 )
		{
			string_strcpy(
				display,
				full_name,
				sizeof ( display ) );
		}
		else
		{
			snprintf(
				display,
				sizeof ( display ),
				"%s/%s",
				full_name,
				contact_key );
		}
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

LIST *entity_full_contact_list(
		LIST *full_name_list,
		LIST *contact_key_list )
{
	LIST *entity_list;
	ENTITY *entity;

	if ( !list_length( full_name_list ) ) return NULL;

	if (	list_length( full_name_list ) !=
		list_length( contact_key_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: list_lengths not same [%d vs. %d]\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			list_length( full_name_list ),
			list_length( contact_key_list ) );
		exit( 1 );
	}

	list_rewind( full_name_list );
	list_rewind( contact_key_list );

	entity_list = list_new();

	do {
		entity =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			entity_new(
				list_get( full_name_list ) );

		entity->contact_key = list_get( contact_key_list );

		list_set( entity_list, entity );
		list_next( contact_key_list );

	} while( list_next( full_name_list ) );

	return entity_list;
}

ENTITY *entity_full_name_entity( char *full_name /* stack memory */ )
{
	ENTITY *entity;

	if ( !full_name ) return NULL;

	/* Safely returns */
	/* -------------- */
	entity = entity_new( strdup( full_name ) );

	entity->contact_key =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		entity_fetch_contact_key(
			full_name );

	return entity;
}

char *entity_insert_field_string(
		const char *entity_full_name_column,
		const char *entity_contact_key_column,
		boolean entity_contact_key_boolean )
{
	char field_string[ 128 ];
	char *ptr = field_string;

	ptr += sprintf( ptr,
		"%s",
		entity_full_name_column );

	if ( entity_contact_key_boolean )
	{
		ptr += sprintf( ptr,
			",%s",
			entity_contact_key_column );
	}

	return strdup( field_string );
}

char *entity_select_string(
		const char *entity_select,
		const char *entity_contact_key_column,
		boolean contact_key_boolean )
{
	static char select_string[ 256 ];

	string_strcpy(
		select_string,
		(char *)entity_select,
		sizeof ( select_string ) );

	if ( contact_key_boolean )
	{
		sprintf(select_string + strlen( select_string ),
			",%s",
			entity_contact_key_column );
	}

	return select_string;
}

char *entity_contact_key_where(
		boolean entity_contact_key_boolean,
		char *contact_key )
{
	char where[ 1024 ];

	if ( !entity_contact_key_boolean
	||   !contact_key )
	{
		strcpy( where, "1 = 1" );
	}
	else
	{
		char *escape_contact_key;

		escape_contact_key =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			entity_escape_contact_key(
				contact_key );

		snprintf(
			where,
			sizeof ( where ),
			"contact_key = '%s'",
			escape_contact_key );

		free( escape_contact_key );
	}

	return strdup( where );
}

boolean entity_contact_key_boolean(
		const char *entity_table,
		const char *entity_contact_key_column )
{
	static boolean contact_key_boolean = -1;

	if ( contact_key_boolean == -1 )
	{
		contact_key_boolean =
			appaserver_table_column_boolean(
				entity_table /* table_name */,
				entity_contact_key_column /* column_name */ );
	}

	return contact_key_boolean;
}

char *entity_contact_key(
		const char *entity_contact_key_column,
		const char *entity_contact_key_default,
		char *contact_key,
		boolean entity_contact_key_boolean,
		boolean stack_memory_boolean )
{
	char *key = {0};

	if ( entity_contact_key_boolean )
	{
		if (	!contact_key
		||	!*contact_key
		||	strcmp(
				contact_key,
				entity_contact_key_column ) == 0 )
		{
			key = (char *)entity_contact_key_default;
		}
		else
		{
			if ( stack_memory_boolean )
				key = strdup( contact_key );
			else
				key = contact_key;
		}
	}

	return key;
}

char *entity_contact_key_datum(
		const char sql_delimiter,
		char *contact_key,
		boolean contact_key_boolean )
{
	static char datum[ 32 ];

	if ( contact_key_boolean )
	{
		if ( !contact_key || !*contact_key )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
				"contact_key is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		snprintf(
			datum,
			sizeof ( datum ),
			"%c%s",
			sql_delimiter,
			contact_key );
	}

	return datum;
}

LIST *entity_primary_key_list(
		const char *entity_full_name_column,
		const char *entity_contact_key_column,
		boolean entity_contact_key_boolean )
{
	LIST *list = list_new();

	list_set( list, (char *)entity_full_name_column );

	if ( entity_contact_key_boolean )
		list_set( list, (char *)entity_contact_key_column );

	return list;
}
