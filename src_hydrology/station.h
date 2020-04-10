/* $APPASERVER_HOME/src_hydrology/station.h		   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#ifndef STATION_H
#define STATION_H

#include "list.h"
#include "dictionary.h"
#include "measurement.h"

/* Constants */
/* --------- */
#define STATION_WATERQUALITY_SELECT_LIST_STRING	\
"station,latitude,longitude"

#define STATION_SELECT_LIST_STRING	\
"station,lat_nad83,long_nad83,agency,ground_surface_elevation_ft,vertical_datum,ngvd29_navd88_conversion,station_type,comments"

typedef struct 
{
	char *station_name;
	char *agency;
	double ground_surface_elevation_ft;
	boolean ground_surface_elevation_null;
	double lat_nad83;
	double long_nad83;
	char *vertical_datum;
	double ngvd29_navd88_conversion;
	char *station_type;
	char *comments;
	LIST *measurement_list;
	LIST *datatype_name_list;
	LIST *station_datatype_list;
	LIST *shef_upload_datatype_list;
} STATION;

/* Operations */
/* ---------- */
STATION *station_new(		char *station_name );

double station_get_ground_surface_elevation_ft(
				char **ground_surface_elevation_null,
				char *application_name,
				char *station_name );

LIST *station_get_global_station_list(
				char *application_name );

STATION *station_seek(		char *station_name,
				LIST *station_list );

boolean station_load(		double *lat_nad83,
				double *long_nad83,
				char **agency,
				double *ground_surface_elevation_ft,
				boolean *ground_surface_elevation_null,
				char **vertical_datum,
				double *ngvd29_navd88_conversion,
				char **station_type,
				char **comments,
				char *application_name,
				char *station_name );

LIST *station_get_agency_station_list(
				char *application_name,
				char *agency );

LIST *station_get_login_name_station_list(
				char *application_name,
				char *login_name );

LIST *station_get_enp_station_list(
				char *application_name );

LIST *station_get_waterquality_station_list(
				char *application_name );

LIST *station_get_agency_name_list(
				char *application_name,
				char *login_name );

void station_parse_station_buffer(
			double *lat_nad83,
			double *long_nad83,
			char **agency,
			double *ground_surface_elevation_ft,
			boolean *ground_surface_elevation_null,
			char **vertical_datum,
			double *ngvd29_navd88_conversion,
			char **station_type,
			char **comments,
			char *input_buffer );

LIST *station_get_rectangle_station_name_list(
			char *application_name,
			double southwest_latitude,
			double southwest_longitude,
			double northeast_latitude,
			double northeast_longitude );

LIST *station_get_zone_rectangle_station_name_list(
			LIST *global_station_list,
			double southwest_latitude,
			double southwest_longitude,
			double northeast_latitude,
			double northeast_longitude,
			double zone );

LIST *station_get_station_type_station_list(
			LIST *station_list,
			char *station_type );

LIST *station_get_station_type_list(
			char *application_name,
			char *login_name );

DICTIONARY *station_load_dictionary(
			char *application_name,
			char *station_name );

boolean station_exists(	char *application_name,
			char *station_name );

STATION *station_fetch_new(
			char *application_name,
			char *station_name );

STATION *station_get_or_set_station(
			LIST *input_station_list,
			char *application_name,
			char *station_name );

LIST *station_fetch_station_datatype_list(
			char *application_name,
			char *station_name );

#endif
