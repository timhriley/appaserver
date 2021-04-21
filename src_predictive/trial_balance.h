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

enum fund_aggregation		{	single_fund,
					sequential,
					consolidated );

enum output_medium 		{	output_table,
					output_PDF,
					output_stdout };

/* Structures */
/* ---------- */
typedef struct
{
	/* Attributes */
	/* ---------- */
	char *application_name;
	char *session;
	char *login_name;
	char *role_name;
	char *as_of_date;
	int years_ago;
	LIST *fund_name_list;
	enum subclassification_option subclassification_option;
	enum fund_aggregation fund_aggregation;

	/* Process */
	/* ------- */
	LIST *prior_year_element_list;

} TRIAL_BALANCE_PRIOR_YEAR;

typedef struct
{
	/* Attributes */
	/* ---------- */
	char *application_name;
	char *session;
	char *login_name;
	char *role_name;
	char *as_of_date;
	int prior_year_count;
	char *fund_name;
	char *subclassification_option_string;
	char *fund_aggregation;
	char *output_medium_string;

	/* Process */
	/* ------- */
	enum subclassification_option trial_balance_subclassification_option;
	enum fund_aggregation trial_balance_fund_aggregation;
	enum output_medium trial_balance_output_medium;
	char *beginning_date;
	LIST *preclose_element_list;
	LIST *current_element_list;
	LIST *prior_year_list;

} TRIAL_BALANCE;

/* Operations */
/* ---------- */
TRIAL_BALANCE *trial_balance_new(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *as_of_date,
			int prior_year_count,
			LISTA *fund_name_list,
			char *subclassification_option_string,
			char *fund_aggregation_string,
			char *output_medium_string );

LIST *trial_balance_element_list(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *transaction_date_time_closing,
			char *fund_name,
			enum subclassification_option,
			enum fund_aggregation );

enum subclassification_option trial_balance_subclassification_option(
			char *subclassification_option_string );

enum fund_aggregation trial_balance_fund_aggregation(
			char *fund_aggregation_string );

enum output_medium trial_balance_output_medium(
			char *output_medium_string );

TRIAL_BALANCE *trial_balance_steady_state(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *as_of_date,
			int prior_year_count,
			LIST *fund_name_list,
			char *subclassification_option_string,
			char *fund_aggregation_string,
			char *output_medium_string,
			TRIAL_BALANCE *trial_balance );

TRIAL_BALANCE_PRIOR_YEAR *trial_balance_prior_year_new(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *as_of_date,
			int years_ago,
			char *fund_name,
			enum subclassification_option,
			enum fund_aggregation );

TRIAL_BALANCE_PRIOR_YEAR *trial_balance_prior_year_fetch(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *as_of_date,
			int years_ago,
			char *fund_name,
			enum subclassification_option,
			enum fund_aggregation );

#endif

