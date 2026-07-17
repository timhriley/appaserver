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
	char *fund_name;
	char *full_name;
	char *contact_key;
	char *transaction_date_time;
} SUBSIDIARY_TRANSACTION_INSERT;

/* Usage */
/* ----- */
SUBSIDIARY_TRANSACTION_INSERT *
	subsidiary_transaction_insert_new(
		PREUPDATE_CHANGE *preupdate_change_fund_name,
		PREUPDATE_CHANGE *preupdate_change_full_name,
		PREUPDATE_CHANGE *preupdate_change_contact_key,
		PREUPDATE_CHANGE *preupdate_change_foreign_date_time );

/* Process */
/* ------- */
SUBSIDIARY_TRANSACTION_INSERT *
	subsidiary_transaction_insert_calloc(
		void );

typedef struct
{
	char *fund_name;
	char *full_name;
	char *contact_key;
	char *transaction_date_time;
} SUBSIDIARY_TRANSACTION_DELETE;

/* Usage */
/* ----- */
SUBSIDIARY_TRANSACTION_DELETE *
	subsidiary_transaction_delete_new(
		PREUPDATE_CHANGE *preupdate_change_fund_name,
		PREUPDATE_CHANGE *preupdate_change_full_name,
		PREUPDATE_CHANGE *preupdate_change_contact_key,
		PREUPDATE_CHANGE *preupdate_change_foreign_date_time );

/* Process */
/* ------- */
SUBSIDIARY_TRANSACTION_DELETE *
	subsidiary_transaction_delete_calloc(
		void );

typedef struct
{
	boolean predictive_fund_boolean;
	boolean entity_contact_key_boolean;
	TRANSACTION *delete_transaction;
	TRANSACTION *insert_transaction;
	char *update_template;
	char *update_null_sql;
} SUBSIDIARY_TRANSACTION;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
SUBSIDIARY_TRANSACTION *
	subsidiary_transaction_new(
		const char *foreign_table_name,
		const char *foreign_fund_name_column,
		const char *foreign_full_name_column,
		const char *foreign_contact_key_column,
		const char *foreign_date_time_column,
		const char *update_date_time_column,
		LIST *insert_journal_list,
		double foreign_amount,
		char *transaction_memo,
		SUBSIDIARY_TRANSACTION_INSERT *
			subsidiary_transaction_insert,
		SUBSIDIARY_TRANSACTION_DELETE *
			subsidiary_transaction_delete,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

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
		const char *foreign_fund_name_column,
		const char *foreign_full_name_column,
		const char *foreign_contact_key_column,
		const char *foreign_date_time_column,
		const char *update_date_time_column,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *foreign_date_time,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *subsidiary_transaction_update_null_sql(
		const char *foreign_table_name,
		const char *foreign_fund_name_column,
		const char *foreign_full_name_column,
		const char *foreign_contact_key_column,
		const char *foreign_date_time_column,
		const char *update_date_time_column,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *foreign_date_time,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *subsidiary_transaction_update_where(
		const char *foreign_fund_name_column,
		const char *foreign_full_name_column,
		const char *foreign_contact_key_column,
		const char *foreign_date_time_column,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *foreign_date_time,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Driver */
/* ------ */

/* ------------------------------------ */
/* Updates the parent table.		*/
/* Returns transaction_date_time.	*/
/* ------------------------------------ */
char *subsidiary_transaction_execute(
		char *application_name,
		TRANSACTION *delete_transaction,
		TRANSACTION *insert_transaction,
		char *update_template,
		char *update_null_sql,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

#endif
