/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/folder_attribute.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "sql.h"
#include "piece.h"
#include "attribute.h"
#include "relation_mto1.h"
#include "appaserver_error.h"
#include "appaserver.h"
#include "insert.h"
#include "role_folder.h"
#include "folder_attribute.h"

LIST *folder_attribute_list(
		char *role_name,
		char *folder_name,
		LIST *role_attribute_exclude_name_list,
		boolean fetch_attribute )
{
	LIST *list;

	if ( role_name )
	{
		list =
			folder_attribute_role_cache_list(
				role_name,
				folder_name,
				role_attribute_exclude_name_list,
				fetch_attribute );
	}
	else
	{
		list =
			folder_attribute_fetch_list(
				folder_name,
				role_attribute_exclude_name_list,
				fetch_attribute );
	}

	return list;
}

LIST *folder_attribute_role_cache_list(
		char *role_name,
		char *folder_name,
		LIST *role_attribute_exclude_name_list,
		boolean fetch_attribute )
{
	static LIST *role_cache_list = {0};
	LIST *list;

	if ( !role_name
	||   !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !role_cache_list )
	{
		role_cache_list =
			folder_attribute_role_list(
				role_name
					/* role_name_list_string */,
				fetch_attribute );
	}

	list =
		folder_attribute_seek_list(
			folder_name,
			role_attribute_exclude_name_list,
			role_cache_list
				/* folder_attribute_list */ );

	return list;
}


LIST *folder_attribute_role_list(
		char *role_name_list_string,
		boolean fetch_attribute )
{
	char *where_string;
	char *system_string;

	if ( !role_name_list_string )
	{
		char message[ 128 ];

		sprintf(message, "role_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	where_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		folder_attribute_role_where_string(
			FOLDER_PRIMARY_KEY,
			ROLE_FOLDER_TABLE,
			role_name_list_string );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		folder_attribute_system_string(
			FOLDER_ATTRIBUTE_SELECT,
			FOLDER_ATTRIBUTE_TABLE,
			where_string );

	return
	folder_attribute_system_list(
		system_string,
		fetch_attribute );
}

FOLDER_ATTRIBUTE *folder_attribute_calloc( void )
{
	FOLDER_ATTRIBUTE *folder_attribute;

	if ( ! ( folder_attribute = calloc( 1, sizeof ( FOLDER_ATTRIBUTE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return folder_attribute;
}

FOLDER_ATTRIBUTE *folder_attribute_new(
		char *folder_name,
		char *attribute_name )
{
	FOLDER_ATTRIBUTE *folder_attribute = folder_attribute_calloc();

	folder_attribute->folder_name = folder_name;
	folder_attribute->attribute_name = attribute_name;

	return folder_attribute;
}

char *folder_attribute_fetch_where_string(
		const char *folder_primary_key,
		char *folder_name,
		LIST *role_attribute_exclude_name_list )
{
	char where_string[ 512 ];

	if ( !folder_name )
	{
		strcpy( where_string, "1 = 1" );
	}
	else
	if ( list_length( role_attribute_exclude_name_list ) )
	{
		sprintf(where_string,
			"%s = '%s' and %s",
			folder_primary_key,
			folder_name,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			folder_attribute_exclude_where_string(
				ATTRIBUTE_PRIMARY_KEY,
				role_attribute_exclude_name_list ) );
	}
	else
	{
		sprintf(where_string,
			"%s = '%s'",
			folder_primary_key,
			folder_name );
	}

	return strdup( where_string );
}

LIST *folder_attribute_fetch_list(
		char *folder_name,
		LIST *role_attribute_exclude_name_list,
		boolean fetch_attribute )
{
	return
	folder_attribute_system_list(
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		folder_attribute_system_string(
			FOLDER_ATTRIBUTE_SELECT,
			FOLDER_ATTRIBUTE_TABLE,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			folder_attribute_fetch_where_string(
				FOLDER_PRIMARY_KEY,
				folder_name,
				role_attribute_exclude_name_list ) ),
		fetch_attribute );
}

char *folder_attribute_system_string(
		char *folder_attribute_select,
		char *folder_attribute_table,
		char *where_string )
{
	char system_string[ 1024 ];
	char *order_string;

	/* Ironically, this sorts by primary_key_index then display_order */
	/* -------------------------------------------------------------- */
	order_string =
		"table_name,"
		"ifnull(display_order,0),"
		"ifnull(primary_key_index,0)";

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\" \"%s\"",
		folder_attribute_select,
		folder_attribute_table,
		where_string,
		order_string );

	return strdup( system_string );
}

LIST *folder_attribute_system_list(
		char *system_string,
		boolean fetch_attribute )
{
	char input[ 1024 ];
	FILE *input_pipe;
	LIST *list;
	FOLDER_ATTRIBUTE *folder_attribute;

	if ( !system_string )
	{
		char message[ 128 ];

		sprintf(message, "system_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	input_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_pipe(
			system_string );

	list = list_new();

	while ( string_input( input, input_pipe, 1024 ) )
	{
		if ( ( folder_attribute =
			folder_attribute_parse(
				input,
				fetch_attribute ) ) )
		{
			list_set( list, folder_attribute );
		}
	}

	pclose( input_pipe );
	return list;
}

FOLDER_ATTRIBUTE *folder_attribute_parse(
		char *input,
		boolean fetch_attribute )
{
	char folder_name[ 128 ];
	char attribute_name[ 128 ];
	char buffer[ 1024 ];
	FOLDER_ATTRIBUTE *folder_attribute;

	if ( !input || !*input ) return (FOLDER_ATTRIBUTE *)0;

	/* See FOLDER_ATTRIBUTE_SELECT */
	/* --------------------------- */
	piece( folder_name, SQL_DELIMITER, input, 0 );
	piece( attribute_name, SQL_DELIMITER, input, 1 );

	folder_attribute =
		folder_attribute_new(
			strdup( folder_name ),
			strdup( attribute_name ) );

	piece( buffer, SQL_DELIMITER, input, 2 );
	folder_attribute->primary_key_index = atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	folder_attribute->display_order = atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	folder_attribute->omit_insert = ( *buffer == 'y' );

	piece( buffer, SQL_DELIMITER, input, 5 );
	folder_attribute->omit_insert_prompt = ( *buffer == 'y' );

	piece( buffer, SQL_DELIMITER, input, 6 );
	folder_attribute->omit_update = ( *buffer == 'y' );

	piece( buffer, SQL_DELIMITER, input, 7 );
	folder_attribute->additional_unique_index = ( *buffer == 'y' );

	piece( buffer, SQL_DELIMITER, input, 8 );
	folder_attribute->additional_index = ( *buffer == 'y' );

	piece( buffer, SQL_DELIMITER, input, 9 );
	folder_attribute->insert_required = ( *buffer == 'y' );

	piece( buffer, SQL_DELIMITER, input, 10 );
	folder_attribute->lookup_required = ( *buffer == 'y' );

	piece( buffer, SQL_DELIMITER, input, 11 );
	if ( *buffer ) folder_attribute->default_value = strdup( buffer );

	folder_attribute->prompt =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		folder_attribute_prompt(
			folder_attribute->attribute_name,
			folder_attribute->primary_key_index );

	if ( fetch_attribute )
	{
		if ( ! ( folder_attribute->attribute =
				attribute_fetch(
					attribute_name ) ) )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: attribute_fetch(%s) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				attribute_name );

			exit( 1 );
		}
	}

	return folder_attribute;
}

LIST *folder_attribute_primary_list(
		char *folder_name,
		LIST *folder_attribute_list )
{
	FOLDER_ATTRIBUTE *folder_attribute;
	LIST *primary_list;

	if ( !list_rewind( folder_attribute_list ) ) return (LIST *)0;

	primary_list = list_new();

	do {
		folder_attribute =
			list_get(
				folder_attribute_list );

		if ( folder_name
		&&   strcmp(
			folder_attribute->folder_name,
			folder_name ) != 0 )
		{
			continue;
		}

		if ( folder_attribute->primary_key_index )
		{
			list_set(
				primary_list, 
				folder_attribute );
		}

	} while ( list_next( folder_attribute_list ) );

	return primary_list;
}

LIST *folder_attribute_primary_key_list(
		char *folder_name,
		LIST *folder_attribute_list )
{
	FOLDER_ATTRIBUTE *folder_attribute;
	LIST *primary_key_list = list_new();

	if ( list_rewind( folder_attribute_list ) )
	do {
		folder_attribute =
			list_get(
				folder_attribute_list );

		if ( folder_name
		&&   strcmp( folder_name, folder_attribute->folder_name ) != 0 )
		{
			break;
		}

		if ( folder_attribute->primary_key_index )
		{
			list_set(
				primary_key_list, 
				folder_attribute->attribute_name );
		}

	} while ( list_next( folder_attribute_list ) );

	if ( !list_length( primary_key_list ) )
	{
		list_free( primary_key_list );
		primary_key_list = NULL;
	}

	return primary_key_list;
}

LIST *folder_attribute_non_primary_name_list(
		LIST *folder_attribute_list )
{
	FOLDER_ATTRIBUTE *folder_attribute;
	LIST *non_primary_name_list = list_new();

	if ( list_rewind( folder_attribute_list ) )
	do {
		folder_attribute =
			list_get(
				folder_attribute_list );

		if ( !folder_attribute->primary_key_index )
		{
			list_set(
				non_primary_name_list, 
				folder_attribute->attribute_name );
		}

	} while ( list_next( folder_attribute_list ) );

	if ( !list_length( non_primary_name_list ) )
	{
		list_free( non_primary_name_list );
		non_primary_name_list = NULL;
	}

	return non_primary_name_list;
}

LIST *folder_attribute_date_name_list(
		LIST *folder_attribute_list )
{
	FOLDER_ATTRIBUTE *folder_attribute;
	LIST *date_name_list = {0};

	if ( list_rewind( folder_attribute_list ) )
	do {
		folder_attribute =
			list_get(
				folder_attribute_list );

		if ( !folder_attribute->attribute )
		{
			char message[ 128 ];

			sprintf(message,
				"folder_attribute->attribute is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( attribute_is_date(
			folder_attribute->
				attribute->
				datatype_name )
		||   attribute_is_date_time(
			folder_attribute->
				attribute->
				datatype_name )
		||   attribute_is_current_date_time(
			folder_attribute->
				attribute->
				datatype_name ) )
		{
			if ( !date_name_list ) date_name_list = list_new();

			list_set(
				date_name_list, 
				folder_attribute->attribute_name );
		}

	} while ( list_next( folder_attribute_list ) );

	return date_name_list;
}

LIST *folder_attribute_time_name_list(
		LIST *folder_attribute_list )
{
	FOLDER_ATTRIBUTE *folder_attribute;
	LIST *time_name_list = {0};

	if ( list_rewind( folder_attribute_list ) )
	do {
		folder_attribute =
			list_get(
				folder_attribute_list );

		if ( !folder_attribute->attribute )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: attribute returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( attribute_is_time(
			folder_attribute->
				attribute->
				datatype_name ) )
		{
			if ( ! time_name_list ) time_name_list = list_new();

			list_set(
				time_name_list, 
				folder_attribute->attribute_name );
		}

	} while ( list_next( folder_attribute_list ) );

	return time_name_list;
}

LIST *folder_attribute_float_name_list(
		LIST *folder_attribute_list )
{
	FOLDER_ATTRIBUTE *folder_attribute;
	LIST *float_name_list = {0};

	if ( list_rewind( folder_attribute_list ) )
	do {
		folder_attribute =
			list_get(
				folder_attribute_list );

		if ( !folder_attribute->attribute )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: attribute returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( attribute_is_float(
			folder_attribute->
				attribute->
				datatype_name ) )
		{
			if ( !float_name_list )
				float_name_list =
					list_new();

			list_set(
				float_name_list, 
				folder_attribute->attribute_name );
		}

	} while ( list_next( folder_attribute_list ) );

	return float_name_list;
}

LIST *folder_attribute_integer_name_list(
		LIST *folder_attribute_list )
{
	FOLDER_ATTRIBUTE *folder_attribute;
	LIST *integer_name_list = {0};

	if ( list_rewind( folder_attribute_list ) )
	do {
		folder_attribute =
			list_get(
				folder_attribute_list );

		if ( !folder_attribute->attribute )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: attribute returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( attribute_is_integer(
			ATTRIBUTE_DATATYPE_INTEGER,
			folder_attribute->
				attribute->
				datatype_name ) )
		{
			if ( !integer_name_list )
				integer_name_list =
					list_new();

			list_set(
				integer_name_list, 
				folder_attribute->attribute_name );
		}

	} while ( list_next( folder_attribute_list ) );

	return integer_name_list;
}

LIST *folder_attribute_number_name_list(
		LIST *folder_attribute_list )
{
	FOLDER_ATTRIBUTE *folder_attribute;
	LIST *number_name_list = {0};

	if ( list_rewind( folder_attribute_list ) )
	do {
		folder_attribute =
			list_get(
				folder_attribute_list );

		if ( !folder_attribute->attribute )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: attribute returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( attribute_is_number(
			folder_attribute->
				attribute->
				datatype_name ) )
		{
			if ( !number_name_list )
				number_name_list =
					list_new();

			list_set(
				number_name_list, 
				folder_attribute->attribute_name );
		}

	} while ( list_next( folder_attribute_list ) );

	return number_name_list;
}

LIST *folder_attribute_append_isa_list(
		LIST *folder_attribute_list,
		LIST *relation_mto1_isa_list )
{
	RELATION_MTO1 *relation_mto1;
	LIST *append_isa_list;
	LIST *non_primary_list;

	if ( !folder_attribute_list )
	{
		char message[ 128 ];

		sprintf(message, "folder_attribute_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( relation_mto1_isa_list ) )
		return folder_attribute_list;

	append_isa_list = list_copy( folder_attribute_list );

	do {
		relation_mto1 =
			list_get(
				relation_mto1_isa_list );

		if ( !list_length(
			relation_mto1->
				one_folder->
				folder_attribute_list ) )
		{
			char message[ 128 ];

			sprintf(message,
				"one_folder->folder_attribute_list is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		non_primary_list =
			folder_attribute_non_primary_list(
				relation_mto1->
					one_folder->
					folder_attribute_list );

		list_set_list(
			append_isa_list,
			non_primary_list );

	} while( list_next( relation_mto1_isa_list ) );

	return append_isa_list;
}

char *folder_attribute_primary_where_string(
		const char *folder_primary_key,
		const char *attribute_primary_key,
		char *folder_name,
		char *attribute_name )
{
	static char where_string[ 128 ];

	if ( !folder_name
	||   !attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		where_string,
		sizeof ( where_string ),
		"%s = '%s' and %s = '%s'",
		folder_primary_key,
		folder_name,
		attribute_primary_key,
		attribute_name );

	return where_string;
}

boolean folder_attribute_exists(
		char *folder_name,
		char *attribute_name )
{
	if ( !folder_name
	||   !attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	folder_attribute_exists_boolean(
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		folder_attribute_exists_system_string(
			FOLDER_ATTRIBUTE_TABLE,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			folder_attribute_primary_where_string(
				FOLDER_PRIMARY_KEY,
				ATTRIBUTE_PRIMARY_KEY,
				folder_name,
				attribute_name ) ) );
}

char *folder_attribute_exists_system_string(
		const char *folder_attribute_table,
		char *folder_attribute_primary_where )
{
	static char system_string[ 256 ];

	if ( !folder_attribute_primary_where )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh 'count(1)' %s \"%s\"",
		folder_attribute_table,
		folder_attribute_primary_where );

	return system_string;
}

boolean folder_attribute_exists_boolean( char *system_string )
{
	return (boolean)atoi( string_fetch_pipe( system_string ) );
}

char *folder_attribute_sort_attribute_name(
		LIST *folder_attribute_list )
{
	if ( folder_attribute_seek(
			SORT_ORDER_ATTRIBUTE_NAME,
			folder_attribute_list ) )
	{
		return SORT_ORDER_ATTRIBUTE_NAME;
	}
	else
	if ( folder_attribute_seek(
			DISPLAY_ORDER_ATTRIBUTE_NAME,
			folder_attribute_list ) )
	{
		return DISPLAY_ORDER_ATTRIBUTE_NAME;
	}
	else
	if ( folder_attribute_seek(
			SEQUENCE_NUMBER_ATTRIBUTE_NAME,
			folder_attribute_list ) )
	{
		return SEQUENCE_NUMBER_ATTRIBUTE_NAME;
	}

	return (char *)0;
}

FOLDER_ATTRIBUTE *folder_attribute_seek(
		char *attribute_name,
		LIST *folder_attribute_list )
{
	FOLDER_ATTRIBUTE *folder_attribute;

	if ( list_rewind( folder_attribute_list ) )
	do {
		folder_attribute =
			list_get(
				folder_attribute_list );

		if ( string_strcmp(
			folder_attribute->attribute_name,
			attribute_name ) == 0 )
		{
			return folder_attribute;
		}

	} while ( list_next( folder_attribute_list ) );

	return NULL;
}

LIST *folder_attribute_seek_list(
		char *folder_name,
		LIST *role_attribute_exclude_name_list,
		LIST *folder_attribute_list )
{
	LIST *list = list_new();
	FOLDER_ATTRIBUTE *folder_attribute;
	boolean got_one = 0;

	if ( !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( folder_attribute_list ) )
	do {
		folder_attribute = list_get( folder_attribute_list );

		if ( list_string_exists(
			folder_attribute->attribute_name,
			role_attribute_exclude_name_list ) )
		{
			continue;
		}

		if ( strcmp(
			folder_attribute->folder_name,
			folder_name ) == 0 )
		{
			list_set( list, folder_attribute );
			got_one = 1;
		}
		else
		{
			if ( got_one ) break;
		}

	} while ( list_next( folder_attribute_list ) );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

LIST *folder_attribute_non_primary_list(
		LIST *folder_attribute_list )
{
	FOLDER_ATTRIBUTE *folder_attribute;
	LIST *return_list = list_new();

	if ( list_rewind( folder_attribute_list ) )
	do {
		folder_attribute =
			list_get(
				folder_attribute_list );

		if ( !folder_attribute->primary_key_index )
		{
			list_set( return_list, folder_attribute );
		}

	} while ( list_next( folder_attribute_list ) );

	if ( !list_length( return_list ) )
	{
		list_free( return_list );
		return_list = NULL;
	}

	return return_list;
}

LIST *folder_attribute_folder_prefixed_name_list(
		LIST *folder_attribute_list )
{
	FOLDER_ATTRIBUTE *folder_attribute;
	char prefixed_name[ 128 ];
	LIST *name_list = list_new();

	if ( list_rewind( folder_attribute_list ) )
	do {
		folder_attribute =
			list_get(
				folder_attribute_list );

		sprintf(prefixed_name,
			"%s.%s",
			folder_attribute->folder_name,
			folder_attribute->attribute_name );

		list_set(
			name_list,
			strdup( prefixed_name ) );

	} while ( list_next( folder_attribute_list ) );

	if ( !list_length( name_list ) )
	{
		list_free( name_list );
		name_list = NULL;
	}

	return name_list;
}

LIST *folder_attribute_append_isa_name_list(
		char *folder_name,
		LIST *folder_attribute_append_isa_list )
{
	FOLDER_ATTRIBUTE *folder_attribute;
	LIST *name_list = list_new();

	if ( list_rewind( folder_attribute_append_isa_list ) )
	do {
		folder_attribute =
			list_get(
				folder_attribute_append_isa_list );

		if ( string_strcmp(
			folder_name,
			folder_attribute->folder_name ) == 0 )
		{
			list_set(
				name_list,
				folder_attribute->attribute_name );
		}

	} while ( list_next( folder_attribute_append_isa_list ) );

	if ( !list_length( name_list ) )
	{
		list_free( name_list );
		name_list = NULL;
	}

	return name_list;
}

LIST *folder_attribute_name_list(
		char *folder_name,
		LIST *folder_attribute_append_isa_list )
{
	FOLDER_ATTRIBUTE *folder_attribute;
	LIST *name_list = list_new();

	if ( list_rewind( folder_attribute_append_isa_list ) )
	do {
		folder_attribute =
			list_get(
				folder_attribute_append_isa_list );

		if ( !folder_attribute->primary_key_index ) break;

		if ( !folder_name )
		{
			list_set(
				name_list,
				folder_attribute->attribute_name );
		}
		else
		if ( strcmp( folder_name, folder_attribute->folder_name ) == 0 )
		{
			list_set(
				name_list,
				folder_attribute->attribute_name );
		}

	} while ( list_next( folder_attribute_append_isa_list ) );

	if ( list_rewind( folder_attribute_append_isa_list ) )
	do {
		folder_attribute =
			list_get(
				folder_attribute_append_isa_list );

		if ( folder_attribute->primary_key_index ) continue;

		if ( !folder_name )
		{
			list_set(
				name_list,
				folder_attribute->attribute_name );
		}
		else
		if ( strcmp( folder_name, folder_attribute->folder_name ) == 0 )
		{
			list_set(
				name_list,
				folder_attribute->attribute_name );
		}

	} while ( list_next( folder_attribute_append_isa_list ) );

	if ( !list_length( name_list ) )
	{
		list_free( name_list );
		name_list = NULL;
	}

	return name_list;
}

LIST *folder_attribute_number_list( LIST *folder_attribute_list )
{
	FOLDER_ATTRIBUTE *folder_attribute;
	LIST *number_list = list_new();

	if ( list_rewind( folder_attribute_list ) )
	do {
		folder_attribute =
			list_get(
				folder_attribute_list );

		if ( !folder_attribute->attribute )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: attribute is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( attribute_is_number(
			folder_attribute->
				attribute->
				datatype_name ) )
		{
			list_set( number_list, folder_attribute );
		}

	} while ( list_next( folder_attribute_list ) );

	if ( !list_length( number_list ) )
	{
		list_free( number_list );
		number_list = NULL;
	}

	return number_list;
}

char *folder_attribute_list_display( LIST *folder_attribute_list )
{
	char display[ STRING_64K ];
	char *ptr = display;
	FOLDER_ATTRIBUTE *folder_attribute;

	*ptr = '\0';

	if ( list_rewind( folder_attribute_list ) )
	do {
		folder_attribute =
			list_get(
				folder_attribute_list );

		if ( ptr != display ) ptr += sprintf( ptr, "\n" );

		ptr += sprintf(
			ptr,
	"folder_name = %s; attribute_name = %s; primary_key_index = %d; ",
			folder_attribute->folder_name,
			folder_attribute->attribute_name,
			folder_attribute->primary_key_index );

		if ( !folder_attribute->attribute )
		{
			ptr += sprintf(
				ptr,
				"folder_attribute->attribute is empty." );
		}
		else
		{
			ptr += sprintf(
				ptr,
				"attribute_datatype = %s; "
				"width = %d; "
				"default_value = '%s'.\n",
				folder_attribute->attribute->datatype_name,
				folder_attribute->attribute->width,
				(folder_attribute->default_value)
					? folder_attribute->default_value
					: "" );
		}

	} while ( list_next( folder_attribute_list ) );

	return strdup( display );
}

char *folder_attribute_prompt(
		char *attribute_name,
		int primary_key_index )
{
	char prompt[ 128 ];
	char *ptr = prompt;

	if ( primary_key_index ) ptr += sprintf( ptr, "*" );

	string_initial_capital( ptr, attribute_name );

	return strdup( prompt );
}

int folder_attribute_calendar_date_name_list_length(
		LIST *folder_attribute_list )
{
	FOLDER_ATTRIBUTE *folder_attribute;
	int length = 0;

	if ( list_rewind( folder_attribute_list ) )
	do {
		folder_attribute =
			list_get(
				folder_attribute_list );

		if ( !folder_attribute->attribute )
		{
			char message[ 128 ];

			sprintf(message,
				"folder_attribute->attribute is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( attribute_is_date(
			folder_attribute->
				attribute->
				datatype_name )
		||   attribute_is_date_time(
			folder_attribute->
				attribute->
				datatype_name )
		||   attribute_is_current_date_time(
			folder_attribute->
				attribute->
				datatype_name ) )
		{
			length++;
		}

	} while ( list_next( folder_attribute_list ) );

	return length;
}

LIST *folder_attribute_fetch_primary_key_list(
		char *folder_name )
{
	LIST *list;

	list =
		folder_attribute_fetch_list(
			folder_name,
			(LIST *)0 /* role_attribute_exclude_name_list */,
			0 /* not fetch_attribute */ );

	return
	folder_attribute_primary_key_list(
		folder_name,
		list );
}

LIST *folder_attribute_name_list_attribute_list(
		LIST *folder_name_list )
{
	LIST *list = list_new();

	if ( list_rewind( folder_name_list ) )
	do {
		list_set_list(
			list,
			folder_attribute_fetch_list(
				list_get( folder_name_list ),
				(LIST *)0 /* exclude_attribute_name_list */,
				1 /* fetch_attribute */ ) );
			
	} while ( list_next( folder_name_list ) );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

FOLDER_ATTRIBUTE *folder_attribute_fetch(
		char *folder_name,
		char *attribute_name,
		boolean fetch_attribute )
{
	char *input;

	if ( !folder_name
	||   !attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	input =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_pipe_input(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			folder_attribute_system_string(
				FOLDER_ATTRIBUTE_SELECT,
				FOLDER_ATTRIBUTE_TABLE,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				folder_attribute_primary_where_string(
					FOLDER_PRIMARY_KEY,
					ATTRIBUTE_PRIMARY_KEY,
					folder_name,
					attribute_name ) ) );

	if ( !input )
	{
		char message[ 128 ];

		sprintf(message,
			"string_pipe_input(%s,%s) returned empty.",
			folder_name,
			attribute_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	folder_attribute_parse(
		input,
		fetch_attribute );
}

FILE *folder_attribute_input_pipe( char *system_string )
{
	if ( !system_string )
	{
		char message[ 128 ];

		sprintf(message, "system_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return popen( system_string, "r" );
}

char *folder_attribute_insert_statement(
		char *folder_attribute_select,
		char *folder_attribute_table,
		FOLDER_ATTRIBUTE *folder_attribute )
{
	char *primary_string;
	char *display_order_string;
	char *omit_insert_string;
	char *omit_insert_prompt_string;
	char *omit_update_string;
	char *additional_unique_index_string;
	char *additional_index_string;
	char *insert_required_string;
	char *lookup_required_string;
	char *default_value_string;
	char *statement_string;

	if ( !folder_attribute_table
	||   !folder_attribute_select
	||   !folder_attribute
	||   !folder_attribute->folder_name
	||   !folder_attribute->attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	primary_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		attribute_insert_value_string(
			folder_attribute->primary_key_index /* integer */,
			(char *)0 /* string */ );

	display_order_string =
		attribute_insert_value_string(
			folder_attribute->display_order /* integer */,
			(char *)0 /* string */ );

	omit_insert_string =
		attribute_insert_value_string(
			0 /* integer */,
			(folder_attribute->omit_insert)
				? "y" : "" /* string */ );

	omit_insert_prompt_string =
			attribute_insert_value_string(
			0 /* integer */,
			(folder_attribute->omit_insert_prompt)
				? "y" : ""  /* string */ );

	omit_update_string =
		attribute_insert_value_string(
			0 /* integer */,
			(folder_attribute->omit_update)
				? "y" : "" /* string */ );

	additional_unique_index_string =
		attribute_insert_value_string(
			0 /* integer */,
			(folder_attribute->additional_unique_index)
				? "y" : "" /* string */ );

	additional_index_string =
		attribute_insert_value_string(
			0 /* integer */,
			(folder_attribute->additional_index)
				? "y" : "" /* string */ );

	insert_required_string =
		attribute_insert_value_string(
			0 /* integer */,
			(folder_attribute->insert_required)
				? "y" : "" /* string */ );

	lookup_required_string =
		attribute_insert_value_string(
			0 /* integer */,
			(folder_attribute->lookup_required)
				? "y" : "" /* string */ );

	default_value_string =
		attribute_insert_value_string(
			0 /* integer */,
			folder_attribute->default_value );

	statement_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		folder_attribute_insert_statement_string(
			folder_attribute_select,
			folder_attribute_table,
			folder_attribute->folder_name,
			folder_attribute->attribute_name,
			primary_string,
			display_order_string,
			omit_insert_string,
			omit_insert_prompt_string,
			omit_update_string,
			additional_unique_index_string,
			additional_index_string,
			insert_required_string,
			lookup_required_string,
			default_value_string );

	free( primary_string );
	free( display_order_string );
	free( omit_insert_string );
	free( omit_insert_prompt_string );
	free( omit_update_string );
	free( additional_unique_index_string );
	free( additional_index_string );
	free( insert_required_string );
	free( lookup_required_string );
	free( default_value_string );

	return statement_string;
}

char *folder_attribute_insert_statement_string(
		char *folder_attribute_select,
		char *folder_attribute_table,
		char *folder_name,
		char *attribute_name,
		char *primary_string,
		char *display_order_string,
		char *omit_insert_string,
		char *omit_insert_prompt_string,
		char *omit_update_string,
		char *additional_unique_index_string,
		char *additional_index_string,
		char *insert_required_string,
		char *lookup_required_string,
		char *default_value_string )
{
	char statement_string[ 4096 ];

	sprintf(statement_string,
"insert into %s (%s) values ('%s','%s',%s,%s,%s,%s,%s,%s,%s,%s,%s,%s);",
		folder_attribute_table,
		folder_attribute_select,
		folder_name,
		attribute_name,
		primary_string,
		display_order_string,
		omit_insert_string,
		omit_insert_prompt_string,
		omit_update_string,
		additional_unique_index_string,
		additional_index_string,
		insert_required_string,
		lookup_required_string,
		default_value_string );

	return strdup( statement_string );
}

char *folder_attribute_where_string(
		LIST *folder_attribute_list,
		LIST *primary_key_list,
		LIST *primary_data_list )
{
	static char where_string[ 1024 ];
	char *ptr = where_string;
	char *primary_key;
	char *primary_data;
	FOLDER_ATTRIBUTE *folder_attribute;
	char *datatype_name = {0};

	if ( !list_length( primary_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "primary_key_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_length( primary_key_list ) !=
	     list_length( primary_data_list ) )
	{
		char message[ 1024 ];

		sprintf(message,
			"cannot align: (%s) <> (%s)",
			list_display_delimited(
				primary_key_list,
				',' ),
			list_display_delimited(
				primary_data_list,
				',' ) );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	list_rewind( primary_data_list );
	list_rewind( primary_key_list );

	do {
		primary_key = list_get( primary_key_list );

		if ( ptr != where_string ) ptr += sprintf( ptr, " and " );

		if ( ( folder_attribute =
				folder_attribute_seek(
					primary_key,
					folder_attribute_list ) ) )
		{
			if ( !folder_attribute->attribute )
			{
				char message[ 128 ];

				snprintf(
					message,
					sizeof ( message ),
				"folder_attribute->attribute is empty." );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			datatype_name =
				folder_attribute->
					attribute->
					datatype_name;
		}

		primary_data = list_get( primary_data_list );

		if ( attribute_is_number(
			datatype_name ) )
		{
			ptr += sprintf(
				ptr,
				"%s = %s",
				primary_key,
				primary_data );
		}
		else
		{
			ptr += sprintf(
				ptr,
				"%s = '%s'",
				primary_key,
				primary_data );
		}

		list_next( primary_data_list );

	} while ( list_next( primary_key_list ) );

	return where_string;
}

char *folder_attribute_list_string( char *folder_name )
{
	LIST *name_list;

	if ( !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	name_list =
		folder_attribute_name_list(
			(char *)0 /* folder_name */,
			folder_attribute_fetch_list(
				folder_name,
				(LIST *)0 /* exclude_attribute_name_list */,
				0 /* not fetch_attribute */ ) );

	if ( !list_length( name_list ) )
	{
		char message[ 128 ];

		sprintf(message,
			"folder_attribute_fetch_list(%s) returned empty.",
			folder_name );

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
	list_delimited( name_list, ',' );
}

LIST *folder_attribute_lookup_required_name_list(
		LIST *folder_attribute_list )
{
	FOLDER_ATTRIBUTE *folder_attribute;
	LIST *name_list = {0};

	if ( list_rewind( folder_attribute_list ) )
	do {
		folder_attribute =
			list_get(
				folder_attribute_list );

		if ( folder_attribute->lookup_required )
		{
			if ( !name_list ) name_list = list_new();

			list_set(
				name_list, 
				folder_attribute->attribute_name );
		}

	} while ( list_next( folder_attribute_list ) );

	return name_list;
}

LIST *folder_attribute_insert_required_name_list(
		LIST *folder_attribute_list )
{
	FOLDER_ATTRIBUTE *folder_attribute;
	LIST *name_list = {0};

	if ( list_rewind( folder_attribute_list ) )
	do {
		folder_attribute =
			list_get(
				folder_attribute_list );

		if ( folder_attribute->insert_required )
		{
			if ( !name_list ) name_list = list_new();

			list_set(
				name_list, 
				folder_attribute->attribute_name );
		}

	} while ( list_next( folder_attribute_list ) );

	return name_list;
}

char *folder_attribute_exclude_where_string(
		const char *attribute_primary_key,
		LIST *role_attribute_exclude_name_list )
{
	char *in_clause;
	static char where_string[ 256 ];

	if ( !list_length( role_attribute_exclude_name_list ) )
	{
		return "1 = 1";
	}

	in_clause =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		string_in_clause(
			role_attribute_exclude_name_list );

	if ( strlen( in_clause ) + 19 >= 256 )
	{
		char message[ 128 ];

		sprintf(message,
			STRING_OVERFLOW_TEMPLATE,
			256 );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(where_string,
		"%s not in (%s)",
		attribute_primary_key,
		in_clause );

	free( in_clause );

	return where_string;
}

char *folder_attribute_role_where_string(
		const char *folder_primary_key,
		const char *role_folder_table,
		char *role_name_list_string )
{
	static char where_string[ 256 ];
	LIST *list;
	char *in_clause;
	char role_where_string[ 128 ];

	if ( !role_name_list_string )
	{
		char message[ 128 ];

		sprintf(message, "role_name_list_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	list = string_list( role_name_list_string );
	in_clause = string_in_clause( list );

	sprintf(role_where_string,
		"role in (%s)",
		in_clause );

	sprintf(where_string,
		"%s in "
		"( select distinct %s "
		"from %s "
		"where %s )",
		folder_primary_key,
		folder_primary_key,
		role_folder_table,
		role_where_string );

	return where_string;
}

LIST *folder_attribute_upload_filename_list(
		LIST *folder_attribute_list )
{
	FOLDER_ATTRIBUTE *folder_attribute;
	LIST *upload_filename_list = {0};

	if ( list_rewind( folder_attribute_list ) )
	do {
		folder_attribute =
			list_get(
				folder_attribute_list );

		if ( !folder_attribute->attribute )
		{
			char message[ 128 ];

			sprintf(message,
				"folder_attribute->attribute is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( attribute_is_upload(
			folder_attribute->
				attribute->
				datatype_name ) )
		{
			if ( !upload_filename_list )
				upload_filename_list =
					list_new();

			list_set(
				upload_filename_list, 
				folder_attribute->attribute_name );
		}

	} while ( list_next( folder_attribute_list ) );

	return upload_filename_list;
}

LIST *folder_attribute_viewonly_attribute_name_list(
		LIST *folder_attribute_append_isa_list )
{
	LIST *viewonly_attribute_name_list = {0};
	FOLDER_ATTRIBUTE *folder_attribute;

	if ( list_rewind( folder_attribute_append_isa_list ) )
	do {
		folder_attribute =
			list_get(
				folder_attribute_append_isa_list );

		if ( folder_attribute->omit_update )
		{
			if ( !viewonly_attribute_name_list )
				viewonly_attribute_name_list =
					list_new();

			list_set(
				viewonly_attribute_name_list, 
				folder_attribute->attribute_name );
		}

	} while ( list_next( folder_attribute_append_isa_list ) );

	return viewonly_attribute_name_list;
}

