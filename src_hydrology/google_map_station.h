/* google_map_station.h */
/* -------------------- */

#ifndef GOOGLE_MAP_STATION_H
#define GOOGLE_MAP_STATION_H

#include <stdio.h>
#include <string.h>
#include "list.h"
#include "station.h"

/* Constants */
/* --------- */
#define GOOGLE_MAP_STATION_URL_TEMPLATE		"%s/%s/google_maps_%s.html"
#define GOOGLE_MAP_STATION_PROMPT_TEMPLATE	"/%s/google_maps_%s.html"
#define GOOGLE_MAP_STATION_HTTP_PROMPT_TEMPLATE	"%s://%s/%s/google_maps_%s.html"
#define GOOGLE_MAP_STATION_STARTING_ZOOM	9
#define GOOGLE_MAP_STATION_WIDTH		750
#define GOOGLE_MAP_STATION_HEIGHT		550

typedef struct
{
	char *application_name;
	char *login_name;
	LIST *station_list;
} GOOGLE_MAP_STATION;

/* Prototypes */
/* ---------- */
GOOGLE_MAP_STATION *google_map_station_new(	
					char *application_name,
					char *login_name );

void google_map_station_output_heading( FILE *output_file,
					char *title,
					char *google_map_key_data,
					char *balloon_click_parameter,
					char *balloon_click_function,
					char *application_name,
					int absolute_position_top,
					int absolute_position_left );

void google_map_station_output_station(	FILE *output_file,
					char *station,
					double longitude,
					double latitude );

void google_map_station_output_selectable_station(
					FILE *output_file,
					char *station,
					double longitude,
					double latitude );

void google_map_station_output_head_close(
					FILE *output_file );

void google_map_station_output_body(	FILE *output_file,
					boolean with_table,
					char *additional_javascript );

char *google_map_station_get_map_key(
					char *application_name );

LIST *google_map_station_list(		char *application_name,
					int year,
					char *station_type );

LIST *google_map_fetch_station_list(	char *application_name,
					char *login_name,
					char *station_type,
					int current_year );

LIST *google_map_station_type_list_fetch(
					int current_year );

LIST *google_map_station_datatype_list(
					char *application_name,
					int current_year,
					char *station_name );

#endif
