/* $APPASERVER_HOME/src_hydrology/cubic_feet.h		*/
/* ==================================================== */
/*                                                      */
/* Freely available software: see Appaserver.org	*/
/* ==================================================== */

#ifndef CUBIC_FEET_H
#define CUBIC_FEET_H

#include "list.h"
#include "boolean.h"
#include "julian.h"
#include "hash_table.h"

/* Constants */
/* --------- */
#define UNITS				"cfs"
#define DATE_PIECE		 	0
#define TIME_PIECE		 	-1
#define VALUE_PIECE		 	1
#define INPUT_DELIMITER			','
#define DEFAULT_DAYS_TO_SUM		30
#define SELECT_LIST			"measurement_date,measurement_value"

typedef struct
{
	char *measurement_date;
	double measurement_daily_average;
	double measurement_station_total;
	double moving_sum;
} CUBIC_FEET_MEASUREMENT;

/* CUBIC_FEET_MEASUREMENT operations */
/* --------------------------------- */
CUBIC_FEET_MEASUREMENT *cubic_feet_measurement_calloc(
			void );

LIST *cubic_feet_measurement_list(
			char *system_string );

CUBIC_FEET_MEASUREMENT *cubic_feet_measurement_new(
			char *measurement_date );

char *cubic_feet_measurement_system_string(
			char *application_name,
			char *cubic_feet_station_where,
			char *end_date_string,
			char *cubic_feet_units_convert_process );

typedef struct
{
	char *where;
	LIST *measurement_list;
} CUBIC_FEET_STATION;

/* CUBIC_FEET_STATION operations */
/* ----------------------------- */
CUBIC_FEET_STATION *cubic_feet_station_calloc(
			void );

LIST *cubic_feet_station_list(
			char *application_name,
			LIST *station_name_list,
			char *datatype_name,
			JULIAN *cubic_feet_start_date_julian,
			char *end_date_string,
			char *cubic_feet_units_convert_process );

CUBIC_FEET_STATION *cubic_feet_station_new(
			char *application_name,
			char *station_name,
			char *datatype_name,
			JULIAN *cubic_feet_start_date_julian,
			char *end_date_string,
			char *cubic_feet_units_convert_process );

/* Returns heap memory */
/* ------------------- */
char *cubic_feet_station_where(
			char *station_name,
			char *datatype_name,
			JULIAN *cubic_feet_start_date_julian,
			char *end_date_string );

typedef struct
{
	JULIAN *start_date_julian;
	char *units_display;
	char *title;
	char *subtitle;
	char *units_convert_process;
	LIST *station_list;
	HASH_TABLE *hash_table;
	LIST *total_measurement_list;
	LIST *moving_sum_measurement_list;
} CUBIC_FEET;

/* CUBIC_FEET operations */
/* --------------------- */
CUBIC_FEET *cubic_feet_calloc(
			void );

CUBIC_FEET *cubic_feet_new(
			char *application_name,
			LIST *station_name_list,
			char *datatype_name,
			char *begin_date_string,
			char *end_date_string,
			int days_to_sum,
			char *units_converted );

JULIAN *cubic_feet_start_date_julian(
			char *begin_date_string,
			int days_to_sum );

char *cubic_feet_units_display(
			char *application_name,
			char *datatype_name,
			char *units_converted );

char *cubic_feet_title(
			LIST *station_name_list,
			char *datatype_name );

char *cubic_feet_subtitle(
			char *begin_date_string,
			char *end_date_string,
			int days_to_sum );

char *cubic_feet_units_convert_process(
			char *units_converted );

HASH_TABLE *cubic_feet_hash_table(
			JULIAN *cubic_feet_start_date_julian,
			char *cubic_feet_end_date_string );

HASH_TABLE *cubic_feet_station_total_hash_table(
			/* ------------------ */
			/* Returns hash_table */
			/* ------------------ */
			HASH_TABLE *hash_table,
			LIST *cubic_feet_station_list );

LIST *cubic_feet_total_measurement_list(
			HASH_TABLE *hash_table );

LIST *cubic_feet_moving_sum_measurement_list(
			char *begin_date_string,
			int days_to_sum,
			LIST *cubic_feet_total_measurement_list,
			HASH_TABLE *hash_table );

char *cubic_feet_moving_sum_produce(
			char *begin_date_string,
			int days_to_sum,
			LIST *cubic_feet_total_measurement_list );

LIST *cubic_feet_moving_sum_consume(
			char *temp_filename,
			HASH_TABLE *hash_table,
			LIST *cubic_feet_total_measurement_list );

#endif
