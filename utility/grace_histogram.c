/* --------------------------------------------------- 	*/
/* $APPASERVER_HOME/utility/grace_histogram.c	       	*/
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
#include "grace.h"
#include "histogram.h"
#include "session.h"
#include "list.h"
#include "environ.h"

/* Constants */
/* --------- */
#define DEFAULT_SYMBOL_SIZE_SCALE	30.0

/* Prototypes */
/* ---------- */
void populate_number_double_list( LIST *number_double_list );

int main( int argc, char **argv )
{
	char *application_name;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *ftp_agr_filename;
	char *ftp_output_filename;
	char *output_filename;
	char *agr_filename;
	char *postscript_filename;
	char *title;
	char *subtitle;
	GRACE *grace = {0};
	char graph_identifier[ 128 ];
	int page_width_pixels;
	int page_length_pixels;
	char *distill_landscape_flag;
	char *session;
	HISTOGRAM *histogram;
	HISTOGRAM_SEGMENT *segment;
	GRACE_GRAPH *grace_graph;
	char optional_label[ 128 ] = {0};
	char *label;
	float symbol_size_denominator;
	int label_toggle = 0;
	char count_string[ 128 ];
	char *datatype_name;
	char y_axis[ 128 ];

	if ( argc < 6 )
	{
		fprintf(stderr,
"Usage: %s application session title subtitle label [datatype_name]\n",
			argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	session = argv[ 2 ];
	title = argv[ 3 ];
	subtitle = argv[ 4 ];
	label = argv[ 5 ];

	if ( argc == 7 )
		datatype_name = argv[ 6 ];
	else
		datatype_name = (char *)0;

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

	appaserver_parameter_file = appaserver_parameter_file_new();

	histogram = histogram_new_histogram();

	populate_number_double_list( histogram->number_double_list );

	if ( !list_length( histogram->number_double_list ) )
		exit( 1 );

	histogram->segment_list =
		histogram_get_segment_list(
					&histogram->number_of_segments,
					histogram->number_double_list );

	histogram_count(	histogram->segment_list,
				histogram->number_double_list );

/*
histogram_segment_list_display( histogram->segment_list );
*/

	histogram_soft_delete_empty_edges(
		&histogram->number_of_segments,
		histogram->segment_list );

	if ( datatype_name )
	{
		sprintf( y_axis, "%s Frequency", datatype_name );
	}
	else
	{
		strcpy( y_axis, "Frequency" );
	}

	grace = grace_new_xy_grace(
				(char *)0 /* application_name */,
				(char *)0 /* role_name */,
				title,
				subtitle,
				y_axis,
				"" /* legend */ );

	grace->xaxis_ticklabel_precision = 0;

	grace->world_min_x =
		histogram_get_lowest_top_bound(
			histogram->segment_list ) - 0.3;

	grace->world_max_x =
		histogram_get_highest_top_bound(
			histogram->segment_list ) + 0.3;

	if ( datatype_name )
	{
		sprintf(	graph_identifier,
				"%s_%s",
				datatype_name,
				session );
	}
	else
	{
		strcpy( graph_identifier, session );
	}

	grace->grace_output =
		application_grace_output( application_name );

	grace_get_filenames(
			&agr_filename,
			&ftp_agr_filename,
			&postscript_filename,
			&output_filename,
			&ftp_output_filename,
			application_name,
			appaserver_parameter_file->
				document_root,
			graph_identifier,
			grace->grace_output );

	list_rewind( histogram->segment_list );

	do {
		segment =
			list_get_pointer( 
				histogram->segment_list );

		if ( segment->soft_delete ) continue;

		if ( histogram->number_of_segments > 50 )
		{
			label_toggle = 1 - label_toggle;
		}
		else
		{
			label_toggle = 1;
		}

		if ( label_toggle )
		{
			sprintf( optional_label,
			 	"%.1lf < %.1lf",
			 	segment->bottom_bound,
			 	segment->top_bound );
		}
		else
		{
			*optional_label = '\0';
		}

		sprintf( count_string, "%d", segment->count );

		grace_set_xy_to_point_list(
				grace->graph_list, 
				segment->top_bound,
				strdup( count_string ),
				strdup( optional_label ),
				grace->dataset_no_cycle_color );

	} while( list_next( histogram->segment_list ) );

	grace_set_bar_graph( grace->graph_list );
	grace_set_scale_to_zero( grace->graph_list );
	grace_set_xaxis_ticklabel_angle( grace->graph_list, 45 );
	grace_set_yaxis_ticklabel_precision( grace->graph_list, 0 );

	symbol_size_denominator =
		(histogram->number_of_segments == 1)
			? 2.0
			: (float)histogram->number_of_segments;

	grace_set_symbol_size(	grace->graph_list,
				DEFAULT_SYMBOL_SIZE_SCALE /
				symbol_size_denominator );

	grace_graph =
		(GRACE_GRAPH *)
			list_get_first_pointer(
				grace->graph_list );
	grace_graph->xaxis_label = label;

	grace_set_structures(
				&page_width_pixels,
				&page_length_pixels,
				&distill_landscape_flag,
				&grace->landscape_mode,
				grace,
				grace->graph_list,
				grace->anchor_graph_list,
				grace->begin_date_julian,
				grace->end_date_julian,
				grace->number_of_days,
				grace->grace_graph_type,
				0 /* not force_landscape_mode */ );

	grace_set_x_minor_tickmarks_to_zero( grace->graph_list );
	grace_set_y_minor_tickmarks_to_zero( grace->graph_list );
	grace_set_xaxis_tickmarks_off( grace->graph_list );
	grace_set_yaxis_tickmajor_integer_ceiling( grace->graph_list );

	grace_output_charts(
				output_filename, 
				postscript_filename,
				agr_filename,
				grace->title,
				grace->sub_title,
				grace->xaxis_ticklabel_format,
				grace->grace_graph_type,
				grace->x_label_size,
				page_width_pixels,
				page_length_pixels,
				application_grace_home_directory(
					application_name ),
				application_grace_execution_directory(
					application_name ),
				application_grace_free_option_yn(
					application_name ),
				grace->grace_output,
				application_distill_directory(
					application_name ),
				distill_landscape_flag,
				application_ghost_script_directory(
					application_name ),
				grace_get_optional_label_list(
					grace->graph_list ),
				grace->symbols,
				grace->world_min_x,
				grace->world_max_x,
				grace->xaxis_ticklabel_precision,
				grace->graph_list,
				grace->anchor_graph_list );

	printf(	"%s %s %s\n",
		ftp_agr_filename,
		ftp_output_filename,
		output_filename );

	return 0;

} /* main() */

void populate_number_double_list( LIST *number_double_list )
{
	char input_buffer[ 128 ];
	double *double_pointer;

	while( get_line( input_buffer, stdin ) )
	{
		if ( !*input_buffer ) continue;

		if ( *input_buffer != '-'
		&&   !isdigit( *input_buffer ) )
		{
			continue;
		}

		double_pointer = (double *)calloc( 1, sizeof( double ) );
		if ( !double_pointer )
		{
			fprintf(stderr,
				"Error in %s/%s(): memory allocation failed\n",
				__FILE__,
				__FUNCTION__ );
			exit( 1 );
		}
		*double_pointer = atof( input_buffer );
		list_append_pointer( number_double_list, double_pointer );
	}
} /* populate_number_double_list() */

