/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_system/drilldown.c				*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "String.h"
#include "appaserver_error.h"
#include "appaserver_user.h"
#include "appaserver_parameter.h"
#include "environ.h"
#include "attribute.h"
#include "sql.h"
#include "post_dictionary.h"
#include "drilldown.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *login_name;
	char *session_key;
	char *role_name;
	char *target_frame;
	char *drilldown_base_folder_name;
	char *primary_data_list_string;
	char *update_results_string;
	char *update_error_string;
	APPASERVER_PARAMETER *appaserver_parameter;
	POST_DICTIONARY *post_dictionary;
	DRILLDOWN *drilldown;
	DRILLDOWN_ONE_TO_MANY *drilldown_one_to_many;
	DRILLDOWN_MANY_TO_ONE *drilldown_many_to_one;
	char buffer[ 1024 ];

	application_name = environ_exit_application_name( argv[ 0 ] );

	if ( argc != 10 )
	{
		fprintf( stderr,
"Usage: %s session login_name role target_frame base_folder primary_data_list_string update_results update_error dictionary\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	session_key = argv[ 1 ];
	login_name = argv[ 2 ];
	role_name = argv[ 3 ];
	target_frame = argv[ 4 ];
	drilldown_base_folder_name = argv[ 5 ];
	primary_data_list_string = argv[ 6 ];
	update_results_string = argv[ 7 ];
	update_error_string = argv[ 8 ];

	post_dictionary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_dictionary_string_new(
			DICTIONARY_ATTRIBUTE_DATUM_DELIMITER,
			DICTIONARY_ELEMENT_DELIMITER,
			argv[ 9 ] );

	/* -------------- */
	/* Safely returns */
	/* -------------- */
	appaserver_parameter = appaserver_parameter_new();

	drilldown =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		drilldown_new(
			argc,
			argv,
			application_name,
			session_key,
			login_name,
			role_name,
			drilldown_base_folder_name,
			target_frame,
			primary_data_list_string,
			update_results_string,
			update_error_string,
			post_dictionary->original_post_dictionary,
			appaserver_parameter->data_directory );

	printf( "%s\n%s\n%s\n%s\n%s\n",
		drilldown->drilldown_document->document->html,
		drilldown->drilldown_document->document->document_head->html,
		drilldown->drilldown_document->ajax_client_list_javascript,
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		document_head_close_tag(),
		drilldown->drilldown_document->document->document_body->html );

	if ( drilldown->drilldown_primary )
	{
		drilldown_table_edit_output(
			APPASERVER_USER_PRIMARY_KEY,
			login_name,
			drilldown->
				drilldown_primary->
				table_edit );
	}

	printf( "<br><hr>\n" );

	if ( list_rewind( drilldown->drilldown_one_to_many_list ) )
	do {
		drilldown_one_to_many =
			list_get(
				drilldown->
					drilldown_one_to_many_list );

		printf( "<h2>%s</h2>\n",
			string_initial_capital(
				buffer,
				drilldown_one_to_many->
					relation_many_folder_name ) );

		drilldown_table_edit_output(
			APPASERVER_USER_PRIMARY_KEY,
			login_name,
			drilldown_one_to_many->table_edit );

		printf( "<br><hr>\n" );

	} while ( list_next( drilldown->drilldown_one_to_many_list ) );

	if ( list_rewind( drilldown->drilldown_many_to_one_list ) )
	do {
		drilldown_many_to_one =
			list_get(
				drilldown->
					drilldown_many_to_one_list );

		printf( "<h2>%s</h2>\n",
			string_initial_capital(
				buffer,
				drilldown_many_to_one->
					one_folder_name ) );

		drilldown_table_edit_output(
			APPASERVER_USER_PRIMARY_KEY,
			login_name,
			drilldown_many_to_one->table_edit );

		printf( "<br><hr>\n" );

	} while ( list_next(
			drilldown->
				drilldown_many_to_one_list ) );

	document_close();

	return 0;
}

