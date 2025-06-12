/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/subsidiary_transaction.h		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef SUBSIDIARY_TRANSACTION_H
#define SUBSIDIARY_TRANSACTION_H

#include "list.h"
#include "boolean.h"
#include "transaction.h"
#include "preupdate_change.h"

typedef struct
{
	char *full_name;
	char *street_address;
	char *transaction_date_time;
} SUBSIDIARY_TRANSACTION_INSERT;

/* Usage */
/* ----- */
SUBSIDIARY_TRANSACTION_INSERT *
	subsidiary_transaction_insert_new(
		PREUPDATE_CHANGE *preupdate_change_full_name,
		PREUPDATE_CHANGE *preupdate_change_street_address,
		PREUPDATE_CHANGE *preupdate_change_foreign_date_time,
		boolean journal_list_match_boolean );

/* Process */
/* ------- */
SUBSIDIARY_TRANSACTION_INSERT *
	subsidiary_transaction_insert_calloc(
		void );

typedef struct
{
	char *full_name;
	char *street_address;
	char *transaction_date_time;
} SUBSIDIARY_TRANSACTION_DELETE;

/* Usage */
/* ----- */
SUBSIDIARY_TRANSACTION_DELETE *
	subsidiary_transaction_delete_new(
		PREUPDATE_CHANGE *preupdate_change_full_name,
		PREUPDATE_CHANGE *preupdate_change_street_address,
		PREUPDATE_CHANGE *preupdate_change_foreign_date_time,
		boolean journal_list_match_boolean );

/* Process */
/* ------- */
SUBSIDIARY_TRANSACTION_DELETE *
	subsidiary_transaction_delete_calloc(
		void );

typedef struct
{
	TRANSACTION *delete_transaction;
	TRANSACTION *insert_transaction;
	char *update_template;
} SUBSIDIARY_TRANSACTION;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
SUBSIDIARY_TRANSACTION *
	subsidiary_transaction_new(
		const char *foreign_table_name,
		const char *foreign_full_name_column,
		const char *foreign_street_address_column,
		const char *foreign_date_time_column,
		char *prior_transaction_date_time,
		LIST *insert_journal_list,
		double foreign_amount,
		char *transaction_memo,
		SUBSIDIARY_TRANSACTION_INSERT *
			subsidiary_transaction_insert,
		SUBSIDIARY_TRANSACTION_DELETE *
			subsidiary_transaction_delete );

/* Process */
/* ------- */
SUBSIDIARY_TRANSACTION *subsidiary_transaction_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *subsidiary_transaction_update_template(
		const char *foreign_table_name,
		const char *foreign_full_name_column,
		const char *foreign_street_address_column,
		const char *foreign_date_time_column,
		char *full_name,
		char *street_address,
		char *foreign_date_time,
		char *prior_transaction_date_time );

/* Driver */
/* ------ */
void subsidiary_transaction_execute(
		char *application_name,
		TRANSACTION *delete_transaction,
		TRANSACTION *insert_transaction,
		char *update_template );

#endif
