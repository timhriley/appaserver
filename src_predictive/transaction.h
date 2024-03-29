/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/transaction.h			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "list.h"
#include "date.h"
#include "boolean.h"
#include "entity.h"

#define TRANSACTION_TABLE			"transaction"
#define TRANSACTION_MEMO_LENGTH			60
#define TRANSACTION_LOCK_Y			'y'
#define TRANSACTION_CLOSING_ENTRY_MEMO		"close closing"
#define TRANSACTION_SEMAPHORE_KEY		12227
#define TRANSACTION_PRECLOSE_TIME		"23:59:58"
#define TRANSACTION_CLOSE_TIME			"23:59:59"

#define TRANSACTION_SELECT			"full_name,"		\
						"street_address,"	\
						"transaction_date_time,"\
						"transaction_amount,"	\
						"check_number,"		\
						"memo,"			\
						"lock_transaction_yn"

#define TRANSACTION_DATE_TIME_COLUMN		"transaction_date_time"
#define TRANSACTION_AMOUNT_COLUMN		"transaction_amount"

typedef struct
{
	char *full_name;
	char *street_address;
	char *transaction_date_time;
	double transaction_amount;
	char *memo;
	int check_number;
	LIST *journal_list;
	char lock_transaction_yn;
	char *rental_property_street_address;
} TRANSACTION;

/* Usage */
/* ----- */
LIST *transaction_list(
			char *where,
			boolean fetch_journal_ledger );

/* Process */
/* ------- */
FILE *transaction_input_pipe(
			char *transaction_system_string );

/* Usage */
/* ----- */
TRANSACTION *transaction_fetch(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			boolean fetch_journal_list );

/* Process */
/* ------- */

TRANSACTION *transaction_parse(
			char *input,
			boolean fetch_journal_list );

TRANSACTION *transaction_new(
			char *full_name,
			char *street_address,
			char *transaction_date_time );

TRANSACTION *transaction_calloc(
			void );

/* Usage */
/* ----- */
TRANSACTION *transaction_entity_new(
			ENTITY *entity,
			char *transaction_date_time,
			double transaction_amount,
			int check_number,
			char *memo,
			LIST *journal_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
TRANSACTION *transaction_binary(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double transaction_amount,
			char *memo,
			char *debit_account_name,
			char *credit_account_name );

/* Process */
/* ------- */

/* Usage */
/* ----- */

/* Returns first_transaction_date_time */
/* ----------------------------------- */
char *transaction_list_insert(
			LIST *transaction_list,
			boolean insert_journal_list_boolean );

/* Process */
/* ------- */

/* Usage */
/* ----- */

/* Returns inserted transaction_date_time */
/* -------------------------------------- */
char *transaction_insert(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double transaction_amount,
			int check_number,
			char *memo,
			char lock_transaction_yn,
			LIST *journal_list,
			boolean insert_journal_list_boolean );

/* Process */
/* ------- */
char *transaction_race_free_date_time(
			char *transaction_date_time );

FILE *transaction_insert_pipe_open(
			char *transaction_select,
			char *transaction_table );

/* Returns static memory */
/* --------------------- */
char *transaction_check_number(
			int check_number );

/* Returns static memory */
/* --------------------- */
char *transaction_memo( char *memo );

/* Returns static memory */
/* --------------------- */
char *transaction_lock(	char transaction_lock_yn );

void transaction_insert_pipe(
			FILE *pipe_open,
			char *full_name,
			char *street_address,
			char *transaction_race_free_date_time,
			double transaction_amount,
			char *transaction_check_number,
			char *transaction_memo,
			char *transaction_lock );

/* Usage */
/* ----- */
char *transaction_stamp_insert(
			TRANSACTION *transaction,
			boolean insert_journal_list_boolean );

/* Process */
/* ------- */

/* Usage */
/* ----- */
void transaction_delete(char *full_name,
			char *street_address,
			char *transaction_date_time );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *transaction_delete_system_string(
			char *transaction_table,
			char *transaction_primary_where );

/* Usage */
/* ----- */
boolean transaction_closing_entry_exists(
			char *transaction_table,
			char *transaction_close_time,
			char *transaction_as_of_date );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *transaction_closing_memo_where(
			char *transaction_closing_entry_memo );

/* Returns heap memory */
/* ------------------- */
char *transaction_closing_entry_system_string(
			char *transaction_table,
			char *transaction_closing_entry_where );

/* Usage */
/* ----- */
void transaction_fetch_update(
			char *full_name,
			char *street_address,
			char *transaction_date_time );

/* Process */
/* ------- */

/* Usage */
/* ----- */
void transaction_update(
			double journal_list_transaction_amount,
			char *transaction_primary_where );

/* Process */
/* ------- */

/* Returns static memory or null */
/* ----------------------------- */
char *transaction_set_clause(
			char *transaction_amount_column,
			double journal_list_transaction_amount );

/* Returns heap memory */
/* ------------------- */
char *transaction_update_statement_system_string(
			char *transaction_table,
			char *transaction_set_clause,
			char *transaction_primary_where );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *transaction_date_time_where(
			char *transaction_date_time_column,
			char *transaction_date,
			char *transaction_time );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *transaction_date_time(
			char *transaction_date,
			char *transaction_time );

/* Usage */
/* ----- */

/* Returns as_of_date_string, heap memory, or null */
/* ----------------------------------------------- */
char *transaction_as_of_date(
			char *transaction_table,
			char *as_of_date_string );

/* Process */
/* ------- */
boolean transaction_as_of_date_populated(
			char *as_of_date_string );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *transaction_begin_date_string(
			char *transaction_table,
			char *transaction_as_of_date );

/* Process */
/* ------- */

/* Usage */
/* ----- */
TRANSACTION *transaction_date_time_fetch(
			char *transaction_date_time,
			boolean fetch_journal_list );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *transaction_date_time_fetch_where(
			char *transaction_date_time );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *transaction_date_max(
			char *transaction_table );

/* Process */
/* ------- */

/* Returns heap memory or null */
/* --------------------------- */
char *transaction_date_time_max(
			char *transaction_table );

/* Usage */
/* ----- */
DATE *transaction_prior_closing_transaction_date(
			char *transaction_close_time,
			char *transaction_closing_entry_memo,
			char *transaction_table,
			char *transaction_as_of_date );

/* Process */
/* ------- */

/* Usage */
/* ----- */
void transaction_journal_list_insert(
			LIST *transaction_list,
			boolean with_propagate );

/* Process */
/* ------- */

/* Usage */
/* ----- */
LIST *transaction_list_extract_account_list(
			LIST *transaction_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
void transaction_list_html_display(
			LIST *transaction_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
void transaction_html_display(
			TRANSACTION *transaction );

/* Process */
/* ------- */

/* Usage */
/* ----- */
char *transaction_refresh(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double transaction_amount,
			int check_number,
			char *memo,
			char lock_transaction_yn,
			LIST *journal_list );

/* Process */
/* ------- */

/* Public */
/* ------ */

/* Returns static memory */
/* --------------------- */
char *transaction_primary_where(
			char *full_name,
			char *street_address,
			char *transaction_date_time );

/* Returns heap memory */
/* ------------------- */
char *transaction_system_string(
			char *transaction_select,
			char *transaction_table,
			char *transaction_primary_where );

/* Returns static memory */
/* --------------------- */
char *transaction_date_time_closing(
			char *transaction_preclose_time,
			char *transaction_close_time,
			char *transaction_as_of_date,
			boolean preclose_time_boolean );

boolean transaction_date_time_changed(
			char *preupdate_transaction_date_time );

char *transaction_date_time_earlier(
			char *transaction_date_time,
			char *preupdate_transaction_date_time );

boolean transaction_date_time_exists(
			char *transaction_table,
			char *transaction_date_time_column,
			char *transaction_date_time );

/* Returns heap memory or null */
/* --------------------------- */
char *transaction_minimum_transaction_date_string(
			char *transaction_table );

/* ------------------------------------ */
/* Returns heap memory.			*/
/* Increments second each invocation.   */
/* ------------------------------------ */
char *transaction_increment_date_time(
			char *transaction_date );

/* Returns heap memory or null */
/* --------------------------- */
char *transaction_date_time_memo_latest(
			char *transaction_table,
			char *memo,
			char *full_name,
			char *street_address );

#endif
