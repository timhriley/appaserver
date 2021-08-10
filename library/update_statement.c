/* $APPASERVER_HOME/library/update_statement.c		*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "update_statement.h"

UPDATE_STATEMENT *update_statement_new( char *table_name )
{
	UPDATE_STATEMENT *u;

	u = calloc( 1, sizeof( UPDATE_STATEMENT ) );
	u->table_name = table_name;
	return u;
}

void update_statement_set_attribute_list(
			UPDATE_STATEMENT *u,
			LIST *attribute_list )
{
	u->attribute_list = attribute_list;
}

void update_statement_set_data_list(
			UPDATE_STATEMENT *u,
			LIST *data_list )
{
	u->data_list = data_list;
}

void update_statement_set_primary_attribute_list(
			UPDATE_STATEMENT *u,
			LIST *primary_attribute_list )
{
	u->primary_attribute_list = primary_attribute_list;
}

void update_statement_set_primary_data_list(
			UPDATE_STATEMENT *u,
			LIST *primary_data_list )
{
	u->primary_data_list = primary_data_list;
}

int update_statement_get( char *destination, UPDATE_STATEMENT *u )
{
	char where_clause[ 4096 ];
	char set_clause[ 4096 ];

	if ( !update_statement_set_clause(
					set_clause,
					u->attribute_list,
					u->data_list ) )
	{
		fprintf( stderr, 
		"ERROR: update_statement_get() cannot get set clause.\n" );
		return 0;
	}

	if ( !update_statement_where_clause(
					where_clause,
					u->primary_attribute_list,
					u->primary_data_list ) )
	{
		fprintf( stderr, 
		"ERROR: update_statement_get() cannot get where clause.\n" );
		return 0;
	}

	sprintf( destination,
		 "update %s set %s where %s;",
		 u->table_name,
		 set_clause,
		 where_clause );

	return 1;
}


int update_statement_set_clause(
			char *destination,
			LIST *attribute_list,
			LIST *data_list )
{
	int first_time = 1;
	char *attribute, *data;
	char *pointer = destination;

	if ( !list_rewind( attribute_list ) ) return 0;
	if ( !list_rewind( data_list ) ) return 0;

	if ( list_length( attribute_list ) !=
	     list_length( data_list ) )
	{
		return 0;
	}

	do {
		attribute = list_string( attribute_list );
		data = list_string( data_list );

		if ( first_time )
		{
			first_time = 0;

			if ( !*data )
			{
				pointer += sprintf( pointer, 
					    	"%s=null", 
					    	attribute );
			}
			else
			{
				pointer += sprintf( pointer, 
					    	"%s='%s'", 
					    	attribute,
					    	data );
			}
		}
		else
		{
			if ( !*data )
			{
				pointer += sprintf( pointer, 
					    	",%s=null", 
					    	attribute );
			}
			else
			{
				pointer += sprintf( pointer, 
					    	",%s='%s'", 
					    	attribute,
					    	data );
			}
		} /* if not first_time */

		list_next( data_list );
	} while( list_next( attribute_list ) );
	return 1;
}

int update_statement_where_clause(
			char *destination,
			LIST *primary_attribute_list,
			LIST *primary_data_list )
{
	int first_time = 1;
	char *attribute, *data;
	char *pointer = destination;

	if ( !list_rewind( primary_attribute_list ) ) return 0;
	if ( !list_rewind( primary_data_list ) ) return 0;

	if ( list_length( primary_attribute_list ) !=
	     list_length( primary_data_list ) )
	{
		return 0;
	}

	do {
		attribute = list_string( primary_attribute_list );
		data = list_string( primary_data_list );

		if ( first_time )
		{
			first_time = 0;

			if ( !*data )
			{
				printf( " and (%s = '' or %s is null)",
					attribute, attribute );
			}
			else
			{
				pointer += sprintf( pointer, 
					    	"%s='%s'", 
					    	attribute,
					    	data );
			}
		}
		else
		{
			if ( !*data )
			{
				printf( " and (%s = '' or %s is null)",
					attribute, attribute );
			}
			else
			{
				pointer += sprintf( pointer, 
					    	" and %s='%s'", 
					    	attribute,
					    	data );
			}
		} /* if not first_time */

		list_next( primary_data_list );
	} while( list_next( primary_attribute_list ) );
	return 1;
}

