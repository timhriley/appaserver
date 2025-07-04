/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/rectification.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "piece.h"
#include "sql.h"
#include "appaserver_error.h"
#include "folder.h"
#include "role_folder.h"
#include "frameset.h"
#include "attribute.h"
#include "role.h"
#include "application.h"
#include "appaserver.h"
#include "folder_attribute.h"
#include "drop_column.h"
#include "create_table.h"
#include "drop_table.h"
#include "rectification.h"

LIST *rectification_column_list(
		LIST *rectification_index_list,
		char *table_name )
{
	LIST *list;
	char *system_string;
	FILE *input_pipe;
	char input[ 1024 ];

	if ( !list_length( rectification_index_list )
	||   !table_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	list = list_new();

	system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		rectification_column_system_string(
			table_name );

	input_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_pipe(
			system_string );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			list,
			rectification_column_parse(
				rectification_index_list,
				input ) );
	}

	pclose( input_pipe );

	return list;
}

RECTIFICATION_COLUMN *rectification_column_new( char *column_name )
{
	RECTIFICATION_COLUMN *rectification_column;

	if ( !column_name )
	{
		char message[ 128 ];

		sprintf(message, "column_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	rectification_column = rectification_column_calloc();
	rectification_column->column_name = column_name;

	return rectification_column;
}

RECTIFICATION_COLUMN *rectification_column_parse(
		LIST *rectification_index_list,
		char *input )
{
	RECTIFICATION_COLUMN *rectification_column;
	char column_name[ 128 ];
	char datatype_text[ 128 ];
	RECTIFICATION_INDEX *rectification_index;

	if ( !input || !*input ) return NULL;

	piece( column_name, SQL_DELIMITER, input, 0 );
	rectification_column = rectification_column_new(
		strdup( column_name ) );

	piece( datatype_text, SQL_DELIMITER, input, 1 );
	rectification_column->datatype_text = strdup( datatype_text );

	if ( ( rectification_index = 
			rectification_index_seek(
				rectification_index_list,
				column_name ) ) )
	{
		rectification_column->primary_key_index =
			rectification_index->
				primary_key_index;
	}

	rectification_column->width =
		rectification_column_width(
			datatype_text );

	rectification_column->float_decimal_places =
		rectification_column_float_decimal_places(
			datatype_text );

	rectification_column->datatype_name =
		rectification_column_datatype_name(
			datatype_text );	

	return rectification_column;
}

RECTIFICATION_COLUMN *rectification_column_calloc( void )
{
	RECTIFICATION_COLUMN *rectification_column;

	if ( ! ( rectification_column =
			calloc( 1,
				sizeof ( RECTIFICATION_COLUMN ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return rectification_column;
}

int rectification_column_width( char *datatype_text )
{
	char width_string[ 128 ];

	if ( !datatype_text )
	{
		char message[ 128 ];

		sprintf(message, "datatype_text is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	if ( !string_character_exists( datatype_text, '(' ) )
		return 0;

	piece( width_string, '(', datatype_text, 1 );

	return atoi( width_string );
}

int rectification_column_float_decimal_places( char *datatype_text )
{
	char decimal_string[ 128 ];

	if ( !datatype_text )
	{
		char message[ 128 ];

		sprintf(message, "datatype_text is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	if ( !string_character_exists( datatype_text, ',' ) )
		return 0;

	piece( decimal_string, ',', datatype_text, 1 );

	return atoi( decimal_string );
}

char *rectification_column_datatype_name( char *datatype_text )
{
	char datatype_name[ 128 ];

	if ( !datatype_text )
	{
		char message[ 128 ];

		sprintf(message, "datatype_text is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	piece( datatype_name, '(', datatype_text, 0 );

	return strdup( datatype_name );
}

RECTIFICATION_COLUMN *rectification_column_seek(
		char *column_name,
		LIST *rectification_column_list )
{
	RECTIFICATION_COLUMN *rectification_column;

	if ( !column_name )
	{
		char message[ 128 ];

		sprintf(message, "column_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( rectification_column_list ) )
	do {
		rectification_column =
			list_get(
				rectification_column_list );

		if ( strcmp(
			column_name,
			rectification_column->column_name ) == 0 )
		{
			return rectification_column;
		}

	} while ( list_next( rectification_column_list ) );

	return NULL;
}

LIST *rectification_table_list( void )
{
	LIST *list = list_new();
	LIST *table_name_list;

	table_name_list = application_table_name_list();

	if ( list_rewind( table_name_list ) )
	do {
		list_set(
			list,
			rectification_table_new(
				list_get( table_name_list ) ) );

	} while ( list_next( table_name_list ) );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

RECTIFICATION_TABLE *rectification_table_new( char *table_name )
{
	RECTIFICATION_TABLE *rectification_table;

	if ( !table_name )
	{
		char message[ 128 ];

		sprintf(message, "table_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	rectification_table = rectification_table_calloc();

	rectification_table->table_name = table_name;

	rectification_table->rectification_index_list =
		rectification_index_list(
			CREATE_TABLE_UNIQUE_SUFFIX,
			table_name );

	if ( !list_length( rectification_table->rectification_index_list ) )
	{
		char message[ 1024 ];

		sprintf(message,
"rectification_index_list(%s) returned empty.<br>\n"
"A likely cause is the table was renamed,<br>\n"
"but the unique index was not recreated.<br>\n"
"Execute: echo \"show index from %s;\" | sql",
			table_name,
			table_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	rectification_table->rectification_column_list =
		rectification_column_list(
			rectification_table->rectification_index_list,
			table_name );

	return rectification_table;
}

RECTIFICATION_TABLE *rectification_table_calloc( void )
{
	RECTIFICATION_TABLE *rectification_table;

	if ( ! ( rectification_table =
			calloc( 1,
				sizeof ( RECTIFICATION_TABLE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return rectification_table;
}

char *rectification_column_system_string( char *table_name )
{
	static char system_string[ 128 ];

	if ( !table_name )
	{
		char message[ 128 ];

		sprintf(message, "table_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(system_string,
		"echo \"describe %s;\" | sql.e",
		table_name );

	return system_string;
}

FILE *rectification_table_input_pipe( char *system_string )
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

LIST *rectification_attribute_list( LIST *folder_attribute_list )
{
	LIST *list = list_new();
	FOLDER_ATTRIBUTE *folder_attribute;

	if ( list_rewind( folder_attribute_list ) )
	do {
		folder_attribute = list_get( folder_attribute_list );

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

		list_set(
			list,
			rectification_attribute_new(
				folder_attribute->
					attribute_name,
				folder_attribute->
					primary_key_index,
				folder_attribute->
					attribute->
					datatype_name,
				folder_attribute->
					attribute->
					width,
				folder_attribute->
					attribute->
					float_decimal_places ) );

	} while ( list_next( folder_attribute_list ) );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

RECTIFICATION_ATTRIBUTE *rectification_attribute_new(
		char *attribute_name,
		int primary_key_index,
		char *datatype_name,
		int width,
		int float_decimal_places )
{
	RECTIFICATION_ATTRIBUTE *rectification_attribute;

	if ( !attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "attribute_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	rectification_attribute = rectification_attribute_calloc();

	rectification_attribute->attribute_name = attribute_name;
	rectification_attribute->primary_key_index = primary_key_index;
	rectification_attribute->datatype_name = datatype_name;
	rectification_attribute->width = width;
	rectification_attribute->float_decimal_places = float_decimal_places;

	return rectification_attribute;
}

RECTIFICATION_ATTRIBUTE *rectification_attribute_calloc( void )
{
	RECTIFICATION_ATTRIBUTE *rectification_attribute;

	if ( ! ( rectification_attribute =
			calloc( 1,
				sizeof ( RECTIFICATION_ATTRIBUTE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return rectification_attribute;
}

RECTIFICATION_ATTRIBUTE *rectification_attribute_seek(
		char *attribute_name,
		LIST *rectification_attribute_list )
{
	RECTIFICATION_ATTRIBUTE *rectification_attribute;

	if ( !attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "attribute_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( rectification_attribute_list ) )
	do {
		rectification_attribute =
			list_get(
				rectification_attribute_list );

		if ( strcmp(
			attribute_name,
			rectification_attribute->attribute_name ) == 0 )
		{
			return rectification_attribute;
		}

	} while ( list_next( rectification_attribute_list ) );

	return NULL;
}

LIST *rectification_folder_list(
		char *application_name,
		LIST *folder_list )
{
	LIST *list = list_new();
	FOLDER *folder;

	if ( list_rewind( folder_list ) )
	do {
		folder = list_get( folder_list );

		if ( list_length( folder->folder_attribute_list ) )
		{
			list_set(
				list,
				rectification_folder_new(
					application_name,
					folder->folder_name,
					folder->folder_attribute_list ) );
		}

	} while ( list_next( folder_list ) );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

RECTIFICATION_FOLDER *rectification_folder_new(
		char *application_name,
		char *folder_name,
		LIST *folder_attribute_list )
{
	RECTIFICATION_FOLDER *rectification_folder;

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

	if ( !list_length( folder_attribute_list ) ) return NULL;

	rectification_folder = rectification_folder_calloc();

	rectification_folder->folder_name = folder_name;

	rectification_folder->folder_table_name =
		strdup(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			folder_table_name(
				application_name,
				folder_name ) );

	rectification_folder->rectification_attribute_list =
		rectification_attribute_list(
			folder_attribute_list );

	return rectification_folder;
}

RECTIFICATION_FOLDER *rectification_folder_calloc( void )
{
	RECTIFICATION_FOLDER *rectification_folder;

	if ( ! ( rectification_folder =
			calloc( 1,
				sizeof ( RECTIFICATION_FOLDER ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return rectification_folder;
}

RECTIFICATION_MISMATCH *rectification_mismatch_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		LIST *rectification_attribute_list,
		LIST *rectification_column_list )
{
	RECTIFICATION_MISMATCH *rectification_mismatch;

	if ( !list_length( rectification_column_list ) ) return NULL;

	if ( !list_rewind( rectification_attribute_list ) ) return NULL;

	rectification_mismatch = rectification_mismatch_calloc();
	rectification_mismatch->folder_name = folder_name;

	do {
		rectification_mismatch->rectification_attribute =
			list_get(
				rectification_attribute_list );

		rectification_mismatch->rectification_column =
			rectification_column_seek(
				rectification_mismatch->
					rectification_attribute->
					attribute_name
						/* column_name */,
				rectification_column_list );

		if ( !rectification_mismatch->rectification_column )
		{
			rectification_mismatch->missing_column_name =
				rectification_mismatch->
					rectification_attribute->
					attribute_name;

			return rectification_mismatch;
		}

		rectification_mismatch->width_boolean =
			rectification_mismatch_width_boolean(
				rectification_mismatch->
					rectification_attribute->
					width,
				rectification_mismatch->
					rectification_column->
					width );

		rectification_mismatch->float_boolean =
			rectification_mismatch_float_boolean(
				rectification_mismatch->
					rectification_attribute->
					float_decimal_places,
				rectification_mismatch->
					rectification_column->
					float_decimal_places );

		if ( rectification_mismatch->width_boolean
		||   rectification_mismatch->float_boolean )
			return rectification_mismatch;

		rectification_mismatch->key_boolean =
			rectification_mismatch_key_boolean(
				rectification_mismatch->
					rectification_attribute->
					primary_key_index,
				rectification_mismatch->
					rectification_column->
					primary_key_index );

		if ( rectification_mismatch->key_boolean )
			return rectification_mismatch;

		rectification_mismatch->datatype_boolean =
			rectification_mismatch_datatype_boolean(
				rectification_mismatch->
					rectification_attribute->
					datatype_name,
				rectification_mismatch->
					rectification_attribute->
					width,
				rectification_mismatch->
					rectification_column->
					datatype_name );

		if ( rectification_mismatch->datatype_boolean )
			return rectification_mismatch;

	} while ( list_next( rectification_attribute_list ) );

	list_rewind( rectification_column_list );

	do {
		rectification_mismatch->rectification_column =
			list_get(
				rectification_column_list );

		rectification_mismatch->rectification_attribute =
			rectification_attribute_seek(
				rectification_mismatch->
					rectification_column->
					column_name
						/* attribute_name */,
				rectification_attribute_list );

		if ( !rectification_mismatch->rectification_attribute )
		{
			rectification_mismatch->missing_attribute_name =
				rectification_mismatch->
					rectification_column->
					column_name;

			rectification_mismatch->drop_column_href_string =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				drop_column_href_string(
					DROP_COLUMN_EXECUTABLE,
					application_name,
					session_key,
					login_name,
					role_name,
					folder_name,
					rectification_mismatch->
						missing_attribute_name );

			rectification_mismatch->drop_column_tag =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				rectification_mismatch_drop_column_tag(
					rectification_mismatch->
						drop_column_href_string,
					FRAMESET_TABLE_FRAME /* target_frame */,
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					rectification_mismatch_column_prompt(
						rectification_mismatch->
						    missing_attribute_name ) );

			return rectification_mismatch;
		}

	} while ( list_next( rectification_column_list ) );

	free( rectification_mismatch );
	return NULL;
}

RECTIFICATION_MISMATCH *rectification_mismatch_calloc( void )
{
	RECTIFICATION_MISMATCH *rectification_mismatch;

	if ( ! ( rectification_mismatch =
			calloc( 1,
				sizeof ( RECTIFICATION_MISMATCH ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return rectification_mismatch;
}

boolean rectification_mismatch_width_boolean(
		int attribute_width,
		int column_width )
{
	if ( !attribute_width || !column_width )
		return 0;

	if ( attribute_width == column_width )
		return 0;
	else
		return 1;
}

boolean rectification_mismatch_float_boolean(
		int attribute_float_decimal_places,
		int column_float_decimal_places )
{
	if ( attribute_float_decimal_places ==
	     column_float_decimal_places )
		return 0;
	else
		return 1;
}

boolean rectification_mismatch_key_boolean(
		int attribute_primary_key_index,
		int column_primary_key_index )
{
	return
	( attribute_primary_key_index !=
	  column_primary_key_index );
}

RECTIFICATION *rectification_fetch(
		char *application_name,
		char *session_key,
		char *login_name )
{
	RECTIFICATION *rectification;
	RECTIFICATION_FOLDER *rectification_folder;
	RECTIFICATION_TABLE *rectification_table;

	if ( !application_name
	||   !session_key
	||   !login_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	rectification = rectification_calloc();

	rectification->folder_list =
		folder_list(
			(LIST *)0 /* exclude_attribute_name_list */,
			1 /* fetch_folder_attribute_list */,
			1 /* fetch_attribute */ );

	if ( !list_length( rectification->folder_list ) )
	{
		char message[ 128 ];

		sprintf(message, "folder_list() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	rectification->rectification_folder_list =
		rectification_folder_list(
			application_name,
			rectification->folder_list );

	if ( !list_rewind( rectification->rectification_folder_list ) )
	{
		char message[ 128 ];

		sprintf(message,
			"rectification_folder_list() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	rectification->rectification_table_list =
		rectification_table_list();

	rectification->rectification_mismatch_list = list_new();

	do {
		rectification_folder =
			list_get(
				rectification->rectification_folder_list );

		rectification_table =
			rectification_table_seek(
				rectification_folder->folder_table_name,
				rectification->rectification_table_list );

		if ( !rectification_table )
		{
			list_set(
				rectification->rectification_mismatch_list,
				rectification_mismatch_no_table_new(
					rectification_folder->
						folder_name ) );

			continue;
		}

		list_set(
			rectification->rectification_mismatch_list,
			rectification_mismatch_new(
				application_name,
				session_key,
				login_name,
				ROLE_SYSTEM /* role_name */,
				rectification_folder->folder_name,
				rectification_folder->
					rectification_attribute_list,
				rectification_table->
					rectification_column_list ) );

	} while ( list_next( rectification->rectification_folder_list ) );

	list_rewind( rectification->rectification_table_list );

	do {
		rectification_table =
			list_get(
				rectification->
					rectification_table_list );

		rectification_folder =
			rectification_folder_seek(
				rectification_table->table_name,
			rectification->rectification_folder_list );

		if ( !rectification_folder )
		{
			list_set(
				rectification->rectification_mismatch_list,
				rectification_mismatch_no_folder_new(
					application_name,
					session_key,
					login_name,
					ROLE_SYSTEM /* role_name */,
					rectification_table->
						table_name ) );
		}

	} while ( list_next( rectification->rectification_table_list ) );

	rectification->not_in_role_table_name_list =
		rectification_not_in_role_table_name_list(
			FOLDER_TABLE,
			ROLE_FOLDER_TABLE,
			FOLDER_PRIMARY_KEY );

	return rectification;
}

RECTIFICATION *rectification_calloc( void )
{
	RECTIFICATION *rectification;

	if ( ! ( rectification = calloc( 1, sizeof ( RECTIFICATION ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return rectification;
}

RECTIFICATION_MISMATCH *rectification_mismatch_no_table_new(
		char *folder_name )
{
	RECTIFICATION_MISMATCH *rectification_mismatch;

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

	rectification_mismatch = rectification_mismatch_calloc();
	rectification_mismatch->missing_table = folder_name;

	return rectification_mismatch;
}

boolean rectification_mismatch_datatype_boolean(
		char *attribute_datatype_name,
		int attribute_width,
		char *column_datatype_name )
{
	if ( !column_datatype_name )
	{
		char message[ 128 ];

		sprintf(message, "column_datatype_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !attribute_datatype_name ) return 1;

	if ( attribute_is_date( attribute_datatype_name ) )
	{
		if ( strcmp( column_datatype_name, "date" ) == 0
		||   strcmp( column_datatype_name, "char" ) == 0 )
			return 0;
		else
			return 1;
	}

	if ( attribute_is_time( attribute_datatype_name ) )
	{
		if ( strcmp( column_datatype_name, "char" ) == 0 )
			return 0;
		else
			return 1;
	}

	if ( attribute_is_number( attribute_datatype_name ) )
	{
		if ( strcmp( column_datatype_name, "float" ) == 0
		||   strcmp( column_datatype_name, "double" ) == 0
		||   strcmp( column_datatype_name, "int" ) == 0 )
			return 0;
		else
			return 1;
	}

	if ( attribute_is_date_time( attribute_datatype_name ) )
	{
		if ( strcmp( column_datatype_name, "datetime" ) == 0
		||   strcmp( column_datatype_name, "char" ) == 0 )
			return 0;
		else
			return 1;
	}

	if ( attribute_is_character(
		ATTRIBUTE_DATATYPE_CHARACTER,
		attribute_datatype_name ) )
	{
		if ( strcmp( column_datatype_name, "char" ) == 0 )
		{
			return 0;
		}
		else
		if ( strcmp( column_datatype_name, "text" ) == 0
		&&   attribute_width > 255 )
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}

	if ( attribute_is_notepad(
		ATTRIBUTE_DATATYPE_NOTEPAD,
		attribute_datatype_name ) )
	{
		if ( strcmp( column_datatype_name, "char" ) == 0
		||   strcmp( column_datatype_name, "text" ) == 0
		||   strcmp( column_datatype_name, "longtext" ) == 0 )
			return 0;
		else
			return 1;
	}

	if ( attribute_is_password( attribute_datatype_name ) )
	{
		if ( strcmp( column_datatype_name, "char" ) == 0 )
			return 0;
		else
			return 1;
	}

	if ( attribute_is_upload( attribute_datatype_name ) )
	{
		if ( strcmp( column_datatype_name, "char" ) == 0 )
			return 0;
		else
			return 1;
	}

	if ( attribute_is_timestamp( attribute_datatype_name ) )
	{
		if ( strcmp( column_datatype_name, "timestamp" ) == 0 )
			return 0;
		else
			return 1;
	}

	if ( attribute_is_yes_no( attribute_datatype_name ) )
	{
		if ( strcmp( column_datatype_name, "char" ) == 0 )
			return 0;
		else
			return 1;
	}

	return 0;
}

RECTIFICATION_TABLE *rectification_table_seek(
		char *table_name,
		LIST *rectification_table_list )
{
	RECTIFICATION_TABLE *rectification_table;

	if ( !table_name )
	{
		char message[ 128 ];

		sprintf(message, "table_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( rectification_table_list ) )
	do {
		rectification_table =
			list_get(
				rectification_table_list );

		if ( strcmp(
			table_name,
			rectification_table->table_name ) == 0 )
		{
			return rectification_table;
		}

	} while ( list_next( rectification_table_list ) );

	return NULL;
}

RECTIFICATION_MISMATCH *rectification_mismatch_no_folder_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *table_name )
{
	RECTIFICATION_MISMATCH *rectification_mismatch;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !table_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	rectification_mismatch = rectification_mismatch_calloc();

	rectification_mismatch->missing_folder = table_name;

	rectification_mismatch->drop_table_href_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		drop_table_href_string(
			DROP_TABLE_EXECUTABLE,
			application_name,
			session_key,
			login_name,
			role_name,
			rectification_mismatch->missing_folder
				/* folder_name */ );

	rectification_mismatch->drop_table_tag =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		rectification_mismatch_drop_table_tag(
			rectification_mismatch->drop_table_href_string,
			FRAMESET_TABLE_FRAME /* target_frame */,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			rectification_mismatch_table_prompt(
				rectification_mismatch->missing_folder ) );

	return rectification_mismatch;
}

RECTIFICATION_FOLDER *rectification_folder_seek(
		char *folder_table_name,
		LIST *rectification_folder_list )
{
	RECTIFICATION_FOLDER *rectification_folder;

	if ( !folder_table_name )
	{
		char message[ 128 ];

		sprintf(message, "folder_table_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( rectification_folder_list ) )
	do {
		rectification_folder =
			list_get(
				rectification_folder_list );

		if ( strcmp(
			rectification_folder->folder_table_name,
			folder_table_name ) == 0 )
		{
			return rectification_folder;
		}

	} while ( list_next( rectification_folder_list ) );

	return NULL;
}

char *rectification_mismatch_column_prompt(
		char *missing_attribute_name )
{
	static char prompt[ 128 ];
	char destination[ 64 ];

	if ( !missing_attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "missing_attribute_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(prompt,
		"Drop: %s",
		string_initial_capital(
			destination,
			missing_attribute_name ) );

	return prompt;
}

char *rectification_mismatch_drop_column_tag(
		char *drop_column_href_string,
		char *target_frame,
		char *column_prompt )
{
	char tag[ 256 ];

	if ( !drop_column_href_string
	||   !target_frame
	||   !column_prompt )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(tag,
		"<a href=\"%s\" target=%s>%s</a>",
		drop_column_href_string,
		target_frame,
		column_prompt );

	return strdup( tag );
}

char *rectification_mismatch_table_prompt( char *missing_table )
{
	static char prompt[ 128 ];
	char destination[ 64 ];

	if ( !missing_table )
	{
		char message[ 128 ];

		sprintf(message, "missing_table is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(prompt,
		"Drop %s",
		string_initial_capital(
			destination,
			missing_table ) );

	return prompt;
}

char *rectification_mismatch_drop_table_tag(
		char *drop_table_href_string,
		char *target_frame,
		char *table_prompt )
{
	char table_tag[ 256 ];

	if ( !drop_table_href_string
	||   !target_frame
	||   !table_prompt )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(table_tag,
		"<a href=\"%s\" target=%s>%s</a>",
		drop_table_href_string,
		target_frame,
		table_prompt );

	return strdup( table_tag );
}

LIST *rectification_index_list(
		const char *create_table_unique_suffix,
		char *table_name )
{
	LIST *list;
	char *system_string;
	FILE *input_pipe;
	char input[ 1024 ];

	if ( !table_name )
	{
		char message[ 128 ];

		sprintf(message, "table_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	list = list_new();

	system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		rectification_index_system_string(
			table_name );

	input_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_pipe(
			system_string );

	while ( string_input( input, input_pipe, sizeof ( input ) ) )
	{
		list_set(
			list,
			rectification_index_parse(
				create_table_unique_suffix,
				table_name,
				input ) );
	}

	pclose( input_pipe );

	return list;
}

RECTIFICATION_INDEX *rectification_index_parse(
		const char *create_table_unique_suffix,
		char *table_name,
		char *input )
{
	RECTIFICATION_INDEX *rectification_index;
	char index_name[ 128 ];
	char column_name[ 128 ];
	char primary_string[ 128 ];

	if ( !input || !*input ) return NULL;

	piece( index_name, SQL_DELIMITER, input, 2 );

	if ( !*index_name )
	{
		fprintf(stderr,
		"Warning in %s/%s()/%d: piece( index_name, 2 ) failed.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		return NULL;
	}

	if ( strcmp(
		index_name,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		create_table_unique_index_name(
			(char *)create_table_unique_suffix,
			table_name ) ) == 0 )
	{
		goto rectification_index_continue;
	}

	if ( strcmp(
		index_name,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		create_table_unique_index_name(
			(char *)0 /* create_table_unique_suffix */,
			table_name ) ) != 0 )
	{
		return NULL;
	}

rectification_index_continue:

	piece( column_name, SQL_DELIMITER, input, 4 );
	if ( !*column_name )
	{
		fprintf(stderr,
		"Warning in %s/%s()/%d: piece( column_name, 4 ) failed.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		return NULL;
	}

	rectification_index =
		rectification_index_new(
			strdup( column_name ) );

	piece( primary_string, SQL_DELIMITER, input, 3 );
	if ( !*primary_string )
	{
		fprintf(stderr,
		"Warning in %s/%s()/%d: piece( primary_string, 3 ) failed.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		return NULL;
	}

	rectification_index->primary_key_index =
		rectification_index_primary_key_index(
			primary_string );

	return rectification_index;
}

RECTIFICATION_INDEX *rectification_index_new( char *column_name )
{
	RECTIFICATION_INDEX *rectification_index;

	if ( !column_name )
	{
		char message[ 128 ];

		sprintf(message, "column_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	rectification_index = rectification_index_calloc();
	rectification_index->column_name = column_name;

	return rectification_index;
}

RECTIFICATION_INDEX *rectification_index_calloc( void )
{
	RECTIFICATION_INDEX *rectification_index;

	if ( ! ( rectification_index =
			calloc( 1,
				sizeof ( RECTIFICATION_INDEX ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return rectification_index;
}

char *rectification_index_system_string( char *table_name )
{
	static char system_string[ 128 ];

	if ( !table_name )
	{
		char message[ 128 ];

		sprintf(message, "table_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(system_string,
		"echo \"show index from %s;\" | sql.e",
		table_name );

	return system_string;
}

RECTIFICATION_INDEX *rectification_index_seek(
		LIST *rectification_index_list,
		char *column_name )
{
	RECTIFICATION_INDEX *rectification_index;

	if ( !column_name )
	{
		char message[ 128 ];

		sprintf(message, "column_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( rectification_index_list ) )
	do {
		rectification_index =
			list_get(
				rectification_index_list );

		if ( strcmp(
			column_name,
			rectification_index->column_name ) == 0 )
		{
			return rectification_index;
		}

	} while ( list_next( rectification_index_list ) );

	return NULL;
}

int rectification_index_primary_key_index( char *primary_string )
{
	return
	string_atoi( primary_string );
}

LIST *rectification_not_in_role_table_name_list(
		const char *folder_table,
		const char *role_folder_table,
		const char *folder_primary_key )
{
	return
	list_pipe(
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		rectification_not_in_role_table_system_string(
			folder_table,
			role_folder_table,
			folder_primary_key ) );
}

char *rectification_not_in_role_table_system_string(
		const char *folder_table,
		const char *role_folder_table,
		const char *folder_primary_key )
{
	static char system_string[ 256 ];

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh %s %s \"%s\"",
		folder_primary_key,
		folder_table,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		rectification_not_in_role_table_where(
			role_folder_table,
			folder_primary_key ) );

	return system_string;
}

char *rectification_not_in_role_table_where(
		const char *role_folder_table,
		const char *folder_primary_key )
{
	static char where[ 128 ];

	snprintf(
		where,
		sizeof ( where ),
		"%s <> 'null' and %s not in ( select %s from %s )",
		folder_primary_key,
		folder_primary_key,
		folder_primary_key,
		role_folder_table );

	return where;
}

