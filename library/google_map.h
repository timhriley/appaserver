/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/google_map.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit Appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef GOOGLE_MAP_H
#define GOOGLE_MAP_H

#include "boolean.h"
#include "list.h"

/*
#define GOOGLE_MAP_CENTER_LATITUDE	"25.68821"
#define GOOGLE_MAP_CENTER_LONGITUDE	"-80.84817"
*/
#define GOOGLE_MAP_CENTER_LATITUDE	"38.6"
#define GOOGLE_MAP_CENTER_LONGITUDE	"-121.5"
#define GOOGLE_MAP_STARTING_ZOOM	13
#define GOOGLE_MAP_WIDTH		1150
#define GOOGLE_MAP_HEIGHT		800
#define GOOGLE_MAP_CANVAS_LABEL		"map_canvas"
#define GOOGLE_MAP_KEY_LABEL		"google_map_key"

typedef struct
{
	/* stub */
} GOOGLE_MAP;

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *google_map_key(	char *google_map_key_label );

/* Process */
/* ------- */

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *google_map_initialize_javascript(
			char *google_map_canvas_label,
			char *google_map_center_latitude,
			char *google_map_center_longitude,
			int google_map_starting_zoom );

/* Process */
/* ------- */

/* Usage */
/* ----- */

/* Returns program memory */
/* ---------------------- */
char *google_map_script_open_tag(
			void );

/* Process */
/* ------- */

/* Usage */
/* ----- */

/* Returns program memory */
/* ---------------------- */
char *google_map_script_close_tag(
			void );

/* Process */
/* ------- */

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *google_map_body_html(
			char *google_map_canvas_label );

/* Process */
/* ------- */

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *google_map_canvas_style_html(
			char *google_map_canvas_label,
			int absolute_position_top,
			int absolute_position_left,
			int google_map_width,
			int google_map_height );

/* Process */
/* ------- */

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *google_map_loader_script_html(
			char *google_map_key );

/* Process */
/* ------- */

/* Usage */
/* ----- */

/* Returns program memory */
/* ---------------------- */
char *google_map_create_marker_javascript(
			void );

/* Process */
/* ------- */

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *google_map_click_message(
			char *message );

/* Process */
/* ------- */

typedef struct
{
	double latitude_nad83;
	double longitude_nad83;
	char *javascript;
} GOOGLE_MAP_POINT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
GOOGLE_MAP_POINT *google_map_point_latitude_longitude_new(
			char *latitude_string,
			char *longitude_string,
			char *google_map_click_message );

/* Process */
/* ------- */
GOOGLE_MAP_POINT *google_map_point_calloc(
			void );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
GOOGLE_MAP_POINT *google_map_point_utm_new(
			char *utm_easting_string,
			char *utm_northing_string,
			char *google_map_click_message );

/* Process */
/* ------- */

/* Usage */
/* ----- */
double google_map_point_nad83(
			char *latitude_longitude );

/* Process */
/* ------- */

/* Usage */
/* ----- */
double google_map_point_degrees_minutes(
			char *latitude_longitude );

/* Process */
/* ------- */

/* Usage */
/* ----- */
double google_map_point_degrees_minutes_seconds(
			char *latitude_longitude );

/* Process */
/* ------- */

/* Usage */
/* ----- */
void google_map_point_cs2cs(
			double *latitude_nad83,
			double *longitude_nad83,
			char *utm_easting_string,
			char *utm_northing_string );

/* Process */
/* ------- */

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *google_map_point_cs2cs_system_string(
			char *utm_easting_string,
			char *utm_northing_string );

/* Process */
/* ------- */

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *google_map_point_javascript(
			double latitude_nad83,
			double longitude_nad83,
			char *google_map_click_message );

/* Process */
/* ------- */

#endif
