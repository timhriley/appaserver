/* library/easycharts.c */
/* -------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <unistd.h>
#include "list.h"
#include "timlib.h"
#include "hashtbl.h"
#include "dictionary.h"
#include "environ.h"
#include "document.h"
#include "application.h"
#include "piece.h"
#include "easycharts.h"
#include "appaserver_library.h"
#include "appaserver_link_file.h"

EASYCHARTS *easycharts_new_easycharts(	int width,
					int height )
{
	EASYCHARTS *e;

	e = (EASYCHARTS *)calloc( 1, sizeof( EASYCHARTS ) );

	if ( !e )
	{
		fprintf( stderr,
			 "ERROR in %s/%s(): cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	if ( width )
		e->width = width;
	else
		e->width = EASYCHARTS_WIDTH;

	if ( height )
		e->height = height;
	else
		e->height = EASYCHARTS_HEIGHT;

	e->sample_label_angle = 270;
	e->legend_on = 1;
	e->bold_labels = 1;
	e->bold_legends = 1;
	e->font_size = 14;
	e->label_parameter_name = "sampleLabels";
	e->input_chart_list = list_new_list();

	return e;
} /* easycharts_new_easycharts() */

EASYCHARTS *easycharts_new_timeline_easycharts(
					int width,
					int height )
{
	EASYCHARTS *e;

	e = easycharts_new_easycharts( width, height );

	e->highlight_style = "circle_filled";
	e->point_highlight_size = POINT_HIGHLIGHT_SIZE;
	e->highlight_on = 1;

	return e;
} /* easycharts_new_timeline_easycharts() */

EASYCHARTS *easycharts_new_scatter_easycharts(
					int width,
					int height )
{
	EASYCHARTS *e;

	e = easycharts_new_easycharts( width, height );

	e->highlight_style = "circle_filled";
	e->point_highlight_size = POINT_HIGHLIGHT_SIZE;
	e->series_line_off = 1;
	e->highlight_on = 1;

	return e;
} /* easycharts_new_scatter_easycharts() */

EASYCHARTS_POINT *easycharts_new_point(	double x_value,
					double y_value,
					char *x_label )
{
	EASYCHARTS_POINT *e;

	e = (EASYCHARTS_POINT *)
		calloc( 1, sizeof( EASYCHARTS_POINT ) );
	if ( !e )
	{
		fprintf( stderr,
			 "ERROR in %s/%s(): cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	e->x_value = x_value;
	e->y_value = y_value;
	e->x_label = x_label;
	e->display_value = 1;
	return e;
} /* easycharts_new_point() */

boolean easycharts_set_point(	LIST *point_list,
				double x_value,
				double y_value,
				char *x_label,
				boolean display_value )
{
	EASYCHARTS_POINT *point;

	point = easycharts_new_point( x_value, y_value, x_label );

	if ( !point ) return 0;
	point->display_value = display_value;

	if ( !list_append_pointer( point_list, (char *)point ) )
		return 0;
	else
		return 1;
} /* easycharts_set_point() */

boolean easycharts_output_all_charts(
				FILE *destination,
				LIST *output_chart_list,
				int highlight_on,
				char *highlight_style,
				int point_highlight_size,
				char *series_labels,
				int series_line_off,
				char *applet_library_archive,
				int width,
				int height,
				char *title,
				boolean set_y_lower_range,
				boolean legend_on,
				boolean value_labels_on,
				boolean sample_scroller_on,
				boolean range_scroller_on,
				int xaxis_decimal_count,
				int yaxis_decimal_count,
				boolean range_labels_off,
				boolean value_lines_off,
				int range_step,
				int sample_label_angle,
				boolean bold_labels,
				boolean bold_legends,
				int font_size,
				char *label_parameter_name,
				boolean include_sample_values_output )
{
	EASYCHARTS_OUTPUT_CHART *output_chart;

	if ( !list_rewind( output_chart_list ) ) return 0;

	do {
		output_chart = list_get_pointer( output_chart_list );

		if ( !easycharts_output_chart(
				destination,
				output_chart->output_datatype_list,
				highlight_on,
				highlight_style,
				point_highlight_size,
				series_labels,
				series_line_off,
				output_chart->applet_library_code,
				applet_library_archive,
				width,
				height,
				title,
				output_chart->x_axis_label,
				output_chart->y_axis_label,
				set_y_lower_range,
				legend_on,
				value_labels_on,
				sample_scroller_on,
				range_scroller_on,
				xaxis_decimal_count,
				yaxis_decimal_count,
				range_labels_off,
				value_lines_off,
				range_step,
				sample_label_angle,
				bold_labels,
				bold_legends,
				font_size,
				output_chart->bar_labels_on,
				label_parameter_name,
				include_sample_values_output,
				output_chart->bar_chart,
				output_chart->double_range_adjusters ) )
		{
			return 0;
		}
	} while( list_next( output_chart_list ) );
	return 1;
} /* easycharts_output_all_charts() */

int easycharts_output_chart(
				FILE *destination,
				LIST *output_datatype_list,
				int highlight_on,
				char *highlight_style,
				int point_highlight_size,
				char *series_labels,
				int series_line_off,
				char *applet_library_code,
				char *applet_library_archive,
				int width,
				int height,
				char *title,
				char *x_axis_label,
				char *y_axis_label,
				boolean set_y_lower_range,
				boolean legend_on,
				boolean value_labels_on,
				boolean sample_scroller_on,
				boolean range_scroller_on,
				int xaxis_decimal_count,
				int yaxis_decimal_count,
				boolean range_labels_off,
				boolean value_lines_off,
				int range_step,
				int sample_label_angle,
				boolean bold_labels,
				boolean bold_legends,
				int font_size,
				boolean bar_labels_on,
				char *label_parameter_name,
				boolean include_sample_values_output,
				boolean bar_chart,
				boolean double_range_adjusters )
{
	EASYCHARTS_OUTPUT_DATATYPE *datatype;
	char bold_labels_string[ 16 ];
	char bold_legends_string[ 16 ];
	char buffer[ 128 ];

	if ( bold_labels )
		strcpy( bold_labels_string, "bold," );
	else
		*bold_labels_string = '\0';

	if ( bold_legends )
		strcpy( bold_legends_string, "bold," );
	else
		*bold_legends_string = '\0';

	if ( !output_datatype_list
	||   !list_length( output_datatype_list ) )
	{
		return 0;
	}

	fprintf(destination,
		"<applet code=%s archive=%s width=%d height=%d",
		applet_library_code,
		applet_library_archive,
		width,
		height );

	fprintf( destination, " VIEWASTEXT id=Applet1>\n" );

	if ( title && *title )
	{
		fprintf(destination,
			"<param name=chartTitle value=\"%s\">\n",
			title );
	}

	if ( set_y_lower_range )
	{
		easycharts_output_y_minimum_integer(
			destination,
			easycharts_get_y_minimum_integer(
				output_datatype_list ) );
	}

	fprintf(destination,
		"<param name=seriesCount value=%d>\n",
		list_length( output_datatype_list ) );

	/* Output y axis */
	/* ------------- */
	if ( include_sample_values_output )
	{
		if ( list_length( output_datatype_list ) == 1 )
		{
			datatype =
				(EASYCHARTS_OUTPUT_DATATYPE *)
					list_get_first_pointer(
						output_datatype_list );

			fprintf(destination,
			"<param name=sampleValues value=\"" );

			easycharts_output_point_list(
					destination,
					datatype->point_list,
					0 /* not output_x_axis */ );
			fprintf( destination, "\">\n" );
		}
		else
		{
			int i = 0;

			list_rewind( output_datatype_list );

			do {
				datatype =
					list_get_pointer(
						output_datatype_list );

				fprintf(destination,
				"<param name=sampleValues_%d value=\"",
					i++ );

				easycharts_output_point_list(
					destination,
					datatype->point_list,
					0 /* not output_x_axis */ );
				fprintf( destination, "\">\n" );
			} while( list_next( output_datatype_list ) );
		}
	} /* include_sample_values_output */

	/* Output x axis info */
	/* ------------------ */
	fprintf(destination,
		"<param name=%s value=\"",
		label_parameter_name );
	
	datatype =
		(EASYCHARTS_OUTPUT_DATATYPE *)
			list_get_first_pointer(
				output_datatype_list );

	easycharts_output_point_list(
		destination,
		datatype->point_list,
		1 /* output_x_axis */ );
	
	fprintf( destination, "\">\n" );

	fprintf( destination, "<param name=background value=white>\n" );

	/* Output other graph attributes */
	/* ----------------------------- */
	if ( highlight_on )
		fprintf(destination,
			"<param name=sampleHighlightOn value=\"true\">\n" );
	else
		fprintf(destination,
			"<param name=sampleHighlightOn value=\"false\">\n" );

	if ( highlight_style && *highlight_style )
	{
		fprintf(destination,
			"<param name=sampleHighlightStyle value=\"%s\">\n",
			highlight_style );
	}

	fprintf(destination,
		"<param name=sampleHighlightSize value=\"%d\">\n",
		point_highlight_size );

	if ( series_labels && *series_labels )
	{
		fprintf(destination,
			"<param name=seriesLabels value=\"%s\">\n",
			series_labels );
	}

	if ( x_axis_label && *x_axis_label )
	{
		fprintf(destination,
			"<param name=sampleAxisLabel value=\"%s\">\n",
			format_initial_capital( buffer, x_axis_label ) );

		fprintf(destination,
		"<param name=sampleAxisLabelFont value=\"Arial,%s%d\">\n",
			bold_legends_string,
			font_size );
	}


	if ( y_axis_label && *y_axis_label )
	{
		fprintf(destination,
			"<param name=rangeAxisLabel value=\"%s\">\n",
			format_initial_capital( buffer, y_axis_label ) );
		fprintf(destination,
			"<param name=rangeAxisLabelAngle value=270>\n" );
		fprintf(destination,
		"<param name=rangeAxisLabelFont value=\"Arial,%s%d\">\n",
			bold_legends_string,
			font_size );
	}

	fprintf(destination,
	 	"<param name=sampleLabelsOn value=true>\n" );

	if ( series_line_off )
	{
		fprintf(destination,
		 	"<param name=seriesLineOff value=true>\n" );
	}

	if ( legend_on )
	{
		if ( !series_labels || !*series_labels )
		{
			fprintf(destination,
				"<param name=seriesLabels value=\"%s\">\n",
				list_display_delimited(
					easycharts_get_series_labels_list(
						output_datatype_list ), ',' ) );
		}
		fprintf(destination,
			"<param name=legendOn value=true>\n" );
		fprintf(destination,
			"<param name=legendPosition value=bottom>\n" );
		fprintf(destination,
			"<param name=legendFont value=\"Arial,%s%d\">\n",
			bold_legends_string,
			font_size );
	}

	if ( value_labels_on )
	{
		fprintf(destination,
			"<param name=valueLabelsOn value=true>\n" );
		fprintf(destination,
		"<param name=valueLabelFont value=\"Arial,%s%d\">\n",
			bold_labels_string,
			font_size );
	}

	if ( sample_scroller_on )
	{
		fprintf(destination,
			"<param name=sampleScrollerOn value=true>\n" );
	}

	if ( range_scroller_on )
	{
		fprintf(destination,
			"<param name=rangeAdjusterOn value=true>\n" );
	}

	fprintf(destination,"<param name=rangePosition value=left>\n" );
	fprintf(destination,"<param name=autoLabelSpacingOn value=true>\n" );

	if ( bar_chart )
	{
		fprintf(destination,
			"<param name=barLabelAngle value=%d>\n",
			sample_label_angle );
	}
	else
	{
		fprintf(destination,
			"<param name=sampleLabelAngle value=%d>\n",
			sample_label_angle );
	}

	fprintf(destination,
		"<param name=rangeLabelFont value=\"Arial,%s%d\">\n",
		bold_labels_string,
		font_size );

	fprintf(destination,
		"<param name=sampleLabelFont value=\"Arial,%s%d\">\n",
			bold_labels_string,
			font_size );

	fprintf(destination,
		"<param name=barLabelFont value=\"Arial,%s%d\">\n",
			bold_labels_string,
			font_size );

	if ( !value_lines_off )
	{
		fprintf(destination,"<param name=valueLinesOn value=true>\n" );
	}

	if ( xaxis_decimal_count )
	{
		fprintf(destination,
			"<param name=sampleDecimalCount value=%d>\n",
			xaxis_decimal_count );
	}

	if ( yaxis_decimal_count )
	{
		fprintf(destination,
			"<param name=rangeDecimalCount value=%d>\n",
			yaxis_decimal_count );
	}

	if ( range_step )
	{
		fprintf(destination,
			"<param name=rangeStep value=%d>\n",
			range_step );
	}

	if ( range_labels_off )
	{
		fprintf(destination,
			"<param name=rangeLabelsOff value=true>\n" );
	}

	if ( bar_labels_on )
	{
		fprintf(destination,
			"<param name=barLabelsOn value=true>\n" );

		if ( !bar_chart )
		{
			fprintf(destination,
			"<param name=barAlignment value=horizontal>\n" );
		}
	}

	if ( !bar_chart )
	{
		fprintf( destination,
			 "<param name=multiColorOn value=true>\n" );
	}

	fprintf( destination,
		 "<param name=sampleColors value=\"%s\">\n",
		 EASYCHARTS_COLOR_LIST_STRING );

	if ( double_range_adjusters )
	{
		fprintf( destination,
			 "<param name=seriesRange_1 value=2>\n"
			 "<param name=rangeOn_2 value=true>\n"
			 "<param name=rangeAdjusterPosition value=left>\n"
			 "<param name=rangeAdjusterOn_2 value=true>\n" );
	}

/*
	if ( timeline_info )
	{
		fprintf(destination,
			"<param name=timeFormatInput value=\"%s\">\n",
			timeline_info->time_format_input );
		fprintf(destination,
			"<param name=timeFormatOut value=\"%s\">\n",
			timeline_info->x_axis_ticklabel_format );
		fprintf(destination,
			"<param name=lowerTime value=\"%s\">\n",
			timeline_info->lowest_date_string );
		fprintf(destination,
			"<param name=upperTime value=\"%s\">\n",
			timeline_info->highest_date_string );
		fprintf(destination,
			"<param name=timeScale value=86400>\n" );
		fprintf(destination,
			"<param name=autoTimeLabelsOn value=true>\n" );
		fprintf(destination,
			"<param name=valueLinesOn value=true>\n" );
		fprintf(destination,
		"<param name=sampleLabelStyle value=below_and_floating>\n" );
	}
*/

	fprintf( destination, "</applet>\n" );

	return 1;
} /* easycharts_output_chart() */

void easycharts_output_point_list(	FILE *destination,
					LIST *point_list,
					boolean output_x_axis )
{
	EASYCHARTS_POINT *point;
	int first_time = 1;

	if ( !list_rewind( point_list ) ) return;

	do {
		point =
			(EASYCHARTS_POINT *)
				list_get_pointer( point_list );

		if ( first_time )
			first_time = 0;
		else
			fprintf( destination, "," );

		if ( output_x_axis )
		{
			if ( point->x_label )
			{
				fprintf(destination,
					"%s",
					point->x_label );
			}
			else
			{
				fprintf(destination,
					"%.3lf",
					point->x_value );
			}
		}
		else
		{
			if ( point->display_value )
			{
				fprintf(destination,
					"%.3lf",
					point->y_value );
			}
			else
			{
				fprintf(destination, "x" );
			}
		}
	} while( list_next( point_list ) );
} /* easycharts_output_point_list() */

void easycharts_get_chart_filename(
				char **chart_filename,
				char **prompt_filename,
				char *application_name,
				char *document_root_directory,
				int pid )
{
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
			application_get_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_get_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			EASYCHARTS_FILENAME_STEM,
			application_name,
			pid,
			(char *)0 /* session */,
			"html" );

	*chart_filename =
		appaserver_link_get_output_filename(
			appaserver_link_file->
				output_file->
				document_root_directory,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	*prompt_filename =
		appaserver_link_get_link_prompt(
			appaserver_link_file->
				link_prompt->
				prepend_http_boolean,
			appaserver_link_file->
				link_prompt->
				http_prefix,
			appaserver_link_file->
				link_prompt->server_address,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

/*
	sprintf(	local_chart_filename,
			EASYCHARTS_CHART_TEMPLATE,
			appaserver_mount_point,
			application_name,
			pid );

	if ( prepend_http_protocol )
	{
		server_address = appaserver_library_get_server_address();

		sprintf(	local_prompt_filename,
				EASYCHARTS_HTTP_PROMPT_TEMPLATE,
				application_get_http_prefix( application_name ),
				server_address,
				application_name,
				pid );
	}
	else
	{
		sprintf(	local_prompt_filename,
				EASYCHARTS_PROMPT_TEMPLATE,
				application_name,
				pid );
	}
*/

} /* easycharts_get_chart_filename() */

EASYCHARTS_INPUT_CHART *easycharts_new_input_chart( void )
{
	EASYCHARTS_INPUT_CHART *input_chart;

	input_chart =
		( EASYCHARTS_INPUT_CHART *)
			calloc( 1, sizeof( EASYCHARTS_INPUT_CHART ) );
	if ( !input_chart )
	{
		fprintf( stderr,
			 "ERROR in %s/%s(): cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	input_chart->datatype_list = list_new_list();
	input_chart->date_time_dictionary = dictionary_new_medium_dictionary();
	return input_chart;
} /* easycharts_new_input_chart() */

EASYCHARTS_INPUT_DATATYPE *easycharts_new_input_datatype(
			char *datatype_name,
			char *units )
{
	EASYCHARTS_INPUT_DATATYPE *input_datatype;

	input_datatype =
		( EASYCHARTS_INPUT_DATATYPE *)
			calloc( 1, sizeof( EASYCHARTS_INPUT_DATATYPE ) );
	if ( !input_datatype )
	{
		fprintf( stderr,
			 "ERROR in %s/%s(): cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	input_datatype->datatype_name = datatype_name;
	input_datatype->units = units;
	input_datatype->value_hash_table =
		hash_table_new_hash_table( hash_table_medium );

	return input_datatype;
} /* easycharts_new_input_datatype() */

LIST *easycharts_new_datatype_list(
			LIST *datatype_name_list,
			LIST *datatype_units_list )
{
	EASYCHARTS_INPUT_DATATYPE *input_datatype;
	LIST *datatype_list;
	char *datatype_name;
	char *units = {0};

	if ( !datatype_name_list
	||   !list_length( datatype_name_list ) )
	{
		return (LIST *)0;
	}

	if ( datatype_units_list
	&&   list_length( datatype_name_list ) !=
	     list_length( datatype_units_list ) )
	{
		return (LIST *)0;
	}

	datatype_list = list_new_list();
	list_rewind( datatype_name_list );

	if ( datatype_units_list )
		list_rewind( datatype_units_list );

	do {
		datatype_name =
			list_get_pointer(
				datatype_name_list );

		if ( datatype_units_list )
		{
			units = list_get_pointer( datatype_units_list );
		}

		input_datatype = easycharts_new_input_datatype(
					datatype_name,
					units );

		list_append_pointer( datatype_list, input_datatype );
		if ( datatype_units_list )
			list_next( datatype_units_list );

	} while( list_next( datatype_name_list ) );

	return datatype_list;
} /* easycharts_new_datatype_list() */

boolean easycharts_set_all_input_values(
			LIST *input_chart_list,
			char *input_sys_string,
			int datatype_piece,
			int date_time_piece,
			int value_piece,
			char delimiter )
{
	FILE *input_pipe;
	char datatype_name[ 128 ];
	char date_time_string[ 128 ];
	char value_string[ 128 ];
	char input_buffer[ 1024 ];
	boolean null_value;
	boolean got_input = 0;

	input_pipe = popen( input_sys_string, "r" );

	*datatype_name = '\0';

	while( get_line( input_buffer, input_pipe ) )
	{
		if ( !*input_buffer || *input_buffer == '#' )
		{
			continue;
		}

		if ( datatype_piece != -1 )
		{
			piece(	datatype_name,
				delimiter,
				input_buffer,
				datatype_piece );
		}

		piece(	date_time_string,
			delimiter,
			input_buffer,
			date_time_piece );

		piece(	value_string,
			delimiter,
			input_buffer,
			value_piece );

		null_value = ( *value_string ) ? 0 : 1;

/*
fprintf( stderr, "got datatype_name = (%s)\n", datatype_name );
fprintf( stderr, "got date_time_string = (%s)\n", date_time_string );
fprintf( stderr, "got value_string = (%s)\n", value_string );
*/

		if ( !easycharts_set_input_value(
			input_chart_list,
			datatype_name,
			strdup( date_time_string ),
			atof( value_string ),
			null_value ) )
		{
			return 0;
		}
		got_input = 1;
	}

	pclose( input_pipe );
	return got_input;
} /* easycharts_set_all_input_values() */

EASYCHARTS_INPUT_VALUE *easycharts_new_input_value(
				char *date_time,
				double value,
				boolean null_value )
{
	EASYCHARTS_INPUT_VALUE *e;

	e = (EASYCHARTS_INPUT_VALUE *)
		calloc( 1, sizeof( EASYCHARTS_INPUT_VALUE ) );
	if ( !e )
	{
		fprintf( stderr,
			 "ERROR in %s/%s(): cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	e->date_time = date_time;
	e->value = value;
	e->null_value = null_value;

	return e;
} /* easycharts_new_input_value() */

boolean easycharts_set_input_value(
				LIST *input_chart_list,
				char *datatype_name,
				char *date_time,
				double value,
				boolean null_value )
{
	EASYCHARTS_INPUT_VALUE *input_value;
	EASYCHARTS_INPUT_DATATYPE *datatype;
	EASYCHARTS_INPUT_CHART *input_chart;

	if ( !list_rewind( input_chart_list ) ) return 0;

	input_value =
		easycharts_new_input_value(
			date_time, value, null_value );

	do {
		input_chart = list_get_pointer( input_chart_list );

		if ( !*datatype_name
		||   list_item_exists(	input_chart->datatype_list,
					datatype_name,
					easycharts_datatype_compare ) )
		{
			datatype =
				list_get_current_pointer(
					input_chart->datatype_list );

			hash_table_set_pointer(
				datatype->value_hash_table,
				input_value->date_time,
				(void *)input_value );

			if ( input_chart->date_time_dictionary )
			{
				dictionary_set_pointer(
					input_chart->date_time_dictionary,
					input_value->date_time,
					"" );
			}
		}
	} while( list_next( input_chart_list ) );
	return 1;
} /* easycharts_set_input_value() */

int easycharts_point_compare(		EASYCHARTS_POINT *point1,
					EASYCHARTS_POINT *point2 )
{
	if ( point1->x_value < point2->x_value )
		return -1;
	else
	if ( point1->x_value == point2->x_value )
		return 0;
	else
		return 1;
}

int easycharts_datatype_compare(	EASYCHARTS_INPUT_DATATYPE *datatype,
					char *compare_datatype_name )
{
	return strcmp( datatype->datatype_name, compare_datatype_name );
}

int easycharts_get_y_minimum_integer( LIST *output_datatype_list )
{
	int rounded_min;

	rounded_min = easycharts_get_rounded_min_y( output_datatype_list );

	return rounded_min;
} /* easycharts_get_y_minimum_integer() */

int easycharts_get_rounded_max_y( LIST *point_list )
{
	EASYCHARTS_POINT *point;
	int rounded_max = -EASYCHARTS_MAX_VALUE;

	if ( list_rewind( point_list ) )
	{
		do {
			point =
				(EASYCHARTS_POINT *)
					list_get_pointer( point_list );
			if ( point->display_value
			&&   (int)point->y_value > rounded_max )
			{
				rounded_max = (int)point->y_value + 1;
			}
		} while( list_next( point_list ) );
	}
	return rounded_max;
} /* easycharts_get_rounded_max_y() */

int easycharts_get_rounded_min_y( LIST *output_datatype_list )
{
	EASYCHARTS_OUTPUT_DATATYPE *datatype;
	EASYCHARTS_POINT *point;
	double minimum = 9999999.0;

	if ( !list_rewind( output_datatype_list ) ) return 0;

	do {
		datatype = list_get_pointer( output_datatype_list );

		if ( datatype->point_list
		&&   list_rewind( datatype->point_list ) )
		{
			do {
				point =
				(EASYCHARTS_POINT *)
					list_get_pointer(
						datatype->point_list );
				if ( point->display_value
				&&   point->y_value < minimum )
				{
					minimum = point->y_value;
				}
			} while( list_next( datatype->point_list ) );
		}
	} while( list_next( output_datatype_list ) );
	return (int)(minimum - 0.999999);
} /* easycharts_get_rounded_min_y() */

EASYCHARTS_OUTPUT_CHART *easycharts_timeline_get_output_chart(
					LIST *datatype_list,
					LIST *date_time_key_list )
{
	EASYCHARTS_INPUT_DATATYPE *input_datatype;
	EASYCHARTS_OUTPUT_CHART *output_chart;
	EASYCHARTS_OUTPUT_DATATYPE *output_datatype;
	EASYCHARTS_INPUT_VALUE *input_value;
	char *date_time_key;

	if ( !datatype_list
	||   !list_rewind( datatype_list )
	||   !list_length( date_time_key_list ) )
	{
		return (EASYCHARTS_OUTPUT_CHART *)0;
	}

	output_chart = easycharts_new_output_chart();

	do {
		input_datatype =
			list_get_pointer( datatype_list );

		output_datatype =
			easycharts_new_output_datatype(
				input_datatype->datatype_name,
				input_datatype->units );

		list_append_pointer(	output_chart->output_datatype_list,
					output_datatype );

		list_rewind( date_time_key_list );

		do {
			date_time_key = list_get_pointer( date_time_key_list );

			input_value =
				hash_table_get_pointer(
					input_datatype->value_hash_table,
					date_time_key );

			if ( !input_value )
			{
				if ( !easycharts_set_point(
						output_datatype->point_list,
						0.0,
						0.0,
						date_time_key /* x_label */,
						0 /* dont display_value */ ) )
				{
					return (EASYCHARTS_OUTPUT_CHART *)0;
				}
			}
			else
			{
				if ( !easycharts_set_point(
					output_datatype->point_list,
					0.0 /* x_value */,
					input_value->value /* y_value */,
					date_time_key /* x_label */,
					1 - input_value->null_value ) )
				{
					return (EASYCHARTS_OUTPUT_CHART *)0;
				}
			}

		} while( list_next( date_time_key_list ) );
	} while( list_next( datatype_list ) );
	return output_chart;

} /* easycharts_timeline_get_output_chart() */

EASYCHARTS_OUTPUT_DATATYPE *easycharts_new_output_datatype(
				char *datatype_name,
				char *units )
{
	EASYCHARTS_OUTPUT_DATATYPE *a;

	a = (EASYCHARTS_OUTPUT_DATATYPE *)
		calloc( 1, sizeof( EASYCHARTS_OUTPUT_DATATYPE ) );

	if ( !a )
	{
		fprintf( stderr,
			 "ERROR in %s/%s(): cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	a->datatype_name = datatype_name;
	a->units = units;
	a->point_list = list_new_list();
	return a;
}

void easycharts_output_y_minimum_integer(
				FILE *destination,
				int y_minimum_integer )
{
	fprintf(destination,
	 		"<param name=lowerRange value=\"%d\">\n",
			y_minimum_integer );
}

LIST *easycharts_get_series_labels_list( LIST *output_datatype_list )
{
	EASYCHARTS_OUTPUT_DATATYPE *datatype;
	LIST *series_labels_list = list_new_list();
	char series_label[ 512 ];

	if ( list_rewind( output_datatype_list ) )
	{
		do {
			datatype = list_get_pointer( output_datatype_list );

			if ( datatype->units && *datatype->units )
			{
				sprintf( series_label,
					 "%s (%s)",
					 datatype->datatype_name,
					 datatype->units );
			}
			else
			{
				strcpy( series_label, datatype->datatype_name );
			}

			strcpy( series_label,
				easycharts_series_label_convert_comma(
					series_label ) );

			format_initial_capital( series_label, series_label );
			list_append_pointer(	series_labels_list,
						strdup( series_label ) );
		} while( list_next( output_datatype_list ) );
	}

	return series_labels_list;
} /* easycharts_get_series_labels_list() */

char *easycharts_series_label_convert_comma( char *series_label )
{
	static char buffer[ 128 ];

	strcpy( buffer, series_label );
	search_replace_character(
			buffer,
			',',
			'-' );
	return buffer;
} /* easycharts_series_label_convert_comma() */

double easycharts_get_y_point_average( LIST *output_chart_list )
{
	EASYCHARTS_OUTPUT_CHART *output_chart;
	EASYCHARTS_OUTPUT_DATATYPE *datatype;
	EASYCHARTS_POINT *point;
	double sum = 0.0;
	int count = 0;

	if ( !list_length( output_chart_list ) ) return 0.0;

	output_chart = list_get_first_pointer( output_chart_list );

	if ( !list_length( output_chart->output_datatype_list ) )
		return 0.0;

	datatype = list_get_first_pointer(
					output_chart->output_datatype_list );
	if ( datatype->point_list
	&&   list_rewind( datatype->point_list ) )
	{
		do {
			point =
				list_get_pointer(
					datatype->point_list );

			if ( point->display_value )
			{
				sum += point->y_value;
				count++;
			}
		} while( list_next( datatype->point_list ) );
	}

	if ( count )
		return sum / (double)count;
	else
		return 0.0;
} /* easycharts_get_y_point_average() */

double easycharts_get_y_point_range( LIST *output_chart_list )
{
	EASYCHARTS_OUTPUT_CHART *output_chart;
	EASYCHARTS_OUTPUT_DATATYPE *datatype;
	EASYCHARTS_POINT *point;
	/*
	double highest = DBL_MIN;
	double lowest = DBL_MAX;
	*/
	double highest = DOUBLE_MINIMUM;
	double lowest = DOUBLE_MAXIMUM;

	if ( !list_length( output_chart_list ) ) return 0.0;

	output_chart = list_get_first_pointer( output_chart_list );

	if ( !list_length( output_chart->output_datatype_list ) )
		return 0.0;

	datatype = list_get_first_pointer(
					output_chart->output_datatype_list );
	if ( datatype->point_list
	&&   list_rewind( datatype->point_list ) )
	{
		do {
			point =
				list_get_pointer(
					datatype->point_list );

			if ( point->display_value )
			{
				if ( point->y_value > highest )
					highest = point->y_value;
				else
				if ( point->y_value < lowest )
					lowest = point->y_value;
			}
		} while( list_next( datatype->point_list ) );
	}

	return highest - lowest;
} /* easycharts_get_y_point_range() */

int easycharts_get_yaxis_decimal_count( LIST *output_chart_list )
{
	double y_point_range;

	y_point_range = easycharts_get_y_point_range( output_chart_list );

	if ( y_point_range > 30.0 )
		return 0;
	else
	if ( y_point_range > 20.0 )
		return 1;
	else
	if ( y_point_range > 10.0 )
		return 2;
	else
		return 3;
} /* easycharts_get_yaxis_decimal_count() */

int easycharts_get_range_step( LIST *output_chart_list )
{
	double abs_y_point_average;
	int range_step;

	abs_y_point_average =
		abs_float(
			easycharts_get_y_point_average(
				output_chart_list ) );

	if ( abs_y_point_average > 10 )
		range_step = 0;
	else
	if ( abs_y_point_average > 5.0 )
		range_step = 2;
	else
		range_step = 1;

	return range_step;
} /* easycharts_get_range_step() */

#ifdef NOT_DEFINED
int easycharts_get_point_count( LIST *dataset_list )
{
	EASYCHARTS_OUTPUT_DATASET *dataset;

	if ( !list_length( dataset_list ) )
		return 0;

	dataset =
		(EASYCHARTS_OUTPUT_DATASET *)
			list_get_first_pointer( dataset_list );
	return list_length( dataset->point_list );

} /* easycharts_get_point_count() */
#endif

void easycharts_trim_label_year_to_two( LIST *output_chart_list )
{
	EASYCHARTS_OUTPUT_CHART *output_chart;
	EASYCHARTS_OUTPUT_DATATYPE *output_datatype;
	EASYCHARTS_POINT *point;
	char buffer[ 3 ] = {0};

	if ( !list_rewind( output_chart_list ) ) return;

	do {
		output_chart = list_get_pointer( output_chart_list );

		if ( !list_rewind( output_chart->output_datatype_list ) )
			continue;

		do {
			output_datatype =
				list_get_pointer( 
					output_chart->output_datatype_list );

			if ( !list_rewind( output_datatype->point_list ) )
				continue;
	
			do {
				point =
					list_get_pointer(
						output_datatype->point_list );
	
				if ( point->x_label
				&&  *point->x_label
				&&  strlen( point->x_label ) >= 4 )
				{
					*buffer = *(point->x_label + 2);
					*(buffer + 1 )= *(point->x_label + 3 );
					point->x_label = strdup( buffer );
				}
			} while( list_next( output_datatype->point_list ) );
		} while( list_next( output_chart->output_datatype_list ) );
	} while( list_next( output_chart_list ) );
} /* easycharts_trim_label_year_to_two() */

void easycharts_output_html( FILE *chart_file )
{
	fprintf(chart_file,
		"<html><head><title>Easychart</title></head>\n"
		"<body onload=\"javascript:status='';\"></body></html>\n" );
}

void easycharts_set_printer_sized(
				int *height,
				int *width,
				int *font_size,
				boolean *bold_labels,
				boolean *bold_legends )
{
	*height = (double)*height / 1.8;
	*width = (double)*width / 1.8;
	*font_size = 12;
	*bold_labels = 0;
	*bold_legends = 0;
} /* easycharts_set_printer_sized() */

void easycharts_set_half_height(
				int *height,
				char printer_sized_yn )
{
	if ( printer_sized_yn == 'y' )
	{
		*height = (double)*height / 1.4;
	}
	else
	{
		*height = (double)*height / 2.0;
	}
} /* easycharts_set_half_height() */

#ifdef NOT_DEFINED
char *easycharts_xy_input_dataset_list_display(
				LIST *input_dataset_list,
				DICTIONARY *label_key_dictionary )
{
	char return_buffer[ 65536 ];
	char *return_buffer_pointer = return_buffer;
	EASYCHARTS_XY_VALUE *xy_value;
	EASYCHARTS_XY_DATASET *dataset;
	char *label;
	LIST *label_key_list;

	label_key_list =
		dictionary_get_ordered_key_list(
			label_key_dictionary );

	if ( !label_key_list
	||   !list_length( label_key_list ) )
	{
		return "";
	}

	if ( !list_rewind( input_dataset_list ) ) return "";

	*return_buffer_pointer = '\0';
	do {
		dataset =
			(EASYCHARTS_XY_DATASET *)
				list_get_pointer(
					input_dataset_list );

		list_rewind( label_key_list );

		do {
			label = list_get_pointer( label_key_list );

			xy_value =
				hash_table_get_pointer(
					dataset->dataset_hash_table,
					label );

			if ( !xy_value )
			{
				return_buffer_pointer +=
					sprintf(
						return_buffer_pointer,
						"%s:0,1",
						label );
			}
			else
			{
				return_buffer_pointer +=
					sprintf(
						return_buffer_pointer,
						"%s:%lf,%d",
						label,
						xy_value->value,
						xy_value->null_value );
			}

			return_buffer_pointer +=
				sprintf( return_buffer_pointer, "\n" );

		} while( list_next( label_key_list ) );
	} while( list_next( input_dataset_list ) );
	return strdup( return_buffer );
} /* easycharts_xy_input_dataset_list_display() */

DICTIONARY *easycharts_remove_key_if_any_missing_key_dictionary(
				LIST *input_dataset_list,
				DICTIONARY *label_key_dictionary )
{
	EASYCHARTS_XY_VALUE *xy_value;
	EASYCHARTS_XY_DATASET *dataset;
	char *label;
	LIST *label_key_list;

	label_key_list =
		dictionary_get_ordered_key_list(
			label_key_dictionary );

	if ( !label_key_list
	||   !list_length( label_key_list ) )
	{
		return label_key_dictionary;
	}

	if ( !list_rewind( input_dataset_list ) ) return label_key_dictionary;

	do {
		dataset =
			(EASYCHARTS_XY_DATASET *)
				list_get_pointer(
					input_dataset_list );

		list_rewind( label_key_list );

		do {
			label = list_get_pointer( label_key_list );

			xy_value =
				hash_table_get_pointer(
					dataset->dataset_hash_table,
					label );

			if ( !xy_value || xy_value->null_value )
			{
				dictionary_delete_key(
					label_key_dictionary,
					label );

			}
		} while( list_next( label_key_list ) );
	} while( list_next( input_dataset_list ) );
	return label_key_dictionary;
} /* easycharts_remove_key_if_any_missing_key_dictionary() */
#endif

#ifdef NOT_DEFINED
EASYCHARTS_TIMELINE_INFO *easycharts_new_timeline_info(
				char *time_format_input,
				char *x_axis_ticklabel_format,
				char *lowest_date_string,
				char *highest_date_string )
{
	EASYCHARTS_TIMELINE_INFO *e;

	e = (EASYCHARTS_TIMELINE_INFO *)
		calloc( 1, sizeof( EASYCHARTS_TIMELINE_INFO ) );
	if ( !e )
	{
		fprintf( stderr,
			 "ERROR in %s/%s(): cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	e->time_format_input = time_format_input;
	e->x_axis_ticklabel_format = x_axis_ticklabel_format;
	e->lowest_date_string = lowest_date_string;
	e->highest_date_string = highest_date_string;
	return e;
} /* easycharts_new_timeline_info() */
#endif

EASYCHARTS_OUTPUT_CHART *easycharts_new_output_chart( void )
{
	EASYCHARTS_OUTPUT_CHART *a;

	a = (EASYCHARTS_OUTPUT_CHART *)
		calloc( 1, sizeof( EASYCHARTS_OUTPUT_CHART ) );
	a->output_datatype_list = list_new_list();
	return a;
}

LIST *easycharts_get_regression_point_list(
					double *slope,
					double *y_intercept,
					double *root_mean_squared_error,
					LIST *point_list )
{
	FILE *file;
	char buffer[ 256 ];
	char output_filename[ 128 ];
	EASYCHARTS_POINT *point;
	LIST *regression_point_list;
	double new_y;

	if ( !list_rewind( point_list ) ) return (LIST *)0;

	sprintf( output_filename, "/tmp/regression_%d.dat", getpid() );
	sprintf( buffer, "regression.e > %s", output_filename );
	file = popen( buffer, "w" );

	do {
		point = list_get_pointer( point_list );
		fprintf(file,
			"%lf %lf\n",
			point->x_value,
			point->y_value );
	} while( list_next( point_list ) );

	pclose( file );

	file = fopen( output_filename, "r" );
	if ( !get_line( buffer, file ) )
	{
		fclose( file );
		return (LIST *)0;
	}
	fclose( file );
	sscanf(	buffer,
		"%lf %lf %lf",
		slope,
		y_intercept,
		root_mean_squared_error );

	list_rewind( point_list );
	regression_point_list = list_new_list();

	do {
		point = list_get_pointer( point_list );

		new_y = (*slope * point->x_value) + *y_intercept;

		point = easycharts_new_point(
					point->x_value,
					new_y,
					(char *)0 /* x_label */ );

		list_append_pointer(
			regression_point_list,
			point );
	} while( list_next( point_list ) );

	sprintf( buffer, "rm -f %s", output_filename );
	if ( system( buffer ) ){};
	return regression_point_list;
}

void easycharts_output_graph_window(
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

	printf( "<h1>Easycharts Chart Viewer " );
	fflush( stdout );
	if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ){};
	printf( "</h1>\n" );

	if ( where_clause && *where_clause )
		printf( "<br>Search criteria: %s\n", where_clause );

	printf( "<br><hr><a href=\"%s\" target=%s>Press to view chart.</a>\n",
		prompt_filename,
		window_name );

	easycharts_output_home_link();

	if ( with_document_output ) document_close();

} /* easycharts_output_graph_window() */

void easycharts_output_home_link( void )
{
	printf(
"<br><p>For more information, visit the <a href=\"http://objectplanet.com/easycharts\" target=_new>Easycharts Home Page.</a>" );
}

LIST *easycharts_timeline_get_output_chart_list( LIST *input_chart_list )
{
	EASYCHARTS_INPUT_CHART *input_chart;
	EASYCHARTS_OUTPUT_CHART *output_chart;
	LIST *output_chart_list;
	LIST *date_time_key_list;

	if ( !list_rewind( input_chart_list ) ) return (LIST *)0;

	output_chart_list = list_new_list();

	do {
		input_chart = list_get_pointer( input_chart_list );

		date_time_key_list =
			dictionary_get_ordered_key_list(
				input_chart->date_time_dictionary );

		if ( !date_time_key_list
		||   !list_length( date_time_key_list ) )
		{
			continue;
		}

		output_chart = easycharts_timeline_get_output_chart(
					input_chart->datatype_list,
					date_time_key_list );

		/* Copy input parameters to output */
		/* ------------------------------- */
		output_chart->bar_chart = input_chart->bar_chart;
		output_chart->bar_labels_on = input_chart->bar_labels_on;
		output_chart->x_axis_label = input_chart->x_axis_label;
		output_chart->y_axis_label = input_chart->y_axis_label;
		output_chart->double_range_adjusters =
			input_chart->double_range_adjusters;
		output_chart->applet_library_code =
			input_chart->applet_library_code;

		if ( !output_chart ) return (LIST *)0;
		list_append_pointer( output_chart_list, output_chart );
		list_free_string_container( date_time_key_list );
	} while( list_next( input_chart_list ) );

	return output_chart_list;
} /* easycharts_timeline_get_output_chart_list() */

LIST *easycharts_scatter_get_output_chart_list(
				LIST *input_chart_list )
{
	EASYCHARTS_INPUT_CHART *input_chart;
	EASYCHARTS_OUTPUT_CHART *output_chart;
	LIST *output_chart_list;
	LIST *date_time_key_list;

	if ( !list_rewind( input_chart_list ) ) return (LIST *)0;

	output_chart_list = list_new_list();

	do {
		input_chart = list_get_pointer( input_chart_list );

		date_time_key_list =
			dictionary_get_key_list(
				input_chart->date_time_dictionary );

		if ( !date_time_key_list
		||   !list_length( date_time_key_list ) )
		{
			continue;
		}

		output_chart = easycharts_scatter_get_output_chart(
					input_chart->datatype_list,
					date_time_key_list,
					input_chart->y_axis_label );

		/* Copy input parameters to output */
		/* ------------------------------- */
		output_chart->bar_chart = input_chart->bar_chart;
		output_chart->bar_labels_on = input_chart->bar_labels_on;
		output_chart->x_axis_label = input_chart->x_axis_label;
/*
		output_chart->y_axis_label = input_chart->y_axis_label;
*/

		output_chart->applet_library_code =
			input_chart->applet_library_code;

		if ( !output_chart ) return (LIST *)0;
		list_append_pointer( output_chart_list, output_chart );
		list_free_string_container( date_time_key_list );
	} while( list_next( input_chart_list ) );

	return output_chart_list;
} /* easycharts_scatter_get_output_chart_list() */

EASYCHARTS_OUTPUT_CHART *easycharts_scatter_get_output_chart(
			LIST *datatype_list,
			LIST *date_time_key_list,
			char *input_chart_y_axis_label )
{
	EASYCHARTS_OUTPUT_CHART *output_chart;
	EASYCHARTS_OUTPUT_DATATYPE *output_datatype;
	EASYCHARTS_OUTPUT_DATATYPE *regression_datatype;
	EASYCHARTS_INPUT_DATATYPE *datatype_1;
	EASYCHARTS_INPUT_DATATYPE *datatype_2;
	char y_axis_label[ 128 ];
	double slope;
	double y_intercept;
	double root_mean_squared_error;
	char units_label[ 128 ];

	if ( list_length( datatype_list ) != 2 )
	{
		fprintf( stderr,
			"ERROR in %s/%s(): expected only 2 datatypes\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	output_chart = easycharts_new_output_chart();

	list_rewind( datatype_list );
	datatype_1 = list_get_pointer( datatype_list );
	list_next( datatype_list );
	datatype_2 = list_get_pointer( datatype_list );

	output_datatype = easycharts_new_output_datatype(
				datatype_2->datatype_name,
				datatype_2->units );
	list_append_pointer(
		output_chart->output_datatype_list,
		output_datatype );

	output_datatype->point_list =
		easycharts_scatter_get_point_list(
				datatype_1,
				datatype_2,
				date_time_key_list );

	regression_datatype =
		easycharts_new_output_datatype(
			"regression",
			(char *)0 /* units */ );

	list_append_pointer(
		output_chart->output_datatype_list,
		regression_datatype );

	regression_datatype->point_list =
		easycharts_get_regression_point_list(
			&slope,
			&y_intercept,
			&root_mean_squared_error,
			output_datatype->point_list );

	if ( datatype_2->units
	&&   *datatype_2->units )
	{
		sprintf( units_label, "(%s)", datatype_2->units );
	}
	else
	{
		*units_label = '\0';
	}

	if ( y_intercept >= 0.0 )
	{
		sprintf(y_axis_label,
		 "%s\\ny = %.2lfx + %.2lf (%s = %.2lf)",
			input_chart_y_axis_label,
		 	slope,
		 	y_intercept,
			RESIDUAL_STANDARD_ESTIMATE,
			root_mean_squared_error );
	}
	else
	{
		sprintf(y_axis_label,
		 "%s\\ny = %.2lfx - %.2lf (%s = %.2lf)",
			input_chart_y_axis_label,
		 	slope,
		 	fabs( y_intercept ),
			RESIDUAL_STANDARD_ESTIMATE,
			root_mean_squared_error );
	}

	output_chart->y_axis_label = strdup( y_axis_label );

	return output_chart;

} /* easycharts_scatter_get_output_chart() */

LIST *easycharts_scatter_get_point_list(
				EASYCHARTS_INPUT_DATATYPE *datatype_1,
				EASYCHARTS_INPUT_DATATYPE *datatype_2,
				LIST *date_time_key_list )
{
	LIST *point_list;
	EASYCHARTS_POINT *point;
	char *date_time_key;
	EASYCHARTS_INPUT_VALUE *datatype_1_value;
	EASYCHARTS_INPUT_VALUE *datatype_2_value;

	if ( !list_rewind( date_time_key_list ) ) return (LIST *)0;

	point_list = list_new_list();

	do {
		date_time_key = list_get_pointer( date_time_key_list );

		if ( ( datatype_1_value =
			hash_table_get_pointer(
				datatype_1->value_hash_table,
				date_time_key ) ) )
		{
			if ( ( datatype_2_value =
				hash_table_get_pointer(
					datatype_2->value_hash_table,
					date_time_key ) ) )
			{
				if ( datatype_1_value->null_value
				||   datatype_2_value->null_value )
				{
					continue;
				}

				point =
					easycharts_new_point(
						datatype_2_value->value,
						datatype_1_value->value,
						(char *)0 /* x_label */ );

				list_add_pointer_in_order(
					point_list,
					point,
					easycharts_point_compare );
			}
		}
	} while( list_next( date_time_key_list ) );
	return point_list;
} /* easycharts_scatter_get_point_list() */

char *easycharts_output_chart_list_display( LIST *output_chart_list )
{
	char buffer[ 65536 ];
	char *ptr = buffer;
	EASYCHARTS_OUTPUT_CHART *output_chart;
	EASYCHARTS_OUTPUT_DATATYPE *output_datatype;
	EASYCHARTS_POINT *output_point;

	if ( !list_rewind( output_chart_list ) ) return "";

	*ptr = '\0';
	do {
		output_chart = list_get_pointer( output_chart_list );

		if ( !output_chart->output_datatype_list
		||   !list_rewind( output_chart->output_datatype_list ) )
		{
			continue;
		}

		do {
			output_datatype =
				list_get_pointer(
					output_chart->output_datatype_list );

			ptr += sprintf(	ptr,
					"datatype = %s:",
					output_datatype->datatype_name );

			if ( !output_datatype->point_list
			||   !list_rewind( output_datatype->point_list ) )
			{
				continue;
			}

			do {
				output_point =
					list_get_pointer(
						output_datatype->point_list );

				if ( output_point->x_label )
				{
					ptr += sprintf(	ptr,
							"(%s,%lf)",
							output_point->x_label,
							output_point->y_value );
				}
				else
				{
					ptr += sprintf(	ptr,
							"(%lf,%lf)",
							output_point->x_value,
							output_point->y_value );
				}
			} while( list_next( output_datatype->point_list ) );
			ptr += sprintf( ptr, "\n" );
		} while( list_next( output_chart->output_datatype_list ) );
		ptr += sprintf( ptr, "\n" );
	} while( list_next( output_chart_list ) );

	*ptr = '\0';
	return strdup( buffer );
} /* easycharts_output_chart_list_display() */

DICTIONARY *easycharts_remove_key_if_any_missing_key_dictionary(
				LIST *input_datatype_list,
				DICTIONARY *date_time_dictionary )
{
	EASYCHARTS_INPUT_VALUE *input_value;
	EASYCHARTS_INPUT_DATATYPE *input_datatype;
	char *date_time;
	LIST *date_time_list;

	date_time_list =
		dictionary_get_key_list(
			date_time_dictionary );

	if ( !date_time_list
	||   !list_length( date_time_list ) )
	{
		return date_time_dictionary;
	}

	if ( !list_rewind( input_datatype_list ) ) return date_time_dictionary;

	do {
		input_datatype = list_get_pointer( input_datatype_list );

		list_rewind( date_time_list );

		do {
			date_time = list_get_pointer( date_time_list );

			input_value =
				hash_table_get_pointer(
					input_datatype->value_hash_table,
					date_time );

			if ( !input_value || input_value->null_value )
			{
				dictionary_delete_key(
					date_time_dictionary,
					date_time );

			}
		} while( list_next( date_time_list ) );
	} while( list_next( input_datatype_list ) );
	return date_time_dictionary;
} /* easycharts_remove_key_if_any_missing_key_dictionary() */

