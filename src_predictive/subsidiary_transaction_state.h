/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/subsidiary_transaction_state.h	*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef SUBSIDIARY_TRANSACTION_STATE_H
#define SUBSIDIARY_TRANSACTION_STATE_H

#include "list.h"
#include "boolean.h"
#include "preupdate_change.h"

typedef struct
{
	PREUPDATE_CHANGE *preupdate_change_full_name;
	PREUPDATE_CHANGE *preupdate_change_street_address;
	PREUPDATE_CHANGE *preupdate_change_foreign_date_time;
	PREUPDATE_CHANGE *preupdate_change_foreign_amount;
	PREUPDATE_CHANGE *preupdate_change_account_name;
} SUBSIDIARY_TRANSACTION_STATE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
SUBSIDIARY_TRANSACTION_STATE *
	subsidiary_transaction_state_new(
		const char *preupdate_full_name_placeholder,
		const char *preupdate_street_address_placeholder,
		const char *preupdate_foreign_date_time_placeholder,
		const char *preupdate_foreign_amount_placeholder,
		const char *preupdate_account_name_placeholder,
		char *state,
		char *preupdate_full_name,
		char *preupdate_street_address,
		char *preupdate_foreign_date_time,
		char *preupdate_foreign_amount,
		char *preupdate_account_name,
		char *full_name,
		char *street_address,
		char *foreign_date_time,
		char *foreign_amount_string,
		char *account_name );

/* Process */
/* ------- */
SUBSIDIARY_TRANSACTION_STATE *
	subsidiary_transaction_state_calloc(
		void );

#endif
