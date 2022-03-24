/* $APPASERVER_HOME/library/application_constants.c	*/
/* ==================================================== */
/*                                                      */
/* Freely available software: see Appaserver.org	*/
/* ==================================================== */

#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "application_constants.h"
#include "appaserver_parameter.h"
#include "appaserver_library.h"
#include "timlib.h"
#include "sql.h"
#include "piece.h"

APPLICATION_CONSTANTS *application_constants_new( void )
{
	APPLICATION_CONSTANTS *application_constants;

	if ( ! ( application_constants =
			calloc( 1, sizeof( APPLICATION_CONSTANTS ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return application_constants;
}

DICTIONARY *application_constants_dictionary( void )
{
	char system_string[ 1024 ];
	char input_buffer[ 1024 ];
	char key[ 128 ];
	char data[ 65536 ];
	FILE *input_pipe;
	DICTIONARY *dictionary = dictionary_small();

	sprintf(system_string,
		"select.sh \"%s\" %s",
		APPLICATION_CONSTANTS_SELECT,
		APPLICATION_CONSTANTS_TABLE );

	input_pipe = popen( system_string, "r" );

	while( string_input( input_buffer, input_pipe, 1024 ) )
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

void application_constants_free(
			APPLICATION_CONSTANTS *application_constants )
{
	dictionary_free( application_constants->dictionary );
	free( application_constants );
}

char *application_constants_quick_fetch(
				char *key )
{
	APPLICATION_CONSTANTS *application_constants;
	char *data;
	char *results;

	application_constants = application_constants_new();

	application_constants->dictionary =
		application_constants_dictionary();

	data =
		application_constants_fetch(
			application_constants->dictionary,
			key );

	if ( !data ) return "";

	results = strdup( data );
	application_constants_free( application_constants );

	return results;
}

boolean application_constants_cat_javascript_source( void )
{
	char *value;
	boolean return_value;

	return_value = 0;

	if ( ( value =
		application_constants_quick_fetch(
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

