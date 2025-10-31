/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/feeder_load_event.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FEEDER_LOAD_EVENT_H
#define FEEDER_LOAD_EVENT_H

#include "list.h"
#include "boolean.h"
#include "date.h"
#include "appaserver_user.h"

#define FEEDER_LOAD_EVENT_TABLE		"feeder_load_event"

#define FEEDER_LOAD_EVENT_SELECT	"feeder_account,"	\
					"feeder_load_date_time,"\
					"full_name,"		\
					"street_address,"	\
					"feeder_load_filename,"	\
					"account_end_date,"	\
					"account_end_balance"

#define FEEDER_LOAD_EVENT_INSERT	"feeder_account,"	\
					"feeder_load_filename,"	\
					"account_end_date,"	\
					"account_end_balance,"	\
					"feeder_load_date_time,"\
					"full_name,"		\
					"street_address"

typedef struct
{
	char *feeder_account_name;
	char *exchange_format_filename;
	char *feeder_row_account_end_date;
	double feeder_row_account_end_balance;
	char *feeder_load_date_time;
	APPASERVER_USER *appaserver_user;
} FEEDER_LOAD_EVENT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FEEDER_LOAD_EVENT *feeder_load_event_new(
		char *login_name,
		char *feeder_account_name,
		char *exchange_format_filename,
		char *feeder_row_account_end_date,
		double feeder_row_account_end_balance,
		char *feeder_load_date_time );

/* Process */
/* ------- */
FEEDER_LOAD_EVENT *feeder_load_event_calloc(
		void );

/* Usage */
/* ----- */
FEEDER_LOAD_EVENT *feeder_load_event_fetch(
		char *feeder_account_name,
		char *feeder_load_date_time );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *feeder_load_event_primary_where(
		char *feeder_account_name,
		char *feeder_load_date_time );

/* Returns heap memory */
/* ------------------- */
char *feeder_load_event_system_string(
		const char *feeder_load_event_select,
		const char *feeder_load_event_table,
		char *feeder_load_event_primary_where );

/* Usage */
/* ----- */
FEEDER_LOAD_EVENT *feeder_load_event_parse(
		char *input );

/* Usage */
/* ----- */
FEEDER_LOAD_EVENT *
	feeder_load_event_latest_fetch(
		const char *feeder_load_event_table,
		char *feeder_account_name );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *feeder_load_event_account_where(
		char *feeder_account_name );

/* Returns heap memory */
/* ------------------- */
char *feeder_load_event_latest_system_string(
		const char *feeder_load_event_table,
		char *feeder_load_event_account_where );

/* --------------------------- */
/* Returns heap memory or null */
/* --------------------------- */
char *feeder_load_event_latest_date_time(
		char *feeder_load_event_latest_system_string );

/* Usage */
/* ----- */
void feeder_load_event_insert(
		const char *feeder_load_event_table,
		const char *feeder_load_event_insert,
		char *feeder_account_name,
		char *feeder_load_filename,
		char *feeder_row_account_end_date,
		double feeder_row_account_end_balance,
		char *feeder_load_date_time,
		char *full_name,
		char *street_address );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *feeder_load_event_insert_system_string(
		const char *feeder_load_event_table,
		const char *feeder_load_event_insert,
		const char sql_delimiter );

void feeder_load_event_insert_pipe(
		const char sql_delimiter,
		char *feeder_account_name,
		char *feeder_load_filename,
		char *feeder_row_account_end_date,
		double feeder_row_account_end_balance,
		char *feeder_load_date_time,
		char *full_name,
		char *street_address,
		FILE *appaserver_output_pipe );

/* Usage */
/* ----- */
double feeder_load_event_prior_account_end_balance(
		const char *feeder_load_event_table,
		char *feeder_account_name,
		boolean account_accumulate_debit_boolean );

#endif
