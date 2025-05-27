/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/subsidiary_transaction_state.c	*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "subsidiary_transaction_state.h"

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
		char *foreign_date_time )
{
	SUBSIDIARY_TRANSACTION_STATE *subsidiary_transaction_state;

	if ( !state
	||   !preupdate_full_name
	||   !preupdate_street_address
	||   !preupdate_foreign_date_time )
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

	subsidiary_transaction_state =
		subsidiary_transaction_state_calloc();

	subsidiary_transaction_state->preupdate_change_full_name =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		preupdate_change_new(
			APPASERVER_INSERT_STATE,
			APPASERVER_PREDELETE_STATE,
			state,
			preupdate_full_name
				/* preupdate_datum */,
			full_name
				/* postupdate_datum */,
			preupdate_full_name_placeholder
				/* preupdate_placeholder_name */ );

	subsidiary_transaction_state->preupdate_change_street_address =
		preupdate_change_new(
			APPASERVER_INSERT_STATE,
			APPASERVER_PREDELETE_STATE,
			state,
			preupdate_street_address,
			street_address,
			preupdate_street_address_placeholder );

	subsidiary_transaction_state->preupdate_change_foreign_date_time =
		preupdate_change_new(
			APPASERVER_INSERT_STATE,
			APPASERVER_PREDELETE_STATE,
			state,
			preupdate_foreign_date_time,
			foreign_date_time,
			preupdate_foreign_date_time_placeholder );

	return subsidiary_transaction_state;
}

SUBSIDIARY_TRANSACTION_STATE *
	subsidiary_transaction_state_calloc(
		void )
{
	SUBSIDIARY_TRANSACTION_STATE *
		subsidiary_transaction_state;

	if ( ! ( subsidiary_transaction_state =
		  calloc(
			1,
			sizeof ( SUBSIDIARY_TRANSACTION_STATE ) ) ) )
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

	return subsidiary_transaction_state;
}
