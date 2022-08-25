/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/feeder.h				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef FEEDER_H
#define FEEDER_H

#include "list.h"
#include "boolean.h"
#include "date.h"
#include "transaction.h"
#include "journal.h"

#define FEEDER_LOAD_TRANSACTION_DAYS_AGO				\
					"feeder_load_transaction_days_ago"

#define FEEDER_DESCRIPTION_SIZE		140

#define FEEDER_EXIST_ROW_SELECT		"feeder_date,"			\
					"feeder_description,"		\
					"transaction_date_time,"	\
					"feeder_row_balance"

typedef struct
{
	char *feeder_date;
	char *feeder_description;
	char *transaction_date_time;
	double feeder_row_balance;
} FEEDER_EXIST_ROW;

/* Usage */
/* ----- */
LIST *feeder_exist_row_list(
			char *feeder_account,
			char *feeder_load_file_minimum_date );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *feeder_exist_row_where(
			char *feeder_account,
			char *feeder_load_file_minimum_date );

/* Returns heap memory */
/* ------------------- */
char *feeder_exist_row_system_string(
			char *feeder_exist_row_select,
			char *feeder_load_row_table,
			char *feeder_exist_row_where );

FILE *feeder_exist_row_input_open(
			char *feeder_exist_row_system_string );

FEEDER_EXIST_ROW *feeder_exist_row_parse(
			char *input );

FEEDER_EXIST_ROW *feeder_exist_row_calloc(
			void );

/* Public */
/* ------ */
FEEDER_EXIST_ROW *feeder_exist_row_seek(
			char *international_date,
			char *description_embedded,
			LIST *feeder_exist_row_list );

#define FEEDER_PHRASE_TABLE		"feeder_phrase"

#define FEEDER_PHRASE_SELECT		"feeder_phrase,"		\
					"nominal_account,"		\
					"full_name,"			\
					"street_address,"		\
					"feeder_phrase_ignore_yn"

typedef struct
{
	char *phrase;
	char *nominal_account;
	char *full_name;
	char *street_address;
	boolean feeder_phrase_ignore;
} FEEDER_PHRASE;

/* Usage */
/* ----- */
LIST *feeder_phrase_list(
			char *feeder_phrase_select,
	 		char *feeder_phrase_table );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *feeder_phrase_system_string(
			char *feeder_phrase_select,
			char *feeder_phrase_table );

FILE *feeder_phrase_pipe_open(
			char *feeder_phrase_system_string );

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
			char *feeder_description,
			LIST *feeder_phrase_list );

/* Process */
/* ------- */
char feeder_phrase_delimiter(
			char *phrase );

typedef struct
{
	/* Attributes */
	/* ---------- */
	char *full_name;
	char *street_address;
	char *transaction_date_time;
	char *account_name;
	double debit_amount;
	double credit_amount;
	double amount;
	int check_number;
	boolean taken;

	/* Process */
	/* ------- */
	char *subquery;
	char *where;
	LIST *journal_system_list;
} FEEDER_MATCHED_JOURNAL;

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
LIST *feeder_matched_journal_list(
			char *feeder_account,
			char *feeder_load_row_table,
			char *feeder_load_file_minimum_date,
			char *account_uncleared_checks );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *feeder_matched_journal_subquery(
			char *feeder_account,
			char *account_uncleared_checks,
			char *journal_table,
			char *feeder_load_row_table );

int feeder_matched_journal_days_ago(
			char *feeder_load_transaction_days_ago );

/* Returns heap memory */
/* ------------------- */
char *feeder_matched_journal_minimum_date(
			char *feeder_load_file_minimum_date,
			int feeder_matched_journal_days_ago );

/* Returns heap memory */
/* ------------------- */
char *feeder_matched_journal_where(
			char *feeder_account,
			char *account_uncleared_checks,
			char *feeder_matched_journal_subquery,
			char *feeder_matched_journal_minimum_date );

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

/* Public */
/* ------ */
FEEDER_MATCHED_JOURNAL *
	feeder_matched_journal_check_seek(
			int check_number,
			double amount,
			LIST *feeder_matched_journal_list );

FEEDER_MATCHED_JOURNAL *
	feeder_matched_journal_amount_seek(
			double amount,
			LIST *feeder_matched_journal_list );

/* Driver */
/* ------ */
void feeder_matched_journal_not_taken_display(
			LIST *feeder_matched_journal_list );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *feeder_matched_journal_not_taken_system_string(
			void );

typedef struct
{
	char *debit_account;
	char *credit_account;
	TRANSACTION *transaction;
} FEEDER_TRANSACTION;

/* Usage */
/* ----- */
FEEDER_TRANSACTION *feeder_transaction_new(
			char *feeder_account,
			FEEDER_PHRASE *feeder_phrase_seek,
			double feeder_load_row_amount,
			char *transaction_date_time,
			char *memo );

/* Process */
/* ------- */
FEEDER_TRANSACTION *feeder_transaction_calloc(
			void );

#define FEEDER_LOAD_ROW_TABLE		"feeder_load_row"

#define FEEDER_LOAD_ROW_INSERT_COLUMNS	"full_name,"			\
					"street_address,"		\
					"transaction_date_time,"	\
					"feeder_account,"		\
					"feeder_date,"			\
					"feeder_description,"		\
					"feeder_amount,"		\
					"feeder_row_balance,"		\
					"row_number,"			\
					"feeder_phrase,"		\
					"feeder_load_date"

typedef struct
{
	char *american_date;
	char *international_date;
	char *feeder_description;
	double debit;
	double credit;
	double feeder_amount;
	double balance;
	char *reference_string;
	int check_number;
	int row_number;
	char *description_embedded;
	char *transaction_date_time;
	char *memo;
} FEEDER_LOAD_ROW;

/* Usage */
/* ----- */
FEEDER_LOAD_ROW *feeder_load_row_new(
			int date_column /* one_based */,
			int description_column /* one_based */,
			int debit_column /* one_based */,
			int credit_column /* one_based */,
			int balance_column /* one_based */,
			int reference_column /* one_based */,
			char *input,
			int row_number );

/* Process */
/* ------- */
FEEDER_LOAD_ROW *feeder_load_row_calloc(
			void );


/* Returns heap memory or null */
/* --------------------------- */
char *feeder_load_row_international_date(
			char *american_date );

double feeder_load_row_amount(
			double debit,
			double credit );

int feeder_load_row_check_number(
			char *feeder_description );

/* Usage */
/* ----- */

/* Returns heap memory or feeder_description */
/* ----------------------------------------- */
char *feeder_load_row_description_embedded(
			char *feeder_description,
			double balance,
			char *reference_string,
			int feeder_load_row_check_number );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *feeder_load_row_trim_date(
			char *feeder_description );

/* Returns heap memory */
/* ------------------- */
char *feeder_load_row_description_build(
			char *sed_trim_double_spaces,
			double balance,
			char *reference_string );

/* Returns feeder_load_row_description_build */
/* ----------------------------------------- */
char *feeder_load_row_description_crop(
			char *feeder_load_row_description_build,
			int feeder_description_size );

/* Public */
/* ------ */
double feeder_load_row_account_end_balance(
			FEEDER_LOAD_ROW *last_feeder_load_row );

typedef struct
{
	FEEDER_LOAD_ROW *feeder_load_row;
	FEEDER_EXIST_ROW *feeder_exist_row_seek;
	FEEDER_MATCHED_JOURNAL *feeder_matched_journal;
	FEEDER_PHRASE *feeder_phrase_seek;
	char *memo;
	char *transaction_date_time;
	FEEDER_TRANSACTION *feeder_transaction;
} FEEDER_ROW;

/* Usage */
/* ----- */
LIST *feeder_row_list(	DATE *feeder_load_date /* in/out */,
			char *feeder_account,
			LIST *feeder_phrase_list,
			LIST *feeder_exist_row_list,
			LIST *feeder_matched_journal_list,
			LIST *feeder_load_row_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
FEEDER_ROW *feeder_row_new(
			DATE *feeder_load_date /* in/out */,
			char *feeder_account,
			LIST *feeder_phrase_list,
			LIST *feeder_exist_row_list,
			LIST *feeder_matched_journal_list,
			FEEDER_LOAD_ROW *feeder_load_row,
			char *minimum_transaction_date_time );

/* Process */
/* ------- */
FEEDER_ROW *feeder_row_calloc(
			void );

/* Returns heap memory */
/* ------------------- */
char *feeder_row_transaction_date_time(
			char *feeder_load_row_international_date,
			char *date_hms,
			char *minimum_transaction_date_time );

/* Usage */
/* ----- */
void feeder_row_balance_set(
			LIST *feeder_row_list,
			double feeder_prior_account_end_balance,
			FEEDER_ROW *feeder_row_first_out_balance );

/* Process */
/* ------- */

/* Usage */
/* ----- */
FEEDER_ROW *feeder_row_first_out_balance(
			LIST *feeder_row_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
double feeder_row_list_sum_amount(
			LIST *feeder_row_list );

/* Process */
/* ------- */

/* Public */
/* ------ */
int feeder_row_seek_matched_count(
			LIST *feeder_row_list,
			FEEDER_ROW *feeder_row_first_out_balance );

/* Driver */
/* ------ */
boolean feeder_row_error_display(
			LIST *feeder_row_list );

/* Process */
/* ------- */
LIST *feeder_row_error_extract_list(
			LIST *feeder_row_list );

/* Driver */
/* ------ */
boolean feeder_row_list_display(
			LIST *feeder_row_list,
			FEEDER_ROW *feeder_row_first_out_balance );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *feeder_row_list_display_system_string(
			void );

/* Always succeeds */
/* --------------- */
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

/* Driver */
/* ------ */
void feeder_row_list_insert(
			char *feeder_account,
			char *feeder_load_date_string,
			LIST *feeder_row_list,
			FEEDER_ROW *feeder_row_first_out_balance );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *feeder_row_list_insert_system_string(
			char *feeder_load_row_insert_columns,
			char *feeder_load_row_table,
			char sql_delimiter );

/* Always succeeds */
/* --------------- */
FILE *feeder_row_list_insert_open(
			char *feeder_row_list_insert_system_string );

JOURNAL *feeder_row_journal(
			FEEDER_PHRASE *feeder_phrase_seek,
			char *transaction_date_time,
			FEEDER_MATCHED_JOURNAL *feeder_matched_journal );

/* Always succeeds */
/* --------------- */
char *feeder_row_phrase(
			FEEDER_PHRASE *feeder_phrase_seek );

void feeder_row_insert_pipe(
			FILE *feeder_row_list_insert_open,
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *feeder_account,
			char *international_date,
			char *description_embedded,
			double feeder_amount,
			double balance,
			int row_number,
			char *feeder_row_phrase,
			char *feeder_load_date_string,
			char sql_delimiter );

/* Driver */
/* ------ */
void feeder_row_transaction_insert(
			LIST *feeder_row_list,
			FEEDER_ROW *feeder_row_first_out_balance );

/* Process */
/* ------- */
LIST *feeder_row_extract_transaction_list(
			LIST *feeder_row_list,
			FEEDER_ROW *feeder_row_first_out_balance );

typedef struct
{
	char *basename_filename;
	FILE *input_open;
	char input[ 1024 ];
	LIST *feeder_load_row_list;
	char *remove_character;
	int row_number;
} FEEDER_LOAD_FILE;

/* Usage */
/* ----- */
FEEDER_LOAD_FILE *feeder_load_file_fetch(
			char *feeder_load_filename,
			int date_column /* one_based */,
			int description_column /* one_based */,
			int debit_column /* one_based */,
			int credit_column /* one_based */,
			int balance_column /* one_based */,
			int reference_column /* one_based */,
			boolean reverse_order_boolean );

/* Process */
/* ------- */
FEEDER_LOAD_FILE *feeder_load_file_calloc(
			void );

/* Returns heap memory */
/* ------------------- */
char *feeder_load_file_basename_filename(
			char *feeder_load_filename );

FILE *feeder_load_file_input_open(
			char *feeder_load_filename );

/* Public */
/* ------ */

/* Returns heap memory or null */
/* --------------------------- */
char *feeder_load_file_minimum_date(
			char *feeder_load_filename,
			int date_column /* one_based */ );

#define FEEDER_LOAD_EVENT_TABLE		"feeder_load_event"

#define FEEDER_LOAD_EVENT_SELECT	"feeder_load_date,"	\
					"feeder_account,"	\
					"login_name,"		\
					"feeder_load_filename,"	\
					"account_end_date,"	\
					"account_end_balance"

#define FEEDER_LOAD_EVENT_INSERT	"feeder_load_date,"	\
					"feeder_account"

#define FEEDER_LOAD_EVENT_PRIMARY_KEY	"feeder_load_date,"	\
					"feeder_account"

typedef struct
{
	char *feeder_load_date_string;
	char *feeder_account;
	char *login_name;
	char *basename_filename;
	char *account_end_date;
	double account_end_balance;
} FEEDER_LOAD_EVENT;

/* Usage */
/* ----- */
FEEDER_LOAD_EVENT *feeder_load_event_new(
			char *feeder_load_date_string,
			char *feeder_account,
			char *login_name,
			char *feeder_load_file_basename_filename,
			char *feeder_account_end_date,
			double feeder_account_ending_balance );

/* Process */
/* ------- */
FEEDER_LOAD_EVENT *feeder_load_event_calloc(
			void );

/* Usage */
/* ----- */
FEEDER_LOAD_EVENT *feeder_load_event_fetch(
			char *feeder_load_date_string,
			char *feeder_account );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *feeder_load_event_primary_where(
			char *feeder_load_date_string,
			char *feeder_account );

/* Returns heap memory */
/* ------------------- */
char *feeder_load_event_system_string(
			char *feeder_load_event_select,
			char *feeder_load_event_table,
			char *feeder_load_event_primary_where );

FILE *feeder_load_event_input_open(
			char *feeder_load_event_system_string );

FEEDER_LOAD_EVENT *feeder_load_event_parse(
			char *input );

/* Usage */
/* ----- */
FEEDER_LOAD_EVENT *feeder_load_event_latest_fetch(
			char *feeder_load_event_table,
			char *feeder_account );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *feeder_load_event_account_where(
			char *feeder_account );

/* Returns heap memory */
/* ------------------- */
char *feeder_load_event_latest_system_string(
			char *feeder_load_event_table,
			char *feeder_load_event_account_where );

/* --------------------------- */
/* Returns heap memory or null */
/* --------------------------- */
char *feeder_load_event_latest_load_date_string(
			char *feeder_load_event_latest_system_string );

/* Driver */
/* ------ */
void feeder_load_event_insert(
			char *feeder_load_event_table,
			char *feeder_load_event_insert,
			char *feeder_load_date_string,
			char *feeder_account );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *feeder_load_event_insert_system_string(
			char *feeder_load_event_table,
			char *feeder_load_event_insert,
			char sql_delimiter );

FILE *feeder_load_event_insert_open(
			char *feeder_load_event_insert_system_string );

void feeder_load_event_insert_pipe(
			FILE *insert_open,
			char sql_delimiter,
			char *feeder_load_date_string,
			char *feeder_account );

/* Driver */
/* ------ */
void feeder_load_event_update(
			char *feeder_load_event_table,
			char *feeder_load_event_primary_key,
			char *feeder_load_date_string,
			char *feeder_account,
			char *login_name,
			char *basename_filename,
			char *account_end_date,
			double account_end_balance );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *feeder_load_event_update_system_string(
			char *feeder_load_event_table,
			char *feeder_load_event_primary_key );

FILE *feeder_load_event_update_open(
			char *feeder_load_event_update_system_string );

void feeder_load_event_update_pipe(
			FILE *feeder_load_event_update_open,
			char *feeder_load_date_string,
			char *feeder_account,
			char *login_name,
			char *basename_filename,
			char *account_end_date,
			double account_end_balance );

typedef struct
{
	LIST *feeder_phrase_list;
	char *feeder_load_file_minimum_date;
	LIST *feeder_exist_row_list;
	LIST *feeder_matched_journal_list;
	DATE *feeder_load_date;
	char *feeder_load_date_string;
	FEEDER_LOAD_FILE *feeder_load_file;
	LIST *feeder_row_list;
	double account_end_balance;
	double prior_account_end_balance;
	FEEDER_ROW *feeder_row_first_out_balance;
	char *account_end_date;
	FEEDER_LOAD_EVENT *feeder_load_event;
} FEEDER;

/* Usage */
/* ----- */
FEEDER *feeder_fetch(
			char *login_name,
			char *feeder_account,
			char *feeder_load_filename,
			int date_column /* one_based */,
			int description_column /* one_based */,
			int debit_column /* one_based */,
			int credit_column /* one_based */,
			int balance_column /* one_based */,
			int reference_column /* one_based */,
			boolean reverse_order_boolean,
			double parameter_account_end_balance );

/* Process */
/* ------- */
FEEDER *feeder_calloc(	void );

/* Returns this->international_date or null */
/* ---------------------------------------- */
char *feeder_account_end_date(
			FEEDER_LOAD_ROW *last_feeder_load_row );

/* Usage */
/* ----- */
double feeder_account_end_balance(
			char *feeder_account,
			int balance_column,
			double parameter_account_end_balance,
			LIST *feeder_row_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
double feeder_prior_account_end_balance(
			char *feeder_account,
			double feeder_account_end_balance,
			LIST *feeder_row_list );

/* Process */
/* ------- */

/* Driver */
/* ------ */

/* Returns heap memory or "" */
/* ------------------------- */
char *feeder_parameter_account_end_balance_error(
			double parameter_account_end_balance,
			double feeder_load_row_account_end_balance,
			FEEDER_ROW *feeder_row_first_out_balance,
			int feeder_row_seek_matched_count );

#endif
