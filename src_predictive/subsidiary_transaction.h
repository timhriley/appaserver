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
#include "subsidiary_transaction_delete.h"

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
		char *transaction_date_time,
		LIST *insert_journal_list,
		char *insert_full_name,
		char *insert_street_address,
		char *foreign_date_time,
		double foreign_amount,
		char *transaction_memo,
		boolean insert_boolean,
		boolean delete_boolean,
		SUBSIDIARY_TRANSACTION_DELETE *
			subsidiary_transaction_delete );

/* Process */
/* ------- */
SUBSIDIARY_TRANSACTION *subsidiary_transaction_calloc(
		void );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
TRANSACTION *subsidiary_transaction_delete_transaction(
		char *delete_full_name,
		char *delete_street_address,
		char *delete_transaction_date_time );

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
		char *transaction_date_time );

/* Driver */
/* ------ */
void subsidiary_transaction_execute(
		char *application_name,
		TRANSACTION *delete_transaction,
		TRANSACTION *insert_transaction,
		char *update_template );

#endif
