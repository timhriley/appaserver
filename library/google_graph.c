/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/google_graph.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "String.h"
#include "piece.h"
#include "date.h"
#include "date_convert.h"
#include "application.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "environ.h"
#include "document.h"
#include "google.h"
#include "google_graph.h"

GOOGLE_GRAPH *google_graph_new(
		char *application_name,
		char *session_key,
		char *data_directory,
		char *date_attribute_name,
		char *time_attribute_name,
		char *number_attribute_name,
		LIST *query_fetch_row_list,
		char *chart_title,
		char *chart_filename_key /* static memory */,
		int chart_number,
		char *google_graph_function
			/* either LineChart or ColumnChart */ )
{
	GOOGLE_GRAPH *google_graph;
	char *visualization_function_name;
	char *id_string;
	char *loader_script_html;
	char *corechart_script_html;
	char *function_script_open;
	char *number_attribute_label;
	char *datatable_statement;
	char *options_JSON;
	char *instantiation_statement;
	char *draw_statement;
	char *function_script_close;
	char *set_callback_script_html;
	char *division_tag;

	if ( !application_name
	||   !data_directory
	||   !date_attribute_name
	||   !number_attribute_name
	||   !list_length( query_fetch_row_list )
	||   !chart_title
	||   !chart_filename_key
	||   !chart_number
	||   !google_graph_function )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	google_graph = google_graph_calloc();

	google_graph->google_filename =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		google_filename_new(
			application_name,
			session_key,
			data_directory,
			(char *)0 /* number_attribute_name */,
			chart_filename_key );

	visualization_function_name =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		google_graph_visualization_function_name(
			chart_number );

	id_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		google_graph_id_string(
			chart_number );

	loader_script_html =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		google_graph_loader_script_html();

	corechart_script_html =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		google_graph_corechart_script_html();

	function_script_open =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		google_graph_function_script_open(
			visualization_function_name );

	number_attribute_label =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		google_graph_number_attribute_label(
			number_attribute_name );

	datatable_statement =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		google_graph_datatable_statement(
			number_attribute_label );

	options_JSON =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		google_graph_options_JSON(
			chart_title );

	instantiation_statement =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		google_graph_instantiation_statement(
			id_string,
			google_graph_function );

	draw_statement =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		google_graph_draw_statement();

	google_graph->google_graph_point_list =
		google_graph_point_list(
			date_attribute_name,
			time_attribute_name,
			number_attribute_name,
			query_fetch_row_list );

	function_script_close =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		google_graph_function_script_close();

	set_callback_script_html =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		google_graph_set_callback_script_html(
			visualization_function_name );

	division_tag =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		google_graph_division_tag(
			id_string );

	google_graph->document_open_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		google_graph_document_open_html(
			loader_script_html,
			set_callback_script_html,
			corechart_script_html,
			function_script_open,
			datatable_statement );

	google_graph->document_close_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		google_graph_document_close_html(
			options_JSON,
			instantiation_statement,
			draw_statement,
			function_script_close,
			set_callback_script_html,
			division_tag );

	return google_graph;
}

char *google_graph_corechart_script_html( void )
{
	return
"<script type=\"text/javascript\">"
"google.charts.load('current',"
"{packages: ['corechart']});</script>";
}

GOOGLE_GRAPH_VALUE *google_graph_value_new( char *value_string )
{
	GOOGLE_GRAPH_VALUE *google_graph_value = google_graph_value_calloc();

	google_graph_value->null_boolean =
		google_graph_value_null_boolean(
			value_string );

	google_graph_value->display =
		/* -------------------------------------- */
		/* Returns value_string or program memory */
		/* -------------------------------------- */
		google_graph_value_display(
			value_string,
			google_graph_value->null_boolean );

	return google_graph_value;
}

GOOGLE_GRAPH_VALUE *google_graph_value_calloc( void )
{
	GOOGLE_GRAPH_VALUE *google_graph_value;

	if ( ! ( google_graph_value =
			calloc( 1, sizeof ( GOOGLE_GRAPH_VALUE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return google_graph_value;
}

boolean google_graph_value_null_boolean( char *value_string )
{
	return
	(  !value_string
	|| !*value_string
	|| strcmp( value_string, "null" ) == 0 );
}

char *google_graph_value_display(
		char *value_string,
		boolean null_boolean )
{
	if ( null_boolean )
		return "undefined";
	else
		return value_string;
}

char *google_graph_id_string( int chart_number )
{
	static char string[ 16 ];

	sprintf( string, "chart_id%d", chart_number );

	return string;
}

char *google_graph_loader_script_html( void )
{
	return
"<script type=\"text/javascript\""
" src=\"https://www.gstatic.com/charts/loader.js\">"
"</script>";
}

char *google_graph_point_date_javascript(
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
	static char javascript[ 64 ];

	if ( !yyyy_mm_dd
	||   string_character_count( '-', yyyy_mm_dd ) != 2 )
	{
		char message[ 128 ];

		sprintf(message, "yyyy_mm_dd is empty or invalid." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( string_character_count( ' ', yyyy_mm_dd ) == 1 )
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
	if ( string_character_count( ':', yyyy_mm_dd ) == 1 )
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
		date_time_parse(
			&hours,
			&minutes,
			hhmm );
	}

	sprintf(javascript,
		"Date(%s,%d,%s,%d,%d,0)",
		year,
		GOOGLE_GRAPH_CONVERT_MONTH_EXPRESSION,
		day,
		hours,
		minutes );

	return javascript;
}

char *google_graph_point_hhmm( QUERY_CELL *time_query_cell )
{
	char *hhmm = {0};

	if ( time_query_cell )
	{
		hhmm = time_query_cell->select_datum;
		if ( strcmp( hhmm, "null" ) == 0 ) hhmm = (char *)0;
	}

	return hhmm;
}

char *google_graph_visualization_function_name( int chart_number )
{
	static char visualization_function_name[ 32 ];

	sprintf(visualization_function_name,
		"drawVisualization%d",
		chart_number );

	return visualization_function_name;
}

GOOGLE_GRAPH_POINT *google_graph_point_new(
		char *javascript )
{
	GOOGLE_GRAPH_POINT *google_graph_point;

	if ( !javascript )
	{
		char message[ 128 ];

		sprintf(message, "javascript is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	google_graph_point = google_graph_point_calloc();

	google_graph_point->javascript = javascript;

	return google_graph_point;
}

GOOGLE_GRAPH_POINT *google_graph_point_calloc( void )
{
	GOOGLE_GRAPH_POINT *google_graph_point;

	if ( ! ( google_graph_point =
			calloc( 1,
				sizeof ( GOOGLE_GRAPH_POINT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return google_graph_point;
}

char *google_graph_point_javascript(
		QUERY_CELL *date_query_cell,
		QUERY_CELL *time_query_cell,
		QUERY_CELL *number_query_cell )
{
	char javascript[ 128 ];
	char *hhmm;
	char *date_javascript;
	GOOGLE_GRAPH_VALUE *google_graph_value;

	if ( !date_query_cell
	||   !number_query_cell )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	hhmm =
		/* -------------------------------------------- */
		/* Returns component of time_query_cell or null */
		/* -------------------------------------------- */
		google_graph_point_hhmm(
			time_query_cell );

	date_javascript =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		google_graph_point_date_javascript(
			date_query_cell->select_datum,
			hhmm );

	google_graph_value =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		google_graph_value_new(
			number_query_cell->select_datum );

	sprintf(javascript,
		"\t\t[new %s,%s]",
		date_javascript,
		google_graph_value->display );

	return strdup( javascript );
}

char *google_graph_set_callback_script_html(
		char *visualization_function_name )
{
	static char callback_script_html[ 256 ];

	if ( !visualization_function_name )
	{
		char message[ 128 ];

		sprintf(message, "visualization_function_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(callback_script_html,
		"<script type=\"text/javascript\">\n"
		"google.setOnLoadCallback(%s);\n"
		"</script>",
		visualization_function_name );

	return callback_script_html;
}

char *google_graph_options_JSON( char *chart_title )
{
	static char options_JSON[ 256 ];

	if ( !chart_title )
	{
		char message[ 128 ];

		sprintf(message, "chart_title is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(options_JSON,
		"\tvar options = {\n"
		"\t\ttitle: \"%s\"};",
		chart_title );

	return options_JSON;
}

char *google_graph_instantiation_statement(
		char *google_graph_id_string,
		char *google_graph_function )
{
	static char instantiation_statement[ 128 ];

	sprintf(instantiation_statement,
		"\tvar chart = new google.visualization.%s(\n"
		"\t\tdocument.getElementById('%s'));",
		google_graph_function,
		google_graph_id_string );

	return instantiation_statement;
}

char *google_graph_draw_statement( void )
{
	return "\tchart.draw(data, options);";
}

char *google_graph_function_script_open( char *visualization_function_name )
{
	static char function_script_open[ 128 ];

	sprintf(function_script_open,
		"<script type=\"text/javascript\">\n"
		"function %s()\n"
		"{",
		visualization_function_name );

	return function_script_open;
}

char *google_graph_function_script_close( void )
{
	return "}\n</script>";
}

char *google_graph_division_tag( char *google_graph_id_string )
{
	static char division_tag[ 1024 ];

	if ( !google_graph_id_string )
	{
		char message[ 128 ];

		sprintf(message, "google_graph_id_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(division_tag,
		"<div id=\"%s\" style=\"\n"
		"\tposition: absolute;\n"
		"\tleft: 10px;\n"
		"\ttop: 10px;\n"
		"\twidth: 900px;\n"
		"\theight: 600px;\n"
		"\tborder-width: thin;\n"
		"\tborder-style: solid;\n"
		"\tborder-color: teal\"/>",
		google_graph_id_string );

	return division_tag;
}

LIST *google_graph_point_list(
		char *date_attribute_name,
		char *time_attribute_name,
		char *number_attribute_name,
		LIST *query_fetch_row_list )
{
	LIST *point_list;
	QUERY_ROW *query_row;
	QUERY_CELL *date_query_cell;
	QUERY_CELL *time_query_cell = {0};
	QUERY_CELL *number_query_cell;
	char *javascript;
	GOOGLE_GRAPH_POINT *google_graph_point;

	if ( !date_attribute_name
	||   !number_attribute_name
	||   !list_rewind( query_fetch_row_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	point_list = list_new();

	do {
		query_row =
			list_get(
				query_fetch_row_list );

		if ( ! ( date_query_cell =
				query_cell_seek(
					date_attribute_name,
					query_row->cell_list ) ) )
		{
			char message[ 128 ];

			sprintf(message,
				"query_cell_seek(%s) returned empty.",
				date_attribute_name );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( time_attribute_name )
		{
			if ( ! ( time_query_cell =
					query_cell_seek(
						time_attribute_name,
						query_row->cell_list ) ) )
			{
				char message[ 128 ];

				sprintf(message,
					"query_cell_seek(%s) returned empty.",
					time_attribute_name );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}
		}

		if ( ! ( number_query_cell =
				query_cell_seek(
					number_attribute_name,
					query_row->cell_list ) ) )
		{
			char message[ 128 ];

			sprintf(message,
				"query_cell_seek(%s) returned empty.",
				number_attribute_name );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		javascript =
			/* Returns heap memory */
			/* ------------------- */
			google_graph_point_javascript(
				date_query_cell,
				time_query_cell,
				number_query_cell );

		google_graph_point =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			google_graph_point_new(
				javascript );

		list_set(
			point_list,
			google_graph_point );

	} while ( list_next( query_fetch_row_list ) );

	return point_list;
}

char *google_graph_number_attribute_label( char *number_attribute_name )
{
	static char number_attribute_label[ 128 ];

	if ( !number_attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "number_attribute_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	string_initial_capital(
		number_attribute_label,
		number_attribute_name );

	return number_attribute_label;
}

char *google_graph_datatable_statement( char *number_attribute_label )
{
	static char datatable_statement[ 256 ];

	if ( !number_attribute_label )
	{
		char message[ 128 ];

		sprintf(message, "number_attribute_label is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(datatable_statement,
"\tvar data = new google.visualization.arrayToDataTable([\n"
"\t\t['','%s'],",
		number_attribute_label );

	return datatable_statement;
}

char *google_graph_script_function_close( void )
{
	return "}\n</script>";
}

char *google_graph_document_open_html(
		char *loader_script_html,
		char *set_callback_script_html,
		char *corechart_script_html,
		char *function_script_open,
		char *datatable_statement )
{
	char html[ 4096 ];
	char *ptr = html;

	if ( !loader_script_html
	||   !set_callback_script_html
	||   !corechart_script_html
	||   !function_script_open
	||   !datatable_statement )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr += sprintf(
		ptr,
		"%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		document_html(
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_type_html(),
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_standard_string() ) );

	ptr += sprintf(
		ptr,
		"%s\n",
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		document_head_open_tag() );

	ptr += sprintf(
		ptr,
		"%s\n",
		loader_script_html );

	ptr += sprintf(
		ptr,
		"%s\n",
		corechart_script_html );

	ptr += sprintf(
		ptr,
		"%s\n",
		function_script_open );

	ptr += sprintf(
		ptr,
		"%s",
		datatable_statement );

	return strdup( html );
}

char *google_graph_document_close_html(
		char *options_JSON,
		char *instantiation_statement,
		char *draw_statement,
		char *function_script_close,
		char *set_callback_script_html,
		char *division_tag )
{
	char document_close_html[ 4096 ];

	if ( !options_JSON
	||   !instantiation_statement
	||   !draw_statement
	||   !function_script_close
	||   !set_callback_script_html
	||   !division_tag )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(document_close_html,
		"%s\n"
		"%s\n"
		"%s\n"
		"%s\n"
		"%s\n"
		"</head>\n"
		"<body>\n"
		"%s\n"
		"</body>\n"
		"</html>",
		options_JSON,
		instantiation_statement,
		draw_statement,
		function_script_close,
		set_callback_script_html,
		division_tag );

	return strdup( document_close_html );
}

GOOGLE_GRAPH *google_graph_calloc( void )
{
	GOOGLE_GRAPH *google_graph;

	if ( ! ( google_graph = calloc( 1, sizeof ( GOOGLE_GRAPH ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return google_graph;
}

void google_graph_output(
		GOOGLE_GRAPH *google_graph,
		FILE *output_file )
{
	GOOGLE_GRAPH_POINT *google_graph_point;

	if ( !google_graph
	||   !output_file )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	fprintf(output_file,
		"%s\n",
		google_graph->document_open_html );

	if ( list_rewind( google_graph->google_graph_point_list ) )
	do {
		google_graph_point =
			list_get(
				google_graph->google_graph_point_list );

		fprintf(output_file,
			"%s",
			google_graph_point->javascript );

		if ( list_last_boolean(
			google_graph->
				google_graph_point_list ) )
		{
			fprintf( output_file, "]);" );
		}
		else
		{
			fprintf( output_file, "," );
		}

		fprintf( output_file, "\n" );

	} while ( list_next( google_graph->google_graph_point_list ) );

	fprintf(output_file,
		"%s\n",
		google_graph->document_close_html );

}
