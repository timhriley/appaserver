/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/reoccurring.h			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef REOCCURRING_H
#define REOCCURRING_H

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define FEEDER_PHRASE_DELIMITER		'|'
#define REOCCURRING_TRANSACTION_FOLDER_NAME \
					"reoccurring_transaction"

/* Structures */
/* ---------- */
typedef struct
{
	char *full_name;
	char *street_address;
	char *transaction_description;
	char *debit_account;
	char *credit_account;
	double transaction_amount;
	char *bank_upload_feeder_phrase;
	double accrued_daily_amount;
	double accrued_monthly_amount;
	char *rental_property_street_address;
} REOCCURRING_TRANSACTION;

typedef struct
{
	LIST *reoccurring_transaction_list;
} REOCCURRING_STRUCTURE;

/* Operations */
/* ---------- */
REOCCURRING_STRUCTURE *reoccurring_structure_new(
					void );

REOCCURRING_TRANSACTION *reoccurring_reoccurring_transaction_calloc(
					void );

REOCCURRING_TRANSACTION *reoccurring_transaction_fetch(
					char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_description,
					double transaction_amount );

boolean reoccurring_transaction_load(
					char **debit_account,
					char **credit_account,
					double *transaction_amount,
					double *accrued_daily_amount,
					double *accrued_monthly_amount,
					char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_description );

LIST *reoccurring_fetch_list(		char *application_name,
					int starting_sequence_number );

LIST *reoccurring_fetch_existing_cash_journal_ledger_list(
					char *application_name,
					char *minimum_bank_date,
					char *fund_name );

LIST *reoccurring_fetch_reoccurring_transaction_list(
					char *application_name );

void reoccurring_transaction_parse(
					char **full_name,
					char **street_address,
					char **debit_account,
					char **credit_account,
					double *transaction_amount,
					char **bank_upload_feeder_phrase,
					double *accrued_daily_amount,
					double *accrued_monthly_amount,
					char *input_buffer );

REOCCURRING_TRANSACTION *reoccurring_seek_bank_upload_feeder_phrase(
					LIST *reoccurring_transaction_list,
					char *bank_description );

char *reoccurring_transaction_select(
					void );

/* -------------------------- */
/* Returns 256K static memory */
/* -------------------------- */
char *reoccurring_memo(
					char *transaction_description,
					char *credit_account );

int reoccurring_days_between_last_transaction(
					char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_date_time,
					char *debit_account,
					char *credit_account );

void reoccurring_transaction_subquery(
					char *sub_query,
					char *debit_account,
					char *credit_account );

#endif

