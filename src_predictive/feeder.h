/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/feeder.h				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef FEEDER_H
#define FEEDER_H

#include "list.h"
#include "boolean.h"
#include "date.h"

#define FEEDER_PHRASE_TABLE		"feeder_phrase"

#define FEEDER_PHRASE_SELECT		"feeder_phrase,"		\
					"nominal_account,"		\
					"full_name,"			\
					"street_address,"		\
					"feeder_phrase_ignore_yn"

#define FEEDER_KEYS_MATCH_SUM_MAX 	18
#define FEEDER_DESCRIPTION_SIZE		140

typedef struct
{
	int line_number;
	char *american_date;
	char *international_date;
	char *description;
	double amount;
	double debit;
	double credit;
	double balance;
	char *description_embedded;
} FEEDER_LOAD_ROW;

/* Usage */
/* ----- */
FEEDER_LOAD_ROW *feeder_load_row_new(
			int date_column /* one_based */,
			int description_column /* one_based */,
			int debit_column /* one_based */,
			int credit_column /* one_based */,
			int balance_column /* one_based */,
			char *input,
			int line_number );

/* Process */
/* ------- */
FEEDER_LOAD_ROW *feeder_load_row_calloc(
			void );

int feeder_load_row_line_number(
			int line_number );

/* Returns heap memory or null */
/* --------------------------- */
char *feeder_load_row_international_date(
			char *american_date );

double feeder_load_row_amount(
			double debit,
			double credit );

/* Returns heap memory */
/* ------------------- */
char *feeder_load_row_description_embedded(
			char *description,
			int line_number );

/* Returns static memory */
/* --------------------- */
char *feeder_load_row_trim_bank_date(
			char *description );

/* Returns heap memory */
/* ------------------- */
char *feeder_load_row_description_build(
			char *sed_trim_double_spaces,
			int line_number );

/* Returns feeder_load_row_description_build */
/* ----------------------------------------- */
char *feeder_load_row_description_crop(
			char *feeder_load_row_description_build,
			int feeder_description_size );

/* Public */
/* ------ */
double feeder_load_row_last_running_balance(
			boolean reverse_order,
			LIST *feeder_load_row_list );

typedef struct
{
	int line_count;
	int begin_sequence_number;
	LIST *feeder_load_row_list;
} FEEDER_LOAD_FILE;

/* Usage */
/* ----- */
FEEDER_LOAD_FILE *feeder_load_file_fetch(
			char *feeder_load_filename,
			int date_column /* one_based */,
			int description_column /* one_based */,
			int debit_column /* one_based */,
			int credit_column /* one_based */,
			int balance_column /* one_based */,
			boolean reverse_order );

/* Process */
/* ------- */
FEEDER_LOAD_FILE *feeder_load_file_calloc(
			void );

int feeder_load_file_line_count(
			char *feeder_load_filename,
			int date_piece_offset );

int feeder_load_file_begin_sequence_number(
			int feeder_load_file_line_count ); 

typedef struct
{
	char *feeder_phrase;
	char *nominal_account;
	char *full_name;
	char *street_address;
	boolean feeder_phrase_ignore;
} FEEDER_PHRASE;

/* Usage */
/* ----- */
LIST *feeder_phrase_list(
			void );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *feeder_phrase_system_string(
			char *feeder_phrase_select,
			char *feeder_phrase_table );

/* Usage */
/* ----- */
FEEDER_PHRASE *feeder_phrase_parse(
			char *input );

/* Process */
/* ------- */
FEEDER_PHRASE *feeder_phrase_new(
			char *feeder_phrase );

FEEDER_PHRASE *feeder_phrase_calloc(
			void );

typedef struct
{
	char *feeder_load_date_time;
	char *login_name;
	char *basename_filename;
	char *feeder_account;
	double ending_balance;
	char *sha256sum;
} FEEDER_LOAD_EVENT;

/* Usage */
/* ----- */
FEEDER_LOAD_EVENT *feeder_load_event_new(
			char *feeder_load_date_time,
			char *login_name,
			char *basename_filename,
			char *feeder_account,
			double ending_balance,
			double feeder_load_row_last_running_balance,
			char *feeder_load_sha256sum );

/* Process */
/* ------- */
FEEDER_LOAD_EVENT *feeder_load_event_calloc(
			void );

double feeder_load_event_ending_balance(
			double ending_balance,
			double feeder_load_row_last_running_balance );

/* Driver */
/* ------ */
void feeder_load_event_insert(
			char *feeder_load_event_table,
			char *feeder_load_select,
			char *feeder_load_date_time,
			char *login_name,
			char *basename_filename,
			char *sha256sum,
			char *feeder_account,
			double ending_balance );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *feeder_load_event_insert_system_string(
			char *feeder_load_event_table,
			char *feeder_load_select,
			char sql_delimiter );

void feeder_load_event_insert_pipe(
			FILE *insert_pipe,
			char sql_delimiter,
			char *feeder_load_date_time,
			char *login_name,
			char *basename_filename,
			char *sha256sum,
			char *feeder_account,
			double ending_balance );

typedef struct
{
	FEEDER_LOAD_FILE *feeder_load_file;
	LIST *feeder_phrase_list;
	double feeder_load_row_last_running_balance;
	char *basename_filename;
	char *sha256sum;
	FEEDER_LOAD_EVENT *feeder_load_event;
} FEEDER_LOAD;

/* Usage */
/* ----- */
FEEDER_LOAD *feeder_load_new(
			char *process_name,
			char *login_name,
			char *feeder_account,
			char *feeder_load_filename,
			int date_column /* one_based */,
			int description_column /* one_based */,
			int debit_column /* one_based */,
			int credit_column /* one_based */,
			int balance_column /* one_based */,
			boolean reverse_order,
			double ending_balance );

/* Process */
/* ------- */
FEEDER_LOAD *feeder_load_calloc(
			void );

/* Returns heap memory */
/* ------------------- */
char *feeder_load_basename_filename(
			char *feeder_load_filename );


/* Returns heap memory */
/* ------------------- */
char *feeder_load_sha256sum(
			char *feeder_load_filename );

#endif
