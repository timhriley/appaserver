/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/tax.h				*/
/* -------------------------------------------------------------------- */
/* This is the appaserver tax ADT.					*/
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
#define TAX_DEPRECIATION_DESCRIPTION	"depreciation"
#define TAX_FORM_LINE_KEY		".tax_form_line "
#define TAX_FORM_LINE_ACCOUNT_KEY	".account "

/* Enumerated types */
/* ---------------- */
enum tax_form_line_category {
	revenue,
	mortgage_interest_paid,
	depreciation_expense,
	operating_expense,
	loss };

/* Structures */
/* ---------- */
typedef struct
{
	char *property_street_address;
	double recovery_amount;
} TAX_INPUT_RENTAL_PROPERTY;

typedef struct
{
	double fixed_asset_recovery_amount;
	double prior_fixed_asset_recovery_amount;
	double property_recovery_amount;
	double prior_property_recovery_amount;
	double total_recovery_amount;
} TAX_INPUT_RECOVERY;

typedef struct
{
	char *property_street_address;
	double tax_form_line_total;
} TAX_OUTPUT_RENTAL_PROPERTY;

typedef struct
{
	char *tax_form_line;
	char *tax_form_description;
	enum tax_form_line_category tax_form_line_category;
	LIST *tax_form_line_account_list;
	LIST *rental_property_list;
} TAX_FORM_LINE_RENTAL;
	
typedef struct
{
	LIST *tax_form_line_rental_list;
	double revenue_total;
	double mortgage_interest_total;
	double depreciation_total;
	double expense_total;
	double positive_net_income;
	double loss_total;
} TAX_OUTPUT_RENTAL;

typedef struct
{
	char *account_name;
	char *hard_coded_account_key;
	char *subclassification;
	char *element;
	boolean accumulate_debit;
	LIST *journal_ledger_list;
	double tax_form_account_total;
} TAX_FORM_LINE_ACCOUNT;

typedef struct
{
	char *tax_form_line;
	char *tax_form_description;
	boolean itemize_accounts;
	double tax_form_line_total;
	LIST *tax_form_line_account_list;
} TAX_FORM_LINE;

typedef struct
{
	char *tax_form;
	LIST *tax_form_line_list;
} TAX_FORM;

typedef struct
{
	char *tax_form;
	LIST *tax_form_line_list;
	LIST *unaccounted_journal_ledger_list;
} TAX_PROCESS;

typedef struct
{
	TAX_FORM *tax_form;
	int tax_year;
	char *begin_date_string;
	char *end_date_string;
	TAX_INPUT_RECOVERY *tax_input_recovery;
	LIST *rental_property_list;
} TAX_INPUT;

typedef struct
{
	TAX_INPUT tax_input;
	TAX_PROCESS tax_process;
	TAX_OUTPUT_RENTAL tax_output_rental;
} TAX;

/* Operations */
/* ---------- */
TAX_INPUT_RECOVERY *tax_input_recovery_new(
					char *application_name,
					int tax_year );

TAX *tax_new(				char *application_name,
					char *tax_form,
					int tax_year );

TAX_FORM *tax_form_new(			char *application_name,
					char *tax_form,
					int tax_year );

TAX_FORM_LINE *tax_form_line_new(	char *tax_form_line,
					char *tax_form_description,
					boolean itemize_accounts );

TAX_FORM_LINE_ACCOUNT *tax_form_line_account_new(
					char *account_name );
	
LIST *tax_form_fetch_line_list(		char *application_name,
					char *tax_form,
					int tax_year );

LIST *tax_fetch_account_list(		char *application_name,
					char *tax_form_line );

LIST *tax_fetch_account_transaction_list(
					char *application_name,
					char *begin_date_string,
					char *end_date_string,
					char *account_name );

/* ------------------------------------ */
/* Returns process.tax_form_line_list. 	*/
/* tax_form_account_total is set.	*/
/* tax_form_line_total is set.		*/
/* ------------------------------------ */
LIST *tax_process_set_totals(		LIST *input_tax_form_line_list,
					int tax_year );

TAX_FORM_LINE_ACCOUNT *tax_form_line_account_seek(
					LIST *tax_form_line_list,
					char *account_name );

LIST *tax_fetch_rental_property_list(	char *application_name,
					char *end_date_string,
					int tax_year );

LIST *tax_get_tax_form_line_rental_list(
					LIST *tax_form_line_list,
					LIST *rental_property_list );

LIST *tax_form_line_rental_get_empty_property_list(
					LIST *rental_property_list );

/*
void tax_form_line_account_rental_property_list_set(
					LIST *rental_property_list,
					LIST *journal_ledger_list,
					boolean accumulate_debit,
					LIST *rental_property_list );
*/

TAX_FORM_LINE_RENTAL *tax_form_line_rental_new(
					char *tax_form_line,
					char *tax_form_description,
					LIST *tax_form_line_account_list );

TAX_INPUT_RENTAL_PROPERTY *tax_input_rental_property_new(
					char *input_buffer );

TAX_OUTPUT_RENTAL_PROPERTY *tax_output_rental_property_new(
					char *property_street_address );

void tax_form_line_address_rental_property_list_set(
				LIST *rental_property_list,
				double debit_amount,
				double credit_amount,
				char *property_street_address,
				boolean accumulate_debit );

void tax_form_line_distribute_rental_property_list_set(
				LIST *rental_property_list,
				double debit_amount,
				double credit_amount,
				boolean accumulate_debit,
				double denominator );

void tax_line_rental_list_accumulate_line_total(
				LIST *tax_form_line_rental_list,
				LIST *tax_input_rental_property_list );

void tax_rental_property_list_accumulate_line_total(
				LIST *rental_property_list,
				LIST *tax_form_line_account_list );

void tax_rental_journal_ledger_list_accumulate_line_total(
				LIST *rental_property_list,
				LIST *journal_ledger_list,
				boolean accumulate_debit );

double tax_fetch_recovery_amount(
				char *application_name,
				char *folder_name,
				int tax_year );

void tax_form_line_set_depreciation(
				LIST *tax_form_line_list,
				double total_recovery_amount );

TAX_INPUT_RENTAL_PROPERTY *tax_rental_property_seek(
				LIST *tax_input_rental_property_list,
				char *property_street_address );

LIST *tax_fetch_property_street_address_list(
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
