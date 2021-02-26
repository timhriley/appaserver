/* src_sparrow/sparrow_validation_form.c				*/
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
#include "site_visit.h"
#include "sparrow_observation.h"
#include "physical_observation.h"
#include "vegetation_observation.h"
#include "sparrow_library.h"

/* Constants */
/* --------- */
#define TARGET_FRAME		EDIT_FRAME

/* Prototypes */
/* ---------- */
void output_site_visit(			SITE_VISIT *site_visit );

void output_sparrow_observation_list(	LIST *sparrow_observation_list );

void output_physical_observation_list(	LIST *physical_observation_list );

void output_vegetation_observation_list(LIST *physical_observation_list );

void validation_form_output_heading(	char *title,
					char *action_string,
					char *target_frame,
					int table_border );

void validation_form_output_trailer(	void );

void output_sparrow_validation_form(
					SITE_VISIT *site_visit,
					char *action_string,
					char *title );

int main( int argc, char **argv )
{
	char *application_name;
	char original_application_name[ 128 ];
	char *process_name;
	char *login_name;
	char *session;
	char *quad_sheet;
	int site_number;
	char *visit_date;
	SITE_VISIT *site_visit;
	char *title;
	char action_string[ 1024 ];
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 8 )
	{
		fprintf( stderr, 
"Usage: %s ignored process session login_name quad_sheet site_number visit_date\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	strcpy( original_application_name, application_name );

	process_name = argv[ 2 ];
	session = argv[ 3 ];
	login_name = argv[ 4 ];
	quad_sheet = argv[ 5 ];
	site_number = atoi( argv[ 6 ] );
	visit_date = argv[ 7 ];

	appaserver_parameter_file = appaserver_parameter_file_new();

	sprintf( action_string,
		"%s/post_sparrow_validation_form?%s+%s+%s+%s+%d+%s",
		 appaserver_parameter_file->apache_cgi_directory,
		 original_application_name,
		 login_name,
		 session,
		 quad_sheet,
		 site_number,
		 visit_date );

	site_visit =
		site_visit_new(
			quad_sheet,
			site_number,
			visit_date );

	if ( !site_visit_load(
			&site_visit->visit_time,
			&site_visit->survey_year,
			&site_visit->survey_number,
			&site_visit->observer,
			&site_visit->total_vegetation_cover_percent,
			&site_visit->reason_no_observations,
			&site_visit->validated_yn,
			&site_visit->person_validating,
			&site_visit->date_validated,
			&site_visit->comments,
			&site_visit->field_sheet,
			site_visit,
			application_name ) )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: cannot load site_visit of (%s,%d,%s)\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			site_visit->quad_sheet,
			site_visit->site_number,
			site_visit->visit_date );
		exit( 1 );
	}

	site_visit->sparrow_observation_list =
		sparrow_observation_get_list(
			application_name,
			site_visit->quad_sheet,
			site_visit->site_number,
			site_visit->visit_date );

	site_visit->physical_observation_list =
		physical_observation_get_list(
			application_name,
			site_visit->quad_sheet,
			site_visit->site_number,
			site_visit->visit_date );

	site_visit->vegetation_observation_list =
		vegetation_observation_get_list(
			application_name,
			site_visit->quad_sheet,
			site_visit->site_number,
			site_visit->visit_date );

	title = sparrow_library_get_title_string(
					site_visit->quad_sheet,
					site_visit->site_number,
					site_visit->visit_date );

	document = document_new( title, application_name );
	document_set_output_content_type( document );

	document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(
			document->application_name,
			document->onload_control_string );

	printf( "<h1>%s</h1>\n", title );
	fflush( stdout );

	output_sparrow_validation_form(
			site_visit,
			action_string,
			title );

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	exit( 0 );
}

void output_sparrow_validation_form(
			SITE_VISIT *site_visit,
			char *action_string,
			char *title )
{
	output_site_visit( site_visit );

	output_sparrow_observation_list(
		site_visit->sparrow_observation_list );

	output_physical_observation_list(
		site_visit->physical_observation_list );

	output_vegetation_observation_list(
		site_visit->vegetation_observation_list );

	printf( "<br>\n" );
	printf( "<hr>\n" );
	printf( "<hr>\n" );
	validation_form_output_heading(	title,
					action_string,
					TARGET_FRAME,
					1 /* table_border */ );

	validation_form_output_trailer();

}

void validation_form_output_heading(	char *title,
					char *action_string,
					char *target_frame,
					int table_border )
{
	printf( "<h3>%s</h3>\n", title );

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
			0 /* form_number */ );

	printf( "</table></form>\n" );
}

void output_site_visit( SITE_VISIT *site_visit )
{
	FILE *output_pipe;
	char sys_string[ 1024 ];
	char *table_title = "Site Visit";
	char buffer[ 128 ];

	sprintf( sys_string,
		 "html_table.e '^^%s' datatype,value '%c' left,left",
		 table_title,
		 '|' );

	fflush( stdout );
	output_pipe = popen( sys_string, "w" );

	fprintf(	output_pipe,
			"Quad sheet|%s\n",
			format_initial_capital(
				buffer,
				site_visit->quad_sheet ) );

	fprintf(	output_pipe,
			"Site number|%d\n",
			site_visit->site_number );

	fprintf(	output_pipe,
			"Visit date|%s\n",
			site_visit->visit_date );

	fprintf(	output_pipe,
			"Visit time|%s\n",
			site_visit->visit_time );

	fprintf(	output_pipe,
			"Survey year|%d\n",
			site_visit->survey_year );

	fprintf(	output_pipe,
			"Survey number|%d\n",
			site_visit->survey_number );

	fprintf(	output_pipe,
			"Observer|%s\n",
			site_visit->observer );

	fprintf(	output_pipe,
			"Total vegetation cover percent|%d\n",
			site_visit->total_vegetation_cover_percent );

	fprintf(	output_pipe,
			"Reason no observations|%s\n",
			format_initial_capital(
				buffer,
				site_visit->reason_no_observations ) );

	fprintf(	output_pipe,
			"Comments|%s\n",
			site_visit->comments );

	fprintf(	output_pipe,
			"Field sheet|%s\n",
			site_visit->field_sheet );

	pclose( output_pipe );

}

void output_sparrow_observation_list( LIST *sparrow_observation_list )
{
	FILE *output_pipe;
	char sys_string[ 1024 ];
	char *table_title = "Sparrow Observation";
	SPARROW_OBSERVATION *sparrow_observation;
	char *heading;
	char *justification;

	heading = "distance_from_observer_m,azimuth_degrees,bird_count";
	justification = "right,right,right";

	sprintf( sys_string,
		 "html_table.e '^^%s' '%s', '%c' '%s'",
		 table_title,
		 heading,
		 '|',
		 justification );

	fflush( stdout );
	output_pipe = popen( sys_string, "w" );

	if ( !list_rewind( sparrow_observation_list ) )
	{
		pclose( output_pipe );
		return;
	}

	do {
		sparrow_observation =
			list_get_pointer(
				sparrow_observation_list );

		fprintf(	output_pipe,
				"%s|%s|%d\n",
				sparrow_observation->distance_from_observer_m,
				sparrow_observation->azimuth_degrees,
				sparrow_observation->bird_count );

	} while( list_next( sparrow_observation_list ) );

	pclose( output_pipe );

}

void output_physical_observation_list( LIST *physical_observation_list )
{
	FILE *output_pipe;
	char sys_string[ 1024 ];
	char *table_title = "Physical Observation";
	PHYSICAL_OBSERVATION *physical_observation;
	char *heading;
	char *justification;
	char buffer[ 128 ];

	heading = "physical_parameter,measurement_number,measurement_value";
	justification = "left,right,right";

	sprintf( sys_string,
		 "html_table.e '^^%s' '%s', '%c' '%s'",
		 table_title,
		 heading,
		 '|',
		 justification );

	fflush( stdout );
	output_pipe = popen( sys_string, "w" );

	if ( !list_rewind( physical_observation_list ) )
	{
		pclose( output_pipe );
		return;
	}

	do {
		physical_observation =
			list_get_pointer(
				physical_observation_list );

		fprintf(	output_pipe,
				"%s|%d|%.3lf\n",
				format_initial_capital(
					buffer,
					physical_observation->
						physical_parameter ),
				physical_observation->measurement_number,
				physical_observation->measurement_value );

	} while( list_next( physical_observation_list ) );

	pclose( output_pipe );

}

void output_vegetation_observation_list( LIST *vegetation_observation_list )
{
	FILE *output_pipe;
	char sys_string[ 1024 ];
	char *table_title = "Vegetation Observation";
	VEGETATION_OBSERVATION *vegetation_observation;
	char *heading;
	char *justification;
	char buffer[ 128 ];

	heading = "vegetation_name,predominance_rank";
	justification = "left,right";

	sprintf( sys_string,
		 "html_table.e '^^%s' '%s', '%c' '%s'",
		 table_title,
		 heading,
		 '|',
		 justification );

	fflush( stdout );
	output_pipe = popen( sys_string, "w" );

	if ( !list_rewind( vegetation_observation_list ) )
	{
		pclose( output_pipe );
		return;
	}

	do {
		vegetation_observation =
			list_get_pointer(
				vegetation_observation_list );

		fprintf(	output_pipe,
				"%s|%d\n",
				format_first_initial_capital(
					buffer,
					vegetation_observation->
						vegetation_name ),
				vegetation_observation->predominance_rank );

	} while( list_next( vegetation_observation_list ) );

	pclose( output_pipe );

}

