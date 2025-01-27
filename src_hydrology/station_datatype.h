/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_hydrology/station_datatype.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef STATION_DATATYPE_H
#define STATION_DATATYPE_H

#include "boolean.h"
#include "list.h"
#include "datatype.h"

#define STATION_DATATYPE_TABLE	"station_datatype"

#define STATION_DATATYPE_SELECT						\
				"station,"				\
				"datatype,"				\
				"validation_required_yn,"		\
				"plot_for_station_check_yn,"		\
				"sensor_depth_ft,"			\
				"manipulate_agency,"			\
				"bypass_data_collection_frequency_yn"

typedef struct
{
	char *station_name;
	char *datatype_name;
	boolean validation_required_boolean;
	boolean plot_for_station_check_boolean;
	double sensor_depth_ft;
	char *manipulate_agency;
	boolean bypass_data_collection_frequency_boolean;
	DATATYPE *datatype;
} STATION_DATATYPE;

/* Usage */
/* ----- */
LIST *station_datatype_list(
		char *station_name,
		char *datatype_name,
		char *where,
		boolean fetch_datatype );

/* Process */
/* ------- */

/* Returns static memory or where */
/* ------------------------------ */
char *station_datatype_where(
		char *station_name,
		char *datatype_name,
		char *where );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
STATION_DATATYPE *station_datatype_parse(
		boolean fetch_datatype,
		char *input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
STATION_DATATYPE *station_datatype_new(
		char *station_name,
		char *datatype_name );

/* Process */
/* ------- */
STATION_DATATYPE *station_datatype_calloc(
		void );

/* Usage */
/* ----- */
void station_datatype_output(
		LIST *station_datatype_list );

/* Usage */
/* ----- */
void station_datatype_site_visit_calibrate_output(
		LIST *station_datatype_list );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *station_datatype_string(
		char widget_drop_down_label_delimiter,
		char *station_name,
		char *datatype_name,
		char *units );

#endif
