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
	boolean lock_transaction;
} TRANSACTION;

/* Usage */
/* ----- */
LIST *transaction_list(
			char *where,
			boolean fetch_journal_ledger );

/* Usage */
/* ----- */
TRANSACTION *transaction_fetch(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			boolean fetch_journal_ledger );

/* Process */
/* ------- */

TRANSACTION *transaction_parse(
			char *input,
			boolean fetch_journal_ledger );

TRANSACTION *transaction_new(
			char *full_name,
			char *street_address,
			char *transaction_date_time );

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
			char *debit_account,
			char *credit_account );

/* Process */
/* ------- */

/* Usage */
/* ----- */

/* Returns inserted transaction_date_time */
/* -------------------------------------- */
char *transaction_insert(
			char *appaserver_error_filename,
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
char *transaction_race_free_date_time(
			char *transaction_date_time );

FILE *transaction_insert_pipe_open(
			char *appaserver_error_filename,
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
void transaction_update(
			double transaction_amount,
			char *transaction_primary_where );

/* Process */
/* ------- */

/* Returns static memory or null */
/* ----------------------------- */
char *transaction_set_clause(
			char *transaction_amount_column,
			double transaction_amount );

/* Returns heap memory or null */
/* --------------------------- */
char *transaction_update_statement(
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

/* Returns as_of_date, heap memory, or null */
/* ---------------------------------------- */
char *transaction_as_of_date(
			char *transaction_table,
			char *as_of_date );

/* Process */
/* ------- */
boolean transaction_as_of_date_populated(
			char *as_of_date );

/* Usage */
/* ----- */
char *transaction_begin_date_string(
			char *transaction_table,
			char *transaction_as_of_date );

/* Process */
/* ------- */

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

/* Private */
/* ------- */
TRANSACTION *transaction_calloc(
			void );

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

/* Returns heap memory */
/* ------------------- */
char *transaction_raw_display(
			TRANSACTION *transaction );

boolean transaction_date_time_changed(
			char *preupdate_transaction_date_time );

boolean transaction_date_time_exists(
			char *transaction_table,
			char *transaction_date_time_column,
			char *transaction_date_time );

/* Returns heap memory or null */
/* --------------------------- */
char *transaction_minimum_transaction_date_string(
			char *transaction_table );

typedef struct
{
	char *transaction_system_string;
	FILE *input_pipe;
	LIST *list;
} TRANSACTION_LIST;

/* Usage */
/* ----- */
TRANSACTION_LIST *transaction_list_fetch(
			char *where,
			boolean fetch_journal_ledger );

/* Process */
/* ------- */

/* Usage */
/* ----- */
TRANSACTION_LIST *transaction_list_new(
			LIST *list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
void transaction_list_insert(
			TRANSACTION_LIST *transaction_list,
			char *appaserver_error_filename );

/* Process */
/* ------- */

/* Usage */
/* ----- */
void transaction_list_html_display(
			TRANSACTION_LIST *transaction_list );

/* Process */
/* ------- */

/* Private */
/* ------- */
TRANSACTION_LIST *transaction_list_calloc(
			void );

/* Public */
/* ------ */
TRANSACTION *transaction_list_check_seek(
			int check_number,
			TRANSACTION_LIST *transaction_list );

TRANSACTION *transaction_list_entity_seek(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			TRANSACTION_LIST *transaction_list );

LIST *transaction_list_account_name_list(
			TRANSACTION_LIST *transaction_list );

#endif
