/* $APPASERVER_HOME/src_hydrology/hydrology_library.h			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef HYDROLOGY_LIBRARY_H
#define HYDROLOGY_LIBRARY_H

/* Includes */
/* -------- */
#include "list.h"
#include "list_usage.h"
#include "aggregate_statistic.h"
#include "aggregate_level.h"
#include "validation_level.h"
#include "dictionary.h"
#include "boolean.h"
#include "julian.h"

/* Constants */
/* --------- */
#define HYDROLOGY_LIBRARY_SKIP_HEADER_ROWS	"skip_header_rows"
#define DRIFT_MEASUREMENT_DETECTION_LIMIT 0.5
#define DRIFT_RATIO_PROCESS_NAME	"linear_interpolation_drift_by_ratio"
#define OXYGEN_SOLUBILITY_DATATYPE	"oxygen_solubility"
#define VALID_FREQUENCY_TIME_SEQUENCE	"0,6,12,15,18,24,30,36,42,45,48,54"
#define NULL_VALUE_INSERT_DISPLAY_COUNT	10000
#define NULL_VALUE_COLUMNS_TO_INSERT	"station,datatype,measurement_date,measurement_time,reason_value_missing"

#define MISSING_REPLACEMENT		"-666.000"
#define NULL_REPLACEMENT		-999.000
#define HYDROLOGY_LIBRARY_MISSING	"Missing"

/* Prototypes */
/* ---------- */
char hydrology_library_get_aggregation_sum(
					char *application_name,
					char *datatype_name );

char get_bar_graph_yn(			char *application,
					char *appaserver_mount_point,
					char *datatype );

enum aggregate_statistic based_on_datatype_get_aggregate_statistic(
					char *application_name,
					char *datatype,
					enum aggregate_statistic );

LIST *based_on_datatype_get_aggregate_statistic_list(
					char *application_name,
					LIST *datatype_name_list );

double hydrology_library_get_measurement_value(
					boolean *is_null,
					char *application_name,
					char *station,
					char *datatype,
					char *measurement_date,
					char *measurement_time );

double hydrology_library_get_latest_before_measurement_value(
					char *application_name,
					char *station,
					char *datatype,
					char *begin_date );

char *hydrology_library_get_latest_measurement_date_time(
					char **latest_measurement_time,
					char *application_name,
					char *station,
					char *datatype,
					char *begin_date );

void hydrology_library_get_period_of_record_begin_end_dates(
					char **begin_date_string,
					char **end_date_string,
					char *application_name,
					char *station,
					char *datatype );

char *hydrology_library_get_period_of_record_end_date(
					char *application_name,
					char *station,
					char *datatype );

char *hydrology_library_get_period_of_record_begin_date(
					char *application_name,
					char *station,
					char *datatype );

void hydrology_library_get_clean_begin_end_date(
					char **begin_date,
					char **end_date,
					char *application_name,
					char *station,
					char *datatype );

void hydrology_library_with_list_get_clean_begin_end_date(
					char **begin_date,
					char **end_date,
					char *application_name,
					LIST *station_name_list,
					LIST *datatype_name_list );

char *hydrology_library_get_units_string(
					boolean *bar_graph,
					char *application_name,
					char *datatype );

boolean hydrology_library_get_bar_graph(char *application_name,
					char *datatype_name );

void hydrology_library_get_title(
					char *title,
					char *sub_title,
					enum validation_level,
					enum aggregate_statistic,
					enum aggregate_level,
					char *station_name,
					char *datatype,
					char *begin_date_string,
					char *end_date_string,
					char accumulate_yn);

int get_latest_measurements_per_day( 	char *application,
					char *station,
					char *datatype );

int hydrology_library_insert_null_measurements(
					char *station,
					char *datatype,
					char *begin_date,
					char *begin_time,
					char *end_date,
					char *end_time,
					char *reason_value_missing,
					int minutes_offset,
					char really_yn,
					char *process_title,
					boolean with_table_output,
					char *application_name );

double hydrology_library_get_switch_julian(
					LIST *expected_count_list,
					int minutes_offset );

char *hydrology_library_get_expected_count_list_string(
					char *application,
					char *station,
					char *datatype,
					char delimiter );

char *hydrology_library_get_flow_units(	char *application_name );

char *hydrology_library_get_stage_units(char *application_name );

char *hydrology_library_get_datatype_units(
					char *application_name,
					char *datatype_name );

char *hydrology_library_get_datatype_units_display(
					char *application_name,
					char *datatype,
					char *datatype_units,
					char *datatype_units_converted,
					enum aggregate_statistic );

char *hydrology_library_get_output_invalid_units_error(
					char *datatype,
					char *units,
					char *units_converted );

boolean hydrology_library_can_convert_to_units(
					char *application_name,
					char *datatype,
					char *units_converted );

boolean hydrology_library_some_measurements_validated(
					char *application_name,
					char *where_clause,
					char *station,
					char *datatype,
					char *begin_measurement_date,
					char *end_measurement_date );

void hydrology_library_get_clean_begin_end_time(
					char **begin_time_string,
					char **end_time_string );

boolean hydrology_library_get_drift_ratio_variables(
					DICTIONARY *parameter_dictionary,
					double *first_multiplier_double,
					double *last_multiplier_double,
					char *application_name,
					double first_value_double,
					double last_value_double,
					char *station,
					char *datatype,
					char *begin_date_string,
					char *begin_time_string,
					char *end_date_string,
					char *end_time_string );

char *hydrology_library_get_ground_surface_elevation_string(
				double ground_surface_elevation_ft,
				boolean ground_surface_elevation_null );

void hydrology_library_output_station_text_filename(
				char *output_filename,
				char *application_name,
				char *station_name,
				boolean with_zap_file );

void hydrology_library_output_station_text_file(
				FILE *output_file,
				char *application_name,
				char *station_name );

void hydrology_library_output_station_table(
				char *application_name,
				char *station_name );

void hydrology_library_output_data_collection_frequency_table(
				char *application_name,
				char *station_name,
				char *datatype_name );

void hydrology_library_output_data_collection_frequency_text_file(
				FILE *output_file,
				char *application_name,
				char *station_name,
				char *datatype_name );

boolean hydrology_library_get_begin_end_year(
				char **begin_year,
				char **end_year,
				char *year_expression );

/* Returns program memory. */
/* ----------------------- */
char *hydrology_library_provisional_where(
				enum validation_level validation_level );

#endif
