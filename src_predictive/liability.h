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
	char *credit_account_name;
	char *loss_account_name;
	LIST *current_liability_account_list;
	LIST *entity_list;
	LIST *purchase_list;
	int starting_check_number;
	LIST *liability_account_entity_list;
} LIABILITY;

/* Operations */
/* ---------- */
LIABILITY *liability_calloc(
			void );

LIST *liability_account_entity_list(
			void );

#endif
