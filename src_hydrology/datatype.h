/* $APPASERVER_HOME/src_hydrology/datatype.h		*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#ifndef DATATYPE_H
#define DATATYPE_H

#include <string.h>
#include <ctype.h>
#include "aggregate_statistic.h"
#include "appaserver_library.h"
#include "boolean.h"
#include "list.h"
#include "timlib.h"
#include "piece.h"
#include "units.h"

/* Structures */
/* ---------- */
typedef struct
{
	char *datatype_alias;
	char *datatype_name;
} DATATYPE_ALIAS;

typedef struct
{
	char *datatype_name;
	char *units_string;
	UNITS *units;
	boolean bar_chart;
	boolean scale_graph_to_zero;
	boolean aggregation_sum;
	boolean set_negative_values_to_zero;
	boolean calibrated;
	char *ysi_load_heading;
	char *exo_load_heading;
	int column_piece;
	LIST *datatype_alias_list;
	int measurement_count;
	double units_converted_multiply_by;
} DATATYPE;

/* Prototypes */
/* ---------- */
DATATYPE *datatype_new( char *datatype_name );

LIST *datatype_get_datatype_list(
			char *application_name,
			char *station,
			char plot_for_station_check_yn,
			enum aggregate_statistic );

LIST *datatype_list_get(
			char *application_name );

LIST *datatype_fetch_list(
			char *application_name );

LIST *datatype_get_list(
			char *application_name );

char *datatype_get_units_string(
			boolean *bar_graph,
			char *application_name,
			char *datatype_name );

DATATYPE *datatype_unit_record2datatype(
			char *record );

DATATYPE *datatype_parse(
			char *application_name,
			char *record );

boolean datatype_list_exists(
			LIST *datatype_list,
			char *datatype_name );

LIST *datatype_with_station_name_list_get_datatype_list(
			char *application_name,
			LIST *station_name_list );

LIST *datatype_list(
			char *application_name,
			char *station_name );

LIST *datatype_with_station_name_get_datatype_list(
			char *application_name,
			char *station_name );

char *datatype_list_display(
			LIST *datatype_list );

LIST *datatype_with_station_name_list_get_datatype_bar_graph_list(
			char *application_name,
			LIST *station_name_list );

boolean datatype_bar_chart(
			char *application_name,
			char *datatype_name );

DATATYPE *datatype_set_or_get_datatype(
			LIST *datatype_list,
			/* ------------------- */
			/* Assume stack memory */
			/* ------------------- */
			char *datatype_name );

DATATYPE *datatype_list_seek(
			LIST *datatype_list,
			char *datatype_name );

boolean datatype_get_bypass_data_collection_frequency(
				char *application_name,
				char *station,
				char *datatype );

boolean datatype_bypass_data_collection_frequency(
				char *application_name,
				char *station,
				char *datatype );

DATATYPE *datatype_list_ysi_load_heading_seek(
				LIST *datatype_list,
				char *two_line_datatype_heading );

DATATYPE *datatype_list_exo_load_heading_seek(
				LIST *datatype_list,
				char *two_line_datatype_heading );

LIST *datatype_list_get_unique_unit_list(
				LIST *datatype_list );

LIST *datatype_get_datatypes_for_unit(
				LIST *datatype_list,
				char *unit );

DATATYPE *datatype_calloc(	void );

LIST *datatype_alias_list(
				char *application_name );

DATATYPE_ALIAS *datatype_alias_new(
				void );

DATATYPE_ALIAS *datatype_alias_seek(
				LIST *datatype_alias_list,
				char *datatype_alias_name );

void datatype_free(		DATATYPE *datatype );

LIST *datatype_fetch_alias_list(
				char *application_name,
				char *datatype_name );

char *datatype_alias_display(
				LIST *station_list,
				char *application_name,
				char *station_name,
				char *datatype_alias );

/* -------------------------------- */
/* Sets units_converted_multiply_by */
/* -------------------------------- */
DATATYPE *datatype_seek_phrase(
				LIST *station_datatype_list,
				char *station_name,
				/* -----------------------	*/
				/* Samples: Salinity (PSU)	*/
				/*	    Salinity		*/
				/* ----------------------- 	*/
				char *datatype_units_seek_phrase );

char *datatype_name_seek_phrase(
				LIST *station_datatype_list,
				char *station_name,
				/* -----------------------	*/
				/* Samples: Salinity (PSU)	*/
				/*	    Salinity		*/
				/* ----------------------- 	*/
				char *datatype_units_seek_phrase );

LIST *datatype_column_piece_datatype_list(
				LIST *datatype_list );

LIST *datatype_fetch_datatype_alias_list(
				char *application_name );

char *datatype_alias_datatype_name(
				LIST *datatype_alias_list,
				char *datatype_label );

#endif
