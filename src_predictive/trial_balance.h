/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/trial_balance.h		*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#ifndef TRIAL_BALANCE_H
#define TRIAL_BALANCE_H

#include "list.h"
#include "boolean.h"
#include "element.h"

/* Constants */
/* --------- */

/* Enumerated types */
/* ---------------- */
enum subclassification_option	{	subclassification_display,
					subclassification_omit };

enum output_medium 		{	output_table,
					output_PDF };

/* Structures */
/* ---------- */
typedef struct
{
	/* Attributes */
	/* ---------- */
	char *as_of_date;
	char *fund_name;
	char *subclassification_option_string;
	char *output_medium_string;

	/* Process */
	/* ------- */
	enum subclassification_option trial_balance_subclassification_option;
	enum output_medium trial_balance_output_medium;
	LIST *preclose_element_list;
	LIST *current_element_list;
	LIST *prior_year_element_list;
	LIST *prior_prior_year_element_list;
	LIST *prior_prior_prior_year_element_list;

} TRIAL_BALANCE;

/* Operations */
/* ---------- */
TRIAL_BALANCE *trial_balance_new(
			char *as_of_date,
			char *fund_name,
			char *subclassification_option_string,
			char *output_medium_string );

LIST *trial_balance_element_list(
			char *transaction_date_time_closing,
			char *fund_name,
			enum subclassification_option );

enum subclassification_option trial_balance_subclassification_option(
			char *subclassification_option_string );

enum output_medium trial_balance_output_medium(
			char *output_medium_string );

TRIAL_BALANCE *trial_balance_steady_state(
			char *as_of_date,
			char *fund_name,
			char *subclassification_option_string,
			char *output_medium_string,
			TRIAL_BALANCE *trial_balance );

#endif

