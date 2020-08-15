/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/element.c			*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
#include "list.h"
#include "sql.h"
#include "piece.h"
#include "environ.h"
#include "boolean.h"
#include "account.h"
#include "transaction.h"
#include "subclassification.h"
#include "element.h"

char *element_select( void )
{
	return "element,accumulate_debit_yn";
}

ELEMENT *element_parse(	char *input )
{
	char element_name[ 128 ];
	char piece_buffer[ 16 ];
	ELEMENT *element;

	if ( !input ) return (ELEMENT *)0;

	piece( element_name, SQL_DELIMITER, input, 0 );
	element = element_new( strdup( element_name ) );

	piece( piece_buffer, SQL_DELIMITER, input, 1 );
	element->accumulate_debit = ( *piece_buffer == 'y' );

	return element;
}

ELEMENT *element_fetch( char *element_name )
{
	char sys_string[ 1024 ];
	char where[ 256 ];

	sprintf(where,
		"element = '%s'	",
		element_name );

	sprintf(sys_string,
		"echo \"select %s from %s where %s;\" | sql.e",
		/* -------------------------- */
		/* Safely returns heap memory */
		/* -------------------------- */
		element_select(),
		"element",
		where );

	return element_parse( pipe2string( sys_string ) );
}

ELEMENT *element_account_name_fetch(
			char *account_name )
{
	char sys_string[ 1024 ];
	char *from;
	char join[ 256 ];
	char where[ 256 ];

	from = "account,subclassification,element";

	sprintf(join,
	"account.subclassification = subclassification.subclassification and "
	"subclassification.element = element.element " );

	sprintf(where,
		"account = '%s' and			"
		"%s					",
		account_escape_name( account_name ),
		join );

	sprintf(sys_string,
		"echo \"select %s from %s where %s;\" | sql.e",
		/* -------------------------- */
		/* Safely returns heap memory */
		/* -------------------------- */
		element_select(),
		from,
		where );

	return element_parse( pipe2string( sys_string ) );
}

ELEMENT *element_new( char *element_name )
{
	ELEMENT *element;

	if ( ! ( element = calloc( 1, sizeof( ELEMENT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	element->element_name = element_name;

	element->accumulate_debit =
		element_accumulate_debit(
			element->element_name );

	return element;
}

boolean element_accumulate_debit(
			char *element_name )
{
	ELEMENT *element;

	if ( ! ( element =
			element_fetch(
				element_name ) ) )
	{
		fprintf( stderr,
	"Warning in %s/%s()/%d: cannot fetch element for (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 element_name );
		return 0;
	}

	return element->accumulate_debit;
}

LIST *element_list(	LIST *filter_element_name_list,
			char *fund_name,
			char *as_of_date,
			boolean omit_subclassification )
{
	LIST *element_list;
	ELEMENT *element;
	char sys_string[ 1024 ];
	char input_buffer[ 256 ];
	char element_name[ 128 ];
	char accumulate_debit_yn[ 2 ];
	FILE *input_pipe;

	sprintf( sys_string,
		 "echo \"select %s from %s;\" | sql",
		 element_select(),
		 "element" );

	element_list = list_new();
	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( element_name, SQL_DELIMITER, input_buffer, 0 );

		if ( list_length( filter_element_name_list )
		&&   !list_exists_string(
			filter_element_name_list,
			element_name ) )
		{
			continue;
		}

		element =
			element_new(
				strdup( element_name ) );

		piece(	accumulate_debit_yn,
			SQL_DELIMITER,
			input_buffer,
			1 );

		element->accumulate_debit = ( *accumulate_debit_yn == 'y' );

		if ( omit_subclassification )
		{
			element->account_list =
				element_account_list(
					&element->element_total,
					element->element_name,
					fund_name,
					as_of_date );
		}
		else
		{
			element->subclassification_list =
				element_subclassification_list(
					&element->element_total,
					element->element_name,
					fund_name,
					as_of_date );
		}

		list_append_pointer(	element_list,
					element );
	}

	pclose( input_pipe );

	return element_list_sort( element_list );
}

LIST *element_subclassification_list(
			double *element_total,
			char *element_name,
			char *fund_name,
			char *as_of_date )
{
	LIST *subclassification_list;
	SUBCLASSIFICATION *subclassification;
	char sys_string[ 1024 ];
	char where[ 256 ];
	char subclassification_name[ 128 ];
	FILE *input_pipe;

	*element_total = 0.0;

	sprintf( where, "element = '%s'", element_name );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s order by %s;\" | sql",
		 "subclassification",
		 "subclassification",
		 where,
		 "display_order" );

	subclassification_list = list_new();
	input_pipe = popen( sys_string, "r" );

	while( get_line( subclassification_name, input_pipe ) )
	{
		subclassification =
			subclassification_new(
				strdup( subclassification_name ) );

		subclassification->account_list =
			subclassification_account_list(
				&subclassification->subclassification_total,
				subclassification->subclassification_name,
				fund_name,
				as_of_date );

		*element_total += subclassification->subclassification_total;

		list_set(	subclassification_list,
				subclassification );
	}

	pclose( input_pipe );
	return subclassification_list;
}

LIST *element_list_sort( LIST *element_list )
{
	ELEMENT *element;
	LIST *return_element_list;

	return_element_list = list_new();

	if ( ( element = element_seek(
				element_list,
				ELEMENT_ASSET ) ) )
	{
		list_set( return_element_list, element );
	}

	if ( ( element = element_seek(
				element_list,
				ELEMENT_LIABILITY ) ) )
	{
		list_set( return_element_list, element );
	}

	if ( ( element = element_seek(
				element_list,
				ELEMENT_REVENUE ) ) )
	{
		list_set( return_element_list, element );
	}

	if ( ( element = element_seek(
				element_list,
				ELEMENT_EXPENSE ) ) )
	{
		list_set( return_element_list, element );
	}

	if ( ( element = element_seek(
				element_list,
				ELEMENT_GAIN ) ) )
	{
		list_set( return_element_list, element );
	}

	if ( ( element = element_seek(
				element_list,
				ELEMENT_LOSS ) ) )
	{
		list_set( return_element_list, element );
	}

	if ( ( element = element_seek(
				element_list,
				ELEMENT_EQUITY ) ) )
	{
		list_set( return_element_list, element );
	}
	return return_element_list;
}

ELEMENT *element_list_seek(
			LIST *element_list,
			char *element_name )
{
	return element_seek( element_list, element_name );
}

ELEMENT *element_seek(	LIST *element_list,
			char *element_name )
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

LIST *element_account_list(
			double *element_total,
			char *element_name,
			char *fund_name,
			char *as_of_date )
{
	ACCOUNT *account;
	char sys_string[ 1024 ];
	char where[ 256 ];
	char account_name[ 128 ];
	FILE *input_pipe;
	char *folder;
	LIST *account_list;
	char *fund_where;
	JOURNAL *latest_journal;

	fund_where =
		transaction_fund_where(
			fund_name );

	*element_total = 0.0;

	folder = "account,subclassification";

	sprintf(	where,
			"%s and					"
			"element = '%s' and			"
			"account.subclassification =		"
			"subclassification.subclassification	",
			fund_where,
			element_name );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql",
		 "account",
		 folder,
		 where );

	account_list = list_new();
	input_pipe = popen( sys_string, "r" );

	while( get_line( account_name, input_pipe ) )
	{
		latest_journal =
			journal_latest(
				account_name,
				as_of_date );

		if ( !latest_journal
		||   timlib_double_virtually_same(
			latest_journal->balance,
			0.0 ) )
		{
			continue;
		}

		account = account_fetch( strdup( account_name ) );

		/* Change account name from stack memory to heap. */
		/* ---------------------------------------------- */
		latest_journal->account_name = account->account_name;

		account->latest_journal = latest_journal;

		*element_total += account->latest_journal->balance;

		list_add_pointer_in_order(
			account_list,
			account,
			account_balance_match_function );
	}

	pclose( input_pipe );
	return account_list;
}

boolean element_is_period( char *element_name )
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

ACCOUNT *element_account_seek(
			LIST *element_list,
			char *account_name )
{
	ELEMENT *element;
	SUBCLASSIFICATION *subclassification;
	ACCOUNT *account;

	if ( !list_rewind( element_list ) ) return (ACCOUNT *)0;

	do {
		element = list_get( element_list );

		if ( list_length( element->account_list ) )
		{
			return account_seek(
					element->account_list,
					account_name );
		}

		if ( !list_rewind( element->subclassification_list ) )
			continue;

		do {
			subclassification =
				list_get(
					element->
						subclassification_list );

			if ( !list_rewind( subclassification->account_list ) )
				continue;


			if ( ( account =
				account_seek(
					subclassification->account_list,
					account_name ) ) )
			{
				return account;
			}

		} while( list_next( element->subclassification_list ) );
	} while( list_next( element_list ) );
	return (ACCOUNT *)0;
}

boolean element_account_accumulate_debit(
			char *account_name )
{
	ELEMENT *element;

	if ( ! ( element =
			element_account_name_fetch(
				account_name ) ) )
	{
		fprintf( stderr,
	"Warning in %s/%s()/%d: cannot fetch element for account = (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 account_name );
		return 0;
	}

	return element->accumulate_debit;
}

double element_value(	LIST *subclassification_list,
			boolean element_accumulate_debit )
{
	double total_element = 0.0;
	ACCOUNT *account;
	SUBCLASSIFICATION *subclassification;
	double latest_journal_balance;

	if ( !list_rewind( subclassification_list ) ) return 0.0;
	
	do {
		subclassification = list_get( subclassification_list );

		if ( !list_rewind( subclassification->account_list ) )
			continue;

		do {
			account =
				list_get(
					subclassification->account_list );

			if ( !account->latest_journal
			||   !account->latest_journal->balance )
				continue;

			if (	element_accumulate_debit ==
				account->accumulate_debit )
			{
				latest_journal_balance =
					account->latest_journal->balance;
			}
			else
			{
				latest_journal_balance =
					0.0 - account->latest_journal->balance;
			}

			total_element += latest_journal_balance;

		} while( list_next( subclassification->account_list ) );

	} while( list_next( subclassification_list ) );

	return total_element;
}

