/* -----------------------------------------------------------	*/
/* $APPASERVER_HOME/src_hydrology/post_current_vs_historical.c	*/
/* -----------------------------------------------------------	*/
/* Freely available software: see Appaserver.org		*/
/* -----------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "list.h"
#include "piece.h"
#include "folder.h"
#include "post2dictionary.h"
#include "operation.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "process.h"
#include "application.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "session.h"
#include "current_vs_historical.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char *login_name;
	char *session;
	char *process_name;
	char *state_string;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	DICTIONARY *post_dictionary;
	LIST *station_name_list;
	char *datatype_name = "";
	int current_year;
	int historical_range_years;
	char *station_type = "";
	char sys_string[ 4096 ];

	if ( argc > 1 )
	{
		application_name = argv[ 1 ];

		appaserver_error_starting_argv_append_file(
					argc,
					argv,
					application_name );
	}

	if ( argc < 10 )
	{
		fprintf( stderr, 
"Usage: %s application login_name session process state station_name_list datatype current_year historical_range_years [station_type]\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 2 ];
	session = argv[ 3 ];
	process_name = argv[ 4 ];
	state_string = argv[ 5 ];
	station_name_list = list_string2list( argv[ 6 ], ',' );
	datatype_name = argv[ 7 ];
	current_year = atoi( argv[ 8 ] );
	historical_range_years = atoi( argv[ 9 ] );

	if ( argc == 11
	&&   *argv[ 10 ]
	&&   strcmp( argv[ 10 ], "station_type" ) != 0 )
	{
		station_type = argv[ 10 ];
	}

	environ_set_environment(
		APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
		application_name );

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

	if ( session_remote_ip_address_changed(
		application_name,
		session ) )
	{
		session_message_ip_address_changed_exit(
				application_name,
				login_name );
	}

	if ( !session_access(	application_name,
				session,
				login_name ) )
	{
		session_access_failed_message_and_exit(
			application_name, session, login_name );
	}

	appaserver_parameter_file = appaserver_parameter_file_new();

	post_dictionary =
		post2dictionary(stdin,
				appaserver_parameter_file->
					appaserver_data_directory,
				session );

	if ( dictionary_exists_key(
				post_dictionary,
				DATATYPE_RADIO_BUTTON_IDENTIFIER ) )
	{
		datatype_name =
			dictionary_fetch(
				post_dictionary,
				DATATYPE_RADIO_BUTTON_IDENTIFIER );
	}

	if ( !list_length( station_name_list ) )
	{
		station_name_list =
			dictionary_get_indexed_data_list(
				post_dictionary,
				CURRENT_VS_HISTORICAL_STATION_PREFIX );
	}

	sprintf( sys_string, 
"output_current_vs_historical %s %s %s %s \"%s\" \"%s\" %d %d \"%s\" 2>>%s",
	 	 login_name,
		 session,
		 process_name,
		 state_string,
		 list_display_delimited( station_name_list, ',' ),
		 datatype_name,
		 current_year,
		 historical_range_years,
		 station_type,
		 appaserver_error_get_filename(
			application_name ) );

	if ( system( sys_string ) ){};

	return 0;

} /* main() */

