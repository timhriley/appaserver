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
#define TRANSACTION_REVERSE_MEMO		"Reversing entry"
#define TRANSACTION_BEGIN_TIME			"00:00:00"

#define TRANSACTION_DATE_TIME_COLUMN		"transaction_date_time"
#define TRANSACTION_AMOUNT_COLUMN		"transaction_amount"

#define TRANSACTION_SELECT	"full_name,"			\
				"street_address,"		\
				"transaction_date_time,"	\
				"transaction_amount,"		\
				"check_number,"			\
				"memo"

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
		double journal_transaction_amount,
		char *transaction_primary_where );

/* Process */
/* ------- */

/* Returns static memory or null */
/* ----------------------------- */
char *transaction_set_clause(
		char *transaction_amount_column,
		double journal_transaction_amount );

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

/* Returns heap memory */
/* ------------------- */
char *transaction_display(
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

/* Returns static memory */
/* --------------------- */
char *transaction_fetch_where(
		char *full_name,
		char *street_address,
		char *transaction_date_time );

/* Usage */
/* ----- */

/* Returns transaction_date_time or heap memory */
/* -------------------------------------------- */
char *transaction_race_free_date_time(
		char *transaction_date_time );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *transaction_primary_where(
		char *full_name,
		char *street_address,
		char *transaction_date_time );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *transaction_system_string(
		char *transaction_select,
		char *transaction_table,
		char *transaction_primary_where );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *transaction_fetch_memo(
		char *transaction_date_time );

#endif
