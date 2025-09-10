/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/reverse_nominal.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
#include "appaserver_error.h"
#include "close_nominal.h"
#include "reverse_nominal.h"

REVERSE_NOMINAL_DO *reverse_nominal_do_calloc( void )
{
	REVERSE_NOMINAL_DO *reverse_nominal_do;

	if ( ! ( reverse_nominal_do =
			calloc( 1,
				sizeof ( REVERSE_NOMINAL_DO ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return reverse_nominal_do;
}

REVERSE_NOMINAL_DO *reverse_nominal_do_fetch( char *reverse_date_string )
{
	REVERSE_NOMINAL_DO *reverse_nominal_do;

	if ( !reverse_date_string )
	{
		char message[ 128 ];

		sprintf(message, "reverse_date_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	reverse_nominal_do = reverse_nominal_do_calloc();

	reverse_nominal_do->
		transaction_date_reverse_nominal_do =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			transaction_date_reverse_nominal_do_new(
				reverse_date_string );

	if ( reverse_nominal_do->
		transaction_date_reverse_nominal_do->
		transaction_date_close_boolean )
	{
		return reverse_nominal_do;
	}

	reverse_nominal_do->element_name_list =
		reverse_nominal_do_element_name_list(
			ELEMENT_REVENUE,
			ELEMENT_EXPENSE,
			ELEMENT_EXPENDITURE,
			ELEMENT_GAIN,
			ELEMENT_LOSS );

	reverse_nominal_do->element_statement_list =
		element_statement_list(
			reverse_nominal_do->element_name_list,
			reverse_nominal_do->
				transaction_date_reverse_nominal_do->
				transaction_date_close_date_time_string
				/* end_date_time_string */,
			1 /* fetch_subclassification_list */,
			1 /* fetch_account_list */,
			1 /* fetch_journal_latest */,
			0 /* not fetch_transaction */ );

	reverse_nominal_do->equity_subclassification_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		reverse_nominal_do_equity_subclassification_where(
			SUBCLASSIFICATION_DRAWING,
			SUBCLASSIFICATION_APPROPRIATION,
			SUBCLASSIFICATION_ENCUMBRANCE,
			SUBCLASSIFICATION_ESTIMATED_INTERFUND,
			SUBCLASSIFICATION_ESTIMATED_REVENUE,
			SUBCLASSIFICATION_INTERFUND );

	reverse_nominal_do->equity_subclassification_statement_list =
		subclassification_where_statement_list(
			reverse_nominal_do->equity_subclassification_where,
			reverse_nominal_do->
				transaction_date_reverse_nominal_do->
				transaction_date_close_date_time_string
					/* end_date_time_string */,
			1 /* fetch_element */,
			1 /* fetch_account_list */,
			1 /* fetch_journal_latest */,
			0 /* not fetch_transaction */ );

	reverse_nominal_do->entity_self =
		entity_self_fetch(
			0 /* not fetch_entity_boolean */ );

	if ( !reverse_nominal_do->entity_self )
	{
		char message[ 128 ];

		sprintf(message,
			"entity_self_fetch() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	reverse_nominal_do->close_transaction =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		close_transaction_new(
			reverse_nominal_do->
				transaction_date_reverse_nominal_do->
				transaction_date_close_date_time_string,
			reverse_nominal_do->element_statement_list,
			reverse_nominal_do->
				equity_subclassification_statement_list,
			reverse_nominal_do->entity_self->full_name,
			reverse_nominal_do->entity_self->street_address );

	if ( reverse_nominal_do->close_transaction->transaction )
	{
		reverse_nominal_do->journal_debit_sum =
			journal_debit_sum(
				reverse_nominal_do->
					close_transaction->
					transaction->
					journal_list );

		reverse_nominal_do->journal_credit_sum =
			journal_credit_sum(
				reverse_nominal_do->
					close_transaction->
					transaction->
					journal_list );
	}

	return reverse_nominal_do;
}

REVERSE_NOMINAL *reverse_nominal_fetch(
		char *application_name,
		char *process_name,
		char *reverse_date_string,
		boolean undo )
{
	REVERSE_NOMINAL *reverse_nominal;

	if ( !application_name
	||   !process_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	reverse_nominal = reverse_nominal_calloc();

	if ( undo )
	{
		reverse_nominal->reverse_nominal_undo =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			reverse_nominal_undo_fetch(
				TRANSACTION_TABLE,
				TRANSACTION_CLOSE_MEMO );

		if ( !reverse_nominal->
			reverse_nominal_undo->
			transaction_date_reverse_nominal_undo->
			transaction_date_time_memo_maximum_string )
		{
			reverse_nominal->undo_no_transaction_message =
				/* ---------------------- */
				/* Returns program memory */
				/* ---------------------- */
				reverse_nominal_undo_no_transaction_message();

			reverse_date_string = (char *)0;

			goto set_statement_caption;
		}

		reverse_date_string =
		    reverse_nominal->
			reverse_nominal_undo->
			transaction_date_reverse_nominal_undo->
			transaction_date_time_memo_maximum_string;
	}
	else
	{
		if ( !string_populated_boolean(
			TRANSACTION_DATE_AS_OF_DATE_FILLER,
			reverse_date_string ) )
		{
			reverse_nominal->empty_date_message =
				/* ---------------------- */
				/* Returns program memory */
				/* ---------------------- */
				reverse_nominal_empty_date_message();

			reverse_date_string = (char *)0;

			goto set_statement_caption;
		}

		reverse_nominal->reverse_nominal_do =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			reverse_nominal_do_fetch(
				reverse_date_string );

		if ( reverse_nominal->
			reverse_nominal_do->
			transaction_date_reverse_nominal_do->
			transaction_date_close_boolean )
		{
			reverse_nominal->do_transaction_exists_message =
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				reverse_nominal_do_transaction_exists_message(
				   reverse_nominal->
				     reverse_nominal_do->
				     transaction_date_reverse_nominal_do->
				     transaction_date_close_date_time_string );

			goto set_statement_caption;
		}

		if ( !reverse_nominal->
			reverse_nominal_do->
			close_transaction->
			transaction )
		{
			reverse_nominal->do_no_transaction_message =
				/* ---------------------- */
				/* Returns program memory */
				/* ---------------------- */
				reverse_nominal_do_no_transaction_message();
		}
	}

set_statement_caption:

	reverse_nominal->statement_caption =
		statement_caption_new(
			application_name,
			process_name,
			(char *)0 /* begin_date_string */,
			reverse_date_string /* end_date_string */ );

	return reverse_nominal;
}

REVERSE_NOMINAL *reverse_nominal_calloc( void )
{
	REVERSE_NOMINAL *reverse_nominal;

	if ( ! ( reverse_nominal = calloc( 1, sizeof ( REVERSE_NOMINAL ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return reverse_nominal;
}

char *reverse_nominal_do_transaction_exists_message(
		char *transaction_date_reverse_date_time )
{
	static char message[ 128 ];

	sprintf(message,
		"<h3>ERROR: a reversing entry exists as transaction %s.</h3>",
		transaction_date_reverse_date_time );

	return message;
}

char *reverse_nominal_undo_execute_message( char *transaction_date_time )
{
	static char message[ 128 ];

	if ( !transaction_date_time )
	{
		strcpy( message, "<h3>No reversing entry to delete.</h3>" );
	}
	else
	{
		snprintf(
			message,
			sizeof ( message ),
			"<h3>Transaction delete of %s complete.</h3>",
			transaction_date_time );
	}

	return message;
}

char *reverse_nominal_undo_no_transaction_message( void )
{
	return
	"<h3>Warning: there are no reversing entries to undo.</h3>";
}

char *reverse_nominal_do_no_transaction_message( void )
{
	return
	"<h3>Warning: there are no transactions to reverse.</h3>";
}

char *reverse_nominal_empty_date_message( void )
{
	return
	"<h3>Please enter the reverse date.</h3>";
}

char *reverse_nominal_do_execute_message(
		char *transaction_date_reverse_date_time )
{
	static char message[ 128 ];

	snprintf(
		message,
		sizeof ( message ),
	"<h3>Reversing entry inserted with transaction %s</h3>",
		transaction_date_reverse_date_time );

	return message;
}

