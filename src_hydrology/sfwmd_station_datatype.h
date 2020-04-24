/* sfwmd_station_datatype.h */
/* ------------------------ */

#ifndef SFWMD_STATION_DATATYPE_H
#define SFWMD_STATION_DATATYPE_H

#include "list.h"
#include "hash_table.h"

typedef struct
{
	char *station;
	char *datatype;
	char *db_key;
	double measurement_value;
	boolean is_null;
	char last_validation_date_string[ 16 ];
	char measurement_date_string[ 16 ];
	char measurement_time_string[ 16 ];
	double conversion_factor;
	char last_validation_date[ 16 ];
	boolean provisional;
} SFWMD_STATION_DATATYPE;

#define SFWMD_STATION_DATATYPE_FOLDER	"dbhydro_dfe"

HASH_TABLE *sfwmd_station_datatype_hash_table_load(
					char *application_name );

SFWMD_STATION_DATATYPE *sfwmd_station_datatype_calloc(
					void );

void sfwmd_spreadsheet_parse(	char *application_name,
				char *filename );

SFWMD_STATION_DATATYPE *sfwmd_parse(
				/* ------------------- */
				/* Expect stack memory */
				/* ------------------- */
				char *db_key,
				HASH_TABLE *sfwmd_station_datatype_hash_table,
				char *input_buffer );

char *sfwmd_to_yyyy_mm_dd(	char *d, char *s );

int sfwmd_ora_month2integer(	char *mon );

#endif
