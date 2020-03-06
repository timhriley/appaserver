/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_hydrology/merged.h		*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ----------------------------------------------------	*/

#ifndef MERGED_H
#define MERGED_H

/* Includes */
/* -------- */
#include "list.h"
#include "hash_table.h"
#include "dictionary.h"

/* Constants */
/* --------- */
#define MERGED_DATE_PIECE		 	0
#define MERGED_TIME_PIECE		 	1
#define MERGED_VALUE_PIECE		 	2

#define MERGED_HYDROLOGY_SELECT_LIST		"measurement_date,measurement_time,measurement_value"
#define MERGED_WATERQUALITY_SELECT_LIST		"collection_date,collection_time,concentration"

/* Structures */
/* ---------- */
typedef struct
{
	char *measurement_date_string;
	double measurement_value;
	boolean is_null;
	int count;
} MERGED_MEASUREMENT;

typedef struct
{
	char *station;
	char *datatype;
	char *units;
	boolean bar_graph;
	HASH_TABLE *measurement_hash_table;
} MERGED_STATION_DATATYPE;

typedef struct
{
	LIST *waterquality_station_name_list;
	LIST *waterquality_parameter_name_list;
	LIST *waterquality_units_name_list;
	LIST *hydrology_station_name_list;
	LIST *hydrology_datatype_name_list;
} MERGED_INPUT;

typedef struct
{
	MERGED_INPUT *merged_input;
	LIST *waterquality_station_datatype_list;
	LIST *hydrology_station_datatype_list;
} MERGED;

/* Operations */
/* ---------- */

/* ------------------- */
/* Returns heap memory */
/* ------------------- */
char *merged_station_datatype_list_display(
				LIST *station_datatype_list );

MERGED *merged_new(		MERGED_INPUT *merged_input );

MERGED_INPUT *merged_input_new( DICTIONARY *parameter_dictionary );

MERGED_STATION_DATATYPE *merged_station_datatype_new(
				void );

LIST *merged_waterquality_station_datatype_list(
				LIST *waterquality_station_name_list,
				LIST *waterquality_parameter_name_list,
				LIST *waterquality_units_name_list );

LIST *merged_hydrology_station_datatype_list(
				LIST *hydrology_station_name_list,
				LIST *hydrology_datatype_name_list );

void merged_measurement_hash_table(
				char *application_name,
				char *begin_date,
				char *end_date,
				LIST *hydrology_station_datatype_list );

void merged_results_hash_table(
				char *begin_date,
				char *end_date,
				LIST *waterquality_station_datatype_list );

HASH_TABLE *merged_hash_table(	char *sys_string );

MERGED_MEASUREMENT *merged_measurement_new(
				char *measurement_date_string );

HASH_TABLE *merged_key_hash_table(
				LIST *station_datatype_list );

char *merged_waterquality_sys_string(
				char *begin_date,
				char *end_date,
				char *station,
				char *parameter,
				char *units );

char *merged_hydrology_sys_string(
				char *application_name,
				char *begin_date,
				char *end_date,
				char *station_name,
				char *datatype_name );

LIST *merged_heading_list(	LIST *waterquality_station_datatype_list,
				LIST *hydrology_station_datatype_list );

#endif
