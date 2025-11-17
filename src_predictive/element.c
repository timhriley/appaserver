/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/element.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "float.h"
#include "sql.h"
#include "piece.h"
#include "appaserver_error.h"
#include "subclassification.h"
#include "account.h"
#include "element.h"

ELEMENT *element_calloc( void )
{
	ELEMENT *element;

	if ( ! ( element = calloc( 1, sizeof ( ELEMENT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return element;
}

ELEMENT *element_new( char *element_name )
{
	ELEMENT *element = element_calloc();

	element->element_name = element_name;

	return element;
}

ELEMENT *element_parse( char *input )
{
	char element_name[ 128 ];
	char piece_buffer[ 16 ];
	ELEMENT *element;

	if ( !input ) return (ELEMENT *)0;

	/* See ELEMENT_SELECT */
	/* ------------------ */
	piece( element_name, SQL_DELIMITER, input, 0 );
	element = element_new( strdup( element_name ) );

	if ( piece( piece_buffer, SQL_DELIMITER, input, 1 ) )
	{
		element->accumulate_debit = ( *piece_buffer == 'y' );
	}

	return element;
}

ELEMENT *element_fetch( char *element_name )
{
	static LIST *list = {0};

	if ( !element_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: element_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list ) list = element_list();

	return element_seek( element_name, list );
}

char *element_primary_where( char *element_name )
{
	static char where[ 128 ];

	if ( !element_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: element_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf( where, "element = '%s'", element_name );

	return where;
}

char *element_system_string(
		char *element_select,
		char *element_table,
		char *where )
{
	char system_string[ 1024 ];

	if ( !element_select
	||   !element_table )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !where ) where = "1 = 1";

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		element_select,
		element_table,
		where );

	return strdup( system_string );
}

ELEMENT *element_statement_parse(
		char *input,
		char *end_date_time_string,
		boolean fetch_subclassification_list,
		boolean fetch_account_list,
		boolean fetch_journal_latest,
		boolean fetch_transaction,
		boolean latest_zero_balance_boolean )
{
	ELEMENT *element;

	if ( !input || !*input ) return (ELEMENT *)0;

	if ( ! ( element = element_parse( input ) ) )
	{
		return (ELEMENT *)0;
	}

	if ( fetch_subclassification_list )
	{
		element->subclassification_statement_list =
			subclassification_where_statement_list(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				element_primary_where( element->element_name ),
				end_date_time_string,
				0 /* not fetch_element */,
				fetch_account_list,
				fetch_journal_latest,
				fetch_transaction,
				latest_zero_balance_boolean );
	}

	return element;
}

ELEMENT *element_seek(
		char *element_name,
		LIST *element_list )
{
	ELEMENT *element;

	if ( list_rewind( element_list ) )
	do {
		element = list_get( element_list );

		if ( strcmp( element->element_name, element_name ) == 0 )
		{
			return element;
		}

	} while( list_next( element_list ) );

	return NULL;
}

boolean element_is_nominal( char *element_name )
{
	if ( !element_name )
	{
		char message[ 128 ];

		sprintf(message, "element_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( strcmp(	element_name,
			ELEMENT_REVENUE ) == 0 )
		return 1;
	else
	if ( strcmp(	element_name,
			ELEMENT_EXPENSE ) == 0 )
		return 1;
	else
	if ( strcmp(	element_name,
			ELEMENT_GAIN ) == 0 )
		return 1;
	else
	if ( strcmp(	element_name,
			ELEMENT_LOSS ) == 0 )
		return 1;
	else
		return 0;
}

LIST *element_statement_list(
		LIST *element_name_list,
		char *end_date_time_string,
		boolean fetch_subclassification_list,
		boolean fetch_account_list,
		boolean fetch_journal_latest,
		boolean fetch_transaction,
		boolean latest_zero_balance_boolean )
{
	LIST *element_list;
	char *element_name;

	element_list = list_new();

	if ( list_rewind( element_name_list ) )
	do {
		element_name = list_get( element_name_list );

		list_set(
			element_list,
			element_statement_fetch(
				element_name,
				end_date_time_string,
				fetch_subclassification_list,
				fetch_account_list,
				fetch_journal_latest,
				fetch_transaction,
				latest_zero_balance_boolean ) );

	} while ( list_next( element_name_list ) );

	if ( !list_length( element_list ) )
	{
		list_free( element_list );
		return (LIST *)0;
	}
	else
	{
		return element_list;
	}
}

void element_list_sum_set( LIST *element_statement_list )
{
	ELEMENT *element;

	if ( list_rewind( element_statement_list ) )
	do {
		element = list_get( element_statement_list );

		element->sum =
			element_sum(
				element );

	} while ( list_next( element_statement_list ) );
}

double element_sum( ELEMENT *element )
{
	if ( !element
	||   !list_length( element->subclassification_statement_list ) )
	{
		return 0.0;
	}

	return
	/* --------------------------- */
	/* Sets subclassification->sum */
	/* --------------------------- */
	subclassification_list_sum_set(
		element->subclassification_statement_list );
}

FILE *element_pipe( char *element_system_string )
{
	if ( !element_system_string )
	{
		char message[ 128 ];

		sprintf(message, "element_system_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	popen( element_system_string, "r" );
}

LIST *element_account_statement_list(
		LIST *subclassification_statement_list,
		LIST *account_statement_list )
{
	LIST *statement_list = {0};

	if ( list_length( subclassification_statement_list ) )
	{
		statement_list =
			subclassification_account_statement_list(
				subclassification_statement_list );
	}
	else
	if ( list_length( account_statement_list ) )
	{
		statement_list =
			account_balance_sort_list(
				account_statement_list );
	}

	return statement_list;
}

ELEMENT *element_statement_fetch(
		char *element_name,
		char *end_date_time_string,
		boolean fetch_subclassification_list,
		boolean fetch_account_list,
		boolean fetch_journal_latest,
		boolean fetch_transaction,
		boolean latest_zero_balance_boolean )
{
	FILE *pipe;
	ELEMENT *element;
	char input[ 128 ];

	if ( !element_name
	||   !end_date_time_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	pipe =
		element_pipe(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			element_system_string(
				ELEMENT_SELECT,
				ELEMENT_TABLE,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				element_primary_where( element_name ) ) );

	element =
		element_statement_parse(
			string_input( input, pipe, 128 ),
			end_date_time_string,
			fetch_subclassification_list,
			fetch_account_list,
			fetch_journal_latest,
			fetch_transaction,
			latest_zero_balance_boolean );


	pclose( pipe );

	return element;
}

double element_list_debit_sum(
		LIST *element_statement_list )
{
	ELEMENT *element;
	double sum;

	if ( !list_rewind( element_statement_list ) ) return 0.0;

	sum = 0.0;

	do {
		element = list_get( element_statement_list );

		if ( list_length( element->subclassification_statement_list ) )
		{
			sum +=
				subclassification_list_debit_sum(
				     element->
					subclassification_statement_list,
				     element->accumulate_debit );
		}

	} while ( list_next( element_statement_list ) );

	return sum;
}

double element_list_credit_sum(
		LIST *element_statement_list )
{
	ELEMENT *element;
	double sum = {0};

	if ( list_rewind( element_statement_list ) )
	do {
		element = list_get( element_statement_list );

		if ( list_length( element->subclassification_statement_list ) )
		{
			sum +=
				subclassification_list_credit_sum(
				    element->
					subclassification_statement_list,
				    element->accumulate_debit );
		}

	} while ( list_next( element_statement_list ) );

	return sum;
}

void element_list_account_statement_list_set(
		LIST *element_statement_list )
{
	ELEMENT *element;

	if ( list_rewind( element_statement_list ) )
	do {
		element = list_get( element_statement_list );

		element->account_statement_list =
			element_account_statement_list(
				element->subclassification_statement_list,
				element->account_statement_list );

	} while ( list_next( element_statement_list ) );
}

void element_delta_prior_percent_set(
		ELEMENT *prior_element /* in/out */,
		ELEMENT *current_element )
{
	if ( !prior_element
	||   !list_length( prior_element->subclassification_statement_list )
	||   !current_element
	||   !list_length( current_element->subclassification_statement_list ) )
	{
		return;
	}

	if ( prior_element->sum )
	{
		prior_element->delta_prior_percent =
			float_delta_prior_percent(
				prior_element->sum,
				current_element->sum );
	}

	subclassification_list_delta_prior_percent_set(
		prior_element->subclassification_statement_list
			/* prior_subclassification_list in/out */,
		current_element->subclassification_statement_list
			/* current_subclassification_list */ );
}

void element_account_transaction_count_set(
		LIST *element_statement_list,
		char *transaction_begin_date_string,
		char *end_date_time_string )
{
	ELEMENT *element;

	if ( list_rewind( element_statement_list ) )
	do {
		element = list_get( element_statement_list );

		if ( list_length( element->subclassification_statement_list ) )
		{
			subclassification_account_transaction_count_set(
				element->subclassification_statement_list,
				transaction_begin_date_string,
				end_date_time_string );
		}

	} while ( list_next( element_statement_list ) );
}

void element_account_action_string_set(
		LIST *element_statement_list,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *transaction_begin_date_string,
		char *end_date_time_string )
{
	ELEMENT *element;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !transaction_begin_date_string
	||   !end_date_time_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( element_statement_list ) )
	do {
		element = list_get( element_statement_list );

		if ( list_length( element->subclassification_statement_list ) )
		{
			subclassification_account_action_string_set(
				element->subclassification_statement_list,
				application_name,
				session_key,
				login_name,
				role_name,
				transaction_begin_date_string,
				end_date_time_string );
		}

	} while ( list_next( element_statement_list ) );
}

void element_percent_of_asset_set( LIST *element_statement_list )
{
	ELEMENT *element_asset;
	ELEMENT *element;

	if ( !list_length( element_statement_list ) ) return;

	if ( ! ( element_asset =
			element_seek(
				ELEMENT_ASSET,
				element_statement_list ) ) )
	{
		return;
	}

	if ( !element_asset->sum ) return;

	list_rewind( element_statement_list );

	do {
		element = list_get( element_statement_list );

		if ( !element_is_nominal( element->element_name ) )
		{
			if ( !list_length(
				element->
					subclassification_statement_list ) )
			{
				continue;
			}

			element->percent_of_asset =
				float_percent_of_total(
					/* Set by statement_fetch()->	*/
					/* element_list_sum_set()	*/
					element->sum,
					element_asset->sum );

			subclassification_percent_of_asset_set(
				element->subclassification_statement_list,
				element_asset->sum );
		}

	} while ( list_next( element_statement_list ) );
}

void element_percent_of_income_set( LIST *element_statement_list )
{
	ELEMENT *element_revenue;
	ELEMENT *element_gain;
	double income;
	ELEMENT *element;

	if ( !list_length( element_statement_list ) ) return;

	element_revenue =
		element_seek(
			ELEMENT_REVENUE,
			element_statement_list );

	element_gain =
		element_seek(
			ELEMENT_GAIN,
			element_statement_list );

	income =
		element_income(
			element_revenue,
			element_gain );

	if ( float_virtually_same( income, 0.0 ) ) return;

	list_rewind( element_statement_list );

	do {
		element = list_get( element_statement_list );

		if ( element_is_nominal( element->element_name ) )
		{
			if ( !list_length(
				element->subclassification_statement_list ) )
			{
				continue;
			}

			element->percent_of_income =
				float_percent_of_total(
					/* Set by statement_fetch()->	*/
					/* element_list_sum_set()	*/
					element->sum,
					income );

			subclassification_percent_of_income_set(
				element->subclassification_statement_list,
				income );
		}

	} while ( list_next( element_statement_list ) );
}

double element_seek_sum(
		char *element_name,
		LIST *element_statement_list )
{
	ELEMENT *element;

	if ( ! ( element =
			element_seek(
				element_name,
				element_statement_list ) ) )
	{
		return 0.0;
	}
	else
	{
		return element->sum;
	}
}

double element_net_income( LIST *element_statement_list )
{
	double revenue;
	double gain;
	double expense;
	double loss;

	revenue =
		element_seek_sum(
			ELEMENT_REVENUE,
			element_statement_list );

	gain =
		element_seek_sum(
			ELEMENT_GAIN,
			element_statement_list );

	expense =
		element_seek_sum(
			(char *)element_resolve_expense(
				ELEMENT_EXPENSE,
				ELEMENT_EXPENDITURE ),
			element_statement_list );

	loss =
		element_seek_sum(
			ELEMENT_LOSS,
			element_statement_list );

	return (revenue + gain) - (expense + loss);

}

LIST *element_list( void )
{
	return
	element_system_list(
		element_system_string(
			ELEMENT_SELECT,
			ELEMENT_TABLE,
			(char *)0 /* where */ ) );
}

LIST *element_system_list( char *element_system_string )
{
	char input[ 32 ];
	FILE *pipe;
	LIST *list = list_new();

	pipe = element_pipe( element_system_string );

	while ( string_input( input, pipe, 32 ) )
	{
		list_set(
			list,
			element_parse( input ) );
	}

	pclose( pipe );

	return list;
}

double element_income(
		ELEMENT *element_revenue,
		ELEMENT *element_gain )
{
	double income = {0};

	if ( element_revenue ) income = element_revenue->sum;
	if ( element_gain ) income += element_gain->sum;

	return income;
}

const char *element_resolve_expense(
		const char *element_expense,
		const char *element_expenditure )
{
	if ( element_fetch(
		(char *)element_expense ) )
	{
		return element_expense;
	}
	else
	{
		return element_expenditure;
	}
}
