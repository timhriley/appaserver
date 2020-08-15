/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/bank_upload.h			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef BANK_UPLOAD_H
#define BANK_UPLOAD_H

#include "transaction.h"
#include "journal.h"
#include "reoccurring.h"

/* Enumerated types */
/* ---------------- */
enum bank_upload_exception {	bank_upload_exception_none,
				duplicated_spreadsheet_file,
				empty_transaction_rows,
				sequence_number_not_generated,
				internal_read_permission };

/* Constants */
/* --------- */
#define BANK_UPLOAD_DESCRIPTION_SIZE	140
#define BANK_UPLOAD_FILENAME_SIZE	80
#define BANK_UPLOAD_TABLE_NAME		"bank_upload"

/* Look ahead count for keys_match_sum.e */
/* ------------------------------------- */
#define TRANSACTIONS_CHECK_COUNT	20

#define INSERT_BANK_UPLOAD		\
	"bank_date,bank_description,sequence_number,bank_amount,bank_upload_date_time"

#define INSERT_BANK_UPLOAD_EVENT		\
	"bank_upload_date_time,login_name,bank_upload_filename,file_sha256sum,feeder_account"

#define INSERT_BANK_UPLOAD_EVENT_FUND		\
	"bank_upload_date_time,login_name,bank_upload_filename,file_sha256sum,feeder_account,fund"

#define INSERT_BANK_UPLOAD_ARCHIVE	\
	"bank_date,bank_description,sequence_number,bank_amount,bank_running_balance,bank_upload_date_time"

/* Structures */
/* ---------- */
typedef struct
{
	char *bank_date;
	char *bank_description;
	char *bank_description_embedded;
	int sequence_number;
	double bank_amount;
	double bank_running_balance;
	char *fund_name;
	int check_number;
	boolean existing_bank_upload;
	boolean existing_transaction;
	TRANSACTION *feeder_phrase_match_build_transaction;
	LIST *reconciled_transaction_list;
	LIST *feeder_match_sum_existing_journal_list;
	JOURNAL *feeder_check_number_existing_journal;
} BANK_UPLOAD;

typedef struct
{
	LIST *bank_upload_list;
	char *input_filename;
	char *file_sha256sum;
	char *minimum_bank_date;
	int file_row_count;
	int table_insert_count;
	int date_piece_offset;
	int description_piece_offset;
	int debit_piece_offset;
	int credit_piece_offset;
	int balance_piece_offset;
	LIST *error_line_list;
} BANK_UPLOAD_FILE;

typedef struct
{
	BANK_UPLOAD_FILE file;
	LIST *existing_cash_journal_list;
	LIST *non_reconciled_cash_journal_list;
	LIST *uncleared_checks_transaction_list;
	REOCCURRING_STRUCTURE *reoccurring_structure;
	int starting_sequence_number;
	char *fund_name;
	char *feeder_account;
	char *bank_upload_date_time;
} BANK_UPLOAD_STRUCTURE;

/* Operations */
/* ---------- */
BANK_UPLOAD_STRUCTURE *bank_upload_structure_calloc(
			void );

BANK_UPLOAD_STRUCTURE *bank_upload_structure_new(
			char *fund_name,
			char *feeder_account,
			char *input_filename,
			boolean reverse_order,
			int date_piece_offset,
			int description_piece_offset,
			int debit_piece_offset,
			int credit_piece_offset,
			int balance_piece_offset );

BANK_UPLOAD *bank_upload_calloc(
			void );

BANK_UPLOAD *bank_upload_new(
			char *bank_date,
			char *bank_description );

void bank_upload_transaction_direct_insert(
			char *bank_date,
			char *bank_description_embedded,
			char *full_name,
			char *street_address,
			char *transaction_date_time );

/* Returns table_insert_count */
/* -------------------------- */
int bank_upload_insert(
			char *fund_name,
			LIST *bank_upload_list,
			char *bank_upload_date_time );

int bank_upload_line_count(
			char *input_filename,
			int date_piece_offset );

boolean bank_upload_bank_date_international(
			char *bank_date_international,
			char *bank_date );

LIST *bank_upload_bank_upload_table_list(	
			int starting_sequence_number,
			char *begin_date );

BANK_UPLOAD *bank_upload_fetch(
			char *bank_date,
			char *bank_description );

LIST *bank_upload_existing_cash_journal_list(
			char *fund_name );

LIST *bank_upload_uncleared_checks_transaction_list(
			char *minimum_transaction_date,
			char *fund_name );

void bank_upload_set_transaction(
			LIST *bank_upload_list,
			LIST *reoccurring_transaction_list,
			LIST *existing_cash_journal_list );

/* Insert into TRANSACTION and JOURNAL  */
/* ------------------------------------ */
void bank_upload_transaction_insert(
			LIST *bank_upload_list );

char *bank_upload_select(
			void );

BANK_UPLOAD *bank_upload_parse(	
			char *input );

void bank_upload_transaction_table_display(
			LIST *bank_upload_list );

void bank_upload_transaction_text_display(
			LIST *bank_upload_list );

void bank_upload_journal_text_display(
			JOURNAL *journal );

void bank_upload_table_display(
			LIST *bank_upload_list );

int bank_upload_starting_sequence_number(
			char *input_filename,
			int date_piece_offset );

BANK_UPLOAD *bank_upload_dictionary_extract(
			DICTIONARY *dictionary );

char *bank_upload_description_embedded(
			char *bank_description,
			char *fund_name,
			double bank_amount,
			double bank_running_balance );

/* Sets:
		bank_upload->bank_date
		bank_upload->bank_description
		bank_upload->bank_description_embedded
		bank_upload->sequence_number
		bank_upload->bank_amount
		bank_upload->bank_running_balance
*/
LIST *bank_upload_file_list(
					LIST *error_line_list,
					char **file_sha256sum,
					char **minimum_bank_date,
					char *input_filename,
					boolean reverse_order,
					int date_piece_offset,
					int description_piece_offset,
					int debit_piece_offset,
					int credit_piece_offset,
					int balance_piece_offset,
					int starting_sequence_number,
					char *fund_name );

char *bank_upload_transaction_memo(
					char *full_name,
					char *street_address,
					char *transaction_date_time );

void bank_upload_event_insert(		char *bank_upload_date_time,
					char *login_name,
					char *bank_upload_filename,
					char *file_sha256sum,
					char *fund_name,
					char *feeder_account );

boolean bank_upload_sha256sum_exists(	 char *file_sha256sum );

void bank_upload_archive_insert(	char *fund_name,
					LIST *bank_upload_list,
					char *bank_upload_date_time );

int bank_upload_feeder_phrase_match_transaction_count(
					LIST *bank_upload_list );

boolean bank_upload_transaction_exists(
					char *bank_date,
					char *bank_description );

char *bank_upload_primary_where(
			char *bank_date,
			char *bank_description_embedded );

double bank_upload_bank_amount(
			char *bank_date,
			char *bank_description );

/* --------------------------------------------------- */
/* Returns transaction_date_time or null if not found. */
/* Message will help to explain not found.	       */
/* --------------------------------------------------- */
char *bank_upload_pending_amount_withdrawal(
					char *message,
					char **full_name,
					double amount );

/* --------------------------------------------------- */
/* Returns transaction_date_time or null if not found. */
/* Message will help to explain not found.	       */
/* --------------------------------------------------- */
char *bank_upload_pending_amount_deposit(
					char *message,
					char **full_name,
					double amount );

BANK_UPLOAD *bank_upload_parse_row(	char *input_row );

BANK_UPLOAD *bank_upload_sequence_number_fetch(
					int sequence_number );

BANK_UPLOAD *bank_upload_prior_fetch(
					int sequence_number );

double bank_upload_archive_latest_running_balance(
					void );

/* Insert into BANK_UPLOAD_TRANSACTION */
/* ----------------------------------- */
void bank_upload_reconciliation_transaction_insert(
					char *bank_date,
					char *bank_description_embedded,
					LIST *transaction_list );

LIST *bank_upload_feeder_transaction_list(
					char *fund_name,
					char *bank_description,
					double abs_bank_amount,
					double exact_value,
					boolean select_debit );

char *bank_upload_transaction_bank_upload_subquery(
					void );

char *bank_upload_transaction_journal_subquery(
					void );

LIST *bank_upload_transaction_list_string_parse(
	/* ------------------------------------------------------------ */
	/* Format: full_name^street_address^transaction_date_time[|...] */
	/* ------------------------------------------------------------ */
	char *transaction_list_string,
	char delimiter );

void bank_upload_transaction_balance_propagate(
					char *bank_date );

char *bank_upload_account_html(
		boolean existing_bank_upload,
		boolean existing_transaction,
		TRANSACTION *feeder_phrase_match_build_transaction,
		JOURNAL *feeder_check_number_existing_journal,
		LIST *match_sum_existing_journal_list );

char *bank_upload_description_crop(	char *bank_description );

char *bank_upload_unique_bank_description(
					boolean exists_fund,
					char *fund_name,
					char *input_bank_description,
					char *bank_amount,
					char *bank_balance );

LIST *bank_upload_transaction_date_time_list(
					char *minimum_transaction_date,
					char *account_name );

void bank_upload_set_reoccurring_transaction(
				LIST *bank_upload_list,
				LIST *reoccurring_transaction_list,
				LIST *non_reconciled_cash_journal_list );

int bank_upload_parse_check_number(
				char *bank_description );

/* Does journal_propagate() */
/* ------------------------ */
void bank_upload_cleared_checks_update(
			char *fund_name,
			LIST *bank_upload_list );

void bank_upload_cleared_journal_text_display(
			JOURNAL *feeder_check_number_existing_journal );

char *bank_upload_insert_bank_upload_filename(
			char *bank_upload_filename );

/* Insert into BANK_UPLOAD_TRANSACTION */
/* ----------------------------------- */
void bank_upload_direct_bank_upload_transaction_insert(
			LIST *bank_upload_list );

boolean bank_upload_exists(	char *bank_date,
				char *bank_description_embedded,
				char *minimum_bank_date );

void bank_upload_free(		BANK_UPLOAD *b );

int bank_upload_file_row_count(
				LIST *bank_upload_list );

/* Returns strdup() */
/* ---------------- */
char *bank_upload_journal_list_html(
				LIST *match_sum_existing_journal_list );

void bank_upload_match_sum_existing_journal_list(
				LIST *bank_upload_list,
				LIST *existing_cash_journal_list );

void bank_upload_feeder_phrase_match_build_transaction(
				LIST *bank_upload_list,
				LIST *reoccurring_transaction_list );

void bank_upload_check_number_existing_journal(
				LIST *bank_upload_list,
				LIST *existing_cash_journal_list );

LIST *bank_upload_key_list(
			char *minimum_bank_date );

/* Returns static memory */
/* --------------------- */
char *bank_upload_minimum_bank_date(
			char *minimum_bank_date,
			LIST *bank_upload_list );

/* Returns static memory */
/* --------------------- */
char *bank_upload_minimum_transaction_date(
			JOURNAL *feeder_check_number_existing_journal,
			LIST *feeder_match_sum_existing_journal_list );

BANK_UPLOAD *bank_upload_parse(
			char *input );

LIST *bank_upload_transaction_list(
			LIST *bank_upload_list );

#endif

