/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/accrual.h				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef ACCRUAL_H
#define ACCRUAL_H

#include "transaction.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define ACCRUAL_MEMO		"Accrual"

/* Structures */
/* ---------- */
typedef struct
{
	char *full_name;
	char *street_address;
	char *purchase_date_time;
	char *asset_name;
	char *accrual_date;
	double accrual_amount;
	double database_accrual_amount;
	char *transaction_date_time;
	char *database_transaction_date_time;
	TRANSACTION *transaction;
} ACCRUAL;

/* Operations */
/* ---------- */
ACCRUAL *accrual_calloc( void );

ACCRUAL *accrual_fetch(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *accrual_date );

void accrual_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *accrual_date,
			double accrual_amount,
			double database_accrual_amount,
			char *transaction_date_time,
			char *database_transaction_date_time );

ACCRUAL *accrual_parse(
			char *application_name,
			char *input_buffer );

char *accrual_get_select(
			void );

char *accrual_get_update_sys_string(
			char *application_name );

double accrual_get_amount(
			double extension,
			double accrual_period_years,
			char *prior_accrual_date_string,
			char *accrual_date_string,
			double accumulated_accrual );

void accrual_journal_ledger_refresh(
			char *application_name,
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double accrual_amount,
			char *asset_account_name,
			char *expense_account_name );

double accrual_get_fraction_of_year(
			char *prior_accrual_date_string,
			char *accrual_date_string );

LIST *accrual_fetch_list(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name );

/* Returns new accumulated_accrual */
/* ------------------------------------ */
double accrual_list_set(
			LIST *accrual_list,
			/* ----------------------------------- */
			/* Arrived date is the effective date. */
			/* ----------------------------------- */
			char *arrived_date_string,
			double extension,
			double accrual_period_years );

void accrual_list_update_and_transaction_propagate(
			LIST *accrual_list,
			char *application_name,
			char *asset_account_name,
			char *expense_account_name );

void accrual_list_delete(
			LIST *accrual_list,
			char *application_name,
			char *asset_account_name,
			char *expense_account_name );

void accrual_delete(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *asset_name,
			char *accrual_date );

ACCRUAL *accrual_list_seek(
			LIST *accrual_list,
			char *accrual_date );

char *accrual_get_prior_accrual_date(
			LIST *accrual_list );

double accrual_monthly_accrue(
			char *begin_date_string,
			char *end_date_string,
			double monthly_accrual );

char *accrual_get_opening_begin_date_string(
			DATE *end_date );

double accrual_get_month_percent(
			int begin_date_day,
			int end_date_day,
			int days_in_month );

/* begin_date and end_date are both in the same month. */
/* --------------------------------------------------- */
double accrual_monthly_within_month_accrue(
			DATE *begin_date,
			DATE *end_date,
			double monthly_accrual );

/* begin_date and end_date are one month apart. */
/* -------------------------------------------- */
double accrual_monthly_next_month_accrue(
			DATE *begin_date,
			DATE *end_date,
			double monthly_accrual );

/* begin_date and end_date are more than one month apart. */
/* ------------------------------------------------------ */
double accrual_monthly_multi_month_accrue(
				DATE *begin_date,
				DATE *end_date,
				double monthly_accrual,
				int months_between );

#endif

