/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/google_map.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit Appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "String.h"
#include "timlib.h"
#include "piece.h"
#include "column.h"
#include "application.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "environ.h"
#include "document.h"
#include "application_constant.h"
#include "google_map.h"

GOOGLE_MAP_POINT *google_map_point_latitude_longitude_new(
			char *latitude_string,
			char *longitude_string,
			char *google_map_click_message )
{
	GOOGLE_MAP_POINT *google_map_point;

	if ( !latitude_string
	||   !longitude_string
	||   !google_map_click_message )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	google_map_point = google_map_point_calloc();

	google_map_point->latitude_nad83 =
		google_map_point_nad83(
			latitude_string /* latitude_longitude */ );

	google_map_point->longitude_nad83 =
		google_map_point_nad83(
			longitude_string /* latitude_string */ );

	google_map_point->javascript =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		google_map_point_javascript(
			google_map_point->latitude_nad83,
			google_map_point->longitude_nad83,
			google_map_click_message );

	return google_map_point;
}

GOOGLE_MAP_POINT *google_map_point_calloc( void )
{
	GOOGLE_MAP_POINT *google_map_point;

	if ( ! ( google_map_point = calloc( 1, sizeof ( GOOGLE_MAP_POINT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return google_map_point;
}

GOOGLE_MAP_POINT *google_map_point_utm_new(
			char *utm_easting_string,
			char *utm_northing_string,
			char *google_map_click_message )
{
	GOOGLE_MAP_POINT *google_map_point;

	if ( !utm_easting_string
	||   !utm_northing_string
	||   !google_map_click_message )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	google_map_point = google_map_point_calloc();

	google_map_point_cs2cs(
		&google_map_point->latitude_nad83,
		&google_map_point->longitude_nad83,
		utm_easting_string,
		utm_northing_string );

	google_map_point->javascript =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		google_map_point_javascript(
			google_map_point->latitude_nad83,
			google_map_point->longitude_nad83,
			google_map_click_message );

	return google_map_point;
}

double google_map_point_nad83( char *latitude_longitude )
{
	int character_count;
	double nad83;

	if ( !latitude_longitude )
	{
		char message[ 128 ];

		sprintf(message, "latitude_longitude empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	character_count =
		string_character_count(
			' ',
			latitude_longitude );

	if ( !character_count )
	{
		nad83 = atof( latitude_longitude );
	}
	else
	if ( character_count == 1 )
	{
		nad83 = google_map_point_degrees_minutes( latitude_longitude );
	}
	else
	{
		nad83 =
			google_map_point_degrees_minutes_seconds(
				latitude_longitude );
	}

	return nad83;
}

double google_map_point_degrees_minutes( char *latitude_longitude )
{
	char degrees[ 16 ];
	char minutes_float[ 16 ];
	double result;

	if ( !latitude_longitude )
	{
		char message[ 128 ];

		sprintf(message, "latitude_longitude is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !*latitude_longitude ) return 0.0;

	*minutes_float = '\0';

	column( degrees, 0, latitude_longitude );
	column( minutes_float, 1, latitude_longitude );

	if ( *degrees == '-' )
	{
		result = atof( degrees ) -
		          (atof( minutes_float ) / 60.0 );
	}
	else
	{
		result = atof( degrees ) +
		          (atof( minutes_float ) / 60.0 );
	}

	return result;
}

double google_map_point_degrees_minutes_seconds( char *latitude_longitude )
{
	char degrees[ 16 ];
	char minutes[ 16 ];
	char seconds[ 16 ];
	double result;

	if ( !latitude_longitude )
	{
		char message[ 128 ];

		sprintf(message, "latitude_longitude is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !*latitude_longitude ) return 0.0;

	*minutes = '\0';
	*seconds = '\0';

	column( degrees, 0, latitude_longitude );
	column( minutes, 1, latitude_longitude );
	column( seconds, 2, latitude_longitude );

	if ( *degrees == '-' )
	{
		result = atof( degrees ) -
		          ((atof( minutes ) / 60.0 ) +
		           (atof( seconds ) / 3600.0 ) );
	}
	else
	{
		result = atof( degrees ) +
		          ((atof( minutes ) / 60.0 ) +
		           (atof( seconds ) / 3600.0 ) );
	}
	return result;
}

void google_map_point_cs2cs(
			double *latitude_nad83,
			double *longitude_nad83,
			char *utm_easting_string,
			char *utm_northing_string )
{
	char *pipe_fetch;
	char longitude_latitude_string[ 128 ];

	if ( !latitude_nad83
	||   !longitude_nad83
	||   !utm_easting_string
	||   !utm_northing_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	pipe_fetch =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_pipe_fetch(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			google_map_point_cs2cs_system_string(
				utm_easting_string,
				utm_northing_string ) );

	if ( !pipe_fetch )
	{
		char message[ 128 ];

		sprintf(message,
			"string_pipe_fetch(%s,%s) returned empty.",
			utm_easting_string,
			utm_northing_string );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	column( longitude_latitude_string, 0, pipe_fetch );
	*longitude_nad83 = atof( longitude_latitude_string );

	column( longitude_latitude_string, 1, pipe_fetch );
	*latitude_nad83 = atof( longitude_latitude_string );
}

char *google_map_point_cs2cs_system_string(
			char *utm_easting_string,
			char *utm_northing_string )
{
	static char system_string[ 128 ];

	if ( !utm_easting_string
	||   !utm_northing_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(system_string,
		"echo \"%s %s\" | cs2cs_utm.sh",
		utm_easting_string,
		utm_northing_string );

	return system_string;
}

char *google_map_point_javascript(
			double latitude_nad83,
			double longitude_nad83,
			char *google_map_click_message )
{
	char javascript[ 256 ];

	if ( !latitude_nad83
	||   !longitude_nad83
	||   !google_map_click_message )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(javascript,
		"\tpoint = new google.maps.LatLng(%.7lf,%.7lf);\n"
		"\tcreate_selectable_map_marker( map, point, '%s' );\n\n",
		latitude_nad83,
		longitude_nad83,
		google_map_click_message );

	return strdup( javascript );
}

char *google_map_key( char *google_map_key_label )
{
	if ( !google_map_key_label )
	{
		char message[ 128 ];

		sprintf(message, "google_map_key_label is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	application_constant_fetch(
		google_map_key_label );
}

char *google_map_initialize_javascript(
		char *google_map_canvas_label,
		char *google_map_center_latitude,
		char *google_map_center_longitude,
		int google_map_starting_zoom )
{
	char script[ 1024 ];

	if ( !google_map_canvas_label
	||   !google_map_center_latitude
	||   !google_map_center_longitude )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(script,
	"function initialize()\n"
	"{\n"
	"	var point;\n"           
	"	var the_center = "
	"new google.maps.LatLng(%s,%s);\n" 
	"\n"                            
	"	var mapOptions = {\n"
	"		center: the_center,\n"
	"		zoom: %d,\n"
	"		mapTypeId: google.maps.MapTypeId.roadmap\n"
	"};\n"
	"\n"
	"	var map = new google.maps.Map(\n"
	"		document.getElementById(\"%s\"),\n"
	"		mapOptions);\n"
	"\n",
	google_map_center_latitude,
	google_map_center_longitude,
	google_map_starting_zoom,
	google_map_canvas_label );

	return strdup( script );
}

char *google_map_script_open_tag( void )
{
	return
	"<script type=\"text/javascript\">\n"
	"\n"
	"//<![CDATA[\n"
	"\n";
}

char *google_map_script_close_tag( void )
{
	return
	"}\n"
	"//]]>\n"
	"</script>\n";
}

char *google_map_body_html( char *google_map_canvas_label )
{
	char html[ 1024 ];

	if ( !google_map_canvas_label )
	{
		char message[ 128 ];

		sprintf(message, "google_map_canvas_label is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	snprintf(
		html,
		sizeof ( html ),
		"<body onload=\"initialize()\" "
		"leftmargin=0 "
		"topmargin=0 "
		"marginwidth=0 "
		"marginheight=0>\n"
		"<div id=\"%s\"></div>\n"
		"</body>\n",
		google_map_canvas_label );

	return strdup( html );
}

char *google_map_canvas_style_html(
		char *google_map_canvas_label,
		int absolute_position_top,
		int absolute_position_left,
		int google_map_width,
		int google_map_height )
{
	char style_html[ 2048 ];
	char *ptr = style_html;

	if ( !google_map_canvas_label )
	{
		char message[ 128 ];

		sprintf(message, "google_map_canvas_label is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr += sprintf( ptr,
"<style type=\"text/css\">\n"
"#%s {\n"
"	width: %dpx;\n"
"	height: %dpx;\n",
	google_map_canvas_label,
	google_map_width,
	google_map_height );

	if ( absolute_position_top + absolute_position_left )
	{
		ptr += sprintf( ptr,
"	position: absolute;\n"
"	top: %dpx;\n"
"	left: %dpx;\n",
		absolute_position_top,
		absolute_position_left );
	}

	sprintf( ptr,
"}\n"
"</style>\n" );

	return strdup( style_html );
}

char *google_map_loader_script_html( char *google_map_key )
{
	char html[ 1024 ];

	if ( !google_map_key )
	{
		char message[ 128 ];

		sprintf(message, "google_map_key is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

/*
	sprintf(html,
"<script type=\"text/javascript\"\n"
"src=\"https://maps.googleapis.com/maps/api/js?key=%s+sensor=false\">\n"
"</script>\n",
		google_map_key );
*/

	snprintf(
		html,
		sizeof ( html ),
"<script async src=\"https://maps.googleapis.com/maps/api/js?key=%s&callback=console.debug&libraries=maps,marker&v=beta\"></script>",
		google_map_key );

	return strdup( html );
}

char *google_map_create_marker_javascript( void )
{
	return
"var infowindow;\n"
"function create_selectable_map_marker( map, point, click_message )\n"
"{\n"
"	var content;\n"
"	var marker;\n"
"\n"
"	content = '<u style=\"font-style: 2em;\" onclick=onclick_selectable_function(\"' + click_message + '\")>' + click_message + '</u>';\n"
"\n"
"	marker = new google.maps.Marker({\n"
"		position: point,\n"
"		map: map\n"
"});\n"
"\n"
"	infowindow = new google.maps.InfoWindow({\n"
"	content: '<br>' + click_message\n"
"});\n"
"\n"
"	google.maps.event.addListener(marker, 'click', function() {\n"
"	infowindow.setContent( content );\n"
"	infowindow.open( map, marker );\n"
"	});\n"
"}\n";
}

char *google_map_click_message( char *message )
{
	char destination[ 1024 ];

	if ( !message )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	strdup( string_initial_capital( destination, message ) );
}

