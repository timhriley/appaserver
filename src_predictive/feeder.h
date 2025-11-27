/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/feeder.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FEEDER_H
#define FEEDER_H

#include "list.h"
#include "boolean.h"
#include "date.h"
#include "appaserver_user.h"
#include "html.h"
#include "transaction.h"
#include "journal.h"
#include "account.h"
#include "feeder_account.h"
#include "feeder_load_event.h"

/* #define DEBUG_MODE 1 */

#define FEEDER_LOAD_TRANSACTION_DAYS_AGO				\
					"feeder_load_transaction_days_ago"

#define FEEDER_MATCH_DEFAULT_DAYS_AGO	6

#define FEEDER_DESCRIPTION_SIZE		140

#define FEEDER_INVALID_BEGIN_AMOUNT_TEMPLATE				\
"<h3>Mismatched input file. Either:</h3><ul><li>The bank's begin date is too recent or ... <li>The Application Constant of feeder_load_transaction_days_ago is too small or ...<li>A duplicate transaction exists.</ul><ul><li>Feeder Load Event->Account End Balance is: %.2lf<li>Exchange Format File yields an account end balance of: %.2lf<li>Execute the Audit process called Feeder Row Journal Audit for a clue.</ul>"

#define FEEDER_EXIST_ROW_SELECT		"feeder_date,"			\
					"file_row_description,"		\
					"transaction_date_time,"	\
					"file_row_amount,"		\
					"file_row_balance"

#define FEEDER_ROW_TABLE		"feeder_row"

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

#define FEEDER_ROW_FUND_INSERT_COLUMNS	"fund_name,"			\
					"feeder_account,"		\
					"feeder_load_date_time,"	\
					"feeder_date,"			\
					"feeder_row_number,"		\
					"file_row_description,"		\
					"file_row_amount,"		\
					"check_number,"			\
					"full_name,"			\
					"street_address,"		\
					"transaction_date_time,"	\
					"feeder_phrase"

#define FEEDER_PHRASE_TABLE		"feeder_phrase"

#define FEEDER_PHRASE_SELECT		"feeder_phrase,"		\
					"nominal_account,"		\
					"full_name,"			\
					"street_address"

#define FEEDER_PHRASE_DELIMITER		'|'

#define FEEDER_ACCOUNT_TABLE		"feeder_account"

enum feeder_row_status {
	feeder_row_status_okay,
	feeder_row_status_out_of_balance,
	feeder_row_status_cannot_determine };

typedef struct
{
	char *feeder_date;
	char *file_row_description;
	char *transaction_date_time;
	double file_row_amount;
	double file_row_balance;
} FEEDER_EXIST_ROW;

/* Usage */
/* ----- */
LIST *feeder_exist_row_list(
		const char *feeder_row_table,
		char *feeder_account_name,
		char *feeder_match_minimum_date );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *feeder_exist_row_where(
		char *feeder_account_name,
		char *feeder_match_minimum_date );

/* Returns heap memory */
/* ------------------- */
char *feeder_exist_row_system_string(
		const char *feeder_exist_row_select,
		const char *feeder_row_table,
		char *feeder_exist_row_where );

FEEDER_EXIST_ROW *feeder_exist_row_parse(
		char *input );

FEEDER_EXIST_ROW *feeder_exist_row_calloc(
		void );

/* Usage */
/* ----- */
FEEDER_EXIST_ROW *feeder_exist_row_seek(
		char *international_date,
		char *description_embedded,
		LIST *feeder_exist_row_list );

typedef struct
{
	char *phrase;
	char *nominal_account;
	char *full_name;
	char *street_address;
	int match_length;
} FEEDER_PHRASE;

/* Usage */
/* ----- */
LIST *feeder_phrase_list(
		char *feeder_phrase_select,
 		char *feeder_phrase_table );

/* Usage */
/* ----- */
FEEDER_PHRASE *feeder_phrase_parse(
		char *input );

/* Usage */
/* ----- */
FEEDER_PHRASE *feeder_phrase_new(
		char *phrase );

/* Process */
/* ------- */
FEEDER_PHRASE *feeder_phrase_calloc(
		void );

/* Usage */
/* ----- */
FEEDER_PHRASE *feeder_phrase_seek(
		char *financial_institution_full_name,
		char *financial_institution_street_address,
		char *feeder_load_row_description_space_trim,
		LIST *feeder_phrase_list );

/* Process */
/* ------- */
void feeder_phrase_zap_match_length(
		LIST *feeder_phrase_list );

/* Usage */
/* ----- */
void feeder_phrase_set_match_length(
		const char feeder_phrase_delimiter,
		char *description_space_trim,
		LIST *feeder_phrase_list );

/* Usage */
/* ----- */
FEEDER_PHRASE *feeder_phrase_extract(
		char *financial_institution_full_name,
		char *financial_institution_street_address,
		LIST *feeder_phrase_list );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *feeder_phrase_primary_where(
		char *feeder_phrase );

/* Usage */
/* ----- */

/* Returns feeder_phrase */
/* --------------------- */
FEEDER_PHRASE *feeder_phrase_entity_set(
		char *financial_institution_full_name,
		char *feeder_phrase_seek_full_name,
		FEEDER_PHRASE *feeder_phrase /* in/out */ );

/* Process */
/* ------- */

/* Returns either parameter */
/* ------------------------ */
char *feeder_phrase_full_name(
		char *financial_institution_full_name,
		char *feeder_phrase_full_name );

/* Returns either parameter */
/* ------------------------ */
char *feeder_phrase_street_address(
		char *financial_institution_street_address,
		char *feeder_phrase_street_address );

typedef struct
{
	char *full_name;
	char *street_address;
	char *transaction_date_time;
	char *account_name;
	double debit_amount;
	double credit_amount;
	double amount;

	/* Set externally */
	/* -------------- */
	int check_number;
	boolean taken;
	char *check_update_statement;
} FEEDER_MATCHED_JOURNAL;

/* Usage */
/* ----- */
LIST *feeder_matched_journal_list(
		const char *feeder_row_table,
		char *feeder_account_name,
		char *feeder_match_minimum_date,
		char *account_uncleared_checks_string );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *feeder_matched_journal_subquery(
		const char *journal_table,
		const char *feeder_row_table,
		char *feeder_account_name,
		char *account_uncleared_checks_string );

/* Returns heap memory */
/* ------------------- */
char *feeder_matched_journal_where(
		char *feeder_account_name,
		char *account_uncleared_checks,
		char *feeder_matched_journal_subquery,
		char *feeder_match_minimum_date );

/* Usage */
/* ----- */
FEEDER_MATCHED_JOURNAL *feeder_matched_journal_new(
		JOURNAL *journal );

/* Process */
/* ------- */
FEEDER_MATCHED_JOURNAL *feeder_matched_journal_calloc(
		void );

double feeder_matched_journal_amount(
		double debit_amount,
		double credit_amount );

/* Usage */
/* ----- */
FEEDER_MATCHED_JOURNAL *
	feeder_matched_journal_check_seek(
		char *feeder_account_name,
		char *account_uncleared_checks_string,
		int check_number,
		double exchange_journal_amount,
		LIST *feeder_matched_journal_list );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *feeder_matched_journal_check_update_statement(
		const char *journal_table,
		char *feeder_account_name,
		char *account_uncleared_checks_string,
		char *full_name,
		char *street_address,
		char *transaction_date_time );

/* Usage */
/* ----- */
FEEDER_MATCHED_JOURNAL *
	feeder_matched_journal_amount_seek(
		double exchange_journal_amount,
		LIST *feeder_matched_journal_list );

typedef struct
{
	char *debit_account;
	char *credit_account;
	TRANSACTION *transaction;
} FEEDER_TRANSACTION;

/* Usage */
/* ----- */
FEEDER_TRANSACTION *feeder_transaction_new(
		char *feeder_account_name,
		FEEDER_PHRASE *feeder_phrase_seek,
		double exchange_journal_amount,
		char *transaction_date_time,
		char *memo );

/* Process */
/* ------- */
FEEDER_TRANSACTION *feeder_transaction_calloc(
		void );

typedef struct
{
	double exchange_journal_amount;
	char *transaction_date_time;
	double debit_amount;
	double credit_amount;
	double journal_balance;
	char *international_date;
	int check_number;
	char *description_space_trim;
	char *description_embedded;

	/* Set externally */
	/* -------------- */
	double file_row_balance /* Not used if multi-fund */;
} FEEDER_LOAD_ROW;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FEEDER_LOAD_ROW *feeder_load_row_new(
		double exchange_journal_amount,
		char *exchange_journal_description,
		char *transaction_date_time,
		double debit_amount,
		double credit_amount,
		double journal_balance );

/* Process */
/* ------- */
FEEDER_LOAD_ROW *feeder_load_row_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *feeder_load_row_international_date(
		char *transaction_date_time );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *feeder_load_row_description_space_trim(
		char *exchange_journal_description );

/* Usage */
/* ----- */

/* Returns heap memory or description_space_trim */
/* --------------------------------------------- */
char *feeder_load_row_description_embedded(
		double journal_balance,
		char *description_space_trim,
		int feeder_load_row_check_number );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *feeder_load_row_trim_date(
		char *description_space_trim );

/* Returns heap memory */
/* ------------------- */
char *feeder_load_row_description_build(
		double journal_balance,
		char *feeder_load_row_trim_date );

/* Returns feeder_load_row_description_build */
/* ----------------------------------------- */
char *feeder_load_row_description_crop(
		const int feeder_description_size,
		char *feeder_load_row_description_build );

/* Usage */
/* ----- */
void feeder_load_row_file_row_balance_set(
		double exchange_balance_amount,
		LIST *feeder_load_row_list
			/* reads exchange_journal_amount */
			/* sets file_row_balance */ );

/* Process */
/* ------- */

/* Returns exchange_balance_amount */
/* ------------------------------- */
double feeder_load_row_file_row_balance(
		double exchange_balance_amount );

/* Usage */
/* ----- */
LIST *feeder_load_row_list(
		LIST *exchange_journal_list );

/* Usage */
/* ----- */
int feeder_load_row_check_number(
		char *feeder_load_row_description_space_trim );

/* Process */
/* ------- */

/* If like "VETERANS AFFAIRS DES:PAYMENT CHECK #:1827" */
/* --------------------------------------------------- */
int feeder_load_row_pound_colon_number(
		char *feeder_load_row_description_space_trim );

/* If like "#1827" */
/* --------------- */
int feeder_load_row_pound_number(
		char *feeder_load_row_description_space_trim );

/* If like "check 1827" or "CHECK 1827" */
/* ------------------------------------ */
int feeder_load_row_check_text_number(
		char *feeder_load_row_description_space_trim );

/* Usage */
/* ----- */
void feeder_load_row_list_raw_display(
		FILE *stream,
		LIST *feeder_load_row_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *feeder_load_row_raw_display(
		FEEDER_LOAD_ROW *feeder_load_row );

typedef struct
{
	char *feeder_account_name;
	FEEDER_LOAD_ROW *feeder_load_row;
	int feeder_row_number;
	FEEDER_EXIST_ROW *feeder_exist_row_seek;
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
	double calculate_balance /* Not used if multi-fund */;
	enum feeder_row_status feeder_row_status;
} FEEDER_ROW;

/* Usage */
/* ----- */
LIST *feeder_row_list(
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
		boolean transaction_fund_column_boolean,
		LIST *feeder_row_list /* sets feeder_row_status */ );

/* Usage */
/* ----- */
enum feeder_row_status feeder_row_status_evaluate(
		double feeder_load_row_file_row_balance,
		double feeder_row_calculate_balance,
		boolean transaction_fund_column_boolean );

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
		char *feeder_account_name,
		char *feeder_load_date_time );

/* Process */
/* ------- */

/* Returns program memory */
/* ---------------------- */
char *feeder_row_final_number_select(
		void );

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
		FEEDER_EXIST_ROW *feeder_exist_row_seek,
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
		boolean transaction_fund_column_boolean );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *feeder_row_list_insert_system_string(
		const char sql_delimiter,
		const char *feeder_row_insert_columns,
		const char *feeder_row_fund_insert_columns,
		const char *feeder_row_table,
		boolean transaction_fund_column_boolean );

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

typedef struct
{
	char *feeder_account_name;
	FEEDER_ACCOUNT *feeder_account;
	LIST *feeder_load_row_list;
	boolean transaction_fund_column_boolean;
	char *account_uncleared_checks_string;
	LIST *feeder_phrase_list;
	LIST *feeder_exist_row_list;
	int match_days_ago;
	char *match_minimum_date;
	LIST *feeder_matched_journal_list;
	LIST *feeder_row_list;
	int feeder_row_count;
	FEEDER_LOAD_EVENT *feeder_load_event_latest_fetch;
	boolean latest_fetch_match_boolean;
	boolean feeder_row_list_non_match_boolean;
	int feeder_row_insert_count;
	boolean account_accumulate_debit_boolean;
	double feeder_load_event_prior_account_end_balance;
	boolean feeder_row_list_status_out_of_balance_boolean;
	char *feeder_row_account_end_date;
	double feeder_row_account_end_balance;
	char *feeder_load_date_time;
	FEEDER_LOAD_EVENT *feeder_load_event;
} FEEDER;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FEEDER *feeder_fetch(
		char *application_name,
		char *login_name,
		char *feeder_account_name,
		char *exchange_format_filename,
		LIST *exchange_journal_list,
		double exchange_journal_begin_amount,
		double exchange_balance_amount,
		char *exchange_minimum_date_string );

/* Process */
/* ------- */
FEEDER *feeder_calloc(
		void );

/* Safely returns */
/* -------------- */
int feeder_match_days_ago(
		const char *feeder_load_transaction_days_ago,
		const int feeder_match_default_days_ago );

/* Returns heap memory or "" */
/* ------------------------- */
char *feeder_match_minimum_date(
		char *exchange_minimum_date_string,
		int feeder_match_days_ago );

/* Usage */
/* ----- */
boolean feeder_latest_fetch_match_boolean(
		double exchange_journal_begin_amount,
		LIST *feeder_row_list,
		FEEDER_LOAD_EVENT *feeder_load_event_latest_fetch );

/* Process */
/* ------- */
double feeder_latest_fetch_match_difference(
		double feeder_row_account_end_balance,
		double feeder_row_exist_sum );

/* Usage */
/* ----- */
void feeder_execute(
		char *process_name,
		char *fund_name,
		FEEDER *feeder );

/* Usage */
/* ----- */
void feeder_display(
		FEEDER *feeder );

/* Usage */
/* ----- */
boolean feeder_execute_boolean(
		boolean execute_boolean,
		boolean feeder_row_list_non_match_boolean,
		boolean feeder_row_list_out_of_balance_boolean );

#endif
