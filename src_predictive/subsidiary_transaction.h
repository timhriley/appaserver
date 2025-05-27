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
	boolean preupdate_change;
	LIST *journal_transaction_list;
	boolean exist_boolean;
	boolean journal_list_match_boolean;
	boolean delete_boolean;
	char *delete_full_name;
	char *delete_street_address;
	char *delete_transaction_date_time;
	TRANSACTION *delete_transaction;
	boolean insert_boolean;
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
		char *preupdate_full_name,
		char *preupdate_street_address,
		char *preupdate_foreign_date_time,
		char *full_name,
		char *street_address,
		char *foreign_date_time,
		double foreign_amount,
		PREUPDATE_CHANGE *preupdate_change_full_name,
		PREUPDATE_CHANGE *preupdate_change_street_address,
		PREUPDATE_CHANGE *preupdate_change_foreign_date_time,
		LIST *insert_journal_list,
		char *transaction_memo );

/* Process */
/* ------- */
SUBSIDIARY_TRANSACTION *subsidiary_transaction_calloc(
		void );

boolean subsidiary_transaction_exist_boolean(
		LIST *journal_transaction_list );

boolean subsidiary_transaction_delete_boolean(
		boolean subsidiary_transaction_preupdate_change,
		boolean subsidiary_transaction_exist_boolean,
		boolean journal_list_match_boolean );

boolean subsidiary_transaction_insert_boolean(
		LIST *insert_journal_list,
		boolean subsidiary_transaction_exist_boolean,
		boolean journal_list_match_boolean );

/* Usage */
/* ----- */
boolean subsidiary_transaction_preupdate_change(
		PREUPDATE_CHANGE *preupdate_change_full_name,
		PREUPDATE_CHANGE *preupdate_change_street_address,
		PREUPDATE_CHANGE *preupdate_change_foreign_date_time );

/* Usage */
/* ----- */

/* Returns attribute_datum, preupdate_attribute_datum, or null */
/* ----------------------------------------------------------- */
char *subsidiary_transaction_delete_datum(
		char *attribute_datum,
		char *preupdate_attribute_datum,
		PREUPDATE_CHANGE *preupdate_change_datum );

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

/* Returns heap memory */
/* ------------------- */
char *subsidiary_transaction_update_template(
		const char *foreign_table_name,
		const char *foreign_full_name_column,
		const char *foreign_street_address_column,
		const char *foreign_date_time_column,
		char *full_name,
		char *street_address,
		char *foreign_date_time );

/* Driver */
/* ------ */
void subsidiary_transaction_execute(
		char *application_name,
		TRANSACTION *delete_transaction,
		TRANSACTION *insert_transaction,
		char *update_template );

#endif
