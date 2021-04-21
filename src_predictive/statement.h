/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/statement.h			*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#ifndef STATEMENT_H
#define STATEMENT_H

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
					consolidated,
					no_fund };

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
	char *fund_name;
	enum subclassification_option subclassification_option;
	enum fund_aggregation fund_aggregation;

	/* Process */
	/* ------- */
	LIST *prior_year_element_list;

} STATEMENT_PRIOR_YEAR;

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
	enum subclassification_option subclassification_option;
	enum fund_aggregation fund_aggregation;

	/* Process */
	/* ------- */
	char *begin_date_string;
	LIST *preclose_element_list;
	LIST *current_element_list;
	LIST *prior_year_list;

} STATEMENT_FUND;

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
	LIST *fund_name_list;
	char *subclassification_option_string;
	char *fund_aggregation;
	char *output_medium_string;

	/* Process */
	/* ------- */
	enum subclassification_option statement_subclassification_option;
	enum fund_aggregation statement_fund_aggregation;
	enum output_medium statement_output_medium;
	LIST *statement_fund_list;

} STATEMENT;

/* Operations */
/* ---------- */
STATEMENT *statement_new(
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

LIST *statement_element_list(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *transaction_date_time_closing,
			char *fund_name,
			enum subclassification_option,
			enum fund_aggregation );

enum subclassification_option statement_subclassification_option(
			char *subclassification_option_string );

enum fund_aggregation statement_fund_aggregation(
			LIST *fund_name_list,
			char *fund_aggregation_string );

enum output_medium statement_output_medium(
			char *output_medium_string );

STATEMENT *statement_steady_state(
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
			STATEMENT *statement );

STATEMENT_PRIOR_YEAR *statement_prior_year_new(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *as_of_date,
			int years_ago,
			char *fund_name,
			enum subclassification_option,
			enum fund_aggregation );

STATEMENT_PRIOR_YEAR *statement_prior_year_fetch(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *as_of_date,
			int years_ago,
			char *fund_name,
			enum subclassification_option,
			enum fund_aggregation );

STATEMENT_FUND *statement_fund_fetch(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *as_of_date,
			int prior_year_count,
			char *fund_name,
			LIST *fund_name_list,
			enum subclassification_option );

STATEMENT_FUND *statement_fund_new(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *as_of_date,
			int prior_year_count,
			char *fund_name,
			enum subclassification_option,
			enum fund_aggregation );

LIST *statement_fund_list(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *as_of_date,
			int prior_year_count,
			LIST *fund_name_list,
			enum subclassification_option,
			enum fund_aggregation );

char *statement_begin_date_string(
			char *fund_name,
			char *as_of_date );

#endif

