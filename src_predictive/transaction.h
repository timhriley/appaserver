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
			void );

/* Returns inserted transaction_date_time */
/* -------------------------------------- */
char *transaction_insert(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double transaction_amount,
			char *memo,
			int check_number,
			boolean lock_transaction );

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

/* TRANSACTION with program_name addition */
/* -------------------------------------- */
FILE *transaction_program_insert_open(
			void );

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

/* Returns transaction_list */
/* ------------------------ */
LIST *transaction_list_program_insert(
			LIST *transaction_list );

/* TRANSACTION with property_street_address addition */
/* ------------------------------------------------- */
FILE *transaction_property_insert_open(
			void );

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

/* All projects execute this. */
/* -------------------------- */
void transaction_journal_list_insert(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			LIST *journal_list );

char *transaction_select(
			void );

/* Safely returns heap memory */
/* -------------------------- */
char *transaction_primary_where(
			char *full_name,
			char *street_address,
			char *transaction_date_time );

TRANSACTION *transaction_parse(
			char *input_buffer );

TRANSACTION *transaction_seek(
			LIST *transaction_list,
			char *full_name,
			char *street_address,
			char *transaction_date_time );

/* Performs journal_propagate() */
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

char *transaction_display(
			TRANSACTION *transaction );

void transaction_update(
			double transaction_amount,
			char *full_name,
			char *street_address,
			char *transaction_date_time );

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

TRANSACTION *transaction_check_seek(
			LIST *transaction_list,
			int check_number );

char *transaction_memo(	char *memo );

void transaction_check_insert(
			FILE *insert_pipe,
			int check_number );

void transaction_lock_insert(
			FILE *insert_pipe,
			boolean lock_transaction );

void transaction_refresh(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double transaction_amount,
			char *memo,
			int check_number,
			boolean lock_transaction,
			LIST *journal_list );

#endif
