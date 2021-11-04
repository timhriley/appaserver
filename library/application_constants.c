/* $APPASERVER_HOME/library/application_constants.c	*/
/* ==================================================== */
/*                                                      */
/* Freely available software: see Appaserver.org	*/
/* ==================================================== */

#include "stdlib.h"
#include "string.h"
#include "application_constants.h"
#include "appaserver_parameter_file.h"
#include "appaserver_library.h"
#include "timlib.h"
#include "sql.h"
#include "piece.h"

APPLICATION_CONSTANTS *application_constants_new( void )
{
	APPLICATION_CONSTANTS *a;

	a = (APPLICATION_CONSTANTS *)
		calloc( 1, sizeof( APPLICATION_CONSTANTS ) );

	return a;
}

DICTIONARY *application_constants_dictionary(
				char *application_name )
{
	return application_constants_get_dictionary(
				application_name );
}

DICTIONARY *application_constants_get_dictionary(
				char *application_name )
{
	char sys_string[ 1024 ];
	DICTIONARY *dictionary;
	char input_buffer[ 65536 ];
	char key[ 128 ];
	char data[ 65536 ];
	FILE *input_pipe;
	char *select = "application_constant,application_constant_value";

	dictionary = dictionary_new();

	sprintf( sys_string,
		"get_folder_data	application=%s			"
		"			select=%s			"
		"			folder=application_constants	",
		application_name,
		select );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( key, SQL_DELIMITER, input_buffer, 0 );
		piece( data, SQL_DELIMITER, input_buffer, 1 );

		dictionary_set(
			dictionary,
			strdup( key ),
			strdup( data ) );
	}

	pclose( input_pipe );
	return dictionary;
}

char *application_constants_safe_fetch(
				DICTIONARY *application_constants_dictionary,
				char *key )
{
	char *results;

	if ( ! ( results =
			dictionary_fetch(
				key,
				application_constants_dictionary ) ) )
	{
		fprintf( stderr,
			 "Warning in %s/%s()/%d: cannot fetch key = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 key );
		return "";
	}
	return results;
}

char *application_constants_fetch(
				DICTIONARY *application_constants_dictionary,
				char *key )
{
	return dictionary_fetch( key, application_constants_dictionary );

}

void application_constants_get_easycharts_width_height(
			int *easycharts_width,
			int *easycharts_height,
			char *application_name )
{
	APPLICATION_CONSTANTS *application_constants;
	char *results;

	application_constants = application_constants_new();
	application_constants->dictionary =
		application_constants_get_dictionary(
			application_name );

	if ( ( results =
		application_constants_fetch(
			application_constants->dictionary,
			"easycharts_width" ) ) )
	{
		*easycharts_width = atoi( results );
	}
	else
	{
		*easycharts_width = 0;
	}

	if ( ( results =
		application_constants_fetch(
			application_constants->dictionary,
			"easycharts_height" ) ) )
	{
		*easycharts_height = atoi( results );
	}
	else
	{
		*easycharts_height = 0;
	}

}

void application_constants_free(
			APPLICATION_CONSTANTS *application_constants )
{
	dictionary_free( application_constants->dictionary );
	free( application_constants );
}

char *application_constants_quick_fetch(
				char *application_name,
				char *key )
{
	APPLICATION_CONSTANTS *application_constants;
	char *data;
	char *results;

	application_constants = application_constants_new();
	application_constants->dictionary =
		application_constants_get_dictionary(
			application_name );

	data = application_constants_fetch(
				application_constants->dictionary,
				key );

	if ( !data ) return "";

	results = strdup( data );
	application_constants_free( application_constants );

	return results;
}

boolean application_constants_cat_javascript_source( char *application_name )
{
	char *value;
	boolean return_value;

	return_value = 0;

	if ( ( value = application_constants_quick_fetch(
					application_name,
					"cat_javascript_source" ) ) )
	{
		if ( timlib_strcmp( value, "yes" ) == 0
		||   timlib_strcmp( value, "true" ) == 0
		||   timlib_strcmp( value, "1" ) == 0 )
		{
			return_value = 1;
		}
	}

	return return_value;

}

