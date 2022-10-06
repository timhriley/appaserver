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
#include "html_table.h"

#define FEEDER_LOAD_TRANSACTION_DAYS_AGO				\
					"feeder_load_transaction_days_ago"

#define FEEDER_DESCRIPTION_SIZE		140

#define FEEDER_EXIST_ROW_SELECT		"feeder_date,"			\
					"file_row_description,"		\
					"transaction_date_time,"	\
					"file_row_balance"

typedef struct
{
	char *feeder_date;
	char *file_row_description;
	char *transaction_date_time;
	double file_row_balance;
} FEEDER_EXIST_ROW;

/* Usage */
/* ----- */
LIST *feeder_exist_row_list(
			char *feeder_account_name,
			char *feeder_load_file_minimum_date );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *feeder_exist_row_where(
			char *feeder_account_name,
			char *feeder_load_file_minimum_date );

/* Returns heap memory */
/* ------------------- */
char *feeder_exist_row_system_string(
			char *feeder_exist_row_select,
			char *feeder_row_table,
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
					"street_address"

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
			char *file_row_description,
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
	char *check_update_statement;
} FEEDER_MATCHED_JOURNAL;

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
LIST *feeder_matched_journal_list(
			char *feeder_account_name,
			char *feeder_row_table,
			char *feeder_load_file_minimum_date,
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
			char *feeder_account_name,
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

/* Usage */
/* ----- */
FEEDER_MATCHED_JOURNAL *
	feeder_matched_journal_check_seek(
			char *feeder_account_name,
			char *account_uncleared_checks,
			int check_number,
			double amount,
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

/* Public */
/* ------ */
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
			char *feeder_account_name,
			FEEDER_PHRASE *feeder_phrase_seek,
			double file_row_amount,
			char *transaction_date_time,
			char *memo );

/* Process */
/* ------- */
FEEDER_TRANSACTION *feeder_transaction_calloc(
			void );

typedef struct
{
	char *full_name;
	char *street_address;
	char *transaction_date_time;
	char *feeder_account_name;
	char *feeder_date;
	char *feeder_phrase;
	char *feeder_load_date_time;
	char *american_date;
	char *international_date;
	char *file_row_description;
	double debit;
	double credit;
	double file_row_amount;
	double file_row_balance;
	char *reference_string;
	int check_number;
	int row_number;
	char *description_embedded;
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
			char *file_row_description );

/* Usage */
/* ----- */

/* Returns heap memory or file_row_description */
/* ------------------------------------------- */
char *feeder_load_row_description_embedded(
			char *file_row_description,
			double balance,
			char *reference_string,
			int feeder_load_row_check_number );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *feeder_load_row_trim_date(
			char *file_row_description );

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

/* -------------------------------------- */
/* Returns this->file_row_balance or null */
/* -------------------------------------- */
double feeder_load_row_account_end_balance(
			FEEDER_LOAD_ROW *last_feeder_load_row );

/* -------------------------------------- */
/* Returns this->file_row_balance or null */
/* -------------------------------------- */
double feeder_load_row_prior_account_end_balance(
			FEEDER_LOAD_ROW *first_feeder_load_row );

#define FEEDER_ROW_TABLE		"feeder_row"

#define FEEDER_ROW_SELECT		"feeder_account,"		\
					"feeder_load_date_time,"	\
					"feeder_date,"			\
					"row_number,"			\
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
					"row_number,"			\
					"full_name,"			\
					"street_address,"		\
					"transaction_date_time,"	\
					"file_row_description,"		\
					"file_row_amount,"		\
					"file_row_balance,"		\
					"calculate_balance,"		\
					"check_number,"			\
					"feeder_phrase"

typedef struct
{
	char *feeder_account_name;
	char *feeder_load_date_time;
	char *feeder_date;
	int row_number;
	char *full_name;
	char *street_address;
	char *transaction_date_time;
	char *file_row_description;
	double file_row_amount;
	double file_row_balance;
	FEEDER_LOAD_ROW *feeder_load_row;
	FEEDER_EXIST_ROW *feeder_exist_row_seek;
	FEEDER_MATCHED_JOURNAL *feeder_matched_journal;
	FEEDER_PHRASE *feeder_phrase_seek;
	char *memo;
	FEEDER_TRANSACTION *feeder_transaction;
	double calculate_balance;
	int check_number;
	char *feeder_phrase;
} FEEDER_ROW;

/* Usage */
/* ----- */
LIST *feeder_row_list(	char *feeder_account_name,
			char *account_uncleared_checks,
			LIST *feeder_phrase_list,
			LIST *feeder_exist_row_list,
			LIST *feeder_matched_journal_list,
			LIST *feeder_load_row_list );

/* Process */
/* ------- */
DATE *feeder_row_date(	void );

/* Usage */
/* ----- */
FEEDER_ROW *feeder_row_new(
			DATE *feeder_load_date /* in/out */,
			char *feeder_account_name,
			char *account_uncleared_checks,
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
void feeder_row_list_calculate_balance_set(
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
			LIST *feeder_row_list,
			FEEDER_ROW *feeder_row_first_out_balance );

/* Process */
/* ------- */

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *feeder_row_system_string(
			char *feeder_row_select,
			char *feeder_row_table,
			char *where );

/* Process */
/* ------- */

/* Usage */
/* ----- */
LIST *feeder_row_system_list(
			char *feeder_row_system_string );

/* Process */
/* ------- */

/* Always succeeds */
/* --------------- */
FILE *feeder_row_input_pipe(
			char *feeder_row_system_string );

/* Usage */
/* ----- */
FEEDER_ROW *feeder_row_parse(
			char *input );

/* Process */
/* ------- */

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
int feeder_row_maximum_row_number(
			char *feeder_row_table,
			char *feeder_account_name,
			char *feeder_load_date_time );

/* Process */
/* ------- */

/* Usage */
/* ----- */
FEEDER_ROW *feeder_row_fetch(
			char *feeder_account_name,
			char *feeder_load_date_time,
			int row_number );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *feeder_row_primary_where(
			char *feeder_account_name,
			char *feeder_load_date_time,
			int row_number );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *feeder_row_maximum_transaction_date_time(
			char *feeder_row_table,
			char *feeder_account_name,
			char *feeder_load_date_time );

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
			int balance_column,
			LIST *feeder_row_list );

/* Process */
/* ------- */
LIST *feeder_row_error_extract_list(
			LIST *feeder_row_list );

/* Driver */
/* ------ */
boolean feeder_row_list_display(
			int balance_column,
			LIST *feeder_row_list,
			FEEDER_ROW *feeder_row_first_out_balance );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *feeder_row_list_display_system_string(
			int balance_column );

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
			int balance_column,
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
			char *feeder_account_name,
			char *feeder_load_date_time,
			char *international_date,
			int row_number,
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *description_embedded,
			double file_row_amount,
			double file_row_balance,
			double calculate_balance,
			int check_number,
			char *feeder_row_phrase,
			char sql_delimiter );

/* Driver */
/* ------ */
void feeder_row_transaction_insert(
			char *feeder_account_name,
			char *account_uncleared_checks,
			LIST *feeder_row_list,
			FEEDER_ROW *feeder_row_first_out_balance );

/* Usage */
/* ----- */
LIST *feeder_row_extract_transaction_list(
			LIST *feeder_row_list,
			FEEDER_ROW *feeder_row_first_out_balance );

/* Process */
/* ------- */

/* Usage */
/* ----- */

/* Returns first_transaction_date_time */
/* ----------------------------------- */
char *feeder_row_check_journal_update(
			LIST *feeder_row_list,
			FEEDER_ROW *feeder_row_first_out_balance );

/* Process */
/* ------- */
FILE *feeder_row_check_journal_update_pipe(
			void );

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

#define FEEDER_LOAD_EVENT_SELECT	"feeder_account,"	\
					"feeder_load_date_time,"\
					"login_name,"		\
					"feeder_load_filename,"	\
					"account_end_date,"	\
					"account_end_balance"

#define FEEDER_LOAD_EVENT_INSERT	"feeder_account,"	\
					"feeder_load_date_time,"\
					"login_name,"		\
					"feeder_load_filename,"	\
					"account_end_date,"	\
					"account_end_balance"

typedef struct
{
	char *feeder_account_name;
	char *feeder_load_date_time;
	char *login_name;
	char *basename_filename;
	char *account_end_date;
	double account_end_calculate_balance;
} FEEDER_LOAD_EVENT;

/* Usage */
/* ----- */
FEEDER_LOAD_EVENT *feeder_load_event_new(
			char *feeder_account_name,
			char *feeder_load_date_time,
			char *login_name,
			char *feeder_load_file_basename_filename,
			char *feeder_account_end_date,
			double feeder_account_ending_calculate_balance );

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

FEEDER_LOAD_EVENT *feeder_load_event_parse(
			char *input );

/* Usage */
/* ----- */
FEEDER_LOAD_EVENT *feeder_load_event_latest_fetch(
			char *feeder_load_event_table,
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
			char *feeder_load_event_table,
			char *feeder_load_event_account_where );

/* --------------------------- */
/* Returns heap memory or null */
/* --------------------------- */
char *feeder_load_event_latest_date_time(
			char *feeder_load_event_latest_system_string );

/* Driver */
/* ------ */
void feeder_load_event_insert(
			char *feeder_load_event_table,
			char *feeder_load_event_insert,
			char *feeder_account_name,
			char *feeder_load_date_time,
			char *login_name,
			char *basename_filename,
			char *account_end_date,
			double feeder_account_end_calculate_balance );

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
			char *feeder_account_name,
			char *feeder_load_date_time,
			char *login_name,
			char *basename_filename,
			char *account_end_date,
			double account_end_calculate_balance );

typedef struct
{
	char *account_uncleared_checks;
	LIST *feeder_phrase_list;
	char *feeder_load_file_minimum_date;
	LIST *feeder_exist_row_list;
	LIST *feeder_matched_journal_list;
	char *feeder_load_date_time;
	FEEDER_LOAD_FILE *feeder_load_file;
	LIST *feeder_row_list;
	FEEDER_ROW *feeder_row_first_out_balance;
	double account_end_balance;
	double prior_account_end_balance;
	char *account_end_date;
	double account_end_calculate_balance;
	FEEDER_LOAD_EVENT *feeder_load_event;
} FEEDER;

/* Usage */
/* ----- */
FEEDER *feeder_fetch(
			char *login_name,
			char *feeder_account_name,
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

/* Returns feeder_row->feeder_load_row->international_date or null */
/* --------------------------------------------------------------- */
char *feeder_account_end_date(
			LIST *feeder_row_list,
			FEEDER_ROW *feeder_row_first_out_balance );

double feeder_account_end_calculate_balance(
			LIST *feeder_row_list,
			FEEDER_ROW *feeder_row_first_out_balance );

/* Usage */
/* ----- */
double feeder_account_end_balance(
			char *feeder_account_name,
			int balance_column,
			double parameter_account_end_balance,
			LIST *feeder_row_list,
			FEEDER_ROW *feeder_row_first_out_balance );

/* Process */
/* ------- */

/* Usage */
/* ----- */
double feeder_prior_account_end_balance(
			char *feeder_account_name,
			char *feeder_load_file_minimum_date,
			boolean balance_column,
			double feeder_account_end_balance,
			LIST *feeder_row_list,
			FEEDER_ROW *feeder_row_first_out_balance );

/* Process */
/* ------- */

/* Driver */
/* ------ */

/* Returns heap memory or "" */
/* ------------------------- */
char *feeder_parameter_account_end_balance_error(
			double parameter_account_end_balance,
			FEEDER_ROW *feeder_row_first_out_balance,
			double feeder_account_end_calculate_balance,
			int feeder_row_seek_matched_count );

#define FEEDER_AUDIT_HTML_TITLE		"Feeder After Execute Audit"

typedef struct
{
	FEEDER_LOAD_EVENT *feeder_load_event;
	int feeder_row_maximum_row_number;
	FEEDER_ROW *feeder_row_fetch;
	char *feeder_row_maximum_transaction_date_time;
	JOURNAL *journal_account_fetch;
	double balance_difference;
	boolean difference_zero;
	HTML_TABLE *html_table;
} FEEDER_AUDIT;

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
FEEDER_AUDIT *feeder_audit_fetch(
			char *feeder_account_name );

/* Process */
/* ------- */
FEEDER_AUDIT *feeder_audit_calloc(
			void );

LIST *feeder_audit_html_heading_list(
			void );

double feeder_audit_balance_difference(
			double journal_balance,
			double calculate_balance );

boolean feeder_audit_difference_zero(
			double feeder_audit_balance_difference );

/* Usage */
/* ----- */
HTML_ROW *feeder_audit_html_row(
			FEEDER_ROW *feeder_row,
			double journal_balance,
			double feeder_audit_balance_difference,
			boolean feeder_audit_difference_zero );

/* Process */
/* ------- */
LIST *feeder_audit_html_cell_list(
			int row_number,
			char *full_name,
			char *file_row_description,
			char *transaction_date_time,
			char *feeder_date,
			double file_row_amount,
			int check_number,
			double calculate_balance,
			double journal_balance,
			double feeder_audit_balance_difference,
			boolean feeder_audit_difference_zero );

#endif
