/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/close_nominal.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
#include "appaserver_error.h"
#include "account.h"
#include "subclassification.h"
#include "close_nominal.h"

CLOSE_NOMINAL_DO *close_nominal_do_calloc( void )
{
	CLOSE_NOMINAL_DO *close_nominal_do;

	if ( ! ( close_nominal_do = calloc( 1, sizeof ( CLOSE_NOMINAL_DO ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return close_nominal_do;
}

CLOSE_NOMINAL_DO *close_nominal_do_fetch( char *as_of_date_string )
{
	CLOSE_NOMINAL_DO *close_nominal_do;

	if ( !as_of_date_string )
	{
		char message[ 128 ];

		sprintf(message, "as_of_date_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	close_nominal_do = close_nominal_do_calloc();

	close_nominal_do->
		transaction_date_close_nominal_do =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			transaction_date_close_nominal_do_new(
				as_of_date_string );

	if ( close_nominal_do->
		transaction_date_close_nominal_do->
		transaction_date_close_boolean )
	{
		return close_nominal_do;
	}

	close_nominal_do->element_name_list =
		close_nominal_do_element_name_list(
			ELEMENT_REVENUE,
			ELEMENT_EXPENSE,
			ELEMENT_EXPENDITURE,
			ELEMENT_GAIN,
			ELEMENT_LOSS );

	close_nominal_do->element_statement_list =
		element_statement_list(
			close_nominal_do->element_name_list,
			close_nominal_do->
				transaction_date_close_nominal_do->
				transaction_date_close_date_time
				/* end_date_time_string */,
			1 /* fetch_subclassification_list */,
			1 /* fetch_account_list */,
			1 /* fetch_journal_latest */,
			0 /* not fetch_transaction */ );

	close_nominal_do->equity_subclassification_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		close_nominal_do_equity_subclassification_where(
			SUBCLASSIFICATION_DRAWING,
			SUBCLASSIFICATION_APPROPRIATION,
			SUBCLASSIFICATION_ENCUMBRANCE,
			SUBCLASSIFICATION_ESTIMATED_INTERFUND,
			SUBCLASSIFICATION_ESTIMATED_REVENUE,
			SUBCLASSIFICATION_INTERFUND );

	close_nominal_do->equity_subclassification_statement_list =
		subclassification_where_statement_list(
			close_nominal_do->equity_subclassification_where,
			close_nominal_do->
				transaction_date_close_nominal_do->
				transaction_date_close_date_time
					/* end_date_time_string */,
			1 /* fetch_element */,
			1 /* fetch_account_list */,
			1 /* fetch_journal_latest */,
			0 /* not fetch_transaction */ );

	close_nominal_do->entity_self =
		entity_self_fetch(
			0 /* not fetch_entity_boolean */ );

	if ( !close_nominal_do->entity_self )
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

	close_nominal_do->close_transaction =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		close_transaction_new(
			close_nominal_do->
				transaction_date_close_nominal_do->
				transaction_date_close_date_time,
			close_nominal_do->element_statement_list,
			close_nominal_do->
				equity_subclassification_statement_list,
			close_nominal_do->entity_self->full_name,
			close_nominal_do->entity_self->street_address );

	if ( close_nominal_do->close_transaction->transaction )
	{
		close_nominal_do->journal_debit_sum =
			journal_debit_sum(
				close_nominal_do->
					close_transaction->
					transaction->
					journal_list );

		close_nominal_do->journal_credit_sum =
			journal_credit_sum(
				close_nominal_do->
					close_transaction->
					transaction->
					journal_list );
	}

	return close_nominal_do;
}

LIST *close_nominal_do_element_name_list(
		const char *element_revenue,
		const char *element_expense,
		const char *element_expenditure,
		const char *element_gain,
		const char *element_loss )
{
	LIST *list = list_new();

	list_set( list, (char *)element_revenue );
	list_set( list, (char *)element_expense );
	list_set( list, (char *)element_expenditure );
	list_set( list, (char *)element_gain );
	list_set( list, (char *)element_loss );

	return list;
}

CLOSE_NOMINAL *close_nominal_fetch(
		char *application_name,
		char *process_name,
		char *as_of_date_string,
		boolean undo )
{
	CLOSE_NOMINAL *close_nominal;

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

	close_nominal = close_nominal_calloc();

	if ( undo )
	{
		close_nominal->close_nominal_undo =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			close_nominal_undo_fetch(
				TRANSACTION_TABLE,
				TRANSACTION_CLOSE_MEMO );

		if ( !close_nominal->
			close_nominal_undo->
			transaction_date_close_nominal_undo->
			transaction_date_time_memo_maximum_string )
		{
			close_nominal->undo_no_transaction_message =
				/* ---------------------- */
				/* Returns program memory */
				/* ---------------------- */
				close_nominal_undo_no_transaction_message();

			as_of_date_string = (char *)0;

			goto set_statement_caption;
		}

		as_of_date_string =
		    close_nominal->
			close_nominal_undo->
			transaction_date_close_nominal_undo->
			transaction_date_time_memo_maximum_string;
	}
	else
	{
		if ( !transaction_date_as_of_date_populated(
			as_of_date_string ) )
		{
			close_nominal->empty_date_message =
				/* ---------------------- */
				/* Returns program memory */
				/* ---------------------- */
				close_nominal_empty_date_message();

			as_of_date_string = (char *)0;

			goto set_statement_caption;
		}

		close_nominal->close_nominal_do =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			close_nominal_do_fetch(
				as_of_date_string );

		if ( close_nominal->
			close_nominal_do->
			transaction_date_close_nominal_do->
			transaction_date_close_boolean )
		{
			close_nominal->do_transaction_exists_message =
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				close_nominal_do_transaction_exists_message(
				   close_nominal->
					close_nominal_do->
					transaction_date_close_nominal_do->
					transaction_date_close_date_time );

			goto set_statement_caption;
		}

		if ( !close_nominal->
			close_nominal_do->
			close_transaction->
			transaction )
		{
			close_nominal->do_no_transaction_message =
				/* ---------------------- */
				/* Returns program memory */
				/* ---------------------- */
				close_nominal_do_no_transaction_message();
		}
	}

set_statement_caption:

	close_nominal->statement_caption =
		statement_caption_new(
			application_name,
			process_name,
			(char *)0 /* begin_date_string */,
			as_of_date_string /* end_date_string */ );

	return close_nominal;
}

CLOSE_NOMINAL *close_nominal_calloc( void )
{
	CLOSE_NOMINAL *close_nominal;

	if ( ! ( close_nominal = calloc( 1, sizeof ( CLOSE_NOMINAL ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return close_nominal;
}

char *close_nominal_do_transaction_exists_message(
		char *transaction_date_close_date_time )
{
	static char message[ 128 ];

	sprintf(message,
		"<h3>ERROR: a closing entry exists as transaction %s.</h3>",
		transaction_date_close_date_time );

	return message;
}

CLOSE_NOMINAL_UNDO *close_nominal_undo_fetch(
		const char *transaction_table,
		const char *transaction_close_memo )
{
	CLOSE_NOMINAL_UNDO *close_nominal_undo =
		close_nominal_undo_calloc();

	close_nominal_undo->
		transaction_date_close_nominal_undo =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			transaction_date_close_nominal_undo_new(
				transaction_table,
				transaction_close_memo );

	if ( !close_nominal_undo->
		transaction_date_close_nominal_undo->
		transaction_date_time_memo_maximum_string )
	{
		return close_nominal_undo;
	}

	if ( ! ( close_nominal_undo->transaction =
		    transaction_fetch(
			(char *)0 /* full_name */,
			(char *)0 /* street_address */,
			close_nominal_undo->
				transaction_date_close_nominal_undo->
				transaction_date_time_memo_maximum_string,
			1 /* fetch_journal_list */ ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"transaction_fetch(%s) returned empty.",
			close_nominal_undo->
				transaction_date_close_nominal_undo->
				transaction_date_time_memo_maximum_string );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return close_nominal_undo;
}

CLOSE_NOMINAL_UNDO *close_nominal_undo_calloc( void )
{
	CLOSE_NOMINAL_UNDO *close_nominal_undo;

	if ( ! ( close_nominal_undo =
			calloc( 1, sizeof ( CLOSE_NOMINAL_UNDO ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return close_nominal_undo;
}

void close_nominal_undo_display( TRANSACTION *transaction )
{
	if ( !transaction )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: transaction is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	transaction_html_display( transaction );
}

void close_nominal_undo_execute( TRANSACTION *transaction )
{
	if ( !transaction )
	{
		char message[ 128 ];

		sprintf(message, "transaction is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	transaction_delete(
		transaction->full_name,
		transaction->street_address,
		transaction->transaction_date_time );
}

char *close_nominal_undo_execute_message( char *transaction_date_time )
{
	static char message[ 128 ];

	if ( !transaction_date_time )
	{
		strcpy( message, "<h3>No closing entry to delete.</h3>" );
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

char *close_nominal_undo_no_transaction_message( void )
{
	return
	"<h3>Warning: there are no closing entries to undo.</h3>";
}

char *close_nominal_do_no_transaction_message( void )
{
	return
	"<h3>Warning: there are no transactions to close.</h3>";
}

char *close_nominal_empty_date_message( void )
{
	return
	"<h3>Please enter the as of date to close.</h3>";
}

char *close_nominal_do_execute_message(
		char *transaction_date_close_date_time )
{
	static char message[ 128 ];

	sprintf(message,
	"<h3>Closing entry inserted with transaction %s</h3>",
		transaction_date_close_date_time );

	return message;
}

char *close_nominal_do_no_execute_message( void )
{
	return
	"<h3>Closing entry display complete.</h3>";
}

char *close_nominal_undo_no_execute_message( void )
{
	return
	"<h3>Will delete this transaction:</h3>";
}

boolean close_nominal_do_no_transactions_boolean(
		double transaction_amount )
{
	return
	float_virtually_same( transaction_amount, 0.0 );
}

char *close_nominal_do_equity_subclassification_where(
		const char *subclassification_drawing,
		const char *subclassification_appropriation,
		const char *subclassification_encumbrance,
		const char *subclassification_estimated_interfund,
		const char *subclassification_estimated_revenue,
		const char *subclassification_interfund )
{
	static char where[ 256 ];

	snprintf(
		where,
		sizeof ( where ),
		"subclassification in "
		"('%s','%s','%s','%s','%s','%s')",
		subclassification_drawing,
		subclassification_appropriation,
		subclassification_encumbrance,
		subclassification_estimated_interfund,
		subclassification_estimated_revenue,
		subclassification_interfund );

	return where;
}

