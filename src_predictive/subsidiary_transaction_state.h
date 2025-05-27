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
		char *state,
		char *preupdate_full_name,
		char *preupdate_street_address,
		char *preupdate_foreign_date_time,
		char *full_name,
		char *street_address,
		char *foreign_date_time );

/* Process */
/* ------- */
SUBSIDIARY_TRANSACTION_STATE *
	subsidiary_transaction_state_calloc(
		void );

#endif
