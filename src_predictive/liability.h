/* --------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/liability.h	 */
/* --------------------------------------------- */
/*						 */
/* Freely available software: see Appaserver.org */
/* --------------------------------------------- */

#ifndef LIABILITY_H
#define LIABILITY_H

#include "entity.h"
#include "list.h"

/* Constants */
/* --------- */
#define LIABILITY_MEMO		"Liability Payment"

/* Structures */
/* ---------- */
typedef struct
{
	double dialog_box_payment_amount;
	int starting_check_number;
	char *account_loss;
	char *liability_credit_account_name;
	LIST *liability_account_entity_list;
	LIST *liability_current_account_list;
	LIST *liability_tax_redirect_account_list;
	LIST *liability_entity_list;
	LIST *liability_after_balance_zero_entity_list;
	LIST *liability_steady_state_entity_list;
} LIABILITY;

typedef struct
{
	char *account_name;
	ENTITY *entity;
} LIABILITY_ACCOUNT_ENTITY;

/* Operations */
/* ---------- */
LIABILITY_ACCOUNT_ENTITY *liability_account_entity_seek(
			char *account_name,
			LIST *liability_account_entity_list );

LIABILITY_ACCOUNT_ENTITY *liability_account_entity_calloc(
			void );

LIABILITY_ACCOUNT_ENTITY *liability_account_entity_parse(
			char *input );

LIST *liability_account_entity_list(
			void );

char *liability_account_entity_sys_string(
			char *where );

LIST *liability_account_entity_system_list(
			char *sys_string );

LIABILITY *liability_new(
			double dialog_box_payment_amount,
			int starting_check_number  );

LIABILITY *liability_calloc(
			void );

LIST *liability_current_account_list(
			void );

LIST *liability_tax_redirect_account_list(
			LIST *liability_current_account_list,
			LIST *liability_account_entity_list );

/* Also sets entity->liability_entity_debit_account_name */
/* ----------------------------------------------------- */
LIST *liability_entity_list(
			LIST *liability_account_list,
			LIST *input_entity_list,
			double dialog_box_payment_amount );

double liability_entity_amount_due(
			LIST *journal_list );

LIST *liability_transaction_list(
			LIST *liability_entity_list,
			char *liability_credit_account_name,
			char *account_loss,
			int starting_check_number );

TRANSACTION *liability_entity_transaction(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double payment_amount,
			double loss_amount,
			char *liability_entity_debit_account_name,
			char *account_loss,
			char *liability_credit_account_name,
			char *memo,
			int check_number );

void liability_set_entity(
			LIST *transaction_after_balance_zero_journal_list,
			char *full_name,
			char *street_address );

ENTITY *liability_steady_state_entity(
			ENTITY *entity,
			LIST *liability_after_balance_zero_journal_list );

double liability_entity_payment_amount(
			double dialog_box_payment_amount,
			double liability_entity_amount_due );

double liability_entity_loss_amount(
			double dialog_box_payment_amount,
			double liability_entity_payment_amount );

LIST *liability_steady_state_entity_list(
			LIST *entity_list );

char *liability_credit_account_name(
			int starting_check_number );

LIST *liability_after_balance_zero_journal_list(
			LIST *liability_tax_redirect_account_list,
			char *full_name,
			char *street_address );

char *liability_entity_debit_account_name(
			char *account_name );

LIST *liability_after_balance_zero_entity_list(
			LIST *liability_entity_list,
			LIST *liability_account_list );;

#endif
