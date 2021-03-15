/* --------------------------------------------------- 	*/
/* $APPASERVER_HOME/utility/google_timechart.c	       	*/
/* --------------------------------------------------- 	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "timlib.h"
#include "application.h"
#include "piece.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "appaserver_error.h"
#include "google_chart.h"
#include "appaserver_link_file.h"
#include "list.h"
#include "environ.h"

/* Constants */
/* --------- */
#define FILENAME_STEM		"google_timeline"

/* Prototypes */
/* ---------- */
boolean populate_point_array(	LIST *timeline_list,
				LIST *datatype_name_list,
				char delimiter );

int main( int argc, char **argv )
{
	char *application_name;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *output_filename;
	char *prompt_filename;
	FILE *output_file;
	char *title;
	char *subtitle;
	GOOGLE_OUTPUT_CHART *google_output_chart;
	LIST *datatype_name_list;
	char chart_title[ 128 ];
	char delimiter;

	if ( argc != 6 )
	{
		fprintf(stderr,
"Usage: %s application title subtitle datatype_name[^datatype_name...] delimiter\n",
			argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	title = argv[ 2 ];
	subtitle = argv[ 3 ];
	datatype_name_list = list_string2list( argv[ 4 ], '^' );
	delimiter = *argv[ 5 ];

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	appaserver_parameter_file = appaserver_parameter_file_new();

	appaserver_link_get_pid_filename(
			&output_filename,
			&prompt_filename,
			application_name,
			appaserver_parameter_file->document_root,
			getpid(),
			FILENAME_STEM,
			"html" /* extension */ );

	output_file = fopen( output_filename, "w" );

	if ( !output_file )
	{
		fprintf(stderr,
			"ERROR in %s/%s(): cannot open %s\n",
			__FILE__,
			__FUNCTION__,
			output_filename );
		exit( 1 );
	}

	google_output_chart =
		google_output_chart_new(
			GOOGLE_CHART_POSITION_LEFT,
			GOOGLE_CHART_POSITION_TOP,
			GOOGLE_CHART_WIDTH,
			GOOGLE_CHART_HEIGHT );

	google_output_chart->datatype_name_list = datatype_name_list;

	if ( !populate_point_array(
			google_output_chart->timeline_list,
			google_output_chart->datatype_name_list,
			delimiter ) )
	{
		exit( 1 );
	}

	if ( !*title && !*subtitle )
	{
		*chart_title = '\0';
	}
	else
	if ( *title && !*subtitle )
	{
		strcpy( chart_title, title );
	}
	else
	{
		sprintf( chart_title, "%s %s", title, subtitle );
	}

	document_output_html_stream( output_file );

	fprintf( output_file, "<head>\n" );

	document_output_stylesheet(
		output_file,
		application_name,
		"style.css" );

	google_chart_include( output_file );

	google_chart_output_visualization_annotated(
		output_file,
		google_output_chart->google_chart_type,
		google_output_chart->timeline_list,
		google_output_chart->barchart_list,
		google_output_chart->datatype_name_list,
		GOOGLE_ANNOTATED_TIMELINE
			/* google_package_name */,
		aggregate_level_none,
		google_output_chart->chart_number,
		chart_title,
		(char *)0 /* yaxis_label */ );

#ifdef NOT_DEFINED
	google_chart_output_visualization_non_annotated(
		output_file,
		google_output_chart->google_chart_type,
		google_output_chart->timeline_list,
		google_output_chart->barchart_list,
		google_output_chart->datatype_name_list,
		chart_title,
		(char *)0 /* yaxis_label */,
		google_output_chart->width,
		google_output_chart->height,
		google_output_chart->background_color,
		google_output_chart->legend_position_bottom,
		0 /* not chart_type_bar */,
		google_output_chart->google_package_name,
		0 /* not dont_display_range_selector */,
		aggregate_level_none,
		google_output_chart->chart_number );
#endif

	fprintf( output_file, "</head>\n" );
	fprintf( output_file, "<body>\n" );

	google_chart_float_chart(
				output_file,
				chart_title,
				google_output_chart->width,
				google_output_chart->height,
				google_output_chart->chart_number );

	google_chart_output_chart_instantiation(
		output_file,
		google_output_chart->chart_number );

	fprintf( output_file, "</body>\n" );
	fprintf( output_file, "</html>\n" );

	fclose( output_file );

	printf(	"%s %s\n",
		prompt_filename,
		output_filename );

	return 0;

}

boolean populate_point_array(
			LIST *timeline_list,
			LIST *datatype_name_list,
			char delimiter )
{
	char input_buffer[ 1024 ];
	boolean got_one = 0;

	while( get_line( input_buffer, stdin ) )
	{
		got_one = 1;

		google_timeline_set_point_string(
			timeline_list,
			datatype_name_list,
			input_buffer,
			delimiter );
	}

	return got_one;

}

