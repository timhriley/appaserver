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
	LIST *liability_current_liability_account_list;
	LIST *liability_entity_list;
	LIST *liability_account_entity_list;
} LIABILITY;

/* Operations */
/* ---------- */
LIABILITY *liability_calloc(
			void );

LIST *liability_entity_list(
			LIST *current_liability_account_list );

LIST *liability_current_liability_account_list(
			void );

LIST *liability_account_entity_list(
			LIST *entity_list,
			double dialog_box_payment_amount,
			int starting_check_number,
			LIST *current_liability_account_list );

#endif
