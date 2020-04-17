/* ------------------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_hydrology/output_current_vs_historical.c      	*/
/* ------------------------------------------------------------- 	*/
/* Freely available software: see Appaserver.org			*/
/* ------------------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "timlib.h"
#include "environ.h"
#include "element.h"
#include "document.h"
#include "datatype.h"
#include "application.h"
#include "google_map_station.h"
#include "google_chart.h"
#include "process.h"
#include "date.h"
#include "current_vs_historical.h"
#include "appaserver_link_file.h"

/* Constants */
/* --------- */
#define STRATUM_DATATYPE_CURRENT		"current"
#define STRATUM_DATATYPE_HISTORICAL		"historical"
#define LOCAL_CHART_POSITION_TOP		50
#define LOCAL_CHART_POSITION_LEFT		265
#define LOCAL_CHART_WIDTH			800
#define LOCAL_CHART_HEIGHT			500

/* Prototypes */
/* ---------- */
LIST *get_historical_long_term_datatype_name_display_list(
				LIST *datatype_name_list );

boolean populate_point_array_historical_fetch(
				LIST *barchart_list,
				LIST *datatype_name_list,
				char *sys_string,
				char *stratum_datatype_name );

void output_historical_current(
				FILE *output_file,
				LIST *station_name_list,
				char *datatype_name,
				boolean bar_chart,
				char *application_name );

void remove_exclude_datatype_name_list(
				LIST *datatype_list,
				char *exclude_datatype_name_list_string );

void populate_point_array_current_sys_string(
				char *sys_string,
				LIST *station_name_list,
				char *datatype_name,
				boolean bar_chart,
				char *application_name );

boolean populate_point_array_historical_current(
				LIST *timeline_list,
				LIST *datatype_name_list,
				LIST *station_name_list,
				char *datatype_name,
				boolean bar_chart,
				char *application_name );

boolean populate_point_array_current(
				LIST *timeline_list,
				LIST *datatype_name_list,
				LIST *station_name_list,
				char *datatype_name,
				boolean bar_chart,
				char *application_name );

void populate_point_array_historical_long_term_sys_string(
				char *sys_string,
				LIST *station_name_list,
				char *aggregation_function,
				char *application_name,
				char *datatype_name,
				char *stratum_datatype_name );

boolean populate_point_array_historical(
				LIST *barchart_list,
				LIST *month_name_list,
				LIST *station_name_list,
				char *application_name,
				char *aggregation_function,
				char *datatype_name );

GOOGLE_OUTPUT_CHART *get_google_historical_long_term_chart(
				LIST *station_name_list,
				char *datatype_name,
				boolean bar_chart,
				char *application_name );

GOOGLE_OUTPUT_CHART *get_google_historical_current_chart(
				LIST *station_name_list,
				char *datatype_name,
				boolean bar_chart,
				char *application_name );

GOOGLE_OUTPUT_CHART *get_google_current_chart(
				char *application_name,
				LIST *station_name_list,
				char *datatype_name );

void remove_suffix_2(		LIST *datatype_list );

void move_datatype_to_top(	LIST *datatype_list,
				char *datatype_name );

char *get_post_action_string(
				char *application_name,
				char *login_name,
				char *session,
				char *process_name,
				enum state,
				LIST *station_name_list,
				char *datatype_name,
				char *station_type );

void output_current(
				FILE *output_file,
				char *application_name,
				LIST *station_name_list,
				char *datatype_name );

void output_historical(
				FILE *output_file,
				char *application_name,
				LIST *station_name_list,
				char *datatype_name );

boolean output_historical_long_term(
				FILE *output_file,
				LIST *station_name_list,
				char *datatype_name,
				boolean bar_chart,
				char *units,
				char *application_name );

boolean output_datatype(	char **datatype_name,
				FILE *output_file,
				char *application_name,
				char *login_name,
				char *session,
				char *process_name,
				enum state,
				LIST *station_name_list );

void output_menu(		FILE *output_file,
				char *application_name,
				char *login_name,
				char *session,
				char *process_name,
				enum state,
				LIST *station_name_list,
				char *datatype_name );

void output_map(		FILE *output_file,
				char *application_name,
				char *login_name,
				char *station_type,
				char *session,
				char *process_name );

void output_current_vs_historical(
				FILE *output_file,
				char *application_name,
				char *login_name,
				char *session,
				char *process_name,
				enum state,
				LIST *station_name_list,
				char *datatype_name,
				char *station_type );

int main( int argc, char **argv )
{
	char *application_name;
	char *login_name;
	char *session;
	char *process_name;
	enum state state;
	char buffer[ 256 ];
	char *output_filename;
	char *http_filename;
	FILE *output_file;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	LIST *station_name_list;
	char *datatype_name;
	char *station_type = {0};
	DOCUMENT *document;
	char *por_historical_begin_date;
	char *por_historical_end_date;
	char *por_current_begin_date;
	char *por_current_end_date;
	char *current_begin_date;
	char *current_end_date;

	application_name = argv[ 1 ];

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc < 8 )
	{
		fprintf( stderr,
"Usage: %s application login_name session process state station_name_list datatype [station_type]\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 2 ];
	session = argv[ 3 ];
	process_name = argv[ 4 ];
	state = current_vs_historical_get_state( argv[ 5 ] );
	station_name_list = list_string2list( argv[ 6 ], ',' );
	datatype_name = argv[ 7 ];

	if ( argc == 9
	&&   *argv[ 8 ]
	&&   strcmp( argv[ 8 ], "station_type" ) != 0 )
	{
		station_type = argv[ 8 ];
	}

	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( state == initial )
	{
		APPASERVER_LINK_FILE *appaserver_link_file;

		appaserver_link_file =
			appaserver_link_file_new(
				application_get_http_prefix( application_name ),
				appaserver_library_get_server_address(),
				( application_get_prepend_http_protocol_yn(
					application_name ) == 'y' ),
	 			appaserver_parameter_file->
					document_root,
				process_name /* filename_stem */,
				application_name,
				0 /* process_id */,
				session,
				"html" );

		output_filename =
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

		http_filename =
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

		if ( ! ( output_file = fopen( output_filename, "w" ) ) )
		{
			fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s for write.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 output_filename );
			exit( 1 );
		}
	}
	else
	if ( !list_length( station_name_list ) )
	{
		document_output_content_type();
		state = map;
		output_file = stdout;
	}
	else
	{
		document = document_new(
				(char *)0 /* insert_update_key */,
				application_name );

		document_output_head(
			document->application_name,
			format_initial_capital(
				buffer,
				process_name ) /* title */,
			1 /* output_content_type */,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

		document_output_body(
			document->application_name,
			ADDITIONAL_ONLOAD_CONTROL_STRING );

		output_file = stdout;
	}

	output_current_vs_historical(
				output_file,
				application_name,
				login_name,
				session,
				process_name,
				state,
				station_name_list,
				datatype_name,
				station_type );

	if ( state == initial )
	{
		fprintf( output_file,
"</body>\n"
"</html>\n" );

		fclose( output_file );

		current_vs_historical_get_dates(
			&por_historical_begin_date,
			&por_historical_end_date,
			&por_current_begin_date,
			&por_current_end_date,
			&current_begin_date,
			&current_end_date,
			application_name );

		document_output_html_stream( stdout );

		printf(
"<head>\n"
"<link rel=stylesheet type=text/css href=/appaserver/%s/style.css>\n"
"</head>\n"
"<body>\n"
"<h1>%s</h1>\n"
"<table>\n"
"<tr><td>Historical Period-of-record begin date (red bar):<td>%s\n"
"<tr><td>Historical Period-of-record end date (red bar):<td>%s\n"
"<tr><td>Current Period-of-record begin date (blue bar):<td>%s\n"
"<tr><td>Current Period-of-record end date (blue bar):<td>%s\n"
"<tr><td>Current begin date:<td>%s\n"
"<tr><td>Current end date:<td>%s\n"
"</table>\n"
"<p><a href=%s target=_new>Click here</a> to view the screen.\n"
"</body>\n"
"</html>\n",
			application_name,
			format_initial_capital(
				buffer,
				process_name ),
			por_historical_begin_date,
			por_historical_end_date,
			por_current_begin_date,
			por_current_end_date,
			current_begin_date,
			current_end_date,
			http_filename );

		process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	}
	else
	{
		document_close();
	}

	return 0;

} /* main() */

void output_current_vs_historical(
				FILE *output_file,
				char *application_name,
				char *login_name,
				char *session,
				char *process_name,
				enum state state,
				LIST *station_name_list,
				char *datatype_name,
				char *station_type )
{
	if ( state == initial || state == map )
	{
		output_map(	output_file,
				application_name,
				login_name,
				station_type,
				session,
				process_name );

		output_menu(	output_file,
				application_name,
				login_name,
				session,
				process_name,
				state,
				station_name_list,
				datatype_name );
	}
	else
	if ( state == post_map )
	{
		state = current;

		if ( !output_datatype(
				&datatype_name,
				output_file,
				application_name,
				login_name,
				session,
				process_name,
				state,
				station_name_list )
		|| ( !datatype_name ) )
		{
			fprintf( output_file,
"<div style=\"position: absolute; left: %dpx; top: 50px\"><h3>No reportable datatypes found.</h3></div>\n",
				 MESSAGE_LEFT_POSITION );
		}
		else
		{
			output_current(
				output_file,
				application_name,
				station_name_list,
				datatype_name );
		}

		output_menu(	output_file,
				application_name,
				login_name,
				session,
				process_name,
				state,
				station_name_list,
				datatype_name );
	}
	else
	if ( state == historical )
	{
		output_datatype(
				&datatype_name,
				output_file,
				application_name,
				login_name,
				session,
				process_name,
				state,
				station_name_list );

		output_historical(
				output_file,
				application_name,
				station_name_list,
				datatype_name );

		output_menu(	output_file,
				application_name,
				login_name,
				session,
				process_name,
				state,
				station_name_list,
				datatype_name );
	}
	else
	if ( state == current )
	{
		output_datatype(
				&datatype_name,
				output_file,
				application_name,
				login_name,
				session,
				process_name,
				state,
				station_name_list );

		output_current(
				output_file,
				application_name,
				station_name_list,
				datatype_name );

		output_menu(	output_file,
				application_name,
				login_name,
				session,
				process_name,
				state,
				station_name_list,
				datatype_name );
	}

} /* output_current_vs_historical() */

void output_menu(		FILE *output_file,
				char *application_name,
				char *login_name,
				char *session,
				char *process_name,
				enum state state,
				LIST *station_name_list,
				char *datatype_name )
{
	char menu_string[ 65536 ];
	LIST *station_type_list = {0};

	if ( state == initial || state == map )
	{

		sprintf( menu_string,
"<li><a href=%s><span class=menu>Map</span></a>\n"
"<li><a><span class=menu>Station Type</span></a>\n",
		get_post_action_string(
			application_name,
			login_name,
			session,
			process_name,
			map,
			(LIST *)0 /* station_name_list */,
			(char *)0 /* datatype_name */,
			(char *)0 /* station_type */ ) );

		station_type_list =
			station_get_station_type_list(
				application_name,
				login_name );
	}
	else
	if ( state == historical )
	{
		sprintf( menu_string,
"<li><a href=%s><span class=menu>Map</span></a>\n"
"<li><a href=%s><span class=menu>Current</span></a>\n",
		get_post_action_string(
			application_name,
			login_name,
			session,
			process_name,
			map,
			(LIST *)0 /* station_name_list */,
			(char *)0 /* datatype_name */,
			(char *)0 /* station_type */ ),
		get_post_action_string(
			application_name,
			login_name,
			session,
			process_name,
			current,
			station_name_list,
			datatype_name,
			(char *)0 /* station_type */ ) );
	}
	else
	if ( state == current )
	{
		sprintf( menu_string,
"<li><a href=%s><span class=menu>Map</span></a>\n"
"<li><a href=%s><span class=menu>Historical</span></a>\n",
		get_post_action_string(
			application_name,
			login_name,
			session,
			process_name,
			map,
			(LIST *)0 /* station_name_list */,
			(char *)0 /* datatype_name */,
			(char *)0 /* station_type */),
		get_post_action_string(
			application_name,
			login_name,
			session,
			process_name,
			historical,
			station_name_list,
			datatype_name,
			(char *)0 /* station_type */ ) );
	}

	if ( list_rewind( station_type_list ) )
	{
		char buffer[ 128 ];
		char *station_type;
		char *ptr = menu_string + strlen( menu_string );

		ptr += sprintf( ptr, "<ul>\n" );

		do {
			station_type = list_get( station_type_list );

			ptr += sprintf( ptr,
"<li><a href=%s><span class=menu>%s</span></a>\n",
				get_post_action_string(
					application_name,
					login_name,
					session,
					process_name,
					map,
					(LIST *)0 /* station_name_list */,
					(char *)0 /* datatype_name */,
					station_type ),
				format_initial_capital(
					buffer,
					station_type ) );

		} while( list_next( station_type_list ) );

		ptr += sprintf( ptr,
"<li><a href=%s><span class=menu>%s</span></a>\n",
				get_post_action_string(
					application_name,
					login_name,
					session,
					process_name,
					map,
					(LIST *)0 /* station_name_list */,
					(char *)0 /* datatype_name */,
					NULL_OPERATOR ),
				format_initial_capital(
					buffer,
					NULL_OPERATOR ) );

		ptr += sprintf( ptr, "</ul>\n" );

	} /* if ( list_rewind( station_type_list ) ) */

	fprintf( output_file,
"<script type=text/javascript>//<![CDATA[\n"
"document.writeln(\"<style type='text/css'>#menu { display: none; }</style>\");\n"
"//]]></script>\n"
"<div class=dynarch_menu_class>\n"
"<ul id=menu>\n"
"%s\n"
"</ul>\n"
"</div>\n",
		 menu_string );

	document_output_dynarch_heading( output_file );

} /* output_menu() */

void output_map(		FILE *output_file,
				char *application_name,
				char *login_name,
				char *station_type,
				char *session,
				char *process_name )
{
	GOOGLE_MAP_STATION *google_map_station;
	char *google_map_key_data;
	STATION *station;
	char title[ 128 ];

	format_initial_capital( title, process_name );

	google_map_key_data =
		google_map_station_get_map_key(
			application_name );

	google_map_station_output_heading(
				output_file,
				title,
				google_map_key_data,
				"station",
				"populate_station_name_list( station )",
				application_name,
				MAP_POSITION_TOP,
				MAP_POSITION_LEFT );

	google_map_station =
		google_map_station_new(
			application_name,
			login_name,
			station_type );

	if ( list_rewind( google_map_station->station_list ) )
	{
		do {
			station = list_get_pointer(
					google_map_station->station_list );

			if ( station->long_nad83 && station->lat_nad83 )
			{
				google_map_station_output_selectable_station(
						output_file,
						station->station_name,
						station->long_nad83,
						station->lat_nad83 );
			}
		} while( list_next( google_map_station->station_list ) );
	}

	google_map_station_output_head_close( output_file );

	google_map_station_output_body(
				output_file,
				0 /* not with_table */,
				ADDITIONAL_ONLOAD_CONTROL_STRING );

	fprintf( output_file,
"<script type=text/javascript>\n"
"function populate_station_name_list( station_name )\n"
"{\n"
"	var element_name;\n"
"	var element;\n"
"	var process_element;\n"
"	var form;\n"
"\n"
"	process_element = false;\n"
"	form = document.forms[ 0 ];\n"
"\n"
"	for( var i = 0; i < form.elements.length; i++ )\n"
"	{\n"
"		element = form.elements[ i ];\n"
"		element_name = element.name;\n"
"		if ( element_name == \"%s_1\" )\n"
"		{\n"
"			process_element = element;\n"
"			break;\n"
"		}\n"
"	}\n"
"	if ( process_element == false ) return false;\n"
"\n"
"	for(	var j = 0;\n"
"		j < process_element.options.length;\n"
"		j++ )\n"
"	{\n"
"		if ( process_element.options[ j ].text == station_name )\n"
"		{\n"
"			process_element.options[ j ] =\n"
"				new Option(\n"
"					\"\",\n"
"					\"\",\n"
"					0,\n"
"					1 /* selected */ );\n"
"			return true;\n"
"		}\n"
"	}\n"
"\n"
"	for(	var j = 0;\n"
"		j < process_element.options.length;\n"
"		j++ )\n"
"	{\n"
"		if ( process_element.options[ j ].text == \"\" )\n"
"		{\n"
"			break;\n"
"		}\n"
"	}\n"
"\n"
"	process_element.options[ j ] =\n"
"		new Option(\n"
"			station_name,\n"
"			station_name,\n"
"			0,\n"
"			1 /* selected */ );\n"
"	return true;\n"
"\n"
"}\n"
"\n"
"</script>\n"
"<div class=station_list_class>\n"
"<form enctype=\"multipart/form-data\" method=post\n"
"	action=%s>\n"
"<table>\n"
"<tr><td valign=top><select name=%s_1 size=10 multiple></select>\n"
"<tr>\n"
"<td valign=top><input type=submit value=Submit>\n"
"</table>\n"
"</form>\n"
"</div>\n",
		CURRENT_VS_HISTORICAL_STATION_PREFIX,
		get_post_action_string(
			application_name,
			login_name,
			session,
			process_name,
			post_map,
			(LIST *)0 /* station_name_list */,
			(char *)0 /* datatype_name */,
			(char *)0 /* station_type */ ),
		CURRENT_VS_HISTORICAL_STATION_PREFIX );

} /* output_map() */

boolean output_datatype(char **datatype_name,
			FILE *output_file,
			char *application_name,
			char *login_name,
			char *session,
			char *process_name,
			enum state state,
			LIST *station_name_list )
{
	LIST *datatype_list;
	DATATYPE *datatype;
	char datatype_name_buffer[ 128 ];

	datatype_list =
		datatype_with_station_name_list_get_datatype_bar_graph_list(
			application_name,
			station_name_list );

	remove_exclude_datatype_name_list(
			datatype_list,
			EXCLUDE_DATATYPE_NAME_LIST_STRING );

	if ( !list_length( datatype_list ) ) return 0;

	remove_suffix_2( datatype_list );

	move_datatype_to_top(	datatype_list,
				"stage" );

	move_datatype_to_top(	datatype_list,
				"rain" );

	fprintf( output_file,
"<div class=historical_heading>%s</div>\n"
"<div class=historical_radio_button>\n"
"<form enctype=\"multipart/form-data\" method=post\n"
"	action=%s>\n",
		list_display( station_name_list ),
		get_post_action_string(
			application_name,
			login_name,
			session,
			process_name,
			state,
			(LIST *)0 /* station_name_list */,
			(char *)0 /* datatype_name */,
			(char *)0 /* station_type */ ) );

	if ( !list_rewind( datatype_list ) )
	{
		fprintf( output_file,
"</form>\n"
"</div>\n" );
		return 0;
	}

	do {
		datatype = list_get_pointer( datatype_list );

		fprintf( output_file,
"<input type=radio name=%s value='%s' onClick=forms[0].submit()",
			DATATYPE_RADIO_BUTTON_IDENTIFIER,
			datatype->datatype_name );

		if ( timlib_strcmp(*datatype_name,
				   datatype->datatype_name ) == 0 )
		{
			fprintf( output_file, " checked" );
		}
		else
		if ( strlen( *datatype_name ) == 0 )
		{
			fprintf( output_file, " checked" );

			*datatype_name = datatype->datatype_name;

		}

		fprintf( output_file, ">" );

		fprintf(
				output_file,
				"%s<br>\n",
				format_initial_capital(
					datatype_name_buffer,
					datatype->datatype_name ) );

	} while( list_next( datatype_list ) );

	fprintf( output_file,
"<input name=%s_1 type=hidden value=%s>\n",
		CURRENT_VS_HISTORICAL_STATION_PREFIX,
		list_display_delimited( station_name_list, ',' ) );

	fprintf( output_file,
"</form>\n"
"</div>\n" );

	return 1;

} /* output_datatype() */

char *get_post_action_string(
				char *application_name,
				char *login_name,
				char *session,
				char *process_name,
				enum state state,
				LIST *station_name_list,
				char *datatype_name,
				char *station_type )
{
	char post_action_string[ 1024 ];

	sprintf( post_action_string,
"\"/cgi-bin/post_current_vs_historical?%s+%s+%s+%s+%s+%s+%s+%s\"",
		application_name,
		login_name,
		session,
		process_name,
		current_vs_historical_get_state_string( state ),
		list_display_delimited( station_name_list, ',' ),
		(datatype_name) ? datatype_name : "",
		(station_type) ? station_type : "" );

	return strdup( post_action_string );

} /* get_post_action_string() */

void remove_suffix_2( LIST *datatype_list )
{
	DATATYPE *datatype;

	if ( !list_rewind( datatype_list ) ) return;

	do {
		datatype = list_get_pointer( datatype_list );

		if ( character_exists( datatype->datatype_name, '2' ) )
		{
			list_delete_current( datatype_list );
		}

	} while( list_next( datatype_list ) );

} /* remove_suffix_2() */

void move_datatype_to_top(	LIST *datatype_list,
				char *datatype_name )
{
	DATATYPE *datatype;

	if ( !list_rewind( datatype_list ) ) return;

	do {
		datatype = list_get_pointer( datatype_list );

		if ( strcmp( datatype->datatype_name, datatype_name ) == 0 )
		{

			list_delete_current( datatype_list );

			list_insert_head(	datatype_list,
						datatype,
						sizeof( DATATYPE ) );
			return;
		}

	} while( list_next( datatype_list ) );

} /* move_datatype_to_top() */

void output_historical(		FILE *output_file,
				char *application_name,
				LIST *station_name_list,
				char *datatype_name )
{
	boolean bar_chart = 0;
	char *units;

	units = datatype_get_units_string(
			&bar_chart,
			application_name,
			datatype_name );

	if ( output_historical_long_term(
				output_file,
				station_name_list,
				datatype_name,
				bar_chart,
				units,
				application_name ) )
	{
/*
		output_historical_current(
				output_file,
				station_name_list,
				datatype_name,
				bar_chart,
				application_name );
*/
	}

} /* output_historical() */

boolean output_historical_long_term(
				FILE *output_file,
				LIST *station_name_list,
				char *datatype_name,
				boolean bar_chart,
				char *units,
				char *application_name )
{
	GOOGLE_OUTPUT_CHART *google_chart;
	char yaxis_label[ 128 ];
	LIST *datatype_name_display_list;

	if ( ! ( google_chart =
			get_google_historical_long_term_chart(
				station_name_list,
				datatype_name,
				bar_chart,
				application_name ) ) )
	{
		fprintf( output_file,
"<div style=\"position: absolute; left: %dpx; top: 50px\"><h3>No long term data selected.</h3></div>\n",
			 MESSAGE_LEFT_POSITION );
		return 0;
	}


	if ( bar_chart )
	{
		sprintf(	yaxis_label,
				"Monthly Average (Sum) %s (%s)",
				datatype_name,
				units );
	}
	else
	{
		sprintf(	yaxis_label,
				"Monthly Average %s (%s)",
				datatype_name,
				units );
	}

	datatype_name_display_list =
		get_historical_long_term_datatype_name_display_list(
			google_chart->datatype_name_list );

	format_initial_capital( yaxis_label, yaxis_label );

	google_chart_include( output_file );

	google_chart_output_visualization_non_annotated(
				output_file,
				google_chart->google_chart_type,
				google_chart->timeline_list,
				google_chart->barchart_list,
				datatype_name_display_list,
				"" /* title */,
				strdup( yaxis_label ),
				google_chart->width,
				google_chart->height,
				google_chart->background_color,
				google_chart->legend_position_bottom,
				1 /* chart_type_bar */,
				google_chart->google_package_name,
				0 /* not dont_display_range_selector */,
				daily /* aggregate_level */,
				google_chart->chart_number );

	google_chart_output_chart_instantiation(
		output_file,
		google_chart->chart_number );

	google_chart_anchor_chart(
				output_file,
				"" /* chart_title */,
				google_chart->google_package_name,
				google_chart->left,
				google_chart->top,
				google_chart->width,
				google_chart->height,
				google_chart->chart_number );

	return 1;

} /* output_historical_long_term() */

GOOGLE_OUTPUT_CHART *get_google_current_chart(
				char *application_name,
				LIST *station_name_list,
				char *datatype_name )
{
	GOOGLE_OUTPUT_CHART *google_chart;
	boolean bar_chart;

	google_chart =
		google_output_chart_new(
			LOCAL_CHART_POSITION_LEFT,
			LOCAL_CHART_POSITION_TOP,
			LOCAL_CHART_WIDTH,
			LOCAL_CHART_HEIGHT );

	bar_chart = datatype_bar_chart( application_name, datatype_name );

	list_append_pointer(	google_chart->datatype_name_list,
				datatype_name );

	if ( bar_chart )
	{
		list_append_pointer(	google_chart->datatype_name_list,
					DATATYPE_QUANTUM_TOTAL );
	}

	if ( !populate_point_array_current(
				google_chart->timeline_list,
				google_chart->datatype_name_list,
				station_name_list,
				datatype_name,
				bar_chart,
				application_name ) )
	{
		return (GOOGLE_OUTPUT_CHART *)0;;
	}

	return google_chart;

} /* get_google_current_chart() */

GOOGLE_OUTPUT_CHART *get_google_historical_long_term_chart(
				LIST *station_name_list,
				char *datatype_name,
				boolean bar_chart,
				char *application_name )

{
	GOOGLE_OUTPUT_CHART *google_chart;
	char *aggregation_function;

	google_chart =
		google_output_chart_new(
			LOCAL_CHART_POSITION_LEFT,
			LOCAL_CHART_POSITION_TOP,
			LOCAL_CHART_WIDTH,
			HISTORICAL_CHART_HEIGHT );

	google_chart->google_chart_type = google_column_chart;
	google_chart->google_package_name = GOOGLE_CORECHART;

	if ( bar_chart )
		aggregation_function = AGGREGATION_SUM;
	else
		aggregation_function = AGGREGATION_AVG;

	list_append_pointer(
		google_chart->datatype_name_list,
		STRATUM_DATATYPE_CURRENT );

	list_append_pointer(
		google_chart->datatype_name_list,
		STRATUM_DATATYPE_HISTORICAL );

/*
#define MONTH_LIST_STRING "jan,feb,mar,apr,may,jun,jul,aug,sep,oct,nov,dec"
*/
	google_barchart_append_datatype_name_string(
		google_chart->barchart_list,
		MONTH_LIST_STRING /* datatype_name_list_string */,
		list_length( google_chart->datatype_name_list ),
		',' /* delimiter */ );

	if ( !populate_point_array_historical(
			google_chart->barchart_list,
			google_chart->datatype_name_list
				/* month_name_list */ ,
			station_name_list,
			application_name,
			aggregation_function,
			datatype_name ) )
	{
		return (GOOGLE_OUTPUT_CHART *)0;;
	}

	return google_chart;

} /* get_google_historical_long_term_chart() */

boolean populate_point_array_historical(
				LIST *barchart_list,
				LIST *month_name_list,
				LIST *station_name_list,
				char *application_name,
				char *aggregation_function,
				char *datatype_name )
{
	char sys_string[ 2048 ];

	populate_point_array_historical_long_term_sys_string(
		sys_string,
		station_name_list,
		aggregation_function,
		application_name,
		datatype_name,
		STRATUM_DATATYPE_CURRENT );

	populate_point_array_historical_fetch(
		barchart_list,
		month_name_list /* datatype_name_list */,
		sys_string,
		STRATUM_DATATYPE_CURRENT );

	populate_point_array_historical_long_term_sys_string(
		sys_string,
		station_name_list,
		aggregation_function,
		application_name,
		datatype_name,
		STRATUM_DATATYPE_HISTORICAL );

	return populate_point_array_historical_fetch(
		barchart_list,
		month_name_list /* datatype_name_list */,
		sys_string,
		STRATUM_DATATYPE_HISTORICAL );

} /* populate_point_array_historical() */

boolean populate_point_array_historical_fetch(
				LIST *barchart_list,
				LIST *datatype_name_list,
				char *sys_string,
				char *stratum_datatype_name )
{
	char input_buffer[ 1024 ];
	FILE *input_pipe;
	char month[ 16 ];
	char value_string[ 16 ];
	boolean got_one = 0;

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		got_one = 1;

		piece(	month,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	value_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		google_barchart_set_point(
			barchart_list,
			month /* stratum_name */,
			datatype_name_list,
			stratum_datatype_name,
			atof( value_string ) );
	}

	pclose( input_pipe );

	return got_one;

} /* populate_point_array_historical_fetch() */

void populate_point_array_historical_long_term_sys_string(
				char *sys_string,
				LIST *station_name_list,
				char *aggregation_function,
				char *application_name,
				char *datatype_name,
				char *stratum_datatype_name )
{
	char select_clause[ 1024 ];
	char *group_clause;
	char *station_in_clause;
	char where_clause[ 1024 ];
	char where_date_clause[ 1024 ];
	char *por_historical_begin_date;
	char *por_historical_end_date;
	char *por_current_begin_date;
	char *por_current_end_date;
	char *current_begin_date;
	char *current_end_date;

	current_vs_historical_get_dates(
			&por_historical_begin_date,
			&por_historical_end_date,
			&por_current_begin_date,
			&por_current_end_date,
			&current_begin_date,
			&current_end_date,
			application_name );

	station_in_clause =
		timlib_with_list_get_in_clause(
			station_name_list );

	group_clause =
	"concat( station, date_format( measurement_date, '%Y-%b' ) )";

	sprintf(select_clause,
		"%s,				"
		"'%s',				"
		"%s( measurement_value )	",
		group_clause,
		stratum_datatype_name,
		aggregation_function );

	if ( strcmp( stratum_datatype_name, STRATUM_DATATYPE_CURRENT ) == 0 )
	{
		sprintf( where_date_clause,
		 	"measurement_date >= '%s' and		"
			"measurement_date <= '%s'		",
			por_current_begin_date,
			por_current_end_date );
	}
	else
	{
		sprintf(where_date_clause,
		 	"measurement_date >= '%s' and		"
			"measurement_date <= '%s'		",
			por_historical_begin_date,
			por_historical_end_date );
	}

	sprintf( where_clause,
		 "station in (%s) and			"
		 "datatype = '%s' and			"
		 "%s					",
		 station_in_clause,
		 datatype_name,
		 where_date_clause );

	sprintf(sys_string,
"echo \"	select %s				 "
"		from measurement			 "
"		where %s				 "
"		group by %s;\"				|"
"sql_quick.e '|'					|"
"piece_inverse.e 0 '-'					|"
"grep -v '|$'						|"
"sed 's/|/^/1'						|"
"sort							|"
"statistics_on_group.e Month				|"
"egrep 'Average|Month'					|"
"piece.e ':' 1						|"
"joinlines.e '%c' 2					 ",
		 select_clause,
		 where_clause,
		 group_clause,
		 FOLDER_DATA_DELIMITER );

} /* populate_point_array_historical_long_term_sys_string() */

void remove_exclude_datatype_name_list(
			LIST *datatype_list,
			char *exclude_datatype_name_list_string )
{
	LIST *exclude_datatype_name_list;
	char *datatype_name;
	DATATYPE *datatype;

	exclude_datatype_name_list =
		list_string2list( 
			exclude_datatype_name_list_string,
			',' );

	if ( !list_rewind( exclude_datatype_name_list ) ) return;

	do {
		datatype_name = list_get_pointer( exclude_datatype_name_list );

		if ( !list_rewind( datatype_list ) ) return;

		do {
			datatype = list_get_pointer( datatype_list );

			if ( timlib_exists_string(
					datatype->datatype_name,
					datatype_name ) )
			{
				list_delete_current( datatype_list );
			}

		} while( list_next( datatype_list ) );

	} while( list_next( exclude_datatype_name_list ) );

} /* remove_exclude_datatype_name_list() */

void output_current(	FILE *output_file,
			char *application_name,
			LIST *station_name_list,
			char *datatype_name )
{
	GOOGLE_OUTPUT_CHART *google_chart;
	char yaxis_label[ 128 ];

	if ( ! ( google_chart =
			get_google_current_chart(
				application_name,
				station_name_list,
				datatype_name ) ) )
	{
		fprintf( output_file,
"<div style=\"position: absolute; left: %dpx; top: 50px\"><h3>No current data selected.</h3></div>\n",
			 MESSAGE_LEFT_POSITION );
		return;
	}

	sprintf(	yaxis_label,
			"Daily Average %s",
			datatype_name );
	format_initial_capital( yaxis_label, yaxis_label );

	google_chart_include( output_file );

	google_chart_output_visualization_annotated(
				output_file,
				google_chart->google_chart_type,
				google_chart->timeline_list,
				google_chart->barchart_list,
				google_chart->datatype_name_list,
				google_chart->google_package_name,
				daily /* aggregate_level */,
				google_chart->chart_number );

	google_chart_output_chart_instantiation(
		output_file,
		google_chart->chart_number );

	google_chart_anchor_chart(
				output_file,
				"" /* chart_title */,
				google_chart->google_package_name,
				google_chart->left,
				google_chart->top,
				google_chart->width,
				google_chart->height,
				google_chart->chart_number );

} /* output_current() */

boolean populate_point_array_current(
				LIST *timeline_list,
				LIST *datatype_name_list,
				LIST *station_name_list,
				char *datatype_name,
				boolean bar_chart,
				char *application_name )
{
	char sys_string[ 2048 ];
	char input_buffer[ 1024 ];
	FILE *input_pipe;
	boolean got_one = 0;
	char date_time_string[ 128 ];
	char point_string[ 128 ];
	double total = 0.0;

	populate_point_array_current_sys_string(
		sys_string,
		station_name_list,
		datatype_name,
		bar_chart,
		application_name );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		got_one = 1;

		google_timeline_set_point_string(
			timeline_list,
			datatype_name_list,
			input_buffer,
			FOLDER_DATA_DELIMITER );

		piece(	date_time_string, 
			FOLDER_DATA_DELIMITER,
			input_buffer, 
			0 );

		piece(	point_string, 
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		if ( bar_chart )
		{
			total += atof( point_string );

			google_timeline_set_point(
				timeline_list,
				datatype_name_list,
				date_time_string,
				(char *)0 /* hhmm */,
				DATATYPE_QUANTUM_TOTAL,
				total );
		}
	}

	pclose( input_pipe );

	return got_one;

} /* populate_point_array_current() */

void populate_point_array_current_sys_string(
				char *sys_string,
				LIST *station_name_list,
				char *datatype_name,
				boolean bar_chart,
				char *application_name )
{
	char *aggregation_function;
	char *select_clause;
	char *station_in_clause;
	char where_clause[ 1024 ];
	char where_date_clause[ 1024 ];
	char *por_historical_begin_date;
	char *por_historical_end_date;
	char *por_current_begin_date;
	char *por_current_end_date;
	char *current_begin_date;
	char *current_end_date;

	if ( bar_chart )
		aggregation_function = "sum";
	else
		aggregation_function = "avg";

	current_vs_historical_get_dates(
			&por_historical_begin_date,
			&por_historical_end_date,
			&por_current_begin_date,
			&por_current_end_date,
			&current_begin_date,
			&current_end_date,
			application_name );

	station_in_clause =
		timlib_with_list_get_in_clause(
			station_name_list );

	select_clause = "measurement_date,datatype";

	sprintf( where_date_clause,
	 	"measurement_date >= '%s'",
		current_begin_date );

	sprintf( where_clause,
		 "station in (%s) and			"
		 "datatype = '%s' and			"
		 "%s					",
		 station_in_clause,
		 datatype_name,
		 where_date_clause );

/*
	sprintf( sys_string,
"echo \"	select %s,%s( measurement_value )	 	 "
"		from measurement				 "
"		where %s					 "
"		group by %s;\"					|"
"sql_quick.e '|'						|"
"grep -v '|$'							|"
"piece_inverse.e 0 '/' 						|"
"sort								|"
"statistics_on_group.e Day					|"
"egrep 'Average|Day'						|"
"piece.e ':' 1							|"
"joinlines.e '%c' 2						|"
"tr '/' '%c'							|"
"cat								 ",
		 select_clause,
		 aggregation_function,
		 where_clause,
		 select_clause,
		 FOLDER_DATA_DELIMITER,
		 FOLDER_DATA_DELIMITER );
*/

	sprintf( sys_string,
"echo \"	select %s,%s( measurement_value )	 	 "
"		from measurement				 "
"		where %s					 "
"		group by %s;\"					|"
"sql_quick.e '%c'						|"
"grep -v '\\^$'							|"
"cat								 ",
		 select_clause,
		 aggregation_function,
		 where_clause,
		 select_clause,
		 FOLDER_DATA_DELIMITER );

} /* populate_point_array_current_sys_string() */

void output_historical_current(
				FILE *output_file,
				LIST *station_name_list,
				char *datatype_name,
				boolean bar_chart,
				char *application_name )
{
	GOOGLE_OUTPUT_CHART *google_chart;
	char yaxis_label[ 128 ];

	if ( ! ( google_chart =
			get_google_historical_current_chart(
				station_name_list,
				datatype_name,
				bar_chart,
				application_name ) ) )
	{
		fprintf( output_file,
"<div style=\"position: absolute; left: %dpx; top: 450px\"><h3>No current data selected.</h3></div>\n",
			 MESSAGE_LEFT_POSITION );
		return;
	}

	sprintf(	yaxis_label,
			"Daily %s %s",
			(bar_chart) ? "Sum" : "Average",
			datatype_name );

	format_initial_capital( yaxis_label, yaxis_label );

	google_chart_include( output_file );

	google_chart_output_visualization_annotated(
				output_file,
				google_chart->google_chart_type,
				google_chart->timeline_list,
				google_chart->barchart_list,
				google_chart->datatype_name_list,
				google_chart->google_package_name,
				daily /* aggregate_level */,
				google_chart->chart_number );

	google_chart_output_chart_instantiation(
		output_file,
		google_chart->chart_number );

	google_chart_anchor_chart(
				output_file,
				"" /* chart_title */,
				google_chart->google_package_name,
				google_chart->left,
				google_chart->top,
				google_chart->width,
				google_chart->height,
				google_chart->chart_number );

} /* output_historical_current() */

GOOGLE_OUTPUT_CHART *get_google_historical_current_chart(
				LIST *station_name_list,
				char *datatype_name,
				boolean bar_chart,
				char *application_name )

{
	GOOGLE_OUTPUT_CHART *google_chart;

	google_chart =
		google_output_chart_new(
		     LOCAL_CHART_POSITION_LEFT + 140,
		     LOCAL_CHART_POSITION_TOP + HISTORICAL_CHART_HEIGHT + 2,
		     LOCAL_CHART_WIDTH - 280,
		     HISTORICAL_CHART_HEIGHT / 3 );

	list_append_pointer(	google_chart->datatype_name_list,
				datatype_name );

	if ( !populate_point_array_historical_current(
				google_chart->timeline_list,
				google_chart->datatype_name_list,
				station_name_list,
				datatype_name,
				bar_chart,
				application_name ) )
	{
		return (GOOGLE_OUTPUT_CHART *)0;;
	}

	return google_chart;

} /* get_google_historical_current_chart() */

boolean populate_point_array_historical_current(
				LIST *timeline_list,
				LIST *datatype_name_list,
				LIST *station_name_list,
				char *datatype_name,
				boolean bar_chart,
				char *application_name )
{
	char sys_string[ 2048 ];
	char input_buffer[ 1024 ];
	FILE *input_pipe;
	boolean got_one = 0;

	populate_point_array_current_sys_string(
		sys_string,
		station_name_list,
		datatype_name,
		bar_chart,
		application_name );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		got_one = 1;

		google_timeline_set_point_string(
			timeline_list,
			datatype_name_list,
			input_buffer,
			FOLDER_DATA_DELIMITER );
	}

	pclose( input_pipe );

	return got_one;

} /* populate_point_array_historical_current() */

LIST *get_historical_long_term_datatype_name_display_list(
				LIST *datatype_name_list )
{
	char *datatype_name;
	char datatype_name_display[ 128 ];
	LIST *datatype_name_display_list;

	datatype_name_display_list = list_new();

	if ( list_rewind( datatype_name_list ) )
	{
		do {
			datatype_name =
				list_get_pointer(
					datatype_name_list );

			format_initial_capital(
				datatype_name_display,
				datatype_name );

			list_append_pointer(
				datatype_name_display_list,
				strdup( datatype_name_display ) );

		} while( list_next( datatype_name_list ) );
	}

	return datatype_name_display_list;

} /* get_historical_long_term_datatype_name_display_list() */

