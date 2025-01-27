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

TAX_FORM *tax_form_new(	char *tax_form_string )
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
	piece( tax_form_line, SQL_DELIMITER, input, 1 );

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
				fetch_journa_list );
	}
	return tax_form_line;
}

char *tax_form_line_primary_where(
			char *tax_form_string,
			char *tax_form_line_string )
{
	static char where[ 256 ];

	sprintf(where,
		"tax_form = '%s' and"
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
			boolean fetch_journal_list )
{
	char tax_form_string[ 128 ];
	char tax_form_line_string[ 128 ];
	char *account_name[ 128 ];
	TAX_FORM_LINE_ACCOUNT *tax_form_line_account;

	/* See attribute_list tax_form_line_account */
	/* ---------------------------------------- */
	piece( tax_form_string, SQL_DELIMITER, input, 0 );
	piece( tax_form_line, SQL_DELIMITER, input, 1 );
	piece( account_name, SQL_DELIMITER, input, 2 );

	tax_form_line_account =
		tax_form_line_account_new(
			strdup( tax_form_string ),
			strdup( tax_form_line_string ),
			strdup( account_name ) );

	tax_form_line_account->account =
		account_fetch(
			tax_form_line_account->account_name );

	tax_form_line_account->current_liability =
		subclassification_current_liability(
			tax_form_line_account->
				account->
				subclassification_name );

	if ( fetch_journal_list )
	{
		tax_form_line_account->journal_list =
			journal_year_list(
				tax_year,
				tax_form_line_account->account_name );
	}

	return tax_form_line_account;
}

LIST *tax_form_line_account_list(
			char *tax_form_string,
			char *tax_form_line_string,
			int tax_year,
			boolean fetch_journa_list )
{
	FILE *input_pipe;
	char input[ 1024 ];
	LIST *line_account_list;
	TAX_FORM_LINE_ACCOUNT *tax_form_line_account;

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
			tax_form_line_list,
			( tax_form_line_account =
				tax_form_line_account_parse(
					input,
					fetch_journal_list ) ) );

		tax_form_line_account->subclassification_name =
			account_subclassification_name(
				tax_form_line_account->
					account_name );

		tax_form_line_account->element_name =
			subclassification_element_name(
				tax_form_line_account->subclassification );

		tax_form_line_account->accumulate_debit =
			element_accumulate_debit(
				tax_form_line_account->element_name );

		tax_form_account->current_liability =
			subclassification_current_liability(
				tax_form_account->subclassification_name );
	}

	pclose( input_pipe );
	return tax_form_line_account_list;
}

#ifdef NOT_DEFINED
LIST *tax_fetch_account_transaction_list(
			char *begin_date_string,
			char *end_date_string,
			char *account_name )
{
	char where[ 1024 ];

	sprintf( where,
		 "transaction_date_time >= '%s' and			"
		 "transaction_date_time <= '%s 23:59:59' and		"
		 "exists (select 1					"
		 "	  from journal_ledger				"
		 "	  where transaction.full_name =			"
		 "		journal_ledger.full_name and		"
		 "	        transaction.street_address =		"
		 "		journal_ledger.street_address and	"
		 "	        transaction.transaction_date_time =	"
		 "		journal_ledger.transaction_date_time and"
		 "		journal_ledger.account = '%s')		",
		 begin_date_string,
		 end_date_string,
		 account_name );

	return transaction_list_fetch( where, 1 /* fetch_journal_list */ );
}

TAX_FORM_LINE_ACCOUNT *tax_form_line_account_seek(
				LIST *tax_form_line_list,
				char *account_name )
{
	TAX_FORM_LINE *tax_form_line;
	TAX_FORM_LINE_ACCOUNT *tax_form_line_account;
	LIST *tax_form_line_account_list;

	if ( !list_rewind( tax_form_line_list ) )
		return (TAX_FORM_LINE_ACCOUNT *)0;

	do {
		tax_form_line =
			list_get_pointer(
				tax_form_line_list );

		tax_form_line_account_list =
			tax_form_line->
				tax_form_line_account_list;

		if ( !list_rewind( tax_form_line_account_list ) )
			continue;

		do {
			tax_form_line_account =
				list_get_pointer(
					tax_form_line_account_list );

			if ( timlib_strcmp( tax_form_line_account->
						account_name,
					    account_name ) == 0 )
			{
				return tax_form_line_account;
			}
		} while( list_next( tax_form_line_account_list ) );

	} while( list_next( tax_form_line_list ) );

	return (TAX_FORM_LINE_ACCOUNT *)0;

}

/* ------------------------------------ */
/* Returns process.tax_form_line_list. 	*/
/* tax_form_account_total is set.	*/
/* tax_form_line_total is set.		*/
/* ------------------------------------ */
LIST *tax_process_set_totals(	LIST *input_tax_form_line_list,
				int tax_year )
{
	TAX_FORM_LINE *tax_form_line;
	LIST *process_tax_form_line_list;

	if ( !list_rewind( input_tax_form_line_list ) )
		return (LIST *)0;

	process_tax_form_line_list = list_new();

	do {
		tax_form_line = list_get( input_tax_form_line_list );

		tax_form_line->tax_form_line_total =
			tax_form_line_get_total(
				/* --------------------------- */
				/* Sets tax_form_account_total */
				/* --------------------------- */
				tax_form_line->
					tax_form_line_account_list,
				tax_year );

		list_append_pointer(
			process_tax_form_line_list,
			tax_form_line );

	} while ( list_next( input_tax_form_line_list ) );

	return process_tax_form_line_list;

}

/* --------------------------- */
/* Returns tax_form_line_list. */
/* --------------------------- */
LIST *tax_process_set_journal_list(
				LIST *unaccounted_journal_list,
				LIST *tax_form_line_list,
				LIST *transaction_list,
				char *account_name )
{
	TRANSACTION *transaction;
	JOURNAL *journal;
	TAX_FORM_LINE_ACCOUNT *tax_form_line_account;

	if ( !list_rewind( transaction_list ) )
		return tax_form_line_list;

	do {
		transaction = list_get( transaction_list );

		if ( !list_rewind( transaction->journal_list ) )
			continue;

		do {
			journal =
				list_get(
					transaction->journal_list );

			/* ------------------------------------ */
			/* Ignore the cash and accumulated	*/
			/* depreciation entries. We want the	*/
			/* other ones.				*/
			/* ------------------------------------ */
			if ( timlib_strcmp(
				journal->account_name,
				account_name ) == 0 )
			{
				continue;
			}

			tax_form_line_account =
				tax_form_line_account_seek(
					tax_form_line_list,
					journal->account_name );

			if ( !tax_form_line_account
			&&   unaccounted_journal_list )
			{
				list_append_pointer(
					unaccounted_journal_list,
					journal );
				continue;
			}

			if ( !tax_form_line_account->journal_list )
			{
				tax_form_line_account->
					journal_list =
						list_new();
			}

			journal->property_street_address =
				transaction->property_street_address;

			list_set(
				tax_form_line_account->
					journal_list,
				journal );

		} while( list_next( transaction->journal_list ) );

	} while( list_next( transaction_list ) );

	return tax_form_line_list;
}

LIST *tax_fetch_property_street_address_list(
					char *application_name,
					char *begin_date_string,
					char *end_date_string )
{
	char sys_string[ 1024 ];
	char *select;
	char *folder_from;
	char where[ 256 ];
	LIST *street_address_list;

	select = "property_street_address";

	sprintf( where,
		 "service_placement_date is not null and	"
		 "service_placement_date >= '%s' and		"
		 "(disposal_date is null or			"
		 " disposal_date <= '%s')			",
		 begin_date_string,
		 end_date_string );

	/* PROPERTY_PURCHASE */
	/* ----------------- */
	folder_from = "property_purchase";

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=%s			"
		 "			where=\"%s\"			"
		 "			order=select			",
		 application_name,
		 select,
		 folder_from,
		 where );

	street_address_list = pipe2list( sys_string );

	/* PRIOR_PROPERTY */
	/* -------------- */
	folder_from = "prior_property";

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=%s			"
		 "			where=\"%s\"			"
		 "			order=select			",
		 application_name,
		 select,
		 folder_from,
		 where );

	list_append_list(
		street_address_list,
		pipe2list( sys_string ) );

	return street_address_list;

}

LIST *tax_fetch_rental_property_list(	char *application_name,
					char *end_date_string,
					int tax_year )
{
	TAX_INPUT_RENTAL_PROPERTY *tax_input_rental_property;
	char input_buffer[ 128 ];
	char join_where[ 512 ];
	char where[ 1024 ];
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char *select;
	char *folder_from;
	LIST *rental_property_list = list_new();

	/* PROPERTY_PURCHASE */
	/* ----------------- */
	select = "property_purchase.property_street_address,recovery_amount";
	folder_from = "property_purchase,tax_property_recovery";

	sprintf( join_where,
		 "property_purchase.property_street_address =	"
		 "tax_property_recovery.property_street_address " );

	sprintf( where,
		 "service_placement_date is not null and	"
		 "( disposal_date is null			"
		 "  or disposal_date <= '%s' ) and		"
		 "tax_year = %d					",
		 end_date_string,
		 tax_year );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=%s			"
		 "			where=\"%s\"			"
		 "			order=service_placement_date	",
		 application_name,
		 select,
		 folder_from,
		 where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		tax_input_rental_property =
			tax_input_rental_property_new(
				input_buffer );

		list_append_pointer(
			rental_property_list,
			tax_input_rental_property );
	}

	pclose( input_pipe );

	/* PRIOR_PROPERTY */
	/* -------------- */
	select = "prior_property.property_street_address,recovery_amount";
	folder_from = "prior_property,tax_prior_property_recovery";

	sprintf( join_where,
		 "prior_property.property_street_address =		"
		 "tax_prior_property_recovery.property_street_address 	" );

	sprintf( where,
		 "service_placement_date is not null and	"
		 "( disposal_date is null			"
		 "  or disposal_date <= '%s' ) and		"
		 "tax_year = %d					",
		 end_date_string,
		 tax_year );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=%s			"
		 "			where=\"%s\"			"
		 "			order=service_placement_date	",
		 application_name,
		 select,
		 folder_from,
		 where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		tax_input_rental_property =
			tax_input_rental_property_new(
				input_buffer );

		list_append_pointer(
			rental_property_list,
			tax_input_rental_property );
	}

	pclose( input_pipe );

	return rental_property_list;

}

TAX_OUTPUT_RENTAL_PROPERTY *tax_output_rental_property_new(
				char *property_street_address )
{
	TAX_OUTPUT_RENTAL_PROPERTY *t;

	if ( ! ( t = calloc( 1, sizeof ( TAX_OUTPUT_RENTAL_PROPERTY ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	t->property_street_address = property_street_address;

	return t;

}

TAX_INPUT_RENTAL_PROPERTY *tax_input_rental_property_new(
				char *input_buffer )
{
	TAX_INPUT_RENTAL_PROPERTY *t;
	char piece_buffer[ 128 ];

	if ( ! ( t = calloc( 1, sizeof ( TAX_INPUT_RENTAL_PROPERTY ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	piece(	piece_buffer,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		0 );

	t->property_street_address = strdup( piece_buffer );

	piece(	piece_buffer,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		1 );

	t->recovery_amount = atof( piece_buffer );

	return t;

}

TAX_FORM_LINE_RENTAL *tax_form_line_rental_new(
				char *tax_form_line,
				char *tax_form_description,
				LIST *tax_form_line_account_list )
{
	TAX_FORM_LINE_RENTAL *t;

	if ( ! ( t = calloc( 1, sizeof ( TAX_FORM_LINE_RENTAL ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	t->tax_form_line = tax_form_line;
	t->tax_form_description = tax_form_description;
	t->tax_form_line_account_list = tax_form_line_account_list;

	return t;

}

LIST *tax_form_line_rental_get_empty_property_list(
			LIST *input_rental_property_list )
{
	TAX_INPUT_RENTAL_PROPERTY *tax_input_rental_property;
	TAX_OUTPUT_RENTAL_PROPERTY *tax_output_rental_property;
	LIST *empty_rental_property_list;

	if ( !list_rewind( input_rental_property_list ) )
		return (LIST *)0;

	empty_rental_property_list = list_new();

	do {
		tax_input_rental_property =
			list_get_pointer(
				input_rental_property_list );

		tax_output_rental_property =
			tax_output_rental_property_new(
				tax_input_rental_property->
					property_street_address );

		list_append_pointer(
			empty_rental_property_list,
			tax_output_rental_property );

	} while( list_next( input_rental_property_list ) );

	return empty_rental_property_list;

}

LIST *tax_get_tax_form_line_rental_list(
			LIST *tax_form_line_list,
			LIST *rental_property_list )
{
	TAX_FORM_LINE *tax_form_line;
	TAX_FORM_LINE_RENTAL *tax_form_line_rental;
	LIST *tax_form_line_rental_list;

	if ( !list_rewind( tax_form_line_list ) ) return (LIST *)0;

	tax_form_line_rental_list = list_new();

	do {
		tax_form_line = list_get( tax_form_line_list );

		if ( !list_length(
			tax_form_line->
				tax_form_line_account_list ) )
		{
			continue;
		}

		tax_form_line_rental =
			tax_form_line_rental_new(
				tax_form_line->tax_form_line,
				tax_form_line->tax_form_description,
				tax_form_line->tax_form_line_account_list );

		list_append_pointer(
			tax_form_line_rental_list,
			tax_form_line_rental );

		tax_form_line_rental->rental_property_list =
			tax_form_line_rental_get_empty_property_list(
				rental_property_list );

	} while( list_next( tax_form_line_list ) );

	return tax_form_line_rental_list;

}

void tax_line_rental_list_accumulate_line_total(
				LIST *tax_form_line_rental_list,
				LIST *tax_input_rental_property_list )
{
	TAX_FORM_LINE_RENTAL *tax_form_line_rental;

	if ( !list_rewind( tax_form_line_rental_list ) ) return;

	do {
		tax_form_line_rental =
			list_get_pointer(
				tax_form_line_rental_list );

		if ( timlib_strncmp(
			tax_form_line_rental->tax_form_description,
			TAX_DEPRECIATION_DESCRIPTION ) == 0 )
		{
			tax_rental_property_list_accumulate_depreciation(
				tax_form_line_rental->
					rental_property_list,
				tax_input_rental_property_list );
		}
		else
		{
			tax_rental_property_list_accumulate_line_total(
				tax_form_line_rental->
					rental_property_list,
				tax_form_line_rental->
					tax_form_line_account_list );
		}

	} while( list_next( tax_form_line_rental_list ) );

}

void tax_form_line_address_rental_property_list_set(
			LIST *rental_property_list,
			double debit_amount,
			double credit_amount,
			char *property_street_address,
			boolean accumulate_debit )
{
	TAX_OUTPUT_RENTAL_PROPERTY *tax_output_rental_property;

	if ( !list_rewind( rental_property_list ) ) return;

	do {
		tax_output_rental_property =
			list_get_pointer(
				rental_property_list );

		if ( timlib_strcmp( 
				property_street_address,
				tax_output_rental_property->
					property_street_address ) == 0 )
		{
			tax_output_rental_property->
				tax_form_line_total +=
					journal_amount(
						debit_amount,
						credit_amount,
						accumulate_debit );
			return;
		}

	} while( list_next( rental_property_list ) );

	fprintf( stderr,
		 "ERROR in %s/%s()/%d: cannot find rental property = %s.\n",
		 __FILE__,
		 __FUNCTION__,
		 __LINE__,
		 property_street_address );

	exit( 1 );

}

void tax_form_line_distribute_rental_property_list_set(
			LIST *rental_property_list,
			double debit_amount,
			double credit_amount,
			boolean accumulate_debit,
			double denominator )
{
	TAX_OUTPUT_RENTAL_PROPERTY *tax_output_rental_property;

	if ( !list_rewind( rental_property_list ) ) return;

	do {
		tax_output_rental_property =
			list_get_pointer(
				rental_property_list );

		tax_output_rental_property->
			tax_form_line_total +=
				( journal_amount(
					debit_amount,
					credit_amount,
					accumulate_debit ) /
				  denominator );

	} while( list_next( rental_property_list ) );

}

void tax_rental_journal_list_accumulate_line_total(
				LIST *rental_property_list,
				LIST *journal_list,
				boolean accumulate_debit )
{
	JOURNAL *journal;
	double denominator;

	if ( !list_rewind( journal_list ) ) return;

	if ( !list_length( rental_property_list ) ) return;

	denominator =
		(double)
		list_length(
			rental_property_list );

	do {
		journal = list_get( journal_list );

		if ( journal->property_street_address
		&&   *journal->property_street_address )
		{
			tax_form_line_address_rental_property_list_set(
				rental_property_list,
				journal->debit_amount,
				journal->credit_amount,
				journal->property_street_address,
				accumulate_debit );
		}
		else
		{
			tax_form_line_distribute_rental_property_list_set(
				rental_property_list,
				journal->debit_amount,
				journal->credit_amount,
				accumulate_debit,
				denominator );
		}

	} while( list_next( journal_list ) );
}

void tax_rental_property_list_accumulate_line_total(
				LIST *rental_property_list,
				LIST *tax_form_line_account_list )
{
	TAX_FORM_LINE_ACCOUNT *tax_form_line_account;

	if ( !list_rewind( tax_form_line_account_list ) ) return;

	do {
		tax_form_line_account =
			list_get_pointer(
				tax_form_line_account_list );

		tax_rental_journal_list_accumulate_line_total(
				rental_property_list,
				tax_form_line_account->journal_list,
				tax_form_line_account->accumulate_debit );

	} while( list_next( tax_form_line_account_list ) );
}

void tax_form_line_set_depreciation(
				LIST *tax_form_line_list,
				double total_recovery_amount )
{
	TAX_FORM_LINE *tax_form_line;

	if ( !list_rewind( tax_form_line_list ) ) return;

	do {
		tax_form_line =
			list_get_pointer(
				tax_form_line_list );

		if ( timlib_strncmp(
			tax_form_line->tax_form_description,
			TAX_DEPRECIATION_DESCRIPTION ) == 0 )
		{
			tax_form_line->tax_form_line_total =
				total_recovery_amount;

			return;
		}

	} while( list_next( tax_form_line_list ) );
}

TAX_INPUT_RENTAL_PROPERTY *tax_rental_property_seek(
				LIST *tax_input_rental_property_list,
				char *property_street_address )
{
	TAX_INPUT_RENTAL_PROPERTY *tax_input_rental_property;

	if ( !list_rewind( tax_input_rental_property_list ) )
	{
		return ( TAX_INPUT_RENTAL_PROPERTY *)0;
	}

	do {
		tax_input_rental_property =
			list_get_pointer(
				tax_input_rental_property_list );

		if ( strcmp(	tax_input_rental_property->
					property_street_address,
				property_street_address ) == 0 )
		{
			return tax_input_rental_property;
		}

	} while( list_next( tax_input_rental_property_list ) );

	return ( TAX_INPUT_RENTAL_PROPERTY *)0;

}

void tax_rental_property_list_accumulate_depreciation(
				LIST *tax_output_rental_property_list,
				LIST *tax_input_rental_property_list )
{
	TAX_INPUT_RENTAL_PROPERTY *tax_input_rental_property;
	TAX_OUTPUT_RENTAL_PROPERTY *tax_output_rental_property;

	if ( !list_rewind( tax_output_rental_property_list ) ) return;

	do {
		tax_output_rental_property =
			list_get_pointer(
				tax_output_rental_property_list );

		tax_input_rental_property =
			tax_rental_property_seek(
				tax_input_rental_property_list,
				tax_output_rental_property->
					property_street_address );

		if ( !tax_input_rental_property )
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: cannot find property = %s\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				tax_output_rental_property->
					property_street_address );
			exit( 1 );
				 
		}

		tax_output_rental_property->tax_form_line_total =
			tax_input_rental_property->recovery_amount;

	} while( list_next( tax_output_rental_property_list ) );

}

/* --------------------------- */
/* Sets tax_form_account_total */
/* --------------------------- */
double tax_form_line_get_total(	LIST *tax_form_line_account_list,
				int tax_year )
{
	TAX_FORM_LINE_ACCOUNT *a;
	double total;

	if ( !list_rewind( tax_form_line_account_list ) ) return 0.0;

	total = 0.0;

	do {
		a = list_get( tax_form_line_account_list );

		if ( list_length( a->journal_list ) )
		{
			a->tax_form_account_total =
				tax_form_line_account_get_total(
					a->journal_list,
					a->accumulate_debit,
					tax_year );

			total += a->tax_form_account_total;
		}

	} while ( list_next( tax_form_line_account_list ) );

	return total;

}

double tax_form_line_account_get_total(	LIST *journal_list,
					boolean accumulate_debit,
					int tax_year )
{
	JOURNAL *journal;
	double total;
	double amount;
	char closing_transaction_date[ 16 ];
	char *closing_transaction_date_time;

	sprintf( closing_transaction_date, "%d-12-31", tax_year );

	closing_transaction_date_time =
		transaction_closing_transaction_date_time(
				closing_transaction_date );

	if ( !list_rewind( journal_list ) ) return 0.0;

	total = 0.0;

	do {
		journal = list_get( journal_list );

		if ( strcmp(	journal->transaction_date_time,
				closing_transaction_date_time ) == 0 )
		{
			continue;
		}

		amount = journal_amount(
				journal->debit_amount,
				journal->credit_amount,
				accumulate_debit );

		total += amount;

	} while( list_next( journal_list ) );

	return total;
}

TAX_INPUT_RECOVERY *tax_input_recovery_new(
					char *application_name,
					int tax_year )
{
	TAX_INPUT_RECOVERY *t;

	if ( ! ( t = calloc( 1, sizeof( TAX_INPUT_RECOVERY ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	/* FIXED_ASSET_PURCHASE */
	/* -------------------- */
	if ( folder_exists_folder(
		application_name,
		"tax_fixed_asset_recovery" ) )
	{
		t->fixed_asset_recovery_amount =
			tax_fetch_recovery_amount(
				application_name,
				"tax_fixed_asset_recovery",
				tax_year );
	}

	/* PROPERTY_PURCHASE */
	/* ----------------- */
	if ( folder_exists_folder(
		application_name,
		"tax_property_recovery" ) )
	{
		t->property_recovery_amount =
			tax_fetch_recovery_amount(
				application_name,
				"tax_property_recovery",
				tax_year );
	}

	/* PRIOR_FIXED_ASSET */
	/* ----------------- */
	if ( folder_exists_folder(
		application_name,
		"tax_prior_fixed_asset_recovery" ) )
	{
		t->prior_fixed_asset_recovery_amount =
			tax_fetch_recovery_amount(
				application_name,
				"tax_prior_fixed_asset_recovery",
				tax_year );
	}

	/* PRIOR_PROPERTY */
	/* -------------- */
	if ( folder_exists_folder(
		application_name,
		"tax_prior_property_recovery" ) )
	{
		t->prior_property_recovery_amount =
			tax_fetch_recovery_amount(
				application_name,
				"tax_prior_property_recovery",
				tax_year );
	}

	t->total_recovery_amount =
		t->fixed_asset_recovery_amount +
		t->property_recovery_amount +
		t->prior_fixed_asset_recovery_amount +
		t->prior_property_recovery_amount;

	return t;
}

double tax_fetch_recovery_amount(
			char *application_name,
			char *folder_name,
			int tax_year )
{
	char *select;
	char where[ 128 ];
	char sys_string[ 1024 ];

	select = "sum(recovery_amount)";

	sprintf( where,
		 "tax_year = %d",
		 tax_year );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=%s		"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 folder_name,
		 where );

	return atof( pipe2string( sys_string ) );
}
#endif

