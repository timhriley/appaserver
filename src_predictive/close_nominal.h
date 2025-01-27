/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/close_nominal.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef CLOSE_NOMINAL_H
#define CLOSE_NOMINAL_H

#include "list.h"
#include "boolean.h"
#include "entity.h"
#include "entity_self.h"
#include "element.h"
#include "journal.h"
#include "statement.h"

typedef struct
{
	TRANSACTION *transaction;
} CLOSE_NOMINAL_TRANSACTION;

/* Usage */
/* ----- */
CLOSE_NOMINAL_TRANSACTION *
	close_nominal_transaction_new(
		LIST *element_statement_list /* in/out */,
		char *transaction_date_close_date_time,
		char *account_drawing,
		double close_nominal_do_drawing_sum,
		double close_nominal_do_transaction_amount,
		double close_nominal_do_retained_earnings,
		char *account_closing_entry,
		char *full_name,
		char *street_address );

/* Process */
/* ------- */
CLOSE_NOMINAL_TRANSACTION *
	close_nominal_transaction_calloc(
		void );

/* Usage */
/* ----- */
LIST *close_nominal_transaction_journal_list(
		LIST *element_statement_list /* in/out */,
		char *transaction_date_close_date_time,
		char *account_drawing,
		double close_nominal_do_drawing_sum,
		double close_nominal_do_retained_earnings,
		char *account_closing_entry,
		char *full_name,
		char *street_address );

/* Usage */
/* ----- */
double close_nominal_transaction_debit_amount(
		boolean element_accumulate_debit,
		double balance );


/* Usage */
/* ----- */
double close_nominal_transaction_credit_amount(
		boolean element_accumulate_debit,
		double balance );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
JOURNAL *close_nominal_transaction_drawing_journal(
		char *transaction_date_time_closing,
		char *account_drawing,
		double close_nominal_drawing_sum,
		char *full_name,
		char *street_address );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
JOURNAL *close_nominal_transaction_close_journal(
		char *transaction_date_time_closing,
		double close_nominal_do_retained_earnings,
		char *account_closing_entry,
		char *full_name,
		char *street_address );

typedef struct
{
	TRANSACTION_DATE_CLOSE_NOMINAL_DO *
		transaction_date_close_nominal_do;
	LIST *element_name_list;
	LIST *element_statement_list;
	ELEMENT *revenue_element;
	double revenue_sum;
	ELEMENT *gain_element;
	double gain_sum;
	char *account_drawing;
	double drawing_sum;
	double debit_sum;
	ELEMENT *expense_element;
	double expense_sum;
	ELEMENT *loss_element;
	double loss_sum;
	double credit_sum;
	double transaction_amount;
	boolean no_transactions_boolean;
	double retained_earnings;
	char *account_closing_entry;
	ENTITY_SELF *entity_self;
	CLOSE_NOMINAL_TRANSACTION *close_nominal_transaction;
	double journal_debit_sum;
	double journal_credit_sum;
} CLOSE_NOMINAL_DO;

/* Usage */
/* ----- */
CLOSE_NOMINAL_DO *close_nominal_do_fetch(
		char *as_of_date_string );

/* Process */
/* ------- */
CLOSE_NOMINAL_DO *close_nominal_do_calloc(
		void );

LIST *close_nominal_do_element_name_list(
		char *element_revenue,
		char *element_expense,
		char *element_gain,
		char *element_loss );

double close_nominal_do_drawing_sum(
		char *transaction_date_time_closing,
		char *account_drawing );

double close_nominal_do_debit_sum(
		double revenue_sum,
		double gain_sum,
		double close_nominal_do_drawing_sum );

double close_nominal_do_credit_sum(
		double expense_sum,
		double loss_sum );

double close_nominal_do_transaction_amount(
		double close_nominal_do_debit_sum,
		double close_nominal_do_credit_sum );

boolean close_nominal_do_no_transactions_boolean(
		double close_nominal_do_transaction_amount );

double close_nominal_do_retained_earnings(
		double revenue_sum,
		double gain_sum,
		double expense_sum,
		double loss_sum,
		double close_nominal_do_drawing_sum );

/* Returns static memory */
/* --------------------- */
char *close_nominal_do_transaction_exists_message(
		char *transaction_date_close_date_time );

typedef struct
{
	ENTITY_SELF *entity_self;
	TRANSACTION_DATE_CLOSE_NOMINAL_UNDO *
		transaction_date_close_nominal_undo;
	TRANSACTION *transaction;
} CLOSE_NOMINAL_UNDO;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CLOSE_NOMINAL_UNDO *close_nominal_undo_fetch(
		void );

/* Process */
/* ------- */
CLOSE_NOMINAL_UNDO *close_nominal_undo_calloc(
		void );

/* Driver */
/* ------ */
void close_nominal_undo_display(
		TRANSACTION *transaction );

/* Driver */
/* ------ */
void close_nominal_undo_execute(
		TRANSACTION *transaction );

typedef struct
{
	CLOSE_NOMINAL_DO *close_nominal_do;
	CLOSE_NOMINAL_UNDO *close_nominal_undo;
	char *undo_no_transaction_message;
	char *empty_date_message;
	char *do_transaction_exists_message;
	char *do_no_transaction_message;
	STATEMENT_CAPTION *statement_caption;
} CLOSE_NOMINAL;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CLOSE_NOMINAL *close_nominal_fetch(
		char *application_name,
		char *process_name,
		char *as_of_date_string,
		boolean undo );

/* Process */
/* ------- */
CLOSE_NOMINAL *close_nominal_calloc(
		void );

/* Returns program memory */
/* ---------------------- */
char *close_nominal_undo_no_transaction_message(
		void );

/* Returns program memory */
/* ---------------------- */
char *close_nominal_empty_date_message(
		void );

/* Returns program memory */
/* ---------------------- */
char *close_nominal_do_no_transaction_message(
		void );

/* Driver */
/* ------ */

/* Returns static memory */
/* --------------------- */
char *close_nominal_do_execute_message(
		char *transaction_date_close_date_time );

/* Returns program memory */
/* ---------------------- */
char *close_nominal_do_no_execute_message(
		void );

/* Returns program memory */
/* ---------------------- */
char *close_nominal_undo_execute_message(
		void );

/* Returns program memory */
/* ---------------------- */
char *close_nominal_undo_no_execute_message(
		void );

#endif
