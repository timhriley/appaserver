/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/insert_feeder_phrase.c		*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include "environ.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "document.h"
#include "entity.h"
#include "feeder_phrase.h"
#include "insert_feeder.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *feeder_account;
	char *new_full_name;
	char *feeder_phrase;
	INSERT_FEEDER *insert_feeder;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 5 )
	{
		fprintf(stderr,
	"Usage: %s process feeder_account new_full_name feeder_phrase\n",
			argv[ 0 ] );

		exit ( 1 );
	}

	process_name = argv[ 1 ];
	feeder_account = argv[ 2 ];
	new_full_name = argv[ 3 ];
	feeder_phrase = argv[ 4 ];

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		process_name /* title */ );

	insert_feeder =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		insert_feeder_new(
			feeder_account,
			new_full_name,
			feeder_phrase );

	if ( insert_feeder->error_message )
	{
		printf( "<h3>%s</h3>\n",
			insert_feeder->error_message );
	}
	else
	{
		char *feeder_error_message;
		char *entity_error_message;

		feeder_error_message =
			/* ----------------------------- */
			/* Returns error message or null */
			/* ----------------------------- */
			feeder_phrase_insert(
				FEEDER_PHRASE_TABLE,
				FEEDER_PHRASE_SELECT,
				ENTITY_CONTACT_KEY_COLUMN,
				insert_feeder->entity_contact_key_boolean,
				feeder_phrase,
				feeder_account,
				new_full_name,
				insert_feeder->entity_contact_key );

		entity_error_message =
			/* ----------------------------- */
			/* Returns error message or null */
			/* ----------------------------- */
			entity_insert(
				ENTITY_TABLE,
				ENTITY_FULL_NAME_COLUMN,
				ENTITY_CONTACT_KEY_COLUMN,
				insert_feeder->entity_contact_key_boolean,
				new_full_name,
				insert_feeder->entity_contact_key,
				0 /* not ignore_duplicate_boolean */ );
	
		if ( feeder_error_message )
			printf( "<h3>%s</h3>\n", feeder_error_message );

		if ( entity_error_message )
			printf( "<h3>%s</h3>\n", entity_error_message );

		printf( "<h3>Insert complete for feeder phrase: %s</h3>\n",
			feeder_phrase );
	}

	document_close();

	return 0;
}

