/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/investment.h		*/
/* ---------------------------------------------------- */
/* This is the appaserver investment ADT.		*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef INVESTMENT_H
#define INVESTMENT_H

#include "folder.h"
#include "list.h"

/* Constants */
/* --------- */
#define ACCOUNT_BALANCE_FOLDER_NAME		"equity_account_balance"
#define INVESTMENT_ACCOUNT_FOLDER_NAME		"investment_account"

/* Must match INVESTMENT_OPERATION */
/* ------------------------------- */
#define INVESTMENT_OPERATION_TRANSFER		"transfer"
#define INVESTMENT_OPERATION_PURCHASE		"purchase"
#define INVESTMENT_OPERATION_TIME_PASSAGE	"time_passage"
#define INVESTMENT_OPERATION_SALE		"sale"

/* Structures */
/* ---------- */
typedef struct
{
	char *full_name;
	char *street_address;
	char *account_number;
	char *classification;
	char *investment_account;
	char *fair_value_adjustment_account;
} INVESTMENT_ACCOUNT;

typedef struct
{
/*
	double share_price;
	double share_quantity_change;
*/
	enum preupdate_change_state full_name_change_state;
	enum preupdate_change_state street_address_change_state;
	enum preupdate_change_state account_number_change_state;
	enum preupdate_change_state date_time_change_state;
	char *earlier_date_time;
	char *begin_date_time;
} INVESTMENT_PROCESS;

typedef struct
{
	char *date_time;
	char *fund_name;
	char *state;
	char *preupdate_full_name;
	char *preupdate_street_address;
	char *preupdate_account_number;
	char *preupdate_date_time;
} INVESTMENT_INPUT;

typedef struct
{
	char *date_time;
	char *investment_operation;
	double share_price;
	double share_quantity_change;
	double share_quantity_balance;
	double cash_in;
	double market_value;
	double book_value_change;
	double book_value_balance;
	double moving_share_price;
	double unrealized_gain_balance;
	double unrealized_gain_change;
	double realized_gain;
	char *transaction_date_time;
	TRANSACTION *transaction;
	boolean table_first_row;
} ACCOUNT_BALANCE;

typedef struct
{
	INVESTMENT_INPUT input;
	INVESTMENT_ACCOUNT investment_account;
	INVESTMENT_PROCESS *process;
	LIST *input_account_balance_list;
	LIST *output_account_balance_list;
} INVESTMENT_EQUITY;

/* Operations */
/* ---------- */
INVESTMENT_EQUITY *investment_equity_new(
					char *application_name,
					char *full_name,
					char *street_address,
					char *fund_name,
					char *account_number,
					char *date_time,
					char *state,
					char *preupdate_full_name,
					char *preupdate_street_address,
					char *preupdate_account_number,
					char *preupdate_date_time );

INVESTMENT_PROCESS *investment_process_new(
					char *full_name,
					char *street_address,
					char *account_number,
					char *date_time,
					char *preupdate_full_name,
					char *preupdate_street_address,
					char *preupdate_account_number,
					char *preupdate_date_time );

ACCOUNT_BALANCE *investment_account_balance_seek(
					LIST *account_balance_list,
					char *date_time );

ACCOUNT_BALANCE *investment_account_balance_new(
					char *date_time );

boolean investment_fetch_account_balance(
					double *share_price,
					double *share_quantity_change,
					char *application_name,
					char *full_name,
					char *street_address,
					char *account_number,
					char *date_time );

boolean investment_fetch_account(	char **investment_account,
					char **fair_value_adjustment_account,
					char **classification,
					char *application_name,
					char *full_name,
					char *street_address,
					char *account_number );

ACCOUNT_BALANCE *investment_account_balance_fetch(
					char *application_name,
					char *full_name,
					char *street_address,
					char *account_number,
					char *date_time );

char *investment_account_balance_fetch_prior_date_time(
					char *application_name,
					char *full_name,
					char *street_address,
					char *account_number,
					char *date_time );

void investment_account_balance_list_update(
					LIST *output_account_balance_list,
					char *application_name,
					LIST *input_account_balance_list,
					char *full_name,
					char *street_address,
					char *account_number );

void investment_account_balance_update(	FILE *output_pipe,
					ACCOUNT_BALANCE *output_account_balance,
					char *application_name,
					ACCOUNT_BALANCE *input_account_balance,
					char *full_name,
					char *street_address,
					char *account_number );

ACCOUNT_BALANCE *investment_account_balance_calculate(
					char *date_time,
					double share_price,
					double share_quantity_change,
					double prior_share_quantity_balance,
					double prior_book_value_balance,
					double prior_moving_share_price,
					double prior_unrealized_gain_balance,
					char *investment_operation,
					boolean table_first_row );

ACCOUNT_BALANCE *investment_account_balance_purchase(
					char *date_time,
					double share_price,
					double share_quantity_change,
					double prior_share_quantity_balance,
					double prior_book_value_balance,
					double prior_unrealized_gain_balance,
					boolean table_first_row );

ACCOUNT_BALANCE *investment_account_balance_time_passage(
					char *date_time,
					double share_price,
					double share_quantity_change,
					double prior_share_quantity_balance,
					double prior_moving_share_price,
					double prior_unrealized_gain_balance );

ACCOUNT_BALANCE *investment_account_balance_sale(
					char *date_time,
					double share_price,
					double share_quantity_change,
					double prior_share_quantity_balance,
					double prior_book_value_balance,
					double prior_moving_share_price,
					double prior_unrealized_gain_balance );

ACCOUNT_BALANCE *investment_account_balance_parse(
					char *input_buffer );

LIST *investment_get_fair_value_adjustment_ledger_list(
					double transaction_amount,
					char *debit_account,
					char *credit_account );

FILE *investment_open_update_pipe(	void );

TRANSACTION *investment_build_transaction(
					char *application_name,
					char *full_name,
					char *street_address,
					char *date_time,
					char *fund_name,
					char *investment_operation,
					char *investment_account,
					char *fair_value_adjustment_account,
					double share_quantity_change,
					double book_value_change,
					double unrealized_gain_change,
					double realized_gain );

TRANSACTION *investment_build_purchase_transaction(
					char *application_name,
					char *full_name,
					char *street_address,
					char *date_time,
					char *fund_name,
					char *investment_operation,
					char *investment_account,
					char *fair_value_adjustment_account,
					char *credit_account,
					double share_quantity_change,
					double book_value_change,
					double unrealized_gain_change );

TRANSACTION *investment_build_sale_transaction(
					char *application_name,
					char *full_name,
					char *street_address,
					char *date_time,
					char *fund_name,
					char *investment_operation,
					char *investment_account,
					char *fair_value_adjustment_account,
					double unrealized_gain_change,
					double realized_gain,
					double book_value_change );

TRANSACTION *investment_build_time_passage_transaction(
					char *application_name,
					char *full_name,
					char *street_address,
					char *date_time,
					char *fund_name,
					char *investment_operation,
					char *investment_account,
					char *fair_value_adjustment_account,
					double unrealized_gain_change,
					double realized_gain );

char *investment_get_memo(		char *investment_operation );

LIST *investment_fetch_account_balance_list(
					char *application_name,
					char *full_name,
					char *street_address,
					char *account_number,
					char *begin_date_time );

char *investment_account_balance_get_select(
					void );

char *investment_get_update_sys_string(
					char *application_name );

void investment_transaction_date_time_update(
					char *full_name,
					char *street_address,
					char *account_number,
					char *date_time,
					char *transaction_date_time,
					char *application_name );

char *investment_fetch_cash_or_equity_account_name(
					char *application_name,
					char *fund_name,
					char *investment_operation );

boolean investment_get_account_balance_table_first_row(
				char *application_name,
				char *full_name,
				char *street_address,
				char *account_number,
				char *date_time );

LIST *investment_calculate_account_balance_list(
					char *application_name,
					char *full_name,
					char *street_address,
					char *fund_name,
					char *investment_account,
					char *fair_value_adjustment_account,
					LIST *input_account_balance_list );
#endif
