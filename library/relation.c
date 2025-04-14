/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/relation.c		 			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "piece.h"
#include "timlib.h"
#include "sql.h"
#include "appaserver_error.h"
#include "foreign_attribute.h"
#include "folder_attribute.h"
#include "appaserver.h"
#include "relation.h"

LIST *relation_cache_list( void )
{
	static LIST *cache_list = {0};
	char *system_string;
	FILE *input_pipe;
	char input[ STRING_64K ];

	if ( cache_list ) return cache_list;

	cache_list = list_new();

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		relation_system_string(
			RELATION_SELECT,
			RELATION_TABLE,
			"pair_one2m_order" /* order_string */ );

	input_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_pipe(
			system_string );

	while ( string_input( input, input_pipe, sizeof ( input ) ) )
	{
		list_set(
			cache_list,
			relation_parse( input ) );
	}

	pclose( input_pipe );
	return cache_list;
}

RELATION *relation_parse( char *input )
{
	char many_folder_name[ 128 ];
	char one_folder_name[ 128 ];
	char related_attribute_name[ 128 ];
	char field[ 128 ];
	char hint_message[ 65536 ];
	RELATION *relation;

	if ( !input || !*input ) return NULL;

	/* See: RELATION_SELECT */
	/* -------------------- */
	piece( many_folder_name, SQL_DELIMITER, input, 0 );
	piece( one_folder_name, SQL_DELIMITER, input, 1 );
	piece( related_attribute_name, SQL_DELIMITER, input, 2 );

	relation =
		relation_new(
			strdup( many_folder_name ),
			strdup( one_folder_name ),
			strdup( related_attribute_name ) );

	piece( field, SQL_DELIMITER, input, 3 );
	if ( *field ) relation->pair_one2m_order = atoi( field );

	piece( field, SQL_DELIMITER, input, 4 );
	relation->omit_drillthru = ( *field == 'y' );

	piece( field, SQL_DELIMITER, input, 5 );
	relation->omit_drilldown = ( *field == 'y' );

	piece( field, SQL_DELIMITER, input, 6 );
	relation->relation_type_isa = ( *field == 'y' );

	piece( field, SQL_DELIMITER, input, 7 );
	relation->copy_common_attributes = ( *field == 'y' );

	piece( field, SQL_DELIMITER, input, 8 );
	relation->automatic_preselection = ( *field == 'y' );

	piece( field, SQL_DELIMITER, input, 9 );
	relation->drop_down_multi_select = ( *field == 'y' );

	piece( field, SQL_DELIMITER, input, 10 );
	relation->join_one2m_each_row = ( *field == 'y' );

	piece( field, SQL_DELIMITER, input, 11 );
	relation->ajax_fill_drop_down = ( *field == 'y' );

	piece( hint_message, SQL_DELIMITER, input, 12 );
	if ( *hint_message ) relation->hint_message = strdup( hint_message );

	return relation;
}

LIST *relation_seek_mto1_list( char *many_folder_name )
{
	LIST *cache_list;
	LIST *list = {0};
	RELATION *relation;

	if ( !many_folder_name )
	{
		char message[ 128 ];

		sprintf(message, "many_folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	/* Returns static LIST * */
	/* --------------------- */
	cache_list = relation_cache_list();

	if ( list_rewind( cache_list ) )
	do {
		relation = list_get( cache_list );

		if ( strcmp(
			relation->many_folder_name,
			many_folder_name ) == 0 )
		{
			if ( !list ) list = list_new();

			list_set( list, relation );
		}

	} while ( list_next( cache_list ) );

	return list;
}

LIST *relation_seek_one2m_list( char *one_folder_name )
{
	LIST *cache_list;
	LIST *list = {0};
	RELATION *relation;

	if ( !one_folder_name )
	{
		char message[ 128 ];

		sprintf(message, "one_folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	/* Returns static LIST * */
	/* --------------------- */
	cache_list = relation_cache_list();

	if ( list_rewind( cache_list ) )
	do {
		relation = list_get( cache_list );

		if ( strcmp(
			relation->one_folder_name,
			one_folder_name ) == 0 )
		{
			if ( !list ) list = list_new();

			list_set( list, relation );
		}

	} while ( list_next( cache_list ) );

	return list;
}

RELATION *relation_calloc( void )
{
	RELATION *relation;

	if ( ! ( relation = calloc( 1, sizeof ( RELATION ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return relation;
}

RELATION *relation_new(
		char *many_folder_name,
		char *one_folder_name,
		char *related_attribute_name )
{
	RELATION *relation;

	if ( !many_folder_name
	||   !one_folder_name
	||   !related_attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	relation = relation_calloc();
	relation->many_folder_name = many_folder_name;
	relation->one_folder_name = one_folder_name;
	relation->related_attribute_name = related_attribute_name;

	return relation;
}

LIST *relation_foreign_key_list(
		LIST *one_folder_primary_key_list,
		char *related_attribute_name,
		LIST *foreign_attribute_name_list )
{
	LIST *return_list;

	if ( !list_length( one_folder_primary_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "one_folder_primary_key_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !related_attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "related_attribute_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_length( foreign_attribute_name_list ) )
	{
		return_list = foreign_attribute_name_list;
	}
	else
	if ( strcmp( related_attribute_name, "null" ) != 0 )
	{
		return_list =
			list_duplicate(
				one_folder_primary_key_list );

		list_replace_last( 
			return_list,
			related_attribute_name );
	}
	else
	{
		return_list =
			list_duplicate(
				one_folder_primary_key_list );
	}

	return return_list;
}

char *relation_name(
		const char attribute_multi_key_delimiter,
		LIST *relation_foreign_key_list )
{
	if ( !list_length( relation_foreign_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "relation_foreign_key_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	/* ------------------------- */
	/* Returns heap memory or "" */
	/* ------------------------- */
	list_display_delimited(
		relation_foreign_key_list,
		(char)attribute_multi_key_delimiter );
}

char *relation_prompt(
		char *one_folder_name,
		char *related_attribute_name,
		LIST *foreign_attribute_name_list,
		boolean relation_foreign_key_exists_primary )
{
	char prompt[ 256 ];
	char *ptr = prompt;

	if ( !one_folder_name
	||   !related_attribute_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( relation_foreign_key_exists_primary )
		ptr += sprintf( ptr, "*" );

	if ( list_length( foreign_attribute_name_list ) )
	{
		char buffer[ 256 ];
		char *display_delimited;

		display_delimited =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			list_display_delimited(
				foreign_attribute_name_list,
				',' );

		ptr += sprintf(
			ptr,
			"%s",
			string_initial_capital(
				buffer,
				display_delimited ) );

		free( display_delimited );
	}
	else
	if ( strcmp( related_attribute_name, "null" ) != 0 )
	{
		char buffer1[ 128 ];
		char buffer2[ 128 ];

		ptr += sprintf(
			ptr,
			"%s (%s)",
			string_initial_capital(
				buffer1,
				one_folder_name ),
			string_initial_capital(
				buffer2,
				related_attribute_name ) );
	}
	else
	{
		char buffer[ 128 ];

		ptr += sprintf(
			ptr,
			"%s",
			string_initial_capital(
				buffer,
				one_folder_name ) );
	}

	return strdup( prompt );
}

boolean relation_foreign_key_exists_primary(
		LIST *many_folder_primary_key_list,
		LIST *relation_foreign_key_list )
{
	char *foreign_key;

	if ( !list_rewind( relation_foreign_key_list )
	||   !list_length( many_folder_primary_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		foreign_key = list_get( relation_foreign_key_list );

		if ( list_string_exists(
			foreign_key,
			many_folder_primary_key_list ) )
		{
			return 1;
		}
	} while ( list_next( relation_foreign_key_list ) );

	return 0;
}

char *relation_insert_statement(
		char *relation_table,
		char *relation_primary_key,
		char *many_folder_name,
		char *one_folder_name,
		char *related_attribute_name )
{
	static char insert_statement[ 256 ];

	if ( !relation_table
	||   !relation_primary_key
	||   !many_folder_name
	||   !one_folder_name
	||   !related_attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(insert_statement,
		"insert into %s (%s) values ('%s','%s','%s');",
		relation_table,
		relation_primary_key,
		many_folder_name,
		one_folder_name,
		related_attribute_name );

	return insert_statement;
}

char *relation_mnemonic( LIST *foreign_key_list )
{
	if ( !list_length( foreign_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "foreign_key_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	appaserver_mnemonic( foreign_key_list );
}

LIST *relation_translate_list(
		LIST *primary_key_list,
		LIST *relation_foreign_key_list )
{
	LIST *translate_list;

	if ( !list_length( primary_key_list )
	||   !list_length( relation_foreign_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if (	list_length( primary_key_list ) !=
		list_length( relation_foreign_key_list ) )
	{
		char message[ 128 ];

		sprintf(message,
			"list_length()=%d != list_length()=%d.",
			list_length( primary_key_list ),
			list_length( relation_foreign_key_list ) );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	translate_list = list_new();

	list_rewind( primary_key_list );
	list_rewind( relation_foreign_key_list );

	do {
		list_set(
			translate_list,
			relation_translate_new(
				list_get( primary_key_list )
					/* primary_key */,
				list_get( relation_foreign_key_list )
					/* foreign_key */ ) );

		list_next( relation_foreign_key_list );

	} while ( list_next( primary_key_list ) );

	return translate_list;
}

RELATION_TRANSLATE *relation_translate_new(
		char *primary_key,
		char *foreign_key )
{
	RELATION_TRANSLATE *relation_translate;

	if ( !primary_key
	||   !foreign_key )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	relation_translate = relation_translate_calloc();

	relation_translate->primary_key = primary_key;
	relation_translate->foreign_key = foreign_key;

	return relation_translate;
}

RELATION_TRANSLATE *relation_translate_calloc( void )
{
	RELATION_TRANSLATE *relation_translate;

	if ( ! ( relation_translate =
			calloc( 1,
				sizeof ( RELATION_TRANSLATE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return relation_translate;
}

char *relation_translate_list_display( LIST *relation_translate_list )
{
	char display[ 4096 ];
	char *ptr = display;
	RELATION_TRANSLATE *relation_translate;

	*ptr = '\0';

	if ( list_rewind( relation_translate_list ) )
	do {
		relation_translate =
			list_get(
				relation_translate_list );

		ptr += sprintf(
			ptr,
			"\nprimary_key=%s, foreign_key=%s\n",
			relation_translate->primary_key,
			relation_translate->foreign_key );

	} while ( list_next( relation_translate_list ) );

	if ( !*display )
		return "NULL";
	else
		return strdup( display );
}

char *relation_translate_primary_delimited_string(
		LIST *relation_translate_list )
{
	char string[ 2048 ];
	char *ptr = string;
	RELATION_TRANSLATE *relation_translate;

	*ptr = '\0';

	if ( list_rewind( relation_translate_list ) )
	do {
		relation_translate =
			list_get(
				relation_translate_list );

		if ( ptr != string ) ptr += sprintf( ptr, "," );

		ptr += sprintf(
			ptr,
			"%s",
			relation_translate->primary_key );

	} while ( list_next( relation_translate_list ) );

	return strdup( string );
}

char *relation_translate_foreign_delimited_string(
		LIST *relation_translate_list )
{
	char string[ 2048 ];
	char *ptr = string;
	RELATION_TRANSLATE *relation_translate;

	*ptr = '\0';

	if ( list_rewind( relation_translate_list ) )
	do {
		relation_translate =
			list_get(
				relation_translate_list );

		if ( ptr != string ) ptr += sprintf( ptr, "," );

		ptr += sprintf(
			ptr,
			"%s",
			relation_translate->foreign_key );

	} while ( list_next( relation_translate_list ) );

	return strdup( string );
}

char *relation_list_display( LIST *relation_list )
{
	RELATION *relation;
	char display[ 65536 ];
	char *ptr = display;

	*ptr = '\0';

	if ( list_rewind( relation_list ) )
	do {
		relation = list_get( relation_list );

		if ( ptr != display ) ptr += sprintf( ptr, ";\n" );

		ptr += sprintf( ptr,
			"many_folder_name=%s, "
			"one_folder_name=%s, "
			"related_attribute_name=%s, "
			"pair_one2m_order=%d, "
			"omit_drillthru=%d, "
			"omit_drilldown=%d, "
			"relation_type_isa=%d, "
			"copy_common_attributes=%d, "
			"automatic_preselection=%d, "
			"drop_down_multi_select=%d, "
			"join_one2m_each_row=%d, "
			"ajax_fill_drop_down=%d",
			relation->many_folder_name,
			relation->one_folder_name,
			relation->related_attribute_name,
			relation->pair_one2m_order,
			relation->omit_drillthru,
			relation->omit_drilldown,
			relation->relation_type_isa,
			relation->copy_common_attributes,
			relation->automatic_preselection,
			relation->drop_down_multi_select,
			relation->join_one2m_each_row,
			relation->ajax_fill_drop_down );

	} while ( list_next( relation_list ) );

	return strdup( display );
}

LIST *relation_translate_primary_list( LIST *relation_translate_list )
{
	LIST *list = list_new();
	RELATION_TRANSLATE *relation_translate;

	if ( list_rewind( relation_translate_list ) )
	do {
		relation_translate =
			list_get(
				relation_translate_list );

		list_set( list, relation_translate->primary_key );

	} while ( list_next( relation_translate_list ) );

	return list;
}

LIST *relation_translate_foreign_list( LIST *relation_translate_list )
{
	LIST *list = list_new();
	RELATION_TRANSLATE *relation_translate;

	if ( list_rewind( relation_translate_list ) )
	do {
		relation_translate =
			list_get(
				relation_translate_list );

		list_set( list, relation_translate->foreign_key );

	} while ( list_next( relation_translate_list ) );

	return list;
}

char *relation_translate_primary_key(
		LIST *relation_translate_list,
		char *foreign_key )
{
	RELATION_TRANSLATE *relation_translate;

	if ( !foreign_key )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"foreign_key is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( relation_translate_list ) )
	do {
		relation_translate =
			list_get(
				relation_translate_list );

		if ( strcmp(
			relation_translate->foreign_key,
			foreign_key ) == 0 )
		{
			return relation_translate->primary_key;
		}

	} while ( list_next( relation_translate_list ) );

	return NULL;
}

char *relation_translate_foreign_key(
		LIST *relation_translate_list,
		char *primary_key )
{
	RELATION_TRANSLATE *relation_translate;

	if ( !primary_key )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"primary_key is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( relation_translate_list ) )
	do {
		relation_translate =
			list_get(
				relation_translate_list );

		if ( strcmp(
			relation_translate->primary_key,
			primary_key ) == 0 )
		{
			return relation_translate->foreign_key;
		}

	} while ( list_next( relation_translate_list ) );

	return NULL;
}

char *relation_system_string(
		const char *relation_select,
		const char *relation_table,
		char *order_string )
{
	char system_string[ 65536 ];

	if ( !order_string ) order_string = "";

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh \"%s\" %s \"\" \"%s\"",
		relation_select,
		relation_table,
		order_string );

	return strdup( system_string );
}

