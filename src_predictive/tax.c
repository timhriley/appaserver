/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/tax.c				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include "timlib.h"
#include "appaserver_library.h"
#include "piece.h"
#include "folder.h"
#include "date.h"
#include "account.h"
#include "element.h"
#include "subclassification.h"
#include "transaction.h"
#include "journal.h"
#include "sql.h"
#include "String.h"
#include "tax.h"

TAX *tax_calloc( void )
{
	TAX *tax;

	if ( ! ( tax = calloc( 1, sizeof( TAX ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return tax;
}

TAX *tax_new(		char *tax_form_string,
			int tax_year )
{
	TAX *tax = tax_calloc();

	tax->tax_form_string = tax_form_string;
	tax->tax_year = tax_year;
	return tax;
}

TAX_FORM_LINE_ACCOUNT *tax_form_line_account_new(
			char *tax_form_string,
			char *tax_form_line,
			char *account_name )
{
	TAX_FORM_LINE_ACCOUNT *tax_form_line_account;

	if ( ! ( tax_form_line_account =
			calloc( 1, sizeof( TAX_FORM_LINE_ACCOUNT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	tax_form_line_account->tax_form_string = tax_form_string;
	tax_form_line_account->tax_form_line = tax_form_line;
	tax_form_line_account->account_name = account_name;

	return tax_form_line_account;
}

TAX_FORM *tax_form_new(	char *tax_form_string,
			int tax_year )
{
	TAX_FORM *tax_form;

	if ( ! ( tax_form = calloc( 1, sizeof( TAX_FORM ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	tax_form->tax_form_string = tax_form_string;
	tax_form->tax_year = tax_year;

	return tax_form;
}

TAX_FORM_LINE *tax_form_line_new(
			char *tax_form_string,
			char *tax_form_line_string )
{
	TAX_FORM_LINE *tax_form_line;

	if ( ! ( tax_form_line = calloc( 1, sizeof( TAX_FORM_LINE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	tax_form_line->tax_form_string = tax_form_string;
	tax_form_line->tax_form_line_string = tax_form_line_string;

	return tax_form_line;
}

char *tax_form_primary_where( char *tax_form_string )
{
	static char where[ 256 ];

	sprintf(where,
		"tax_form = '%s'",
		tax_form_string );

	return where;
}

char *tax_form_line_system_string( char *where )
{
	char system_string[ 1024 ];
	char *order;

	order = "tax_form_line";

	sprintf(system_string,
		"select.sh '*' tax_form_line \"%s\" %s",
		where,
		order );

	return strdup( system_string );
}

TAX_FORM_LINE *tax_form_line_parse(
			char *input,
			int tax_year,
			boolean fetch_account_list,
			boolean fetch_journal_list )
{
	char tax_form_string[ 128 ];
	char tax_form_line_string[ 128 ];
	char piece_buffer[ 128 ];
	TAX_FORM_LINE *tax_form_line;

	/* See attribute_list tax_form_line */
	/* -------------------------------- */
	piece( tax_form_string, SQL_DELIMITER, input, 0 );
	piece( tax_form_line_string, SQL_DELIMITER, input, 1 );

	tax_form_line =
		tax_form_line_new(
			strdup( tax_form_string ),
			strdup( tax_form_line_string ) );

	piece( piece_buffer, SQL_DELIMITER, input, 2 );
	tax_form_line->tax_form_description = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	tax_form_line->itemize_accounts = (*piece_buffer == 'y');

	if ( fetch_account_list )
	{
		tax_form_line->tax_form_line_account_list =
			tax_form_line_account_list(
				tax_form_line->tax_form_string,
				tax_form_line->tax_form_line_string,
				tax_year,
				fetch_journal_list );
	}
	return tax_form_line;
}

char *tax_form_line_primary_where(
			char *tax_form_string,
			char *tax_form_line_string )
{
	static char where[ 256 ];

	sprintf(where,
		"tax_form = '%s' and "
		"tax_form_line = '%s'",
		tax_form_string,
		tax_form_line_string );

	return where;
}

char *tax_form_line_account_system_string(
			char *where )
{
	char system_string[ 1024 ];
	char *order;

	order = "account";

	sprintf(system_string,
		"select.sh '*' tax_form_line_account \"%s\" %s",
		where,
		order );

	return strdup( system_string );
}

TAX_FORM_LINE_ACCOUNT *tax_form_line_account_parse(
			char *input,
			int tax_year,
			boolean fetch_journal_list )
{
	char tax_form_string[ 128 ];
	char tax_form_line_string[ 128 ];
	char account_name[ 128 ];
	TAX_FORM_LINE_ACCOUNT *tax_form_line_account;

	/* See attribute_list tax_form_line_account */
	/* ---------------------------------------- */
	piece( tax_form_string, SQL_DELIMITER, input, 0 );
	piece( tax_form_line_string, SQL_DELIMITER, input, 1 );
	piece( account_name, SQL_DELIMITER, input, 2 );

	tax_form_line_account =
		tax_form_line_account_new(
			strdup( tax_form_string ),
			strdup( tax_form_line_string ),
			strdup( account_name ) );

	tax_form_line_account->account =
		account_fetch(
			tax_form_line_account->
				account_name );

	if ( fetch_journal_list )
	{
		tax_form_line_account->journal_list =
			journal_year_list(
				tax_year,
				tax_form_line_account->account_name,
				1 /* fetch_memo */ );
	}

	return tax_form_line_account;
}

LIST *tax_form_line_account_list(
			char *tax_form_string,
			char *tax_form_line_string,
			int tax_year,
			boolean fetch_journal_list )
{
	FILE *input_pipe;
	char input[ 1024 ];
	LIST *line_account_list;

	input_pipe =
		popen(
			tax_form_line_account_system_string(
				tax_form_line_primary_where(
					tax_form_string,
					tax_form_line_string ) ),
			"r" );

	line_account_list = list_new();

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			line_account_list,
			/* ------------------------------ */
			/* Sets account->accumulate_debit */
			/* ------------------------------ */
			tax_form_line_account_parse(
				input,
				tax_year,
				fetch_journal_list ) );
	}

	pclose( input_pipe );
	return line_account_list;
}

void tax_form_line_list_steady_state(
			LIST *tax_form_line_list )
{
	TAX_FORM_LINE *tax_form_line;

	if ( !list_rewind( tax_form_line_list ) ) return;

	do {
		tax_form_line = list_get( tax_form_line_list );

		tax_form_line_account_list_steady_state(
			tax_form_line->tax_form_line_account_list );

		tax_form_line->tax_form_line_total =
			tax_form_line_total(
				tax_form_line->
					tax_form_line_account_list );

	} while ( list_next( tax_form_line_list ) );
}

double tax_form_line_total(
			LIST *tax_form_line_account_list )
{
	TAX_FORM_LINE_ACCOUNT *tax_form_line_account;
	double total = 0.0;

	if ( !list_rewind( tax_form_line_account_list ) ) return 0.0;

	do {
		tax_form_line_account = list_get( tax_form_line_account_list );

		total += tax_form_line_account->tax_form_line_account_total;

	} while ( list_next( tax_form_line_account_list ) );

	return total;
}

double tax_form_line_account_total(
			LIST *journal_list,
			boolean accumulate_debit,
			boolean current_liability,
			boolean receivable )
{
	JOURNAL *journal;
	double total;

	if ( !list_rewind( journal_list ) ) return 0.0;

	total = 0.0;

	do {
		journal = list_get( journal_list );

		if ( current_liability )
		{
			total +=
				(journal->journal_amount =
					journal->debit_amount);
		}
		else
		if ( receivable )
		{
			total +=
				(journal->journal_amount =
					journal->credit_amount);
		}
		else
		{
			total +=
				(journal->journal_amount =
					journal_amount(
						journal->debit_amount,
						journal->credit_amount,
						accumulate_debit ) );
		}

	} while( list_next( journal_list ) );

	return total;
}

void tax_form_line_account_list_steady_state(
			LIST *tax_form_line_account_list )
{
	TAX_FORM_LINE_ACCOUNT *tax_form_line_account;

	if ( !list_rewind( tax_form_line_account_list ) ) return;

	do {
		tax_form_line_account = list_get( tax_form_line_account_list );

		if ( !list_length( tax_form_line_account->journal_list ) )
		{
			continue;
		}

		tax_form_line_account->current_liability =
			subclassification_current_liability(
				tax_form_line_account->
					account->
					subclassification_name );

		tax_form_line_account->receivable =
			subclassification_receivable(
				tax_form_line_account->
					account->
					subclassification_name );

		tax_form_line_account->tax_form_line_account_total =
			tax_form_line_account_total(
				tax_form_line_account->journal_list,
				tax_form_line_account->
					account->
					accumulate_debit,
				tax_form_line_account->
					current_liability,
				tax_form_line_account->
					receivable );

	} while ( list_next( tax_form_line_account_list ) );
}

TAX_FORM *tax_form_fetch(
			char *tax_form_string,
			int tax_year,
			boolean fetch_line_list,
			boolean fetch_account_list,
			boolean fetch_journal_list )
{
	TAX_FORM *tax_form;

	tax_form =
		tax_form_new(
			tax_form_string,
			tax_year );

	if ( fetch_line_list )
	{
		tax_form->tax_form_line_list =
			tax_form_line_list(
				tax_form_string,
				tax_year,
				fetch_account_list,
				fetch_journal_list );
	}

	return tax_form;
}

LIST *tax_form_line_system_list(
			char *system_string,
			int tax_year,
			boolean fetch_account_list,
			boolean fetch_journal_list )
{
	FILE *input_pipe;
	char input[ 1024 ];
	LIST *system_list;

	if ( !system_string ) return (LIST *)0;

	system_list = list_new();

	input_pipe = popen( system_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			system_list,
			tax_form_line_parse(
				input,
				tax_year,
				fetch_account_list,
				fetch_journal_list ) );
	}

	pclose( input_pipe );
	return system_list;
}

LIST *tax_form_line_list(
			char *tax_form_string,
			int tax_year,
			boolean fetch_account_list,
			boolean fetch_journal_list )
{
	return 
		tax_form_line_system_list(
			tax_form_line_system_string(
				tax_form_primary_where(
					tax_form_string ) ),
			tax_year,
			fetch_account_list,
			fetch_journal_list );
}

