/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/transaction.h			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "list.h"
#include "date.h"
#include "boolean.h"

/* Constants */
/* --------- */
#define TRANSACTION_TABLE			"transaction"
#define TRANSACTION_BUFFER_TRIM			60
#define TRANSACTION_CLOSING_TRANSACTION_TIME	"23:59:59"
#define TRANSACTION_PRIOR_TRANSACTION_TIME	"23:59:58"
#define TRANSACTION_CLOSING_ENTRY_MEMO		"close closing"
#define TRANSACTION_FOLDER_NAME			"transaction"
#define TRANSACTION_SEMAPHORE_KEY		12227

/* Structures */
/* ---------- */

typedef struct
{
	char *full_name;
	char *street_address;
	char *transaction_date_time;
	double transaction_amount;
	char *memo;
	int check_number;
	LIST *journal_list;
	boolean lock_transaction;
	char *property_street_address;
	char *program_name;
} TRANSACTION;

TRANSACTION *transaction_calloc(
			void );

TRANSACTION *transaction_new(
			char *full_name,
			char *street_address,
			char *transaction_date_time );

TRANSACTION *transaction_fetch(
			char *full_name,
			char *street_address,
			char *transaction_date_time );

/* TRANSACTION without any additions */
/* --------------------------------- */
FILE *transaction_insert_open(
			boolean replace );

/* Returns inserted transaction_date_time */
/* -------------------------------------- */
char *transaction_insert(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double transaction_amount,
			char *memo,
			int check_number,
			boolean lock_transaction,
			boolean replace );

/* Returns inserted transaction_date_time */
/* -------------------------------------- */
char *transaction_journal_insert(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double transaction_amount,
			char *memo,
			int check_number,
			boolean lock_transaction,
			LIST *journal_list,
			boolean replace );

/* Returns inserted transaction_date_time */
/* -------------------------------------- */
char *transaction_insert_pipe(
			FILE *insert_pipe,
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double transaction_amount,
			char *memo,
			int check_number,
			boolean lock_transaction );

/* Returns transaction_list */
/* ------------------------ */
LIST *transaction_list_insert(
			LIST *transaction_list );

void transaction_list_stderr(
			LIST *transaction_list );

/* TRANSACTION with program_name addition */
/* -------------------------------------- */
FILE *transaction_program_insert_open(
			boolean replace );

/* Returns inserted transaction_date_time */
/* -------------------------------------- */
char *transaction_program_insert(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *program_name,
			double transaction_amount,
			char *memo,
			int check_number,
			boolean lock_transaction,
			boolean replace );

/* Returns inserted transaction_date_time */
/* -------------------------------------- */
char *transaction_program_insert_pipe(
			FILE *insert_pipe,
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *program_name,
			double transaction_amount,
			char *memo,
			int check_number,
			boolean lock_transaction );

/* TRANSACTION with property_street_address addition */
/* ------------------------------------------------- */
FILE *transaction_property_insert_open(
			boolean replace );

/* Returns inserted transaction_date_time */
/* -------------------------------------- */
char *transaction_property_insert(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *property_street_address,
			double transaction_amount,
			char *memo,
			int check_number,
			boolean lock_transaction );

/* Returns inserted transaction_date_time */
/* -------------------------------------- */
char *transaction_property_insert_pipe(
			FILE *insert_pipe,
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *property_street_address,
			double transaction_amount,
			char *memo,
			int check_number,
			boolean lock_transaction );

/* Returns transaction_list */
/* ------------------------ */
LIST *transaction_list_property_insert(
			LIST *transaction_list );

/* Returns program memory */
/* ---------------------- */
char *transaction_select(
			void );

/* Safely returns heap memory */
/* -------------------------- */
char *transaction_primary_where(
			char *full_name,
			char *street_address,
			char *transaction_date_time );

/* Also fetches journal_list() */
/* --------------------------- */
TRANSACTION *transaction_parse(
			char *input,
			boolean fetch_journal_list );

TRANSACTION *transaction_seek(
			LIST *transaction_list,
			char *full_name,
			char *street_address,
			char *transaction_date_time );

/* Executes journal_propagate() */
/* ---------------------------- */
void transaction_delete(char *full_name,
			char *street_address,
			char *transaction_date_time );

/* ======== */
/* FEATURES */
/* ======== */

/* Returns static memory */
/* --------------------- */
char *transaction_escape_full_name(
			char *full_name );
char *transaction_full_name_escape(
			char *full_name );

/* Returns race-free transaction_date_time */
/* --------------------------------------- */
char *transaction_race_free(
			char *transaction_date_time );

boolean transaction_exists(
			char *transaction_date_time );

char *transaction_audit(
			TRANSACTION *transaction );

void transaction_update(
			double transaction_amount,
			char *full_name,
			char *street_address,
			char *transaction_date_time );

TRANSACTION *transaction_full(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double transaction_amount,
			char *memo );

TRANSACTION *transaction_binary(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double transaction_amount,
			char *memo,
			char *debit_account,
			char *credit_account );

LIST *transaction_binary_journal_list(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double transaction_amount,
			char *debit_account,
			char *credit_account );

TRANSACTION *transaction_check_seek(
			LIST *transaction_list,
			int check_number );

void transaction_check_insert(
			FILE *insert_pipe,
			int check_number );

void transaction_lock_insert(
			FILE *insert_pipe,
			boolean lock_transaction );

char *transaction_journal_refresh(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double transaction_amount,
			char *memo,
			int check_number,
			LIST *journal_list );

char *transaction_refresh(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double transaction_amount,
			char *memo,
			int check_number,
			LIST *journal_list );

char *transaction_program_refresh(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *program_name,
			double transaction_amount,
			char *memo,
			int check_number,
			LIST *journal_list );

/* Returns begin_date_string */
/* ------------------------- */
char *transaction_report_title_sub_title(
			char *title,
			char *sub_title,
			char *process_name,
			char *fund_name,
			char *as_of_date,
			int fund_name_list_length,
			char *logo_filename );

DATE *transaction_prior_closing_transaction_date(
			char *fund_name,
			char *ending_transaction_date );

/* Returns heap memory. */
/* -------------------- */
char *transaction_date_time_max(
			void );

/* Returns heap memory. */
/* -------------------- */
char *transaction_date_max(
			void );

char *transaction_fund_where(
			char *fund_name );

boolean transaction_fund_attribute_exists(
			void );

LIST *transaction_fund_name_list(
			void );

LIST *transaction_list_fetch(
			char *where );

LIST *transaction_system_list(
			char *sys_string );

char *transaction_journal_join(
			void );

char *transaction_time_append(
			char *transaction_date );

char *transaction_date_maximum(
			void );

char *transaction_date_minimum(
			void );

char *transaction_date_maximum(
			void );

char *transaction_beginning_date_string(
			char *fund_name,
			char *ending_transaction_date );

char *transaction_date_prior_closing_beginning(
			char *fund_name,
			char *as_of_date );

double transaction_net_income_fetch(
			char *fund_name,
			char *as_of_date );

double transaction_net_income(
			double total_revenues,
			double total_expenses,
			double total_gains,
			double total_losses );

double transaction_net_income_fetch(
			char *fund_name,
			char *as_of_date );

char *transaction_closing_transaction_date_time(
			char *as_of_date );

void transaction_journal_list_pipe_display(
			FILE *output_pipe,
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *memo,
			LIST *journal_list );

LIST *transaction_after_balance_zero_journal_list(
			char *account_name );

char *transaction_latest_zero_balance_transaction_date_time(
			char *account_name );

char *transaction_date_time_closing(
			char *as_of_date );

char *transaction_closing_date_time(
			char *as_of_date );

boolean transaction_date_time_exists(
			char *transaction_date_time );

/* Returns the existing transaction_date_time */
/* ------------------------------------------ */
char *transaction_exists_closing_entry(
			char *as_of_date );

char *transaction_existing_closing_date_time(
			char *as_of_date );

char *transaction_exists_closing_date_time(
			char *as_of_date );

LIST *transaction_date_time_account_name_list(
			char *transaction_date_time );

void transaction_list_stderr(
			LIST *transaction_list );

/* Returns static memory */
/* --------------------- */
char *transaction_full_name_display(
			char *full_name,
			char *street_address );

void transaction_list_journal_insert(
			LIST *transaction_list );

char *transaction_sys_string(
			char *where );

LIST *transaction_journal_list(
			char *full_name,
			char *street_address,
			char *transaction_date_time );

TRANSACTION *transaction_full(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double transaction_amount,
			char *memo );

char *transaction_generate_date_time(
			char *transaction_date );

void transaction_list_html_display(
			LIST *transaction_list );

void transaction_stderr(
			TRANSACTION *transaction );

/* ---------------------------------------------- */
/* Returns account_name_list			  */
/* ---------------------------------------------- */
/* Note: transaction_date_time changed if needed. */
/* ---------------------------------------------- */
LIST *transaction_list_journal_program_insert(
			char **first_transaction_date_time,
			LIST *transaction_list,
			boolean replace );

/* ------------------------- */
/* Returns account_name_list */
/* ------------------------- */
LIST *transaction_list_delete(
			LIST *transaction_list );

#endif
