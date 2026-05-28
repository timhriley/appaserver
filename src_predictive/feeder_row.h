/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/feeder_row.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FEEDER_ROW_H
#define FEEDER_ROW_H

#include "list.h"
#include "boolean.h"
#include "date.h"
#include "appaserver_user.h"
#include "html.h"
#include "predictive.h"
#include "transaction.h"
#include "journal.h"
#include "account.h"
#include "feeder_account.h"
#include "feeder_load_row.h"
#include "feeder_load_event.h"
#include "feeder_transaction.h"
#include "feeder_phrase.h"
#include "feeder_matched_journal.h"

#define FEEDER_ROW_TABLE		"feeder_row"

#define FEEDER_ROW_EXIST_SELECT		"feeder_date,"			\
					"file_row_description,"		\
					"transaction_date_time,"	\
					"file_row_amount,"		\
					"file_row_balance"

#define FEEDER_ROW_SELECT		"feeder_account,"		\
					"feeder_load_date_time,"	\
					"feeder_row_number,"		\
					"feeder_date,"			\
					"full_name,"			\
					"street_address,"		\
					"transaction_date_time,"	\
					"file_row_description,"		\
					"file_row_amount,"		\
					"file_row_balance,"		\
					"calculate_balance,"		\
					"check_number,"			\
					"feeder_phrase"


#define FEEDER_ROW_INSERT_COLUMNS	"feeder_account,"		\
					"feeder_load_date_time,"	\
					"feeder_date,"			\
					"feeder_row_number,"		\
					"file_row_description,"		\
					"file_row_amount,"		\
					"file_row_balance,"		\
					"calculate_balance,"		\
					"check_number,"			\
					"full_name,"			\
					"street_address,"		\
					"transaction_date_time,"	\
					"feeder_phrase"

enum feeder_row_status {
	feeder_row_status_cannot_determine,
	feeder_row_status_okay,
	feeder_row_status_out_of_balance };

typedef struct
{
	char *feeder_date;
	char *file_row_description;
	char *transaction_date_time;
	double file_row_amount;
	double file_row_balance;
} FEEDER_ROW_EXIST;

/* Usage */
/* ----- */
LIST *feeder_row_exist_list(
		const char *feeder_row_table,
		char *feeder_account_name,
		char *feeder_match_minimum_date );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *feeder_row_exist_where(
		char *feeder_account_name,
		char *feeder_match_minimum_date );

/* Returns heap memory */
/* ------------------- */
char *feeder_row_exist_system_string(
		const char *feeder_row_exist_select,
		const char *feeder_row_table,
		char *feeder_row_exist_where );

FEEDER_ROW_EXIST *feeder_row_exist_parse(
		char *input );

FEEDER_ROW_EXIST *feeder_row_exist_calloc(
		void );

/* Usage */
/* ----- */
FEEDER_ROW_EXIST *feeder_row_exist_seek(
		char *international_date,
		char *description_embedded,
		LIST *feeder_row_exist_list );

typedef struct
{
	char *feeder_account_name;
	FEEDER_LOAD_ROW *feeder_load_row;
	int feeder_row_number;
	FEEDER_ROW_EXIST *feeder_row_exist_seek;
	FEEDER_MATCHED_JOURNAL *feeder_matched_journal;
	FEEDER_PHRASE *feeder_phrase_seek;
	FEEDER_TRANSACTION *feeder_transaction;
	char *transaction_date_time;

	/* Set by feeder_row_parse() */
	/* ------------------------- */
	char *feeder_load_date_time;
	char *feeder_date;
	char *full_name;
	char *street_address;
	char *file_row_description;
	double file_row_amount;
	double file_row_balance;
	int check_number;
	char *feeder_phrase;

	/* Set externally */
	/* -------------- */
	double calculate_balance;
	enum feeder_row_status feeder_row_status;
	JOURNAL *journal /* Set by FEEDER_AUDIT_JOURNAL */;
} FEEDER_ROW;

/* Usage */
/* ----- */
LIST *feeder_row_list(
		char *fund_name,
		char *feeder_account_name,
		char *financial_institution_full_name,
		char *financial_institution_street_address,
		char *account_uncleared_checks_string,
		LIST *feeder_phrase_list,
		LIST *feeder_exist_row_list,
		LIST *feeder_matched_journal_list,
		LIST *feeder_load_row_list );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FEEDER_ROW *feeder_row_new(
		char *fund_name,
		char *feeder_account_name,
		char *financial_institution_full_name,
		char *financial_institution_street_address,
		char *account_uncleared_checks_string,
		LIST *feeder_phrase_list,
		LIST *feeder_exist_row_list,
		LIST *feeder_matched_journal_list,
		FEEDER_LOAD_ROW *feeder_load_row,
		char *minimum_transaction_date_time,
		int feeder_row_number );

/* Process */
/* ------- */
FEEDER_ROW *feeder_row_calloc(
		void );

/* Usage */
/* ----- */
void feeder_row_calculate_balance_set(
		LIST *feeder_row_list /* sets calculate_balance */,
		double feeder_load_event_prior_account_end_balance );

/* Usage */
/* ----- */
double feeder_row_calculate_balance(
		FEEDER_ROW *feeder_row,
		double feeder_load_event_prior_account_end_balance );

/* Usage */
/* ----- */
boolean feeder_row_list_non_match_boolean(
		LIST *feeder_row_list );

/* Usage */
/* ----- */
void feeder_row_list_status_set(
		LIST *feeder_row_list /* sets feeder_row_status */ );

/* Usage */
/* ----- */
enum feeder_row_status feeder_row_status_evaluate(
		double feeder_load_row_file_row_balance,
		double feeder_row_calculate_balance );

/* Usage */
/* ----- */
boolean feeder_row_list_status_out_of_balance_boolean(
		LIST *feeder_row_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *feeder_row_system_string(
		const char *feeder_row_select,
		const char *feeder_row_table,
		char *where );

/* Usage */
/* ----- */
LIST *feeder_row_system_list(
		char *feeder_row_system_string );

/* Usage */
/* ----- */
FEEDER_ROW *feeder_row_parse(
		char *input );

/* Usage */
/* ----- */
int feeder_row_final_number(
		const char *feeder_row_table,
		char *fund_name,
		char *feeder_account_name,
		char *feeder_load_date_time );

/* Process */
/* ------- */

/* Returns program memory */
/* ---------------------- */
char *feeder_row_final_number_select(
		void );

/* Returns parameter or static memory */
/* ---------------------------------- */
char *feeder_row_final_number_where(
		const char *feeder_fund_column_name,
		char *fund_name,
		char *feeder_load_event_primary_where );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *feeder_row_maximum_transaction_date_time(
		const char *feeder_row_table,
		char *feeder_account_name,
		char *feeder_load_date_time );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *feeder_row_minimum_transaction_date_time(
		const char *feeder_row_table,
		char *feeder_account_name,
		char *feeder_load_date_time );

/* Usage */
/* ----- */
FEEDER_ROW *feeder_row_fetch(
		char *feeder_account_name,
		char *feeder_load_date_time,
		int feeder_row_number );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *feeder_row_primary_where(
		char *feeder_account_name,
		char *feeder_load_date_time,
		int feeder_row_number );

/* Usage */
/* ----- */

/* Returns feeder_row->feeder_load_row->calculate_balance */
/* ------------------------------------------------------ */
double feeder_row_account_end_balance(
		LIST *feeder_row_list );

/* Usage */
/* ----- */

/* Returns feeder_row->feeder_load_row->international_date */
/* ------------------------------------------------------- */
char *feeder_row_account_end_date(
		LIST *feeder_row_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *feeder_row_transaction_date_time(
		char *feeder_load_row_international_date,
		char *minimum_transaction_date_time );

/* Usage */
/* ----- */
int feeder_row_count(
		LIST *feeder_row_list );

/* Usage */
/* ----- */
int feeder_row_insert_count(
		LIST *feeder_row_list );

/* Usage */
/* ----- */
void feeder_row_error_display(
		LIST *feeder_row_list );

/* Process */
/* ------- */
LIST *feeder_row_error_extract_list(
		LIST *feeder_row_list );

/* Usage */
/* ----- */
void feeder_row_list_display(
		LIST *feeder_row_list );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *feeder_row_list_display_system_string(
		void );

/* Safely returns */
/* -------------- */
FILE *feeder_row_list_display_pipe(
		char *system_string );

/* Usage */
/* ----- */

/* Returns display_pipe or null */
/* ---------------------------- */
FILE *feeder_row_display_output(
		FILE *display_pipe,
		FEEDER_ROW *feeder_row );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *feeder_row_display_results(
		FEEDER_ROW_EXIST *feeder_exist_row_seek,
		FEEDER_MATCHED_JOURNAL *feeder_matched_journal,
		FEEDER_PHRASE *feeder_phrase_seek );

/* Usage */
/* ----- */

/* Returns program memory */
/* ---------------------- */
char *feeder_row_status_string(
		enum feeder_row_status feeder_row_status );

/* Driver */
/* ------ */
void feeder_row_list_insert(
		char *fund_name,
		char *feeder_account_name,
		char *feeder_load_date_time,
		LIST *feeder_row_list,
		boolean predictive_fund_boolean );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *feeder_row_list_insert_system_string(
		const char sql_delimiter,
		const char *feeder_row_insert_columns,
		const char *feeder_row_table,
		const char *feeder_fund_column_name,
		boolean predictive_fund_boolean );

/* Safely returns */
/* -------------- */
JOURNAL *feeder_row_journal(
		FEEDER_MATCHED_JOURNAL *feeder_matched_journal,
		FEEDER_PHRASE *feeder_phrase_seek,
		/* ------------------------------------------ */
		/* See feeder_row_transaction_date_time_set() */
		/* ------------------------------------------ */
		char *transaction_date_time );

/* Safely returns */
/* -------------- */
char *feeder_row_phrase(
		FEEDER_PHRASE *feeder_phrase_seek );

void feeder_row_insert(
		const char sql_delimiter,
		char *fund_name,
		char *feeder_account_name,
		char *feeder_load_date_time,
		char *feeder_date,
		int feeder_row_number,
		char *file_row_description,
		double file_row_amount,
		double feeder_load_row_file_row_balance,
		double feeder_row_calculate_balance,
		int check_number,
		boolean transaction_fund_column_boolean,
		FILE *appaserver_output_pipe,
		char *full_name,
		char *street_address,
		char *transaction_date_time,
		char *feeder_row_phrase );

/* Usage */
/* ----- */
double feeder_row_exist_sum(
		LIST *feeder_row_list );

/* Driver */
/* ------ */

/* ---------------------------- */
/* May reset			*/
/* feeder_row->			*/
/*	feeder_transaction->	*/
/*	transaction_date_time	*/
/* and				*/
/* feeder_row->			*/
/*	transaction_date_time	*/
/* ---------------------------- */
void feeder_row_transaction_insert(
		char *fund_name,
		LIST *feeder_row_list );

/* Process */
/* ------- */

/* Set each feeder_row->transaction_date_time */
/* ------------------------------------------ */
void feeder_row_transaction_date_time_set(
		LIST *feeder_row_list /* in/out */ );

/* Usage */
/* ----- */
void feeder_row_journal_propagate(
		char *fund_name,
		char *feeder_account_name,
		char *feeder_load_date_time,
		char *account_uncleared_checks );

/* Usage */
/* ----- */
LIST *feeder_row_extract_transaction_list(
		char *fund_name,
		LIST *feeder_row_list );

/* Usage */
/* ------ */
void feeder_row_check_journal_update(
		LIST *feeder_row_list );

/* Process */
/* ------- */
FILE *feeder_row_check_journal_update_pipe(
		void );

/* Usage */
/* ----- */
void feeder_row_list_raw_display(
		FILE *stream,
		LIST *feeder_row_list );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *feeder_row_raw_display(
		FEEDER_ROW *feeder_row );

#endif
