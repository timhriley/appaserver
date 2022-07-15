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

#define FEEDER_LOAD_ROW_TABLE		"feeder_load_row"
#define FEEDER_DESCRIPTION_SIZE		140

#define FEEDER_EXIST_ROW_SELECT		"feeder_date,"		\
					"feeder_description,"	\
					"transaction_date_time"

typedef struct
{
	char *feeder_date;
	char *feeder_description;
	char *transaction_date_time;
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
			void );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *feeder_phrase_system_string(
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

/* Public */
/* ------ */
FEEDER_PHRASE *feeder_phrase_seek(
			char *feeder_description,
			LIST *feeder_phrase_list );

typedef struct
{
	char *subquery;
	char *where;
	LIST *journal_system_list;
	JOURNAL *journal;
	char *full_name;
	char *street_address;
	char *transaction_date_time;
	char *account_name;
	double debit_amount;
	double credit_amount;
	double amount;
	int check_number;
	boolean taken;
} FEEDER_MATCHED_JOURNAL;

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
LIST *feeder_matched_journal_list(
			char *feeder_account,
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

/* Returns heap memory */
/* ------------------- */
char *feeder_matched_journal_where(
			char *feeder_account,
			char *account_uncleared_checks,
			char *feeder_matched_journal_subquery,
			char *minimum_transaction_date );

/* Usage */
/* ----- */
FEEDER_MATCHED_JOURNAL *feeder_matched_journal_new(
			JOURNAL *journal );

/* Process */
/* ------- */
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
			double feeder_file_row_amount,
			char *transaction_date_time,
			char *memo );

/* Process */
/* ------- */
FEEDER_TRANSACTION *feeder_transaction_calloc(
			void );

#define FIELD_FILE_ROW_INSERT		"full_name,"			\
					"street_address,"		\
					"transaction_date_time,"	\
					"feeder_account,"		\
					"feeder_date,"			\
					"feeder_description,"		\
					"feeder_amount,"		\
					"feeder_running_balance,"	\
					"row_number,"			\
					"feeder_phrase,"		\
					"feeder_load_date_time"

typedef struct
{
	int line_number;
	char *american_date;
	char *international_date;
	char *feeder_description;
	double amount;
	double debit;
	double credit;
	double balance;
	int check_number;
	char *description_embedded;
	char *transaction_date_time;
	char *memo;
	FEEDER_EXIST_ROW *feeder_exist_row_seek;
	FEEDER_MATCHED_JOURNAL *feeder_matched_journal;
	FEEDER_PHRASE *feeder_phrase_seek;
	FEEDER_TRANSACTION *feeder_transaction;
} FEEDER_FILE_ROW;

/* Usage */
/* ----- */
FEEDER_FILE_ROW *feeder_file_row_new(
			DATE *feeder_load_date /* in/out */,
			char *feeder_account,
			int date_column /* one_based */,
			int description_column /* one_based */,
			int debit_column /* one_based */,
			int credit_column /* one_based */,
			int balance_column /* one_based */,
			LIST *feeder_phrase_list,
			LIST *feeder_exist_row_list,
			LIST *feeder_matched_journal_list,
			char *input,
			int line_number );

/* Process */
/* ------- */
FEEDER_FILE_ROW *feeder_file_row_calloc(
			void );


/* Returns heap memory or null */
/* --------------------------- */
char *feeder_file_row_international_date(
			char *american_date );

double feeder_file_row_amount(
			double debit,
			double credit );

int feeder_file_row_check_number(
			char *feeder_description );

/* Returns heap memory */
/* ------------------- */
char *feeder_file_row_transaction_date_time(
			char *feeder_file_row_international_date,
			char *date_hms );

/* Usage */
/* ----- */

/* Returns heap memory or feeder_description */
/* ----------------------------------------- */
char *feeder_file_row_description_embedded(
			char *feeder_description,
			double balance,
			int line_number,
			int feeder_file_row_check_number );

/* Returns static memory */
/* --------------------- */
char *feeder_file_row_trim_date(
			char *feeder_description );

/* Returns heap memory */
/* ------------------- */
char *feeder_file_row_description_build(
			char *sed_trim_double_spaces,
			double balance,
			int line_number );

/* Returns feeder_file_row_description_build */
/* ----------------------------------------- */
char *feeder_file_row_description_crop(
			char *feeder_file_row_description_build,
			int feeder_description_size );

/* Public */
/* ------ */
double feeder_file_row_list_sum_amount(
			LIST *feeder_file_row_list );

/* Driver */
/* ------ */
void feeder_file_row_list_insert(
			char *feeder_account,
			char *feeder_load_date_time,
			LIST *feeder_file_row_list );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *feeder_file_row_list_insert_system_string(
			char *feeder_file_row_insert,
			char *feeder_load_row_table,
			char sql_delimiter );

/* Always succeeds */
/* --------------- */
FILE *feeder_file_row_list_insert_pipe(
			char *feeder_file_row_list_insert_system_string );

/* Always succeeds */
/* --------------- */
JOURNAL *feeder_file_row_journal(
			FEEDER_PHRASE *feeder_phrase_seek,
			char *transaction_date_time,
			FEEDER_MATCHED_JOURNAL *feeder_matched_journal );

/* Always succeeds */
/* --------------- */
char *feeder_file_row_phrase(
			FEEDER_PHRASE *feeder_phrase_seek );

void feeder_file_row_insert_pipe(
			FILE *feeder_file_row_list_insert_pipe,
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *feeder_account,
			char *international_date,
			char *description_embedded,
			double amount,
			double balance,
			int row_number,
			char *feeder_file_row_phrase,
			char *feeder_load_date_time,
			char sql_delimiter );

/* Driver */
/* ------ */
void feeder_file_row_list_display(
			LIST *feeder_file_row_list );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *feeder_file_row_list_display_system_string(
			void );

/* Always succeeds */
/* --------------- */
FILE *feeder_file_row_list_display_pipe(
			char *system_string );

/* Usage */
/* ----- */

/* Returns display_pipe or null */
/* ---------------------------- */
FILE *feeder_file_row_display_output(
			FILE *display_pipe,
			FEEDER_FILE_ROW *feeder_file_row );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *feeder_file_row_display_results(
			FEEDER_EXIST_ROW *feeder_exist_row_seek,
			FEEDER_MATCHED_JOURNAL *feeder_matched_journal,
			FEEDER_PHRASE *feeder_phrase_seek );

/* Driver */
/* ------ */
void feeder_file_row_transaction_insert(
			LIST *feeder_file_row_list,
			char *appaserver_error_filename );

/* Process */
/* ------- */
TRANSACTION_LIST *
	feeder_file_row_extract_transaction_list(
			LIST *feeder_file_row_list );

typedef struct
{
	char *basename_filename;
	char *sha256sum;
	boolean feeder_load_event_sha256sum_exists;
	FILE *file;
	char input[ 1024 ];
	LIST *feeder_file_row_list;
	int line_number;
	char *remove_character;
	FEEDER_FILE_ROW *feeder_file_row;
} FEEDER_LOAD_FILE;

/* Usage */
/* ----- */
FEEDER_LOAD_FILE *feeder_load_file_fetch(
			DATE *feeder_load_date /* in/out */,
			char *feeder_account,
			char *feeder_load_filename,
			int date_column /* one_based */,
			int description_column /* one_based */,
			int debit_column /* one_based */,
			int credit_column /* one_based */,
			int balance_column /* one_based */,
			boolean reverse_order,
			LIST *feeder_phrase_list,
			LIST *feeder_exist_row_list,
			LIST *feeder_matched_journal_list );

/* Process */
/* ------- */
FEEDER_LOAD_FILE *feeder_load_file_calloc(
			void );

/* Returns heap memory */
/* ------------------- */
char *feeder_load_file_basename_filename(
			char *feeder_load_filename );

char *feeder_load_file_sha256sum(
			char *feeder_load_filename );

/* Public */
/* ------ */

/* Returns heap memory or null */
/* --------------------------- */
char *feeder_load_file_minimum_date(
			char *feeder_load_filename,
			int date_column /* one_based */ );

#define FEEDER_LOAD_EVENT_TABLE		"feeder_load_event"

typedef struct
{
	char *feeder_load_date_time;
	char *login_name;
	char *basename_filename;
	char *feeder_account;
	double ending_balance;
	char *sha256sum;
} FEEDER_LOAD_EVENT;

/* Usage */
/* ----- */
FEEDER_LOAD_EVENT *feeder_load_event_new(
			char *feeder_load_date_time,
			char *login_name,
			char *feeder_load_file_basename_filename,
			char *feeder_account,
			double feeder_load_ending_balance,
			char *feeder_load_file_sha256sum );

/* Process */
/* ------- */
FEEDER_LOAD_EVENT *feeder_load_event_calloc(
			void );

/* Public */
/* ------ */
boolean feeder_load_event_sha256sum_exists(
			char *feeder_load_event_table,
			char *feeder_load_file_sha256sum );

/* Driver */
/* ------ */
void feeder_load_event_insert(
			char *feeder_load_event_table,
			char *feeder_load_select,
			char *feeder_load_date_time,
			char *login_name,
			char *basename_filename,
			char *sha256sum,
			char *feeder_account,
			double ending_balance );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *feeder_load_event_insert_system_string(
			char *feeder_load_event_table,
			char *feeder_load_select,
			char sql_delimiter );

void feeder_load_event_insert_pipe(
			FILE *insert_pipe,
			char sql_delimiter,
			char *feeder_load_date_time,
			char *login_name,
			char *basename_filename,
			char *sha256sum,
			char *feeder_account,
			double ending_balance );

typedef struct
{
	LIST *feeder_phrase_list;
	char *feeder_load_file_minimum_date;
	LIST *feeder_exist_row_list;
	LIST *feeder_matched_journal_list;
	DATE *feeder_load_date;
	char *feeder_load_date_time;
	FEEDER_LOAD_FILE *feeder_load_file;
	boolean feeder_load_row_account_empty;
	double feeder_file_row_list_sum_amount;
	double prior_running_balance;
	boolean in_balance_boolean;
	double ending_balance;
	FEEDER_LOAD_EVENT *feeder_load_event;
} FEEDER_LOAD;

/* Usage */
/* ----- */
FEEDER_LOAD *feeder_load_new(
			char *login_name,
			char *feeder_account,
			char *feeder_load_filename,
			int date_column /* one_based */,
			int description_column /* one_based */,
			int debit_column /* one_based */,
			int credit_column /* one_based */,
			int balance_column /* one_based */,
			boolean reverse_order,
			double ending_balance );

/* Process */
/* ------- */
FEEDER_LOAD *feeder_load_calloc(
			void );

boolean feeder_load_row_account_empty(
			char *feeder_load_row_table,
			char *feeder_account );

double feeder_load_fetch_prior_running_balance(
			char *feeder_load_row_table,
			char *feeder_account );

void feeder_load_missing_running_balance_set(
			LIST *feeder_file_row_list,
			double prior_running_balance );

boolean feeder_load_in_balance_boolean(
			double feeder_load_prior_running_balance,
			LIST *feeder_file_row_list,
			double ending_balance );

double feeder_load_ending_balance(
			FEEDER_FILE_ROW *last_feeder_file_row );

#endif
