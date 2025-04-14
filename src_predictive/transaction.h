/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/transaction.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and  available software. Visit appaserver.org		*/
/* -------------------------------------------------------------------- */

#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "list.h"
#include "date.h"
#include "boolean.h"
#include "entity.h"

#define TRANSACTION_TABLE			"transaction"
#define TRANSACTION_MEMO_LENGTH			60
#define TRANSACTION_SEMAPHORE_KEY		12227

#define TRANSACTION_OPEN_MEMO			"Opening entry"
#define TRANSACTION_CLOSE_MEMO			"Closing entry"
#define TRANSACTION_BEGIN_TIME			"00:00:00"
#define TRANSACTION_DATE_PRECLOSE_TIME		"23:59:58"
#define TRANSACTION_DATE_CLOSE_TIME		"23:59:59"

#define TRANSACTION_DATE_TIME_COLUMN		"transaction_date_time"
#define TRANSACTION_AMOUNT_COLUMN		"transaction_amount"

#define TRANSACTION_SELECT	"full_name,"			\
				"street_address,"		\
				"transaction_date_time,"	\
				"transaction_amount,"		\
				"check_number"

#define TRANSACTION_INSERT	"full_name,"			\
				"street_address,"		\
				"transaction_date_time,"	\
				"transaction_amount,"		\
				"check_number,"			\
				"memo"

#define TRANSACTION_LOCK_INSERT	"full_name,"			\
				"street_address,"		\
				"transaction_date_time,"	\
				"transaction_amount,"		\
				"check_number,"			\
				"memo,"				\
				"transaction_lock_yn"

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
		char *entity_self_full_name,
		char *entity_self_street_address );

/* Process */
/* ------- */
TRANSACTION_DATE_CLOSE_NOMINAL_UNDO *
	transaction_date_close_nominal_undo_calloc(
		void );

typedef struct
{
	boolean transaction_date_close_boolean;
	char *transaction_date_close_date_time;
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

/* Returns static memory */
/* --------------------- */
char *transaction_date_close_where_string(
		const char *transaction_close_memo,
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
char *transaction_earlier_date_time(
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
		const char *transaction_close_memo,
		char *entity_self_full_name,
		char *entity_self_street_address );

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
	char *full_name;
	char *street_address;
	char *transaction_date_time;

	/* Set externally */
	/* -------------- */
	double transaction_amount;
	int check_number;
	char *memo;
	LIST *journal_list;
	char *rental_property_street_address;
	char *fund_name;
} TRANSACTION;

/* Usage */
/* ----- */
LIST *transaction_list(
		char *where,
		boolean fetch_journal_list );

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

/* Usage */
/* ----- */
TRANSACTION *transaction_parse(
		char *input,
		boolean fetch_journal_list );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
TRANSACTION *transaction_new(
		char *full_name,
		char *street_address,
		char *transaction_date_time );

/* Process */
/* ------- */
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

/* Usage */
/* ----- */

/* May reset transaction->transaction_date_time. */
/* --------------------------------------------- */
void transaction_list_insert(
		LIST *transaction_list,
		boolean insert_journal_list_boolean,
		boolean transaction_lock_boolean );

/* Process */
/* ------- */
char transaction_lock_yn(
		boolean transaction_lock_boolean );

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

/* Returns either parameter */
/* ------------------------ */
const char *transaction_insert_column_list_string(
		const char *transaction_insert,
		const char *transaction_lock_insert,
		char transaction_lock_yn );

FILE *transaction_insert_pipe_open(
		const char *transaction_insert_column_list_string,
		const char *transaction_table );

/* Returns static memory */
/* --------------------- */
char *transaction_check_number(
		int check_number );

/* Returns static memory */
/* --------------------- */
char *transaction_memo(
		char *memo );

/* Returns static memory */
/* --------------------- */
char *transaction_lock(
		char transaction_lock_yn );

void transaction_insert_pipe(
		FILE *pipe_open,
		char *full_name,
		char *street_address,
		char *transaction_race_free_date_time,
		double transaction_amount,
		char *transaction_check_number,
		char *transaction_memo,
		char transaction_lock_yn );

/* Usage */
/* ----- */
char *transaction_stamp_insert(
		TRANSACTION *transaction,
		boolean insert_journal_list_boolean,
		boolean transaction_lock_boolean );

/* Usage */
/* ----- */
void transaction_delete(
		char *full_name,
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
void transaction_fetch_update(
		char *full_name,
		char *street_address,
		char *transaction_date_time );

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
void transaction_subsidiary_date_time_update(
		const char sql_delimiter,
		const char *subsidiary_table_name,
		char *application_name,
		char *primary_key_list_string,
		char *primary_data_list_string,
		const char *date_time_column_name,
		char *date_time_column_value );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *transaction_date_time_update_statement(
		const char sql_delimiter,
		const char *subsidiary_table_name,
		char *primary_key_list_string,
		char *primary_data_list_string,
		const char *date_time_column_name,
		char *date_time_column_value );

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

/* Process */
/* ------- */
boolean transaction_date_as_of_date_populated(
		char *as_of_date_string );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *transaction_date_begin_date_string(
		const char *transaction_table,
		char *transaction_as_of_date );

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
void transaction_journal_list_insert(
		LIST *transaction_list,
		boolean with_propagate );

/* Usage */
/* ----- */
LIST *transaction_list_extract_account_list(
		LIST *transaction_list );

/* Usage */
/* ----- */
void transaction_list_html_display(
		LIST *transaction_list );

/* Usage */
/* ----- */
void transaction_html_display(
		TRANSACTION *transaction );

/* Usage */
/* ----- */
TRANSACTION *transaction_binary_account_key(
		const char *debit_account_key,
		const char *credit_account_key,
		char *full_name,
		char *street_address,
		char *transaction_date_time,
		double transaction_amount,
		char *memo );

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

/* Usage */
/* ----- */

/* ------------------------------------ */
/* Returns heap memory.			*/
/* Increments second each invocation.   */
/* ------------------------------------ */
char *transaction_increment_date_time(
		char *transaction_date_string );

/* Usage */
/* ----- */
char *transaction_race_free_date_time(
		char *transaction_date_time );

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

#endif
