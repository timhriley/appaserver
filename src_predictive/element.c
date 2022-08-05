/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/element.c			*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "list.h"
#include "boolean.h"
#include "sql.h"
#include "String.h"
#include "piece.h"
#include "subclassification.h"
#include "account.h"
#include "element.h"

ELEMENT *element_calloc( void )
{
	ELEMENT *element;

	if ( ! ( element = calloc( 1, sizeof( ELEMENT ) ) ) )
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
	char input[ 128 ];
	ELEMENT *element;
	FILE *pipe;

	if ( !element_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: element_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
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
				element_primary_where(
					element_name ) ) );

	element =
		element_parse(
			string_input(
				input,
				pipe,
				128 ) );

	pclose( pipe );

	return element;
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
			char *transaction_date_time_closing,
			boolean fetch_subclassification_list,
			boolean fetch_account_list,
			boolean fetch_journal_latest,
			boolean fetch_transaction )
{
	ELEMENT *element;

	if ( !input ) return (ELEMENT *)0;

	if ( ! ( element = element_parse( input ) ) )
	{
		return (ELEMENT *)0;
	}

	if ( fetch_subclassification_list )
	{
		element->subclassification_statement_list =
			subclassification_statement_list(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				element_primary_where( element->element_name ),
				transaction_date_time_closing,
				fetch_account_list,
				fetch_journal_latest,
				fetch_transaction );
	}

	return element;
}

ELEMENT *element_seek(	char *element_name,
			LIST *element_list )
{
	ELEMENT *element;

	if ( !list_rewind( element_list ) ) return (ELEMENT *)0;

	do {
		element = list_get( element_list );

		if ( strcmp( element->element_name, element_name ) == 0 )
		{
			return element;
		}

	} while( list_next( element_list ) );

	return (ELEMENT *)0;
}

boolean element_is_nominal( char *element_name )
{
	if ( !element_name )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty element_name.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
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
			LIST *filter_element_name_list,
			char *transaction_date_time_closing,
			boolean fetch_subclassification_list,
			boolean fetch_account_list,
			boolean fetch_journal_latest,
			boolean fetch_transaction )
{
	LIST *element_list;
	char *element_name;

	if ( !list_rewind( filter_element_name_list ) ) return (LIST *)0;

	element_list = list_new();

	do {
		element_name = list_get( filter_element_name_list );

		list_set(
			element_list,
			element_statement_fetch(
				element_name,
				transaction_date_time_closing,
				fetch_subclassification_list,
				fetch_account_list,
				fetch_journal_latest,
				fetch_transaction ) );

	} while ( list_next( filter_element_name_list ) );

	if ( !list_length( element_list ) )
		return (LIST *)0;
	else
		return element_list;
}

double element_sum( ELEMENT *element )
{
	if ( !list_length( element->subclassification_statement_list ) )
	{
		return 0.0;
	}

	return
	subclassification_list_sum(
		/* --------------------------- */
		/* Sets subclassification->sum */
		/* --------------------------- */
		element->subclassification_statement_list );
}

FILE *element_pipe( char *element_system_string )
{
	if ( !element_system_string )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: element_system_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}


	return
	popen( element_system_string, "r" );
}

void element_account_statement_list_set( ELEMENT *element )
{
	if ( !element )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: element is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( list_length( element->subclassification_statement_list ) )
	{
		element->account_statement_list =
			subclassification_account_statement_list(
				element->subclassification_statement_list );

		if ( element->account_statement_list )
		{
			element->account_statement_list =
				account_balance_sort_list(
					element->account_statement_list );
		}
	}
}

ELEMENT *element_statement_fetch(
			char *element_name,
			char *transaction_date_time_closing,
			boolean fetch_subclassification_list,
			boolean fetch_account_list,
			boolean fetch_journal_latest,
			boolean fetch_transaction )
{
	FILE *pipe;
	ELEMENT *element;
	char input[ 128 ];

	if ( !element_name
	||   !transaction_date_time_closing )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	pipe =
		element_pipe(
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
			transaction_date_time_closing,
			fetch_subclassification_list,
			fetch_account_list,
			fetch_journal_latest,
			fetch_transaction );


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
	double sum;

	if ( !list_rewind( element_statement_list ) ) return 0.0;

	sum = 0.0;

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

	if ( !list_rewind( element_statement_list ) ) return;

	do {
		element = list_get( element_statement_list );

		element_account_statement_list_set( element );

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
			char *transaction_date_time_closing )
{
	ELEMENT *element;

	if ( !list_rewind( element_statement_list ) ) return;

	do {
		element = list_get( element_statement_list );

		if ( list_length( element->subclassification_statement_list ) )
		{
			subclassification_account_transaction_count_set(
				element->subclassification_statement_list,
				transaction_begin_date_string,
				transaction_date_time_closing );
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
			char *transaction_date_time_closing )
{
	ELEMENT *element;

	if ( !list_rewind( element_statement_list ) ) return;

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
				transaction_date_time_closing );
		}

	} while ( list_next( element_statement_list ) );
}

void element_percent_of_asset_set(
			LIST *element_statement_list )
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
				element->subclassification_statement_list ) )
			{
				continue;
			}

			element->percent_of_asset =
				float_percent_of_total(
					/* Set by statement_fetch()->	*/
					/* element_list_sum()		*/
					element->sum,
					element_asset->sum );

			subclassification_percent_of_asset_set(
				element->subclassification_statement_list,
				element_asset->sum );
		}

	} while ( list_next( element_statement_list ) );
}

void element_percent_of_revenue_set(
			LIST *element_statement_list )
{
	ELEMENT *element_revenue;
	ELEMENT *element;

	if ( !list_length( element_statement_list ) ) return;

	if ( ! ( element_revenue =
			element_seek(
				ELEMENT_REVENUE,
				element_statement_list ) ) )
	{
		return;
	}

	if ( !element_revenue->sum ) return;

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

			element->percent_of_revenue =
				float_percent_of_total(
					/* Set by statement_fetch()->	*/
					/* element_list_sum()		*/
					element->sum,
					element_revenue->sum );

			subclassification_percent_of_revenue_set(
				element->subclassification_statement_list,
				element_revenue->sum );
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

double element_net_income(
			LIST *element_statement_list )
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
			ELEMENT_EXPENSE,
			element_statement_list );

	loss =
		element_seek_sum(
			ELEMENT_LOSS,
			element_statement_list );

	return (revenue + gain) - (expense + loss);

}

