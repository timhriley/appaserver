/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/accrual.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef ACCRUAL_H
#define ACCRUAL_H

#include "list.h"
#include "boolean.h"
#include "transaction.h"

#define ACCRUAL_TABLE			"accrual"

#define ACCRUAL_PROPERTY_ATTRIBUTE	"rental_property_street_address"

#define ACCRUAL_SELECT_ATTRIBUTES	"full_name,"			\
					"street_address,"		\
					"accrual_description,"	\
					"debit_account,"		\
					"credit_account,"		\
					"accrued_daily_amount,"		\
					"accrued_monthly_amount"

typedef struct
{
	char *full_name;
	char *street_address;
	char *accrual_description;
	char *debit_account;
	char *credit_account;
	double accrued_daily_amount;
	double accrued_monthly_amount;
	boolean property_attribute_exists;
	char *rental_property_street_address;
	char *transaction_increment_date_time;
	TRANSACTION *transaction;
} ACCRUAL;

/* Usage */
/* ----- */
LIST *accrual_list_fetch(
		void );

/* Usage */
/* ----- */
LIST *accrual_system_list(
		char *accrual_system_string,
		boolean accrual_property_attribute_exists );

/* Process */
/* ------- */
FILE *accrual_input_pipe(
		char *accrual_system_string );

/* Usage */
/* ----- */
ACCRUAL *accrual_fetch(
		char *full_name,
		char *street_address,
		char *accrual_description );

/* Usage */
/* ----- */
ACCRUAL *accrual_parse(
		boolean accrual_property_attribute_exists,
		char *input );

/* Usage */
/* ----- */
ACCRUAL *accrual_new(
		char *full_name,
		char *street_address,
		char *accrual_description );


/* Process */
/* ------- */
ACCRUAL *accrual_calloc(
		void );

/* Usage */
/* ----- */
void accrual_list_transaction_set(
		LIST *accrual_list /* in/out */ );

/* Usage */
/* ----- */
TRANSACTION *accrual_transaction(
		char *full_name,
		char *street_address,
		char *accrual_description,
		char *transaction_increment_date_time,
		char *debit_account,
		char *credit_account,
		double accrued_daily_amount,
		double accrued_monthly_amount,
		char *rental_property_street_address );

/* Usage */
/* ----- */
TRANSACTION *accrual_daily_transaction(
		char *full_name,
		char *street_address,
		char *accrual_description,
		char *transaction_increment_date_time,
		char *debit_account,
		char *credit_account,
		double accrued_daily_amount );

/* Process */
/* ------- */
double accrual_daily_accrued_amount(
		double accrued_daily_amount,
		int accrual_last_transaction_days_between );

/* Usage */
/* ----- */
TRANSACTION *accrual_monthly_transaction(
		char *full_name,
		char *street_address,
		char *accrual_description,
		char *transaction_increment_date_time,
		char *debit_account,
		char *credit_account,
		double accrued_monthly_amount,
		char *rental_property_street_address );

/* Usage */
/* ----- */
int accrual_last_transaction_days_between(
		char *transaction_table,
		char *full_name,
		char *street_address,
		char *transaction_increment_date_time,
		char *debit_account,
		char *credit_account );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *accrual_max_transaction_date_time(
		char *transaction_table,
		char *full_name,
		char *street_address,
		char *debit_account,
		char *credit_account );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *accrual_journal_transaction_subquery(
		char *journal_table,
		char *transaction_table,
		char *debit_account,
		char *credit_account );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *accrual_primary_where(
		char *full_name,
		char *street_address,
		char *accrual_description );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *accrual_escape_description(
		char *accrual_description );

/* Usage */
/* ----- */
double accrual_monthly_accrue(
		char *begin_date_string,
		char *end_date_string,
		double accrued_monthly_amount );

/* Process */
/* ------- */
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

/* Usage */
/* ----- */
char *accrual_opening_begin_date_string(
		DATE *end_date );

/* Public */
/* ------ */
boolean accrual_property_attribute_exists(
		char *accrual_table,
		char *accrual_property_attribute );

/* Returns heap memory */
/* ------------------- */
char *accrual_select(
		char *accrual_select_attributes,
		char *accrual_property_attribute,
		boolean accrual_property_attribute_exists );

/* Returns heap memory */
/* ------------------- */
char *accrual_system_string(
		char *accrual_select,
		char *accrual_table,
		char *where );

/* Returns heap memory */
/* ------------------- */
char *accrual_memo(
		char *accrual_description,
		char *credit_account );

LIST *accrual_transaction_list_extract(
		LIST *accrual_list );

double accrual_month_percent(
		int begin_date_day,
		int end_date_day,
		int days_in_month );

#endif

