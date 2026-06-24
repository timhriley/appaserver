/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/insert_expense.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef INSERT_EXPENSE_H
#define INSERT_EXPENSE_H

#include "list.h"
#include "boolean.h"
#include "transaction.h"

#define INSERT_EXPENSE_JOURNAL_DUPLICATE_MESSAGE \
"Duplication Error. Please execute this path: Insert --> Transaction -->Transaction"

typedef struct
{
	char *transaction_date;
	boolean predictive_fund_boolean;
	boolean fund_empty_boolean;
	boolean feeder_empty_boolean;
	boolean new_name_boolean;
	boolean name_empty_boolean;
	char *full_name;
	boolean debit_empty_boolean;
	boolean amount_empty_boolean;
	boolean check_duplicate_boolean;
	boolean journal_duplicate_boolean;
	char *credit_account;
	char *transaction_date_time;
	boolean entity_contact_key_boolean;
	char *entity_contact_key;
} INSERT_EXPENSE_INPUT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
INSERT_EXPENSE_INPUT *
	insert_expense_input_new(
		char *fund_name,
		char *feeder_account,
		char *full_name,
		char *contact_key,
		char *new_full_name,
		char *transaction_date,
		char *debit_account,
		double transaction_amount,
		int check_number );

INSERT_EXPENSE_INPUT *
	insert_expense_input_calloc(
		void );

boolean insert_expense_input_feeder_empty_boolean(
		char *feeder_account );

boolean insert_expense_input_new_name_boolean(
		char *new_full_name );

boolean insert_expense_input_name_empty_boolean(
		char *full_name,
		boolean insert_expense_input_new_name_boolean );

/* Returns either parameter */
/* ------------------------ */
char *insert_expense_input_full_name(
		char *full_name,
		char *new_full_name,
		boolean
		insert_expense_input_new_name_boolean );

boolean insert_expense_input_debit_empty_boolean(
		char *debit_account );

boolean insert_expense_input_amount_empty_boolean(
		double transaction_amount );

/* Usage */
/* ----- */

/* Returns parameter or heap memory */
/* -------------------------------- */
char *insert_expense_input_transaction_date(
		char *transaction_date );

/* Usage */
/* ----- */
boolean insert_expense_input_fund_empty_boolean(
		char *fund_name,
		boolean predictive_fund_boolean );

/* Usage */
/* ----- */
boolean insert_expense_input_check_duplicate_boolean(
		const char *transaction_table,
		double transaction_amount,
		int check_number );

/* Usage */
/* ----- */
boolean insert_expense_input_journal_duplicate_boolean(
		const char *journal_table,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *transaction_date,
		char *debit_account,
		double transaction_amount,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *insert_expense_input_journal_where(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *transaction_date,
		char *debit_account,
		double transaction_amount,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */

/* Returns parameter or heap memory */
/* -------------------------------- */
char *insert_expense_input_credit_account(
		const char *account_uncleared_checks_key,
		char *feeder_account,
		int check_number );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *insert_expense_input_transaction_date_time(
		char *transaction_date );

typedef struct
{
	INSERT_EXPENSE_INPUT *
		insert_expense_input;

	char *error_message;
	TRANSACTION *transaction_binary;
} INSERT_EXPENSE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
INSERT_EXPENSE *
	insert_expense_new(
		char *fund_name,
		char *feeder_account,
		char *full_name,
		char *contact_key,
		char *new_full_name,
		char *transaction_date,
		char *debit_account,
		double transaction_amount,
		int check_number,
		char *memo );

/* Process */
/* ------- */
INSERT_EXPENSE *insert_expense_calloc(
		void );

/* Returns program memory, heap memory, or null */
/* -------------------------------------------- */
char *insert_expense_error_message(
		int check_number,
		boolean fund_empty_boolean,
		boolean feeder_empty_boolean,
		boolean name_empty_boolean,
		boolean debit_empty_boolean,
		boolean amount_empty_boolean,
		boolean check_duplicate_boolean,
		boolean journal_duplicate_boolean );

#endif
