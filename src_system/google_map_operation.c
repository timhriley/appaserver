/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_system/google_map_operation.c			*/
/* --------------------------------------------------------------------	*/
/* This is the operation process for the google map button.		*/
/*									*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "list.h"
#include "appaserver_parameter.h"
#include "appaserver_error.h"
#include "environ.h"
#include "document.h"
#include "google_map_operation.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *folder_name;
	char *target_frame;
	char *operation_name;
	char *latitude_string;
	char *longitude_string;
	char *utm_easting_string;
	char *utm_northing_string;
	char *primary_data_list_string;
	pid_t parent_process_id;
	int operation_row_checked_count;
	APPASERVER_PARAMETER *appaserver_parameter;
	GOOGLE_MAP_OPERATION *google_map_operation;

	application_name = environment_exit_application_name( argv[ 0 ] );

/*
	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );
*/

	if ( argc != 14 )
	{
		fprintf( stderr,
"Usage: %s session login_name role folder target_frame operation latitude longitude utm_easting utm_northing primary_data_list_string process_id operation_row_checked_count\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	session_key = argv [ 1 ];
	login_name = argv [ 2 ];
	role_name = argv[ 3 ];
	folder_name = argv[ 4 ];
	target_frame = argv[ 5 ];
	operation_name = argv[ 6 ];
	latitude_string = argv[ 7 ];
	longitude_string = argv[ 8 ];
	utm_easting_string = argv[ 9 ];
	utm_northing_string = argv[ 10 ];
	primary_data_list_string = argv[ 11 ];
	parent_process_id = (pid_t)atoi( argv[ 12 ] );
	operation_row_checked_count = atoi( argv[ 13 ] );

	/* Safely returns */
	/* -------------- */
	appaserver_parameter = appaserver_parameter_new();

	google_map_operation =
		google_map_operation_new(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			target_frame,
			operation_name,
			latitude_string,
			longitude_string,
			utm_easting_string,
			utm_northing_string,
			primary_data_list_string,
			parent_process_id,
			operation_row_checked_count,
			appaserver_parameter->data_directory );

	if ( google_map_operation )
	{
		if ( google_map_operation->
			operation_semaphore->
			group_last_time )
		{
			if ( !google_map_operation->google_filename
			||   !google_map_operation->
				google_filename->
				appaserver_link_anchor_html )
			{
				char message[ 128 ];

				snprintf(
					message,
					sizeof ( message ),
				"google_filename is empty or incomplete." );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			printf(	"%s\n",
				google_map_operation->
					document_form_html );

			printf(	"%s\n",
				google_map_operation->
					google_filename->
					appaserver_link_anchor_html );

			document_close();
		}
	}

	return 0;
}

