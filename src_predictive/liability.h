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
#include "transaction.h"

/* Constants */
/* --------- */
#define LIABILITY_MEMO		"Liability Payment"

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	double dialog_box_payment_amount;
	int starting_check_number;
	LIST *input_entity_list;

	/* Process */
	/* ------- */
	LIST *liability_balance_zero_account_list;
	char *account_loss;
	char *liability_credit_account_name;
	LIST *liability_account_entity_list;
	LIST *liability_tax_redirect_account_list;
	LIST *liability_entity_list;
	LIST *liability_balance_zero_entity_list;
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

char *liability_account_entity_system_string(
			char *where );

LIST *liability_account_entity_system_list(
			char *system_string );

LIABILITY *liability_new(
			double dialog_box_payment_amount,
			int starting_check_number,
			LIST *liability_balance_zero_account_list,
			LIST *liability_account_entity_list,
			LIST *input_entity_list );

LIABILITY *liability_calloc(
			void );

/* Sets account_balance_zero_journal_list */
/* -------------------------------------- */
LIST *liability_balance_zero_account_list(
			void );

LIST *liability_tax_redirect_account_list(
			LIST *liability_balance_zero_account_list,
			LIST *liability_account_entity_list );

LIST *liability_entity_list(
			LIST *liability_account_list,
			LIST *input_entity_list,
			double dialog_box_payment_amount );

LIST *liability_transaction_list(
			LIST *liability_entity_list,
			char *liability_credit_account_name,
			int starting_check_number );

void liability_set_entity(
			LIST *account_balance_zero_account_list,
			char *full_name,
			char *street_address );

ENTITY *liability_steady_state_entity(
			ENTITY *entity,
			LIST *liability_balance_zero_journal_list );

LIST *liability_steady_state_entity_list(
			LIST *liability_balance_zero_entity_list );

char *liability_credit_account_name(
			int starting_check_number );

char *liability_entity_debit_account_name(
			char *account_name );

/* Sets entity_balance_zero_journal_list */
/* ------------------------------------- */
LIST *liability_balance_zero_entity_list(
			LIST *liability_entity_list,
			LIST *liability_account_list );;

TRANSACTION *liability_transaction(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			double payment_amount,
			double additional_payment_amount,
			LIST *entity_balance_zero_account_list,
			char *liability_credit_account_name,
			char *memo,
			int check_number );

#endif
