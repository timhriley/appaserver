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
} FEEDER_LOAD_EVENT;

/* Usage */
/* ----- */
FEEDER_LOAD_EVENT *feeder_load_event =
	feeder_load_event_new(
			char *feeder_load_date_time,
			char *login_name,
			char *filename,
			char *feeder_account,
			double feeder_ending_balance );

/* Process */
/* ------- */

typedef struct
{
	FEEDER_LOAD_EVENT *feeder_load_event;
	LIST *feeder_phrase_list;
} FEEDER_LOAD;

/* Usage */
/* ----- */
FEEDER_LOAD *feeder_load_new(
			char *process_name,
			char *login_name,
			char *feeder_account,
			char *filename,
			int date_column /* one_based */,
			int description_column /* one_based */,
			int debit_column /* one_based */,
			int credit_column /* one_based */,
			int balance_column /* one_based */,
			boolean reverse_order,
			double feeder_ending_balance );

/* Process */
/* ------- */

#endif
