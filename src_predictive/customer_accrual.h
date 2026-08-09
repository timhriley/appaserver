/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/customer_accrual.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "boolean.h"
#include "list.h"
#include "receivable.h"
#include "liability.h"

typedef struct
{
	LIST *account_receivable_name_list;
	RECEIVABLE *receivable;
	LIST *exclude_account_name_list;
	LIST *account_current_liability_name_list;
	LIABILITY *liability;
	LIST *update_string_list;
} CUSTOMER_ACCRUAL;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CUSTOMER_ACCRUAL *customer_accrual_fetch(
		char *customer_full_name,
		char *customer_contact_key,
		boolean entity_contact_key_boolean,
		char *customer_sum_primary_data_string,
		boolean customer_sum_receivable_expected_boolean,
		boolean customer_sum_payable_due_boolean );

/* Process */
/* ------- */
CUSTOMER_ACCRUAL *customer_accrual_calloc(
		void );

