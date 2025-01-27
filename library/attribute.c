/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/attribute.c					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "sql.h"
#include "piece.h"
#include "environ.h"
#include "folder.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "query.h"
#include "attribute.h"

ATTRIBUTE *attribute_calloc( void )
{
	ATTRIBUTE *attribute;

	if ( ! ( attribute = calloc( 1, sizeof ( ATTRIBUTE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return attribute;
}

ATTRIBUTE *attribute_new( char *attribute_name )
{
	ATTRIBUTE *attribute;

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

	attribute = attribute_calloc();
	attribute->attribute_name = attribute_name;

	return attribute;
}

ATTRIBUTE *attribute_seek(
		char *attribute_name,
		LIST *attribute_list )
{
	ATTRIBUTE *attribute;

	if ( list_rewind( attribute_list ) )
	do {
		attribute = list_get( attribute_list );

		if ( string_strcmp(
				attribute->attribute_name,
				attribute_name ) == 0 )
		{
			return attribute;
		}

	} while( list_next( attribute_list ) );

	return NULL;
}

boolean attribute_exists(
		char *attribute_name,
		LIST *attribute_list )
{
	if ( attribute_seek( attribute_name, attribute_list ) )
		return 1;
	else
		return 0;
}

char *attribute_system_string(
		char *attribute_select,
		char *attribute_table,
		char *where )
{
	char system_string[ 1024 ];

	if ( !where ) where = "1 = 1";

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		attribute_select,
		attribute_table	,
		where );

	return strdup( system_string );
}

LIST *attribute_system_list( char *system_string )
{
	char input[ 2048 ];
	FILE *pipe;
	LIST *list;
	ATTRIBUTE *attribute;

	pipe = popen( system_string, "r"  );

	list = list_new();

	while ( string_input( input, pipe, 2048 ) )
	{
		if ( ( attribute = attribute_parse( input ) ) )
		{
			list_set( list, attribute );
		}
	}

	pclose( pipe );

	return list;
}

ATTRIBUTE *attribute_parse( char *input )
{
	ATTRIBUTE *attribute;
	char buffer[ 4096 ];

	if ( !input || !*input ) return NULL;

	/* See ATTRIBUTE_SELECT */
	/* -------------------- */
	piece( buffer, SQL_DELIMITER, input, 0 );
	attribute = attribute_new( strdup( buffer ) );

	piece( buffer, SQL_DELIMITER, input, 1 );
	if ( *buffer ) attribute->datatype_name = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	if ( *buffer ) attribute->width = atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer ) attribute->float_decimal_places = atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	if ( *buffer ) attribute->hint_message = strdup( buffer );

	return attribute;
}

boolean attribute_is_date( char *datatype_name )
{
	if (  string_strcmp( datatype_name, "date" ) == 0
	||    string_strcmp( datatype_name, "current_date" ) == 0 )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

boolean attribute_is_time( char *datatype_name )
{
	if (  string_strcmp( datatype_name, "time" ) == 0
	||    string_strcmp( datatype_name, "current_time" ) == 0 )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

boolean attribute_is_integer( char *datatype_name )
{
	if ( !datatype_name ) return 0;

	if (  strcmp( datatype_name, "integer" ) == 0 )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

boolean attribute_is_number( char *datatype_name )
{
	if ( !datatype_name ) return 0;

	if (  strcmp( datatype_name, "float" ) == 0
	||    strcmp( datatype_name, "integer" ) == 0
	||    strcmp( datatype_name, "reference_number" ) == 0 )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

boolean attribute_is_float( char *datatype_name )
{
	if (  string_strcmp( datatype_name, "float" ) == 0 )
		return 1;
	else
		return 0;
}

boolean attribute_is_current_date( char *datatype_name )
{
	if (  string_strcmp( datatype_name, "current_date" ) == 0 )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

boolean attribute_is_current_date_time( char *datatype_name )
{
	if (  string_strcmp( datatype_name, "current_date_time" ) == 0 )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

boolean attribute_is_date_time( char *datatype_name )
{
	if (  string_strcmp( datatype_name, "date_time" ) == 0 )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

boolean attribute_is_text( char *datatype_name )
{
	if ( string_strcmp( datatype_name, "text" ) == 0 )
		return 1;
	else
		return 0;
}

boolean attribute_is_notepad( char *datatype_name )
{
	if ( string_strcmp( datatype_name, "notepad" ) == 0 )
		return 1;
	else
		return 0;
}

boolean attribute_is_password( char *datatype_name )
{
	if ( string_strcmp( datatype_name, "password" ) == 0 )
		return 1;
	else
		return 0;
}

boolean attribute_is_encrypt( char *datatype_name )
{
	if ( string_strcmp( datatype_name, "encrypt" ) == 0 )
		return 1;
	else
		return 0;
}

boolean attribute_is_upload( char *datatype_name )
{
	if ( string_strcmp( datatype_name, "upload_file" ) == 0 )
		return 1;
	else
		return 0;
}

boolean attribute_is_timestamp( char *datatype_name )
{
	if ( string_strcmp( datatype_name, "timestamp" ) == 0 )
		return 1;
	else
		return 0;
}

LIST *attribute_list( void )
{
	static LIST *list = {0};

	if ( list ) return list;

	list =
		attribute_system_list(
			attribute_system_string(
				ATTRIBUTE_SELECT,
				ATTRIBUTE_TABLE,
				(char *)0 /* where */ ) );

	if ( !list_length( list ) )
	{
		char message[ 1024 ];

		sprintf(message,
			"attribute_system_list(%s) returned empty.",
			ATTRIBUTE_SELECT );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return list;
}

ATTRIBUTE *attribute_fetch( char *attribute_name )
{
	static LIST *list = {0};

	if ( list ) return attribute_seek( attribute_name, list );

	list = attribute_list();
	return attribute_seek( attribute_name, list );
}

char *attribute_full_attribute_name(
		char *folder_table_name,
		char *attribute_name )
{
	if ( !attribute_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: attribute_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( folder_table_name && *folder_table_name )
	{
		char full_attribute_name[ 128 ];

		sprintf(full_attribute_name,
			"%s.%s",
			folder_table_name,
			attribute_name );

		return strdup( full_attribute_name );
	}
	else
	{
		return attribute_name;
	}
}

boolean attribute_is_yes_no( char *attribute_name )
{
	int str_len;

	if ( !attribute_name ) return 0;

	str_len = strlen( attribute_name );

	if ( str_len > 3
	&&   strncmp(	attribute_name + str_len - 3,
			"_yn",
			3 ) == 0 )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int attribute_width( char *attribute_name )
{
	ATTRIBUTE *attribute = attribute_fetch( attribute_name );

	if ( !attribute )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: attribute_fetch(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			attribute_name );
		exit( 1 );
	}

	return attribute->width;
}

char *attribute_name_trim_datatype(
		char *attribute_name,
		boolean attribute_is_date,
		boolean attribute_is_time )
{
	char attribute_trimmed[ 128 ];

	if ( !attribute_name
	||   strlen( attribute_name ) <= 5 )
	{
		return (char *)0;
	}

	*attribute_trimmed = '\0';

	if ( attribute_is_date )
	{
		if ( strcmp(
			/* -------------------------- */
			/* Returns someplace in input */
			/* -------------------------- */
			string_right(
				attribute_name,
				5 ),
			"_date" ) == 0 )
		{
			strcpy( attribute_trimmed, attribute_name );

			/* --------------------------------- */
			/* Returns buffer with end shortened */
			/* --------------------------------- */
			string_trim_right( attribute_trimmed, 5 );
		}
	}
	else
	if ( attribute_is_time )
	{
		if ( strcmp(
			/* -------------------------- */
			/* Returns someplace in input */
			/* -------------------------- */
			string_right(
				attribute_name,
				5 ),
			"_time" ) == 0 )
		{
			strcpy( attribute_trimmed, attribute_name );

			/* --------------------------------- */
			/* Returns buffer with end shortened */
			/* --------------------------------- */
			string_trim_right( attribute_trimmed, 5 );
		}
	}

	if ( *attribute_trimmed )
		return strdup( attribute_trimmed );
	else
		return (char *)0;
}

char *attribute_database_datatype(
		char *datatype_name,
		int width,
		int float_decimal_places,
		int primary_key_index,
		char *default_value )
{
	char buffer[ 1024 ];

	if ( !datatype_name || !*datatype_name )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty datatype.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( strcmp( datatype_name, "timestamp" ) == 0 )
	{
		strcpy( buffer, "timestamp" );
	}
	else
	if ( strcmp( datatype_name, "password" ) == 0 )
	{
		sprintf(buffer,
			"char (%d)",
			width );
	}
	else
	if ( strcmp( datatype_name, "text" ) == 0
	||   strcmp( datatype_name, "hidden_text" ) == 0
	||   strcmp( datatype_name, "notepad" ) == 0
	||   strcmp( datatype_name, "http_filename" ) == 0 )
	{
		if ( width < 256)
		{
			sprintf(buffer,
			 	"char (%d)",
			 	width );
		}
		else
		if ( width > 255 && width < 65536 )
		{
			strcpy( buffer, "text" );
		}
		else
		{
			strcpy( buffer, "longtext" );
		}
	}
	else
	if ( strcmp( datatype_name, "integer" ) == 0 )
	{
		strcpy( buffer, "integer" );
	}
	else
	if ( strcmp( datatype_name, "date" ) == 0
	||   strcmp( datatype_name, "current_date" ) == 0 )
	{
		strcpy( buffer, "date" );
	}
	else
	if ( strcmp( datatype_name, "float" ) == 0 )
	{
		sprintf(buffer,
		 	"double (%d,%d)",
		 	width,
		 	float_decimal_places );
	}
	else
	if ( strcmp( datatype_name, "reference_number" ) == 0 )
	{
		strcpy( buffer, "integer" );
	}
	else
	if ( strcmp( datatype_name, "time" ) == 0
	||   strcmp( datatype_name, "current_time" ) == 0 )
	{
		sprintf(buffer,
			"char (%d)",
			width );
	}
	else
	if ( strcmp( datatype_name, "date_time" ) == 0
	||   strcmp( datatype_name, "current_date_time" ) == 0 )
	{
		strcpy( buffer, "datetime" );
	}
	else
	{
		sprintf( buffer, "char (%d)", width );
	}

	if ( primary_key_index )
	{
		strcat( buffer, " not null" );
	}

	if ( default_value )
	{
		sprintf(buffer + strlen( buffer ),
			" default '%s'",
			default_value );
	}

	return strdup( buffer );
}

char *attribute_insert_statement(
		char *attribute_select,
		char *attribute_table,
		ATTRIBUTE *attribute )
{
	char *width_string;
	char *float_string;
	char *hint_string;
	char *statement_string;

	if ( !attribute_table
	||   !attribute_select
	||   !attribute
	||   !attribute->attribute_name
	||   !attribute->datatype_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	width_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		attribute_insert_value_string(
			attribute->width /* integer */,
			(char *)0 /* string */ );

	float_string =
		attribute_insert_value_string(
			attribute->float_decimal_places /* integer */,
			(char *)0 /* string */ );

	hint_string =
		attribute_insert_value_string(
			0 /* integer */,
			attribute->hint_message /* string */ );

	statement_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		attribute_insert_statement_string(
			attribute_select,
			attribute_table,
			attribute->attribute_name,
			attribute->datatype_name,
			width_string,
			float_string,
			hint_string );

	free( width_string );
	free( float_string );
	free( hint_string );

	return statement_string;
}

char *attribute_insert_statement_string(
		char *attribute_select,
		char *attribute_table,
		char *attribute_name,
		char *datatype_name,
		char *width_string,
		char *float_string,
		char *hint_string )
{
	char statement_string[ 4096 ];

	if ( !attribute_select
	||   !attribute_table
	||   !attribute_name
	||   !datatype_name
	||   !width_string
	||   !float_string
	||   !hint_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	sprintf(statement_string,
"insert into %s (%s) values ('%s','%s',%s,%s,%s);",
		attribute_table,
		attribute_select,
		attribute_name,
		datatype_name,
		width_string,
		float_string,
		hint_string );

	return strdup( statement_string );
}

char *attribute_delete_statement(
		const char *attribute_table,
		char *attribute_name )
{
	static char statement[ 128 ];

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

	sprintf(statement,
		"delete from %s where column_name = '%s';",
		attribute_table,
		attribute_name );

	return statement;
}

char *attribute_trim_number_characters( char *number )
{
	if ( !number ) return (char *)0;

	return
	/* ------------- */
	/* Returns datum */
	/* ------------- */
	string_trim_character_array(
		number /* datum */,
		",$" );
}

LIST *attribute_data_list(
		char delimiter,
		char *data_list_string )
{
	if ( !delimiter
	||   !data_list_string )
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
	list_string_list(
		data_list_string,
		delimiter );
}

DICTIONARY *attribute_query_dictionary(
		const char *query_relation_operator_prefix,
		const char *query_equal,
		LIST *attribute_name_list,
		LIST *attribute_data_list )
{
	DICTIONARY *query_dictionary = dictionary_small();
	char *attribute_name;

	if ( list_length( attribute_name_list ) !=
	     list_length( attribute_data_list ) )
	{
		char message[ 128 ];

		sprintf(message,
"attribute_name_list=[%s] doesn't align with attribute_data_list=[%s].",
			list_display( attribute_name_list ),
			list_display( attribute_data_list ) );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( attribute_name_list )
	||   !list_rewind( attribute_data_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	do {
		attribute_name = list_get( attribute_name_list );

		dictionary_set(
			query_dictionary,
			attribute_name,
			list_get( attribute_data_list ) );

		dictionary_set(
			query_dictionary,
			strdup(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				query_relation_key(
					(char *)query_relation_operator_prefix,
					attribute_name ) ),
			(char *)query_equal );

		list_next( attribute_data_list );

	} while ( list_next( attribute_name_list ) );

	return query_dictionary;
}

char *attribute_insert_value_string(
		int integer,
		char *string )
{
	char value_string[ 65536 ];

	if ( !integer && ( !string || !*string ) )
	{
		strcpy( value_string, "null" );
	}
	else
	if ( integer )
	{
		sprintf(value_string,
			"%d",
			integer );
	}
	else
	{
		sprintf(value_string,
			"'%s'",
			string );
	}

	return strdup( value_string );
}

void attribute_name_list_sort_stdout( LIST *attribute_name_list )
{
	FILE *output_pipe;

	fflush( stdout );
	output_pipe = appaserver_output_pipe( "sort -u" );

	if ( list_rewind( attribute_name_list ) )
	do {
		fprintf(output_pipe,
			 "%s\n",
			(char *)list_get(
				attribute_name_list ) );

	} while ( list_next( attribute_name_list ) );

	pclose( output_pipe );
	fflush( stdout );
}

void attribute_folder_name_list_stdout( char *folder_name )
{
	LIST *list =
		folder_attribute_name_list(
			(char *)0 /* folder_name */,
			folder_attribute_list(
				(char *)0 /* role_name */,
				folder_name,
				(LIST *)0 /* exclude_attribute... */,
				0 /* not fetch_attribute */ ) );

	list_display_stdout( list );
}

void attribute_name_list_stdout(
		const char *attribute_table,
		const char *attribute_primary_key )
{
	char system_string[ 128 ];

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh %s %s '' select",
		attribute_primary_key,
		attribute_table );

	if ( system( system_string ) ){}
}
