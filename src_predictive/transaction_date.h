/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/transaction_date.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and  available software. Visit appaserver.org		*/
/* -------------------------------------------------------------------- */

#ifndef TRANSACTION_DATE_H
#define TRANSACTION_DATE_H

#include "list.h"
#include "boolean.h"
#include "date.h"

#define TRANSACTION_DATE_AS_OF_DATE_FILLER			\
				"as_of_date"

#define TRANSACTION_DATE_REVERSE_DATE_FILLER			\
				"reverse_date"

#define TRANSACTION_DATE_PRECLOSE_TIME		"23:59:58"
#define TRANSACTION_DATE_CLOSE_TIME		"23:59:59"
#define TRANSACTION_DATE_REVERSE_TIME		"00:00:00"

typedef struct
{
	/* Stub */
} TRANSACTION_DATE;

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *transaction_date_time_string(
		char *date_string,
		char *time_string );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *transaction_date_end_date_time_string(
	char *end_date_string,
	char *time_string );

/* Usage */
/* ----- */

/* Returns as_of_date_string, heap memory, or null */
/* ----------------------------------------------- */
char *transaction_date_as_of(
		const char *transaction_table,
		char *as_of_date_string );

/* Usage */
/* ----- */
boolean transaction_date_close_boolean(
		const char *transaction_table,
		const char *transaction_date_close_time,
		const char *transaction_close_memo,
		char *transaction_date_as_of );

/* Usage */
/* ----- */
boolean transaction_date_reverse_boolean(
		const char *transaction_table,
		const char *transaction_date_reverse_time,
		const char *transaction_reverse_memo,
		char *reverse_date_string );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *transaction_date_where_string(
		const char *memo,
		char *date_string,
		char *time_string );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *transaction_date_begin_date_string(
		const char *transaction_table,
		char *transaction_as_of_date );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *transaction_date_close_date_time_string(
		const char *transaction_date_preclose_time,
		const char *transaction_date_close_time,
		char *transaction_date_as_of,
		boolean preclose_time_boolean );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *transaction_date_reverse_date_time_string(
		const char *transaction_date_reverse_time,
		char *reverse_date_string );

/* Usage */
/* ----- */
DATE *transaction_date_prior_close_date(
		const char *transaction_date_close_time,
		const char *transaction_close_memo,
		const char *transaction_table,
		char *transaction_date_as_of );

/* Usage */
/* ----- */
boolean transaction_date_time_changed(
		char *preupdate_transaction_date_time );

/* Usage */
/* ----- */
char *transaction_date_earlier_date_time(
		char *transaction_date_time,
		char *preupdate_transaction_date_time );

/* Usage */
/* ----- */
boolean transaction_date_time_boolean(
		const char *transaction_table,
		char *transaction_date_time );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *transaction_date_time_where_string(
		char *transaction_date_time );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *transaction_date_time(
		char *transaction_date_string,
		char *transaction_time_string );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *transaction_date_minimum_string(
		const char *transaction_table );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *transaction_date_time_maximum_string(
		const char *transaction_table,
		char *where_string );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *transaction_date_time_memo_maximum_string(
		const char *transaction_table,
		const char *transaction_memo_string );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *transaction_date_count_system_string(
		const char *transaction_table,
		char *where_string );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *transaction_date_prior_end_date_time_string(
		char *transaction_date_begin_date_string );

typedef struct
{
	char *transaction_date_time_memo_maximum_string;
} TRANSACTION_DATE_CLOSE_NOMINAL_UNDO;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
TRANSACTION_DATE_CLOSE_NOMINAL_UNDO *
	transaction_date_close_nominal_undo_new(
		const char *transaction_table,
		const char *transaction_close_memo );

/* Process */
/* ------- */
TRANSACTION_DATE_CLOSE_NOMINAL_UNDO *
	transaction_date_close_nominal_undo_calloc(
		void );

typedef struct
{
	boolean transaction_date_close_boolean;
	char *transaction_date_close_date_time_string;
} TRANSACTION_DATE_CLOSE_NOMINAL_DO;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
TRANSACTION_DATE_CLOSE_NOMINAL_DO *
	transaction_date_close_nominal_do_new(
		char *as_of_date_string );

/* Process */
/* ------- */
TRANSACTION_DATE_CLOSE_NOMINAL_DO *
	transaction_date_close_nominal_do_calloc(
		void );

typedef struct
{
	char *transaction_date_time_memo_maximum_string;
} TRANSACTION_DATE_REVERSE_NOMINAL_UNDO;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
TRANSACTION_DATE_REVERSE_NOMINAL_UNDO *
	transaction_date_reverse_nominal_undo_new(
		const char *transaction_table,
		const char *transaction_reverse_memo );

/* Process */
/* ------- */
TRANSACTION_DATE_REVERSE_NOMINAL_UNDO *
	transaction_date_reverse_nominal_undo_calloc(
		void );

typedef struct
{
	boolean transaction_date_reverse_boolean;
	char *transaction_date_reverse_date_time_string;
} TRANSACTION_DATE_REVERSE_NOMINAL_DO;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
TRANSACTION_DATE_REVERSE_NOMINAL_DO *
	transaction_date_reverse_nominal_do_new(
		char *reverse_date_string );

/* Process */
/* ------- */
TRANSACTION_DATE_REVERSE_NOMINAL_DO *
	transaction_date_reverse_nominal_do_calloc(
		void );

typedef struct
{
	char *transaction_date_as_of;
	char *transaction_date_begin_date_string;
	boolean transaction_date_close_boolean;
	char *end_date_time_string;
	char *prior_end_date_time_string;
} TRANSACTION_DATE_STATEMENT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
TRANSACTION_DATE_STATEMENT *
	transaction_date_statement_new(
		char *as_of_date_string );

/* Process */
/* ------- */
TRANSACTION_DATE_STATEMENT *
	transaction_date_statement_calloc(
		void );

typedef struct
{
	char *transaction_date_as_of;
	char *transaction_date_begin_date_string;
	boolean transaction_date_close_boolean;
	char *preclose_end_date_time_string;
	char *end_date_time_string;
} TRANSACTION_DATE_TRIAL_BALANCE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
TRANSACTION_DATE_TRIAL_BALANCE *
	transaction_date_trial_balance_new(
		char *as_of_date_string );

/* Process */
/* ------- */
TRANSACTION_DATE_TRIAL_BALANCE *
	transaction_date_trial_balance_calloc(
		void );

#endif
