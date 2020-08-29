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
	char *bank_upload_feeder_phrase;
	boolean feeder_phrase_ignore;
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
REOCCURRING_TRANSACTION *reoccurring_transaction_calloc(
			void );

char *reoccurring_transaction_select(
			void );

char *reoccurring_transaction_primary_where(
			char *full_name,
			char *street_address,
			char *transaction_description );

REOCCURRING_TRANSACTION *reoccurring_transaction_parse(
			char *input );

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

int reoccurring_days_between_last_transaction(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *debit_account,
			char *credit_account );

void reoccurring_transaction_subquery(
			char *sub_query,
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

