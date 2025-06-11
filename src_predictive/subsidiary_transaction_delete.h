/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/subsidiary_transaction_delete.h	*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef SUBSIDIARY_TRANSACTION_DELETE_H
#define SUBSIDIARY_TRANSACTION_DELETE_H

#include "list.h"
#include "boolean.h"
#include "preupdate_change.h"

typedef struct
{
	char *full_name;
	char *street_address;
	char *transaction_date_time;
} SUBSIDIARY_TRANSACTION_DELETE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
SUBSIDIARY_TRANSACTION_DELETE *
	subsidiary_transaction_delete_new(
		char *preupdate_full_name,
		char *preupdate_street_address,
		char *preupdate_foreign_date_time,
		char *full_name,
		char *street_address,
		char *foreign_date_time,
		PREUPDATE_CHANGE *preupdate_change_full_name,
		PREUPDATE_CHANGE *preupdate_change_street_address,
		PREUPDATE_CHANGE *preupdate_change_foreign_date_time );

/* Process */
/* ------- */
SUBSIDIARY_TRANSACTION_DELETE *
	subsidiary_transaction_delete_calloc(
		void );

/* Usage */
/* ----- */

/* Returns either parameter */
/* ------------------------ */
char *subsidiary_transaction_delete_datum(
		char *preupdate_attribute_datum,
		char *attribute_datum,
		boolean no_change_boolean );

#endif
