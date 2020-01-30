/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_hydrology/hydrology.h		*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ----------------------------------------------------	*/

#ifndef HYDROLOGY_H
#define HYDROLOGY_H

/* Includes */
/* -------- */
#include "station_datatype.h"
#include "datatype.h"
#include "station.h"

/* Constants */
/* --------- */
#define TURKEY_POINT_DATE_TIME_PIECE		1
#define TURKEY_POINT_FIRST_COLUMN_PIECE		3

/* Structures */
/* ---------- */
typedef struct
{
	LIST *station_list;
} HYDROLOGY_INPUT;

typedef struct
{
	LIST *header_column_datatype_list;
} HYDROLOGY_OUTPUT;

typedef struct
{
	HYDROLOGY_INPUT input;
	HYDROLOGY_OUTPUT output;
} HYDROLOGY;

/* Operations */
/* ---------- */
HYDROLOGY *hydrology_new(	void );

HYDROLOGY *hydrology_fetch_new(	char *application_name,
				char *station_name );

LIST *hydrology_get_header_column_datatype_list(
				LIST *station_datatype_list,
				char *station_name,
				char *input_filename,
				int first_column_piece );

char *hydrology_translate_datatype_name(
				LIST *station_datatype_list,
				char *datatype_name );

STATION *hydrology_set_or_get_station(
				LIST *input_station_list,
				char *application_name,
				char *station_name );

char *hydrology_translate_units_name(
				LIST *station_datatype_list,
				char *units_phrase );

LIST *hydrology_get_header_column_datatype_list(
				LIST *station_datatype_list,
				char *station_name,
				char *input_filename,
				int first_column_piece );

void hydrology_parse_datatype_units_phrase(
				char *datatype_phrase	/* in/out */,
				char *units_phrase	/* in/out */,
				/* -----------------------	*/
				/* Samples: Salinity (PSU)	*/
				/*	    Salinity		*/
				/* ----------------------- 	*/
				char *datatype_units_seek_phrase );

char *hydrology_units_name_seek_phrase(
				LIST *station_datatype_list,
				/* -----------------------	*/
				/* Samples: Salinity (PSU)	*/
				/*	    Salinity		*/
				/* ----------------------- 	*/
				char *datatype_units_seek_phrase );

MEASUREMENT *hydrology_extract_measurement(
				char *input_string,
				int column_piece );

/* Sets station_datatype->measurement_list */
/* --------------------------------------- */
void hydrology_parse_file(
				LIST *station_datatype_list,
				FILE *error_file,
				char *application_name,
				char *input_filename,
				int date_time_piece );

int hydrology_measurement_table_display(
				char *station_name,
				LIST *station_datatype_list );

void hydrology_summary_table_display(
				char *station_name,
				LIST *station_datatype_list );

int hydrology_measurement_insert(
				char *station_name,
				LIST *station_datatype_list );

void hydrology_parse_begin_end_dates(
				char **begin_measurement_date,
				char **end_measurement_date,
				char *input_filespecification,
				char *date_heading_label,
				int date_piece );

/* Returns static memory */
/* --------------------- */
char *hydrology_format_measurement_date(
				/* ------------------------ */
				/* Out: assume stack memory */
				/* ------------------------ */
				char *measurement_time_string,
				/* -- */
				/* In */
				/* -- */
				char *measurement_date_time_string );

boolean hydrology_extract_zulu_date_time(
				/* --- */
				/* Out */
				/* --- */
				char *measurement_date_string,
				/* --- */
				/* Out */
				/* --- */
				char *measurement_time_string,
				/* ----------------------------------- */
				/* In: looks like 2018-08-31T14:30:22Z */
				/* ----------------------------------- */
				char *measurement_date_time_string );

char *hydrology_trim_time(	char *measurement_time_string );

/* Returns static */
/* -------------- */
char *hydrology_extract_hour(	char *measurement_time_string );

/* Returns static */
/* -------------- */
char *hydrology_extract_minute(	char *measurement_time_string );

boolean hydrology_got_heading_label(
				char *date_heading_label,
				char *heading_buffer );

#endif
