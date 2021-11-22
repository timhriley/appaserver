/* $APPASERVER_HOME/library/attribute.c					*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "sql.h"
#include "piece.h"
#include "environ.h"
#include "folder.h"
#include "attribute.h"

ATTRIBUTE *attribute_new( char *attribute_name )
{
	ATTRIBUTE *attribute;

	if ( ! ( attribute = calloc( 1, sizeof( ATTRIBUTE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	attribute->attribute_name = attribute_name;
	return attribute;
}

ATTRIBUTE *attribute_seek(
			char *attribute_name,
			LIST *attribute_list )
{
	ATTRIBUTE *attribute;

	if ( list_rewind( attribute_list ) )
	{
		do {
			attribute = list_get( attribute_list );

			if ( string_strcmp(
					attribute->attribute_name,
					attribute_name ) == 0 )
			{
				return attribute;
			}

		} while( list_next( attribute_list ) );
	}
	return (ATTRIBUTE *)0;
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

char *attribute_system_string( char *where )
{
	char system_string[ 1024 ];

	if ( !where ) where = "1 = 1";

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		ATTRIBUTE_SELECT,
		ATTRIBUTE_TABLE	,
		where );

	return strdup( system_string );
}

LIST *attribute_system_list( char *system_string )
{
	char input[ 2048 ];
	FILE *pipe;
	LIST *list = {0};
	ATTRIBUTE *attribute;

	pipe = popen( system_string, "r"  );

	while ( string_input( input, pipe, 2048 ) )
	{
		if ( ( attribute = attribute_parse( input ) ) )
		{
			if ( !list ) list = list_new();

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

	if ( !input || !*input ) return (ATTRIBUTE *)0;

	/* See ATTRIBUTE_SELECT */
	/* -------------------- */
	piece( buffer, SQL_DELIMITER, input, 0 );
	attribute = attribute_new( strdup( buffer ) );

	piece( buffer, SQL_DELIMITER, input, 1 );
	attribute->datatype_name = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	attribute->width = atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	attribute->float_decimal_places = atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	attribute->hint_message = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 5 );
	attribute->post_change_javascript = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 6 );
	attribute->on_focus_javascript_function = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 7 );
	attribute->lookup_histogram_output = ( *buffer == 'y' );

	piece( buffer, SQL_DELIMITER, input, 8 );
	attribute->lookup_time_chart_output = ( *buffer == 'y' );

	return attribute;
}

boolean attribute_is_date( char *datatype )
{
	if (  string_strcmp( datatype, "date" ) == 0
	||    string_strcmp( datatype, "current_date" ) == 0 )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

boolean attribute_is_time(
			char *datatype )
{
	if (  string_strcmp( datatype, "time" ) == 0
	||    string_strcmp( datatype, "current_time" ) == 0 )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

boolean attribute_is_number(
			char *datatype )
{
	if (  string_strcmp( datatype, "float" ) == 0
	||    string_strcmp( datatype, "integer" ) == 0
	||    string_strcmp( datatype, "reference_number" ) == 0 )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

boolean attribute_is_float(
			char *datatype )
{
	if (  string_strcmp( datatype, "float" ) == 0 )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

boolean attribute_is_date_time( char *datatype )
{
	if (  string_strcmp( datatype, "date_time" ) == 0
	||    string_strcmp( datatype, "current_date_time" ) == 0 )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

LIST *attribute_list( void )
{
	static LIST *list = {0};

	if ( list ) return list;

	list = attribute_system_list( "1 = 1" );

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
			char *folder_name,
			char *attribute_name )
{
	static char full_attribute_name[ 512 ];
	static char *application_name = {0};

	if ( !application_name )
	{
		if ( ! ( application_name =
				environment_application_name() ) )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: environment_application_name() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}
	}

	if ( folder_name && *folder_name )
	{
		sprintf(full_attribute_name,
			"%s.%s",
			/* ----------------------------- */
			/* Returns static memory or null */
			/* ----------------------------- */
			folder_table_name(
				application_name,
				folder_name ),
			attribute_name );
	}
	else
	{
		strcpy( full_attribute_name, attribute_name );
	}

	return full_attribute_name;
}

boolean attribute_is_boolean(
			char *attribute_name )
{
	return attribute_is_yes_no( attribute_name );
}

boolean attribute_is_yes_no(
			char *attribute_name )
{
	int str_len;

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

