/* -------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/units.h					*/
/* -------------------------------------------------------------	*/
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------	*/

#ifndef UNITS_H
#define UNITS_H

#include "list.h"
#include "boolean.h"
#include "dictionary.h"

#define UNITS_CONVERTED_SELECT		"units_converted,"	\
					"multiply_by"

#define UNITS_CONVERTED_TABLE		"units_converted"
#define UNITS_CONVERTED_LABEL		"units_converted"

typedef struct
{
	char *units_name;
	char *units_converted_name;
	double multiply_by;
} UNITS_CONVERTED;

/* Usage */
/* ----- */
LIST *units_converted_list(
			char *units_name,
			char *units_primary_where );

/* Process */
/* ------- */

/* Usage */
/* ----- */
UNITS_CONVERTED *units_converted_fetch(
			char *units_name,
			char *units_converted_name );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *units_converted_primary_where(
			char *units_name,
			char *units_converted_name );

/* Usage */
/* ----- */
UNITS_CONVERTED *units_converted_parse(
			char *units_name,
			char *input );

/* Process */
/* ------- */
UNITS_CONVERTED *units_converted_calloc(
			void );

/* Usage */
/* ----- */

/* Returns component of dictionary or null */
/* --------------------------------------- */
char *units_converted_dictionary_extract(
			DICTIONARY *dictionary );

/* Process */
/* ------- */

#define UNITS_CONVERTED_STDIN_NAVD88_NAME	"feet_navd88"

typedef struct
{
	char *units_name;
	char *units_converted_name;
	int value_piece;
	char delimiter;
	int station_piece;
	boolean temperature_boolean;
	boolean navd88_boolean;
	DICTIONARY *station_navd88_dictionary;
	UNITS_CONVERTED *units_converted;
} UNITS_CONVERTED_STDIN;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
UNITS_CONVERTED_STDIN *units_converted_stdin_new(
			char *units_name,
			char *units_converted_name,
			int value_piece,
			char delimiter,
			int station_piece );

/* Process */
/* ------- */
UNITS_CONVERTED_STDIN *units_converted_stdin_calloc(
			void );

/* Returns program memory or units_name */
/* ------------------------------------ */
char *units_converted_stdin_constant_spelling(
			char *units_name );

boolean units_converted_stdin_temperature_boolean(
			char *units_name );

boolean units_converted_stdin_navd88_boolean(
			char *units_converted_stdin_navd88_name,
			char *units_converted_name );

DICTIONARY *units_converted_stdin_station_navd88_dictionary(
			char *appaserver_station_table,
			char *appaserver_navd88_attribute );

/* Driver */
/* ------ */
void units_converted_stdin_execute(
			UNITS_CONVERTED_STDIN *units_converted_stdin );

/* Process */
/* ------- */

typedef struct
{
	char *value_string;
	double old_value;
	char *station_string;
	double new_value;
	char *output_string;
} UNITS_CONVERTED_ROW;

/* Usage */
/* ----- */
UNITS_CONVERTED_ROW *units_converted_row_calloc(
			void );

/* Process */
/* ------- */

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
UNITS_CONVERTED_ROW *units_converted_row_parse(
			char *input /* in/out */,
			UNITS_CONVERTED_ROW *units_converted_row /* in/out */,
			UNITS_CONVERTED_STDIN *units_converted_stdin );

/* Process */
/* ------- */

/* Returns static memory or null */
/* ----------------------------- */
char *units_converted_row_value_string(
			int value_piece,
			char delimiter,
			char *input );

double units_converted_row_old_value(
			char *units_converted_row_value_string );

/* Returns static memory or null */
/* ----------------------------- */
char *units_converted_row_station_string(
			int station_piece,
			char delimiter,
			char *input );

double units_converted_row_temperature_new_value(
			char *units_name,
			char *units_converted_name,
			double units_converted_row_old_value );

double units_converted_row_new_value(
			double multiply_by,
			double units_converted_row_old_value );

/* Usage */
/* ----- */
double units_converted_row_navd88_new_value(
			DICTIONARY *station_navd88_dictionary,
			double units_converted_row_old_value,
			char *units_converted_row_station_string );

/* Process */
/* ------- */

/* Usage */
/* ----- */

/* Returns input */
/* ------------- */
char *units_converted_row_output_string(
			char *input /* in/out */,
			int value_piece,
			char delimiter,
			double new_value );

/* Process */
/* ------- */

#define UNITS_ALIAS_SELECT		"units_alias,"	\
					"units"

#define UNITS_ALIAS_TABLE		"units_alias"

typedef struct
{
	char *units_alias_name;
	char *units_name;
} UNITS_ALIAS;

/* Usage */
/* ----- */
LIST *units_alias_list( void );

/* Process */
/* ------- */

/* Usage */
/* ----- */
UNITS_ALIAS *units_alias_parse(
			char *input );

/* Process */
/* ------- */
UNITS_ALIAS *units_alias_calloc(
			void );

/* Usage */
/* ----- */
UNITS_ALIAS *units_alias_seek(
			char *units_alias_name,
			LIST *units_alias_list );

/* Process */
/* ------- */

#define UNITS_TABLE			"units"

typedef struct
{
	char *units_name;
	LIST *units_converted_list;
} UNITS;

/* Usage */
/* ----- */
LIST *units_list(	boolean fetch_converted_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
UNITS *units_new(	char *units_name,
			boolean fetch_converted_list );

/* Process */
/* ------- */
UNITS *units_calloc(	void );

/* Returns static memory */
/* --------------------- */
char *units_primary_where(
			char *units_name );

#endif
