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
#include "subsidiary_transaction.h"

typedef struct
{
	char *predictive_fund_name;
	PREUPDATE_CHANGE *preupdate_change_fund_name;
	PREUPDATE_CHANGE *preupdate_change_full_name;
	char *entity_contact_key;
	PREUPDATE_CHANGE *preupdate_change_contact_key;
	PREUPDATE_CHANGE *preupdate_change_foreign_date_time;
	LIST *old_journal_list;
	boolean journal_exist_boolean;
	boolean journal_list_match_boolean;
	SUBSIDIARY_TRANSACTION_INSERT *subsidiary_transaction_insert;
	SUBSIDIARY_TRANSACTION_DELETE *subsidiary_transaction_delete;
} SUBSIDIARY_TRANSACTION_STATE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
SUBSIDIARY_TRANSACTION_STATE *
	subsidiary_transaction_state_new(
		const char *preupdate_fund_name_placeholder,
		const char *preupdate_full_name_placeholder,
		const char *preupdate_contact_key_placeholder,
		const char *preupdate_foreign_date_time_placeholder,
		char *state,
		char *preupdate_fund_name,
		char *preupdate_full_name,
		char *preupdate_contact_key,
		char *preupdate_foreign_date_time,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *foreign_date_time,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		LIST *insert_journal_list );

/* Process */
/* ------- */
SUBSIDIARY_TRANSACTION_STATE *
	subsidiary_transaction_state_calloc(
		void );

/* Any journal rows exist? */
/* ----------------------- */
boolean subsidiary_transaction_state_journal_exist_boolean(
		LIST *old_journal_list );

#endif
