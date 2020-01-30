/* $APPASERVER_HOME/src_hydrology/measurement.h */
/* -------------------------------------------- */

#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <stdio.h>
#include <stdlib.h>
#include "dictionary.h"
#include "julian.h"
#include "boolean.h"

#define MEASUREMENT_QUEUE_TOP_BOTTOM_LINES	50

#define MEASUREMENT_SELECT_LIST	 	"station,datatype,measurement_date,measurement_time,measurement_value"

#define MEASUREMENT_INSERT_LIST	 	"station,datatype,measurement_date,measurement_time,measurement_value"

typedef struct
{
	double measurement_update;
} MEASUREMENT_UPDATE;

typedef struct
{
	char *station_name;
	char *datatype;
	char *measurement_date;
	char *measurement_time;
	char *value_string;
	double measurement_value;
	boolean null_value;
	double delta_value;
	MEASUREMENT_UPDATE *measurement_update;
} MEASUREMENT;

typedef struct
{
	char *application_name;
	char *load_process;
	MEASUREMENT *measurement;
	FILE *insert_pipe;
	FILE *input_pipe;
	FILE *delete_pipe;
	FILE *html_table_pipe;
	char *argv_0;
} MEASUREMENT_STRUCTURE;

/* Constants */
/* --------- */
#define STATION_PIECE		0
#define DATATYPE_PIECE		1
#define DATE_PIECE		2
#define TIME_PIECE		3
#define VALUE_PIECE		4

/* Prototypes */
/* ---------- */
MEASUREMENT_UPDATE *measurement_update_new(
					void );

MEASUREMENT_STRUCTURE *measurement_structure_new(
					char *application_name );

FILE *measurement_open_input_pipe(	char *application_name,
					char *where_clause,
					char delimiter );

void measurement_free(			MEASUREMENT *m );

/* This function does strdup() for the memory. */
/* ------------------------------------------- */
MEASUREMENT *measurement_strdup_new(
					char *station_name,
					char *datatype,
					char *date,
					char *time,
					char *value_string );

boolean measurement_set_delimited_record(
					MEASUREMENT_STRUCTURE *m, 
					char *comma_delimited_record,
					char delimiter );

void measurement_html_display( 		MEASUREMENT_STRUCTURE *m,
					FILE *html_table_pipe );

void measurement_insert( 		MEASUREMENT_STRUCTURE *m );

/*
double measurement_get_value_from_db(	int *record_exists,
					int *db_null_value,
					char *application_name,
					char *station_name,
					char *datatype,
					char *date,
					char *time );
*/

void measurement_output_insert_pipe(	FILE *insert_pipe,
					char *station_name,
					char *datatype,
					char *date,
					char *time,
					double value,
					boolean null_value );

FILE *measurement_open_insert_pipe(	char *application_name,
					boolean replace );

FILE *measurement_open_html_table_pipe(	void );

double measurement_get_value(		boolean *null_value,
					char *value_string );

void measurement_set_load_process( 	MEASUREMENT *m,
					char *load_process,
					boolean execute );

void measurement_set_argv_0(		MEASUREMENT_STRUCTURE *m,
					char *argv_0 );

boolean measurement_structure_fetch(	MEASUREMENT_STRUCTURE *m,
					FILE *input_pipe );

FILE *measurement_open_delete_pipe(	char *application_name );

/* Returns static memory */
/* --------------------- */
char *measurement_display(		MEASUREMENT *m );

/* Returns static memory */
/* --------------------- */
char *measurement_display_delimiter(	MEASUREMENT *m,
					char delimiter );

void measurement_delete(		FILE *delete_pipe,
					MEASUREMENT *m );

void measurement_update(		char *application_name,
					char *station_name,
					char *datatype,
					char *date,
					char *time,
					double value );

MEASUREMENT *measurement_calloc(	void );

LIST *measurement_fetch_list(		FILE *input_pipe,
					char delimiter );

MEASUREMENT *measurement_parse(		char *buffer,
					char delimiter );

boolean measurement_list_update(	char *application_name,
					LIST *measurement_list );

MEASUREMENT *measurement_list_seek(	char *station_name  /* optional */,
					char *datatype_name /* optional */,
					char *measurement_date,
					char *measurement_time,
					LIST *measurement_list );

void measurement_text_output(		MEASUREMENT *m,
					char delimiter );

void measurement_change_text_output(	LIST *measurement_list,
					char delimiter );

MEASUREMENT *measurement_variable_parse(
					char *buffer,
					char delimiter,
					LIST *order_integer_list );

void measurement_non_execute_display(
					MEASUREMENT_STRUCTURE *m,
					FILE *html_table_pipe );

JULIAN *measurement_adjust_time_to_sequence(
					JULIAN *measurement_date_time,
					char *sequence_list_string );

void measurement_text_output(		MEASUREMENT *measurement,
					char delimiter );

#endif
