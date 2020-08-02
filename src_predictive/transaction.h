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

/* Constants */
/* --------- */
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
	LIST *journal_ledger_list;
	boolean lock_transaction;
	char *property_street_address;
	char *program_name;
} TRANSACTION;

/* Operations */
/* ---------- */
TRANSACTION *transaction_new(	char *full_name,
				char *street_address,
				char *transaction_date_time );

TRANSACTION *transaction_fetch(	char *full_name,
				char *street_address,
				char *transaction_date_time );

/* Returns inserted transaction_date_time */
/* -------------------------------------- */
char *transaction_insert(	char *full_name,
				char *street_address,
				char *transaction_date_time,
				double transaction_amount,
				char *memo,
				int check_number,
				boolean lock_transaction );

char *transaction_program_select(
				void );

char *transaction_property_select(
				void );

char *transaction_select(	void );

/* Returns static memory */
/* --------------------- */
char *transaction_escape_full_name(
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

TRANSACTION *transaction_parse(	char *input_buffer );

FILE *transaction_insert_pipe(	void );

char *transaction_unique_transaction_date_time(
				char *transaction_date_time );

boolean transaction_exists(	char *transaction_date_time );

#endif
