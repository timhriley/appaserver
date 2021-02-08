/* src_benthic/benthic_validation_form.c				*/
/* ----------------------------------------------------------------	*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "list.h"
#include "aggregate_level.h"
#include "date.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "application.h"
#include "appaserver_parameter_file.h"
#include "process.h"
#include "form.h"
#include "environ.h"
#include "sampling_point.h"
#include "sweep.h"

/* Constants */
/* --------- */
#define MAX_QUAD_NUMBER		6
#define MAX_SWEEP_NUMBER	5
#define MAX_REPLICATE_NUMBER	4
#define TARGET_FRAME		EDIT_FRAME

/* Type Definitions */
/* ---------------- */

/* Prototypes */
/* ---------- */
void output_throwtrap_replicate(
				char *application_name,
				char *anchor_date,
				char *anchor_time,
				char *location,
				int site_number );

boolean exists_throwtrap_replicate(
				char *application_name,
				char *anchor_date,
				char *anchor_time,
				char *location,
				int site_number,
				int replicate_number );

void output_sampling_point_abiotic_measurement(
					char *application_name,
					char *anchor_date,
					char *anchor_time,
					char *location,
					int site_number );

void output_throwtrap_abiotic_measurement(
					char *application_name,
					char *anchor_date,
					char *anchor_time,
					char *location,
					int site_number,
					int replicate_number );

void validation_form_output_heading(	char *title,
					char *action_string,
					char *target_frame,
					int table_border );

void validation_form_output_trailer(	void );

void output_quad(			char *application_name,
					char *anchor_date,
					char *anchor_time,
					char *location,
					int site_number,
					int quad );

void output_benthic_validation_form(
					char *application_name,
					SAMPLING_POINT *sampling_point,
					char *action_string,
					char *title );

void output_species_measurement_sweep(
					char *application_name,
					char *anchor_date,
					char *anchor_time,
					char *location,
					int site_number,
					int replicate_number,
					int sweep_number );

void output_species_count_sweep(
					char *application_name,
					char *anchor_date,
					char *anchor_time,
					char *location,
					int site_number,
					int replicate_number,
					int sweep_number );

void output_sweep(
					char *application_name,
					char *anchor_date,
					char *anchor_time,
					char *location,
					int site_number,
					int replicate_number,
					int sweep_number );

void output_attempt_success(
					char *application_name,
					char *anchor_date,
					char *anchor_time,
					char *location,
					int site_number );

void output_quad_vegetation(
					char *application_name,
					char *anchor_date,
					char *anchor_time,
					char *location,
					int site_number,
					int quad );

void output_quad_vegetation_total(
					char *application_name,
					char *anchor_date,
					char *anchor_time,
					char *location,
					int site_number,
					int quad );

void output_sampling_point(		SAMPLING_POINT *sampling_point );

void output_abiotic_measurement(
					char *application_name,
					char *anchor_date,
					char *anchor_time,
					char *location,
					int site_number );

void output_quad_substrate(
					char *application_name,
					char *anchor_date,
					char *anchor_time,
					char *location,
					int site_number,
					int quad );

char *get_title_string(			char *collection_name,
					char *location,
					int site_number );

int main( int argc, char **argv )
{
	char *application_name;
	char original_application_name[ 128 ];
	char *login_name;
	char *session;
	char *process_name;
	char *anchor_date;
	char *anchor_time;
	char *location;
	char *site_number_string;
	char *title;
	char action_string[ 1024 ];
	DOCUMENT *document;
	char *database_string = {0};
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	SAMPLING_POINT *sampling_point;

	output_starting_argv_stderr( argc, argv );

	if ( argc != 9 )
	{
		fprintf( stderr, 
"Usage: %s application login_name session process anchor_date anchor_time location site_number\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	strcpy( original_application_name, application_name );

	login_name = argv[ 2 ];
	session = argv[ 3 ];
	process_name = argv[ 4 ];
	anchor_date = argv[ 5 ];
	anchor_time = argv[ 6 ];
	location = argv[ 7 ];
	site_number_string = argv[ 8 ];

	appaserver_parameter_file = new_appaserver_parameter_file();

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}

	sprintf( action_string,
		"%s/post_benthic_validation_form?%s+%s+%s+%s+%s+%s+%s+%s+%s",
		 appaserver_parameter_file->apache_cgi_directory,
		 original_application_name,
		 login_name,
		 session,
		 process_name,
		 anchor_date,
		 anchor_time,
		 location,
		 site_number_string,
		 TARGET_FRAME );

	sampling_point =
		sampling_point_new(
			anchor_date,
			anchor_time,
			location,
			atoi( site_number_string ) );

	if ( !sampling_point_load(
			&sampling_point->collection_name,
			&sampling_point->actual_latitude,
			&sampling_point->actual_longitude,
			&sampling_point->boat_lead_researcher,
			&sampling_point->second_boat_researcher,
			&sampling_point->collection_diver1,
			&sampling_point->collection_diver2,
			&sampling_point->begin_collection_dive_time,
			&sampling_point->end_collection_dive_time,
			&sampling_point->collection_air_pressure_in_PSI,
			&sampling_point->collection_air_pressure_out_PSI,
			&sampling_point->begin_habitat_dive_time,
			&sampling_point->end_habitat_dive_time,
			&sampling_point->habitat_diver,
			&sampling_point->habitat_air_pressure_in_PSI,
			&sampling_point->habitat_air_pressure_out_PSI,
			&sampling_point->notes,
			&sampling_point->bucket_number,
			&sampling_point->tide,
			&sampling_point->project,
			sampling_point,
			application_name ) )
	{
		document_quick_output_body(
			application_name,
			appaserver_parameter_file->appaserver_mount_point );
		printf( "<h3>Please select a sampling point.</h3>\n" );
		document_close();
		exit( 0 );
/*
		fprintf(stderr,
"ERROR in %s/%s()/%d: cannot seek sampling_point of (%s,%s,%s,%s)\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			anchor_date,
			anchor_time,
			location,
			site_number_string );
		exit( 1 );
*/
	}

	title = get_title_string(	sampling_point->collection_name,
					sampling_point->location,
					sampling_point->site_number );

	document = document_new( title, application_name );
	document_set_output_content_type( document );

	document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(
			document->application_name,
			document->onload_control_string );

	printf( "<h1>%s</h1>\n", title );
	fflush( stdout );

	output_benthic_validation_form(
			application_name,
			sampling_point,
			action_string,
			title );

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	exit( 0 );
}

void output_benthic_validation_form(
			char *application_name,
			SAMPLING_POINT *sampling_point,
			char *action_string,
			char *title )
{
	int quad;

	output_sampling_point( sampling_point );

	output_attempt_success(
			application_name,
			sampling_point->anchor_date,
			sampling_point->anchor_time,
			sampling_point->location,
			sampling_point->site_number );

	output_abiotic_measurement(
			application_name,
			sampling_point->anchor_date,
			sampling_point->anchor_time,
			sampling_point->location,
			sampling_point->site_number );

	for( quad = 1; quad <= MAX_QUAD_NUMBER; quad++ )
	{
		output_quad(
			application_name,
			sampling_point->anchor_date,
			sampling_point->anchor_time,
			sampling_point->location,
			sampling_point->site_number,
			quad );

		output_quad_vegetation(
			application_name,
			sampling_point->anchor_date,
			sampling_point->anchor_time,
			sampling_point->location,
			sampling_point->site_number,
			quad );

		output_quad_vegetation_total(
			application_name,
			sampling_point->anchor_date,
			sampling_point->anchor_time,
			sampling_point->location,
			sampling_point->site_number,
			quad );

		output_quad_substrate(
			application_name,
			sampling_point->anchor_date,
			sampling_point->anchor_time,
			sampling_point->location,
			sampling_point->site_number,
			quad );
	}

	output_throwtrap_replicate(
			application_name,
			sampling_point->anchor_date,
			sampling_point->anchor_time,
			sampling_point->location,
			sampling_point->site_number );

	printf( "<br>\n" );
	printf( "<hr>\n" );
	printf( "<hr>\n" );
	validation_form_output_heading(	title,
					action_string,
					TARGET_FRAME,
					1 /* table_border */ );

	validation_form_output_trailer();

}

char *get_title_string(	char *collection_name,
			char *location,
			int site_number )
{
	char static title[ 1024 ];

	sprintf(title,
		"Validate %s, %s/%d",
		collection_name,
		location,
		site_number );

	format_initial_capital( title, title );

	return title;
}

void output_abiotic_measurement(
			char *application_name,
			char *anchor_date,
			char *anchor_time,
			char *location,
			int site_number )
{
	int replicate_number;

	output_sampling_point_abiotic_measurement(
			application_name,
			anchor_date,
			anchor_time,
			location,
			site_number );

	for(	replicate_number = 1;
		replicate_number <= MAX_REPLICATE_NUMBER;
		replicate_number++ )
	{
		if ( exists_throwtrap_replicate(
				application_name,
				anchor_date,
				anchor_time,
				location,
				site_number,
				replicate_number ) )
		{

			output_throwtrap_abiotic_measurement(
					application_name,
					anchor_date,
					anchor_time,
					location,
					site_number,
					replicate_number );
		}
	}

}

void output_sampling_point_abiotic_measurement(
			char *application_name,
			char *anchor_date,
			char *anchor_time,
			char *location,
			int site_number )
{
	char *primary_key_where_clause;
	char sys_string[ 1024 ];
	char select[ 128 ];
	char *order = "display_order";
	char *folder = "sampling_point_abiotic_measurement,datatype";
	char where_join[ 128 ];
	char where_clause[ 1024 ];
	char *table_title = "Sampling Point Abiotic Measurements";

	sprintf( select,
		 "%s.datatype,value",
		 get_table_name(
			application_name,
			"sampling_point_abiotic_measurement" ) );
		 
	primary_key_where_clause =
		sampling_point_get_primary_key_where_clause(
			anchor_date,
			anchor_time,
			location,
			site_number );

	sprintf( where_join,
		 "%s.datatype = %s.datatype",
		 get_table_name(
			application_name,
			"sampling_point_abiotic_measurement" ),
		 get_table_name(
			application_name,
			"datatype" ) );

	sprintf( where_clause,
		 "%s and %s",
		 primary_key_where_clause,
		 where_join );

	sprintf( sys_string,
		 "get_folder_data	application=%s			 "
		 "			select=%s			 "
		 "			folder=%s			 "
		 "			where=\"%s\"			 "
		 "			order=%s			|"
		 "format_initial_capital.e				|"
		 "piece_sum.e '%c' 1					|"
		 "html_table.e '%s' datatype,value '%c' left,right	 ",
		 application_name,
		 select,
		 folder,
		 where_clause,
		 order,
		 FOLDER_DATA_DELIMITER,
		 table_title,
		 FOLDER_DATA_DELIMITER );

	fflush( stdout );
	system( sys_string );
}

void output_throwtrap_abiotic_measurement(
			char *application_name,
			char *anchor_date,
			char *anchor_time,
			char *location,
			int site_number,
			int replicate_number )
{
	char *primary_key_where_clause;
	char sys_string[ 1024 ];
	char select[ 128 ];
	char *order = "display_order";
	char *folder = "throwtrap_abiotic_measurement,datatype";
	char where_join[ 128 ];
	char where_clause[ 1024 ];
	char table_title[ 128 ];

	sprintf(
	table_title,
	"Throwtrap Abiotic Measurements for Sampling Point Replicate %d",
	replicate_number );

	sprintf( select,
		 "%s.datatype,value",
		 get_table_name(
			application_name,
			"throwtrap_abiotic_measurement" ) );
		 
	primary_key_where_clause =
		sampling_point_get_primary_key_where_clause(
			anchor_date,
			anchor_time,
			location,
			site_number );

	sprintf( where_join,
		 "%s.datatype = %s.datatype",
		 get_table_name(
			application_name,
			"throwtrap_abiotic_measurement" ),
		 get_table_name(
			application_name,
			"datatype" ) );

	sprintf( where_clause,
		 "%s and %s and replicate_number = %d",
		 primary_key_where_clause,
		 where_join,
		 replicate_number );

	sprintf( sys_string,
		 "get_folder_data	application=%s			 "
		 "			select=%s			 "
		 "			folder=%s			 "
		 "			where=\"%s\"			 "
		 "			order=%s			|"
		 "format_initial_capital.e				|"
		 "piece_sum.e '%c' 1					|"
		 "html_table.e '%s' datatype,value '%c' left,right	 ",
		 application_name,
		 select,
		 folder,
		 where_clause,
		 order,
		 FOLDER_DATA_DELIMITER,
		 table_title,
		 FOLDER_DATA_DELIMITER );

	fflush( stdout );
	system( sys_string );
}

void output_sampling_point(
			SAMPLING_POINT *sampling_point )
{
	FILE *output_pipe;
	char sys_string[ 1024 ];
	char *table_title = "Sampling Point";

	sprintf( sys_string,
		 "html_table.e '%s' datatype,value '%c' left,left",
		 table_title,
		 '|' );

	fflush( stdout );
	output_pipe = popen( sys_string, "w" );

	fprintf(	output_pipe,
			"Date|%s\n",
			sampling_point->anchor_date );

	fprintf(	output_pipe,
			"Time|%s\n",
			sampling_point->anchor_time );

	fprintf(	output_pipe,
			"Location|%s\n",
			sampling_point->location );

	fprintf(	output_pipe,
			"Site number|%d\n",
			sampling_point->site_number );

	fprintf(	output_pipe,
			"Project|%s\n",
			sampling_point->project );

	fprintf(	output_pipe,
			"Latitude|%s\n",
			sampling_point->actual_latitude );

	fprintf(	output_pipe,
			"Longitude|%s\n",
			sampling_point->actual_longitude );

	fprintf(	output_pipe,
			"Tide|%s\n",
			sampling_point->tide );

	fprintf(	output_pipe,
			"Boat Lead Researcher|%s\n",
			sampling_point->boat_lead_researcher );

	fprintf(	output_pipe,
			"Second Boat Researcher|%s\n",
			sampling_point->second_boat_researcher );

	fprintf(	output_pipe,
			"Collection Diver 1|%s\n",
			sampling_point->collection_diver1 );

	fprintf(	output_pipe,
			"Collection Diver 2|%s\n",
			sampling_point->collection_diver2 );

	fprintf(	output_pipe,
			"Begin Collection Dive Time|%s\n",
			sampling_point->begin_collection_dive_time );

	fprintf(	output_pipe,
			"End Collection Dive Time|%s\n",
			sampling_point->end_collection_dive_time );

	fprintf(	output_pipe,
			"Collection Air Pressure IN (PSI)|%s\n",
			sampling_point->collection_air_pressure_in_PSI );

	fprintf(	output_pipe,
			"Collection Air Pressure OUT (PSI)|%s\n",
			sampling_point->collection_air_pressure_out_PSI );

	fprintf(	output_pipe,
			"Habitat Diver|%s\n",
			sampling_point->habitat_diver );

	fprintf(	output_pipe,
			"Begin Habitat Dive Time|%s\n",
			sampling_point->begin_habitat_dive_time );

	fprintf(	output_pipe,
			"End Habitat Dive Time|%s\n",
			sampling_point->end_habitat_dive_time );

	fprintf(	output_pipe,
			"Habitat Air Pressure IN (PSI)|%s\n",
			sampling_point->habitat_air_pressure_in_PSI );

	fprintf(	output_pipe,
			"Habitat Air Pressure OUT (PSI)|%s\n",
			sampling_point->habitat_air_pressure_out_PSI );

	fprintf(	output_pipe,
			"Notes|%s\n",
			sampling_point->notes );

	fprintf(	output_pipe,
			"Bucket|%s\n",
			sampling_point->bucket_number );

	pclose( output_pipe );

}

void output_attempt_success(
			char *application_name,
			char *anchor_date,
			char *anchor_time,
			char *location,
			int site_number )
{
	char *primary_key_where_clause;
	char sys_string[ 1024 ];
	char *select = "sample_number,attempt_success";
	char *order = "sample_number";
	char *folder = "sampling_site_attempt_success";
	char *table_title = "Sampling Site Attempt Success";

	primary_key_where_clause =
		sampling_point_get_primary_key_where_clause(
			anchor_date,
			anchor_time,
			location,
			site_number );

	sprintf( sys_string,
		 "get_folder_data	application=%s			 "
		 "			select=%s			 "
		 "			folder=%s			 "
		 "			where=\"%s\"			 "
		 "			order=%s			|"
		 "format_initial_capital.e				|"
		 "html_table.e '%s' sample_number,result '%c' left,left	 ",
		 application_name,
		 select,
		 folder,
		 primary_key_where_clause,
		 order,
		 table_title,
		 FOLDER_DATA_DELIMITER );

	fflush( stdout );
	system( sys_string );
}

void output_quad(	char *application_name,
			char *anchor_date,
			char *anchor_time,
			char *location,
			int site_number,
			int quad )
{
	char *primary_key_where_clause;
	char where_clause[ 1024 ];
	char sys_string[ 1024 ];
	char *select;
	char *folder = "quad_replicate";
	char table_title[ 128 ];

	sprintf( table_title, "Quad Replicate %d", quad );;

	select = "quad,closest_to_trap_yn";

	primary_key_where_clause =
		sampling_point_get_primary_key_where_clause(
			anchor_date,
			anchor_time,
			location,
			site_number );

	sprintf( where_clause,
		 "%s and quad = %d",
		 primary_key_where_clause,
		 quad );

	sprintf( sys_string,
		 "get_folder_data	application=%s			 "
		 "			select=%s			 "
		 "			folder=%s			 "
		 "			where=\"%s\"			|"
		 "format_initial_capital.e				|"
		 "html_table.e '%s' '%s' '%c' left,left			 ",
		 application_name,
		 select,
		 folder,
		 where_clause,
		 table_title,
		 select,
		 FOLDER_DATA_DELIMITER );

	fflush( stdout );
	system( sys_string );
}

void output_quad_substrate(
			char *application_name,
			char *anchor_date,
			char *anchor_time,
			char *location,
			int site_number,
			int quad )
{
	char *primary_key_where_clause;
	char where_clause[ 1024 ];
	char sys_string[ 1024 ];
	char *select;
	char *order = "quad,substrate";
	char *folder = "quad_substrate";
	char table_title[ 128 ];

	sprintf( table_title, "Quad Substrate for Quad %d", quad );

	select = "quad,substrate";

	primary_key_where_clause =
		sampling_point_get_primary_key_where_clause(
			anchor_date,
			anchor_time,
			location,
			site_number );

	sprintf( where_clause,
		 "%s and quad = %d",
		 primary_key_where_clause,
		 quad );

	sprintf( sys_string,
		 "get_folder_data	application=%s			 "
		 "			select=%s			 "
		 "			folder=%s			 "
		 "			where=\"%s\"			 "
		 "			order=%s			|"
		 "format_initial_capital.e				|"
		 "html_table.e '^^%s' '%s' '%c' left,left	 	 ",
		 application_name,
		 select,
		 folder,
		 where_clause,
		 order,
		 table_title,
		 select,
		 FOLDER_DATA_DELIMITER );

	fflush( stdout );
	system( sys_string );
}

void output_species_count_sweep(
			char *application_name,
			char *anchor_date,
			char *anchor_time,
			char *location,
			int site_number,
			int replicate_number,
			int sweep_number )
{
	char *primary_key_where_clause;
	char sys_string[ 1024 ];
	char where_clause[ 1024 ];
	char *select;
	char *order = "family_name,genus_name,species_name";
	char *folder = "species_count";
	char table_title[ 128 ];
	char *justify = "left,left,left,right,right,right";

	sprintf(table_title,
		"Species Count for Throwtrap Replicate %d, Sweep %d",
		replicate_number,
		sweep_number );

	select =
"family_name,genus_name,species_name,count_per_square_meter,gravid_count,parasitic_isopods_count";

	primary_key_where_clause =
		sampling_point_get_primary_key_where_clause(
			anchor_date,
			anchor_time,
			location,
			site_number );

	sprintf( where_clause,
		 "%s and replicate_number = %d and sweep_number = %d",
		 primary_key_where_clause,
		 replicate_number,
		 sweep_number );

	sprintf( sys_string,
		 "get_folder_data	application=%s			 "
		 "			select=%s			 "
		 "			folder=%s			 "
		 "			where=\"%s\"			 "
		 "			order=%s			|"
		 "format_initial_capital.e				|"
		 "piece_sum.e '%c' 3,4,5				|"
		 "html_table.e '^^%s' '%s' '%c' %s			 ",
		 application_name,
		 select,
		 folder,
		 where_clause,
		 order,
		 FOLDER_DATA_DELIMITER,
		 table_title,
		 select,
		 FOLDER_DATA_DELIMITER,
		 justify );

	fflush( stdout );
	system( sys_string );
}

void output_quad_vegetation(
			char *application_name,
			char *anchor_date,
			char *anchor_time,
			char *location,
			int site_number,
			int quad )
{
	char *primary_key_where_clause;
	char sys_string[ 1024 ];
	char where_clause[ 1024 ];
	char *select;
	char *order = "quad,vegetation_name";
	char *folder = "quad_vegetation";
	char table_title[ 128 ];

	sprintf( table_title, "Quad Vegetation for Quad %d", quad );

	select = "quad,vegetation_name,abundance_rating,vegetation_height_cm";

	primary_key_where_clause =
		sampling_point_get_primary_key_where_clause(
			anchor_date,
			anchor_time,
			location,
			site_number );

	sprintf( where_clause,
		 "%s and quad = %d",
		 primary_key_where_clause,
		 quad );

	sprintf( sys_string,
		 "get_folder_data	application=%s			 "
		 "			select=%s			 "
		 "			folder=%s			 "
		 "			where=\"%s\"			 "
		 "			order=%s			|"
		 "format_initial_capital.e				|"
		 "piece_sum.e '%c' 2,3					|"
		 "html_table.e '^^%s' '%s' '%c' left,left		 ",
		 application_name,
		 select,
		 folder,
		 where_clause,
		 order,
		 FOLDER_DATA_DELIMITER,
		 table_title,
		 select,
		 FOLDER_DATA_DELIMITER );

	fflush( stdout );
	system( sys_string );
}

void output_quad_vegetation_total(
			char *application_name,
			char *anchor_date,
			char *anchor_time,
			char *location,
			int site_number,
			int quad )
{
	char *primary_key_where_clause;
	char where_clause[ 1024 ];
	char sys_string[ 1024 ];
	char *select;
	char *order = "quad,vegetation_name";
	char *folder = "quad_vegetation_total";
	char table_title[ 128 ];

	sprintf( table_title, "Quad Vegetation Total for Quad %d", quad );

	select = "quad,vegetation_name,abundance_rating";

	primary_key_where_clause =
		sampling_point_get_primary_key_where_clause(
			anchor_date,
			anchor_time,
			location,
			site_number );

	sprintf( where_clause,
		 "%s and quad = %d",
		 primary_key_where_clause,
		 quad );

	sprintf( sys_string,
		 "get_folder_data	application=%s			 "
		 "			select=%s			 "
		 "			folder=%s			 "
		 "			where=\"%s\"			 "
		 "			order=%s			|"
		 "format_initial_capital.e				|"
		 "piece_sum.e '%c' 2					|"
		 "html_table.e '^^%s' '%s' '%c' left,left		 ",
		 application_name,
		 select,
		 folder,
		 where_clause,
		 order,
		 FOLDER_DATA_DELIMITER,
		 table_title,
		 select,
		 FOLDER_DATA_DELIMITER );

	fflush( stdout );
	system( sys_string );
}

void output_sweep(
			char *application_name,
			char *anchor_date,
			char *anchor_time,
			char *location,
			int site_number,
			int replicate_number,
			int sweep_number )
{
	FILE *output_pipe;
	char sys_string[ 1024 ];
	SWEEP_LIST *sweep_list;
	SWEEP *sweep;
	char table_title[ 128 ];

	sprintf(table_title,
		"Replicate %d, Sweep %d",
		replicate_number,
		sweep_number );

	sweep_list =
		sweep_list_new(	
				application_name,
				anchor_date,
				anchor_time,
				location,
				site_number,
				replicate_number );

	sprintf( sys_string,
		 "html_table.e '%s' '%s' '%c' ''",
		 table_title,
		 list_display_delimited(
				sweep_list->non_primary_attribute_name_list,
				',' ),
		 '|' );

	fflush( stdout );
	output_pipe = popen( sys_string, "w" );

	if ( !list_rewind( sweep_list->sweep_list ) )
	{
		pclose( output_pipe );
		return;
	}

	do {
		sweep = list_get_pointer( sweep_list->sweep_list );

		if ( sweep->sweep_number == sweep_number )
		{
			fprintf(	output_pipe,
		"%d|%s|%s|%d|%d|%d|%d|%d|%d|%d|%d|%d|%s|%s|%s|%s|%s|%s\n",
					sweep->sweep_number,
					sweep->pick_date,
					sweep->quality_control_date,
					sweep->wash_time_minutes,
					sweep->pick_time_minutes,
					sweep->quality_control_time_minutes,
					sweep->sort_minutes,
					sweep->identify_penaeid_minutes,
					sweep->identify_fish_minutes,
					sweep->identify_caridean_minutes,
					sweep->identify_crabs_minutes,
					sweep->total_minutes,
					sweep->pick_researcher,
					sweep->quality_control_researcher,
					sweep->laboratory_researcher,
					sweep->no_animals_found_yn,
					sweep->tag_missing_yn,
					sweep->notes );
		}

	} while( list_next( sweep_list->sweep_list ) );

	pclose( output_pipe );

}

void output_species_measurement_sweep(
			char *application_name,
			char *anchor_date,
			char *anchor_time,
			char *location,
			int site_number,
			int replicate_number,
			int sweep_number )
{
	char *primary_key_where_clause;
	char sys_string[ 1024 ];
	char where_clause[ 1024 ];
	char *select;
	char *order = "family_name,genus_name,species_name";
	char *folder = "species_measurement";
	char table_title[ 128 ];
	char *justify = "left,left,left,left,right,left";

	sprintf(	table_title,
			"Species Measurement for Replicate %d, Sweep %d",
			replicate_number,
			sweep_number );

	select =
"family_name,genus_name,species_name,measurement_number,length_millimeters,gender,damaged_yn";

	primary_key_where_clause =
		sampling_point_get_primary_key_where_clause(
			anchor_date,
			anchor_time,
			location,
			site_number );

	sprintf( where_clause,
		 "%s and replicate_number = %d and sweep_number = %d",
		 primary_key_where_clause,
		 replicate_number,
		 sweep_number );

	sprintf( sys_string,
		 "get_folder_data	application=%s			 "
		 "			select=%s			 "
		 "			folder=%s			 "
		 "			where=\"%s\"			 "
		 "			order=%s			|"
		 "format_initial_capital.e				|"
		 "piece_sum.e '%c' 4					|"
		 "html_table.e '^^%s' '%s' '%c' %s			 ",
		 application_name,
		 select,
		 folder,
		 where_clause,
		 order,
		 FOLDER_DATA_DELIMITER,
		 table_title,
		 select,
		 FOLDER_DATA_DELIMITER,
		 justify );

	fflush( stdout );
	system( sys_string );
}

void validation_form_output_heading(	char *title,
					char *action_string,
					char *target_frame,
					int table_border )
{
	printf( "<h1>%s</h1>\n", title );

	/* Output form tag */
	/* --------------- */
	printf( "<form method=post" );
	printf( " action=%s", action_string );
	printf( " target=\"%s\"", target_frame );
	printf( ">\n" );

	/* Output table tag */
	/* ---------------- */
	printf(
"	<table cellspacing=0 cellpadding=0" );

	if ( table_border ) printf( " border" );

	printf( ">\n" );

}

void validation_form_output_trailer( void )
{
	printf( "<tr><td>" );
	form_output_submit_button(
			(char *)0,
			"Validate",
			0 /* form_number */,
			(LIST *)0 /* pair_one2m_related_folder_name_list */ );
	printf( "</table></form>\n" );
}

boolean exists_throwtrap_replicate(
			char *application_name,
			char *anchor_date,
			char *anchor_time,
			char *location,
			int site_number,
			int replicate_number )
{
	char *primary_key_where_clause;
	char sys_string[ 1024 ];
	char *folder = "throwtrap_replicate";
	char where_clause[ 1024 ];

	primary_key_where_clause =
		sampling_point_get_primary_key_where_clause(
			anchor_date,
			anchor_time,
			location,
			site_number );

	sprintf( where_clause,
		 "%s and replicate_number = %d",
		 primary_key_where_clause,
		 replicate_number );

	sprintf( sys_string,
		 "get_folder_data	application=%s			 "
		 "			select=count			 "
		 "			folder=%s			 "
		 "			where=\"%s\"			 ",
		 application_name,
		 folder,
		 where_clause );

	return atoi( pipe2string( sys_string ) );

}

void output_throwtrap_replicate(
			char *application_name,
			char *anchor_date,
			char *anchor_time,
			char *location,
			int site_number )
{
	int replicate_number;
	int sweep_number;

	for(	replicate_number = 1;
		replicate_number <= MAX_REPLICATE_NUMBER;
		replicate_number++ )
	{
		if ( !exists_throwtrap_replicate(
			application_name,
			anchor_date,
			anchor_time,
			location,
			site_number,
			replicate_number ) )
		{
			continue;
		}

		for(	sweep_number = 1;
			sweep_number <= MAX_SWEEP_NUMBER;
			sweep_number++ )
		{
			output_sweep(
				application_name,
				anchor_date,
				anchor_time,
				location,
				site_number,
				replicate_number,
				sweep_number );
	
			output_species_count_sweep(
				application_name,
				anchor_date,
				anchor_time,
				location,
				site_number,
				replicate_number,
				sweep_number );
	
			output_species_measurement_sweep(
				application_name,
				anchor_date,
				anchor_time,
				location,
				site_number,
				replicate_number,
				sweep_number );
		}
	}

}

