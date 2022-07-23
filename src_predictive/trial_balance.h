/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/trial_balance.h			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef TRIAL_BALANCE_H
#define TRIAL_BALANCE_H

#include "list.h"
#include "date.h"
#include "boolean.h"
#include "statement.h"

#define TRIAL_BALANCE_DAYS_FOR_EMPHASIS		35

typedef struct
{
	enum statement_subclassification_option
		statement_subclassification_option;
	enum statement_output_medium
		statement_output_medium;
	char *transaction_as_of_date;
	char *transaction_begin_date_string;
	char *statement_logo_filename;
	char *statement_title;
	char *statement_subtitle;
	LIST *filter_element_name_list;
	boolean transaction_closing_entry_exists;
	char *transaction_date_time_closing;
	STATEMENT *preclose_statement;
	LIST *preclose_prior_year_list;
	STATEMENT *postclose_statement;
	LIST *postclose_prior_year_list;
	double preclose_debit_sum;
	double preclose_credit_sum;
	double postclose_debit_sum;
	double postclose_credit_sum;
} TRIAL_BALANCE;

/* Usage */
/* ----- */
TRIAL_BALANCE *trial_balance_fetch(
			char *application_name,
			char *session_key,
			char *role_name,
			char *process_name,
			char *as_of_date,
			int prior_year_count,
			char *subclassifiction_option_string,
			char *output_medium_string );

/* Process */
/* ------- */
TRIAL_BALANCE *trial_balance_calloc(
			void );

LIST *trial_balance_filter_element_name_list(
			void );

#endif
