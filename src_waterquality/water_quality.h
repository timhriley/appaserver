/* ---------------------------------------------------	*/
/* src_waterquality/water_quality.h			*/
/* ---------------------------------------------------	*/
/* This is the library for the water quality project.	*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#ifndef WATER_QUALITY_H
#define WATER_QUALITY_H

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
#define WATER_QUALITY_STATION_LABEL		"station"
#define WATER_QUALITY_COLLECTION_DATE_LABEL	"collection_date"
#define WATER_QUALITY_COLLECTION_TIME_LABEL	"collection_time"
#define WATER_QUALITY_DEPTH_LABEL		"collection_depth_meters"
#define WATER_QUALITY_LONGITUDE_LABEL		"longitude"
#define WATER_QUALITY_LATITUDE_LABEL		"latitude"
#define COLLECTION_DATE_HEADING_KEY		"collection_date_heading"
#define COLLECTION_TIME_HEADING_KEY		"collection_time_heading"
#define COLLECTION_DEPTH_METERS_HEADING_KEY	\
	"collection_depth_meters_heading"
#define STATION_HEADING_KEY			"station_heading"
#define STATION_LATITUDE_HEADING_KEY		"station_latitude_heading"
#define STATION_LONGITUDE_HEADING_KEY		"station_longitude_heading"

/* Structures */
/* ---------- */
typedef struct
{
	char *exception;
	char *exception_code;
} EXCEPTION;

typedef struct
{
	char *parameter_name;
	char *units;
} PARAMETER_UNIT;

typedef struct
{
	char *column_heading_string;
	PARAMETER_UNIT *parameter_unit;
} PARAMETER_UNIT_ALIAS;

typedef struct
{
	char *concentration;
	PARAMETER_UNIT *parameter_unit;
	LIST *exception_list;
} RESULTS;

typedef struct
{
	char *collection_date;
	char *collection_date_international;
	char *collection_time;
	char *collection_time_without_colon;
	double collection_depth_meters_double;
	char *sample_id;
	char *station;
	char *collection_depth_unit;
	char *lab_lims_id;
	char *lab_test_code;
	char *parameter_string;
	char *aliased_parameter;
	char *aliased_units;
	char *minimum_detection_limit;
	char *concentration;
	char *sample_comments;
	char *results_comments;
	char *matrix_code;
	char *units;
	char *matrix;
	char *flow_no_flow_code;
	char *flow_no_flow;
	char *up_down_stream_code;
	char *up_down_stream;
	char *exception_code_multiple;
} COLLECTION;

typedef struct
{
	PARAMETER_UNIT *parameter_unit;
} STATION_PARAMETER;

typedef struct
{
	char *station_name;
	char *longitude;
	char *latitude;
	LIST *station_parameter_list;
	LIST *collection_list;
	LIST *alias_name_list;
} STATION;

typedef struct
{
	LIST *station_list;
} WATER_PROJECT;

typedef struct
{
	char *unit_alias;
	char *units;
} UNIT_ALIAS;

typedef struct
{
	char *parameter_alias;
	char *parameter_name;
} PARAMETER_ALIAS;

typedef struct
{
	char *station_collection_attribute;
	PARAMETER_UNIT *parameter_unit;
	int column_piece;
} WATER_LOAD_COLUMN;

typedef struct
{
	WATER_PROJECT *water_project;
	LIST *parameter_alias_list;
	LIST *unit_alias_list;
	LIST *parameter_name_list;
	LIST *unit_name_list;
	LIST *exception_list;
	LIST *parameter_unit_list;
} WATER_QUALITY_INPUT;

typedef struct
{
	WATER_QUALITY_INPUT input;
	LIST *parameter_unit_alias_list;
	LIST *load_column_list;
} WATER_QUALITY;

/* Prototypes */
/* ---------- */
LIST *water_parameter_name_list(
				char *application_name );

LIST *water_unit_name_list(
				char *application_name );

LIST *water_parameter_alias_list(
				char *application_name );

LIST *water_unit_alias_list(
				char *application_name );

char *water_quality_get_project_name(
				char *application_name,
				char *project_code );

char *water_quality_get_parameter_name(
				char **matrix,
				char *application_name,
				char *parameter_code );

PARAMETER_UNIT_ALIAS *water_parameter_unit_alias_new(
				char *column_heading_string,
				char *parameter_name,
				char *units );

UNIT_ALIAS *water_unit_alias_new(
				void );

EXCEPTION *water_exception_new(
				char *exception_string,
				char *exception_code );

WATER_QUALITY *water_quality_new(
				char *application_name,
				char *project_name );

WATER_PROJECT *water_project_new(
				char *application_name,
				char *project_name );

STATION_PARAMETER *water_new_station_parameter(
				void );

WATER_LOAD_COLUMN *water_new_load_column(
				void );

PARAMETER_ALIAS *water_new_parameter_alias(
				void );

STATION *water_station_fetch(
				char *application_name,
				char *station_name );

STATION *water_station_calloc(
				void );

COLLECTION *water_collection_new(
				void );

PARAMETER_UNIT *water_new_parameter_unit(
				char *parameter_name,
				char *units );

LIST *water_parameter_unit_list(
				char *application_name );

LIST *water_station_parameter_list(
				char *application_name,
				char *station_name );

LIST *water_station_list(	char *application_name,
				char *project_name );

LIST *water_fetch_turkey_point_column_list(
				/* ------ */
				/* in/out */
				/* ------ */
				char *error_message,
				char *load_table_filename,
				LIST *parameter_unit_alias_list,
				DICTIONARY *application_constants_dictionary );

LIST *water_fetch_fiu_column_list(
				/* ------ */
				/* in/out */
				/* ------ */
				char *error_message,
				char *load_table_filename,
				LIST *parameter_unit_list,
				LIST *parameter_alias_list,
				DICTIONARY *application_constants_dictionary );

PARAMETER_ALIAS *water_seek_parameter_alias(
				char *column_heading_string,
				LIST *parameter_alias_list );

PARAMETER_UNIT *water_parameter_name_seek_parameter_unit(
				char *parameter_name,
				LIST *parameter_unit_list );

PARAMETER_UNIT *water_legacy_seek_alias_parameter_unit(
				char *column_heading_string,
				LIST *parameter_unit_list,
				LIST *parameter_alias_list );

int water_get_station_collection_attribute_piece(
				char *station_collection_attribute,
				LIST *load_column_list );

WATER_LOAD_COLUMN *water_get_next_load_column_parameter_unit(
				LIST *load_column_list );

void water_seek_application_constants_dictionary(
				LIST **collection_date_heading_list,
				LIST **collection_time_heading_list,
				LIST **collection_depth_meters_heading_list,
				LIST **station_heading_list,
				LIST **station_latitude_heading_list,
				LIST **station_longitude_heading_list,
				DICTIONARY *application_constants_dictionary );

LIST *water_get_parameter_unit_alias_list(
				LIST *parameter_name_list,
				LIST *unit_name_list,
				LIST *parameter_alias_list,
				LIST *unit_alias_list );

PARAMETER_UNIT *water_parameter_unit_new(
				char *parameter_name,
				char *units );

char *water_get_column_heading_string(
				char *parameter_name,
				char *unit_name );

PARAMETER_UNIT *water_parameter_unit_alias_seek_parameter_unit(
				char *column_heading_string,
				LIST *parameter_unit_alias_list );

LIST *water_fetch_turkey_point_heading_column_list(
				/* ------ */
				/* in/out */
				/* ------ */
				char *error_message,
				char *heading_string,
				LIST *collection_date_heading_list,
				LIST *station_heading_list,
				LIST *parameter_unit_alias_list );

char *water_parameter_unit_alias_list_display(
				LIST *parameter_unit_alias_list );

LIST *water_exception_list(
				char *application_name );

LIST *water_get_results_exception_list(	char *exception_string,
					LIST *exception_list );

char *water_exception_display(	LIST *exception_list );

char *water_load_column_list_display(
				LIST *load_column_list );

LIST *water_station_alias_name_list(
				char *application_name,
				char *station_name );

char *water_station_select(	void );

STATION *water_station_parse(	char *input_buffer );

/* Also checks station->alias_name_list */
/* ------------------------------------ */
STATION *water_station_get_or_set(
				LIST *station_list,
				char *application_name,
				char *station_name );

STATION *water_station_seek(	LIST *station_list,
				char *station_name );

WATER_PROJECT *water_project_calloc(
				void );

void water_collection_free(	COLLECTION *collection );

#endif
