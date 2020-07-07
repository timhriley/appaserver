/* library/google_map.c					*/
/* ---------------------------------------------------	*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "application_constants.h"
#include "environ.h"
#include "column.h"
#include "timlib.h"
#include "appaserver_library.h"
#include "folder.h"
#include "google_map.h"

GOOGLE_MAP *google_map_new( void )
{
	GOOGLE_MAP *google_map;

	google_map =
		(GOOGLE_MAP *)
			calloc( 1, sizeof( GOOGLE_MAP ) );
	google_map->point_list = list_new();
	return google_map;

} /* google_map_new() */


void google_map_output_selectable_map_thumbtack(
					FILE *output_file,
					char *click_message,
					double latitude,
					double longitude )
{
	fprintf(output_file,
"	point = new google.maps.LatLng(%.7lf,%.7lf);\n",
		latitude,
		longitude );

	fprintf(output_file,
"	create_selectable_map_marker( map, point, '%s' );\n\n",
		click_message );

} /* google_map_output_selectable_map_thumbtack() */

void google_map_output_map_thumbtack(
					FILE *output_file,
					char *click_message,
					double latitude,
					double longitude )
{
	fprintf(output_file,
"	point = new google.maps.LatLng(%.7lf,%.7lf);\n",
		latitude,
		longitude );

	fprintf(output_file,
"	create_map_marker( map, point, '%s' );\n\n",
		click_message );

} /* google_map_output_map_thumbtack() */

void google_map_output_heading_close( FILE *output_file )
{
	fprintf(output_file,
"}\n"
"\n"
"//]]>\n"
"</script>\n"
"</head>\n" );

} /* google_map_output_heading_close() */

void google_map_output_body(	FILE *output_file,
				boolean with_table,
				char *additional_javascript )
{
	fprintf(output_file,
"<body onload=\"initialize()" );

	if ( additional_javascript && additional_javascript )
	{
		fprintf( output_file, ";%s", additional_javascript );
	}

	fprintf(output_file,
"\" leftmargin=0 topmargin=0 marginwidth=0 marginheight=0>\n"
"\n" );

	if ( with_table )
	{
		fprintf( output_file, "<table><tr><td>" );
	}

	fprintf( output_file,
"<div id=\"%s\"></div>\n",
		 GOOGLE_MAP_CANVAS_ID );

} /* google_map_output_body() */

char *google_map_get_map_key(
			char *application_name )
{
	APPLICATION_CONSTANTS *application_constants;
	char *google_map_key_data;

	application_constants = application_constants_new();
	application_constants->dictionary =
		application_constants_get_dictionary(
			application_name );

	if ( ! ( google_map_key_data =
			application_constants_fetch(
				application_constants->dictionary,
				"google_map_key" ) ) )
	{
		fprintf( stderr,
"ERROR in %s/%s(): cannot get google_map_key from APPLICATION_CONSTANTS.\n",
		 	__FILE__,
		 	__FUNCTION__ );
			exit( 1 );
	}
	return google_map_key_data;
} /* google_map_get_map_key() */

void google_map_set_point(	LIST *point_list,
				char *click_message,
				double latitude_nad83,
				double longitude_nad83,
				int utm_easting,
				int utm_northing )
{
	GOOGLE_MAP_POINT *map_point;

	if ( ! ( map_point = (GOOGLE_MAP_POINT *)
				calloc( 1, sizeof( GOOGLE_MAP_POINT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot allocate memory.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	map_point->click_message = click_message;
	map_point->latitude_nad83 = latitude_nad83;
	map_point->longitude_nad83 = longitude_nad83;
	map_point->utm_easting = utm_easting;
	map_point->utm_northing = utm_northing;
	list_append_pointer( point_list, map_point );
} /* google_map_set_point() */

void google_map_convert_to_latitude_longitude(
					LIST *point_list )
{
	char tmp_filename[ 128 ];
	char input_buffer[ 128 ];
	char sys_string[ 1024 ];
	FILE *output_pipe;
	FILE *input_file;
	char column_buffer[ 128 ];
	GOOGLE_MAP_POINT *map_point;

	if ( !list_rewind( point_list ) ) return;

	sprintf(	tmp_filename,
			"/tmp/google_convert_%d.dat",
			getpid() );
	sprintf( sys_string,
		 "cs2cs_utm.sh > %s",
		 tmp_filename );

	output_pipe = popen( sys_string, "w" );
	do {
		map_point = list_get_pointer( point_list );
		fprintf(	output_pipe,
				"%d %d\n",
				map_point->utm_easting,
				map_point->utm_northing );

	} while( list_next( point_list ) );
	pclose( output_pipe );

	if ( ! ( input_file = fopen( tmp_filename, "r" ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot open %s for read.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			tmp_filename );
		exit( 0 );
	}

	list_rewind( point_list );

	while( get_line( input_buffer, input_file ) )
	{
		map_point = list_get_pointer( point_list );
		column( column_buffer, 0, input_buffer );
		map_point->longitude_nad83 = atof( column_buffer );
		column( column_buffer, 1, input_buffer );
		map_point->latitude_nad83 = atof( column_buffer );
		list_next( point_list );
	}
	fclose( input_file );
	sprintf( sys_string, "rm -f %s", tmp_filename );
	if ( system( sys_string ) ){};
} /* google_map_convert_to_latitude_longitude() */

void google_map_output_point_list(	FILE *output_file,
					LIST *point_list )
{
	GOOGLE_MAP_POINT *map_point;

	if ( list_rewind( point_list ) )
	{
		do {
			map_point =
				list_get_pointer(
					point_list );

			if ( map_point->latitude_nad83
			&&   map_point->longitude_nad83 )
			{
				google_map_output_map_thumbtack(
						output_file,
						map_point->click_message,
						map_point->latitude_nad83,
						map_point->longitude_nad83 );
			}
		} while( list_next( point_list ) );
	}

} /* google_map_output_point_list() */

void google_map_output_selectable_point_list(	FILE *output_file,
						LIST *point_list )
{
	GOOGLE_MAP_POINT *map_point;

	if ( !list_rewind( point_list ) ) return;

	do {
		map_point =
			list_get_pointer(
				point_list );

		if ( map_point->latitude_nad83
		&&   map_point->longitude_nad83 )
		{
			google_map_output_selectable_map_thumbtack(
					output_file,
					map_point->click_message,
					map_point->latitude_nad83,
					map_point->longitude_nad83 );
		}
	} while( list_next( point_list ) );

} /* google_map_output_selectable_point_list() */


double google_map_convert_coordinate( char *latitude_longitude )
{
	if ( character_count( ' ', latitude_longitude ) == 1 )
	{
		return google_map_convert_base_60_with_float(
					latitude_longitude );
	}
	else
	if ( character_count( ' ', latitude_longitude ) == 2 )
	{
		return google_map_convert_base_60( latitude_longitude );
	}
	else
	{
		return atof( latitude_longitude );
	}

} /* google_map_convert_coordinate() */

double google_map_convert_base_60( char *latitude_longitude )
{
	char degrees[ 16 ];
	char minutes[ 16 ];
	char seconds[ 16 ];
	double results;

	if ( !*latitude_longitude ) return 0.0;
	if ( character_count( ' ', latitude_longitude ) != 2 ) return 0.0;

	column( degrees, 0, latitude_longitude );
	column( minutes, 1, latitude_longitude );
	column( seconds, 2, latitude_longitude );

	if ( !*degrees ) return 0.0;

	if ( *degrees == '-' )
	{
		results = atof( degrees ) -
		          ((atof( minutes ) / 60.0 ) +
		           (atof( seconds ) / 3600.0 ) );
	}
	else
	{
		results = atof( degrees ) +
		          ((atof( minutes ) / 60.0 ) +
		           (atof( seconds ) / 3600.0 ) );
	}
	return results;
} /* google_map_convert_base_60() */

double google_map_convert_base_60_with_float( char *latitude_longitude )
{
	char degrees[ 16 ];
	char minutes_float[ 16 ];
	double results;

	if ( !*latitude_longitude ) return 0.0;

	if ( character_count( ' ', latitude_longitude ) != 1 )
		return atof( latitude_longitude );

	column( degrees, 0, latitude_longitude );
	column( minutes_float, 1, latitude_longitude );

	if ( !*degrees ) return 0.0;

	if ( *degrees == '-' )
	{
		results = atof( degrees ) -
		          (atof( minutes_float ) / 60.0 );
	}
	else
	{
		results = atof( degrees ) +
		          (atof( minutes_float ) / 60.0 );
	}
	return results;
} /* google_map_convert_base_60_with_float() */

void google_map_output_heading( FILE *output_file,
				char *title,
				char *google_map_key_data,
				char *balloon_click_parameter,
				char *balloon_click_function,
				char *application_name,
				int absolute_position_top,
				int absolute_position_left,
				char *google_map_center_latitude,
				char *google_map_center_longitude,
				int google_map_starting_zoom,
				int google_map_width,
				int google_map_height )
{
	char map_canvas[ 1024 ];
	char *font_style;

	font_style = "font-size: 2em;";

	if ( absolute_position_top + absolute_position_left )
	{
		sprintf( map_canvas,
"#%s {\n"
"	position: absolute;\n"
"	top: %dpx;\n"
"	left: %dpx;\n"
"	width: %dpx;\n"
"	height: %dpx\n"
"	}",
			GOOGLE_MAP_CANVAS_ID,
			absolute_position_top,
			absolute_position_left,
			google_map_width,
			google_map_height );
	}
	else
	{
		sprintf( map_canvas,
"#%s {\n"
"	width: %dpx;\n"
"	height: %dpx\n"
"	}",
			GOOGLE_MAP_CANVAS_ID,
			google_map_width,
			google_map_height );
	}

	fprintf(output_file,
"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n"
"\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n"
"<html xmlns=\"http://www.w3.org/1999/xhtml\">\n"
"<head>\n"
"<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"/>\n"
"<meta name=\"viewport\" content=\"initial-scale=1.0, user-scalable=no\" />\n"
"<style type=\"text/css\">\n"
"%s\n"
"</style>\n"
"<link rel=stylesheet type=text/css href=/appaserver/%s/style.css>\n"
"<script type=\"text/javascript\"\n"
"src=\"https://maps.googleapis.com/maps/api/js?key=%s&sensor=false\">\n"
"</script>\n",
		map_canvas,
		application_name,
		google_map_key_data );

	if ( title )
	{
		fprintf( output_file,
		 	"<title>%s</title>\n",
		 	title );
	}

	fprintf(output_file,
"<script type=\"text/javascript\">\n"
"\n"
"//<![CDATA[\n"
"\n" );

	if ( balloon_click_parameter && balloon_click_function )
	{
		fprintf(output_file,
"function onclick_selectable_function( %s )\n"
"{\n"
"	%s;\n"
"	return true;\n"
"}\n"
"\n",
			balloon_click_parameter,
			balloon_click_function );
	}

	fprintf(output_file,
"function create_map_marker( map, point, click_message )\n"
"{\n"
"	var infowindow;\n"
"\n"
"	infowindow = new google.maps.InfoWindow({\n"
"				content: '<br>' + click_message\n"
"			});\n"
"\n"
"	var marker = new google.maps.Marker({\n"
"		position: point,\n"
"		map: map\n"
"  	});\n"
"\n"
"	google.maps.event.addListener(marker, 'click', function() {\n"
"		infowindow.open( map, marker );\n"
"		});\n"
"}\n" );

	fprintf(output_file,
"var infowindow;\n"
"function create_selectable_map_marker( map, point, click_message )\n"
"{\n"
"	var content;\n"
"	var marker;\n"
"\n"
"	content = '<u style=\"%s\" onclick=onclick_selectable_function(\"' + click_message + '\")>' + click_message + '</u>';\n"
"\n"
"	marker = new google.maps.Marker({\n"
"		position: point,\n"
"		map: map\n"
"  	});\n"
"\n"
"	infowindow = new google.maps.InfoWindow({\n"
"				content: '<br>' + click_message\n"
"			});\n"
"\n"
"	google.maps.event.addListener(marker, 'click', function() {\n"
"		infowindow.setContent( content );\n"
"		infowindow.open( map, marker );\n"
"		});\n"
"}\n",
	font_style );

	fprintf(output_file,
"var rectangle;\n"
"var rectangle_southwest_latitude;\n"
"var rectangle_southwest_longitude;\n"
"var rectangle_northeast_latitude;\n"
"var rectangle_northeast_longitude;\n\n" );

	fprintf(output_file,
"function initialize() {\n"
"	var point;\n"
"	var the_center = new google.maps.LatLng(%s,%s);\n"
"\n"
"	var mapOptions = {\n"
"		center: the_center,\n"
"		zoom: %d,\n"
"		mapTypeId: google.maps.MapTypeId.SATELLITE\n"
"	};\n"
"\n"
"	var map = new google.maps.Map(\n"
"			document.getElementById(\"%s\"),\n"
"			mapOptions);\n"
"\n",
		google_map_center_latitude,
		google_map_center_longitude,
		google_map_starting_zoom,
		GOOGLE_MAP_CANVAS_ID );

/*
	fprintf(output_file,
"	var infowindow = new google.maps.InfoWindow();\n" );
*/

} /* google_map_output_heading() */

void google_map_output_rectangle(
				FILE *output_file,
				char *southwest_latitude,
				char *southwest_longitude,
				char *northeast_latitude,
				char *northeast_longitude )
{
	fprintf( output_file,
"	var bounds = new google.maps.LatLngBounds(\n"
"		new google.maps.LatLng(%s,%s),\n"
"		new google.maps.LatLng(%s,%s)\n"
"	);\n"
"\n"
"	rectangle = new google.maps.Rectangle({\n"
"		bounds: bounds,\n"
"		editable: true,\n"
"		draggable: true\n"
"	});\n"
"\n"
"	rectangle.setMap(map);\n\n",
		 southwest_latitude,
		 southwest_longitude,
		 northeast_latitude,
		 northeast_longitude );

	fprintf( output_file,
"	google.maps.event.addListener(rectangle, 'bounds_changed', rectangle_bounds_changed);\n" );

} /* google_map_output_rectangle() */

void google_map_output_rectangle_bounds_changed(
				FILE *output_file )
{
	fprintf( output_file,
"<script type=\"text/javascript\">\n" );

	fprintf( output_file,
"function rectangle_bounds_changed( event ) {\n"
"	var ne = rectangle.getBounds().getNorthEast();\n"
"	var sw = rectangle.getBounds().getSouthWest();\n"
"\n"
"	rectangle_southwest_latitude = sw.lat();\n"
"	rectangle_southwest_longitude = sw.lng();\n"
"	rectangle_northeast_latitude = ne.lat();\n"
"	rectangle_northeast_longitude = ne.lng();\n"
"\n" );

/*
	fprintf( output_file,
"	alert("
"' sw_lat = ' + rectangle_southwest_latitude + "
"' sw_lng = ' + rectangle_southwest_longitude + "
"' ne_lat = ' + rectangle_northeast_latitude + "
"' ne_lng = ' + rectangle_northeast_longitude);\n" );
*/

	fprintf( output_file,
"	return true;\n" );

	fprintf( output_file,
"}\n" );

	fprintf( output_file,
"</script>\n" );

} /* google_map_output_rectangle_bounds_changed() */

void google_map_output_rectangle_copy_to_form(
				FILE *output_file )
{
	fprintf( output_file,
"<script type=\"text/javascript\">\n" );

	fprintf( output_file,
"function rectangle_copy_to_form() {\n"
"	var element;\n"
"\n"
"	element = timlib_get_element( \"rectangle_southwest_latitude\" );"
"	element.value = rectangle_southwest_latitude;\n"
"\n"
"	element = timlib_get_element( \"rectangle_southwest_longitude\" );"
"	element.value = rectangle_southwest_longitude;\n"
"\n"
"	element = timlib_get_element( \"rectangle_northeast_latitude\" );"
"	element.value = rectangle_northeast_latitude;\n"
"\n"
"	element = timlib_get_element( \"rectangle_northeast_longitude\" );"
"	element.value = rectangle_northeast_longitude;\n\n" );

	fprintf( output_file,
"	return true;\n" );

	fprintf( output_file,
"}\n" );

	fprintf( output_file,
"</script>\n" );

} /* google_map_output_rectangle_copy_to_form() */

char *google_map_get_balloon(	char *application_name,
				char *folder_name,
				char *role_name,
				DICTIONARY *dictionary,
				char *balloon_attribute_name_list_string )
{
	static char label[ 4096 ];
	char buffer[ 4096 ];
	int first_time = 1;
	char *ptr = label;
	LIST *attribute_name_list;
	char *attribute_name;
	char *data;
	LIST *no_display_attribute_name_list = {0};
	FOLDER *folder;

	folder = folder_calloc();

	if ( !balloon_attribute_name_list_string )
	{
		folder->folder_mto1_isa_related_folder_list =
			folder_mto1_isa_related_folder_list(
				list_new() /* existing_related_folder_list */,
				application_name,
				folder_name,
				role_name,
				0 /* recursive_level */ );

		folder->folder_append_isa_attribute_list =
			folder_append_isa_attribute_list(
				application_name,
				folder_name,
				folder->folder_mto1_isa_related_folder_list,
				role_name );

		folder->folder_append_isa_attribute_name_list =
			folder_append_isa_attribute_name_list(
				folder->folder_append_isa_attribute_list );
	}
	else
	{
		folder->folder_append_isa_attribute_name_list =
			list_string2list(
				balloon_attribute_name_list_string,
				',' );
	}

	no_display_attribute_name_list =
		google_map_get_no_display_attribute_name_list(
			dictionary );

	folder->folder_append_isa_attribute_name_list =
		list_subtract_list(
			folder->folder_append_isa_attribute_name_list,
			no_display_attribute_name_list );

	attribute_name_list =
		folder->folder_append_isa_attribute_name_list;

	if ( !list_rewind( attribute_name_list ) ) return "";

	*ptr = '\0';

	do {
		attribute_name = list_get_pointer( attribute_name_list );

		if ( ( data = dictionary_fetch(	dictionary,
						attribute_name ) ) )
		{
			if ( first_time )
				first_time = 0;
			else
				ptr += sprintf( ptr, "<br>" );

			sprintf(	buffer,
					"%s: %s",
					attribute_name,
					data );
			format_initial_capital( buffer, buffer );
			ptr += sprintf( ptr, "%s", buffer );
		}
	} while( list_next( attribute_name_list ) );

	strcpy( buffer, label );
	escape_character( label, buffer, '\'' );
	return label;

} /* google_map_get_balloon() */

LIST *google_map_get_no_display_attribute_name_list(
			DICTIONARY *dictionary )
{
	LIST *no_display_attribute_name_list;

	no_display_attribute_name_list =
		dictionary_extract_and_remove_prefixed_key_list(
				dictionary_remove_index_zero( dictionary ),
				IGNORE_PUSH_BUTTON_PREFIX );

	return no_display_attribute_name_list;

} /* google_map_get_no_display_attribute_name_list() */

