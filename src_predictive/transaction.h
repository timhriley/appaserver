/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/transaction.h			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "list.h"
#include "boolean.h"
#include "journal.h"

/* Constants */
/* --------- */
#define TRANSACTION_FOLDER_NAME		"transaction"
#define TRANSACTION_SEMAPHORE_KEY	12227

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

/* Operations */
/* ---------- */
TRANSACTION *transaction_new(	char *full_name,
				char *street_address,
				char *transaction_date_time,
				char *memo );

TRANSACTION *transaction_fetch(	char *full_name,
				char *street_address,
				char *transaction_date_time );

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
				boolean lock_transaction );

/* Returns inserted transaction_date_time */
/* -------------------------------------- */
char *transaction_insert(	char *full_name,
				char *street_address,
				char *transaction_date_time,
				double transaction_amount,
				char *memo,
				int check_number,
				boolean lock_transaction );

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
				LIST *journal_list );

/* Returns inserted transaction_date_time */
/* -------------------------------------- */
char *transaction_program_journal_insert(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *program_name,
			double transaction_amount,
			char *memo,
			int check_number,
			boolean lock_transaction,
			LIST *journal_list );

char *transaction_property_select(
			void );

char *transaction_select(
			boolean with_program );

/* Returns static memory */
/* --------------------- */
char *transaction_escape_full_name(
			char *full_name );
char *transaction_full_name_escape(
			char *full_name );

/* Returns static memory */
/* --------------------- */
char *transaction_escape_memo(
			char *memo );

/* Safely returns heap memory */
/* -------------------------- */
char *transaction_primary_where(
			char *full_name,
			char *street_address,
			char *transaction_date_time );

TRANSACTION *transaction_program_parse(
			char *input_buffer );

TRANSACTION *transaction_property_parse(
			char *input_buffer );

TRANSACTION *transaction_parse(
			char *input_buffer );

char *transaction_unique_transaction_date_time(
			char *transaction_date_time );

boolean transaction_exists(
			char *transaction_date_time );

void transaction_journal_list_insert(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			LIST *journal_list );

void transaction_delete(char *full_name,
			char *street_address,
			char *transaction_date_time );

/* Returns inserted transaction_date_time */
/* -------------------------------------- */
char *transaction_refresh(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double transaction_amount,
			char *memo,
			int check_number,
			boolean lock_transaction,
			LIST *journal_list );

char *transaction_display(
			TRANSACTION *transaction );

TRANSACTION *transaction_seek(
			LIST *transaction_list,
			char *full_name,
			char *street_address,
			char *transaction_date_time );

/* Returns transaction_list with transaction_date_time changed if needed. */
/* ---------------------------------------------------------------------- */
LIST *ledger_transaction_list_insert(	LIST *transaction_list,
					char *application_name );

/* Returns inserted transaction_date_time */
/* -------------------------------------- */
char *ledger_transaction_insert(	char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					double transaction_amount,
					char *memo,
					int check_number,
					boolean lock_transaction );

FILE *ledger_transaction_insert_open_stream(
				char *application_name );

void ledger_transaction_insert_stream(
				FILE *output_pipe,
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				double transaction_amount,
				char *memo,
				int check_number,
				boolean lock_transaction );

void ledger_transaction_memo_update(	char *application_name,
					TRANSACTION *transction );

TRANSACTION *transaction_binary(
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				char *debit_account,
				char *credit_account,
				double transaction_amount,
				char *memo );

LIST *transaction_binary_journal_list(
				char *full_name,
				char *street_address,
				char *transaction_date_time,
				double transaction_amount,
				char *debit_account,
				char *credit_account );

TRANSACTION *ledger_check_number_seek_transaction(
				LIST *transaction_list,
				int check_number );

#endif
