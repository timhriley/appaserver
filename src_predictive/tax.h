/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/tax.h				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef TAX_H
#define TAX_H

#include "list.h"
#include "boolean.h"
#include "html_table.h"
#include "date.h"
#include "latex.h"
#include "hash_table.h"
#include "dictionary.h"

/* Constants */
/* --------- */

/* Enumerated types */
/* ---------------- */

/* Structures */
/* ---------- */
typedef struct
{
	char *tax_form_line;
	char *tax_form_description;
	LIST *tax_form_line_account_list;
	LIST *rental_property_list;
	double tax_form_line_rental_property_total;
} TAX_FORM_LINE_RENTAL;

typedef struct
{
	double fixed_asset_recovery_amount;
	double prior_fixed_asset_recovery_amount;
	double property_recovery_amount;
	double prior_property_recovery_amount;
	double total_recovery_amount;
} TAX_RECOVERY;

typedef struct
{
	char *property_street_address;
	double tax_form_line_total;
} TAX_OUTPUT_RENTAL_PROPERTY;

typedef struct
{
	char *tax_form_line_string;
	char *tax_form_description;
	LIST *tax_form_line_account_list;
	LIST *rental_property_list;
	double tax_form_line_rental_property_total;
} TAX_FORM_LINE_RENTAL;
	
typedef struct
{
	char *property_street_address;
	double recovery_amount;
	LIST *tax_form_line_rental_list;
	double revenue_total;
	double mortgage_interest_total;
	double depreciation_total;
	double expense_total;
	double positive_net_income;
	double loss_total;
	double tax_rental_property_total;
} TAX_RENTAL_PROPERTY;

typedef struct
{
	/* Attributes */
	/* ---------- */
	char *tax_form_string;
	char *tax_form_line;
	char *account_name;

	/* Process */
	/* ------- */
	ACCOUNT *account;
	boolean current_liability;

	LIST *journal_list;
	double tax_form_line_account_total;
} TAX_FORM_LINE_ACCOUNT;

typedef struct
{
	/* Attributes */
	/* ---------- */
	char *tax_form_string;
	char *tax_form_line_string;
	char *tax_form_description;
	boolean itemize_accounts;

	/* Input */
	/* ----- */
	int tax_year;

	/* Operations */
	/* ---------- */
	LIST *tax_form_line_account_list;
	double tax_form_line_total;
} TAX_FORM_LINE;

typedef struct
{
	/* Attributes */
	/* ---------- */
	char *tax_form_string;

	/* Input */
	/* ----- */
	int tax_year;

	/* Operations */
	/* ---------- */
	LIST *tax_form_line_list;
} TAX_FORM;

typedef struct
{
	/* Input */
	/* ----- */
	char *tax_form_string;
	int tax_year;
	TAX_FORM *tax_form;
	TAX_RECOVERY *tax_recovery;
	LIST *tax_rental_property_list;
} TAX;

/* Operations */
/* ---------- */

/* TAX */
/* --- */
TAX *tax_calloc(	void );

/* TAX_RECOVERY */
/* ------------ */
TAX_RECOVERY *tax_recovery_new(
			char *tax_form_string,
			int tax_year );

/* TAX_FORM */
/* -------- */
TAX_FORM *tax_form_new(
			char *tax_form_string );

TAX_FORM *tax_form_fetch(
			char *tax_form_string,
			int tax_year,
			boolean fetch_line_list,
			boolean fetch_account_list,
			boolean fetch_journal_list );

/* Returns static memory */
/* --------------------- */
char *tax_form_primary_where(
			char *tax_form_string );

/* TAX_FORM_LINE */
/* ------------- */
TAX_FORM_LINE *tax_form_line_calloc(
			void );

TAX_FORM_LINE *tax_form_line_new(
			char *tax_form_string,
			char *tax_form_line_sting );

LIST *tax_form_line_list(
			char *tax_form_string,
			int tax_year,
			boolean fetch_account_list,
			boolean fetch_journal_list );

char *tax_form_line_system_string(
			char *where );

TAX_FORM_LINE *tax_form_line_parse(
			char *input,
			int tax_year,
			boolean fetch_account_list,
			boolean fetch_journal_list );

/* Returns static memory */
/* --------------------- */
char *tax_form_line_primary_where(
			char *tax_form_string,
			char *tax_form_line_string );

double tax_form_line_total(
			LIST *tax_form_line_account_list );

/* TAX_FORM_LINE_ACCOUNT */
/* --------------------- */
TAX_FORM_LINE_ACCOUNT *tax_form_line_account_new(
			char *tax_form_string,
			char *tax_form_line,
			char *account_name );

char *tax_form_line_account_system_string(
			char *where );

LIST *tax_form_line_account_list(
			char *tax_form_string,
			char *tax_form_line_string,
			int tax_year,
			boolean fetch_journa_list );

double tax_form_line_account_total(
			LIST *journal_list,
			boolean accumulate_debit,
			double current_liability );

#ifdef NOT_DEFINED
/* TAX_RENTAL_PROPERTY */
/* ------------------- */
LIST *tax_rental_property_list_fetch(
			int tax_year );

/* TAX_FORM_LINE_RENTAL */
/* -------------------- */
TAX_RENTAL_PROPERTY *tax_input_rental_property_new(
			char *property_street_address );

LIST *tax_form_line_rental_list(
			LIST *tax_form_line_list,
			LIST *rental_property_list );

LIST *tax_form_line_rental_empty_property_list(
			LIST *rental_property_list );

TAX_FORM_LINE_RENTAL *tax_form_line_rental_new(
			char *tax_form_line,
			char *tax_form_description,
			LIST *tax_form_line_account_list );

void tax_form_line_address_rental_list_set(
			LIST *rental_property_list,
			double debit_amount,
			double credit_amount,
			char *property_street_address,
			boolean accumulate_debit );

void tax_form_line_distribute_rental_list_set(
			LIST *rental_property_list,
			double debit_amount,
			double credit_amount,
			boolean accumulate_debit,
			double denominator );

void tax_line_rental_list_accumulate_line_total(
			LIST *tax_form_line_rental_list,
			LIST *tax_input_rental_property_list );

void tax_rental_list_accumulate_line_total(
			LIST *rental_property_list,
			LIST *tax_form_line_account_list );

void tax_rental_journal_list_accumulate_line_total(
			LIST *rental_property_list,
			LIST *journal_list,
			boolean accumulate_debit );

double tax_fetch_recovery_amount(
			char *folder_name,
			int tax_year );

void tax_form_line_set_depreciation(
			LIST *tax_form_line_list,
			double total_recovery_amount );

TAX_RENTAL_PROPERTY *tax_rental_property_seek(
			LIST *tax_rental_property_list,
			char *property_street_address );

LIST *tax_rental_street_address_list(
			char *application_name,
			char *begin_date_string,
			char *end_date_string );

void tax_rental_property_list_accumulate_depreciation(
				LIST *tax_output_rental_property_list,
				LIST *tax_input_rental_property_list );

double tax_form_line_account_get_total(
				LIST *journal_ledger_list,
				boolean accumulate_debit,
				int tax_year );

double tax_form_line_get_total(	LIST *tax_form_line_account_list,
				int tax_year );
#endif

#endif
