/* ---------------------------------------------*/
/* $APPASERVER_HOME/src_predictive/liability.h	*/
/* ---------------------------------------------*/
/*						*/
/* Freely available software: see Appaserver.org*/
/* ---------------------------------------------*/

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
	LIST *liability_entity_list;
} LIABILITY;

typedef struct
{
	char *account_name;
	ENTITY *entity;
} LIABILITY_ACCOUNT_ENTITY;

/* Operations */
/* ---------- */
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

LIABILITY *liability_calloc(
			void );

LIST *liability_current_account_list(
			LIST *liability_account_entity_list );

LIST *liability_entity_list(
			LIST *liability_current_account_list );

ENTITY *liability_account_entity(
			LIST *liability_account_entity_list,
			char *account_name );

LIST *liability_entity_list(
			LIST *account_list );

double liability_entity_amount_due(
			LIST *journal_list );

#endif
