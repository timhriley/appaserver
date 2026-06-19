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
					"feeder_load_filename,"	\
					"account_end_date,"	\
					"account_end_balance"

#define FEEDER_LOAD_EVENT_INSERT	"feeder_account,"	\
					"feeder_load_filename,"	\
					"account_end_date,"	\
					"account_end_balance,"	\
					"feeder_load_date_time,"\
					"full_name"

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

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *feeder_load_event_system_string(
		const char *feeder_load_event_select,
		const char *feeder_load_event_table,
		const char *entity_contact_key_column,
		char *feeder_load_event_primary_where,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *feeder_load_event_select_string(
		const char *feeder_load_event_select,
		const char *entity_contact_key_column,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */
FEEDER_LOAD_EVENT *feeder_load_event_parse(
		boolean entity_contact_key_boolean,
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
		const char *feeder_load_event_insert,
		const char *feeder_load_event_table,
		char *feeder_account_name,
		char *feeder_load_filename,
		char *feeder_row_account_end_date,
		double feeder_row_account_end_balance,
		char *feeder_load_date_time,
		char *full_name,
		char *contact_key,
		boolean event_contact_key_boolean );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *feeder_load_event_insert_string(
		const char sql_delimiter,
		char *feeder_account_name,
		char *feeder_load_filename,
		char *feeder_row_account_end_date,
		double feeder_row_account_end_balance,
		char *feeder_load_date_time,
		char *full_name,
		char *contact_key,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *feeder_load_event_insert_system_string(
		const char *feeder_load_event_insert,
		const char *feeder_load_event_table,
		const char *entity_contact_key_column,
		const char sql_delimiter,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */
double feeder_load_event_prior_account_end_balance(
		const char *feeder_load_event_table,
		char *feeder_account_name );

/* Usage */
/* ----- */
double feeder_load_event_error_double(
		double exchange_journal_begin_amount,
		LIST *feeder_row_list,
		FEEDER_LOAD_EVENT *feeder_load_event_latest_fetch );

/* Process */
/* ------- */
double feeder_load_event_calculate_begin_amount(
		double feeder_row_account_end_balance,
		double feeder_row_exist_sum );

#endif
