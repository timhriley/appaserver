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

#define REOCCURRING_TRANSACTION_TABLE	"reoccurring_transaction"

#define REOCCURRING_TRANSACTION_SELECT	"full_name,"			\
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
	char *rental_property_street_address;
	TRANSACTION *transaction;
} REOCCURRING;

/* Usage */
/* ----- */

/* Process */
/* ------- */

/* Operations */
/* ---------- */
REOCCURRING_TRANSACTION *reoccurring_transaction_calloc(
			void );

char *reoccurring_transaction_select(
			void );

char *reoccurring_transaction_primary_where(
			char *full_name,
			char *street_address,
			char *transaction_description );

/* Usage */
/* ----- */
REOCCURRING *reoccurring_parse(
			boolean reoccurring_property_attribute_exists,
			char *input );

/* Process */
/* ------- */

/* Usage */
/* ----- */
TRANSACTION *reoccurring_accrued_daily_transaction(
			char *full_name,
			char *street_address,
			char *transaction_description,
			char *transaction_increment_date_time,
			char *debit_account,
			char *credit_account,
			double accrued_daily_amount );

/* Process */
/* ------- */

LIST *reoccurring_existing_cash_journal_list(
			char *minimum_bank_date,
			char *fund_name );

LIST *reoccurring_transaction_list(
			void );

REOCCURRING_TRANSACTION *reoccurring_bank_upload_feeder_phrase(
			LIST *reoccurring_transaction_list,
			char *bank_description );

char *reoccurring_transaction_select(
			void );

/* --------------------- */
/* Returns static memory */
/* --------------------- */
char *reoccurring_memo(
			char *transaction_description,
			char *credit_account );

/* Usage */
/* ----- */
int reoccurring_days_between_last_transaction(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *debit_account,
			char *credit_account );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *reoccurring_transaction_subquery(
			char *debit_account,
			char *credit_account );

LIST *reoccurring_transaction_system_list(
			char *sys_string );

REOCCURRING_TRANSACTION *reoccurring_transaction_fetch(
			char *full_name,
			char *street_address,
			char *transaction_description );

REOCCURRING_STRUCTURE *reoccurring_structure_calloc(
			void );

REOCCURRING_TRANSACTION *reoccurring_transaction_new(
			char *full_name,
			char *street_address,
			char *transaction_description );

#endif

