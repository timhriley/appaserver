/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/reverse_nominal.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef REVERSE_NOMINAL_H
#define REVERSE_NOMINAL_H

#include "list.h"
#include "boolean.h"
#include "entity.h"
#include "entity_self.h"
#include "reverse_transaction.h"
#include "transaction_date.h"
#include "statement.h"

typedef struct
{
	TRANSACTION_DATE_REVERSE_NOMINAL_DO *
		transaction_date_reverse_nominal_do;
	char *close_transaction_date_time;
	TRANSACTION *close_transaction;
	ENTITY_SELF *entity_self;
	REVERSE_TRANSACTION *reverse_transaction;
	double journal_debit_sum;
	double journal_credit_sum;
} REVERSE_NOMINAL_DO;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
REVERSE_NOMINAL_DO *reverse_nominal_do_fetch(
		char *reverse_date_string );

/* Process */
/* ------- */
REVERSE_NOMINAL_DO *reverse_nominal_do_calloc(
		void );

/* Returns transaction_date_reverse_date_time minus one second */
/* ----------------------------------------------------------- */
char *reverse_nominal_do_close_transaction_date_time(
		char *transaction_date_reverse_date_time );

/* Returns static memory */
/* --------------------- */
char *reverse_nominal_do_transaction_exists_message(
		char *transaction_date_close_date_time );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *reverse_nominal_do_close_transaction_date_time(
		char *transaction_date_reverse_date_time );

/* Driver */
/* ------ */
void reverse_nominal_undo_execute(
		TRANSACTION *transaction );

typedef struct
{
	REVERSE_NOMINAL_DO *reverse_nominal_do;
	CLOSE_NOMINAL_UNDO *close_nominal_undo;
	char *undo_no_transaction_message;
	char *do_empty_date_message;
	char *do_transaction_exists_message;
	char *do_no_transaction_message;
	STATEMENT_CAPTION *statement_caption;
} REVERSE_NOMINAL;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
REVERSE_NOMINAL *reverse_nominal_fetch(
		char *application_name,
		char *process_name,
		char *reverse_date_string,
		boolean undo );

/* Process */
/* ------- */
REVERSE_NOMINAL *reverse_nominal_calloc(
		void );

/* Returns program memory */
/* ---------------------- */
char *reverse_nominal_undo_no_transaction_message(
		void );

/* Returns program memory */
/* ---------------------- */
char *reverse_nominal_do_empty_date_message(
		void );

/* Returns program memory */
/* ---------------------- */
char *reverse_nominal_do_no_transaction_message(
		void );

/* Driver */
/* ------ */

/* Returns static memory */
/* --------------------- */
char *reverse_nominal_do_execute_message(
		char *transaction_date_close_date_time );

/* Returns program memory */
/* ---------------------- */
char *reverse_nominal_do_no_execute_message(
		void );

/* Returns static memory */
/* --------------------- */
char *reverse_nominal_undo_execute_message(
		char *transaction_date_time );

/* Returns program memory */
/* ---------------------- */
char *reverse_nominal_undo_no_execute_message(
		void );

#endif
