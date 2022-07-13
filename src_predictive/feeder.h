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
#include "journal.h"

#define FEEDER_LOAD_ROW_TABLE		"feeder_load_row"

#define FEEDER_KEYS_MATCH_SUM_MAX 	18
#define FEEDER_DESCRIPTION_SIZE		140

typedef struct
{
	char *basename_filename;
	char *sha256sum;
	boolean feeder_load_event_sha256sum_exists;
	DATE *feeder_load_file_date;
	FILE *file;
	char input[ 1024 ];
	LIST *feeder_load_row_list;
	int line_count;
	char *remove_character;
	FEEDER_LOAD_FILE_ROW *feeder_load_file_row;
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
			boolean reverse_order,
			LIST *feeder_phrase_list,
			LIST *feeder_load_exist_row_list );

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

#define FEEDER_LOAD_EXIST_ROW_SELECT		"feeder_date,"		\
						"feeder_description,"	\
						"transaction_date_time"

typedef struct
{
	char *feeder_date;
	char *feeder_description;
	char *transaction_date_time;
} FEEDER_LOAD_EXIST_ROW;

/* Usage */
/* ----- */
LIST *feeder_load_exist_row_list(
			char *feeder_account,
			char *feeder_load_file_minimum_date );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *feeder_load_exist_row_where(
			char *feeder_account,
			char *feeder_load_file_minimum_date );

/* Returns heap memory */
/* ------------------- */
char *feeder_load_exist_row_system_string(
			char *feeder_load_exist_row_select,
			char *feeder_load_row_table,
			char *feeder_load_exist_row_where );

FEEDER_LOAD_EXIST_ROW *feeder_load_exist_row_parse(
			char *input );

FEEDER_LOAD_EXIST_ROW *feeder_load_exist_row_calloc(
			void );

/* Public */
/* ------ */
FEEDER_LOAD_EXIST_ROW *feeder_load_exist_row_seek(
			char *international_date,
			char *description_embedded,
			LIST *feeder_load_exist_row_list );

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

#define FIELD_LOAD_FILE_ROW_INSERT	"full_name,"			\
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
	FEEDER_LOAD_EXIST_ROW *feeder_load_exist_row_seek;
	FEEDER_PHRASE *feeder_phrase_seek;
	char *transaction_date_time;
} FEEDER_LOAD_FILE_ROW;

/* Usage */
/* ----- */
FEEDER_LOAD_FILE_ROW *feeder_load_file_row_new(
			DATE *feeder_load_file_date /* in/out */,
			int date_column /* one_based */,
			int description_column /* one_based */,
			int debit_column /* one_based */,
			int credit_column /* one_based */,
			int balance_column /* one_based */,
			LIST *feeder_phrase_list,
			LIST *feeder_load_table_row_list,
			char *input,
			int line_number );

/* Process */
/* ------- */
FEEDER_LOAD_FILE_ROW *feeder_load_file_row_calloc(
			void );


/* Returns heap memory or null */
/* --------------------------- */
char *feeder_load_file_row_international_date(
			char *american_date );

double feeder_load_file_row_amount(
			double debit,
			double credit );

int feeder_load_file_row_check_number(
			char *feeder_description );

/* Returns heap memory */
/* ------------------- */
char *feeder_load_file_row_transaction_date_time(
			char *feeder_load_file_row_international_date,
			char *date_hms );

/* Usage */
/* ----- */

/* Returns heap memory or description */
/* ---------------------------------- */
char *feeder_load_file_row_description_embedded(
			char *description,
			double balance,
			int line_number,
			int feeder_load_file_row_check_number );

/* Returns static memory */
/* --------------------- */
char *feeder_load_file_row_trim_date(
			char *description );

/* Returns heap memory */
/* ------------------- */
char *feeder_load_file_row_description_build(
			char *sed_trim_double_spaces,
			double balance,
			int line_number );

/* Returns feeder_load_file_row_description_build */
/* ---------------------------------------------- */
char *feeder_load_file_row_description_crop(
			char *feeder_load_file_row_description_build,
			int feeder_description_size );

/* Public */
/* ------ */
double feeder_load_file_row_last_running_balance(
			boolean reverse_order,
			LIST *feeder_load_file_row_list );

FEEDER_LOAD_FILE_ROW *
	feeder_load_file_row_check_number_seek(
			int check_number,
			LIST *feeder_load_file_row_list );

FEEDER_LOAD_FILE_ROW *
	feeder_load_file_row_amount_seek(
			double debit_amount,
			double credit_amount,
			LIST *feeder_load_file_row_list );

/* Driver */
/* ------ */
void feeder_load_file_row_list_insert(
			char *feeder_account,
			char *feeder_load_date_time,
			LIST *feeder_file_load_row_list,
			LIST *feeder_journal_list );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *feeder_load_file_row_list_insert_system_string(
			char *feeder_load_row_insert,
			char *feeder_load_row_table,
			char sql_delimiter );

FILE *feeder_load_file_row_list_insert_pipe(
			char *feeder_load_file_row_list_insert_system_string );

void feeder_load_file_row_insert(
			FILE *feeder_load_file_row_list_insert_pipe,
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *feeder_account,
			char *feeder_load_file_row_international_date,
			char *feeder_load_file_row_description_embedded,
			double feeder_load_file_row_amount,
			double feeder_load_file_row_balance,
			int row_number,
			char *feeder_phrase,
			char *feeder_load_date_time,
			char sql_delimiter );

typedef struct
{
	FEEDER_LOAD_FILE_ROW *feeder_load_file_row;
	JOURNAL *journal;
	boolean taken;
} FEEDER_JOURNAL;

/* Usage */
/* ----- */
FEEDER_JOURNAL *feeder_journal_new(
			LIST *feeder_load_row_list,
			JOURNAL *journal );

/* Process */
/* ------- */
FEEDER_JOURNAL *feeder_journal_calloc(
			void );

/* Public */
/* ------ */
FEEDER_JOURNAL *feeder_journal_row_seek(
			FEEDER_LOAD_FILE_ROW *feeder_load_file_row,
			LIST *feeder_journal_list );

typedef struct
{
	char *subquery;
	char *where;
	LIST *journal_system_list;
	JOURNAL *journal;
	LIST *feeder_journal_list;
} FEEDER_NOT_MATCHED_JOURNAL_LIST;

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
FEEDER_NOT_MATCHED_JOURNAL_LIST *
	feeder_not_matched_journal_list_new(
			char *feeder_account,
			LIST *feeder_load_file_row_list );

/* Process */
/* ------- */
FEEDER_NOT_MATCHED_JOURNAL_LIST *
	feeder_not_matched_journal_list_calloc(
			void );

/* Returns heap memory */
/* ------------------- */
char *feeder_not_matched_journal_list_subquery(
			char *feeder_account,
			char *account_uncleared_checks,
			char *journal_table,
			char *feeder_load_row_table );

/* Returns heap memory */
/* ------------------- */
char *feeder_not_matched_journal_list_where(
			char *feeder_account,
			char *account_uncleared_checks,
			char *feeder_not_matched_journal_list_subquery );

typedef struct
{
	FEEDER_JOURNAL_ROW *feeder_journal_row_seek;
	FEEDER_LOAD_FILE_ROW *feeder_load_file_row;
	char *debit_account;
	char *credit_account;
	TRANSACTION *transaction;
} FEEDER_TRANSACTION;

/* Usage */
/* ----- */
FEEDER_TRANSACTION *feeder_transaction_new(
			DATE *feeder_load_date /* in/out */,
			char *feeder_account,
			FEEDER_LOAD_FILE_ROW *feeder_load_file_row,
			LIST *feeder_journal_list );

/* Process */
/* ------- */
FEEDER_TRANSACTION *feeder_transaction_calloc(
			void );

/* Public */
/* ------ */
FEEDER_TRANSACTION *feeder_transaction_row_seek(
			FEEDER_LOAD_FILE_ROW *feeder_load_file_row,
			LIST *feeder_transaction_list );

typedef struct
{
	LIST *list;
} FEEDER_TRANSACTION_LIST;

/* Usage */
/* ----- */
FEEDER_TRANSACTION_LIST *feeder_transaction_list_new(
			DATE *feeder_load_date /* in/out */,
			char *feeder_account,
			LIST *feeder_load_row_list,
			LIST *feeder_journal_list );

/* Process */
/* ------- */
FEEDER_TRANSACTION_LIST *feeder_transaction_list_calloc(
			void );

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
	FEEDER_LOAD_FILE *feeder_load_file;
	LIST *feeder_phrase_list;
	double feeder_load_row_last_running_balance;
	char *basename_filename;
	char *sha256sum;
	FEEDER_LOAD_EVENT *feeder_load_event;
} FEEDER_LOAD;

/* Usage */
/* ----- */
FEEDER_LOAD *feeder_load_new(
			char *process_name,
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

/* Returns heap memory */
/* ------------------- */
char *feeder_load_basename_filename(
			char *feeder_load_filename );


/* Returns heap memory */
/* ------------------- */
char *feeder_load_sha256sum(
			char *feeder_load_filename );

#endif
