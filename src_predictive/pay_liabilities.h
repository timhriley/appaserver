/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/pay_liabilities.h			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef PAY_LIABILITIES_H
#define PAY_LIABILITIES_H

/* Includes */
/* -------- */
#include "list.h"
#include "entity.h"
#include "vendor_payment.h"
#include "account.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define PAY_LIABILITIES_MEMO	"Automatic Payment"

/* Structures */
/* ---------- */
typedef struct
{
	LIST *transaction_list;
	LIST *vendor_payment_list;
} PAY_LIABILITIES_OUTPUT;

typedef struct
{
	LIST *current_liability_entity_list;
	LIST *liability_account_entity_list;
	LIST *entity_list;
} PAY_LIABILITIES_PROCESS;

typedef struct
{
	LIST *liability_account_entity_list;
	double dialog_box_payment_amount;
	char *credit_account_name;
	char *loss_account_name;
	LIST *current_liability_account_list;
	LIST *entity_list;
	LIST *purchase_list;
	int starting_check_number;
} PAY_LIABILITIES_INPUT;

typedef struct
{
	PAY_LIABILITIES_INPUT input;
	PAY_LIABILITIES_PROCESS process;
	PAY_LIABILITIES_OUTPUT output;
} PAY_LIABILITIES;

/* Operations */
/* ---------- */
PAY_LIABILITIES *pay_liabilities_calloc(
			void );

PAY_LIABILITIES *pay_liabilities_new(
			char *fund_name,
			LIST *full_name_list,
			LIST *street_address_list,
			int starting_check_number,
			double dialog_box_payment_amount,
			char *transaction_memo );

LIST *pay_liabilities_input_entity_list(
			char *fund_name,
			LIST *full_name_list,
			LIST *street_address_list );

double pay_liabilities_fetch_sum_balance(
			char *fund_name,
			char *full_name,
			char *street_address );

LIST *pay_liabilities_liability_account_entity_list(
			LIST *input_liability_account_entity_list,
			LIST *input_entity_list,
			double dialog_box_payment_amount,
			int starting_check_number );

LIST *pay_liabilities_fetch_liability_account_entity_list(
			void );

LIST *pay_liabilities_current_liability_account_list(
			char *fund_name,
			LIST *exclude_account_name_list );

LIST *pay_liabilities_distribute_liability_account_list(
			LIST *liability_account_list,
			double entity_payment_amount );

LIST *pay_liabilities_process_entity_list(
			LIST *input_entity_list,
			LIST *process_current_liability_entity_list,
			LIST *input_purchase_order_list,
			double dialog_box_payment_amount,
			int starting_check_number );

LIST *pay_liabilities_distribute_purchase_list(
			LIST *input_purchase_list,
			double payment_amount,
			char *full_name,
			char *street_address );

LIST *pay_liabilities_current_liability_entity_list(
			LIST *current_liability_account_list,
			LIST *exclude_entity_list );

LIST *pay_liabilities_output_liability_account_transaction_list(
			LIST *liability_account_entity_list,
			char *credit_account_name,
			char *loss_account_name,
			char *memo );

LIST *pay_liabilities_output_entity_transaction_list(
			LIST *process_entity_list,
			char *credit_account_name,
			char *loss_account_name,
			char *memo,
			int seconds_to_add );

LIST *pay_liabilities_output_vendor_payment_list(
			LIST *process_entity_list,
			LIST *transaction_list );

VENDOR_PAYMENT *pay_liabilities_vendor_payment_seek(
			LIST *vendor_payment_list,
			char *full_name,
			char *street_address );

void pay_liabilities_set_lock_transaction(
			LIST *transaction_list,
			LIST *vendor_payment_list );

LIST *pay_liabilities_liability_account_list(
			void );

/* --------------------------------------------- */
/* Returns memo, program memory, or heap memory. */
/* --------------------------------------------- */
char *pay_liabilities_transaction_memo(
			char *fund_name,
			char *memo,
			int check_number );

LIST *pay_liabilities_liability_account_name_list(
			void );

#endif
