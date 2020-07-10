/* --------------------------------------------------- 	*/
/* src_hydrology/google_map_station_process.c	      	*/
/* --------------------------------------------------- 	*/
/* This is the helper process assigned to the Output    */
/* Measurements prelookup screen. It allows you to	*/
/* choose one or more stations via the google map.	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

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
#include "google_map_station.h"

/* Constants */
/* --------- */
#define TITLE	"Google Map Station"

/* Structures */
/* ---------- */
	
/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	GOOGLE_MAP_STATION *google_map_station;
	char *google_map_key_data;
	STATION *station;
	char *login_name;
	char *database_string = {0};
	char *application_name;

	if ( argc != 3 )
	{
		fprintf( stderr,
			 "Usage: %s application login_name\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	login_name = argv[ 2 ];

	google_map_key_data =
		google_map_station_get_map_key(
			application_name );

	google_map_station_output_heading(
				stdout,
				TITLE,
				google_map_key_data,
				"station",
				"populate_station_list( station )",
				application_name,
				0 /* absolute_position_top */,
				0 /* absolute_position_left */ );

	if ( ! ( google_map_station =
			google_map_station_new(
				application_name,
				login_name ) ) )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: google_map_station_new() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	google_map_station->station_list =
		google_map_fetch_station_list(
			google_map_station->application_name,
			google_map_station->login_name,
			(char *)0 /* station_type */,
			0 /* current_year */ );

	if ( list_rewind( google_map_station->station_list ) )
	{
		do {
			station = list_get_pointer(
					google_map_station->station_list );

			if ( station->long_nad83 && station->lat_nad83 )
			{
				google_map_station_output_selectable_station(
						stdout,
						station->station_name,
						station->long_nad83,
						station->lat_nad83 );
			}
		} while( list_next( google_map_station->station_list ) );
	}

	google_map_station_output_head_close( stdout );

	google_map_station_output_body(
				stdout,
				1 /* with_table */,
				(char *)0 /* additional_javascript */ );

	printf(
"<script type=text/javascript>\n"
"function populate_station_list( station_name )\n"
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
"		if ( element_name == \"station_list\" )\n"
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
"\n"
"		if ( process_element.options[ j ].text == station_name )\n"
"		{\n"
"			station_name = \"\";\n"
"			break;\n"
"		}\n"
"		if ( process_element.options[ j ].text == \"\" )\n"
"		{\n"
"			break;\n"
"		}\n"
"	}\n"
"	process_element.options[ j ] =\n"
"		new Option(\n"
"			station_name,\n"
"			station_name,\n"
"			0,\n"
"			0 /* not selected */ );\n"
"	return true;\n"
"\n"
"}\n"
"function copy_to_main_screen()\n"
"{\n"
"	var parent_element_name;\n"
"	var parent_element;\n"
"	var process_parent_element;\n"
"	var element_name;\n"
"	var element;\n"
"	var process_element;\n"
"	var form;\n"
"	var parent_form;\n"
"\n"
"	form = document.forms[ 0 ];\n"
"	process_element = false;\n"
"	for( var i = 0; i < form.elements.length; i++ )\n"
"	{\n"
"		element = form.elements[ i ];\n"
"		element_name = element.name;\n"
"		if ( element_name == \"station_list\" )\n"
"		{\n"
"			process_element = element;\n"
"			break;\n"
"		}\n"
"	}\n"
"	if ( process_element == false ) return false;\n"
"\n"
"	parent_form = opener.document.forms[ 0 ];\n"
"\n"
"	process_parent_element = false;\n"
"	for( var i = 0; i < parent_form.elements.length; i++ )\n"
"	{\n"
"		parent_element = parent_form.elements[ i ];\n"
"		parent_element_name = parent_element.name;\n"
"		if ( parent_element_name == \"original_station_1\" )\n"
"		{\n"
"			process_parent_element = parent_element;\n"
"			break;\n"
"		}\n"
"	}\n"
"\n"
"	if ( process_parent_element == false ) return false;\n"
"\n"
"	for(	var i = 0;\n"
"		i < process_element.options.length;\n"
"		i++ )\n"
"	{\n"
"\n"
"		if ( process_element.options[ i ].text != \"\" )\n"
"		{\n"
"			var parent_station_name;\n"
"			var station_name;\n"
"\n"
"			station_name = process_element.options[ i ].text;\n"
"\n"
"			for(	var j = 0;\n"
"				j < process_parent_element.options.length;\n"
"				j++ )\n"
"			{\n"
"				parent_station_name =\n"
"				process_parent_element.options[ j ].value;\n"
"\n"
"				if ( parent_station_name == station_name )\n"
"				{\n"
"					process_parent_element.options[ j ].\n"
"						selected = 1;\n"
"				}\n"
"			}\n"
"		}\n"
"	}\n"
"\n"
"	post_change_multi_select_move_right(\n"
"		'original_station_1', 'station_1' );\n"
"	return true;\n"
"\n"
"}\n"
"function post_change_multi_select_move_right(\n"
"		original_element_name, to_element_name )\n"
"{\n"
"	var original_element;\n"
"	var to_element;\n"
"	var i;\n"
"	var j;\n"
"	var new_text;\n"
"	var new_value;\n"
"	var new_value_with_index;\n"
"	var search_element;\n"
"	var original_element;\n"
"	var search_element_name;\n"
"	var parent_form;\n"
"\n"
"	parent_form = opener.document.forms[ 0 ];\n"
"\n"
"	original_element = false;\n"
"	for( var i = 0; i < parent_form.elements.length; i++ )\n"
"	{\n"
"		search_element = parent_form.elements[ i ];\n"
"		search_element_name = search_element.name;\n"
"		if ( search_element_name == original_element_name )\n"
"		{\n"
"			original_element = search_element;\n"
"			break;\n"
"		}\n"
"	}\n"
"\n"
"	if ( original_element == false ) return false;\n"
"\n"
"	to_element = false;\n"
"	for( var i = 0; i < parent_form.elements.length; i++ )\n"
"	{\n"
"		search_element = parent_form.elements[ i ];\n"
"		search_element_name = search_element.name;\n"
"		if ( search_element_name == to_element_name )\n"
"		{\n"
"			to_element = search_element;\n"
"			break;\n"
"		}\n"
"	}\n"
"\n"
"	if ( to_element == false ) return false;\n"
"\n"
"	for( i = 0; i < original_element.options.length; i++ )\n"
"	{\n"
"		if ( original_element.options[ i ].selected )\n"
"		{\n"
"			var new_option;\n"
"\n"
"			new_value = original_element.options[ i ].value;\n"
"\n"
"			if ( new_value != \"\" )\n"
"			{\n"
"				new_value_with_index =\n"
"					new_value + '%c' + i;\n"
"\n"
"				new_text = original_element.options[ i ].text;\n"
"\n"
"				for(	j = 0;\n"
"					j < to_element.options.length;\n"
"					j++ )\n"
"				{\n"
"					if ( to_element.options[ j ].text==\"\" )\n"
"					{\n"
"						break;\n"
"					}\n"
"				}\n"
"\n"
"				new_option =\n"
"					new Option(\n"
"						new_text,\n"
"						new_value_with_index,\n"
"						0,\n"
"						1 /* selected */ );\n"
"\n"
"				to_element.options[ j ] = new_option;\n"
"\n"
"				// Set the selected flag to prevent the\n"
"				// list from scrolling to the top.\n"
"				// ------------------------------------\n"
"				original_element.options[ i ] =\n"
"					new Option( \"\", \"\", 0, 1 );\n"
"			}\n"
"		}\n"
"	}\n"
"	return true;\n"
"} // post_change_multi_select_move_right()\n"
"\n"
"</script>\n"
"<td valign=top>\n"
"<form>\n"
"<table>\n"
"<tr><td valign=top><select name=station_list size=10 multiple></select>\n"
"<tr>\n"
"<td valign=top><input type=button value=Submit onClick=\"copy_to_main_screen(); window.close()\">\n"
"</table>\n"
"</form>\n"
"</table>\n"
"</body>\n"
"</html>\n",
	ELEMENT_MULTI_SELECT_MOVE_LEFT_RIGHT_INDEX_DELIMITER );

	return 0;
} /* main() */

