/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/reoccurring.h			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef REOCCURRING_H
#define REOCCURRING_H

#include "list.h"
#include "boolean.h"
#include "transaction.h"

#define REOCCURRING_TABLE		"reoccurring_transaction"

#define REOCCURRING_PORPERTY_ATTRIBUTE	"rental_property_street_address"

#define REOCCURRING_SELECT_ATTRIBUTES	"full_name,"			\
					"street_address,"		\
					"transaction_description,"	\
					"debit_account,"		\
					"credit_account,"		\
					"accrued_daily_amount,"		\
					"accrued_monthly_amount"

typedef struct
{
	char *full_name;
	char *street_address;
	char *transaction_description;
	char *debit_account;
	char *credit_account;
	double accrued_daily_amount;
	double accrued_monthly_amount;
	boolean property_attribute_exists;
	char *rental_property_street_address;
	char *transaction_increment_date_time;
	TRANSACTION *transaction;
} REOCCURRING;

/* Usage */
/* ----- */
LIST *reoccurring_list(	char *application_name );

/* Process */
/* ------- */

/* Usage */
/* ----- */
LIST *reoccurring_system_list(
			char *reoccurring_system_string,
			boolean reoccurring_property_attribute_exists );

/* Process */
/* ------- */
FILE *reoccurring_input_pipe(
			char *reoccurring_system_string );

/* Usage */
/* ----- */
REOCCURRING *reoccurring_new(
			char *full_name,
			char *street_address,
			char *transaction_description );


/* Process */
/* ------- */
REOCCURRING *reoccurring_calloc(
			void );

char *reoccurring_transaction_select(
			void );

/* Usage */
/* ----- */
REOCCURRING *reoccurring_parse(
			boolean reoccurring_property_attribute_exists,
			char *input );

/* Process */
/* ------- */

/* Usage */
/* ----- */
REOCCURRING *reoccurring_new(
			char *full_name,
			char *street_address,
			char *transaction_description );

/* Process */
/* ------- */
REOCCURRING *reoccurring_calloc(
			void );

/* Usage */
/* ----- */
REOCCURRING *reoccurring_fetch(
			char *full_name,
			char *street_address,
			char *transaction_description,
			boolean reoccurring_property_attribute_exists );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *reoccurring_primary_where(
			char *full_name,
			char *street_address,
			char *transaction_description );

/* Usage */
/* ----- */
LIST *reoccurring_list_transaction_set(
			LIST *reoccurring_list /* in/out */ );

/* Process */
/* ------- */

/* Usage */
/* ----- */
TRANSACTION *reoccurring_daily_transaction(
			char *full_name,
			char *street_address,
			char *transaction_description,
			char *transaction_increment_date_time,
			char *debit_account,
			char *credit_account,
			double accrued_daily_amount );

/* Process */
/* ------- */
double reoccurring_daily_accrued_amount(
			double accrued_daily_amount,
			int reoccurring_last_transaction_days_between );

/* Usage */
/* ----- */
TRANSACTION *reoccurring_monthly_transaction(
			char *full_name,
			char *street_address,
			char *transaction_description,
			char *transaction_increment_date_time,
			char *debit_account,
			char *credit_account,
			double accrued_monthly_amount,
			char *rental_property_street_address );

/* Process */
/* ------- */
/* Usage */
/* ----- */
int reoccurring_last_transaction_days_between(
			char *transaction_table,
			char *full_name,
			char *street_address,
			char *transaction_increment_date_time,
			char *debit_account,
			char *credit_account );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *reoccurring_journal_transaction_subquery(
			char *journal_table,
			char *transaction_table,
			char *debit_account,
			char *credit_account );

/* Public */
/* ------ */
boolean reoccurring_property_attribute_exists(
			char *application_name,
			char *reoccurring_table,
			char *reoccurring_property_attribute );

/* Returns heap memory */
/* ------------------- */
char *reoccurring_select(
			char *reoccurring_select_attributes,
			char *reoccurring_property_attribute,
			boolean reoccurring_property_attribute_exists );

/* Returns heap memory */
/* ------------------- */
char *reoccurring_system_string(
			char *reoccurring_select,
			char *reoccurring_table,
			char *where );

/* Returns heap memory */
/* ------------------- */
char *reoccurring_memo(
			char *transaction_description,
			char *credit_account );

/* Returns heap memory or null */
/* --------------------------- */
char *reoccurring_max_transaction_date_time(
			char *transaction_table,
			char *full_name,
			char *street_address,
			char *debit_account,
			char *credit_account );
#endif

