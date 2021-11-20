/* -------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_alligtor/post_alligator_srf_dataentry_wizard.c	*/
/* -------------------------------------------------------------------	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "date_convert.h"
#include "document.h"
#include "session.h"
#include "process.h"
#include "post2dictionary.h"
#include "application.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "alligator.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
int insert_into_nest(
			char *application_name,
			DICTIONARY *process_dictionary );

int main( int argc, char **argv )
{
	char *login_name, *application_name, *session;
	char *role_name;
	DICTIONARY *post_dictionary;
	char *process_dictionary_string;
	DICTIONARY *process_dictionary;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	int results;

	if ( argc != 5 )
	{
		fprintf( stderr, 
			 "Usage: %s login_name application session role\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 1 ];
	application_name = argv[ 2 ];
	session = argv[ 3 ];
	role_name = argv[ 4 ];

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}
	else
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			application_name );
	}

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

	appaserver_parameter_file = new_appaserver_parameter_file();

	if ( session_remote_ip_address_changed(
		application_name,
		session ) )
	{
		session_message_ip_address_changed_exit(
				application_name,
				login_name );
	}

	if ( !session_access_folder(
				application_name,
				session,
				"nest" /* folder_name */,
				role_name,
				"insert" /* state */ ) )
	{
		session_access_failed_message_and_exit(
			application_name, session, login_name );
	}

	post_dictionary =
		post2dictionary(stdin,
			(char *)0 /* appaserver_data_directory */,
			(char *)0 /* session */ );

	/* ------------------------------------------------------------ */
	/* The post_dictionary looks like:				*/
	/* post_dictionary=utm_easting_0^1111111&utm_northing_0^2222222 */
	/* ------------------------------------------------------------ */
	process_dictionary_string = dictionary_display( post_dictionary );

	strcpy(	process_dictionary_string,
		process_dictionary_string + strlen( "post_dictionary=" ) );

	search_replace_character(
		process_dictionary_string,
		'^',
		'=' );

	process_dictionary =
		dictionary_string2dictionary(
			process_dictionary_string );

	document_quick_output_body(	application_name,
					appaserver_parameter_file->
						appaserver_mount_point );

	results =  insert_into_nest(
			application_name,
			process_dictionary );

	if ( results != -1 )
	{
		if ( !results )
		{
			printf(
		"<p>Process NOT complete. Posted 0 nests.\n" );
		}
		else
		if ( results == 1 )
		{
			printf(
		"<p>Process complete. Posted 1 nest.\n" );
		}
		else
		{
			printf(
		"<p>Process complete. Posted %d nests.\n",
				results );
		}
	}
	else
	{
		printf( "<p>Process NOT complete.\n" );
	}

	document_close();
	exit( 0 );

}


int insert_into_nest(
			char *application_name,
			DICTIONARY *process_dictionary )
{
	DATE_CONVERT *discovery_date_convert;
	FILE *nest_insert_pipe;
	char *table_name;
	char sys_string[ 1024 ];
	char *field_list;
	char *utm_easting;
	char *utm_northing;
	char *discovery_date;
	char *nest_number;
	char *primary_researcher;
	char *transect_number;
	char *basin;
	char *habitat;
	char *monitor_nest_yn;
	char *SRF_yn;
	char *nest_notepad;
	int highest_index;
	int row;
	int posted_count = 0;
	char *in_transect_boundary_yn;
/*
	char *near_pond_yn;
*/

	field_list =
"nest_number,utm_easting,utm_northing,discovery_date,primary_researcher,transect_number,in_transect_boundary_yn,habitat,basin,monitor_nest_yn,SRF_yn,nest_notepad";

	table_name = get_table_name( application_name, "nest" );
	sprintf( sys_string,
		 "insert_statement.e table=%s field=%s			|"
		 "sql.e 2>&1						|"
		 "html_paragraph_wrapper.e				 ",
		 table_name,
		 field_list );
	nest_insert_pipe = popen( sys_string, "w" );

	highest_index = dictionary_get_key_highest_index( process_dictionary );

	if ( !highest_index ) return 0;

	for( row = 1; row <= highest_index; row++ )
	{
		if ( dictionary_get_index_data_strict_index(
						&nest_number,
						process_dictionary,
						"nest_number",
						row ) == 0 )
		{
			printf(
	"<h3>Warning: Cannot get nest number. Ignoring nest.</h3>\n" );
			continue;
		}

		if ( dictionary_get_index_data_strict_index(
						&utm_easting,
						process_dictionary,
						"utm_easting",
						row ) == 0 )
		{
			utm_easting = "";
		}

		if ( dictionary_get_index_data_strict_index(
						&utm_northing,
						process_dictionary,
						"utm_northing",
						row ) == 0 )
		{
			utm_northing = "";
		}

		if ( ! ( transect_number =
				alligator_get_transect_number_string(
					application_name,
					atoi( utm_northing ) ) ) )
		{
			transect_number = "";
		}

		if ( *transect_number )
		{
			double distance;

			if ( alligator_get_in_transect_boundary_should_be_yn(
					&distance,
					application_name,
					atoi( transect_number ),
					atoi( utm_northing ),
					atoi( utm_easting ) ) == 'y' )
			{
				in_transect_boundary_yn = "y";
			}
			else
			{
				in_transect_boundary_yn = "n";
			}
		}
		else
		{
			in_transect_boundary_yn = "n";
		}

		if ( *transect_number )
		{
			int cell_number = 0;

			if ( ! ( basin = alligator_get_basin_should_be(
						&cell_number,
						application_name,
						atof( utm_northing ),
						atof( utm_easting ) ) ) )
			{
				basin = "";
			}

			if ( !cell_number )
			{
				in_transect_boundary_yn = "n";
				transect_number = "";
				basin = "";
			}
		}
		else
		{
			basin = "";
		}

		/* This comes from index zero. */
		/* --------------------------- */
		if ( dictionary_get_index_data_strict_index(
						&discovery_date,
						process_dictionary,
						"discovery_date",
						0 ) == 0 )
		{
			fprintf( stderr,
			 "Error in %s/%s()/%d: cannot get discovery date\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			printf(
	"<h3>Error: Cannot get discovery date.</h3>\n" );
			return 0;
		}

		discovery_date_convert =
			date_convert_new_database_format_date_convert(
					application_name,
					discovery_date );

		if ( !discovery_date_convert )
		{
			fprintf( stderr,
			 "Error in %s/%s(): cannot fetch from database\n",
				 __FILE__,
				 __FUNCTION__ );
			return 0;
		}

		/* This comes from index zero. */
		/* --------------------------- */
		if ( dictionary_get_index_data_strict_index(
						&primary_researcher,
						process_dictionary,
						"primary_researcher",
						0 ) == 0 )
		{
			return 0;
		}

		if ( dictionary_get_index_data_strict_index(
						&habitat,
						process_dictionary,
						"habitat",
						row ) == 0 )
		{
			habitat = "";
		}

		if ( dictionary_get_index_data_strict_index(
						&monitor_nest_yn,
						process_dictionary,
						"monitor_nest_yn",
						row ) == 0 )
		{
			monitor_nest_yn = "";
		}

		if ( !monitor_nest_yn || *monitor_nest_yn != 'y' )
		{
			monitor_nest_yn = "n";
		}

		if ( dictionary_get_index_data_strict_index(
						&SRF_yn,
						process_dictionary,
						"SRF_yn",
						row ) == 0 )
		{
			SRF_yn = "y";
		}

		if ( !SRF_yn || *SRF_yn != 'y' )
		{
			SRF_yn = "n";
		}

		if ( dictionary_get_index_data_strict_index(
						&nest_notepad,
						process_dictionary,
						"nest_notepad",
						row ) == 0 )
		{
			nest_notepad = "";
		}

		fprintf( nest_insert_pipe,
			 "%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",
			 nest_number,
			 utm_easting,
			 utm_northing,
			 discovery_date_convert->return_date,
			 primary_researcher,
			 transect_number,
			 in_transect_boundary_yn,
			 habitat,
			 basin,
			 monitor_nest_yn,
			 SRF_yn,
			 nest_notepad );

		date_convert_free( discovery_date_convert );
		posted_count++;
	}

	pclose( nest_insert_pipe );

	return posted_count;
}

