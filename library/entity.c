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
#include "folder_attribute.h"
#include "security.h"
#include "entity_key.h"
#include "spool.h"
#include "optional_column.h"
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
		boolean contact_key_boolean,
		char *full_name,
		char *contact_key )
{
	ENTITY *entity;

	if ( !full_name
	||   !*full_name
	||   strcmp( full_name, ENTITY_FULL_NAME_COLUMN ) == 0 )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: full_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	entity = entity_calloc();

	entity->entity_contact_key_boolean = contact_key_boolean;

	entity->full_name = full_name;
	entity->contact_key = contact_key;

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
		boolean contact_key_boolean,
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
					contact_key_boolean,
					strdup( full_name ),
					strdup( contact_key ) );
		}
		else
		{
			entity =
				entity_new(
					contact_key_boolean,
					full_name,
					contact_key );
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
		char *contact_key,
		boolean contact_key_boolean )
{
	char *primary_where;
	char *select_string;
	char *system_string;
	char *input;

	if ( !full_name || !*full_name ) return NULL;

	primary_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_primary_where(
			contact_key_boolean,
			full_name,
			contact_key );

	select_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
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

	free( select_string );

	input =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_system_input(
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
	char *contact_key = {0};
	char buffer[ 128 ];
	ENTITY *entity;

	if ( !input || !*input ) return NULL;

	/* See entity_select_string() */
	/* -------------------------- */
	piece( full_name, SQL_DELIMITER, input, 0 );

	if ( contact_key_boolean )
	{
		piece( buffer, SQL_DELIMITER, input, 8 );
		if ( *buffer ) contact_key = strdup( buffer );
	}

	entity =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		entity_new(
			contact_key_boolean,
			strdup( full_name ),
			contact_key );

	piece( buffer, SQL_DELIMITER, input, 1 );
	if ( *buffer ) entity->street_address = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	if ( *buffer ) entity->city = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer ) entity->state_code = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	if ( *buffer ) entity->zip_code = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 5 );
	if ( *buffer ) entity->land_phone_number = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 6 );
	if ( *buffer ) entity->cell_phone_number = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 7 );
	if ( *buffer ) entity->email_address = strdup( buffer );

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

	if ( !entity_full_name_populated_boolean(
		ENTITY_FULL_NAME_COLUMN,
		full_name ) )
	{
		strcpy( where, "1 = 1 " );
	}
	else
	{
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
	}

	return where;
}

char *entity_fetch_contact_key(
		boolean contact_key_boolean,
		char *full_name )
{
	char *primary_where;
	char *system_string;
	char *contact_key = {0};

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

	if ( contact_key_boolean )
	{
		primary_where =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			entity_primary_where(
				contact_key_boolean,
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
			string_system_input(
				system_string );

		free( system_string );
	}

	return contact_key;
}

char *entity_insert_system_string(
		const char *entity_table,
		const char *entity_full_name_column,
		const char *entity_contact_key_column,
		const char sql_delimiter,
		boolean entity_contact_key_boolean,
		boolean ignore_duplicate_boolean )
{
	char *insert_column_string;
	char system_string[ 1024 ];

	insert_column_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		entity_insert_column_string(
			entity_full_name_column /* entity_insert */,
			entity_contact_key_column,
			entity_contact_key_boolean );

	snprintf(
		system_string,
		sizeof ( system_string ),
		"insert_statement table=%s field=%s delimiter='%c' | "
		"sql 2>&1 |"
		"%s",
		entity_table,
		insert_column_string,
		sql_delimiter,
		(ignore_duplicate_boolean)
			? "grep -vi duplicate"
	       		: "cat" );

	free( insert_column_string );

	return strdup( system_string );
}

char *entity_insert_data_string(
		const char sql_delimiter,
		boolean contact_key_boolean,
		char *full_name,
		char *contact_key )
{
	OPTIONAL_COLUMN *optional_column;

	if ( !full_name )
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

	optional_column =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		optional_column_new(
			sql_delimiter,
			full_name /* base_string */,
			contact_key /* component */,
			1 /* escape_boolean */,
			contact_key_boolean /* set_boolean */ );

	return optional_column->return_string;
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

LIST *entity_full_contact_list(
		LIST *full_name_list,
		LIST *contact_key_list )
{
	boolean contact_key_boolean;
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

	contact_key_boolean =
		entity_contact_key_boolean(
			ENTITY_TABLE,
			ENTITY_CONTACT_KEY_COLUMN );

	entity_list = list_new();

	do {
		entity =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			entity_new(
				contact_key_boolean,
				list_get( full_name_list ),
				list_get( contact_key_list ) );

		list_set( entity_list, entity );
		list_next( contact_key_list );

	} while( list_next( full_name_list ) );

	return entity_list;
}

ENTITY *entity_full_name_entity(
		boolean contact_key_boolean,
		char *full_name /* stack memory */ )
{
	ENTITY *entity;

	if ( !full_name ) return NULL;

	entity =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		entity_new(
			contact_key_boolean,
			strdup( full_name ),
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			entity_fetch_contact_key(
				contact_key_boolean,
				full_name ) );

	return entity;
}

char *entity_insert_column_string(
		const char *entity_insert,
		const char *entity_contact_key_column,
		boolean entity_contact_key_boolean )
{
	OPTIONAL_COLUMN *optional_column;

	optional_column =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		optional_column_new(
			',' /* delimiter */,
			(char *)entity_insert /* base_string */,
			(char *)entity_contact_key_column /* component */,
			0 /* not escape_boolean */,
			entity_contact_key_boolean /* set_boolean */ );

	return optional_column->return_string /* heap memory */;
}

char *entity_select_string(
		const char *entity_select,
		const char *entity_contact_key_column,
		boolean contact_key_boolean )
{
	OPTIONAL_COLUMN *optional_column;

	optional_column =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		optional_column_new(
			',' /* delimiter */,
			(char *)entity_select /* base_string */,
			(char *)entity_contact_key_column
				/* component column or datum */,
			0 /* not escape_boolean */,
			contact_key_boolean /* set_boolean */ );

	return optional_column->return_string;
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
		const char *entity_table,
		const char *entity_contact_key_column,
		const char *entity_contact_key_default,
		char *full_name,
		char *contact_key,
		boolean contact_key_boolean )
{
	char *key = {0};

	if ( contact_key_boolean )
	{
		if ( entity_contact_key_populated_boolean(
			entity_contact_key_column,
			contact_key ) )
		{
			key = contact_key;
		}
		else
		{
			key =
				/* --------------------------- */
				/* Returns heap memory or null */
				/* --------------------------- */
				entity_fetch_contact_key(
					1 /* entity_contact_key_boolean */,
					full_name );

			if ( !key )
			{
				key =
				    /* --------------------------- */
				    /* Returns heap memory or null */
				    /* --------------------------- */
				    folder_attribute_fetch_default_value(
					(char *)entity_table
						/* folder_name */,
					(char *)ENTITY_CONTACT_KEY_COLUMN
						/* attribute_name */ );
			}

			if ( !key )
			{
				key = (char *)entity_contact_key_default;
			}
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
	{
		list_set( list, (char *)entity_contact_key_column );
	}

	return list;
}

boolean entity_full_name_populated_boolean(
		const char *entity_full_name_column,
		char *full_name )
{
	return
	entity_contact_key_populated_boolean(
		entity_full_name_column
			/* entity_contact_key_column */,
		full_name
			/* contact_key */ );
}

boolean entity_contact_key_populated_boolean(
		const char *entity_contact_key_column,
		char *contact_key )
{
	boolean populated_boolean = 0;

	if (	contact_key
	&&	*contact_key
	&&	strcmp(
			contact_key,
			entity_contact_key_column ) != 0 )
	{
		populated_boolean = 1;
	}

	return populated_boolean;
}

char *entity_insert(
		const char *entity_table,
		const char *entity_full_name_column,
		const char *entity_contact_key_column,
		boolean contact_key_boolean,
		char *full_name,
		char *contact_key,
		boolean ignore_duplicate_boolean )
{
	char *system_string;
	char *data_string;
	char *spool_string;

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		entity_insert_system_string(
			entity_table,
			entity_full_name_column,
			entity_contact_key_column,
			SQL_DELIMITER,
			contact_key_boolean,
			ignore_duplicate_boolean );

	data_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		entity_insert_data_string(
			SQL_DELIMITER,
			contact_key_boolean,
			full_name,
			contact_key );

	spool_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		spool_data_string(
			system_string,
			data_string );

	free( system_string );
	free( data_string );

	return spool_string;
}

