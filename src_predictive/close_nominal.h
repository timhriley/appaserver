/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/close_nominal.h		*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

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
			char *transaction_date_time_closing,
			char *account_drawing,
			double close_nominal_drawing_sum,
			double close_nominal_retained_earnings,
			char *account_closing_entry,
			ENTITY *entity );

/* Process */
/* ------- */
CLOSE_NOMINAL_TRANSACTION *
	close_nominal_transaction_calloc(
			void );

/* Usage */
/* ----- */
LIST *close_nominal_transaction_journal_list(
			LIST *element_statement_list /* in/out */,
			char *transaction_date_time_closing,
			char *account_drawing,
			double close_nominal_drawing_sum,
			double close_nominal_retained_earnings,
			char *account_closing_entry,
			ENTITY *entity );

/* Process */
/* ------- */
JOURNAL *close_nominal_transaction_drawing_journal(
			char *transaction_date_time_closing,
			char *account_drawing,
			double close_nominal_drawing_sum,
			ENTITY *entity );

JOURNAL *close_nominal_transaction_close_journal(
			char *transaction_date_time_closing,
			double close_nominal_retained_earnings,
			char *account_closing_entry,
			ENTITY *entity );

/* Usage */
/* ----- */
double close_nominal_transaction_debit_amount(
			boolean element_accumulate_debit,
			double balance );


/* Process */
/* ------- */

/* Usage */
/* ----- */
double close_nominal_transaction_credit_amount(
			boolean element_accumulate_debit,
			double balance );

/* Process */
/* ------- */

typedef struct
{
	STATEMENT_CAPTION *statement_caption;
	char *transaction_date_time_closing;
	boolean transaction_date_time_exists;
	LIST *element_name_list;
	LIST *element_statement_list;
	ELEMENT *revenue_element;
	double revenue_sum;
	ELEMENT *gain_element;
	double gain_sum;
	double debit_sum;
	ELEMENT *expense_element;
	double expense_sum;
	ELEMENT *loss_element;
	double loss_sum;
	char *account_drawing;
	double drawing_sum;
	double credit_sum;
	double retained_earnings;
	char *account_closing_entry;
	ENTITY_SELF *entity_self;
	CLOSE_NOMINAL_TRANSACTION *close_nominal_transaction;
} CLOSE_NOMINAL;

/* Usage */
/* ----- */
CLOSE_NOMINAL *close_nominal_fetch(
			char *application_name,
			char *process_name,
			char *as_of_date );

/* Process */
/* ------- */
CLOSE_NOMINAL *close_nominal_calloc(
			void );

LIST *close_nominal_element_name_list(
			char *element_revenue,
			char *element_expense,
			char *element_gain,
			char *element_loss );

double close_nominal_drawing_sum(
			char *transaction_date_time_closing,
			char *account_drawing );

double close_nominal_debit_sum(
			double revenue_sum,
			double gain_sum );

double close_nominal_credit_sum(
			double expense_sum,
			double loss_sum,
			double close_nominal_drawing_sum );

double close_nominal_retained_earnings(
			double close_nominal_debit_sum,
			double close_nominal_credit_sum );

/* Driver */
/* ------ */
void close_nominal_transaction_insert(
			TRANSACTION *transaction );

#endif
