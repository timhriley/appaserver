/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/transaction_balance.h		*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef TRANSACTION_BALANCE_H
#define TRANSACTION_BALANCE_H

#include "list.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define TRANSACTION_BALANCE_SELECT		\
	"transaction_date_time,bank_date,bank_description,full_name,street_address,transaction_amount,bank_amount,cash_running_balance,bank_running_balance,sequence_number"

/* Structures */
/* ---------- */
typedef struct
{
	char *transaction_date_time;
	char *bank_date;
	char *bank_description;
	char *full_name;
	char *street_address;
	double transaction_amount;
	double bank_amount;
	double cash_running_balance;
	double bank_running_balance;
	double cash_running_balance_wrong;
	double bank_running_balance_wrong;
	int sequence_number;
} TRANSACTION_BALANCE_ROW;

typedef struct
{
	double cash_ending_balance;
	char *begin_date;
	LIST *transaction_balance_row_list;
} TRANSACTION_BALANCE_INPUT;

typedef struct
{
	TRANSACTION_BALANCE_ROW *begin_transaction_balance;
	TRANSACTION_BALANCE_ROW *end_transaction_balance;
	int block_count;
	boolean is_inbalance;
} TRANSACTION_BALANCE_BLOCK;

typedef struct
{
	TRANSACTION_BALANCE_INPUT input;
	LIST *inbalance_block_list;
	LIST *outbalance_block_list;
	LIST *merged_block_list;
	boolean last_block_inbalance;
} TRANSACTION_BALANCE;

/* Operations */
/* ---------- */
TRANSACTION_BALANCE *transaction_balance_calloc(
					void );

TRANSACTION_BALANCE_BLOCK *transaction_balance_block_new(
					void );

TRANSACTION_BALANCE_ROW *transaction_balance_row_new(
					void );

TRANSACTION_BALANCE *transaction_balance_new(
					char *begin_date,
					double cash_ending_balance );

TRANSACTION_BALANCE_ROW *transaction_balance_prior_fetch(
					char *application_name,
					char *transaction_date_time );

TRANSACTION_BALANCE_ROW *transaction_balance_transaction_date_time_fetch(
					char *application_name,
					char *transaction_date_time );

LIST *transaction_balance_fetch_row_list(
			char *begin_date );

TRANSACTION_BALANCE_ROW *transaction_balance_parse_row(
					char *input_buffer );

LIST *transaction_balance_inbalance_block_list(
					LIST *transaction_balance_row_list );

LIST *transaction_balance_outbalance_block_list(
					LIST *transaction_balance_row_list );

LIST *transaction_balance_merged_block_list(
					LIST *inbalance_block_list,
					LIST *outbalance_block_list );

double transaction_balance_anomaly_balance_difference(
			double cash_running_balance,
			double bank_running_balance );

void transaction_balance_row_stdout(
					TRANSACTION_BALANCE_ROW *row );

boolean transaction_balance_last_block_inbalance(
					LIST *merged_block_list );

boolean transaction_balance_cash_running_balance_wrong(
			char *first_outbalance_transaction_date_time,
			LIST *transaction_balance_row_list,
			double bank_amount );

boolean transaction_balance_bank_running_balance_wrong(
			char *first_outbalance_transaction_date_time,
			LIST *transaction_balance_row_list,
			double bank_amount );

char *transaction_balance_row_display(
			TRANSACTION_BALANCE_ROW *row,
			LIST *transaction_balance_row_list,
			double bank_amount,
			boolean cash_running_balance_wrong,
			boolean bank_running_balance_wrong,
			int sequence_number );

boolean transaction_balance_debit_credit_reversed(
			double anomaly_balance_difference,
			double transaction_amount,
			double bank_amount,
			boolean cash_running_balance_wrong,
			boolean bank_running_balance_wrong );

#endif

