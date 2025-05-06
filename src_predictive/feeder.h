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
#include "transaction.h"
#include "journal.h"
#include "account.h"
#include "feeder_account.h"
#include "html.h"

#define FEEDER_LOAD_TRANSACTION_DAYS_AGO				\
					"feeder_load_transaction_days_ago"

#define FEEDER_MATCH_DEFAULT_DAYS_AGO	6

#define FEEDER_DESCRIPTION_SIZE		140

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
					"full_name,"			\
					"street_address,"		\
					"transaction_date_time,"	\
					"file_row_description,"		\
					"file_row_amount,"		\
					"file_row_balance,"		\
					"calculate_balance,"		\
					"check_number,"			\
					"feeder_phrase"

#define FEEDER_LOAD_EVENT_TABLE		"feeder_load_event"

#define FEEDER_LOAD_EVENT_SELECT	"feeder_account,"	\
					"feeder_load_date_time,"\
					"full_name,"		\
					"street_address,"	\
					"feeder_load_filename,"	\
					"account_end_date,"	\
					"account_end_balance"

#define FEEDER_LOAD_EVENT_INSERT	"feeder_account,"	\
					"feeder_load_date_time,"\
					"full_name,"		\
					"street_address,"	\
					"feeder_load_filename,"	\
					"account_end_date,"	\
					"account_end_balance"

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

FILE *feeder_exist_row_input_open(
		char *feeder_exist_row_system_string );

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

/* Process */
/* ------- */
FEEDER_PHRASE *feeder_phrase_new(
		char *phrase );

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
char feeder_phrase_delimiter(
		char *phrase );

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
		FEEDER_PHRASE *feeder_phrase );

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
		char *feeder_account_name,
		char *feeder_row_table,
		char *feeder_match_minimum_date,
		char *account_uncleared_checks );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *feeder_matched_journal_subquery(
		char *feeder_account_name,
		char *account_uncleared_checks,
		char *journal_table,
		char *feeder_row_table );

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
		char *account_uncleared_checks,
		int check_number,
		double exchange_journal_amount,
		LIST *feeder_matched_journal_list );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *feeder_matched_journal_check_update_statement(
		char *journal_table,
		char *feeder_account_name,
		char *account_uncleared_checks,
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
	/* Saved as file_row_amount */
	/* ------------------------ */
	double calculate_balance;
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
void feeder_load_row_calculate_balance_set(
		double exchange_balance_amount,
		LIST *feeder_load_row_list /* in/out */ );

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
	double calculate_balance;
	enum feeder_row_status feeder_row_status;
} FEEDER_ROW;

/* Usage */
/* ----- */
LIST *feeder_row_list(
		char *feeder_account_name,
		char *financial_institution_full_name,
		char *financial_institution_street_address,
		char *account_uncleared_checks,
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
		char *account_uncleared_checks,
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
		LIST *feeder_row_list /* in/out */,
		FEEDER_ROW *feeder_row_first_out_balance,
		double feeder_prior_account_end_balance );

/* Usage */
/* ----- */
double feeder_row_calculate_balance(
		FEEDER_ROW *feeder_row,
		double feeder_prior_account_end_balance );

/* Usage */
/* ----- */
FEEDER_ROW *feeder_row_first_out_balance(
		LIST *feeder_row_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
void feeder_row_status_set(
		LIST *feeder_row_list /* in/out */,
		FEEDER_ROW *feeder_row_first_out_balance );

/* Usage */
/* ----- */
enum feeder_row_status feeder_row_status_evaluate(
		double feeder_load_row_calculate_balance,
		double feeder_row_calculate_balance );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *feeder_row_system_string(
		char *feeder_row_select,
		char *feeder_row_table,
		char *where );

/* Usage */
/* ----- */
LIST *feeder_row_system_list(
		char *feeder_row_system_string );

/* Process */
/* ------- */

/* Safely returns */
/* -------------- */
FILE *feeder_row_input_pipe(
		char *feeder_row_system_string );

/* Usage */
/* ----- */
FEEDER_ROW *feeder_row_parse(
		char *input );

/* Usage */
/* ----- */
int feeder_row_final_number(
		char *feeder_row_table,
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
		char *feeder_row_table,
		char *feeder_account_name,
		char *feeder_load_date_time );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *feeder_row_minimum_transaction_date_time(
		char *feeder_row_table,
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
/* ------------------------------------------------------- */
double feeder_row_account_end_balance(
		LIST *feeder_row_list,
		FEEDER_ROW *feeder_row_first_out_balance );

/* Usage */
/* ----- */

/* Returns feeder_row->feeder_load_row->international_date or null */
/* --------------------------------------------------------------- */
char *feeder_row_account_end_date(
		LIST *feeder_row_list,
		FEEDER_ROW *feeder_row_first_out_balance );

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

/* Returns program memory */
/* ---------------------- */
char *feeder_row_no_more_display(
		void );

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
		char *feeder_account_name,
		char *feeder_load_date_time,
		LIST *feeder_row_list,
		FEEDER_ROW *feeder_row_first_out_balance );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *feeder_row_list_insert_system_string(
		char *feeder_row_insert_columns,
		char *feeder_row_table,
		char sql_delimiter );

/* Safely returns */
/* -------------- */
FILE *feeder_row_list_insert_open(
		char *feeder_row_list_insert_system_string );

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
		FILE *feeder_row_list_insert_open,
		char *feeder_account_name,
		char *feeder_load_date_time,
		char *international_date,
		int feeder_row_number,
		char *full_name,
		char *street_address,
		char *transaction_date_time,
		char *description_embedded,
		double exchange_journal_amount,
		double feeder_load_row_calculate_balance,
		double feeder_row_calculate_balance,
		int check_number,
		char *feeder_row_phrase,
		char sql_delimiter );

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
		LIST *feeder_row_list,
		FEEDER_ROW *feeder_row_first_out_balance );

/* Process */
/* ------- */

/* Set each feeder_row->transaction_date_time */
/* ------------------------------------------ */
void feeder_row_transaction_date_time_set(
		LIST *feeder_row_list /* in/out */ );

/* Usage */
/* ----- */
void feeder_row_journal_propagate(
		char *feeder_account_name,
		char *feeder_load_date_time,
		char *account_uncleared_checks );

/* Usage */
/* ----- */
LIST *feeder_row_extract_transaction_list(
		LIST *feeder_row_list,
		FEEDER_ROW *feeder_row_first_out_balance );

/* Usage */
/* ------ */
void feeder_row_check_journal_update(
		LIST *feeder_row_list,
		FEEDER_ROW *feeder_row_first_out_balance );

/* Process */
/* ------- */
FILE *feeder_row_check_journal_update_pipe(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *feeder_row_raw_display(
		FEEDER_ROW *feeder_row );

typedef struct
{
	char *feeder_account_name;
	char *feeder_load_date_time;
	char *feeder_load_filename;
	char *feeder_row_account_end_date;
	double feeder_row_account_end_balance;
	APPASERVER_USER *appaserver_user;
} FEEDER_LOAD_EVENT;

/* Usage */
/* ----- */
FEEDER_LOAD_EVENT *feeder_load_event_new(
		char *feeder_account_name,
		char *feeder_load_date_time,
		char *login_name,
		char *feeder_load_filename,
		char *feeder_row_account_end_date,
		double feeder_row_account_end_balance );

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
		char *feeder_load_event_select,
		char *feeder_load_event_table,
		char *feeder_load_event_primary_where );

FILE *feeder_load_event_input_open(
		char *feeder_load_event_system_string );

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
		char *feeder_load_date_time,
		char *full_name,
		char *street_address,
		char *feeder_load_filename,
		char *feeder_row_account_end_date,
		double feeder_row_account_end_balance );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *feeder_load_event_insert_system_string(
		const char *feeder_load_event_table,
		const char *feeder_load_event_insert,
		const char sql_delimiter );

FILE *feeder_load_event_insert_open(
		char *feeder_load_event_insert_system_string );

void feeder_load_event_insert_pipe(
		FILE *insert_open,
		const char sql_delimiter,
		char *feeder_account_name,
		char *feeder_load_date_time,
		char *full_name,
		char *street_address,
		char *feeder_load_filename,
		char *feeder_row_account_end_date,
		double feeder_row_account_end_balance );

typedef struct
{
	char *feeder_account_name;
	FEEDER_ACCOUNT *feeder_account;
	LIST *feeder_load_row_list;
	char *account_uncleared_checks;
	LIST *feeder_phrase_list;
	LIST *feeder_exist_row_list;
	int match_days_ago;
	char *match_minimum_date;
	LIST *feeder_matched_journal_list;
	LIST *feeder_row_list;
	FEEDER_ROW *feeder_row_first_out_balance;
	int feeder_row_count;
	int feeder_row_insert_count;
	boolean account_accumulate_debit_boolean;
	double prior_account_end_balance;
	char *feeder_row_account_end_date;
	double feeder_row_account_end_balance;
	char *feeder_load_date_time;
	FEEDER_LOAD_EVENT *feeder_load_event;
	char *parameter_end_balance_error;
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
double feeder_prior_account_end_balance(
		const char *feeder_load_event_table,
		char *feeder_account_name,
		boolean account_accumulate_debit_boolean );

/* Usage */
/* ----- */

/* Returns heap memory or "" */
/* ------------------------- */
char *feeder_parameter_end_balance_error(
		double exchange_balance_amount,
		FEEDER_ROW *feeder_row_first_out_balance,
		char *feeder_row_account_end_date,
		double feeder_row_account_end_balance );

/* Usage */
/* ----- */
void feeder_execute(
		char *process_name,
		FEEDER *feeder );

/* Usage */
/* ----- */
void feeder_display(
		FEEDER *feeder );

/* Usage */
/* ----- */
boolean feeder_execute_boolean(
		boolean execute_boolean,
		FEEDER_ROW *feeder_row_first_out_balance );

#define FEEDER_AUDIT_HTML_TITLE		"Feeder After Execute Audit"

typedef struct
{
	FEEDER_LOAD_EVENT *feeder_load_event;
	int feeder_row_final_number;
	FEEDER_ROW *feeder_row_fetch;
	char *end_transaction_date_time;
	JOURNAL *journal_latest;
	ACCOUNT *account_fetch;
	double credit_balance_negate;
	double balance_difference;
	boolean difference_zero;
	HTML_TABLE *html_table;
} FEEDER_AUDIT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FEEDER_AUDIT *feeder_audit_fetch(
		char *application_name,
		char *login_name,
		char *feeder_account_name );

/* Process */
/* ------- */
FEEDER_AUDIT *feeder_audit_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *feeder_audit_end_transaction_date_time(
		const char *transaction_date_close_time,
		char *feeder_date );

LIST *feeder_audit_html_column_list(
		void );

double feeder_audit_credit_balance_negate(
		double journal_balance,
		boolean element_accumulate_debit );

double feeder_audit_balance_difference(
		double calculate_balance,
		double journal_balance );

boolean feeder_audit_difference_zero(
		double feeder_audit_balance_difference );

/* Usage */
/* ----- */
HTML_ROW *feeder_audit_html_row(
		char *application_name,
		char *login_name,
		FEEDER_ROW *feeder_row,
		JOURNAL *journal,
		double journal_balance,
		double feeder_audit_balance_difference,
		boolean feeder_audit_difference_zero );

/* Process */
/* ------- */
LIST *feeder_audit_html_cell_list(
		int feeder_row_number,
		char *feeder_row_full_name,
		char *file_row_description,
		char *feeder_row_transaction_date_time,
		char *feeder_date,
		double feeder_row_file_row_amount,
		double feeder_row_calculate_balance,
		char *journal_full_name,
		char *journal_transaction_date_time,
		double journal_balance,
		double feeder_audit_balance_difference,
		boolean feeder_audit_difference_zero );

#endif
