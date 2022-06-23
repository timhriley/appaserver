/* --------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/liability.h	 */
/* --------------------------------------------- */
/*						 */
/* Freely available software: see Appaserver.org */
/* --------------------------------------------- */

#ifndef LIABILITY_H
#define LIABILITY_H

#include "list.h"
#include "entity.h"
#include "transaction.h"

#define LIABILITY_ACCOUNT_ENTITY_TABLE		"liability_account_entity"

#define LIABILITY_ACCOUNT_ENTITY_SELECT		"account,"	\
						"full_name,"	\
						"street_address"

typedef struct
{
	char *account_name;
	ENTITY *entity;
} LIABILITY_ACCOUNT_ENTITY;

/* Usage */
/* ----- */
LIST *liability_account_entity_list(
			void );

/* Process */
/* ------- */
char *liability_account_entity_system_string(
			char *liability_account_entity_select,
			char *liability_account_entity_table );


LIST *liability_account_entity_system_list(
			char *liability_account_entity_system_string );

LIABILITY_ACCOUNT_ENTITY *liability_account_entity_parse(
			char *string_input );

LIABILITY_ACCOUNT_ENTITY *liability_account_entity_new(
			char *account_name );

LIABILITY_ACCOUNT_ENTITY *liability_account_entity_calloc(
			void );

/* Public */
/* ------ */
LIABILITY_ACCOUNT_ENTITY *liability_account_entity_seek(
			char *account_name,
			LIST *liability_account_entity_list );

#define LIABILITY_MEMO		"Liability Payment"

typedef struct
{
	LIST *liability_account_entity_list;
	char *account_where;
	LIST *following_balance_zero_account_list;
	LIST *tax_redirect_account_list;
	char *credit_account_name;
	LIST *account_entity_list;
	LIST *following_balance_zero_entity_list;
	LIST *steady_state_entity_list;
} LIABILITY;

/* Usage */
/* ----- */
LIABILITY *liability_new(
			double dialog_box_payment_amount,
			int starting_check_number,
			LIST *entity_full_street_list );

/* Process */
/* ------- */
LIABILITY *liability_calloc(
			void );

/* Returns static memory */
/* --------------------- */
char *liability_account_where( void )

/* Sets account_following_balance_zero_journal_list */
/* ------------------------------------------------ */
LIST *liability_following_balance_zero_account_list(
			char *liability_account_where );

LIST *liability_tax_redirect_account_list(
			LIST *liability_following_balance_zero_account_list,
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
