/* -----------------------------------------------------	*/
/* src_alligator/post_alligator_visit_dataentry_wizard.c	*/
/* ---------------------------------------------------		*/
/*								*/
/* Freely available software: see Appaserver.org		*/
/* -----------------------------------------------------	*/

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

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void update_nest(	char *application_name,
			DICTIONARY *process_dictionary );

void insert_water_depth(
			char *application_name,
	 		char *nest_number,
	 		char *observation_date,
			char *water_depth_1_cm,
			char *water_depth_2_cm,
			char *water_depth_3_cm,
			char *water_depth_4_cm );

double get_water_depth_average_cm(
			boolean *is_null,
			char *water_depth_1_cm,
			char *water_depth_2_cm,
			char *water_depth_3_cm,
			char *water_depth_4_cm );

int insert_into_nest_observation(
			char *application_name,
			DICTIONARY *process_dictionary );

int post_dataentry_wizard(
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

	results = post_dataentry_wizard(
			application_name,
			process_dictionary );

	if ( results >= 1 )
	{
		if ( results == 1 )
		{
			printf(
		"<p>Process complete. Posted 1 observation.\n" );
		}
		else
		{
			printf(
		"<p>Process complete. Posted %d observations.\n",
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

int post_dataentry_wizard(	char *application_name,
				DICTIONARY *process_dictionary )
{
	int results = 1;

	if ( ! ( results =
			insert_into_nest_observation(
				application_name,
				process_dictionary ) ) )
	{
		return 0;
	}

	update_nest( application_name, process_dictionary );

	return results;
}

int insert_into_nest_observation(
			char *application_name,
			DICTIONARY *process_dictionary )
{
	DATE_CONVERT *observation_date_convert;
	FILE *nest_observation_insert_pipe;
	char *table_name;
	char sys_string[ 1024 ];
	char *field_list;
	char *nest_number;
	char *observation_date;
	char *nest_visit_number;
	char *observation_view;
	char *nest_status;
	char *female_behavior;
	char *hatchlings_count;
	char *hatched_eggs;
	char *flooded_eggs;
	char *predated_eggs;
	char *crushed_eggs;
	char *other_egg_mortality;
	char *water_depth_1_cm;
	char *water_depth_2_cm;
	char *water_depth_3_cm;
	char *water_depth_4_cm;
	double water_depth_average_cm;
	char water_depth_average_cm_string[ 16 ];
	char *nest_height_above_water_cm;
	char *observation_notepad;
	int row;
	int posted_count = 0;
	int highest_index;
	boolean is_null;

	highest_index = dictionary_get_key_highest_index( process_dictionary );

	field_list =
"nest_number,observation_date,nest_visit_number,observation_view,nest_status,female_behavior,hatchlings_count,hatched_eggs,flooded_eggs,predated_eggs,crushed_eggs,other_egg_mortality,water_depth_average_cm,nest_height_above_water_cm,observation_notepad";

	table_name = get_table_name( application_name, "nest_visit" );
	sprintf( sys_string,
		 "insert_statement.e table=%s field=%s			|"
		 "sql.e 2>&1						|"
		 "html_paragraph_wrapper.e				 ",
		 table_name,
		 field_list );
	nest_observation_insert_pipe = popen( sys_string, "w" );

	if ( !dictionary_get_index_data_strict_index(
					&nest_number,
					process_dictionary,
					"nest_number",
					0 ) )
	{
		fprintf( stderr,
			 "Error in %s/%s(): cannot fetch nest number in (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 dictionary_display( process_dictionary ) );
		return 0;
	}

	for( row = 0; row <= highest_index; row++ )
	{
		if ( !dictionary_get_index_data_strict_index(
					&observation_date,
					process_dictionary,
					"observation_date",
					row ) )
		{
			if ( highest_index == 0 )
				return 0;
			else
				continue;
		}

		observation_date_convert =
			date_convert_new_database_format_date_convert(
					application_name,
					observation_date );

		if ( !observation_date_convert )
		{
			fprintf( stderr,
			 "Error in %s/%s()/%d: cannot fetch from database\n",
			 	__FILE__,
			 	__FUNCTION__,
				__LINE__ );
			return 0;
		}

		if ( !dictionary_get_index_data_strict_index(
					&nest_visit_number,
					process_dictionary,
					"nest_visit_number",
					row ) )
		{
			nest_visit_number = "";
		}

		if ( !dictionary_get_index_data_strict_index(
					&observation_view,
					process_dictionary,
					"observation_view",
					row ) )
		{
			observation_view = "";
		}

		if ( !dictionary_get_index_data_strict_index(
					&nest_status,
					process_dictionary,
					"nest_status",
					row ) )
		{
			nest_status = "";
		}

		if ( !dictionary_get_index_data_strict_index(
					&female_behavior,
					process_dictionary,
					"female_behavior",
					row ) )
		{
			female_behavior = "";
		}

		if ( strcmp( female_behavior, "undefined" ) == 0 )
			female_behavior = "";

		if ( !dictionary_get_index_data_strict_index(
					&hatchlings_count,
					process_dictionary,
					"hatchlings_count",
					row ) )
		{
			hatchlings_count = "";
		}

		if ( !dictionary_get_index_data_strict_index(
					&hatched_eggs,
					process_dictionary,
					"hatched_eggs",
					row ) )
		{
			hatched_eggs = "";
		}

		if ( !dictionary_get_index_data_strict_index(
					&flooded_eggs,
					process_dictionary,
					"flooded_eggs",
					row ) )
		{
			flooded_eggs = "";
		}

		if ( !dictionary_get_index_data_strict_index(
					&predated_eggs,
					process_dictionary,
					"predated_eggs",
					row ) )
		{
			predated_eggs = "";
		}

		if ( !dictionary_get_index_data_strict_index(
					&crushed_eggs,
					process_dictionary,
					"crushed_eggs",
					row ) )
		{
			crushed_eggs = "";
		}

		if ( !dictionary_get_index_data_strict_index(
					&other_egg_mortality,
					process_dictionary,
					"other_egg_mortality",
					row ) )
		{
			other_egg_mortality = "";
		}

		if ( !dictionary_get_index_data_strict_index(
					&water_depth_1_cm,
					process_dictionary,
					"water_depth_1_cm",
					row ) )
		{
			water_depth_1_cm = "";
		}

		if ( !dictionary_get_index_data_strict_index(
					&water_depth_2_cm,
					process_dictionary,
					"water_depth_2_cm",
					row ) )
		{
			water_depth_2_cm = "";
		}

		if ( !dictionary_get_index_data_strict_index(
					&water_depth_3_cm,
					process_dictionary,
					"water_depth_3_cm",
					row ) )
		{
			water_depth_3_cm = "";
		}

		if ( !dictionary_get_index_data_strict_index(
					&water_depth_4_cm,
					process_dictionary,
					"water_depth_4_cm",
					row ) )
		{
			water_depth_4_cm = "";
		}

		if ( !dictionary_get_index_data_strict_index(
					&nest_height_above_water_cm,
					process_dictionary,
					"nest_height_above_water_cm",
					row ) )
		{
			nest_height_above_water_cm = "";
		}

		if ( !dictionary_get_index_data_strict_index(
					&observation_notepad,
					process_dictionary,
					"observation_notepad",
					row ) )
		{
			observation_notepad = "";
		}

		water_depth_average_cm =
			get_water_depth_average_cm(
				&is_null,
				water_depth_1_cm,
				water_depth_2_cm,
				water_depth_3_cm,
				water_depth_4_cm );

		if ( is_null )
		{
			*water_depth_average_cm_string = '\0';
		}
		else
		{
			sprintf(	water_depth_average_cm_string,
					"%.3lf",
					water_depth_average_cm );

			insert_water_depth(
				application_name,
		 		nest_number,
		 		observation_date_convert->return_date,
				water_depth_1_cm,
				water_depth_2_cm,
				water_depth_3_cm,
				water_depth_4_cm );
		}

		fprintf( nest_observation_insert_pipe,
	"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",
		 	nest_number,
		 	observation_date_convert->return_date,
			nest_visit_number,
			observation_view,
			nest_status,
			female_behavior,
			hatchlings_count,
			hatched_eggs,
			flooded_eggs,
			predated_eggs,
			crushed_eggs,
			other_egg_mortality,
			water_depth_average_cm_string,
			nest_height_above_water_cm,
			observation_notepad );

		date_convert_free( observation_date_convert );
		posted_count++;
	}

	pclose( nest_observation_insert_pipe );

	return posted_count;
}

double get_water_depth_average_cm(
				boolean *is_null,
				char *water_depth_1_cm,
				char *water_depth_2_cm,
				char *water_depth_3_cm,
				char *water_depth_4_cm )
{
	double total = 0.0;
	int count = 0;

	if ( *water_depth_1_cm )
	{
		total += atof( water_depth_1_cm );
		count++;
	}

	if ( *water_depth_2_cm )
	{
		total += atof( water_depth_2_cm );
		count++;
	}

	if ( *water_depth_3_cm )
	{
		total += atof( water_depth_3_cm );
		count++;
	}

	if ( *water_depth_4_cm )
	{
		total += atof( water_depth_4_cm );
		count++;
	}

	if ( !count )
	{
		*is_null = 1;
		return 0.0;
	}
	else
	{
		*is_null = 0;
		return total / (double)count;
	}
}

void insert_water_depth(
				char *application_name,
		 		char *nest_number,
		 		char *observation_date,
				char *water_depth_1_cm,
				char *water_depth_2_cm,
				char *water_depth_3_cm,
				char *water_depth_4_cm )
{
	FILE *water_depth_insert_pipe;
	char *table_name;
	char sys_string[ 1024 ];
	char *field_list;
	int measurement_number = 1;

	field_list =
"nest_number,observation_date,measurement_number,water_depth_cm";

	table_name = get_table_name( application_name, "water_depth" );
	sprintf( sys_string,
		 "insert_statement.e table=%s field=%s			|"
		 "sql.e 2>&1						|"
		 "html_paragraph_wrapper.e				 ",
		 table_name,
		 field_list );
	water_depth_insert_pipe = popen( sys_string, "w" );

	if ( *water_depth_1_cm )
	{
		fprintf( water_depth_insert_pipe,
			 "%s|%s|%d|%s\n",
		 	 nest_number,
		 	 observation_date,
			 measurement_number++,
			 water_depth_1_cm );
	}
	if ( *water_depth_2_cm )
	{
		fprintf( water_depth_insert_pipe,
			 "%s|%s|%d|%s\n",
		 	 nest_number,
		 	 observation_date,
			 measurement_number++,
			 water_depth_2_cm );
	}
	if ( *water_depth_3_cm )
	{
		fprintf( water_depth_insert_pipe,
			 "%s|%s|%d|%s\n",
		 	 nest_number,
		 	 observation_date,
			 measurement_number++,
			 water_depth_3_cm );
	}
	if ( *water_depth_4_cm )
	{
		fprintf( water_depth_insert_pipe,
			 "%s|%s|%d|%s\n",
		 	 nest_number,
		 	 observation_date,
			 measurement_number++,
			 water_depth_4_cm );
	}

	pclose( water_depth_insert_pipe );
}

void update_nest(	char *application_name,
			DICTIONARY *process_dictionary )
{
	FILE *nest_update_pipe;
	char *table_name;
	char sys_string[ 1024 ];
	char *key_field_list;
	char *nest_number;
	char *buffer;
	int row;
	int highest_index;

	key_field_list = "nest_number";

	table_name = get_table_name( application_name, "nest" );
	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y		|"
		 "sql.e 2>&1						|"
		 "html_paragraph_wrapper.e				 ",
		 table_name,
		 key_field_list );
	nest_update_pipe = popen( sys_string, "w" );

	if ( !dictionary_get_index_data_strict_index(
					&nest_number,
					process_dictionary,
					"nest_number",
					0 ) )
	{
		fprintf( stderr,
			 "Error in %s/%s(): cannot fetch nest number in (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 dictionary_display( process_dictionary ) );
		return;
	}

	highest_index = dictionary_get_key_highest_index( process_dictionary );

	for( row = 0; row <= highest_index; row++ )
	{
		if ( dictionary_get_index_data_strict_index(
					&buffer,
					process_dictionary,
					"total_eggs",
					row ) )
		{
			fprintf( nest_update_pipe,
				 "%s^total_eggs^%s\n",
				 nest_number,
				 buffer );
		}

		if ( dictionary_get_index_data_strict_index(
					&buffer,
					process_dictionary,
					"banded_eggs",
					row ) )
		{
			fprintf( nest_update_pipe,
				 "%s^banded_eggs^%s\n",
				 nest_number,
				 buffer );
		}

		if ( dictionary_get_index_data_strict_index(
					&buffer,
					process_dictionary,
					"not_banded_eggs",
					row ) )
		{
			fprintf( nest_update_pipe,
				 "%s^not_banded_eggs^%s\n",
				 nest_number,
				 buffer );
		}

		if ( dictionary_get_index_data_strict_index(
					&buffer,
					process_dictionary,
					"female_size_cm",
					row ) )
		{
			fprintf( nest_update_pipe,
				 "%s^female_size_cm^%s\n",
				 nest_number,
				 buffer );
		}

		if ( dictionary_get_index_data_strict_index(
					&buffer,
					process_dictionary,
					"habitat",
					row ) )
		{
			fprintf( nest_update_pipe,
				 "%s^habitat^%s\n",
				 nest_number,
				 buffer );
		}

		if ( dictionary_get_index_data_strict_index(
					&buffer,
					process_dictionary,
					"materials",
					row ) )
		{
			fprintf( nest_update_pipe,
				 "%s^materials^%s\n",
				 nest_number,
				 buffer );
		}

		if ( dictionary_get_index_data_strict_index(
					&buffer,
					process_dictionary,
					"nest_height_cm",
					row ) )
		{
			fprintf( nest_update_pipe,
				 "%s^nest_height_cm^%s\n",
				 nest_number,
				 buffer );
		}

	}

	pclose( nest_update_pipe );

}

