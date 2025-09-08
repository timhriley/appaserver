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
#include "close_transaction.h"
#include "transaction_date.h"
#include "statement.h"

typedef struct
{
	TRANSACTION_DATE_CLOSE_NOMINAL_DO *
		transaction_date_close_nominal_do;
	LIST *element_name_list;
	LIST *element_statement_list;
	char *equity_subclassification_where;
	LIST *equity_subclassification_statement_list;
	ENTITY_SELF *entity_self;
	CLOSE_TRANSACTION *close_transaction;
	double journal_debit_sum;
	double journal_credit_sum;
} CLOSE_NOMINAL_DO;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CLOSE_NOMINAL_DO *close_nominal_do_fetch(
		char *as_of_date_string );

/* Process */
/* ------- */
CLOSE_NOMINAL_DO *close_nominal_do_calloc(
		void );

LIST *close_nominal_do_element_name_list(
		const char *element_revenue,
		const char *element_expense,
		const char *element_expenditure,
		const char *element_gain,
		const char *element_loss );

/* Returns static memory */
/* --------------------- */
char *close_nominal_do_equity_subclassification_where(
		const char *subclassification_drawing,
		const char *subclassification_appropriation,
		const char *subclassification_encumbrance,
		const char *subclassification_estimated_interfund,
		const char *subclassification_estimated_revenue,
		const char *subclassification_interfund );

/* Returns static memory */
/* --------------------- */
char *close_nominal_do_transaction_exists_message(
		char *transaction_date_close_date_time );

typedef struct
{
	TRANSACTION_DATE_CLOSE_NOMINAL_UNDO *
		transaction_date_close_nominal_undo;
	TRANSACTION *transaction;
} CLOSE_NOMINAL_UNDO;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CLOSE_NOMINAL_UNDO *close_nominal_undo_fetch(
		const char *transaction_table,
		const char *transaction_close_memo );

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

/* Returns static memory */
/* --------------------- */
char *close_nominal_undo_execute_message(
		char *transaction_date_time );

/* Returns program memory */
/* ---------------------- */
char *close_nominal_undo_no_execute_message(
		void );

#endif
