/* $APPASERVER_HOME/library/grace_retire.c		*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "list.h"
#include "piece.h"
#include "column.h"
#include "julian.h"
#include "timlib.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "environ.h"
#include "document.h"
#include "application.h"
#include "role.h"
#include "appaserver_link.h"
#include "float.h"
#include "grace_retire.h"

void grace_output_optional_label_list(
			FILE *output_pipe,
			LIST *optional_label_list )
{
	GRACE_OPTIONAL_LABEL *grace_optional_label;
	int counter = 0;
	int length = list_length( optional_label_list );

	do {
		grace_optional_label =
			list_get(
				optional_label_list );

		if ( !grace_optional_label->optional_label
		||   !*grace_optional_label->optional_label )
		{
			continue;
		}

		if ( !counter )
		{
			fprintf( output_pipe,
		 		"@    xaxis  tick spec type both\n"
		 		"@    xaxis  tick spec %d\n"
			 	"@    xaxis  tick minor 0, %lf\n",
		 		length + 2,
			 	grace_optional_label->x - 1.0 );
		}

		fprintf( output_pipe,
			 "@    xaxis  tick major %d, %lf\n"
			 "@    xaxis  ticklabel %d, \"%s\"\n",
			 counter + 1,
			 grace_optional_label->x,
			 counter + 1,
			 grace_optional_label->optional_label );

		counter++;

	} while( list_next( optional_label_list ) );

	fprintf( output_pipe,
		 "@    xaxis  tick minor %d, %d\n",
		 length + 1,
		 length );

}

void grace_output_data(
			FILE *output_pipe,
			LIST *graph_list, 
			char *title,
			char *sub_title,
			char *xaxis_ticklabel_format,
			double x_label_size,
			enum grace_graph_type grace_graph_type,
			int xaxis_ticklabel_precision,
			LIST *optional_label_list,
			boolean symbols,
			double world_min_x,
			double world_max_x,
			boolean is_datatype_overlay_anchor_graph )
{
	GRACE_GRAPH *grace_graph;
	GRACE_DATATYPE *datatype;
	char *title_output;
	char *sub_title_output;
	LIST *datatype_list;
	char *units;
	char *legend;
	double y_label_size;
	static int graph_number = 0;
	double title_char_size = DEFAULT_TITLE_CHAR_SIZE;
	double subtitle_char_size = DEFAULT_SUBTITLE_CHAR_SIZE;
	int str_len;

	if ( !list_rewind( graph_list ) ) return;

	if ( !title ) title = "";
	if ( !sub_title ) sub_title = "";

	str_len = strlen( sub_title );

	if ( str_len > 130 )
		subtitle_char_size -= 0.4;
	else
	if ( str_len > 90 )
		subtitle_char_size -= 0.3;
	else
	if ( str_len > 60 )
		subtitle_char_size -= 0.2;

	str_len = strlen( title );

	if ( str_len > 50 ) title_char_size -= 0.2;

	do {
		grace_graph = list_get( graph_list );

		datatype_list = grace_graph->datatype_list;

		if ( grace_graph_type == unit_graph
		||   grace_graph_type == xy_graph )
			units = grace_graph->units;
		else
			units = "";

		y_label_size = grace_graph->y_label_size;

		/* Place the title on the last non-anchor graph */
		/* -------------------------------------------- */
		if ( !is_datatype_overlay_anchor_graph
		&&   list_at_end( graph_list ) )
		{
			title_output = title;
			sub_title_output = sub_title;
		}
		else
		{
			title_output = "";
			sub_title_output = "";
		}

		if ( grace_graph->world_min_y == grace_graph->world_max_y )
		{
			grace_graph->world_min_y--;
			grace_graph->world_max_y++;
		}

		grace_output_graph_heading(
				output_pipe,
				grace_graph, 
				graph_number,
				title_output,
				sub_title_output,
				xaxis_ticklabel_format,
				x_label_size,
				grace_graph->legend_char_size,
				units,
				y_label_size,
				grace_graph->yaxis_label_place_string,
				grace_graph->yaxis_ticklabel_place_string,
				grace_graph->xaxis_ticklabel_on_off,
				grace_graph->xaxis_tickmarks_on_off,
				grace_graph->legend_view_x,
				grace_graph->legend_view_y,
				grace_graph->y_invert_on_off,
				grace_graph->yaxis_tick_place_string,
				world_min_x,
				world_max_x,
				title_char_size,
				subtitle_char_size,
				xaxis_ticklabel_precision,
				grace_graph->xaxis_label,
				grace_graph->yaxis_grid_lines,
				optional_label_list,
				grace_graph->xaxis_ticklabel_stagger,
				grace_graph->world_min_y,
				grace_graph->world_max_y,
				grace_graph->xaxis_ticklabel_angle,
				grace_graph->yaxis_ticklabel_precision,
				grace_graph->yaxis_ticklabel_on_off,
				grace_graph->yaxis_tickmarks_on_off );

		if ( grace_graph->horizontal_line_at_point )
		{
			grace_output_horizontal_line_at_point(
				output_pipe,
				grace_graph->horizontal_line_at_point );
		}

		if ( !list_rewind( datatype_list ) )
		{
			fprintf(stderr,
				"ERROR in %s.%s(): empty datatype_list\n",
				__FILE__,
				__FUNCTION__ );
			exit( 1 );
		}

		do {
			datatype = list_get( datatype_list );

			if ( datatype->nodisplay_legend )
			{
				legend = "";
			}
			else
			{
				legend = datatype->legend;
			}

			grace_output_datatype(
				output_pipe,
				datatype,
				graph_number,
				legend,
				symbols );

		} while( list_next( datatype_list ) );

		graph_number++;

	} while( list_next( graph_list ) );

}

void grace_output_datatype(
				FILE *output_pipe,
				GRACE_DATATYPE *datatype,
				int graph_number,
				char *legend,
				boolean symbols )
{
	GRACE_DATASET *dataset;

	if ( !list_rewind( datatype->dataset_list ) ) return;

	if ( !legend ) legend = "";

	do {
		dataset = list_get( datatype->dataset_list );

		if ( list_at_head( datatype->dataset_list ) )
		{
			grace_output_full_dataset_heading(	
				output_pipe,
			   	dataset->line_color,
			   	dataset->dataset_number,
				legend,
				datatype->line_linestyle,
				datatype->annotated_value_on_off,
				symbols,
				datatype->datatype_type_bar_xy_xyhilo,
				datatype->symbol_size );
		}
		else
		{
			grace_output_abbreviated_dataset_heading(
					output_pipe,
				   	dataset->dataset_number,
				   	dataset->line_color,
				   	datatype->line_linestyle,
					symbols,
					datatype->symbol_size,
					datatype->datatype_type_bar_xy_xyhilo,
					legend );
		}

		if ( grace_output_point_list( 
				output_pipe,
				dataset->dataset_number,
				dataset->point_list,
				graph_number,
				datatype->datatype_type_bar_xy_xyhilo ) )
		{
			legend = "";
		}

	} while( list_next( datatype->dataset_list ) );

}

/*
"@timestamp def \"Thu Aug 16 17:20:38 2001\"\n"
*/

void grace_output_top_heading(	FILE *output_pipe,
				int page_width_pixels,
				int page_length_pixels )
{
	fprintf( output_pipe,
"# Grace project file\n"
"# -----------------------\n"
"# Automatically generated\n"
"# -----------------------\n"
"#\n"
"@version 50103\n"
"@page size %d, %d\n"
"@page scroll 5%c\n"
"@page inout 5%c\n"
"@link page off\n"
"@map font 0 to \"Times-Roman\", \"Times-Roman\"\n"
"@map font 1 to \"Times-Italic\", \"Times-Italic\"\n"
"@map font 2 to \"Times-Bold\", \"Times-Bold\"\n"
"@map font 3 to \"Times-BoldItalic\", \"Times-BoldItalic\"\n"
"@map font 4 to \"Helvetica\", \"Helvetica\"\n"
"@map font 5 to \"Helvetica-Oblique\", \"Helvetica-Oblique\"\n"
"@map font 6 to \"Helvetica-Bold\", \"Helvetica-Bold\"\n"
"@map font 7 to \"Helvetica-BoldOblique\", \"Helvetica-BoldOblique\"\n"
"@map font 8 to \"Courier\", \"Courier\"\n"
"@map font 9 to \"Courier-Oblique\", \"Courier-Oblique\"\n"
"@map font 10 to \"Courier-Bold\", \"Courier-Bold\"\n"
"@map font 11 to \"Courier-BoldOblique\", \"Courier-BoldOblique\"\n"
"@map font 12 to \"Symbol\", \"Symbol\"\n"
"@map font 13 to \"ZapfDingbats\", \"ZapfDingbats\"\n"
"@map color 0 to (255, 255, 255), \"white\"\n"
"@map color 1 to (0, 0, 0), \"black\"\n"
"@map color 2 to (255, 0, 0), \"red\"\n"
"@map color 3 to (0, 0, 255), \"blue\"\n"
"@map color 4 to (0, 255, 0), \"green\"\n"
"@map color 5 to (255, 165, 0), \"orange\"\n"
"@map color 6 to (188, 143, 143), \"brown\"\n"
"@map color 7 to (148, 0, 211), \"violet\"\n"
"@map color 8 to (0, 255, 255), \"cyan\"\n"
"@map color 9 to (255, 0, 255), \"magenta\"\n"
"@map color 10 to (103, 7, 72), \"maroon\"\n"
"@map color 11 to (114, 33, 188), \"indigo\"\n"
"@map color 12 to (0, 125, 255), \"gr_blue\"\n"
"@map color 13 to (64, 224, 208), \"turquoise\"\n"
"@map color 14 to (0, 139, 0), \"green4\"\n"
"@map color 15 to (220, 220, 220), \"grey\"\n"
"@reference date 0\n"
"@date wrap on\n"
"@date wrap year 1900\n"
"@default linewidth 1.0\n"
"@default linestyle 1\n"
"@default color 1\n"
"@default pattern 1\n"
"@default font 6\n"
"@default char size 1.000000\n"
"@default symbol size 1.000000\n"
"@default sformat \"%c16.8f\"\n"
"@background color 0\n"
"@page background fill on\n"
"@timestamp off\n"
"@timestamp 0.03, 0.03\n"
"@timestamp color 1\n"
"@timestamp rot 0\n"
"@timestamp font 6\n"
"@timestamp char size 1.000000\n"

"@r0 type above\n"
"@r0 linestyle 1\n"
"@r0 linewidth 1.0\n"
"@r0 color 1\n"
"@r0 line 0, 0, 0, 0\n"
"@r1 type above\n"
"@r1 linestyle 1\n"
"@r1 linewidth 1.0\n"
"@r1 color 1\n"
"@r1 line 0, 0, 0, 0\n"
"@r2 type above\n"
"@r2 linestyle 1\n"
"@r2 linewidth 1.0\n"
"@r2 color 1\n"
"@r2 line 0, 0, 0, 0\n"
"@r3 type above\n"
"@r3 linestyle 1\n"
"@r3 linewidth 1.0\n"
"@r3 color 1\n"
"@r3 line 0, 0, 0, 0\n"
"@r4 type above\n"
"@r4 linestyle 1\n"
"@r4 linewidth 1.0\n"
"@r4 color 1\n"
"@r4 line 0, 0, 0, 0\n",
		page_width_pixels,
		page_length_pixels,
		'%', '%', '%' );
}

void grace_output_graph_heading( FILE *output_pipe,
				 GRACE_GRAPH *graph,
				 int graph_number,
				 char *title,
				 char *sub_title,
				 char *xaxis_ticklabel_format,
				 double x_label_size,
				 double legend_char_size,
				 char *units,
				 double y_label_size,
				 char *yaxis_label_place_string,
				 char *yaxis_ticklabel_place_string,
				 char *xaxis_ticklabel_on_off,
				 char *xaxis_tickmarks_on_off,
				 double legend_view_x,
				 double legend_view_y,
				 char *y_invert_on_off,
				 char *yaxis_tick_place_string,
				 double world_min_x,
				 double world_max_x,
				 double title_char_size,
				 double subtitle_char_size,
				 int xaxis_ticklabel_precision,
				 char *xaxis_label,
				 boolean yaxis_grid_lines,
				 LIST *optional_label_list,
				 boolean xaxis_ticklabel_stagger,
				 double world_min_y,
				 double world_max_y,
				 int xaxis_ticklabel_angle,
				 int yaxis_ticklabel_precision,
				 char *yaxis_ticklabel_on_off,
				 char *yaxis_tickmarks_on_off )
{
	char buffer[ 1024 ];

	if ( !units ) units = "";
	if ( !title ) title = "";
	if ( !sub_title ) sub_title = "";
	if ( !xaxis_label ) xaxis_label = "";

	if ( world_min_y > world_max_y )
	{
		double temp = world_min_y;
		world_min_y = world_max_y;
		world_max_y = temp;
	}

	fprintf(output_pipe,
"@g%d on\n"
"@g%d hidden false\n"
"@g%d type XY\n"
"@g%d stacked false\n"
"@g%d bar hgap 0.000000\n"
"@g%d fixedpoint off\n"
"@g%d fixedpoint type 0\n"
"@g%d fixedpoint xy 0.000000, 0.000000\n"
"@g%d fixedpoint format general general\n"
"@g%d fixedpoint prec 6, 6\n",
		graph_number,
		graph_number,
		graph_number,
		graph_number,
		graph_number,
		graph_number,
		graph_number,
		graph_number,
		graph_number,
		graph_number );

	fprintf(output_pipe,
"@with g%d\n"
"@    world xmin %lf\n"
"@    world xmax %lf\n"
"@    world ymin %lf\n"
"@    world ymax %lf\n"
"@    stack world 0, 0, 0, 0\n"
"@    view xmin %lf\n"
"@    view xmax %lf\n"
"@    view ymin %lf\n"
"@    view ymax %lf\n"
"@    title \"%s\"\n"
"@    title font 6\n"
"@    title size %f\n"
"@    title color 1\n"
"@    subtitle \"%s\"\n"
"@    subtitle font 6\n"
"@    subtitle size %f\n"
"@    subtitle color 1\n"
"@    xaxes scale Normal\n"
"@    yaxes scale Normal\n"
"@    xaxes invert off\n"
"@    yaxes invert %s\n"
"@    xaxis  on\n"
"@    xaxis  type zero false\n"
"@    xaxis  offset 0.000000 , 0.000000\n"
"@    xaxis  bar off\n"
"@    xaxis  bar color 1\n"
"@    xaxis  bar linestyle 1\n"
"@    xaxis  bar linewidth 0.8\n"
"@    xaxis  label \"%s\"\n"
"@    xaxis  label layout para\n"
"@    xaxis  label place auto\n"
"@    xaxis  label char size 0.650000\n"
"@    xaxis  label font 6\n"
"@    xaxis  label color 1\n"
"@    xaxis  label place normal\n"
"@    xaxis  tick %s\n"
"@    xaxis  tick major %lf\n"
"@    xaxis  tick minor ticks %d\n"
"@    xaxis  tick default 6\n"
"@    xaxis  tick place rounded false\n"
"@    xaxis  tick in\n"
"@    xaxis  tick major size 1\n"
"@    xaxis  tick major color 1\n"
"@    xaxis  tick major linewidth 1.0\n"
"@    xaxis  tick major linestyle 1\n"
"@    xaxis  tick major grid off\n"
"@    xaxis  tick minor color 1\n"
"@    xaxis  tick minor linewidth 1.0\n"
"@    xaxis  tick minor linestyle 1\n"
"@    xaxis  tick minor grid off\n"
"@    xaxis  tick minor size 0.500000\n"
"@    xaxis  ticklabel %s\n"
"@    xaxis  ticklabel prec %d\n"
"@    xaxis  ticklabel format %s\n"
"@    xaxis  ticklabel append \"\"\n"
"@    xaxis  ticklabel prepend \"\"\n"
"@    xaxis  ticklabel angle %d\n"
"@    xaxis  ticklabel skip 0\n"
"@    xaxis  ticklabel stagger %d\n"
"@    xaxis  ticklabel place normal\n"
"@    xaxis  ticklabel offset auto\n"
"@    xaxis  ticklabel offset 0.000000 , 0.010000\n"
"@    xaxis  ticklabel sign normal\n"
"@    xaxis  ticklabel start type auto\n"
"@    xaxis  ticklabel start 0.000000\n"
"@    xaxis  ticklabel stop type auto\n"
"@    xaxis  ticklabel stop 0.000000\n"
"@    xaxis  ticklabel char size %f\n"
"@    xaxis  ticklabel font 4\n"
"@    xaxis  ticklabel color 1\n"
"@    xaxis  tick place normal\n"
"@    xaxis  ticklabel type auto\n"
"@    xaxis  tick type auto\n",
		 graph_number,
		 world_min_x,
		 world_max_x,
		 world_min_y,
		 world_max_y,
		 graph->view_min_x,
		 graph->view_max_x,
		 graph->view_min_y,
		 graph->view_max_y,
		 title,
		 title_char_size,
		 sub_title,
		 subtitle_char_size,
		 y_invert_on_off,
		 xaxis_label,
		 xaxis_tickmarks_on_off,
		 graph->x_tick_major,
		 graph->x_tick_minor,
		 xaxis_ticklabel_on_off,
		 xaxis_ticklabel_precision,
		 xaxis_ticklabel_format,
		 xaxis_ticklabel_angle,
		 xaxis_ticklabel_stagger,
		 x_label_size );

	if ( optional_label_list
	&&   list_rewind( optional_label_list ) )
	{
		grace_output_optional_label_list(
			output_pipe,
			optional_label_list );
	}

/*
"@    yaxis  label place auto\n"
*/

	fprintf(output_pipe,
"@    yaxis  on\n"
"@    yaxis  type zero false\n"
"@    yaxis  offset 0.000000 , 0.000000\n"
"@    yaxis  bar on\n"
"@    yaxis  bar color 1\n"
"@    yaxis  bar linestyle 1\n"
"@    yaxis  bar linewidth 0.8\n"
"@    yaxis  label \"%s\"\n"
"@    yaxis  label layout para\n"
"@    yaxis  label char size 0.700000\n"
"@    yaxis  label font 6\n"
"@    yaxis  label color 1\n"
"@    yaxis  label place %s\n"
"@    yaxis  tick %s\n"
"@    yaxis  tick major %lf\n"
"@    yaxis  tick minor ticks %d\n"
"@    yaxis  tick default 6\n"
"@    yaxis  tick place rounded true\n"
"@    yaxis  tick in\n"
"@    yaxis  tick major size 1.000000\n"
"@    yaxis  tick major color 15\n"
"@    yaxis  tick major linewidth 1.0\n"
"@    yaxis  tick major linestyle %d\n"
"@    yaxis  tick major grid %s\n"
"@    yaxis  tick minor color 15\n"
"@    yaxis  tick minor linewidth 1.0\n"
"@    yaxis  tick minor linestyle %d\n"
"@    yaxis  tick minor grid %s\n"
"@    yaxis  tick minor size 0.500000\n"
"@    yaxis  ticklabel %s\n"
"@    yaxis  ticklabel prec %d\n"
"@    yaxis  ticklabel format decimal\n"
"@    yaxis  ticklabel append \"\"\n"
"@    yaxis  ticklabel prepend \"\"\n"
"@    yaxis  ticklabel angle 0\n"
"@    yaxis  ticklabel skip 0\n"
"@    yaxis  ticklabel stagger 0\n"
"@    yaxis  ticklabel place %s\n"
"@    yaxis  ticklabel offset auto\n"
"@    yaxis  ticklabel offset 0.000000 , 0.010000\n"
"@    yaxis  ticklabel sign normal\n"
"@    yaxis  ticklabel start type auto\n"
"@    yaxis  ticklabel start 0.000000\n"
"@    yaxis  ticklabel stop type auto\n"
"@    yaxis  ticklabel stop 0.000000\n"
"@    yaxis  ticklabel char size %f\n"
"@    yaxis  ticklabel font 4\n"
"@    yaxis  ticklabel color 1\n"
"@    yaxis  tick place %s\n"
"@    yaxis  ticklabel type auto\n"
"@    yaxis  tick type auto\n"
"@    altxaxis  off\n"
"@    altyaxis  off\n"
"@    legend on\n"
"@    legend loctype view\n"
"@    legend x1 %lf\n"
"@    legend y1 %lf\n"
"@    legend box color 0\n"
"@    legend box pattern 0\n"
"@    legend box linewidth 0.0\n"
"@    legend box linestyle 0\n"
"@    legend box fill color 0\n"
"@    legend box fill pattern 0\n"
"@    legend font 4\n"
"@    legend char size %lf\n"
"@    legend color 1\n"
"@    legend length 1\n"
"@    legend vgap 1\n"
"@    legend hgap 1\n"
"@    legend invert false\n"
"@    frame type 0\n"
"@    frame linestyle 1\n"
"@    frame linewidth 1.0\n"
"@    frame color 1\n"
"@    frame pattern 1\n"
"@    frame background color 0\n"
"@    frame background pattern 0\n",
		 format_initial_capital( buffer, units ),
		 yaxis_label_place_string,
		 (yaxis_tickmarks_on_off) ? yaxis_tickmarks_on_off : "on",
		 graph->y_tick_major,
		 graph->y_tick_minor,
		 (yaxis_grid_lines) ? GRACE_YAXIS_MAJOR_LINESTYLE : 0,
		 (yaxis_grid_lines) ? "on" : "off",
		 (yaxis_grid_lines) ? GRACE_YAXIS_MINOR_LINESTYLE : 0,
		 (yaxis_grid_lines) ? "on" : "off",
		 (yaxis_ticklabel_on_off) ? yaxis_ticklabel_on_off : "on",
		 yaxis_ticklabel_precision,
		 yaxis_ticklabel_place_string,
		 y_label_size,
		 yaxis_tick_place_string,
		 legend_view_x,
		 legend_view_y,
		 legend_char_size );

	fflush( output_pipe );

}

void grace_output_full_dataset_heading(	FILE *output_pipe,
				   	int line_color,
				   	int dataset_number,
					char *legend,
					int line_linestyle,
					char *annotated_value_on_off,
					boolean symbols,
					char *datatype_type_bar_xy_xyhilo,
					double symbol_size )
{
	int line_type;

	symbol_size =
		grace_get_symbol_size(
			symbol_size, symbols, datatype_type_bar_xy_xyhilo );

	if ( !annotated_value_on_off
	||   !*annotated_value_on_off )
	{
		annotated_value_on_off = "off";
	}

	if ( !datatype_type_bar_xy_xyhilo )
		datatype_type_bar_xy_xyhilo = "xy";

	if ( strcmp( datatype_type_bar_xy_xyhilo, "bar" ) == 0 )
		line_type = 0;
	else
		line_type = 1;

	fprintf(output_pipe,
"@    s%d symbol %d\n"
"@    s%d symbol size %f\n"
"@    s%d symbol color %d\n"
"@    s%d symbol pattern 1\n"
"@    s%d symbol fill color %d\n"
"@    s%d symbol fill pattern 1\n"
"@    s%d symbol linewidth 1.0\n"
"@    s%d symbol linestyle 1\n"
"@    s%d symbol char 65\n"
"@    s%d symbol char font 0\n"
"@    s%d symbol skip 0\n"
"@    s%d line type %d\n"
"@    s%d line linestyle %d\n"
"@    s%d line linewidth 1.0\n"
"@    s%d line color %d\n"
"@    s%d line pattern %d\n"
"@    s%d baseline type 0\n"
"@    s%d baseline off\n"
"@    s%d dropline off\n"
"@    s%d fill type 0\n"
"@    s%d fill rule 0\n"
"@    s%d fill color 1\n"
"@    s%d fill pattern 1\n"
"@    s%d avalue %s\n"
"@    s%d avalue type 2\n"
"@    s%d avalue char size 1.000000\n"
"@    s%d avalue font 0\n"
"@    s%d avalue color 1\n"
"@    s%d avalue rot 0\n"
"@    s%d avalue format general\n"
"@    s%d avalue prec 3\n"
"@    s%d avalue prepend \"\"\n"
"@    s%d avalue append \"\"\n"
"@    s%d avalue offset 0.000000 , 0.000000\n"
"@    s%d errorbar on\n"
"@    s%d errorbar place normal\n"
"@    s%d errorbar color 1\n"
"@    s%d errorbar pattern 1\n"
"@    s%d errorbar size 1.000000\n"
"@    s%d errorbar linewidth 1.0\n"
"@    s%d errorbar linestyle 1\n"
"@    s%d errorbar riser linewidth 1.0\n"
"@    s%d errorbar riser linestyle 1\n"
"@    s%d errorbar riser clip off\n"
"@    s%d errorbar riser clip length 0.100000\n"
"@    s%d comment \"%s\"\n"
"@    s%d legend  \"%s\"\n",
	dataset_number,
	symbols,
	dataset_number,
	symbol_size,
	dataset_number,
	GRACE_LINE_COLOR_EXPRESSION,
	dataset_number,
	dataset_number,
	GRACE_LINE_COLOR_EXPRESSION,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	line_type,
	dataset_number,
	line_linestyle,
	dataset_number,
	dataset_number,
	GRACE_LINE_COLOR_EXPRESSION,
	dataset_number,
	1 /* line_pattern */,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	annotated_value_on_off,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	dataset_number,
	legend,
	dataset_number,
	legend );

}

double grace_get_symbol_size(	double symbol_size,
				boolean symbols,
				char *datatype_type_bar_xy_xyhilo )
{

	if ( !symbol_size )
	{
		symbol_size = GRACE_BAR_SYMBOL_SIZE;
		if ( symbols
		&&   datatype_type_bar_xy_xyhilo
		&&   strcmp( datatype_type_bar_xy_xyhilo, "xy" ) == 0 )
		{
			symbol_size += 0.3;
		}
	}
	return symbol_size;

}

void grace_output_abbreviated_dataset_heading(
					FILE *output_pipe,
				   	int dataset_number,
				   	int line_color,
				   	int line_linestyle,
					boolean symbols,
					double symbol_size,
					char *datatype_type_bar_xy_xyhilo,
					char *legend )
{
	symbol_size =
		grace_get_symbol_size(
			symbol_size, symbols, datatype_type_bar_xy_xyhilo );

	fprintf(output_pipe,
		"@s%d line color %d\n"
		"@s%d line linestyle %d\n"
		"@s%d symbol %d\n"
		"@s%d symbol size %f\n"
		"@s%d symbol color %d\n"
		"@s%d symbol fill pattern 1\n"
		"@s%d symbol fill color %d\n"
		"@s%d legend \"%s\"\n",
		dataset_number,
		GRACE_LINE_COLOR_EXPRESSION,
		dataset_number,
		line_linestyle,
		dataset_number,
		symbols,
		dataset_number,
		symbol_size,
		dataset_number,
		GRACE_LINE_COLOR_EXPRESSION,
		dataset_number,
		dataset_number,
		GRACE_LINE_COLOR_EXPRESSION,
		dataset_number,
		legend );

}

GRACE *grace_new_datatype_overlay_grace(char *application_name,
					char *role_name )
{
	GRACE *grace;

	grace = grace_new_grace(	application_name,
					role_name );

	grace->x_label_size = DEFAULT_X_LABEL_SIZE;
	grace->grace_graph_type = datatype_overlay_graph;
	grace->graph_list = list_new();
	grace->anchor_graph_list = list_new_list();
	return grace;
}

GRACE *grace_new_quantum_grace(
				char *application_name,
				char *role_name,
				char *title,
				char *sub_title )
{
	GRACE *grace;
	GRACE_GRAPH *grace_graph;
	GRACE_DATATYPE *grace_datatype;

	grace = grace_new_grace(	application_name,
					role_name );

	grace->title = title;
	grace->sub_title = sub_title;
	grace->xaxis_ticklabel_format = "general";
	grace->grace_graph_type = quantum_graph;

	grace->graph_list = list_new();
	grace_graph = grace_graph_new();
	grace_graph->datatype_list = list_new_list();
	grace_graph->xaxis_ticklabel_stagger = 1;
	grace_graph_set_scale_to_zero( grace_graph, 1 );
	grace_graph->xaxis_tickmarks_on_off = "off";
	list_set( grace->graph_list, grace_graph );

	grace_datatype = grace_new_grace_datatype( 
				"" /* entity_name */,
				"" /* datatype_name */ );
	grace_datatype->datatype_type_bar_xy_xyhilo = "bar";
	grace_datatype->line_linestyle = 0;
	grace_datatype->annotated_value_on_off = "on";
	list_set( grace_graph->datatype_list, grace_datatype );

	return grace;
}

GRACE *grace_new_date_time_grace(
				char *application_name,
				char *role_name,
				char *title,
				char *sub_title )
{
	GRACE *grace;

	grace = grace_new_grace(	application_name,
					role_name );

	grace->title = title;
	grace->sub_title = sub_title;
	grace->graph_list = list_new();

	grace->x_label_size = DEFAULT_X_LABEL_SIZE;
	grace->grace_graph_type = date_time_graph;
	return grace;
}

void grace_date_time_set_attribute_name(
					LIST *graph_list,
					char *float_integer_attribute_name )
{
	GRACE_GRAPH *grace_graph;
	GRACE_DATATYPE *grace_datatype;
	char buffer[ 256 ];

	grace_graph = grace_new_grace_graph();

	/* grace_graph_set_scale_to_zero( grace_graph, 1 ); */

	grace_graph->datatype_list = list_new();
	grace_datatype = grace_new_grace_datatype( 
				"" /* datatype_entity */,
				float_integer_attribute_name );

	grace_datatype->datatype_type_bar_xy_xyhilo = "xy";

	grace_datatype->legend = 
		strdup( format_initial_capital(
				buffer,
				float_integer_attribute_name ) );

	list_set(	grace_graph->datatype_list,
			grace_datatype );

	list_set(	graph_list,
			grace_graph );

}

void grace_date_time_set_data(	LIST *graph_list,
				char *date_data,
				char *time_data,
				char *value_data,
				boolean dataset_no_cycle_color )
{
	int year, month, day;
	int hour = 0;
	int minute = 0;
	GRACE_GRAPH *grace_graph;
	GRACE_DATATYPE *grace_datatype;
	GRACE_DATASET *grace_dataset;
	GRACE_POINT *grace_point;

	if ( !grace_parse_date_yyyy_mm_dd(	
					&year,
					&month,
					&day,
					date_data ) )
	{
		return;
	}

	if ( time_data && *time_data )
		julian_get_hour_minute( &hour, &minute, time_data );

	grace_graph = list_get( graph_list );

	grace_datatype = list_get( grace_graph->datatype_list );


	grace_point = grace_new_grace_point(
		 	greg2jul(month		/* mon  */,
			    	 day		/* day  */, 
			   	 year		/* year */, 
			   	 hour		/* hour */,
			 	 minute		/* min  */,
			    	 0.0		/* sec  */ ),
			value_data,
			(char *)0 /* optional_label */ );

	if ( !list_length( grace_datatype->dataset_list ) )
	{
		grace_dataset =
			grace_new_grace_dataset(
				grace_datatype->datatype_number,
				dataset_no_cycle_color );

		list_set(	grace_datatype->dataset_list,
				grace_dataset );
	}
	else
	{
		grace_dataset =
			list_last(
				grace_datatype->dataset_list );
	}

	list_set( grace_dataset->point_list, grace_point );

}

GRACE *grace_new_unit_graph_grace(
			char *application_name,
			char *role_name,
			char *infrastructure_process,
			char *data_process,
			char *argv_0,
			int datatype_entity_piece,
			int datatype_piece,
			int date_piece,
			int time_piece,
			int value_piece,
			char *title,
			char *sub_title,
			boolean datatype_type_xyhilo,
			enum grace_cycle_colors cycle_colors )
{
	GRACE *grace;

	grace = grace_new_grace(	application_name,
					role_name );

	grace->title = title;
	grace->sub_title = sub_title;
	grace->graph_list = list_new_list();
	grace->datatype_type_xyhilo = datatype_type_xyhilo;

	if ( infrastructure_process )
	{
		if ( !grace_populate_unit_graph_list(
						grace->graph_list,
						argv_0,
						infrastructure_process,
						grace->datatype_type_xyhilo ) )
		{
			char msg[ 1024 ];
			sprintf( msg,
				 "ERROR in %s: cannot populate graph list",
				 argv_0 );
			appaserver_output_error_message(
				application_name, msg, (char *)0 );
			exit( 1 );
		}
	}

	if ( cycle_colors == do_cycle_colors )
	{
		grace->dataset_no_cycle_color = 0;
	}
	else
	if ( cycle_colors == no_cycle_colors )
	{
		grace->dataset_no_cycle_color = 1;
	}
	else
	if ( cycle_colors == no_cycle_colors_if_multiple_datatypes
	&&   grace_contains_multiple_datatypes( grace->graph_list ) )
	{
		grace->dataset_no_cycle_color = 1;
	}

	if ( data_process )
	{
		grace_populate_point_list(
					grace->graph_list,
					data_process,
					datatype_entity_piece,
					datatype_piece,
					date_piece,
					time_piece,
					value_piece,
					unit_graph,
					grace->datatype_type_xyhilo,
					grace->dataset_no_cycle_color );
	}
	grace->x_label_size = DEFAULT_X_LABEL_SIZE;
	grace->grace_graph_type = unit_graph;
	grace->datatype_entity_piece = datatype_entity_piece;
	grace->datatype_piece = datatype_piece;
	grace->date_piece = date_piece;
	grace->time_piece = time_piece;
	grace->value_piece = value_piece;
	return grace;
}

boolean grace_output_point_list(
				FILE *output_pipe,
				int dataset_number,
				LIST *point_list,
				int graph_number,
				char *datatype_type_bar_xy_xyhilo )
{
	GRACE_POINT *p;
	boolean output_any = 0;

	if ( !datatype_type_bar_xy_xyhilo
	||   !*datatype_type_bar_xy_xyhilo )
	{
		datatype_type_bar_xy_xyhilo = "xy";
	}

	if ( !list_rewind( point_list ) )
	{
		fprintf(output_pipe,
			"@target G%d.S%d\n"
			"@type %s\n",
 			graph_number,
 			dataset_number,
 			datatype_type_bar_xy_xyhilo );

		fprintf( output_pipe, "&\n" );
		return 0;
	}

	fprintf(output_pipe,
		"@target G%d.S%d\n"
		"@type %s\n",
 		graph_number,
 		dataset_number,
 		datatype_type_bar_xy_xyhilo );

	do {
		p = list_get( point_list );

		if ( strcmp( datatype_type_bar_xy_xyhilo, "xyhilo" ) == 0 )
		{
			if ( grace_is_null( p->y_string ) )
			{
				continue;
			}

			fprintf( output_pipe, 
		 		"%lf\t%s\t%s\t%s\t%s\n",
		 		p->x,
				p->high_string,
				p->low_string,
				p->y_string,
				p->y_string );

			output_any = 1;
		}
		else
		if ( grace_is_null( p->y_string ) )
		{
			fprintf(output_pipe, 
				"%lf\t\n",
		 		p->x );
		}
		else
		{
			fprintf(output_pipe, 
				"%lf\t%s\n",
		 		p->x, p->y_string );
			output_any = 1;
		}

	} while( list_next( point_list ) );

	fprintf( output_pipe, "&\n" );

	return output_any;

}

void grace_set_point(		boolean *inside_null,
				LIST *dataset_list,
				double x,
				char *y_string,
				char *optional_label,
				int datatype_number,
				boolean dataset_no_cycle_color )
{
	GRACE_DATASET *grace_dataset;
	GRACE_POINT *point;

	if ( !list_length( dataset_list ) )
	{
		grace_dataset = grace_new_grace_dataset(
					datatype_number,
					dataset_no_cycle_color );

		list_set(	dataset_list,
				grace_dataset );
	}
	else
	{
		grace_dataset = list_last( dataset_list );
	}

	if ( grace_is_null( y_string ) )
	{
		if ( 	grace_dataset->dataset_number <=
			GRACE_MAX_BREAKS_NEW_DATASET )
		{
			if ( !*inside_null )
			{
				grace_dataset = grace_new_grace_dataset(
						datatype_number,
						dataset_no_cycle_color );

				*inside_null = 1;

				list_set(	dataset_list,
						grace_dataset );
				return;
			}
			else
			{
				return;
			}
		}
		else
		{
			*inside_null = 1;
			return;
		}
	}

	*inside_null = 0;
	point = grace_new_grace_point(x, y_string, optional_label );
	list_set( grace_dataset->point_list, point );

}

void grace_get_world_minimum_x(
				double *world_min_x,
				JULIAN *begin_date_julian,
				int number_of_days,
				int decrement_x_axis_for_bar )
{
	JULIAN *x_axis_begin_date_julian = julian_new_julian( 0 );

	if ( *world_min_x ) return;

	if ( !begin_date_julian )
	{
		*world_min_x = 0;
	}
	else
	{
		julian_copy( x_axis_begin_date_julian, begin_date_julian );
	
		if ( decrement_x_axis_for_bar )
		{
			if ( number_of_days <= 2 )
			{
				julian_increment_hours(
					x_axis_begin_date_julian->current, -6 );
			}
			else
			{
				x_axis_begin_date_julian->current =
					julian_decrement_day(
						x_axis_begin_date_julian->
						current );
			}
		}
	
		*world_min_x =
			julian_yyyy_mm_dd_time_hhmm_to_julian(
				julian_display_yyyy_mm_dd(
					x_axis_begin_date_julian->current ),
				julian_display_hhmm(
					x_axis_begin_date_julian->current ) );
	}
}

void grace_get_world_minimum_y(
				double *world_min_y,
				double y )
{
	/* if ( !*world_min_y ) *world_min_y = y; */
	*world_min_y = y;
}

void grace_get_world_maximum_x(
				double *world_max_x,
				JULIAN *end_date_julian )
{
	JULIAN *x_axis_end_date_julian = julian_new_julian( 0 );

	if ( !end_date_julian )
	{
		*world_max_x = 0;
	}
	else
	{
		julian_copy( x_axis_end_date_julian, end_date_julian );
		x_axis_end_date_julian->current =
			julian_increment_day(
				x_axis_end_date_julian->current );
		*world_max_x = julian_yyyy_mm_dd_to_julian(
				julian_display_yyyy_mm_dd(
					x_axis_end_date_julian->current ) );
	}
}

void grace_get_world_maximum_y(
				double *world_max_y,
				JULIAN *end_date_julian,
				double y )
{
	if ( !end_date_julian )
	{
		*world_max_y = y;
	}
	else
	{
		*world_max_y = (y) ? y : 0.5;
	}

}

void grace_set_x_tick_major( GRACE_GRAPH *graph, double x_tick_major )
{
	graph->x_tick_major = x_tick_major;
}

void grace_set_title( GRACE *grace, char *title )
{
	grace->title = title;
}

GRACE_POINT *grace_new_grace_point(	double x,
					char *y_string,
					char *optional_label )
{
	return grace_point_new(	x,
				y_string,
				(char *)0 /* low_string */,
				(char *)0 /* high_string */,
				optional_label );
}

GRACE_POINT *grace_point_new(	double x,
				char *y_string,
				char *low_string,
				char *high_string,
				char *optional_label )
{
	GRACE_POINT *g = (GRACE_POINT *)calloc( 1, sizeof( GRACE_POINT ) );

	if ( !g )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: calloc failed\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	g->x = x;
	g->y_string = y_string;
	g->low_string = low_string;
	g->high_string = high_string;
	g->optional_label = optional_label;

	return g;
}

boolean grace_get_is_null( char *value_string )
{
	if ( !value_string )
	{
		return 1;
	}
	else
	if ( !*value_string )
		return 1;
	else
	if ( strcmp( value_string, "null" ) == 0
	||   strcmp( value_string, "NULL" ) == 0 )
	{
		return 1;
	}
	else
		return 0;
}

boolean grace_output_to_file(
				char *output_filename,
				char *postscript_filename,
				char *agr_filename,
				char *title,
				char *sub_title,
				char *xaxis_ticklabel_format,
				enum grace_graph_type grace_graph_type,
				double x_label_size,
				int page_width_pixels,
				int page_length_pixels,
				char *grace_home_directory,
				char *grace_execution_directory,
				boolean grace_free_option,
				char *grace_output,
				LIST *optional_label_list,
				boolean symbols,
				LIST *graph_list,
				LIST *anchor_graph_list,
				int xaxis_ticklabel_precision,
				double world_min_x,
				double world_max_x )
{
	FILE *output_pipe;

	if ( !title ) title = "";
	if ( !sub_title ) sub_title = "";

	output_pipe = 
		grace_open_output_pipe(
					  output_filename,
					  postscript_filename,
					  agr_filename,
					  grace_home_directory,
					  grace_execution_directory,
					  grace_free_option,
					  grace_output );

	grace_output_top_heading(	output_pipe,
					page_width_pixels,
					page_length_pixels );

	grace_output_data(	output_pipe,
				graph_list,
				title,
				sub_title,
				xaxis_ticklabel_format,
				x_label_size,
				grace_graph_type,
				xaxis_ticklabel_precision,
				optional_label_list,
				symbols,
				world_min_x,
				world_max_x,
				0 /* not is_datatype_overlay_anchor_graph */ );

	if ( grace_graph_type == datatype_overlay_graph )
	{
		grace_output_data(
				output_pipe,
				anchor_graph_list,
				title,
				sub_title,
				xaxis_ticklabel_format,
				x_label_size,
				grace_graph_type,
				xaxis_ticklabel_precision,
				optional_label_list,
				symbols,
				world_min_x,
				world_max_x,
				1 /* is_datatype_overlay_anchor_graph */ );
	}

	pclose( output_pipe );
	return 1;

}

boolean grace_convert_postscript_to_pdf(
					char *pdf_filename, 
					char *postscript_filename,
					char *distill_directory,
					char *distill_landscape_flag,
					char *ghost_script_directory )
{
	char sys_string[ 4096 ];
	int results;

	sprintf(	sys_string, 
			"ps2pdf.sh %s %s \"%s\" \"%s\" \"%s\"", 
			postscript_filename,
			pdf_filename,
			distill_directory,
			distill_landscape_flag,
			ghost_script_directory );

	if ( !timlib_file_exists( postscript_filename ) ) return 0;

	results = system( sys_string );

	if ( results != 0 )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: ps2pdf.sh returned an error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		return 0;
	}
	else
	{
		return 1;
	}

}

FILE *grace_open_output_pipe(
				char *output_filename,
				char *postscript_filename,
				char *agr_filename,
				char *grace_home_directory,
				char *grace_execution_directory,
				boolean grace_free_option,
				char *grace_output )
{
	char sys_string[ 4096 ];
	FILE *p;
	char *free_option = "";
	char *hdevice_option;
	char grace_executable[ 1024 ];
	char *local_output_filename;

	if ( strcmp( grace_output, "postscript_pdf" ) != 0 )
		local_output_filename = output_filename;
	else
		local_output_filename = postscript_filename;

	if ( !local_output_filename )
	{
		fprintf( stderr,
"Error in %s/%s()/%d: with grace_output = [%s], got output_filename = [%s] and postscript_filename = [%s].\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 grace_output,
			 output_filename,
			 postscript_filename );
		exit( 1 );
	}

	if ( grace_free_option )
		free_option = "-free";

	if ( !grace_output || !*grace_output )
	{
		fprintf(	stderr,
				"ERROR in %s/%s(): empty grace_output.\n",
				__FILE__,
				__FUNCTION__ );
		exit( 1 );
	}

	if ( strcmp( grace_output, "direct_pdf" ) == 0 )
		hdevice_option = "-hdevice PDF";
	else
	if ( strcmp( grace_output, "jpeg" ) == 0 )
		hdevice_option = "-hdevice JPEG";
	else
	if ( strcmp( grace_output, "postscript_pdf" ) == 0 )
/*
		hdevice_option = "-hdevice EPS";
*/
		hdevice_option = "-hdevice PostScript";
	else
	{
		fprintf(	stderr,
				"ERROR in %s/%s(): invalid grace_output = %s\n",
				__FILE__,
				__FUNCTION__,
				grace_output );
		exit( 1 );
	}

	sprintf(grace_executable,
		GRACE_EXECUTABLE,
		grace_home_directory,
		grace_execution_directory );

/*
		 "-fixed 11x8.5 				"
		 "-hdevice EPS 					"
		 "-hdevice JPEG					"
*/

	sprintf( sys_string,
		 "tee %s				       |"
		 "%s						"
		 "-noask					"
		 "-autoscale x 					"
		 "%s						"
		 "%s	 					"
		 "-pipe						"
		 "-hardcopy					"
		 "-printfile %s) 				",
		agr_filename,
		grace_executable, 
		free_option,
		hdevice_option,
		local_output_filename );

	p = popen( sys_string, "w" );

	return p;

}

void grace_get_filenames(		char **agr_filename,
					char **ftp_agr_filename,
					char **postscript_filename,
					char **output_filename,
					char **pdf_output_filename,
					char *application_name,
					char *document_root_directory,
					char *graph_identifier,
					char *grace_output )
{
	char extension[ 16 ];
	APPASERVER_LINK *appaserver_link;

	if ( !grace_output || !*grace_output )
	{
		char msg[ 1024 ];

		sprintf(msg,
			"ERROR in %s/%s(): empty grace_output",
			__FILE__,
			__FUNCTION__ );
		appaserver_output_error_message(
			application_name, msg, (char *)0 );
		exit( 1 );
	}

	appaserver_link =
		appaserver_link_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		document_root_directory,
			GRACE_FILENAME_STEM,
			application_name,
			0 /* process_id */,
			graph_identifier /* session */,
			(char *)0 /* begin_date_string */,
			(char *)0 /* end_date_string */,
			(char *)0 /* extension */ );

	appaserver_link->extension = "agr";

	*agr_filename =
		appaserver_link_output_filename(
			appaserver_link->document_root_directory,
			appaserver_link_output_tail_half(
				appaserver_link->application_name,
				appaserver_link->filename_stem,
				appaserver_link->begin_date_string,
				appaserver_link->end_date_string,
				appaserver_link->process_id,
				appaserver_link->session_key,
				appaserver_link->extension ) );

	*ftp_agr_filename =
		appaserver_link_prompt_filename(
			appaserver_link_prompt_link_half(
				appaserver_link->prepend_http,
				appaserver_link->http_prefix,
				appaserver_link->server_address ),
			appaserver_link->application_name,
			appaserver_link->filename_stem,
			appaserver_link->begin_date_string,
			appaserver_link->end_date_string,
			appaserver_link->process_id,
			appaserver_link->session_key,
			appaserver_link->extension );

	if ( strcmp( grace_output, "postscript_pdf" ) == 0
	||   strcmp( grace_output, "direct_pdf" ) == 0 )
	{
		strcpy( extension, "pdf" );
	}
	else
	if ( strcmp( grace_output, "jpeg" ) == 0
	||   strcmp( grace_output, "jpg" ) == 0 )
	{
		strcpy( extension, "jpg" );
	}
	else
	{
		char msg[ 1024 ];
		sprintf( msg,
			 "ERROR in %s/%s(): invalid grace_output = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 grace_output );
		appaserver_output_error_message(
			application_name, msg, (char *)0 );
		exit( 1 );
	}

	appaserver_link->extension = extension;

	*output_filename =
		appaserver_link_output_filename(
			appaserver_link->document_root_directory,
			appaserver_link_output_tail_half(
				appaserver_link->application_name,
				appaserver_link->filename_stem,
				appaserver_link->begin_date_string,
				appaserver_link->end_date_string,
				appaserver_link->process_id,
				appaserver_link->session_key,
				appaserver_link->extension ) );

	*pdf_output_filename =
		appaserver_link_prompt_filename(
			appaserver_link_prompt_link_half(
				appaserver_link->prepend_http,
				appaserver_link->http_prefix,
				appaserver_link->server_address ),
			appaserver_link->application_name,
			appaserver_link->filename_stem,
			appaserver_link->begin_date_string,
			appaserver_link->end_date_string,
			appaserver_link->process_id,
			appaserver_link->session_key,
			appaserver_link->extension );

	appaserver_link->extension = "ps";

	*postscript_filename =
		appaserver_link_output_filename(
			appaserver_link->document_root_directory,
			appaserver_link_output_tail_half(
				appaserver_link->application_name,
				appaserver_link->filename_stem,
				appaserver_link->begin_date_string,
				appaserver_link->end_date_string,
				appaserver_link->process_id,
				appaserver_link->session_key,
				appaserver_link->extension ) );
}

int grace_set_structures(	int *page_width_pixels,
				int *page_length_pixels,
				char **distill_landscape_flag,
				boolean *landscape_mode,
				GRACE *grace,
				LIST *graph_list,
				LIST *anchor_graph_list,
				JULIAN *begin_date_julian,
				JULIAN *end_date_julian,
				int number_of_days,
				enum grace_graph_type grace_graph_type,
				boolean force_landscape_mode )
{
	double starting_single_view_min_y = STARTING_SINGLE_VIEW_MIN_Y;
	double starting_view_min_y = STARTING_VIEW_MIN_Y;
	double view_min_y;
	double view_x_range = PORTRAIT_VIEW_X_RANGE;
	double view_y_offset, view_y_increment;
	GRACE_GRAPH *grace_graph;

	*landscape_mode = 1;
	*page_width_pixels = PORTRAIT_PAGE_WIDTH_PIXELS;
	*page_length_pixels = PORTRAIT_PAGE_LENGTH_PIXELS;
	*distill_landscape_flag = "";

	if ( grace_no_legend_set( graph_list ) )
		view_x_range += 0.10;

	if ( grace_graph_type == datatype_overlay_graph )
	{
		grace_overlay_graph_remove_same_datatype(
						graph_list,
						anchor_graph_list );
	}

	if ( !grace_remove_empty_graphs( graph_list,
					 anchor_graph_list,
					 grace_graph_type )
	||    !list_length( graph_list ) )
	{
		return 0;
	}

	if ( !force_landscape_mode
	&&   list_length( graph_list ) >= GRAPHS_UNTIL_PORTRAIT )
	{
		*landscape_mode = 0;
	}

	if ( *landscape_mode )
	{
		grace_set_landscape(	page_width_pixels,
					page_length_pixels,
					distill_landscape_flag,
					&starting_single_view_min_y,
					&view_x_range );

		grace_get_landscape_view_y_minimum_offset_and_increment(
					&view_min_y,
					&view_y_offset,
					&view_y_increment,
					starting_single_view_min_y,
					starting_view_min_y,
					list_length( graph_list ) );
	}
	else
	{
		grace_get_portrait_view_y_minimum_offset_and_increment(
					&view_min_y,
					&view_y_offset,
					&view_y_increment,
					starting_single_view_min_y,
					starting_view_min_y,
					list_length( graph_list ) );
	}

	grace_set_aggregate_variables(
				grace,
				begin_date_julian,
				end_date_julian,
				graph_list,
				number_of_days,
				grace_graph_type,
				view_min_y,
				view_y_offset,
				view_y_increment,
				view_x_range,
				*landscape_mode,
				0 /* not is_datatype_overlay_anchor */ );

	if ( grace_graph_type == datatype_overlay_graph )
	{
		grace_set_aggregate_variables(
				grace,
				begin_date_julian,
				end_date_julian,
				anchor_graph_list,
				number_of_days,
				grace_graph_type,
				view_min_y,
				view_y_offset,
				view_y_increment,
				view_x_range,
				*landscape_mode,
				1 /* is_datatype_overlay_anchor */ );

		grace_make_same_units_have_same_y_tick_marks(
						graph_list,
						anchor_graph_list );
	}
	
	grace->x_label_size = grace_get_x_label_size(
					number_of_days,
					list_length( graph_list ) );

	if ( list_rewind( grace->graph_list ) )
	{
		do {
			grace_graph =
				list_get(
					grace->graph_list );

			grace_graph->legend_char_size = 
				grace_get_legend_char_size(
						grace_graph->datatype_list,
						grace_graph_type );

		} while( list_next( grace->graph_list ) );
	}

	if ( list_rewind( grace->anchor_graph_list ) )
	{
		do {
			grace_graph =
				list_get(
					grace->anchor_graph_list );

			grace_graph->legend_char_size = 
				grace_get_legend_char_size(
						grace_graph->datatype_list,
						grace_graph_type );

		} while( list_next( grace->anchor_graph_list ) );
	}

	return 1;

}

void grace_overlay_graph_remove_same_datatype(
					LIST *graph_list,
					LIST *anchor_graph_list )
{
	GRACE_GRAPH *graph;
	GRACE_GRAPH *anchor_graph = {0};
	GRACE_DATATYPE *graph_datatype;
	GRACE_DATATYPE *anchor_graph_datatype;

	if ( !graph_list ) return;
	if ( !list_length( graph_list ) ) return;

	if ( list_length( graph_list ) != 
	     list_length( anchor_graph_list ) )
	{
		return;
	}

	list_rewind( graph_list );
	list_rewind( anchor_graph_list );

	do {
		graph = list_get( graph_list );
		anchor_graph = list_get( anchor_graph_list );

		graph_datatype = list_first( graph->datatype_list );

		anchor_graph_datatype =
			list_first(
				anchor_graph->datatype_list );

		if ( strcmp(	graph_datatype->datatype_entity,
				anchor_graph_datatype->datatype_entity ) == 0
		&&   strcmp(	graph_datatype->datatype_name,
				anchor_graph_datatype->datatype_name ) == 0 )
		{
			list_delete( graph_list );
			list_rewind( graph_list );
			list_delete( anchor_graph_list );
			list_rewind( anchor_graph_list );
			continue;
		}
		list_next( anchor_graph_list );
	} while( list_next( graph_list ) );
	return;
}

int grace_remove_empty_graphs(	LIST *graph_list,
				LIST *anchor_graph_list,
				enum grace_graph_type grace_graph_type )
{
	GRACE_GRAPH *graph;
	GRACE_GRAPH *anchor_graph = {0};

	if ( !graph_list
	||   !list_length( graph_list ) )
	{
		return 0;
	}

	list_rewind( graph_list );

	if ( grace_graph_type == datatype_overlay_graph )
		list_rewind( anchor_graph_list );

	do {
		graph = list_get( graph_list );

		if ( grace_graph_type == datatype_overlay_graph )
		{
			anchor_graph =
				list_get(
					anchor_graph_list );
		}

		if ( grace_graph_type == datatype_overlay_graph )
		{
			if ( !grace_graph_datatype_populated(
						graph->datatype_list )
			||   !grace_graph_datatype_populated(
						anchor_graph->datatype_list ) )
			{
				list_delete( graph_list );
				list_delete( anchor_graph_list );
			}
		}
		else
		{
			if ( !grace_graph_datatype_populated(
						graph->datatype_list ) )
			{
				list_delete( graph_list );
			}
		}

		if ( grace_graph_type == datatype_overlay_graph )
			list_next( anchor_graph_list );

	} while( list_next( graph_list ) );
	return 1;
}

int grace_graph_datatype_populated( LIST *datatype_list )
{
	GRACE_DATATYPE *datatype;
	GRACE_DATASET *dataset;

	if ( !list_rewind( datatype_list ) )
	{
		return 0;
	}

	do {
		datatype = list_get( datatype_list );

		if ( !list_rewind( datatype->dataset_list ) )
		{
			continue;
		}

		do {
			dataset = list_get( datatype->dataset_list );

			if ( list_length( dataset->point_list ) )
			{
				return 1;
			}
		} while( list_next( datatype->dataset_list ) );
	} while( list_next( datatype_list ) );
	return 0;
}

double grace_get_maximum_y_double( LIST *datatype_list )
{
	GRACE_DATATYPE *datatype;
	GRACE_DATASET *dataset;
	GRACE_POINT *p;
	double maximum = -9999999999.99;
	double y;

	if ( !list_rewind( datatype_list ) )
	{
		fprintf( stderr,
		"ERROR in %s(): empty datatype list\n", __FUNCTION__ );
		exit( 1 );
	}

	do {
		datatype = list_get( datatype_list );

		if ( !list_rewind( datatype->dataset_list ) )
			continue;

		do {
			dataset = list_get( datatype->dataset_list );

			if ( !list_rewind( dataset->point_list ) )
				continue;
			do {
				p = list_get( dataset->point_list );

				if ( grace_is_null( p->y_string ) )
					continue;

				y = atof( p->y_string );

				if ( y > maximum )
				{
					maximum = y;
				}

			} while( list_next( dataset->point_list ) );
		} while( list_next( datatype->dataset_list ) );
	} while( list_next( datatype_list ) );
	return maximum;
}

double grace_get_minimum_y_double( LIST *datatype_list )
{
	GRACE_POINT *p;
	GRACE_DATATYPE *datatype;
	GRACE_DATASET *dataset;
	double minimum = 9999999999.99;
	double y;

	if ( !list_rewind( datatype_list ) )
	{
		fprintf( stderr,
		"ERROR in %s(): empty datatype list\n", __FUNCTION__ );
		exit( 1 );
	}

	do {
		datatype = list_get( datatype_list );

		if ( !list_rewind( datatype->dataset_list ) ) continue;

		do {
			dataset = list_get( datatype->dataset_list );

			if ( !list_rewind( dataset->point_list ) )
				continue;

			do {
				p = list_get( dataset->point_list );

				if ( grace_is_null( p->y_string ) )
					continue;

				y = atof( p->y_string );

				if ( y < minimum )
				{
					minimum = y;
				}
			} while( list_next( dataset->point_list ) );
		} while( list_next( datatype->dataset_list ) );
	} while( list_next( datatype_list ) );
	return minimum;
}

double grace_get_lowest_first_x_double( LIST *datatype_list )
{
	GRACE_POINT *p;
	GRACE_DATATYPE *datatype;
	GRACE_DATASET *dataset;
	double lowest_first_x_double = 999999999999.99;

	if ( !list_rewind( datatype_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s(): empty datatype list\n", __FUNCTION__ );
		exit( 1 );
	}

	do {
		datatype = list_get( datatype_list );

		if ( !list_rewind( datatype->dataset_list ) ) continue;

		do {
			dataset = list_get( datatype->dataset_list );

			if ( !list_rewind( dataset->point_list ) ) continue;

			do {
				p = list_get( dataset->point_list );

				if ( p->x < lowest_first_x_double )
					lowest_first_x_double = p->x;

			} while( list_next( dataset->point_list ) );

		} while( list_next( datatype->dataset_list ) );

	} while( list_next( datatype_list ) );

	return lowest_first_x_double;
}

double grace_get_highest_last_x_double(	LIST *datatype_list )
{
	GRACE_POINT *p;
	GRACE_DATATYPE *datatype;
	GRACE_DATASET *dataset;
	double highest_last_x_double = -99999999.99;

	if ( !list_rewind( datatype_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s(): empty datatype list\n", __FUNCTION__ );
		exit( 1 );
	}

	do {
		datatype = list_get( datatype_list );

		if ( !list_rewind( datatype->dataset_list ) ) continue;

		do {
			dataset =
				list_get(
					datatype->dataset_list );

			if ( !list_rewind( dataset->point_list ) )
				continue;

			do {
				p = list_get( dataset->point_list );

				if ( p->x > highest_last_x_double )
					highest_last_x_double = p->x;

			} while( list_next( dataset->point_list ) );

		} while( list_next( datatype->dataset_list ) );

	} while( list_next( datatype_list ) );

	return highest_last_x_double;
}

void grace_set_x_tick_minor( GRACE_GRAPH *graph, int x_tick_minor )
{
	graph->x_tick_minor = x_tick_minor;
}

void grace_set_y_tick_major( GRACE_GRAPH *graph, double y_tick_major )
{
	graph->y_tick_major = y_tick_major;
}

void grace_set_y_tick_minor( GRACE_GRAPH *graph, int y_tick_minor )
{
	graph->y_tick_minor = y_tick_minor;
}

GRACE_GRAPH *grace_graph_new( void )
{
	return grace_new_grace_graph();
}

GRACE_GRAPH *grace_new_grace_graph( void )
{
	GRACE_GRAPH *g;

	g = (GRACE_GRAPH *)calloc( 1, sizeof( GRACE_GRAPH ) );

	if ( !g )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc failed\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	g->datatype_list = list_new_list();
	g->y_label_size = DEFAULT_Y_LABEL_SIZE;
	g->yaxis_tick_place_string = DEFAULT_YAXIS_TICK_PLACE;
	g->yaxis_label_place_string = DEFAULT_YAXIS_LABEL_PLACE;
	g->yaxis_ticklabel_place_string = DEFAULT_YAXIS_TICKLABEL_PLACE;
	g->xaxis_ticklabel_on_off = "on";
	g->xaxis_tickmarks_on_off = "on";
	g->y_invert_on_off = DEFAULT_Y_INVERT_ON_OFF;
	g->xaxis_ticklabel_angle =
		GRACE_DEFAULT_XAXIS_TICKLABEL_ANGLE;
	g->yaxis_ticklabel_precision =
		GRACE_DEFAULT_YAXIS_TICKLABEL_PRECISION;
	g->yaxis_grid_lines = 1;
	return g;
}

GRACE_DATATYPE *grace_new_grace_datatype( 
				char *datatype_entity,
				char *datatype_name )
{
	GRACE_DATATYPE *datatype;
	static int datatype_number = 0;

	datatype = (GRACE_DATATYPE *)
			calloc( 1, sizeof( GRACE_DATATYPE ) );

	if ( !datatype )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc failed\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	datatype->datatype_entity = datatype_entity;
	datatype->datatype_name = datatype_name;
	datatype->dataset_list = list_new();
	datatype->datatype_number = datatype_number++;
	datatype->line_linestyle = 1;
	return datatype;
}

void grace_append_graph( LIST *graph_list, GRACE_GRAPH *graph )
{
	list_set( graph_list, graph );
}

void grace_set_view_minimum( GRACE_GRAPH *graph, double x, double y )
{
	graph->view_min_x = x;
	graph->view_min_y = y;
}

void grace_set_view_maximum( GRACE_GRAPH *graph, double x, double y )
{
	graph->view_max_x = x;
	graph->view_max_y = y;
}

void grace_set_view_maximum_x( GRACE_GRAPH *graph, double x )
{
	graph->view_max_x = x;
}

int grace_populate_unit_graph_list(
				LIST *graph_list,
				char *argv_0,
				char *infrastructure_process,
				boolean datatype_type_xyhilo )
{
	FILE *p;
	GRACE_GRAPH *grace_graph;
	GRACE_DATATYPE *grace_datatype;
	char *block[ 4096 ];
	int block_count;
	char unit_string[ 128 ];
	char datatype_entity_string[ 128 ];
	char datatype_name[ 128 ];
	char bar_graph_yn[ 128 ];
	char scale_graph_zero_yn[ 128 ];
	int i;
	char legend[ 1024 ];
	char buffer[ 1024 ];

	p = popen( infrastructure_process, "r" );

/* ------------------------------------------------------------------------ */
/* Sample input: 						            */
/* unit|datatype|station|bar_graph_yn|scale_graph_zero_yn		    */
/* ------------------------------------------------------------------------ */
/* celsius|air_temperature|BA|n|n		<-- block 0	            */
/* celsius|bottom_temperature|BA|n|n				            */
/* celsius|surface_temperature|BA|n|n				            */
/* celsius|temperature_cr10|BA|n|n				            */
/* feet|stage|BA|n|n				<-- block 1	            */
/* inches|rain|BA|y|y				<-- block 2	            */
/* mS/cm|conductivity|BA|n|n			<-- block 3	            */
/* volts|voltage|BA|n|n				<-- block 4	            */
/* ------------------------------------------------------------------------ */
	while( ( block_count = timlib_get_block_delimiter(
					block,
					'|',
					1,
					p,
					4095 ) ) )
	{
		if ( count_characters( '|', block[ 0 ] ) < 4 )
		{
			fprintf(stderr,
			"ERROR in %s: bad input from process = (%s) = (%s)\n",
				argv_0,
				infrastructure_process,
				block[ 0 ] );
			pclose( p );
			return 0;
		}

		/* They all have the same unit */
		/* --------------------------- */
		piece( unit_string, '|', block[ 0 ], 0 );

		grace_graph = grace_new_grace_graph();

		grace_graph->units = strdup( unit_string );

		for( i = 0; i < block_count; i++ )
		{
			piece( datatype_name, '|', block[ i ], 1 );
			piece( datatype_entity_string, '|', block[ i ], 2 );
			piece( bar_graph_yn, '|', block[ i ], 3 );
			piece( scale_graph_zero_yn, '|', block[ i ], 4 );

			grace_datatype =
				grace_new_grace_datatype(
					"" /* datatype_entity */,
					strdup( datatype_name ) );

			grace_graph_set_scale_to_zero(
					grace_graph,
					(*scale_graph_zero_yn == 'y') );

			strcpy( legend, datatype_name );

			strcpy(	legend,
				format_initial_capital( buffer, legend ) );

			grace_datatype->legend = strdup( legend );

			if ( datatype_type_xyhilo )
			{
				grace_datatype->
					datatype_type_bar_xy_xyhilo =
						"xyhilo";
			}
			else
			if ( *bar_graph_yn == 'y' )
			{
				grace_datatype->line_linestyle = 0;
				grace_datatype->
					datatype_type_bar_xy_xyhilo = "bar";
			}
			else
			{
				grace_datatype->
					datatype_type_bar_xy_xyhilo = "xy";
			}

			if ( datatype_type_xyhilo )
				grace_datatype->symbol_size =
					GRACE_XYHILO_SYMBOL_SIZE;

			list_set(
				grace_graph->datatype_list,
				grace_datatype );
		}
		list_set( graph_list, grace_graph );
		free_array_string_with_count( block, block_count );
	}
	pclose( p );
	return 1;
}

double grace_get_number_x_ticks_between_labels(
				LIST *datatype_list,
				int label_count )
{
	GRACE_DATATYPE *datatype;
	GRACE_DATASET *dataset;
	GRACE_POINT *point;
	int i = 0;
	LIST *label_point_double_list = list_new();
	double label_point;

	if ( !list_rewind( datatype_list ) )
	{
		fprintf( stderr, 
			 "Error in %s/%s(): empty datatype_list\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	do {
		datatype = list_get( datatype_list );

		if ( !list_rewind( datatype->dataset_list ) ) continue;

		do {
			dataset = list_get( datatype->dataset_list );

			if ( !list_rewind( dataset->point_list ) ) continue;


			do {
				point = list_get( dataset->point_list );


				if ( ! ( ++i % label_count ) )
				{
					label_point = point->x;

					list_append(	label_point_double_list,
							&label_point,
							sizeof( double ) );
				}

			} while( list_next( dataset->point_list ) );

		} while( list_next( datatype->dataset_list ) );

	} while( list_next( datatype_list ) );

	return get_average_difference_double( label_point_double_list );

}

double get_average_difference_double( LIST *double_list )
{
	double sum_difference = 0.0;
	int count = 0;
	double *double_pointer;
	double *next_double_pointer;

	if ( !list_rewind( double_list ) ) return 0.0;
	do {
		double_pointer = list_get( double_list );

		if ( list_next( double_list ) )
		{
			next_double_pointer =
				list_get(
					double_list );

			sum_difference += 
				(*next_double_pointer - *double_pointer );

			list_previous( double_list );

			count++;
		}

	} while( list_next( double_list ) );

	return sum_difference / (double)count;
}

void grace_populate_point_list( LIST *graph_list, 
				char *data_process,
				int datatype_entity_piece,
				int datatype_piece,
				int date_piece,
				int time_piece,
				int value_piece,
				enum grace_graph_type grace_graph_type,
				boolean datatype_type_xyhilo,
				boolean dataset_no_cycle_color )
{
	FILE *p;
	char buffer[ 4096 ];

	p = popen( data_process, "r" );

	/* ---------------------------------------------------- */
	/* Sample daily_value					*/
	/* ---------------------------------------------------- */
	/* BA|bottom_temperature|2000-06-01|null|29.334|24	*/
	/* BA|conductivity|2000-06-01|null|56.550|24		*/
	/* BA|rain|2000-06-01|null|0.000|24			*/
	/* BA|salinity|2000-06-01|null|37.671|24		*/
	/* BA|stage|2000-06-01|null|0.821|144			*/
	/* BA|surface_temperature|2000-06-01|null|29.540|24	*/
	/* BA|voltage|2000-06-01|null|13.050|24			*/
	/* ---------------------------------------------------- */
	while( get_line( buffer, p ) )
	{
		grace_set_string_to_point_list(
				graph_list, 
				datatype_entity_piece,
				datatype_piece,
				date_piece,
				time_piece,
				value_piece,
				buffer,
				grace_graph_type,
				datatype_type_xyhilo,
				dataset_no_cycle_color,
				(char *)0 /* optional_label */ );
	}
	pclose( p );
}

GRACE_DATATYPE *grace_get_grace_datatype(
			GRACE_GRAPH **return_graph,
			LIST *graph_list,
			char *datatype_entity,
			char *datatype_name,
			enum grace_graph_type grace_graph_type )
{
	GRACE_GRAPH *grace_graph;
	GRACE_DATATYPE *grace_datatype;

	if ( !return_graph )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: return_graph is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		return (GRACE_DATATYPE *)0;
	}

	if ( !list_rewind( graph_list ) )
	{
		*return_graph = (GRACE_GRAPH *)0;
		return (GRACE_DATATYPE *)0;
	}

	do {
		grace_graph = (GRACE_GRAPH *)list_get( graph_list );

		grace_datatype =
			grace_graph_get_grace_datatype(
				grace_graph->datatype_list,
				datatype_entity,
				datatype_name,
				grace_graph_type );

		if ( grace_datatype )
		{
			*return_graph = grace_graph;
			return grace_datatype;
		}
		
	} while( list_next( graph_list ) );

	*return_graph = (GRACE_GRAPH *)0;

	return( GRACE_DATATYPE *)0;
}

GRACE_DATATYPE *grace_graph_get_grace_datatype(
			LIST *grace_datatype_list,
			char *datatype_entity,
			char *datatype_name,
			enum grace_graph_type grace_graph_type )
{
	GRACE_DATATYPE *grace_datatype;

	if ( !list_rewind( grace_datatype_list ) ) return (GRACE_DATATYPE *)0;

	do {
		grace_datatype =
		   	list_get(
				grace_datatype_list );

		if ( !grace_datatype->datatype_name )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: grace_datatype->datatype_name is null\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( grace_graph_type == datatype_overlay_graph )
		{
			if ( strcasecmp(
				grace_datatype->datatype_name,
	    	     		datatype_name ) == 0 
			&&   strcasecmp(
				grace_datatype->datatype_entity,
		     		datatype_entity ) == 0 )
			{
				return grace_datatype;
			}
		}
		else
		if ( grace_datatype->datatype_entity
		&&  *grace_datatype->datatype_entity )
		{
			if ( strcasecmp(
				grace_datatype->datatype_name,
	    	     		datatype_name ) == 0 
			&&   strcasecmp(
				grace_datatype->datatype_entity,
		     		datatype_entity ) == 0 )
			{
				return grace_datatype;
			}
		}
		else
		{
			if ( strcasecmp(
				grace_datatype->datatype_name,
				datatype_name ) == 0 )
			{
				return grace_datatype;
			}
		}
	} while( list_next( grace_datatype_list ) );

	return (GRACE_DATATYPE *)0;
}


void grace_set_aggregate_variables(	GRACE *grace,
					JULIAN *begin_date_julian,
					JULIAN *end_date_julian,
					LIST *graph_list,
					int number_of_days,
					enum grace_graph_type grace_graph_type,
					double view_min_y,
					double view_y_offset,
					double view_y_increment,
					double view_x_range,
					int landscape_mode,
					boolean is_datatype_overlay_anchor )
{
	GRACE_GRAPH *grace_graph;
	double x_tick_major;
	int x_tick_minor;
	double y_tick_major = 0.0;
	int y_tick_minor;
	double view_min_x = STARTING_VIEW_MIN_X;
	int graph_list_length;
	double y_range;
	int decrement_x_axis_for_bar = 0;
	LIST *datatype_list;

	if ( !list_rewind( graph_list ) ) return;

	graph_list_length = list_length( graph_list );

	do {

		grace_graph = list_get( graph_list );

		datatype_list = grace_graph->datatype_list;

		if ( number_of_days )
		{
			grace_get_x_tick_major_minor(
					&x_tick_major,
					&x_tick_minor,
					number_of_days );
		}
		else
		{
			grace_get_x_tick_major_minor(
					&x_tick_major,
					&x_tick_minor,
					grace_get_x_range( 
						grace->world_min_x,
						grace->world_max_x ) );
		}

		grace_set_x_tick_major( grace_graph,
					x_tick_major );
	
		grace_set_x_tick_minor( grace_graph,
					x_tick_minor );

		grace_graph->world_max_y =
			ceiling_double(
				grace_get_maximum_y_double(
					datatype_list ) );

		grace_graph->world_min_y =
			floor_double(
				grace_get_minimum_y_double(
					datatype_list ) );

		if ( grace_graph->scale_to_zero )
		{
			y_range = grace_graph->world_max_y;
		}
		else
		{
			y_range =	grace_graph->world_max_y -
					grace_graph->world_min_y;
		}

		/* Set y axis info */
		/* --------------- */
		grace_get_y_tick_major_minor( 
				&y_tick_major, 
				&y_tick_minor,
				&grace_graph->yaxis_ticklabel_precision,
				y_range,
				graph_list_length,
				grace_graph->world_max_y );

		if ( !grace_graph->y_tick_major )
		{
			grace_set_y_tick_major( grace_graph,
						y_tick_major );
		}
	
		grace_set_y_tick_minor( grace_graph,
					y_tick_minor );
	
		if ( y_range > 500 )
			grace_graph->y_label_size = 0.50;

		if ( grace->grace_graph_type == xy_graph )
		{
			if ( !grace_graph->world_min_y )
			{
				grace_graph->world_min_y =
					floor_double(
						grace_get_minimum_y_double(
						datatype_list ) );
			}

			if ( !grace_graph->world_max_y )
			{
				grace_graph->world_max_y =
					ceiling_double(
						grace_get_maximum_y_double(
						datatype_list ) );
			}
		}
		else
		{
			if ( !grace_graph->world_min_y )
			{
				grace_graph->world_min_y =
					floor_double(
						grace_get_minimum_y_double(
						datatype_list ) );
			}

			if ( grace_graph->scale_to_zero )
			{
				grace_get_world_minimum_x(
					&grace->world_min_x,
					begin_date_julian,
					number_of_days,
					decrement_x_axis_for_bar );

				grace_get_world_minimum_y(
					&grace_graph->world_min_y,
					0.0 );
			}
			else
			{
				grace_get_world_minimum_x(
					&grace->world_min_x,
					begin_date_julian,
					number_of_days,
					decrement_x_axis_for_bar );

				grace_get_world_minimum_y(
					&grace_graph->world_min_y,
					floor_double(
						grace_get_minimum_y_double(
						datatype_list ) ) );
			}

			grace_get_world_maximum_x(
					&grace->world_max_x,
					end_date_julian );
		
			grace_get_world_maximum_y(
					&grace_graph->world_max_y,
					end_date_julian,
					ceiling_double(
						grace_get_maximum_y_double(
							datatype_list ) ) );

			grace_adjust_for_negative_y_max(
					&grace_graph->world_min_y,
					&grace_graph->world_max_y );

		} /* if not xy_graph */

		if ( grace_graph->scale_to_zero )
			grace_graph->world_min_y = 0.0;

		grace_graph->view_min_x = view_min_x;
		grace_graph->view_min_y = view_min_y;
		grace_graph->view_max_x = view_min_x + view_x_range;

		grace_graph->view_max_y = view_min_y + view_y_offset;

		if ( grace_graph_type == unit_graph 
		||   grace_graph_type == xy_graph )
		{
			grace_graph->legend_view_x = grace_graph->view_max_x;
			grace_graph->legend_view_y = grace_graph->view_max_y;
		}
		else
		if ( grace_graph_type == datatype_overlay_graph )
		{
			grace_move_legend_bottom_center(
				&grace_graph->legend_view_x,
				&grace_graph->legend_view_y,
				grace_graph->view_min_x,
				grace_graph->view_min_y,
				landscape_mode );

			if ( is_datatype_overlay_anchor )
			{
				grace_graph->legend_view_x -= 0.55;
			}
			else
			{
				grace_graph->legend_view_x -= 0.15;
			}
		}
		else
		if ( grace_graph_type == date_time_graph )
		{
			grace_move_legend_bottom_center(
				&grace_graph->legend_view_x,
				&grace_graph->legend_view_y,
				grace_graph->view_min_x,
				grace_graph->view_min_y,
				landscape_mode );

			if ( landscape_mode )
			{
				grace_graph->legend_view_x += 0.30;
			}
		}

		view_min_y += view_y_increment;

	} while( list_next( graph_list ) );

}

void grace_get_portrait_view_y_minimum_offset_and_increment(
					double *view_min_y,
					double *view_y_offset,
					double *view_y_increment,
					double starting_single_view_min_y,
					double starting_view_min_y,
					int graph_list_length )
{
	if ( graph_list_length == 1 )
	{
		*view_min_y = starting_single_view_min_y;
		*view_y_increment = 0.0;
		*view_y_offset = 0.46;
	}
	else
	if ( graph_list_length == 2 )
	{
		*view_min_y = starting_view_min_y + 0.12;
		*view_y_increment = 0.53;
		*view_y_offset = 0.47;
	}
	else
	if ( graph_list_length == 3 )
	{
		*view_min_y = starting_view_min_y;
		*view_y_increment = 0.41;
		*view_y_offset = 0.31;
	}
	else
	if ( graph_list_length == 4 )
	{
		*view_min_y = starting_view_min_y;
		*view_y_increment = 0.30;
		*view_y_offset = 0.23;
	}
	else
	if ( graph_list_length == 5 )
	{
		*view_min_y = starting_view_min_y;
		*view_y_increment = 0.24;
		*view_y_offset = 0.17;
	}
	else
	if ( graph_list_length == 6 )
	{
		*view_min_y = starting_view_min_y;
		*view_y_increment = 0.20;
		*view_y_offset = 0.14;
	}
	else
	if ( graph_list_length == 7 )
	{
		*view_min_y = starting_view_min_y;
		*view_y_increment = 0.17;
		*view_y_offset = 0.11;
	}
	else
	if ( graph_list_length == 8 )
	{
		*view_min_y = starting_view_min_y;
		*view_y_increment = 0.15;
		*view_y_offset = 0.09;
	}
	else
	if ( graph_list_length == 9 )
	{
		*view_min_y = starting_view_min_y;
		*view_y_increment = 0.13;
		*view_y_offset = 0.08;
	}
	else
	if ( graph_list_length == 10 )
	{
		*view_min_y = starting_view_min_y;
		*view_y_increment = 0.12;
		*view_y_offset = 0.075;
	}
	else
	if ( graph_list_length == 11 )
	{
		*view_min_y = starting_view_min_y;
		*view_y_increment = 0.11;
		*view_y_offset = 0.070;
	}
	else
	if ( graph_list_length == 12 )
	{
		*view_min_y = starting_view_min_y;
		*view_y_increment = 0.099;
		*view_y_offset = 0.065;
	}
	else
	{
		*view_min_y = starting_view_min_y;
		*view_y_increment = 0.90 / (double)graph_list_length;
		*view_y_offset = 0.75 / (double)graph_list_length;
	}
}

void grace_get_landscape_view_y_minimum_offset_and_increment(
					double *view_min_y,
					double *view_y_offset,
					double *view_y_increment,
					double starting_single_view_min_y,
					double starting_view_min_y,
					int graph_list_length )
{
	if ( graph_list_length == 1 )
	{
		*view_min_y = starting_single_view_min_y;
		*view_y_increment = 0.0;
		*view_y_offset = 0.46;
	}
	else
	if ( graph_list_length == 2 )
	{
		*view_min_y = starting_view_min_y;
		*view_y_increment = 0.44;
		*view_y_offset = 0.375;
	}
	else
	if ( graph_list_length == 3 )
	{
		*view_min_y = starting_view_min_y;
		*view_y_increment = 0.30;
		*view_y_offset = 0.23;
	}
	else
	if ( graph_list_length == 4 )
	{
		*view_min_y = starting_view_min_y;
		*view_y_increment = 0.23;
		*view_y_offset = 0.16;
	}
	else
	if ( graph_list_length == 5 )
	{
		*view_min_y = starting_view_min_y;
		*view_y_increment = 0.175;
		*view_y_offset = 0.12;
	}
	else
	{
		*view_min_y = starting_view_min_y;
		*view_y_increment = 0.75 / (double)graph_list_length;
		*view_y_offset = 0.60 / (double)graph_list_length;
	}
}

void grace_graph_set_scale_to_zero( 	GRACE_GRAPH *grace_graph,
				    	int scale_to_zero )
{
	if ( !grace_graph->scale_to_zero )
		grace_graph->scale_to_zero = scale_to_zero;
}

boolean grace_parse_time_hhmm( 	int *hour,
				int *minute,
				char *time )
{
	char buffer[ 3 ];

	if ( strlen( time ) != 4 ) return 0;

	*(buffer + 2) = '\0';

	*buffer = *time;
	*( buffer + 1 ) = *( time + 1 );
	*hour = atoi( buffer );

	*buffer = *( time + 2);
	*( buffer + 1 ) = *( time + 3 );
	*minute = atoi( buffer );
	return 1;
}

boolean grace_parse_time_hh_colon_mm(
				int *hour,
				int *minute,
				char *time )
{
	char buffer[ 3 ];

	if ( strlen( time ) != 5 ) return 0;

	*(buffer + 2) = '\0';

	*buffer = *time;
	*( buffer + 1 ) = *( time + 1 );
	*hour = atoi( buffer );

	*buffer = *( time + 3);
	*( buffer + 1 ) = *( time + 4 );
	*minute = atoi( buffer );
	return 1;
}


boolean grace_parse_date_hh_colon_mm(
				int *hour,
				int *minute,
				char *date_time_string )
{
	char time[ 128 ];

	if ( character_count( ' ', date_time_string ) != 1 ) return 0;

	column( time, 1, date_time_string );

	return grace_parse_time_hh_colon_mm(
			 	hour,
				minute,
				time );
}

boolean grace_parse_date_hhmm(	int *hour,
				int *minute,
				char *date_time_string )
{
	char time[ 128 ];

	if ( character_count( ':', date_time_string ) != 1 ) return 0;

	piece( time, ':', date_time_string, 1 );

	return grace_parse_time_hhmm(
			 	hour,
				minute,
				time );
}

boolean grace_parse_date_yyyy_mm_dd(	int *year,
		    			int *month,
		    			int *day,
		    			char *date )
{
	char buffer[ 128 ];

	if ( character_count( '-', date ) == 0 )
	{
		*year = atoi( buffer );
		*month = 12;
		*day = 31;
		return 1;
	}
	else
	if ( character_count( '-', date ) != 2 )
	{
		fprintf( stderr,
	"%s/%s()/%d: got an invalid date = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 date );
		return 0;
	}

	*year = atoi( piece( buffer, '-', date, 0 ) );
	*month = atoi( piece( buffer, '-', date, 1 ) );
	*day = atoi( piece( buffer, '-', date, 2 ) );
	return 1;
}

void grace_get_y_tick_major_minor(	
				double *y_tick_major,
				int *y_tick_minor,
				int *yaxis_ticklabel_precision,
				double y_range,
				int graph_list_length,
				double world_max_y )
{
	double range_div_twenty_five;

	range_div_twenty_five = y_range / 25.0;

	if ( y_range < 1 )
	{
		*y_tick_major = 0.05;
		*y_tick_minor = 1;
		*yaxis_ticklabel_precision = 2;
	}
	else
	if ( range_div_twenty_five < 0.01 )
	{
		*y_tick_major = 0.02;
		*y_tick_minor = 1;
		*yaxis_ticklabel_precision = 2;
	}
	else
	if ( range_div_twenty_five < 0.1 )
	{
		*y_tick_major = 0.2;
		*y_tick_minor = 1;
		*yaxis_ticklabel_precision = 1;
	}
	else
	if ( range_div_twenty_five < 0.5 )
	{
		*y_tick_major = 0.5;
		*y_tick_minor = 1;
	}
	else
	if ( range_div_twenty_five < 1.0 )
	{
		*y_tick_major = 1.0;
		*y_tick_minor = 1;
	}
	else
	if ( range_div_twenty_five < 2.0 )
	{
		*y_tick_major = 2.0;
		*y_tick_minor = 1;
	}
	else
	if ( range_div_twenty_five < 3.0 )
	{
		*y_tick_major = 3.0;
		*y_tick_minor = 1;
	}
	else
	if ( range_div_twenty_five < 4.0 )
	{
		*y_tick_major = 4.0;
		*y_tick_minor = 1;
	}
	else
	{
		*y_tick_major = (double)(int)range_div_twenty_five;
		*y_tick_minor = 1;
	}

	*y_tick_major *= (double)graph_list_length;

	if ( *y_tick_major >= world_max_y )
		*y_tick_major = world_max_y / 2.0;

}

void grace_get_x_tick_major_minor(
				double *x_tick_major,
				int *x_tick_minor,
				int number_of_days )
{
	if ( number_of_days == 1 )
	{
		*x_tick_major = 0.25;
		*x_tick_minor = 5;
	}
	else
	if ( number_of_days == 2 )
	{
		*x_tick_major = 0.50;
		*x_tick_minor = 11;
	}
	else
	if ( number_of_days == 3 )
	{
		*x_tick_major = 1.0;
		*x_tick_minor = 5 ;
	}
	else
	if ( number_of_days <= 7 )
	{
		*x_tick_major = 1.0 ;
		*x_tick_minor = 3 ;
	}
	else
	if ( number_of_days <= 14 )
	{
		*x_tick_major = 2.0;
		*x_tick_minor = 1;
	}
	else
	if ( number_of_days <= 62 )
	{
		*x_tick_major = 7.0;
		*x_tick_minor = 6;
	}
	else
	if ( number_of_days <= 100 )
	{
		*x_tick_major = 10.0;
		*x_tick_minor = 1;
	}
	else
	if ( number_of_days <= 180 )
	{
		*x_tick_major = 15.0;
		*x_tick_minor = 3;
	}
	else
	if ( number_of_days <= 366 )
	{
		*x_tick_major = 30.0;
		*x_tick_minor = 3;
	}
	else
	if ( number_of_days <= ( 366 * 2 ) )
	{
		*x_tick_major = 90.0;
		*x_tick_minor = 4 ;
	}
	else
	if ( number_of_days <= ( 366 * 3 ) )
	{
		*x_tick_major = 365.0 ;
		*x_tick_minor = 11 ;
	}
	else
	if ( number_of_days <= 366 * 10 )
	{
		*x_tick_major = 365.25 ;
		*x_tick_minor = 3 ;
	}
	else
	if ( number_of_days <=  ( 366 * 15 ) )
	{
		*x_tick_major = ( 365.0 * 2 ) ;
		*x_tick_minor = 1 ;
	}
	else
	if ( number_of_days <= ( 366 * 20 ) )
	{
		*x_tick_major = ( 365.0 * 2 ) ;
		*x_tick_minor = 1 ;
	}
	else
	{
		*x_tick_major = ( 365 * 4 ) ;
		*x_tick_minor = 3 ;
	}
}

boolean grace_set_string_to_point_list(
			LIST *graph_list, 
			int datatype_entity_piece,
			int datatype_piece,
			int date_piece,
			int time_piece,
			int value_piece,
			char *delimited_string,
			enum grace_graph_type grace_graph_type,
			boolean datatype_type_xyhilo,
			boolean dataset_no_cycle_color,
			char *optional_label )
{
	char datatype_entity_string[ 128 ];
	char datatype_name[ 128 ];
	char date_string[ 128 ];
	char time_string[ 128 ];
	char value_string[ 128 ];
	GRACE_DATATYPE *datatype;
	GRACE_GRAPH *grace_graph;
	int year, month, day, hour, minute;
	static char delimiter = {0};

	if ( !delimiter )
	{
		if ( ! ( delimiter =
				timlib_get_delimiter(
					delimited_string ) ) )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot find delimiter in (%s)\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				delimited_string );
			exit( 1 );
		}
	}

	if ( datatype_entity_piece != -1 )
	{
		piece(	datatype_entity_string, 
			delimiter, 
			delimited_string, 
			datatype_entity_piece );
	}
	else
	{
		*datatype_entity_string = '\0';
	}

	piece(	datatype_name, 
		delimiter, 
		delimited_string, 
		datatype_piece );

	if ( ! ( datatype =
			grace_get_grace_datatype(
				&grace_graph,
				graph_list,
				datatype_entity_string,
				datatype_name,
				grace_graph_type ) ) )
	{
		return 0;
	}

	piece(	date_string,
		delimiter,
		delimited_string,
		date_piece );

	if ( !grace_parse_date_yyyy_mm_dd(	
					&year,
					&month,
					&day,
					date_string ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s(): cannot parse date(%d) and time with (%s)\n",
			__FILE__,
			__FUNCTION__,
			date_piece,
			delimited_string );
		exit( 1 );
	}

	if ( time_piece == -1 )
	{
		if ( character_count( ' ', date_string ) == 1
		&&   character_count( ':', date_string ) == 1 )
		{
			grace_parse_date_hh_colon_mm(
					&hour,
					&minute,
					date_string );
		}
		else
		if ( character_count( ':', date_string ) == 1 )
		{
			grace_parse_date_hhmm(
					&hour,
					&minute,
					date_string );
		}
		else
		{
			hour = 0;
			minute = 0;
		}
	}
	else
	{
		piece( time_string, delimiter, delimited_string, time_piece );

		if ( strcasecmp( time_string, "null" ) == 0 )
		{
			hour = 0;
			minute = 0;
		}
		else
		{
			julian_get_hour_minute( &hour, &minute, time_string );
		}
	}

	if ( !piece( value_string, delimiter, delimited_string, value_piece ) )
	{
		*value_string = '\0';
	}

	grace_set_point( 
			&datatype->inside_null,
			datatype->dataset_list,
		 	greg2jul(month		/* mon  */,
			    	 day		/* day  */, 
			   	 year		/* year */, 
			   	 hour		/* hour */,
			 	 minute		/* min  */,
			    	 0.0		/* sec  */ ),
		 	strdup( value_string ),
			optional_label,
			datatype->datatype_number,
			dataset_no_cycle_color );

	if ( datatype_type_xyhilo )
	{
		GRACE_DATASET *dataset;
		GRACE_POINT *grace_point;

		if ( !list_length( datatype->dataset_list ) )
		{
			dataset =
				grace_new_grace_dataset(
					datatype->datatype_number,
					dataset_no_cycle_color );

			list_set(
				datatype->dataset_list,
				dataset );
		}
		else
		{
			dataset = list_last(
					datatype->dataset_list );
		}

		grace_point = list_last( dataset->point_list );

		if ( !grace_point )
		{
			return 0;
		}

		if ( !piece(
			value_string,
			delimiter,
			delimited_string,
			value_piece + 2 /* skip the count too */ ) )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot piece %d in (%s)\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				value_piece + 2,
				delimited_string );
			exit( 1 );
		}

		grace_point->low_string = strdup( value_string );

		if ( !piece(
			value_string,
			delimiter,
			delimited_string,
			value_piece + 3 ) )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot piece %d in (%s)\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				value_piece + 3,
				delimited_string );
			exit( 1 );
		}

		grace_point->high_string = strdup( value_string );
	}

	return 1;

}

int grace_set_begin_end_date(	GRACE *grace,
				char *begin_date_string,
				char *end_date_string )
{
	if ( ! ( grace->begin_date_julian =
			julian_yyyy_mm_dd_new( begin_date_string ) ) )
	{
		return 0;
	}

	if ( ! ( grace->end_date_julian =
			julian_yyyy_mm_dd_new( end_date_string ) ) )
	{
		return 0;
	}

	grace->number_of_days =
		julian_subtract(
			grace->end_date_julian,
			grace->begin_date_julian ) + 1;

	if ( grace->number_of_days <= 2 )
	{
		grace->xaxis_ticklabel_format = "mmddhms";
	}
	else
	if ( grace->number_of_days <= 180 )
	{
		grace->xaxis_ticklabel_format = "daymonth";
	}
	else
	if ( grace->number_of_days <= 366 )
	{
		grace->xaxis_ticklabel_format = "mmdd";
	}
	else
	{
		grace->xaxis_ticklabel_format = "yymmdd";
	}
	return 1;
}

void grace_increase_legend_char_size(
				GRACE_GRAPH *grace_graph,
				double increase_amount )
{
	grace_graph->legend_char_size += increase_amount;
}

double grace_get_legend_char_size(
				LIST *datatype_list,
				enum grace_graph_type grace_graph_type )
{
	GRACE_DATATYPE *datatype;
	int largest_legend_length = 0;
	int legend_length;
	double legend_char_size = GRACE_DEFAULT_LEGEND_CHAR_SIZE;

	if ( grace_graph_type == datatype_overlay_graph )
	{
		return legend_char_size;
	}
	else
	if ( grace_graph_type == date_time_graph )
	{
		return legend_char_size + 0.05;
	}

	if ( !list_rewind( datatype_list ) )
	{
		fprintf( stderr, 
		"ERROR: %s/%s() got empty datatype_list\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}
	do {
		datatype = list_get( datatype_list );

		legend_length = (datatype->legend ) ?
					strlen( datatype->legend ) :
					0;

		if ( legend_length > largest_legend_length )
			largest_legend_length = legend_length;

	} while( list_next( datatype_list ) );

	if ( largest_legend_length > 24 )
		legend_char_size = GRACE_DEFAULT_LEGEND_CHAR_SIZE - 0.25;
	else
	if ( largest_legend_length > 14 )
		legend_char_size = GRACE_DEFAULT_LEGEND_CHAR_SIZE - 0.15;

	return legend_char_size;
}

GRACE_DATATYPE_OVERLAY_INPUT_GROUP *
			grace_new_grace_datatype_overlay_input_group(
					char *anchor_datatype_entity,
					char *anchor_datatype,
					char *units,
					boolean bar_graph,
					boolean scale_graph_zero )
{
	GRACE_DATATYPE_OVERLAY_INPUT_GROUP *g;

	g = ( GRACE_DATATYPE_OVERLAY_INPUT_GROUP *)
		calloc( 1, sizeof( GRACE_DATATYPE_OVERLAY_INPUT_GROUP ) );

	if ( !g )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc failed\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	g->anchor_datatype_overlay_input =
			grace_new_grace_datatype_overlay_input(
					anchor_datatype_entity,
					anchor_datatype,
					units,
					bar_graph,
					scale_graph_zero );

	g->compare_datatype_overlay_input_list = list_new();
	return g;
}

GRACE_DATATYPE_OVERLAY_INPUT *grace_new_grace_datatype_overlay_input(
					char *datatype_entity,
					char *datatype,
					char *units,
					boolean bar_graph,
					boolean scale_graph_zero )
{
	GRACE_DATATYPE_OVERLAY_INPUT *a;

	a = (GRACE_DATATYPE_OVERLAY_INPUT *)
		calloc( 1, sizeof( GRACE_DATATYPE_OVERLAY_INPUT ) );

	if ( !a )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc failed\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	a->datatype_entity = datatype_entity;
	a->datatype = datatype;
	a->units = units;
	a->bar_graph = bar_graph;
	a->scale_graph_zero = scale_graph_zero;
	return a;
}

void grace_set_compare_datatype_overlay_input(
				LIST *compare_datatype_overlay_input_list,
				char *datatype_entity,
				char *datatype,
				char *units,
				boolean bar_graph,
				boolean scale_graph_zero )
{
	GRACE_DATATYPE_OVERLAY_INPUT *d;

	d = grace_new_grace_datatype_overlay_input(
				datatype_entity,
				datatype,
				units,
				bar_graph,
				scale_graph_zero );

	list_set(	compare_datatype_overlay_input_list,
			d );
}

void grace_populate_datatype_overlay_graph_list(
				LIST *graph_list,
				LIST *anchor_graph_list,
				GRACE_DATATYPE_OVERLAY_INPUT *
					anchor_datatype_overlay_input,
				LIST *compare_datatype_overlay_input_list )
{
	GRACE_GRAPH *grace_graph;
	GRACE_GRAPH *anchor_grace_graph;
	GRACE_DATATYPE_OVERLAY_INPUT *grace_datatype_overlay_input;
	GRACE_DATATYPE *anchor_grace_datatype, *grace_datatype;
	char legend[ 1024 ];
	char buffer[ 1024 ];

	/* Build the anchor graph */
	/* ---------------------- */
	anchor_grace_graph = grace_new_grace_graph();

	anchor_grace_graph->yaxis_tick_place_string = "normal";

	anchor_grace_datatype =
			grace_new_grace_datatype(
				anchor_datatype_overlay_input->datatype_entity,
				anchor_datatype_overlay_input->datatype );

	grace_graph_set_scale_to_zero(
			anchor_grace_graph,
			anchor_datatype_overlay_input->scale_graph_zero );

	anchor_grace_graph->units = anchor_datatype_overlay_input->units;

	if ( !*anchor_grace_graph->units )
		anchor_grace_graph->units = "unknown";


	sprintf( 	legend,
			"%s/%s (%s)",
			anchor_datatype_overlay_input->datatype_entity,
			anchor_datatype_overlay_input->datatype,
			anchor_grace_graph->units );

	anchor_grace_datatype->legend =
		strdup( format_initial_capital( buffer, legend ) );

	if ( anchor_datatype_overlay_input->bar_graph )
	{
		anchor_grace_datatype->datatype_type_bar_xy_xyhilo = "bar";
		anchor_grace_datatype->line_linestyle = 0;
	}
	else
	{
		anchor_grace_datatype->datatype_type_bar_xy_xyhilo = "xy";
	}

/*
	anchor_grace_datatype->datatype_type_bar_xy_xyhilo =
			(anchor_datatype_overlay_input->bar_graph)
			? "bar" : "xy";
*/

	list_set(	anchor_grace_graph->datatype_list,
			anchor_grace_datatype );

	/* If no datatypes to compare then return the anchor graph */
	/* ------------------------------------------------------- */
	if ( !list_rewind( compare_datatype_overlay_input_list ) )
	{
		list_set( anchor_graph_list, anchor_grace_graph );
		return;
	}

	/* For each compare graph */
	/* ---------------------- */
	do {
		grace_datatype_overlay_input =
			list_get(
				compare_datatype_overlay_input_list );

		grace_graph =
			grace_new_grace_graph();

		grace_graph_make_yaxis_opposite( grace_graph );

		grace_datatype =
				grace_new_grace_datatype(
					grace_datatype_overlay_input->
						datatype_entity,
					grace_datatype_overlay_input->
						datatype );
	
		grace_graph_set_scale_to_zero(
			grace_graph,
			grace_datatype_overlay_input->scale_graph_zero );
	
		grace_graph->units = grace_datatype_overlay_input->units;

		if ( !*grace_graph->units ) grace_graph->units = "unknown";

		sprintf( 	legend,
				"%s/%s (%s)",
				grace_datatype_overlay_input->datatype_entity,
				grace_datatype_overlay_input->datatype,
				grace_graph->units );
	
		grace_datatype->legend =
			strdup( format_initial_capital( buffer, legend ) );
	
		if ( grace_datatype_overlay_input->bar_graph )
		{
			grace_datatype->line_linestyle = 0;
			grace_datatype->datatype_type_bar_xy_xyhilo = "bar";
		}
		else
		{
			grace_datatype->datatype_type_bar_xy_xyhilo = "xy";
		}

/*
		grace_datatype->datatype_type_bar_xy_xyhilo =
				(grace_datatype_overlay_input->bar_graph)
				? "bar" : "xy";
*/

		/* Reverse the y axis for bars */
		/* --------------------------- */
		if ( strcmp( grace_datatype->datatype_type_bar_xy_xyhilo,
			     "bar" ) == 0
		&&   strcmp( anchor_grace_datatype->datatype_type_bar_xy_xyhilo,
			     "bar" ) == 0 )
		{
			grace_graph->y_invert_on_off = "on";
		}

		list_set(	grace_graph->datatype_list,
				grace_datatype );

		list_set( graph_list, grace_graph );

		list_set(	anchor_graph_list,
				anchor_grace_graph );

	} while( list_next( compare_datatype_overlay_input_list ) );
}

double grace_get_x_label_size( 	int number_of_days,
				int length_graph_list )
{
	if ( number_of_days <= 3 || length_graph_list > 5 )
		return 0.5;
	else
		return DEFAULT_X_LABEL_SIZE;
}

void grace_make_same_units_have_same_y_tick_marks(
						LIST *graph_list,
						LIST *anchor_graph_list )
{
	GRACE_GRAPH *graph, *anchor_graph;
	double new_world_min_y, new_world_max_y;
	double y_range;
	double y_tick_major = 0.0;
	int y_tick_minor;

	if ( !list_length( graph_list ) ) return;

	if ( list_length( graph_list ) != list_length( anchor_graph_list ) )
	{
		fprintf( stderr,
"ERROR in %s.%s(): graph list has different count than anchor graph list\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	list_rewind( graph_list );
	list_rewind( anchor_graph_list );

	do {
		graph = list_get( graph_list );

		anchor_graph = list_get( anchor_graph_list );

		if ( !graph->datatype_list
		||   list_length( graph->datatype_list ) != 1
		||   !anchor_graph->datatype_list
		||   list_length( anchor_graph->datatype_list ) != 1 )
		{
			fprintf( stderr,
"ERROR in %s/%s(): graph and anchor graph have imbalanced datatype lists\n",
			 __FILE__,
			 __FUNCTION__ );
			exit( 1 );
		}

		if ( strcmp( graph->units, "unknown" ) != 0
		&&   strcasecmp( graph->units,
			     anchor_graph->units ) == 0 )
		{
			grace_get_widest_world_y(
					&new_world_min_y,
					&new_world_max_y,
					graph->world_min_y,
					graph->world_max_y,
					anchor_graph->world_min_y,
					anchor_graph->world_max_y );

			graph->world_min_y = anchor_graph->world_min_y =
					new_world_min_y;

			graph->world_max_y = anchor_graph->world_max_y =
					new_world_max_y;

			y_range = graph->world_max_y - graph->world_min_y;

			grace_get_y_tick_major_minor( 
					&y_tick_major, 
					&y_tick_minor,
					&graph->yaxis_ticklabel_precision,
					y_range,
					list_length( graph_list ),
					graph->world_max_y );

			grace_set_y_tick_major( graph,
						y_tick_major );
	
			grace_set_y_tick_minor( graph,
						y_tick_minor );
	
			grace_set_y_tick_major( anchor_graph,
						y_tick_major );
	
			grace_set_y_tick_minor( anchor_graph,
						y_tick_minor );
		}

		list_next( anchor_graph_list );
	} while( list_next( graph_list ) );
}

void grace_get_widest_world_y(	double *new_world_min_y,
				double *new_world_max_y,
				double graph_world_min_y,
				double graph_world_max_y,
				double anchor_graph_world_min_y,
				double anchor_graph_world_max_y )
{
	if ( graph_world_min_y < anchor_graph_world_min_y )
		*new_world_min_y = graph_world_min_y;
	else
		*new_world_min_y = anchor_graph_world_min_y;

	if ( graph_world_max_y > anchor_graph_world_max_y )
		*new_world_max_y = graph_world_max_y;
	else
		*new_world_max_y = anchor_graph_world_max_y;

}

void grace_set_landscape(	int *page_width_pixels,
				int *page_length_pixels,
				char **distill_landscape_flag,
				double *starting_single_view_min_y,
				double *view_x_range )
{
	*page_width_pixels = LANDSCAPE_PAGE_WIDTH_PIXELS;
	*page_length_pixels = LANDSCAPE_PAGE_LENGTH_PIXELS;
/*
	*distill_landscape_flag = "-pagesize 11 8.5 in";
*/
	*distill_landscape_flag = GRACE_DISTILL_LANDSCAPE_FLAG;
	*starting_single_view_min_y -= 0.3;
	*view_x_range = LANDSCAPE_VIEW_X_RANGE;
}

GRACE *grace_new_grace( 	char *application_name,
				char *role_name )
{
	GRACE *grace;

	grace = (GRACE *)calloc( 1, sizeof( GRACE ) );

	if ( !grace )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc failed\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	grace->xaxis_ticklabel_precision = DEFAULT_XAXIS_TICKLABEL_PRECISION;

	if ( application_name && role_name
	&&   *application_name && *role_name )
	{
		ROLE *role =
			role_fetch(
				role_name,
				0 /* not fetch_role_attribute_exclude_list */ );

		grace->dataset_no_cycle_color =
			role->grace_no_cycle_colors;
	}

	return grace;

}

GRACE *grace_new_xy_grace(	char *application_name,
				char *role_name,
				char *title,
				char *sub_title,
				char *units,
				char *legend )
{
	GRACE *grace;
	GRACE_GRAPH *grace_graph;
	GRACE_DATATYPE *grace_datatype;

	grace = grace_new_grace(	application_name,
					role_name );

	grace->xaxis_ticklabel_format = "decimal";
	grace->x_label_size = DEFAULT_X_LABEL_SIZE;
	grace->grace_graph_type = xy_graph;
	grace->title = title;
	grace->sub_title = sub_title;

	grace->graph_list = list_new_list();
	grace_graph = grace_new_grace_graph();
	grace_graph->units = units;
	list_set( grace->graph_list, grace_graph );

	grace_graph->datatype_list = list_new_list();
	grace_datatype = grace_new_grace_datatype( 
				"" /* datatype_entity */,
				"" /* datatype_name */ );

	grace_datatype->legend = legend;
	grace_datatype->datatype_type_bar_xy_xyhilo = "xy";

	grace_datatype->dataset_list = list_new();
	list_set( grace_graph->datatype_list, grace_datatype );
	return grace;
}

void grace_set_xy_to_point_list(
				LIST *graph_list, 
				double x,
				char *y_string,
				char *optional_label,
				boolean dataset_no_cycle_color )
{
	GRACE_GRAPH *graph;
	GRACE_DATATYPE *datatype;

	graph = list_first( graph_list );

	datatype = list_first( graph->datatype_list );

	grace_set_to_point_list(
				datatype, 
				x,
				y_string,
				optional_label,
				dataset_no_cycle_color );
}

void grace_set_to_point_list(
				GRACE_DATATYPE *grace_datatype, 
				double x,
				char *y_string,
				char *optional_label,
				boolean dataset_no_cycle_color )
{
	GRACE_DATASET *dataset;
	GRACE_POINT *grace_point;

	if ( !list_length( grace_datatype->dataset_list ) )
	{
		dataset =
			grace_new_grace_dataset(
					grace_datatype->datatype_number,
					dataset_no_cycle_color );

		list_set(	grace_datatype->dataset_list,
				dataset );
	}
	else
	{
		dataset = list_last( grace_datatype->dataset_list );
	}

	grace_point =
		grace_new_grace_point(
			x,
			y_string,
			(char *)0 /* value_string */ );

	grace_point->optional_label = optional_label;

	list_set( dataset->point_list, grace_point );

}

double grace_get_x_range( double world_min_x,
			  double world_max_x )
{
	return world_max_x - world_min_x;
}

int grace_graph_list_empty( LIST *graph_list )
{
	GRACE_GRAPH *grace_graph;

	if ( !graph_list || !list_rewind( graph_list ) ) return 1;

	do {
		grace_graph = list_get( graph_list );

		if ( grace_graph_datatype_populated(
						grace_graph->datatype_list ) )
		{
			return 0;
		}

	} while( list_next( graph_list ) );

	return 1;
}

void grace_adjust_for_negative_y_max(
					double *world_min_y,
					double *world_max_y )
{
	if ( *world_min_y > *world_max_y )
	{
		double tmp = *world_min_y;
		*world_min_y = *world_max_y;
		*world_max_y = tmp;
	}
}


boolean grace_output_charts(	char *output_filename, 
				char *postscript_filename,
				char *agr_filename,
				char *title,
				char *sub_title,
				char *xaxis_ticklabel_format,
				enum grace_graph_type grace_graph_type,
				double x_label_size,
				int page_width_pixels,
				int page_length_pixels,
				char *grace_home_directory,
				char *grace_execution_directory,
				boolean grace_free_option,
				char *grace_output,
				char *distill_directory,
				char *distill_landscape_flag,
				char *ghost_script_directory,
				LIST *optional_label_list,
				boolean symbols,
				double world_min_x,
				double world_max_x,
				int xaxis_ticklabel_precision,
				LIST *graph_list,
				LIST *anchor_graph_list )
{
	if ( !grace_output_to_file(	output_filename, 
					postscript_filename,
					agr_filename,
					title,
					sub_title,
					xaxis_ticklabel_format,
					grace_graph_type,
					x_label_size,
					page_width_pixels,
					page_length_pixels,
					grace_home_directory,
					grace_execution_directory,
					grace_free_option,
					grace_output,
					optional_label_list,
					symbols,
					graph_list,
					anchor_graph_list,
					xaxis_ticklabel_precision,
					world_min_x,
					world_max_x ) )
	{
		return 0;
	}

	if ( strcmp( grace_output, "postscript_pdf" ) == 0 )
	{
		if ( !grace_convert_postscript_to_pdf(
					output_filename, 
					postscript_filename,
					distill_directory,
					distill_landscape_flag,
					ghost_script_directory ) )
		{
			return 0;
		}
	}
	return 1;
}

void grace_graph_set_global_world_max_y(LIST *graph_list,
					double ymax )
{
	GRACE_GRAPH *grace_graph;

	if ( list_rewind( graph_list ) )
	{
		do {
			grace_graph = list_get( graph_list );
			grace_graph->world_max_y = ymax;
		} while( list_next( graph_list ) );
	}
}

void grace_graph_set_global_world_min_y(LIST *graph_list,
					double world_min_y )
{
	GRACE_GRAPH *grace_graph;

	if ( list_rewind( graph_list ) )
	{
		do {
			grace_graph = list_get( graph_list );
			grace_graph->world_min_y = world_min_y;
		} while( list_next( graph_list ) );
	}
}

void grace_graph_set_global_y_tick_major(
				LIST *graph_list,
				double y_tick_major )
{
	GRACE_GRAPH *grace_graph;

	if ( list_rewind( graph_list ) )
	{
		do {
			grace_graph = list_get( graph_list );
			grace_graph->y_tick_major = y_tick_major;
		} while( list_next( graph_list ) );
	}
}

void grace_graph_set_global_y_tick_minor(
				LIST *graph_list,
				int y_tick_minor )
{
	GRACE_GRAPH *grace_graph;

	if ( list_rewind( graph_list ) )
	{
		do {
			grace_graph = list_get( graph_list );
			grace_graph->y_tick_minor = y_tick_minor;
		} while( list_next( graph_list ) );
	}
}

char *grace_get_agr_prompt( void )
{
	char *agr_prompt;

	agr_prompt =
"<br>&lt;Left Click&gt; to view Grace project file or &lt;Right Click&gt; to save.";
	return agr_prompt;
}

void grace_output_grace_home_link( void )
{
	printf(
"<br><p>For more information, visit the <a href=\"http://plasma-gate.weizmann.ac.il/Grace/\" target=_new>Grace Home Page.</a>" );
}

LIST *grace_get_optional_label_list( LIST *graph_list )
{
	LIST *optional_label_list;
	GRACE_GRAPH *grace_graph;
	GRACE_DATATYPE *grace_datatype;
	GRACE_DATASET *grace_dataset;
	GRACE_POINT *grace_point;
	GRACE_OPTIONAL_LABEL *optional_label;

	optional_label_list = list_new_list();

	if ( !list_rewind( graph_list ) ) return optional_label_list;

	do {
		grace_graph = list_get( graph_list );

		if ( !list_rewind( grace_graph->datatype_list ) ) continue;

		do {
			grace_datatype =
				list_get(
					grace_graph->datatype_list );

			if ( !list_rewind( grace_datatype->dataset_list ) )
				continue;

			do {
				grace_dataset =
					list_get(
						grace_datatype->dataset_list );

				if ( !list_rewind(
					grace_dataset->point_list ) )
				{
					continue;
				}

				do {
					grace_point =
						list_get(
							grace_dataset->
								point_list );

					optional_label =
					grace_new_optional_label(
						grace_point->x,
						grace_point->
							optional_label );

					list_set(
						optional_label_list,
						optional_label );
				} while( list_next(
						grace_dataset->point_list) );
			} while( list_next( grace_datatype->dataset_list ) );
		} while( list_next( grace_graph->datatype_list ) );
	} while( list_next( graph_list ) );
	return optional_label_list;
}

void grace_output_graph_window(
			char *application_name,
			char *pdf_output_filename,
			char *ftp_agr_filename,
			boolean with_document_output,
			char *where_clause )
{
	char window_label[ 128 ];

	sprintf( window_label, "grace_window_%d", getpid() );

	if ( with_document_output )
	{
		document_quick_output( application_name );
	}

	printf(
"<body bgcolor=\"%s\" onload=\"window.open('%s','%s','menubar=no,resizable=yes,scrollbars=yes,status=no,toolbar=no,location=no', 'false');\">\n",
			application_background_color(
				application_name ),
			pdf_output_filename,
			window_label );

	grace_output_screen_title();

	if ( where_clause && *where_clause )
		printf( "<br>Search criteria: %s\n", where_clause );

	printf(
"<br><hr><a href=\"%s\" target=\"%s\">Press to view graph.</a>\n",
			pdf_output_filename,
			window_label );

	appaserver_library_output_ftp_prompt(
		ftp_agr_filename, 
		grace_get_agr_prompt(),
		(char *)0    /* target */,
		"application/agr" );

	grace_output_grace_home_link();

	if ( with_document_output )
	{
		printf( "%s\n", document_close_html() );
	}

	fflush( stdout );
}

void grace_email_graph(	char *application_name,
			char *email_address,
			char *chart_email_command_line,
			char *pdf_jpg_filename,
			boolean with_document_output,
			char *where_clause )
{
	char search_replace_command_line[ 512 ];
	char sys_string[ 1024 ];

	if ( with_document_output )
	{
		document_quick_output( application_name );
	}

	printf( "<h1>Emailed graph to %s ", email_address );
	fflush( stdout );

	if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ){};

	printf( "</h1>\n" );

	if ( where_clause && *where_clause )
		printf( "<br>Search criteria: %s\n", where_clause );

	strcpy( search_replace_command_line, chart_email_command_line );

	search_replace_string(	search_replace_command_line,
				"{filename}",
				pdf_jpg_filename );

	search_replace_string(	search_replace_command_line,
				"{}",
				email_address );

	sprintf(sys_string,
		"echo '' | %s",
		search_replace_command_line );

	if ( system( sys_string ) ) {};

	if ( with_document_output )
	{
		printf( "%s\n", document_close_html() );
	}
}

void grace_error_exit( char *function_name, int line_number )
{
	fprintf( stderr,
		"%s/%s(): Fatal error at line %d\n",
		__FILE__,
		function_name,
		line_number );
	exit( 1 );
}

void grace_set_world_min_y_based_on_grace_point_low(
				LIST *graph_list )
{
	GRACE_GRAPH *grace_graph;

	if ( !list_rewind( graph_list ) ) return;
	do {
		grace_graph = list_get( graph_list );
		grace_graph->world_min_y =
			floor_double(
				grace_get_minimum_y_from_point_low(
					grace_graph->datatype_list ) );
	} while( list_next( graph_list ) );
}

void grace_set_world_max_y_based_on_grace_point_high(
				LIST *graph_list )
{
	GRACE_GRAPH *grace_graph;

	if ( !list_rewind( graph_list ) ) return;
	do {
		grace_graph = list_get( graph_list );
		grace_graph->world_max_y =
			ceiling_double(
				grace_get_maximum_y_from_point_high(
					grace_graph->datatype_list ) );

	} while( list_next( graph_list ) );
}

void grace_set_world_min_x_based_on_grace_point_low(
				double *world_min_x,
				LIST *graph_list )
{
	GRACE_GRAPH *grace_graph;

	if ( !list_rewind( graph_list ) ) return;
	do {
		grace_graph = list_get( graph_list );
		*world_min_x =
			floor_double(
				grace_get_minimum_x_from_point_low(
					grace_graph->datatype_list ) );

	} while( list_next( graph_list ) );
}

void grace_set_world_max_x_based_on_grace_point_high(
				double *world_max_x,
				LIST *graph_list )
{
	GRACE_GRAPH *grace_graph;

	if ( !list_rewind( graph_list ) ) return;
	do {
		grace_graph = list_get( graph_list );
		*world_max_x =
			ceiling_double(
				grace_get_maximum_x_from_point_high(
					grace_graph->datatype_list ) );

	} while( list_next( graph_list ) );
}

double grace_get_minimum_x_from_point_low(
					LIST *datatype_list )
{
	GRACE_POINT *p;
	GRACE_DATATYPE *datatype;
	GRACE_DATASET *dataset;
	double minimum = 9999999999.99;

	if ( !list_rewind( datatype_list ) )
	{
		fprintf( stderr,
		"ERROR in %s(): empty datatype list\n", __FUNCTION__ );
		exit( 1 );
	}

	do {
		datatype = list_get( datatype_list );

		if ( !list_rewind( datatype->dataset_list ) )
			continue;

		do {
			dataset = list_get( datatype->dataset_list );

			if ( !list_rewind( dataset->point_list ) )
				continue;

			do {
				p = list_get( dataset->point_list );

				if ( p->x < minimum )
				{
					minimum = p->x;
				}

			} while( list_next( dataset->point_list ) );
		} while( list_next( datatype->dataset_list ) );
	} while( list_next( datatype_list ) );
	return minimum;
}

double grace_get_minimum_y_from_point_low(
				LIST *datatype_list )
{
	GRACE_POINT *p;
	GRACE_DATATYPE *datatype;
	GRACE_DATASET *dataset;
	double minimum = 9999999999.99;
	double low;

	if ( !list_rewind( datatype_list ) )
	{
		fprintf( stderr,
		"ERROR in %s(): empty datatype list\n", __FUNCTION__ );
		exit( 1 );
	}

	do {
		datatype = list_get( datatype_list );

		if ( !list_rewind( datatype->dataset_list ) )
			continue;

		do {
			dataset = list_get( datatype->dataset_list );

			if ( !list_rewind( dataset->point_list ) )
				continue;

			do {
				p = list_get( dataset->point_list );

				if ( grace_is_null( p->y_string ) )
					continue;

				if ( p->low_string )
					low = atof( p->low_string );
				else
					low = atof( p->y_string );

				if ( low < minimum )
				{
					minimum = low;
				}

			} while( list_next( dataset->point_list ) );
		} while( list_next( datatype->dataset_list ) );
	} while( list_next( datatype_list ) );
	return minimum;
}

double grace_get_maximum_x_from_point_high(
				LIST *datatype_list )
{
	GRACE_POINT *p;
	GRACE_DATATYPE *datatype;
	GRACE_DATASET *dataset;
	double maximum = -9999999999.99;

	if ( !list_rewind( datatype_list ) )
	{
		fprintf( stderr,
		"ERROR in %s(): empty datatype list\n", __FUNCTION__ );
		exit( 1 );
	}

	do {
		datatype = list_get( datatype_list );

		if ( !list_rewind( datatype->dataset_list ) ) continue;

		do {
			dataset = list_get( datatype->dataset_list );

			if ( !list_rewind( dataset->point_list ) )
				continue;

			do {
				p = list_get( dataset->point_list );

				if ( p->x > maximum )
				{
					maximum = p->x;
				}

			} while( list_next( dataset->point_list ) );
		} while( list_next( datatype->dataset_list ) );
	} while( list_next( datatype_list ) );
	return maximum;
}

double grace_get_maximum_y_from_point_high(
				LIST *datatype_list )
{
	GRACE_POINT *p;
	GRACE_DATATYPE *datatype;
	GRACE_DATASET *dataset;
	double maximum = -9999999999.99;
	double high;

	if ( !list_rewind( datatype_list ) )
	{
		fprintf( stderr,
		"ERROR in %s(): empty datatype list\n", __FUNCTION__ );
		exit( 1 );
	}

	do {
		datatype = list_get( datatype_list );

		if ( !list_rewind( datatype->dataset_list ) ) continue;

		do {
			dataset = list_get( datatype->dataset_list );

			if ( !list_rewind( dataset->point_list ) )
				continue;

			do {
				p = list_get( dataset->point_list );

				if ( grace_is_null( p->y_string ) )
					continue;

				if ( p->high_string )
					high = atof( p->high_string );
				else
					high = atof( p->y_string );

				if ( high > maximum )
				{
					maximum = high;
				}

			} while( list_next( dataset->point_list ) );
		} while( list_next( datatype->dataset_list ) );
	} while( list_next( datatype_list ) );
	return maximum;
}

void grace_set_bar_graph( LIST *graph_list )
{
	GRACE_GRAPH *grace_graph;
	GRACE_DATATYPE *grace_datatype;

	if ( !list_rewind( graph_list ) ) return;

	do {
		grace_graph = list_get( graph_list );
		if ( grace_graph->datatype_list
		&&   list_rewind( grace_graph->datatype_list ) )
		{
			do {
				grace_datatype =
					list_get(
						grace_graph->
							datatype_list );

				grace_datatype->datatype_type_bar_xy_xyhilo =
					"bar";

				grace_datatype->line_linestyle = 0;

			} while( list_next( grace_graph->datatype_list ) );
		}
	} while( list_next( graph_list ) );
}

void grace_set_symbol_size( LIST *graph_list, double symbol_size )
{
	GRACE_GRAPH *grace_graph;
	GRACE_DATATYPE *grace_datatype;

	if ( !list_rewind( graph_list ) ) return;

	do {
		grace_graph = list_get( graph_list );
		if ( grace_graph->datatype_list
		&&   list_rewind( grace_graph->datatype_list ) )
		{
			do {
				grace_datatype =
					list_get(
						grace_graph->
							datatype_list );

				grace_datatype->symbol_size = symbol_size;

			} while( list_next( grace_graph->datatype_list ) );
		}

	} while( list_next( graph_list ) );
}

void grace_set_xaxis_ticklabel_angle(	LIST *graph_list,
					int xaxis_ticklabel_angle )
{
	GRACE_GRAPH *grace_graph;

	if ( !list_rewind( graph_list ) ) return;

	do {
		grace_graph = list_get( graph_list );
		grace_graph->xaxis_ticklabel_angle = xaxis_ticklabel_angle;

	} while( list_next( graph_list ) );
}

void grace_set_yaxis_ticklabel_precision(
					LIST *graph_list,
					int yaxis_ticklabel_precision )
{
	GRACE_GRAPH *grace_graph;

	if ( !list_rewind( graph_list ) ) return;

	do {
		grace_graph = list_get( graph_list );
		grace_graph->yaxis_ticklabel_precision =
			yaxis_ticklabel_precision;

	} while( list_next( graph_list ) );
}

void grace_set_scale_to_zero( LIST *graph_list )
{
	GRACE_GRAPH *grace_graph;

	if ( !list_rewind( graph_list ) ) return;

	do {
		grace_graph = list_get( graph_list );
		grace_graph->scale_to_zero = 1;
	} while( list_next( graph_list ) );
}

void grace_set_x_minor_tickmarks_to_zero( LIST *graph_list )
{
	GRACE_GRAPH *grace_graph;

	if ( !list_rewind( graph_list ) ) return;

	do {
		grace_graph = list_get( graph_list );
		grace_graph->x_tick_minor = 0;

	} while( list_next( graph_list ) );
}

void grace_set_y_minor_tickmarks_to_zero( LIST *graph_list )
{
	GRACE_GRAPH *grace_graph;

	if ( !list_rewind( graph_list ) ) return;

	do {
		grace_graph = list_get( graph_list );
		grace_graph->y_tick_minor = 0;

	} while( list_next( graph_list ) );
}

GRACE_OPTIONAL_LABEL *grace_new_optional_label(
						double grace_point_x,
						char *optional_label )
{
	GRACE_OPTIONAL_LABEL *grace_optional_label;

	grace_optional_label =
		(GRACE_OPTIONAL_LABEL *)
			calloc( 1, sizeof( GRACE_OPTIONAL_LABEL ) );

	if ( !grace_optional_label )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc failed\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	grace_optional_label->x = grace_point_x;
	grace_optional_label->optional_label = optional_label;
	return grace_optional_label;
}

void grace_set_xaxis_tickmarks_off( LIST *graph_list )
{
	GRACE_GRAPH *grace_graph;

	if ( !list_rewind( graph_list ) ) return;

	do {
		grace_graph = list_get( graph_list );
		grace_graph->xaxis_tickmarks_on_off = "off";

	} while( list_next( graph_list ) );
}

boolean grace_no_legend_set( LIST *graph_list )
{
	GRACE_GRAPH *grace_graph;
	GRACE_DATATYPE *grace_datatype;

	if ( !list_rewind( graph_list ) ) return 1;

	do {
		grace_graph = list_get( graph_list );
		if ( grace_graph->datatype_list
		&&   list_rewind( grace_graph->datatype_list ) )
		{
			do {
				grace_datatype =
					list_get(
						grace_graph->
							datatype_list );

				if ( grace_datatype->legend
				&&   *grace_datatype->legend)
				{
					return 0;
				}

			} while( list_next( grace_graph->datatype_list ) );
		}
	} while( list_next( graph_list ) );
	return 1;
}

void grace_lower_legend(	LIST *graph_list,
				double pixels )
{
	GRACE_GRAPH *grace_graph;

	if ( !list_rewind( graph_list ) ) return;

	do {
		grace_graph = list_get( graph_list );
		grace_graph->legend_view_y -= pixels;
	} while( list_next( graph_list ) );
}

void grace_graph_make_yaxis_opposite( GRACE_GRAPH *grace_graph )
{
	grace_graph->yaxis_tick_place_string = "opposite";
	grace_graph->yaxis_label_place_string =
		OPPOSITE_YAXIS_LABEL_PLACE;
	grace_graph->yaxis_ticklabel_place_string =
		OPPOSITE_YAXIS_TICKLABEL_PLACE;
	grace_graph->xaxis_ticklabel_on_off = "off";
	grace_graph->xaxis_tickmarks_on_off = "off";
}

void grace_set_first_graph_top_of_second( LIST *graph_list )
{
	GRACE_GRAPH *first_grace_graph;
	GRACE_GRAPH *second_grace_graph;

	if ( list_length( graph_list ) < 2 ) return;

	list_rewind( graph_list );
	first_grace_graph = list_get( graph_list );
	list_next( graph_list );
	second_grace_graph = list_get( graph_list );

	first_grace_graph->view_min_x = second_grace_graph->view_min_x;
	first_grace_graph->view_max_x = second_grace_graph->view_max_x;
	first_grace_graph->view_min_y = second_grace_graph->view_min_y;
	first_grace_graph->view_max_y = second_grace_graph->view_max_y;

}

void grace_move_legend_bottom_left(
			GRACE_GRAPH *grace_graph,
			boolean landscape_mode )
{
	grace_graph->legend_view_x = 0.17;
	grace_graph->legend_view_y = 0.61;

	if ( landscape_mode )
	{
		grace_graph->legend_view_y -= 0.30;
	}
}

void grace_move_legend_bottom_right(
			GRACE_GRAPH *grace_graph,
			boolean landscape_mode )
{
	grace_graph->legend_view_x = 0.62;
	grace_graph->legend_view_y = 0.61;

	if ( landscape_mode )
	{
		grace_graph->legend_view_y -= 0.30;
	}
}

void grace_graph_increment_line_color( LIST *datatype_list )
{
	GRACE_DATATYPE *grace_datatype;
	GRACE_DATASET *grace_dataset;

	if ( !list_rewind( datatype_list ) ) return;

	do {
		grace_datatype = list_get( datatype_list );

		if ( !list_rewind( grace_datatype->dataset_list ) )
			continue;

		do {
			grace_dataset =
				list_get(
					grace_datatype->dataset_list );
			grace_dataset->line_color++;

		} while( list_next( grace_datatype->dataset_list ) );

	} while( list_next( datatype_list ) );
}

void grace_graph_set_no_yaxis_grid_lines_offset(
					LIST *graph_list,
					int graph_offset )
{
	GRACE_GRAPH *grace_graph;
	int this_graph = 0;

	if ( !list_rewind( graph_list ) ) return;

	do {
		grace_graph = list_get( graph_list );

		if ( this_graph == graph_offset )
		{
			grace_graph->yaxis_grid_lines = 0;
			return;
		}

		this_graph++;

	} while( list_next( graph_list ) );
}

boolean grace_all_datatype_nodisplay_legend( LIST *datatype_list )
{
	GRACE_DATATYPE *datatype;

	if ( !list_rewind( datatype_list ) )
	{
		return 0;
	}

	do {
		datatype = list_get( datatype_list );
		if ( !datatype->nodisplay_legend ) return 0;

	} while( list_next( datatype_list ) );

	return 1;
}

void grace_set_line_linestyle( LIST *datatype_list, int line_linestyle )
{
	GRACE_DATATYPE *datatype;

	if ( !list_rewind( datatype_list ) )
	{
		return;
	}

	do {
		datatype = list_get( datatype_list );
		datatype->line_linestyle = line_linestyle;
	} while( list_next( datatype_list ) );
}

void grace_set_yaxis_tickmajor_integer_ceiling( LIST *graph_list )
{
	GRACE_GRAPH *grace_graph;

	if ( !list_rewind( graph_list ) ) return;

	do {
		grace_graph = list_get( graph_list );
		grace_graph->y_tick_major =
			(double)(int)grace_graph->y_tick_major + 0.9999999;
	} while( list_next( graph_list ) );
}

void grace_graph_set_no_yaxis_grid_lines(
				LIST *graph_list )
{
	GRACE_GRAPH *grace_graph;

	if ( !list_rewind( graph_list ) ) return;

	do {
		grace_graph = list_get( graph_list );
		grace_graph->yaxis_grid_lines = 0;

	} while( list_next( graph_list ) );
}

boolean grace_is_null( char *y_string )
{
	if ( !y_string ) return 1;
	if ( !*y_string ) return 1;
	if ( strcmp( y_string, "null" ) == 0 ) return 1;
	if ( strcmp( y_string, "NULL" ) == 0 ) return 1;
	return 0;
}

boolean grace_contains_multiple_datatypes(
					LIST *graph_list )
{
	GRACE_GRAPH *grace_graph;
	int datatype_count = 0;

	if ( !list_rewind( graph_list ) ) return 0;

	do {
		grace_graph = list_get( graph_list );

		if ( !list_length( grace_graph->datatype_list ) )
		{
			break;
		}

		datatype_count += list_length( grace_graph->datatype_list );

	} while( list_next( graph_list ) );

	if ( datatype_count >= 2 )
		return 1;
	else
		return 0;

}

GRACE_DATASET *grace_new_grace_dataset(
			int datatype_number,
			boolean dataset_no_cycle_color )
{
	GRACE_DATASET *grace_dataset;
	static int dataset_number = 0;

	if ( ! ( grace_dataset =
			(GRACE_DATASET *)
				calloc( 1, sizeof( GRACE_DATASET ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot allocation memory.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	grace_dataset->dataset_number = dataset_number++;

	if ( dataset_no_cycle_color )
		grace_dataset->line_color = datatype_number + 1;
	else
		grace_dataset->line_color = grace_dataset->dataset_number + 1;

	grace_dataset->point_list = list_new();
	return grace_dataset;

}

void grace_move_legend_up(	double *legend_view_y,
				double up_increment )
{
	*legend_view_y = *legend_view_y + up_increment;

}

void grace_move_legend_bottom_center(
				double *legend_view_x,
				double *legend_view_y,
				double view_min_x,
				double view_min_y,
				boolean landscape_mode )
{
	*legend_view_x = view_min_x + 0.52;
	*legend_view_y = view_min_y - 0.02;

	if ( landscape_mode )
	{
		*legend_view_x += 0.30;
	}
}

void grace_output_screen_title( void )
{
	printf( "<h1>Grace Project Viewer<br>");
	fflush( stdout );

	if ( system( timlib_system_date_string() ) ) {};

	printf( "</h1>\n" );

}

void grace_output_horizontal_line_at_point(
				FILE *output_pipe,
				double horizontal_line_at_point )
{
	fprintf( output_pipe,
		 "@with line\n"
		 "@	line on\n"
		 "@	line loctype world\n"
		 "@	line g0\n"
		 "@	line 0, %.1lf, 100, %.1lf\n"
		 "@	line linewidth 1.5\n"
		 "@	line linestyle 1\n"
		 "@	line color 1\n"
		 "@	line arrow 0\n"
		 "@	line arrow type 0\n"
		 "@	line arrow length 1.0\n"
		 "@	line arrow layout 1.0,1.0\n"
		 "@line def\n",
		 horizontal_line_at_point,
		 horizontal_line_at_point );
}

GRACE_DATATYPE *grace_datatype_seek(	LIST *datatype_list,
					int datatype_number )
{
	GRACE_DATATYPE *grace_datatype;

	if ( !list_rewind( datatype_list ) ) return (GRACE_DATATYPE *)0;

	do {
		grace_datatype = list_get( datatype_list );

		if ( grace_datatype->datatype_number == datatype_number )
			return grace_datatype;

	} while( list_next( datatype_list ) );

	return (GRACE_DATATYPE *)0;

}

GRACE_DATATYPE *grace_get_or_set_datatype(
					LIST *datatype_list,
					int datatype_number )
{
	GRACE_DATATYPE *grace_datatype;

	if ( ! ( grace_datatype = grace_datatype_seek(
					datatype_list,
					datatype_number ) ) )
	{
		grace_datatype =
			grace_new_grace_datatype( 
					(char *)0 /* datatype_entity */,
					(char *)0 /* datatype_name */ );

		list_set( datatype_list, grace_datatype );
	}

	return grace_datatype;

}

void grace_datatype_set_legend(		LIST *datatype_list,
					int datatype_number,
					char *legend )
{
	GRACE_DATATYPE *grace_datatype;

	grace_datatype = grace_get_or_set_datatype(
					datatype_list,
					datatype_number );

	grace_datatype->legend = legend;

}

void grace_set_datatype_number_point(
				GRACE_GRAPH *grace_graph,
				double x,
				char *y_string,
				int datatype_number )
{
	GRACE_DATATYPE *grace_datatype;
	GRACE_DATASET *grace_dataset;
	GRACE_POINT *point;

	grace_datatype = grace_get_or_set_datatype(
					grace_graph->datatype_list,
					datatype_number );

	if ( !list_length( grace_datatype->dataset_list ) )
	{
		grace_dataset =
			grace_new_grace_dataset(
					datatype_number,
					1 /* dataset_no_cycle_color */ );

		list_set(	grace_datatype->dataset_list,
				grace_dataset );
	}
	else
	{
		grace_dataset =
			list_last(
				grace_datatype->dataset_list );
	}

	point = grace_new_grace_point(
			x,
			y_string,
			(char *)0 /* optional_label */ );

	list_set( grace_dataset->point_list, point );

}

void grace_datatype_list_set_datatype_type_bar_xy_xyhilo(
				LIST *graph_list,
				char *datatype_type_bar_xy_xyhilo )
{
	GRACE_GRAPH *grace_graph;
	GRACE_DATATYPE *grace_datatype;
	LIST *datatype_list;

	if ( !list_rewind( graph_list ) ) return;

	do {
		grace_graph = list_get( graph_list );

		datatype_list = grace_graph->datatype_list;

		if ( !list_rewind( datatype_list ) ) continue;

		do {
			grace_datatype = list_get( datatype_list );

			grace_datatype->datatype_type_bar_xy_xyhilo =
				datatype_type_bar_xy_xyhilo;

		} while( list_next( datatype_list ) );

	} while( list_next( graph_list ) );

}

