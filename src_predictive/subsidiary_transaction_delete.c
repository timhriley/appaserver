/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/subsidiary_transaction_delete.c	*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "appaserver_error.h"
#include "subsidiary_transaction_delete.h"

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
		PREUPDATE_CHANGE *preupdate_change_foreign_date_time )
{
	SUBSIDIARY_TRANSACTION_DELETE *subsidiary_transaction_delete;

	if ( !full_name
	||   !street_address
	||   !preupdate_change_full_name
	||   !preupdate_change_street_address
	||   !preupdate_change_foreign_date_time )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	subsidiary_transaction_delete = subsidiary_transaction_delete_calloc();

	subsidiary_transaction_delete->full_name =
		/* ------------------------ */
		/* Returns either parameter */
		/* ------------------------ */
		subsidiary_transaction_delete_datum(
			preupdate_full_name
				/* preupdate_attribute_datum */,
			full_name
				/* attribute_datum */,
			preupdate_change_full_name->
				no_change_boolean );

	subsidiary_transaction_delete->street_address =
		/* ------------------------ */
		/* Returns either parameter */
		/* ------------------------ */
		subsidiary_transaction_delete_datum(
			preupdate_street_address
				/* preupdate_attribute_datum */,
			street_address
				/* attribute_datum */,
			preupdate_change_street_address->
				no_change_boolean );

	subsidiary_transaction_delete->transaction_date_time =
		/* ------------------------ */
		/* Returns either parameter */
		/* ------------------------ */
		subsidiary_transaction_delete_datum(
			preupdate_foreign_date_time
				/* preupdate_attribute_datum */,
			foreign_date_time
				/* attribute_datum */,
			preupdate_change_foreign_date_time->
				no_change_boolean );

	if ( !subsidiary_transaction_delete->transaction_date_time )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
	"subsidiary_transaction_delete->transaction_date_time is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return subsidiary_transaction_delete;
}

SUBSIDIARY_TRANSACTION_DELETE *
	subsidiary_transaction_delete_calloc(
		void )
{
	SUBSIDIARY_TRANSACTION_DELETE *subsidiary_transaction_delete;

	if ( ! (  subsidiary_transaction_delete =
			calloc( 1,
				sizeof ( SUBSIDIARY_TRANSACTION_DELETE ) ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return subsidiary_transaction_delete;
}

char *subsidiary_transaction_delete_datum(
		char *preupdate_attribute_datum,
		char *attribute_datum,
		boolean no_change_boolean )
{
	char *delete_datum;

	if ( !attribute_datum )
		delete_datum = NULL;
	else
	if ( !preupdate_attribute_datum )
		delete_datum = attribute_datum;
	else
	if ( no_change_boolean )
		delete_datum = attribute_datum;
	else
		delete_datum = preupdate_attribute_datum;

	return delete_datum;
}
