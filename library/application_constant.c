/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/application_constant.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "appaserver_parameter.h"
#include "sql.h"
#include "piece.h"
#include "application_constant.h"

APPLICATION_CONSTANT *application_constant_new( void )
{
	APPLICATION_CONSTANT *application_constant =
		application_constant_calloc();

	application_constant->dictionary =
		application_constant_dictionary();

	return application_constant;
}

APPLICATION_CONSTANT *application_constant_calloc( void )
{
	APPLICATION_CONSTANT *application_constant;

	if ( ! ( application_constant =
			calloc( 1, sizeof ( APPLICATION_CONSTANT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return application_constant;
}

DICTIONARY *application_constant_dictionary( void )
{
	char system_string[ 1024 ];
	char input_buffer[ 1024 ];
	char key[ 128 ];
	char data[ 65536 ];
	FILE *input_pipe;
	DICTIONARY *dictionary = dictionary_small();

	sprintf(system_string,
		"select.sh \"%s\" %s",
		APPLICATION_CONSTANT_SELECT,
		APPLICATION_CONSTANT_TABLE );

	input_pipe = popen( system_string, "r" );

	while( string_input( input_buffer, input_pipe, 1024 ) )
	{
		/* See APPLICATION_CONSTANT_SELECT */
		/* ------------------------------- */
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

char *application_constant_safe_get(
		char *key,
		DICTIONARY *application_constant_dictionary )
{
	char *get;

	if ( ! ( get =
			/* --------------------------------------- */
			/* Returns component of dictionary or null */
			/* --------------------------------------- */
			dictionary_get(
				key,
				application_constant_dictionary ) ) )
	{
		fprintf( stderr,
			 "Warning in %s/%s()/%d: cannot fetch key = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 key );

		get = "";
	}
	return get;
}

char *application_constant_get(
		char *key,
		DICTIONARY *application_constant_dictionary )
{
	/* Returns component of dictionary or null */
	/* --------------------------------------- */
	return dictionary_get( key, application_constant_dictionary );

}

void application_constant_free(
		APPLICATION_CONSTANT *application_constant )
{
	if ( application_constant && application_constant->dictionary )
	{
		dictionary_free( application_constant->dictionary );
		free( application_constant );
	}
}

char *application_constant_fetch( char *key )
{
	static APPLICATION_CONSTANT *application_constant = {0};
	char *get;

	if ( !key )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: key is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		exit( 1 );
	}

	if ( !application_constant )
	{
		application_constant = application_constant_new();
	}

	get =
	     /* ------------------------------------------------------------ */
	     /* Returns component of application_constant_dictionary or null */
	     /* ------------------------------------------------------------ */
	     application_constant_get(
			key,
			application_constant->dictionary );

	if ( !get )
		return "";
	else
		return get;
}

boolean application_constant_cat_javascript_source( void )
{
	char *value;
	boolean return_value = 0;

	if ( ( value =
		application_constant_fetch(
			"cat_javascript_source" ) ) )
	{
		if ( string_strcmp( value, "yes" ) == 0
		||   string_strcmp( value, "true" ) == 0
		||   string_strcmp( value, "1" ) == 0 )
		{
			return_value = 1;
		}
	}

	return return_value;
}

