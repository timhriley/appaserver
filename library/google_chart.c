/* $APPASERVER_HOME/library/google_chart.c		*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "date.h"
#include "google_chart.h"
#include "application.h"
#include "appaserver_library.h"
#include "appaserver_link_file.h"
#include "document.h"

GOOGLE_CHART *google_chart_new( void )
{
	GOOGLE_CHART *g;

	g = (GOOGLE_CHART *)calloc( 1, sizeof( GOOGLE_CHART ) );

	if ( !g )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return g;

} /* google_chart_new() */

GOOGLE_OUTPUT_CHART *google_output_chart_new(
				int left,
				int top,
				int width,
				int height )
{
	GOOGLE_OUTPUT_CHART *google_output_chart;
	static int chart_number = 0;

	if ( ! ( google_output_chart =
			(GOOGLE_OUTPUT_CHART *)
				calloc( 1, sizeof( GOOGLE_OUTPUT_CHART ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	google_output_chart->google_chart_type = google_time_line;
	google_output_chart->left = left;
	google_output_chart->top = top;
	google_output_chart->width = width;
	google_output_chart->height = height;
	google_output_chart->barchart_list = list_new();
	google_output_chart->timeline_list = list_new();
	google_output_chart->datatype_name_list = list_new();
	google_output_chart->google_package_name = GOOGLE_ANNOTATED_TIMELINE;
	google_output_chart->background_color = GOOGLE_CHART_BACKGROUND_COLOR;
	google_output_chart->chart_number = ++chart_number;

	return google_output_chart;

} /* google_output_chart_new() */

GOOGLE_INPUT_VALUE *google_chart_input_value_new(
					char *date_time )
{
	GOOGLE_INPUT_VALUE *g;

	g = (GOOGLE_INPUT_VALUE *)calloc( 1, sizeof( GOOGLE_INPUT_VALUE ) );

	if ( !g )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	g->date_time = date_time;

	return g;

} /* google_chart_input_value_new() */

GOOGLE_BARCHART *google_barchart_new(
				char *stratum_name,
				int length_datatype_name_list )
{
	GOOGLE_BARCHART *google_barchart;

	google_barchart =
		(GOOGLE_BARCHART *)
			calloc( 1, sizeof( GOOGLE_BARCHART ) );

	if ( !google_barchart )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	google_barchart->stratum_name = stratum_name;

	google_barchart->point_array =
		google_point_array_double_calloc(
			length_datatype_name_list );

/*
	if ( ! ( google_barchart->point_array =
			calloc(	length_datatype_name_list,
				sizeof( double * ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
*/

	return google_barchart;

} /* google_barchart_new() */

GOOGLE_TIMELINE *google_timeline_new(
				char *date_string,
				char *time_hhmm,
				int length_datatype_name_list )
{
	GOOGLE_TIMELINE *google_timeline;

	google_timeline =
		(GOOGLE_TIMELINE *)
			calloc( 1, sizeof( GOOGLE_TIMELINE ) );

	if ( !google_timeline )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	google_timeline->date_string = date_string;
	google_timeline->time_hhmm = time_hhmm;

	google_timeline->point_array =
		google_point_array_double_calloc(
			length_datatype_name_list );

/*
	if ( ! ( google_timeline->point_array =
			calloc(	length_datatype_name_list,
				sizeof( double * ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
*/

	return google_timeline;

} /* google_timeline_new() */

void google_barchart_append_datatype_name_string(
				LIST *barchart_list,
				char *datatype_name_list_string,
				int length_datatype_name_list,
				char delimiter )
{
	char datatype_name[ 128 ];
	int offset;

	for(	offset = 0;
		piece(	datatype_name,
			delimiter,
			datatype_name_list_string,
			offset );
		offset++ )
	{
		google_barchart_append(
			barchart_list,
			strdup( datatype_name ),
			length_datatype_name_list );
	}

} /* google_barchart_append_datatype_name_string() */

GOOGLE_BARCHART *google_barchart_append(
				LIST *barchart_list,
				char *stratum_name,
				int length_datatype_name_list )
{
	GOOGLE_BARCHART *barchart;

	barchart = google_barchart_new(
				stratum_name,
				length_datatype_name_list );

	list_append_pointer( barchart_list, barchart );

	return barchart;

} /* google_barchart_append() */

GOOGLE_TIMELINE *google_timeline_append(
				LIST *timeline_list,
				char *date_string,
				char *time_hhmm,
				int length_datatype_name_list )
{
	GOOGLE_TIMELINE *timeline;

	timeline = google_timeline_new(
				date_string,
				time_hhmm,
				length_datatype_name_list );

	list_append_pointer( timeline_list, timeline );

	return timeline;

} /* google_timeline_append() */

int google_chart_get_datatype_offset(
				LIST *datatype_name_list,
				char *datatype_name )
{
	int offset = 0;

	if ( !list_rewind( datatype_name_list ) ) return -1;

	do {
		if ( timlib_strcmp(
				list_get_pointer( datatype_name_list ),
				datatype_name ) == 0 )
		{
			return offset;
		}
		offset++;
	} while( list_next( datatype_name_list ) );

	return -1;

} /* google_chart_get_datatype_offset() */

GOOGLE_BARCHART *google_barchart_get_or_set(
				LIST *barchart_list,
				char *stratum_name,
				int length_datatype_name_list )
{
	GOOGLE_BARCHART *barchart;

	if ( length_datatype_name_list
	&&   list_rewind( barchart_list ) )
	{
		do {
			barchart = list_get_pointer( barchart_list );

			if ( timlib_strcmp(	barchart->stratum_name,
						stratum_name ) == 0 )
			{
				return barchart;
			}

		} while( list_next( barchart_list ) );
	}

	return google_barchart_append(
			barchart_list,
			strdup( stratum_name ),
			length_datatype_name_list );

} /* google_barchart_get_or_set() */

GOOGLE_TIMELINE *google_timeline_get_or_set(
				LIST *timeline_list,
				char *date_string,
				char *time_hhmm,
				int length_datatype_name_list )
{
	GOOGLE_TIMELINE *timeline;

	if ( length_datatype_name_list
	&&   list_rewind( timeline_list ) )
	{
		do {
			timeline = list_get_pointer( timeline_list );

			if ( timlib_strcmp(	timeline->date_string,
						date_string ) == 0 )
			{
				if ( !time_hhmm || !*time_hhmm )
					return timeline;

				if ( timlib_strcmp(
					timeline->time_hhmm,
					time_hhmm ) == 0 )
				{
					return timeline;
				}
			}

		} while( list_next( timeline_list ) );
	}

	return google_timeline_append(
			timeline_list,
			strdup( date_string ),
			(time_hhmm) ? strdup( time_hhmm ) : (char *)0,
			length_datatype_name_list );

} /* google_timeline_get_or_set() */

/* ==============================
   Expecting:
	2017-04-19:1445|stage|2.5
     or 2017-04-19|stage|2.5
     or 2017-04-19||2.5
     or 2017-04-19|2.5
  
   ============================== */
void google_timeline_set_point_string(	LIST *timeline_list,
					LIST *datatype_name_list,
					char *delimited_string,
					char delimiter )
{
	char date_string[ 128 ];
	char time_hhmm[ 128 ];
	char datatype_name[ 128 ];
	char point_string[ 128 ];
	char buffer[ 128 ];

	if ( !delimited_string || !*delimited_string ) return;

	if ( !character_count( delimiter, delimited_string ) )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: no delimiter of (%c) in (%s)\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			delimiter,
			delimited_string );
		return;
	}

	piece( date_string, delimiter, delimited_string, 0 );

	if ( character_count( ':', date_string ) == 1 )
	{
		piece( time_hhmm, ':', date_string, 1 );

		if ( strlen( time_hhmm ) != 4 ) *time_hhmm = '\0';

		timlib_strcpy( buffer, date_string, 128 );
		piece( date_string, ':', buffer, 0 );
	}
	else
	{
		*time_hhmm = '\0';
	}

	if ( character_count( delimiter, delimited_string ) == 1 )
	{
		piece( point_string, delimiter, delimited_string, 1 );
		*datatype_name = '\0';
	}
	else
	{
		piece( datatype_name, delimiter, delimited_string, 1 );
		piece( point_string, delimiter, delimited_string, 2 );
	}

	google_timeline_set_point(	timeline_list,
					datatype_name_list,
					date_string,
					time_hhmm,
					datatype_name,
					atof( point_string ) );

} /* google_timeline_set_point_string() */

void google_barchart_set_point(		LIST *barchart_list,
					char *stratum_name,
					LIST *datatype_name_list,
					char *datatype_name,
					double point )
{
	GOOGLE_BARCHART *barchart;
	int offset;

	barchart =
		google_barchart_get_or_set(
			barchart_list,
			stratum_name,
			list_length( datatype_name_list ) );

	if ( ( offset =
		google_chart_get_datatype_offset(
			datatype_name_list,
			datatype_name ) ) < 0 )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot get datatype_name = %s.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			datatype_name );
		exit( 1 );
	}

	barchart->point_array[ offset ] = google_point_double_calloc();

/*
	if ( ! ( barchart->point_array[ offset ] =
			calloc( 1, sizeof( double ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
*/

	*barchart->point_array[ offset ] = point;

} /* google_barchart_set_point() */

void google_timeline_set_point(	LIST *timeline_list,
				LIST *datatype_name_list,
				char *date_string,
				char *time_hhmm,
				char *datatype_name,
				double point )
{
	GOOGLE_TIMELINE *timeline;
	int offset;

	timeline =
		google_timeline_get_or_set(
			timeline_list,
			date_string,
			time_hhmm,
			list_length( datatype_name_list ) );

	if ( datatype_name && *datatype_name )
	{
		if ( ( offset =
			google_chart_get_datatype_offset(
				datatype_name_list,
				datatype_name ) ) < 0 )
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: cannot get datatype_name = %s.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				datatype_name );
			exit( 1 );
	}
	}
	else
	{
		offset = 0;
	}

	timeline->point_array[ offset ] = google_point_double_calloc();
	*timeline->point_array[ offset ] = point;

} /* google_timeline_set_point() */

double *google_point_double_calloc( void )
{
	double *d;

	if ( ! ( d = calloc( 1, sizeof( double * ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return d;

} /* google_point_double_calloc() */

double **google_point_array_double_calloc( int array_length )
{
	double **d;

	if ( ! ( d = calloc( array_length, sizeof( double * ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return d;

} /* google_point_array_double_calloc() */

void google_barchart_display(	LIST *barchart_list,
				LIST *datatype_name_list )
{
	GOOGLE_BARCHART *barchart;
	int offset;
	int length_datatype_name_list;

	length_datatype_name_list = list_length( datatype_name_list );

	fprintf(stderr,
		"datatype_name_list = %s\n",
		list_display( datatype_name_list ) );

	if ( !list_rewind( barchart_list ) ) return;

	do {
		barchart = list_get_pointer( barchart_list );

		fprintf(stderr,
			"stratum_name = %s",
			barchart->stratum_name );

		for( offset = 0; offset < length_datatype_name_list; offset++ )
		{
			if ( barchart->point_array[ offset ] )
			{
				fprintf(stderr,
					", *point_array[%d] = %.3lf",
					offset,
					*barchart->point_array[ offset ] );
			}
			else
			{
				fprintf(stderr,
					", *point_array[%d] is empty",
					offset );
			}
		}
		fprintf( stderr, "\n" );

	} while( list_next( barchart_list ) );

} /* google_barchart_display() */

void google_timeline_display(	LIST *timeline_list,
				LIST *datatype_name_list )
{
	GOOGLE_TIMELINE *timeline;
	int offset;
	int length_datatype_name_list;

	length_datatype_name_list = list_length( datatype_name_list );

	fprintf(stderr,
		"datatype_name_list = %s\n",
		list_display( datatype_name_list ) );

	if ( !list_rewind( timeline_list ) ) return;

	do {
		timeline = list_get_pointer( timeline_list );
		fprintf( stderr, "date_string = %s", timeline->date_string );
		fprintf( stderr, ",time_hhmm = %s", timeline->time_hhmm );

		for( offset = 0; offset < length_datatype_name_list; offset++ )
		{
			if ( timeline->point_array[ offset ] )
			{
				fprintf(stderr,
					", *point_array[%d] = %.3lf",
					offset,
					*timeline->point_array[ offset ] );
			}
			else
			{
				fprintf(stderr,
					", *point_array[%d] is empty",
					offset );
			}
		}
		fprintf( stderr, "\n" );

	} while( list_next( timeline_list ) );

} /* google_timeline_display() */

void google_chart_non_annotated_include( FILE *output_file )
{
	fprintf( output_file,
"<script type=\"text/javascript\" src=\"https://www.google.com/jsapi\"></script>\n" );
}

void google_chart_annotated_include( FILE *output_file )
{
	fprintf( output_file,
"<script type=\"text/javascript\" src=\"https://www.gstatic.com/charts/loader.js\"></script>\n" );
}

char *google_chart_convert_date(	char *destination,
					char *yyyy_mm_dd,
					char *hhmm )
{
	char date_half[ 64 ];
	char time_half[ 64 ];
	char year[ 16 ];
	char month[ 16 ];
	char day[ 16 ];
	int hours = 0;
	int minutes = 0;

	if ( character_count( '-', yyyy_mm_dd ) != 2 )
	{
		strcpy( destination, yyyy_mm_dd );
		return destination;
	}

	if ( character_count( ' ', yyyy_mm_dd ) == 1 )
	{
		piece( date_half, ' ', yyyy_mm_dd, 0 );
		piece( time_half, ' ', yyyy_mm_dd, 1 );

		yyyy_mm_dd = date_half;

		if ( *(time_half + 2) == ':' )
		{
			*(time_half + 2) = *(time_half + 3);
			*(time_half + 3) = *(time_half + 4);
			*(time_half + 4) = '\0';
		}

		hhmm = time_half;
	}
	else
	if ( character_count( ':', yyyy_mm_dd ) == 1 )
	{
		piece( date_half, ':', yyyy_mm_dd, 0 );
		piece( time_half, ':', yyyy_mm_dd, 1 );

		yyyy_mm_dd = date_half;
		hhmm = time_half;
	}

	piece( year, '-', yyyy_mm_dd, 0 );
	piece( month, '-', yyyy_mm_dd, 1 );
	piece( day, '-', yyyy_mm_dd, 2 );

	if ( hhmm )
	{
		date_time_parse(	&hours,
					&minutes,
					hhmm );
	}

	sprintf(	destination,
			"new Date(%s,%d,%s,%d,%d,0)",
			year,
			GOOGLE_CHART_CONVERT_MONTH_EXPRESSION,
			day,
			hours,
			minutes );

	return destination;

} /* google_chart_convert_date() */

void google_chart_output_visualization_annotated(
				FILE *output_file,
				enum google_chart_type google_chart_type,
				LIST *timeline_list,
				LIST *barchart_list,
				LIST *datatype_name_list,
				char *title,
				char *yaxis_label,
		 		int width,
		 		int height,
		 		char *background_color,
				boolean legend_position_bottom,
				boolean chart_type_bar,
				char *google_package_name,
				boolean dont_display_range_selector,
				enum aggregate_level aggregate_level,
				int chart_number )
{
	int length_datatype_name_list;
	char *chart_type_string;
	char *legend_position_bottom_string;
	char *google_chart_instantiation;
	char *first_column_datatype;
	char *visualization_function_name;

	visualization_function_name =
		google_chart_get_visualization_function_name(
			chart_number );

	if ( ! ( length_datatype_name_list =
			list_length( datatype_name_list ) ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty datatype_name_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !google_package_name )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: google_package_name is null.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( timlib_strcmp(	google_package_name,
				GOOGLE_ANNOTATED_TIMELINE ) == 0 )
	{
		if ( aggregate_level == aggregate_level_none
		||   aggregate_level == real_time
		||   aggregate_level == half_hour
		||   aggregate_level == hourly )
		{
			first_column_datatype = "datetime";
		}
		else
		{
			first_column_datatype = "date";
		}
	}
	else
	{
		first_column_datatype = "string";
	}

	fprintf( output_file,
"<script type=\"text/javascript\">\n"
"google.charts.load('visualization', {packages: ['%s']});\n",
		 google_package_name );

	fprintf( output_file,
"function %s()\n"
"{\n"
"	var data = new google.visualization.DataTable();\n"
"\n",
		visualization_function_name );

	google_chart_output_datatype_column_heading(
		output_file,
		google_chart_type,
		first_column_datatype,
		datatype_name_list );

	if ( list_length( timeline_list ) )
	{
		google_chart_output_timeline_list(
			output_file,
			timeline_list,
			length_datatype_name_list );
	}

	if ( list_length( barchart_list ) )
	{
		google_chart_output_barchart_list(
			output_file,
			barchart_list,
			length_datatype_name_list );
	}

	if ( chart_type_bar )
		chart_type_string = "seriesType: \"bars\"";
	else
		chart_type_string = "type: \"line\"";

	if ( legend_position_bottom )
		legend_position_bottom_string = "bottom";
	else
		legend_position_bottom_string = "";

	if ( strcmp( google_package_name, GOOGLE_ANNOTATED_TIMELINE ) == 0 )
		google_chart_instantiation = "AnnotatedTimeLine";
	else
	if ( strcmp( google_package_name, GOOGLE_CORECHART ) == 0 )
		google_chart_instantiation = "LineChart";
	else
		google_chart_instantiation = "corechart";

	fprintf( output_file,
"	var chart = new google.visualization.%s(\n"
"		document.getElementById('chart_div%d'));\n"
"	chart.draw(data, {displayAnnotations: true});\n"
"}\n",
		google_chart_instantiation,
		chart_number );

	fprintf( output_file,
"</script>\n" );

} /* google_chart_output_visualization_annotated() */

void google_chart_output_visualization_non_annotated(
				FILE *output_file,
				enum google_chart_type google_chart_type,
				LIST *timeline_list,
				LIST *barchart_list,
				LIST *datatype_name_list,
				char *title,
				char *yaxis_label,
		 		int width,
		 		int height,
		 		char *background_color,
				boolean legend_position_bottom,
				boolean chart_type_bar,
				char *google_package_name,
				boolean dont_display_range_selector,
				enum aggregate_level aggregate_level,
				int chart_number )
{
	int length_datatype_name_list;
	char *chart_type_string;
	char *legend_position_bottom_string;
	char *google_chart_instantiation;
	char *first_column_datatype;
	char *visualization_function_name;

	visualization_function_name =
		google_chart_get_visualization_function_name(
			chart_number );

	if ( ! ( length_datatype_name_list =
			list_length( datatype_name_list ) ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty datatype_name_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !google_package_name )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: google_package_name is null.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( timlib_strcmp(	google_package_name,
				GOOGLE_ANNOTATED_TIMELINE ) == 0 )
	{
		if ( aggregate_level == aggregate_level_none
		||   aggregate_level == real_time
		||   aggregate_level == half_hour
		||   aggregate_level == hourly )
		{
			first_column_datatype = "datetime";
		}
		else
		{
			first_column_datatype = "date";
		}
	}
	else
	{
		first_column_datatype = "string";
	}

	fprintf( output_file,
"<script type=\"text/javascript\">\n"
"google.load('visualization', '1', {packages: ['%s']});\n",
		 google_package_name );

	fprintf( output_file,
"function %s()\n"
"{\n"
"	var data = new google.visualization.DataTable();\n"
"\n",
		visualization_function_name );

	google_chart_output_datatype_column_heading(
		output_file,
		google_chart_type,
		first_column_datatype,
		datatype_name_list );

	if ( list_length( timeline_list ) )
	{
		google_chart_output_timeline_list(
			output_file,
			timeline_list,
			length_datatype_name_list );
	}

	if ( list_length( barchart_list ) )
	{
		google_chart_output_barchart_list(
			output_file,
			barchart_list,
			length_datatype_name_list );
	}

	if ( chart_type_bar )
		chart_type_string = "seriesType: \"bars\"";
	else
		chart_type_string = "type: \"line\"";

	if ( legend_position_bottom )
		legend_position_bottom_string = "bottom";
	else
		legend_position_bottom_string = "";

	google_chart_output_options(
		output_file,
		title,
		google_chart_type,
		chart_type_string,
		dont_display_range_selector,
		yaxis_label,
		width,
		height,
		background_color,
		legend_position_bottom_string );

	if ( strcmp( google_package_name, GOOGLE_ANNOTATED_TIMELINE ) == 0 )
		google_chart_instantiation = "AnnotatedTimeLine";
	else
	if ( strcmp( google_package_name, GOOGLE_CORECHART ) == 0 )
		google_chart_instantiation = "LineChart";
	else
		google_chart_instantiation = "corechart";

	fprintf( output_file,
"	var chart = new google.visualization.%s(\n"
"		document.getElementById('chart_div%d'));\n"
"	chart.draw(data, options);\n"
"}\n",
		google_chart_instantiation,
		chart_number );

	fprintf( output_file,
"</script>\n" );

} /* google_chart_output_visualization_non_annotated() */

void google_chart_output_chart_instantiation(
				FILE *output_file,
				int chart_number )
{
	char *visualization_function_name;

	visualization_function_name =
		google_chart_get_visualization_function_name(
			chart_number );

	fprintf( output_file,
"<script type=\"text/javascript\">\n"
"google.setOnLoadCallback(%s);\n"
"</script>\n",
		 visualization_function_name );

} /* google_chart_output_chart_instantiation() */

char *google_chart_get_visualization_function_name(
					int chart_number )
{
	static char visualization_function_name[ 32 ];

	sprintf( visualization_function_name,
		 "drawVisualization%d",
		 chart_number );

	return visualization_function_name;

} /* google_chart_get_visualization_function_name() */

void google_chart_output_barchart_list(
			FILE *output_file,
			LIST *barchart_list,
			int length_datatype_name_list )
{
	GOOGLE_BARCHART *barchart;
	int offset;
	char buffer[ 128 ];

	if ( !list_rewind( barchart_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty barchart_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	fprintf( output_file,
"	data.addRows([\n" );

	do {
		barchart = list_get_pointer( barchart_list );

		fprintf(output_file,
		 	"\t\t['%s'",
		 	format_initial_capital(
				buffer,
				barchart->stratum_name ) );

		for( offset = 0; offset < length_datatype_name_list; offset++ )
		{
			if ( barchart->point_array[ offset ] )
			{
				fprintf(output_file,
				 	",%.3lf",
				 	*barchart->point_array[ offset ] );
			}
			else
			{
				fprintf(output_file,
				 	",undefined" );
			}
		}

		fprintf( output_file, "]" );

		if ( !list_at_end( barchart_list ) )
			fprintf( output_file, ",\n" );
		else
			fprintf( output_file, "\n" );

	} while( list_next( barchart_list ) );

	fprintf( output_file,
"	]);\n\n" );

} /* google_chart_output_barchart_list() */

void google_chart_output_timeline_list(
			FILE *output_file,
			LIST *timeline_list,
			int length_datatype_name_list )
{
	GOOGLE_TIMELINE *timeline;
	int offset;
	char buffer[ 128 ];

	if ( !list_rewind( timeline_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty timeline_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	fprintf( output_file,
"	data.addRows([\n" );

	do {
		timeline = list_get_pointer( timeline_list );

		fprintf(output_file,
		 	"\t\t[%s",
		 	google_chart_convert_date(
				buffer,
				timeline->date_string,
				timeline->time_hhmm ) );

		for( offset = 0; offset < length_datatype_name_list; offset++ )
		{
			if ( timeline->point_array[ offset ] )
			{
				fprintf(output_file,
				 	",%.3lf",
				 	*timeline->point_array[ offset ] );
			}
			else
			{
				fprintf(output_file,
				 	",undefined" );
			}
		}

		fprintf( output_file, "]" );

		if ( !list_at_end( timeline_list ) )
			fprintf( output_file, ",\n" );
		else
			fprintf( output_file, "\n" );

	} while( list_next( timeline_list ) );

	fprintf( output_file,
"	]);\n\n" );

} /* google_chart_output_timeline_list() */

void google_chart_output_datatype_column_heading(
			FILE *output_file,
			enum google_chart_type google_chart_type,
			char *first_column_datatype,
			LIST *datatype_name_list )
{
	int offset = 0;
	char *datatype_name;
	char buffer[ 128 ];

	fprintf( output_file,
"	data.addColumn('%s', '');\n",
		first_column_datatype );

	if ( !list_rewind( datatype_name_list ) ) return;

	do {
		datatype_name = list_get_pointer( datatype_name_list );

		fprintf( output_file,
"	data.addColumn('number', '%s');\n",
		 	format_initial_capital( buffer, datatype_name ) );

		if ( google_chart_type == google_cat_whiskers
		&&   ( offset % 3 ) == 1 )
		{
			fprintf(
			output_file,
			"	data.addColumn('number', '');\n" );
		}

		offset++;

	} while( list_next( datatype_name_list ) );

} /* google_chart_output_datatype_column_heading() */

void google_chart_float_chart(	FILE *output_file,
				char *title,
				int width,
				int height,
				int chart_number )
{
	if ( title && *title )
	{
		fprintf( output_file,
"<h3>%s</h3>\n",
			 title );
	}

	fprintf( output_file,
"<div id=\"chart_div%d\" style=\"width: %dpx; height: %dpx\"></div>\n",
		 chart_number,
		 width,
		 height );

} /* google_chart_float_chart() */

void google_chart_anchor_chart(	FILE *output_file,
				char *title,
				char *google_package_name,
				int left,
				int top,
				int width,
				int height,
				int chart_number )
{
	/* Anchor the title */
	/* ---------------- */
	if ( title
	&&   *title
	&&   strcmp(	google_package_name,
			GOOGLE_ANNOTATED_TIMELINE ) == 0 )
	{
		fprintf( output_file,
"<div style=\"	position: absolute;	\n"
"		left: %dpx;		\n"
"		top: %dpx;		\n"
"		font-size: 16px\">	\n"
"%s</div>\n",
			 left,
			 top,
			 title );
		top += 20;
	}

	/* Anchor the chart */
	/* ---------------- */
	fprintf( output_file,
"<div style=\"position: absolute;\n"
"	left: %dpx;\n"
"	top: %dpx;\n"
"	width: %dpx;\n"
"	border-width: thin;\n"
"	border-style: solid;\n"
"	border-color: teal\" >\n"
"	<div id=\"chart_div%d\" style=\"width: %dpx; height: %dpx\"></div>\n"
"</div>\n",
		 left,
		 top,
		 width,
		 chart_number,
		 width,
		 height );

} /* google_chart_anchor_chart() */

void google_chart_output_prompt(
			char *application_name,
			char *prompt_filename,
			char *target_window,
			char *where_clause )
{
	if ( application_name )
	{
		printf(
"<body bgcolor=\"%s\" onload=\"window.open('%s','%s');\">\n",
			application_get_background_color( application_name ),
			prompt_filename,
			target_window );
	}

	printf( "<h1>Google Chart Viewer " );
	fflush( stdout );
	if ( system( timlib_system_date_string() ) ){};
	printf( "</h1>\n" );

	if ( where_clause && *where_clause )
		printf( "<br>Search criteria: %s\n", where_clause );

	printf( "<br><hr><a href=\"%s\" target=%s>Press to view chart.</a>\n",
		prompt_filename,
		target_window );

} /* google_chart_output_prompt() */

void google_chart_output_options(
			FILE *output_file,
			char *title,
			enum google_chart_type google_chart_type,
			char *chart_type_string,
			boolean dont_display_range_selector,
			char *yaxis_label,
			int width,
			int height,
			char *background_color,
			char *legend_position_bottom_string )
{
	boolean got_one = 0;

	fprintf( output_file,
"	var options = {\n" );

	if ( chart_type_string && *chart_type_string )
	{
		if ( got_one ) fprintf( output_file, ",\n" );

		fprintf( output_file,
"		%s",
			 chart_type_string );

		got_one = 1;
	}

	if ( dont_display_range_selector )
	{
		if ( got_one ) fprintf( output_file, ",\n" );

		fprintf( output_file,
"		displayRangeSelector: false,\n"
"		displayZoomButtons: false" );

		got_one = 1;
	}

	if ( yaxis_label && *yaxis_label )
	{
		if ( got_one ) fprintf( output_file, ",\n" );

		fprintf( output_file,
"		vAxis: {title: \"%s\"}",
			 yaxis_label );

		got_one = 1;

	}

	if ( title && *title )
	{
		if ( got_one ) fprintf( output_file, ",\n" );

		fprintf( output_file,
"		title: \"%s\"",
			 title );

		got_one = 1;
	}

	if ( width )
	{
		if ( got_one ) fprintf( output_file, ",\n" );

		fprintf( output_file,
"		width: %d",
			 width );

		got_one = 1;
	}

	if ( height )
	{
		if ( got_one ) fprintf( output_file, ",\n" );

		fprintf( output_file,
"		height: %d",
			 height );

		got_one = 1;
	}

	if ( background_color && *background_color )
	{
		if ( got_one ) fprintf( output_file, ",\n" );

		fprintf( output_file,
"		backgroundColor: \"%s\"",
			 background_color );

		got_one = 1;
	}

	if ( legend_position_bottom_string && *legend_position_bottom_string )
	{
		if ( got_one ) fprintf( output_file, ",\n" );

		fprintf( output_file,
"		legend: { position: \"%s\" }",
			 legend_position_bottom_string );

		got_one = 1;
	}

	if ( google_chart_type == google_cat_whiskers )
	{
		if ( got_one ) fprintf( output_file, ",\n" );

		fprintf( output_file,
"		series: [\n"
"		{color: 'blue',	visibleInLegend: true},\n"
"		{color: 'red',	visibleInLegend: true}\n"
"			]" );

		got_one = 1;
	}

	fprintf( output_file, "\n\t}\n" );

} /* google_chart_output_options() */

GOOGLE_INPUT_VALUE *google_input_value_new( char *date_time )
{
	GOOGLE_INPUT_VALUE *g;

	if ( ! ( g = (GOOGLE_INPUT_VALUE *)
			calloc( 1, sizeof( GOOGLE_INPUT_VALUE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	g->date_time = date_time;
	return g;

} /* google_input_value_new() */

GOOGLE_UNIT_CHART *google_unit_chart_new( char *unit )
{
	GOOGLE_UNIT_CHART *g;

	if ( ! ( g = (GOOGLE_UNIT_CHART *)
			calloc( 1, sizeof( GOOGLE_UNIT_CHART ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	g->unit = unit;

	g->yaxis_label = strdup( unit );
	format_initial_capital( g->yaxis_label, g->yaxis_label );

	g->date_time_dictionary = dictionary_large_new();
	g->unit_datatype_list = list_new();

	return g;

} /* google_unit_chart_new() */

GOOGLE_UNIT_DATATYPE *google_unit_datatype_new( char *datatype_name )
{
	GOOGLE_UNIT_DATATYPE *g;

	if ( ! ( g = (GOOGLE_UNIT_DATATYPE *)
			calloc( 1, sizeof( GOOGLE_UNIT_DATATYPE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	g->datatype_name = datatype_name;
	g->value_hash_table = hash_table_new( hash_table_large );
	return g;

} /* google_unit_datatype_new() */

boolean google_datatype_chart_input_value_list_set(
				LIST *input_value_list,
				char *sys_string,
				int date_piece,
				int time_piece,
				int value_piece,
				char delimiter )
{
	FILE *input_pipe;
	char date_string[ 128 ];
	char time_string[ 128 ];
	char value_string[ 128 ];
	char input_buffer[ 1024 ];
	char *date_time_key;
	boolean null_value;
	GOOGLE_INPUT_VALUE *input_value;
	boolean got_input = 0;

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		if ( !*input_buffer || *input_buffer == '#' )
		{
			continue;
		}

		piece(	date_string,
			delimiter,
			input_buffer,
			date_piece );

		if ( time_piece != -1 )
		{
			piece(	time_string,
				delimiter,
				input_buffer,
				time_piece );
		}
		else
		{
			*time_string = '\0';
		}

		piece(	value_string,
			delimiter,
			input_buffer,
			value_piece );

		null_value = ( *value_string ) ? 0 : 1;

		date_time_key =
			google_chart_get_date_time_key(
				date_string,
				time_string );

		input_value =
			google_chart_input_value_new(
				strdup( date_time_key ) );

		input_value->value = atof( value_string );
		input_value->null_value = null_value;

		list_append_pointer( input_value_list, input_value );

		got_input = 1;
	}

	pclose( input_pipe );
	return got_input;

} /* google_datatype_chart_input_value_list_set() */

boolean google_chart_value_hash_table_set(
				HASH_TABLE *value_hash_table,
				DICTIONARY *date_time_dictionary,
				char *sys_string,
				int date_piece,
				int time_piece,
				int value_piece,
				char delimiter )
{
	FILE *input_pipe;
	char date_string[ 128 ];
	char time_string[ 128 ];
	char value_string[ 128 ];
	char input_buffer[ 1024 ];
	boolean null_value;
	boolean got_input = 0;

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		if ( !*input_buffer || *input_buffer == '#' )
		{
			continue;
		}

		piece(	date_string,
			delimiter,
			input_buffer,
			date_piece );

		if ( time_piece != -1 )
		{
			piece(	time_string,
				delimiter,
				input_buffer,
				time_piece );
		}
		else
		{
			*time_string = '\0';
		}

		piece(	value_string,
			delimiter,
			input_buffer,
			value_piece );

		null_value = ( *value_string ) ? 0 : 1;

		if ( !google_chart_set_input_value(
			value_hash_table,
			date_time_dictionary,
			strdup( date_string ),
			(*time_string) ? strdup( time_string ) : (char *)0,
			atof( value_string ),
			null_value ) )
		{
			pclose( input_pipe );
			return 0;
		}
		got_input = 1;
	}

	pclose( input_pipe );
	return got_input;

} /* google_chart_value_hash_table_set() */

char *google_chart_get_date_time_key(
			char *date_string,
			char *time_string )
{
	static char key[ 128 ];

	if ( time_string && *time_string )
	{
		sprintf( key, "%s:%s", date_string, time_string );
	}
	else
	{
		strcpy( key, date_string );
	}

	return key;

} /* google_chart_get_date_time_key() */

boolean google_chart_set_input_value(
				HASH_TABLE *value_hash_table,
				DICTIONARY *date_time_dictionary,
				char *date_string,
				char *time_string,
				double value,
				boolean null_value )
{
	GOOGLE_INPUT_VALUE *input_value;
	char *date_time_key;

	if ( !value_hash_table )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty value_hash_table.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return 0;
	}

	if ( !date_time_dictionary )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty date_time_dictionary.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return 0;
	}

	date_time_key =
		google_chart_get_date_time_key(
			date_string,
			time_string );

	input_value =
		google_chart_input_value_new(
			strdup( date_time_key ) );

	input_value->value = value;
	input_value->null_value = null_value;

	hash_table_set_pointer(
		value_hash_table,
		input_value->date_time,
		(void *)input_value );

	if ( !dictionary_exists_key(
		date_time_dictionary,
		input_value->date_time ) )
	{
		dictionary_set_pointer(
			date_time_dictionary,
			input_value->date_time,
			"" );
	}

	return 1;

} /* google_chart_set_input_value() */

LIST *google_chart_get_unit_datatype_name_list(
			LIST *unit_datatype_list )
{
	GOOGLE_UNIT_DATATYPE *unit_datatype;
	LIST *datatype_name_list;

	if ( !list_rewind( unit_datatype_list ) ) return (LIST *)0;

	datatype_name_list = list_new();

	do {
		unit_datatype = list_get_pointer( unit_datatype_list );

		list_append_pointer(
			datatype_name_list,
			unit_datatype->datatype_name );

	} while( list_next( unit_datatype_list ) );

	return datatype_name_list;

} /* google_chart_get_unit_datatype_name_list() */

LIST *google_datatype_chart_get_datatype_name_list(
			LIST *datatype_chart_list )
{
	LIST *datatype_name_list;
	GOOGLE_DATATYPE_CHART *datatype_chart;

	if ( !list_rewind( datatype_chart_list ) ) return (LIST *)0;

	datatype_name_list = list_new();

	do {
		datatype_chart = list_get_pointer( datatype_chart_list );

		list_append_pointer(
			datatype_name_list,
			datatype_chart->datatype_name );

	} while( list_next( datatype_chart_list ) );

	return datatype_name_list;

} /* google_datatype_chart_list_get_datatype_name_list() */

LIST *google_chart_datatype_get_output_chart_list(
			LIST *datatype_chart_list,
			int width,
			int height )
{
	GOOGLE_DATATYPE_CHART *datatype_chart;
	GOOGLE_INPUT_VALUE *input_value;
	GOOGLE_OUTPUT_CHART *output_chart;
	LIST *output_chart_list;

	if ( !list_length( datatype_chart_list ) ) return (LIST *)0;

	output_chart_list = list_new_list();

	output_chart =
		google_output_chart_new(
			0 /* left */,
			0 /* top */,
			width,
			height );

	output_chart->datatype_name_list =
		google_datatype_chart_get_datatype_name_list(
			datatype_chart_list );

	list_append_pointer( output_chart_list, output_chart );

	list_rewind( datatype_chart_list );

	do {
		datatype_chart = list_get_pointer( datatype_chart_list );

		if ( !list_rewind( datatype_chart->input_value_list ) )
			continue;

		output_chart->yaxis_label = datatype_chart->yaxis_label;

		do {
			input_value =
				list_get_pointer( 
					datatype_chart->input_value_list );

			if ( !input_value->null_value )
			{
				google_timeline_set_point(
					output_chart->timeline_list,
					output_chart->datatype_name_list,
					input_value->date_time,
					(char *)0 /* time_hhmm */,
					datatype_chart->datatype_name,
					input_value->value );
			}

		} while( list_next( datatype_chart->input_value_list ) );

	} while( list_next( datatype_chart_list ) );

	return output_chart_list;

} /* google_chart_datatype_get_output_chart_list() */

LIST *google_chart_unit_get_output_chart_list(
			LIST *unit_chart_list,
			int width,
			int height )
{
	GOOGLE_UNIT_CHART *unit_chart;
	GOOGLE_OUTPUT_CHART *output_chart;
	LIST *output_chart_list;
	LIST *date_time_key_list;

	if ( !list_rewind( unit_chart_list ) ) return (LIST *)0;

	output_chart_list = list_new_list();

	do {
		unit_chart = list_get_pointer( unit_chart_list );

		date_time_key_list =
			dictionary_get_ordered_key_list(
				unit_chart->date_time_dictionary );

		if ( !date_time_key_list
		||   !list_length( date_time_key_list ) )
		{
			continue;
		}

		output_chart =
			google_chart_unit_get_output_chart(
				unit_chart->unit_datatype_list,
				date_time_key_list,
				width,
				height );

		if ( !output_chart ) continue;

		/* Copy input parameters to output */
		/* ------------------------------- */
		/* output_chart->bar_chart = unit_chart->bar_chart; */
		/* output_chart->xaxis_label = unit_chart->xaxis_label; */
		output_chart->yaxis_label = unit_chart->yaxis_label;

		list_append_pointer( output_chart_list, output_chart );
		list_free_string_container( date_time_key_list );

	} while( list_next( unit_chart_list ) );

	return output_chart_list;

} /* google_chart_unit_get_output_chart_list() */

GOOGLE_OUTPUT_CHART *google_chart_unit_get_output_chart(
				LIST *unit_datatype_list,
				LIST *date_time_key_list,
				int width,
				int height )
{
	GOOGLE_UNIT_DATATYPE *unit_datatype;
	GOOGLE_OUTPUT_CHART *output_chart;
	GOOGLE_INPUT_VALUE *input_value;
	char *date_time_key;

	if ( !list_length( unit_datatype_list )
	||   !list_length( date_time_key_list ) )
	{
		return (GOOGLE_OUTPUT_CHART *)0;
	}

	output_chart =
		google_output_chart_new(
			0 /* left */,
			0 /* top */,
			width,
			height );

	output_chart->datatype_name_list =
		google_chart_get_unit_datatype_name_list(
			unit_datatype_list );

	list_rewind( unit_datatype_list );

	do {
		unit_datatype = list_get_pointer( unit_datatype_list );

		list_rewind( date_time_key_list );

		do {
			date_time_key = list_get_pointer( date_time_key_list );

			input_value =
				hash_table_get_pointer(
					unit_datatype->value_hash_table,
					date_time_key );

			if ( !input_value ) continue;

			if ( !input_value->null_value )
			{
				google_timeline_set_point(
					output_chart->timeline_list,
					output_chart->datatype_name_list,
					input_value->date_time,
					(char *)0 /* time_hhmm */,
					unit_datatype->datatype_name,
					input_value->value );
			}

		} while( list_next( date_time_key_list ) );

	} while( list_next( unit_datatype_list ) );

	return output_chart;

} /* google_chart_unit_get_output_chart() */

void google_chart_output_all_charts(
			FILE *output_file,
			LIST *output_chart_list,
			char *title,
			char *sub_title,
			char *stylesheet )
{

	GOOGLE_OUTPUT_CHART *google_chart;

	document_output_html_stream( output_file );

	fprintf( output_file, "<head>\n" );

	google_chart_non_annotated_include( output_file );

	if ( stylesheet && *stylesheet )
	{
		fprintf( output_file,
			 "<link rel=stylesheet type=text/css href=\"%s\">\n",
			 stylesheet );
	}

	fprintf( output_file, "</head>\n" );

	fprintf( output_file, "<body>\n" );

	if ( title && *title )
	{
		fprintf( output_file, "<h1>%s</h1>\n", title );
	}

	if ( sub_title && *sub_title )
	{
		fprintf( output_file, "<h2>%s</h2>\n", sub_title );
	}

	if ( list_rewind( output_chart_list ) )
	{
		do {
			google_chart = list_get_pointer( output_chart_list );

			google_chart_output_visualization_non_annotated(
				output_file,
				google_chart->google_chart_type,
				google_chart->timeline_list,
				google_chart->barchart_list,
				google_chart->datatype_name_list,
				(char *)0 /* chart_title */,
				google_chart->yaxis_label,
				google_chart->width,
				google_chart->height,
				google_chart->background_color,
				google_chart->legend_position_bottom,
				0 /* not chart_type_bar */,
				google_chart->google_package_name,
				0 /* not dont_display_range_selector */,
				aggregate_level_none,
				google_chart->chart_number );

			google_chart_float_chart(
				output_file,
				(char *)0 /* chart_title */,
				google_chart->width,
				google_chart->height,
				google_chart->chart_number );

			fprintf( output_file, "<br>\n" );
			fprintf( output_file, "<hr>\n" );
			fprintf( output_file, "<br>\n" );

			google_chart_output_chart_instantiation(
				output_file,
				google_chart->chart_number );

		} while( list_next( output_chart_list ) );

	} /* if list_rewind() */

	fprintf( output_file, "</body>\n" );
	fprintf( output_file, "</html>\n" );

} /* google_chart_output_all_charts() */

void google_chart_output_graph_window(
			char *application_name,
			char *appaserver_mount_point,
			boolean with_document_output,
			char *window_name,
			char *prompt_filename,
			char *where_clause )
{
	DOCUMENT *document;

	if ( with_document_output )
	{
		document = document_new( "", application_name );
		document_set_output_content_type( document );
	
		document_output_head(
				document->application_name,
				document->title,
				document->output_content_type,
				appaserver_mount_point,
				document->javascript_module_list,
				document->stylesheet_filename,
				application_get_relative_source_directory(
					application_name ),
				0 /* not with_dynarch_menu */ );
	}


	printf(
"<body bgcolor=\"%s\" onload=\"window.open('%s','%s');\">\n",
		application_get_background_color( application_name ),
		prompt_filename,
		window_name );

	printf( "<h1>Google Chart Viewer " );
	fflush( stdout );
	if ( system( timlib_system_date_string() ) ){};
	printf( "</h1>\n" );

	if ( where_clause && *where_clause )
		printf( "<br>Search criteria: %s\n", where_clause );

	printf( "<br><hr><a href=\"%s\" target=%s>Press to view chart.</a>\n",
		prompt_filename,
		window_name );

	if ( with_document_output ) document_close();

} /* google_chart_output_graph_window() */

GOOGLE_DATATYPE_CHART *google_datatype_chart_get_or_set(
					LIST *datatype_chart_list,
					char *datatype_name )
{
	GOOGLE_DATATYPE_CHART *g;

	if ( list_rewind( datatype_chart_list ) )
	{
		do {
			g = list_get_pointer( datatype_chart_list );

			if ( timlib_strcmp(	g->datatype_name,
						datatype_name ) == 0 )
			{
				return g;
			}

		} while( list_next( datatype_chart_list ) );
	}

	g = google_datatype_append(
		datatype_chart_list,
		strdup( datatype_name ) );

	g->input_value_list = list_new();

	return g;

} /* google_datatype_chart_get_or_set() */

GOOGLE_DATATYPE_CHART *google_datatype_append(
				LIST *datatype_chart_list,
				char *datatype_name )
{
	GOOGLE_DATATYPE_CHART *g;

	g = google_datatype_chart_new( datatype_name );

	list_append_pointer( datatype_chart_list, g );

	return g;

} /* google_datatype_append() */

GOOGLE_DATATYPE_CHART *google_datatype_chart_new(
					char *datatype_name )
{
	GOOGLE_DATATYPE_CHART *g;

	if ( ! ( g = (GOOGLE_DATATYPE_CHART *)
			calloc( 1, sizeof( GOOGLE_DATATYPE_CHART ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	g->datatype_name = datatype_name;
	return g;

} /* google_datatype_chart_new() */

char *google_chart_input_value_list_display(
			LIST *input_value_list )
{
	GOOGLE_INPUT_VALUE *input_value;
	char buffer[ 65536 ];
	char *ptr = buffer;

	*ptr = '\0';

	if ( list_rewind( input_value_list ) )
	{
		do {
			input_value = list_get_pointer( input_value_list );

			ptr += sprintf(
			ptr,
			"date_time = %s; value = %.2lf; null_value = %d\n",
			input_value->date_time,
			input_value->value,
			input_value->null_value );

		} while( list_next( input_value_list ) );
	}

	ptr += sprintf( ptr, "\n" );

	return strdup( buffer );

} /* google_chart_input_value_list_display() */

char *google_datatype_chart_display( GOOGLE_DATATYPE_CHART *datatype_chart )
{
	char buffer[ 65536 ];
	char *results;

	sprintf(
		buffer,
		"datatype_name = %s, yaxis_label = %s, input_value_list = %s\n",
		datatype_chart->datatype_name,
		datatype_chart->yaxis_label,
		(results = google_chart_input_value_list_display(
			datatype_chart->input_value_list ) ) );

	free( results );

	return strdup( buffer );

} /* google_datatype_chart_display() */

char *google_datatype_chart_list_display(
				LIST *datatype_chart_list )
{
	GOOGLE_DATATYPE_CHART *datatype_chart;
	char buffer[ 65536 ];
	char *results;
	char *ptr = buffer;

	*ptr = '\0';

	if ( list_rewind( datatype_chart_list ) )
	{
		do {
			datatype_chart =
				list_get_pointer(
					datatype_chart_list );

			ptr += sprintf( ptr,
					"%s\n",
					(results =
					   google_datatype_chart_display(
						datatype_chart ) ) );

			free( results );

			ptr += sprintf( ptr, "\n" );

		} while( list_next( datatype_chart_list ) );
	}

	return strdup( buffer );

} /* google_datatype_chart_list_display() */

GOOGLE_COMBO_CHART *google_combo_chart_new( void )
{
	GOOGLE_COMBO_CHART *g;

	g = (GOOGLE_COMBO_CHART *)calloc( 1, sizeof( GOOGLE_COMBO_CHART ) );

	if ( !g )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return g;

} /* google_combo_chart_new() */

GOOGLE_HAXIS *google_haxis_new( void )
{
	GOOGLE_HAXIS *g;

	g = (GOOGLE_HAXIS *)calloc( 1, sizeof( GOOGLE_HAXIS ) );

	if ( !g )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return g;

} /* google_haxis_new() */

GOOGLE_VAXIS *google_vaxis_new( void )
{
	GOOGLE_VAXIS *g;

	g = (GOOGLE_VAXIS *)calloc( 1, sizeof( GOOGLE_VAXIS ) );

	if ( !g )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return g;

} /* google_vaxis_new() */

GOOGLE_DATA *google_data_new( void )
{
	GOOGLE_DATA *g;

	g = (GOOGLE_DATA *)calloc( 1, sizeof( GOOGLE_DATA ) );

	if ( !g )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return g;

} /* google_data_new() */

GOOGLE_VAXIS_SERIES *google_vaxis_series_new( void )
{
	GOOGLE_VAXIS_SERIES *g;

	g = (GOOGLE_VAXIS_SERIES *)calloc( 1, sizeof( GOOGLE_VAXIS_SERIES ) );

	if ( !g )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return g;

} /* google_vaxis_series() */

