/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/subclassification.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "float.h"
#include "String.h"
#include "list.h"
#include "sql.h"
#include "piece.h"
#include "boolean.h"
#include "account.h"
#include "appaserver_error.h"
#include "subclassification.h"

LIST *subclassification_statement_list(
		char *element_primary_where,
		char *end_date_time_string,
		boolean fetch_account_list,
		boolean fetch_journal_latest,
		boolean fetch_transaction )
{
	FILE *pipe;
	char input[ 256 ];
	LIST *list;

	if ( !element_primary_where
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

	list = list_new();

	pipe =
		subclassification_pipe(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			subclassification_system_string(
				SUBCLASSIFICATION_SELECT,
				SUBCLASSIFICATION_TABLE,
				element_primary_where ) );

	while ( string_input( input, pipe, 256 ) )
	{
		list_set(
			list,
			subclassification_statement_parse(
				input,
				end_date_time_string,
				fetch_account_list,
				fetch_journal_latest,
				fetch_transaction ) );
	}

	pclose( pipe );

	return list;
}

SUBCLASSIFICATION *subclassification_statement_parse(
		char *input,
		char *end_date_time_string,
		boolean fetch_account_list,
		boolean fetch_journal_latest,
		boolean fetch_transaction )
{
	SUBCLASSIFICATION *subclassification;

	if ( !end_date_time_string )
	{
		char message[ 128 ];

		sprintf(message, "end_date_time_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !input ) return NULL;

	if ( ! ( subclassification =
			subclassification_parse(
				input,
				0 /* not fetch_element */ ) ) )
	{
		return NULL;
	}

	if ( fetch_account_list )
	{
		subclassification->account_statement_list =
			account_statement_list(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				subclassification_primary_where(
					subclassification->
						subclassification_name ),
				end_date_time_string,
				0 /* not fetch_subclassification */,
				0 /* not fetch_element */,
				fetch_journal_latest,
				fetch_transaction );

		subclassification->account_statement_list =
			account_balance_sort_list(
				subclassification->account_statement_list );
	}

	return subclassification;
}

SUBCLASSIFICATION *subclassification_parse(
		char *input,
		boolean fetch_element )
{
	char subclassification_name[ 128 ];
	char piece_buffer[ 128 ];
	SUBCLASSIFICATION *subclassification;

	if ( !input || !*input ) return (SUBCLASSIFICATION *)0;

	/* See SUBCLASSIFICATION_SELECT */
	/* ---------------------------- */
	piece( subclassification_name, SQL_DELIMITER, input, 0 );

	subclassification =
		subclassification_new(
			strdup( subclassification_name ) );

	piece( piece_buffer, SQL_DELIMITER, input, 1 );
	if ( *piece_buffer )
	{
		subclassification->element_name = strdup( piece_buffer );
	}

	piece( piece_buffer, SQL_DELIMITER, input, 2 );
	if ( *piece_buffer )
	{
		subclassification->display_order = atoi( piece_buffer );
	}

	if ( fetch_element && subclassification->element_name )
	{
		subclassification->element =
			element_fetch(
				subclassification->element_name );
	}

	return subclassification;
}

SUBCLASSIFICATION *subclassification_fetch(
		char *subclassification_name,
		boolean fetch_element )
{
	SUBCLASSIFICATION *subclassification;
	static LIST *list = {0};

	if ( !subclassification_name )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: subclassification_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( fetch_element )
	{
		if ( !list )
		{
			list =
				subclassification_list(
					"1 = 1" /* where */,
					1 /* fetch_element */ );
		}

		subclassification =
			subclassification_seek(
				subclassification_name,
				list );
	}
	else
	{
		FILE *pipe;
		char input[ 256 ];

		pipe =
			subclassification_pipe(
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				subclassification_system_string(
					SUBCLASSIFICATION_SELECT,
					SUBCLASSIFICATION_TABLE,
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					subclassification_primary_where(
						subclassification_name ) ) );

		subclassification =
			subclassification_parse(
				string_input( input, pipe, 256 ),
				0 /* not fetch_element */ );

		pclose( pipe );
	}

	return subclassification;
}

char *subclassification_primary_where(
		char *subclassification_name )
{
	static char where[ 128 ];

	if ( !subclassification_name )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: subclassification_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf( where, "subclassification = '%s'", subclassification_name );

	return where;
}

char *subclassification_system_string(
		char *subclassification_select,
		char *subclassification_table,
		char *where )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\" display_order",
		subclassification_select,
		subclassification_table,
		where );

	return strdup( system_string );
}

SUBCLASSIFICATION *subclassification_calloc( void )
{
	SUBCLASSIFICATION *subclassification;

	if ( ! ( subclassification =
			calloc( 1, sizeof ( SUBCLASSIFICATION ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return subclassification;
}

SUBCLASSIFICATION *subclassification_new(
		char *subclassification_name )
{
	SUBCLASSIFICATION *subclassification = subclassification_calloc();

	subclassification->subclassification_name = subclassification_name;

	return subclassification;
}

LIST *subclassification_element_subclassification_name_list(
		char *element_primary_where,
		char *subclassification_table,
		char *order_column )
{
	char system_string[ 1024 ];

	if ( !element_primary_where
	||   !subclassification_table )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"select.sh subclassification %s \"%s\" %s",
		subclassification_table,
		element_primary_where,
		order_column );

	return pipe2list( system_string );
}

LIST *subclassification_account_statement_list(
		LIST *subclassification_statement_list )
{
	LIST *account_statement_list = list_new();
	SUBCLASSIFICATION *subclassification;
	ACCOUNT *account;

	if ( list_rewind( subclassification_statement_list ) )
	do {
		subclassification =
			list_get(
				subclassification_statement_list );

		if ( list_rewind( subclassification->account_statement_list ) )
		do {
			account =
				list_get(
					subclassification->
						account_statement_list );

			list_set_order(
				account_statement_list /* in/out */,
				account,
				account_balance_match_function );

		} while ( list_next(
				subclassification->
					account_statement_list ) );

	} while ( list_next( subclassification_statement_list ) );

	if ( !list_length( account_statement_list ) )
	{
		list_free( account_statement_list );
		account_statement_list = NULL;
	}

	return account_statement_list;
}

double subclassification_list_sum_set(
		LIST *subclassification_statement_list )
{
	SUBCLASSIFICATION *subclassification;
	double list_sum;

	if ( !list_rewind( subclassification_statement_list ) )
	{
		return 0.0;
	}

	list_sum = 0.0;

	do {
		subclassification =
			list_get(
				subclassification_statement_list );

		if ( list_length( subclassification->account_statement_list ) )
		{
			subclassification->sum =
				account_list_sum(
					subclassification->
						account_statement_list );

			list_sum += subclassification->sum;
		}

	} while ( list_next( subclassification_statement_list ) );

	return list_sum;
}

FILE *subclassification_pipe( char *system_string )
{
	return
	popen( system_string, "r" );
}

void subclassification_list_delta_prior_percent_set(
		LIST *prior_subclassification_list /* in/out */,
		LIST *current_subclassification_list )
{
	SUBCLASSIFICATION *prior_subclassification;
	SUBCLASSIFICATION *current_subclassification;

	if ( !list_rewind( current_subclassification_list ) ) return;

	do {
		current_subclassification =
			list_get(
				current_subclassification_list );

		if ( !list_length(
			current_subclassification->
				account_statement_list ) )
		{
			continue;
		}

		if ( ! ( prior_subclassification =
				subclassification_seek(
					current_subclassification->
						subclassification_name,
				prior_subclassification_list
					/* subclassification_list */ ) ) )
		{
			continue;
		}

		subclassification_delta_prior_percent_set(
			prior_subclassification /* in/out */,
			current_subclassification );

	} while ( list_next( current_subclassification_list ) );
}

SUBCLASSIFICATION *subclassification_seek(
		char *subclassification_name,
		LIST *subclassification_list )
{
	SUBCLASSIFICATION *subclassification;

	if ( !subclassification_name )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: subclassification_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( subclassification_list ) )
		return (SUBCLASSIFICATION *)0;

	do {
		subclassification =
			list_get(
				subclassification_list );

		if ( strcmp(
			subclassification->subclassification_name,
			subclassification_name ) == 0 )
		{
			return subclassification;
		}

	} while ( list_next( subclassification_list ) );

	return (SUBCLASSIFICATION *)0;
}

void subclassification_delta_prior_percent_set(
		SUBCLASSIFICATION *prior_subclassification /* in/out */,
		SUBCLASSIFICATION *current_subclassification )
{
	if ( !prior_subclassification
	||   !current_subclassification )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( prior_subclassification->sum )
	{
		prior_subclassification->delta_prior_percent =
			float_delta_prior_percent(
				prior_subclassification->sum,
				current_subclassification->sum );

		account_list_delta_prior_percent_set(
			prior_subclassification->account_statement_list
				/* prior_account_list in/out */,
			current_subclassification->account_statement_list
				/* current_account_list */ );
	}
}

double subclassification_list_debit_sum(
		LIST *subclassification_statement_list,
		boolean element_accumulate_debit )
{
	SUBCLASSIFICATION *subclassification;
	double sum;

	if ( !list_rewind( subclassification_statement_list ) ) return 0.0;

	sum = 0.0;

	do {
		subclassification =
			list_get(
				subclassification_statement_list );

		if ( list_length( subclassification->account_statement_list ) )
		{
			sum +=
				account_list_debit_sum(
					subclassification->
						account_statement_list
							/* account_list */,
					element_accumulate_debit );
		}

	} while ( list_next( subclassification_statement_list ) );

	return sum;
}

double subclassification_list_credit_sum(
		LIST *subclassification_statement_list,
		boolean element_accumulate_debit )
{
	SUBCLASSIFICATION *subclassification;
	double sum;

	if ( !list_rewind( subclassification_statement_list ) ) return 0.0;

	sum = 0.0;

	do {
		subclassification =
			list_get(
				subclassification_statement_list );

		if ( list_length( subclassification->account_statement_list ) )
		{
			sum +=
				account_list_credit_sum(
					subclassification->
						account_statement_list
							/* account_list */,
						element_accumulate_debit );
		}

	} while ( list_next( subclassification_statement_list ) );

	return sum;
}

void subclassification_account_transaction_count_set(
		LIST *subclassification_statement_list,
		char *transaction_begin_date_string,
		char *end_date_time_string )
{
	SUBCLASSIFICATION *subclassification;

	if ( list_rewind( subclassification_statement_list ) )
	do {
		subclassification =
			list_get(
				subclassification_statement_list );

		if ( list_length( subclassification->account_statement_list ) )
		{
			account_transaction_count_set(
				subclassification->account_statement_list,
				transaction_begin_date_string,
				end_date_time_string );
		}

	} while ( list_next( subclassification_statement_list ) );
}

void subclassification_account_action_string_set(
		LIST *subclassification_statement_list,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *transaction_begin_date_string,
		char *end_date_time_string )
{
	SUBCLASSIFICATION *subclassification;

	if ( list_rewind( subclassification_statement_list ) )
	do {
		subclassification =
			list_get(
				subclassification_statement_list );

		if ( list_length( subclassification->account_statement_list ) )
		{
			account_list_action_string_set(
				subclassification->account_statement_list,
				application_name,
				session_key,
				login_name,
				role_name,
				transaction_begin_date_string,
				end_date_time_string );
		}

	} while ( list_next( subclassification_statement_list ) );
}

void subclassification_percent_of_asset_set(
		LIST *subclassification_statement_list,
		double element_asset_sum )
{
	SUBCLASSIFICATION *subclassification;

	if ( !element_asset_sum ) return;

	if ( list_rewind( subclassification_statement_list ) )
	do {
		subclassification =
			list_get(
				subclassification_statement_list );

		if ( !list_length( subclassification->account_statement_list ) )
		{
			continue;
		}

		subclassification->percent_of_asset =
			float_percent_of_total(
				/* Set by statement_fetch()->	*/
				/* element_list_sum()		*/
				subclassification->sum,
				element_asset_sum );

		account_percent_of_asset_set(
			subclassification->account_statement_list,
			element_asset_sum );

	} while ( list_next( subclassification_statement_list ) );
}

void subclassification_percent_of_income_set(
		LIST *subclassification_statement_list,
		double element_income )
{
	SUBCLASSIFICATION *subclassification;

	if ( float_virtually_same( element_income, 0.0 ) ) return;

	if ( list_rewind( subclassification_statement_list ) )
	do {
		subclassification =
			list_get(
				subclassification_statement_list );

		if ( !list_length( subclassification->account_statement_list ) )
		{
			continue;
		}

		subclassification->percent_of_income =
			float_percent_of_total(
				/* Set by statement_fetch()->	*/
				/* element_list_sum()		*/
				subclassification->sum,
				element_income );

		account_percent_of_income_set(
			subclassification->account_statement_list,
			element_income );

	} while ( list_next( subclassification_statement_list ) );
}

SUBCLASSIFICATION *subclassification_element_list_seek(
		char *subclassification_name,
		LIST *element_statement_list )
{
	ELEMENT *element;
	SUBCLASSIFICATION *subclassification;

	if ( list_rewind( element_statement_list ) )
	do {
		element = list_get( element_statement_list );

		if ( !list_length(
			element->subclassification_statement_list ) )
		{
			continue;
		}

		if ( ( subclassification =
			subclassification_seek(
				subclassification_name,
				element->subclassification_statement_list ) ) )
		{
			return subclassification;
		}

	} while ( list_next( element_statement_list ) );

	return (SUBCLASSIFICATION *)0;
}

LIST *subclassification_list(
		char *where,
		boolean fetch_element )
{
	if ( !where ) where = "1 = 1";

	return
	subclassification_system_list(
		subclassification_system_string(
			SUBCLASSIFICATION_SELECT,
			SUBCLASSIFICATION_TABLE,
			where ),
		fetch_element );
}

LIST *subclassification_system_list(
		char *subclassification_system_string,
		boolean fetch_element )
{
	LIST *list = list_new();
	FILE *pipe;
	char input[ 256 ];

	pipe =
		subclassification_pipe(
			subclassification_system_string );

	while ( string_input( input, pipe, 256 ) )
	{
		list_set(
			list,
			subclassification_parse(
				input,
				fetch_element ) );
	}

	pclose( pipe );

	return list;
}

boolean subclassification_current_liability_boolean(
		char *subclassification_current_liability,
		char *subclassification_name )
{
	return ( string_strcmp(
			subclassification_current_liability,
			subclassification_name ) == 0 );
}

boolean subclassification_receivable_boolean(
		char *subclassification_receivable,
		char *subclassification_name )
{
	return ( string_strcmp(
			subclassification_receivable,
			subclassification_name ) == 0 );
}

