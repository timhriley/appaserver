/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/tax.c				*/
/* -------------------------------------------------------------------- */
/* This is the appaserver tax ADT.					*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include "timlib.h"
#include "appaserver_library.h"
#include "piece.h"
#include "folder.h"
#include "date.h"
#include "tax.h"

TAX *tax_new(			char *application_name,
				char *tax_form,
				int tax_year )
{
	TAX *t;
	char date_buffer[ 16 ];

	/* TAX_INPUT */
	/* --------- */
#ifdef NOT_DEFINED
	char *checking_account;
	checking_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			LEDGER_CASH_KEY,
			0 /* not warning_only */,
			__FUNCTION__ );
#endif

	if ( ! ( t = calloc( 1, sizeof( TAX ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	t->tax_input.tax_year = tax_year;

	t->tax_input.tax_form =
		tax_form_new(
			application_name,
			tax_form,
			t->tax_input.tax_year );

	if ( !t->tax_input.tax_form )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: tax_form_new() returned null.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	/* No transactions. */
	/* ---------------- */
	if ( !list_length( t->tax_input.tax_form->tax_form_line_list ) )
	{
		return (TAX *)0;
	}

	sprintf( date_buffer, "%d-01-01", tax_year );
	t->tax_input.begin_date_string = strdup( date_buffer );

	sprintf( date_buffer, "%d-12-31", tax_year );
	t->tax_input.end_date_string = strdup( date_buffer );

	/* Tax Recovery Operation */
	/* ====================== */
	t->tax_input.tax_input_recovery =
		tax_input_recovery_new(
			application_name,
			tax_year );

#ifdef NOT_DEFINED
	/* Tax Recovery Operation */
	/* ====================== */
	if ( t->tax_input.tax_input_recovery->total_recovery_amount )
	{
		tax_form_line_set_depreciation(
			t->tax_process.tax_form_line_list,
			t->tax_input.
				tax_input_recovery->
				total_recovery_amount );
	}

	if ( !t->tax_process.tax_form_line_list )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: tax_process_get_tax_form_line_list() returned null.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	t->tax_process.tax_form = t->tax_input.tax_form->tax_form;
#endif

	return t;

} /* tax_new() */

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

} /* tax_input_recovery_new() */

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

} /* tax_fetch_recovery_amount() */

TAX_FORM_LINE_ACCOUNT *tax_form_line_account_new(
					char *account_name )
{
	TAX_FORM_LINE_ACCOUNT *t;

	if ( ! ( t = calloc( 1, sizeof( TAX_FORM_LINE_ACCOUNT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	t->account_name = account_name;

	return t;

} /* tax_form_line_account_new() */
	
TAX_FORM *tax_form_new(		char *application_name,
				char *tax_form,
				int tax_year )
{
	TAX_FORM *t;

	if ( ! ( t = calloc( 1, sizeof( TAX_FORM ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	t->tax_form = tax_form;

	t->tax_form_line_list =
		tax_form_fetch_line_list(
			application_name,
			t->tax_form,
			tax_year );
	return t;

} /* tax_form_new() */

TAX_FORM_LINE *tax_form_line_new(	char *tax_form_line,
					char *tax_form_description,
					boolean itemize_accounts )
{
	TAX_FORM_LINE *t;

	if ( ! ( t = calloc( 1, sizeof( TAX_FORM_LINE ) ) ) )
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
	t->itemize_accounts = itemize_accounts;

	return t;

} /* tax_form_line_new() */

LIST *tax_form_fetch_line_list(		char *application_name,
					char *tax_form,
					int tax_year )
{
	TAX_FORM_LINE *tax_form_line = {0};
	LIST *tax_form_line_list;
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char input_buffer[ 512 ];
	char *input_buffer_ptr;
	char tax_form_line_name[ 128 ];
	char tax_form_description[ 128 ];
	char itemize_accounts_yn[ 8 ];
	TAX_FORM_LINE_ACCOUNT *account;

	sprintf( sys_string,
		 "select_tax_form_line.sh %s \"%s\"",
		 application_name,
		 tax_form );

	input_pipe = popen( sys_string, "r" );

	tax_form_line_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		if ( timlib_strncmp(
			input_buffer,
			TAX_FORM_LINE_KEY ) == 0 )
		{
			input_buffer_ptr =
				input_buffer +
				strlen( TAX_FORM_LINE_KEY );

			piece(	tax_form_line_name,
				FOLDER_DATA_DELIMITER,
				input_buffer_ptr,
				0 );
	
			piece(	tax_form_description,
				FOLDER_DATA_DELIMITER,
				input_buffer_ptr,
				1 );
	
			piece(	itemize_accounts_yn,
				FOLDER_DATA_DELIMITER,
				input_buffer_ptr,
				2 );
	
			tax_form_line =
				tax_form_line_new(
					strdup( tax_form_line_name ),
					strdup( tax_form_description ),
					(*itemize_accounts_yn == 'y' ) );

			list_append_pointer(
				tax_form_line_list,
				tax_form_line );
		}
		else
		if ( timlib_strncmp(
			input_buffer,
			TAX_FORM_LINE_ACCOUNT_KEY ) == 0 )
		{
			input_buffer_ptr =
				input_buffer +
				strlen( TAX_FORM_LINE_ACCOUNT_KEY );

			account =
				tax_form_line_account_new(
					strdup( input_buffer_ptr
						/* account_name */ ) );

			account->accumulate_debit =
				ledger_account_get_accumulate_debit(
					application_name,
					account->account_name );

			account->journal_ledger_list =
				ledger_get_year_journal_ledger_list(
					application_name,
					tax_year,
					account->account_name );

			if ( !tax_form_line )
			{
				fprintf(stderr,
		"ERROR in %s/%s()/%d: tax_form_line is null for (%s).\n",
					__FILE__,
					__FUNCTION__,
					__LINE__,
					input_buffer_ptr );

				pclose( input_pipe );
				exit( 1 );
			}

			if ( !tax_form_line->tax_form_line_account_list )
			{
				tax_form_line->tax_form_line_account_list =
					list_new();
			}

			list_append_pointer(
				tax_form_line->tax_form_line_account_list,
				account );
		}
		else
		{
			fprintf( stderr,
"ERROR in %s/%s()/%d: invalid input key for record = (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 input_buffer );

			pclose( input_pipe );
			exit( 1 );
		}

	} /* while( get_line() ) */

	pclose( input_pipe );

	return tax_form_line_list;

} /* tax_form_fetch_line_list() */

LIST *tax_fetch_account_transaction_list(
			char *application_name,
			char *begin_date_string,
			char *end_date_string,
			char *account_name )
{
	char where_clause[ 1024 ];

	sprintf( where_clause,
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

	return ledger_fetch_transaction_list(
			application_name,
			where_clause );

} /* tax_fetch_account_transaction_list() */

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

} /* tax_form_line_account_seek() */

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

} /* tax_process_set_totals() */

/* --------------------------- */
/* Returns tax_form_line_list. */
/* --------------------------- */
LIST *tax_process_set_journal_ledger_list(
				LIST *unaccounted_journal_ledger_list,
				LIST *tax_form_line_list,
				LIST *transaction_list,
				char *account_name )
{
	TRANSACTION *transaction;
	JOURNAL_LEDGER *journal_ledger;
	TAX_FORM_LINE_ACCOUNT *tax_form_line_account;

	if ( !list_rewind( transaction_list ) )
		return tax_form_line_list;

	do {
		transaction = list_get_pointer( transaction_list );

		if ( !list_rewind( transaction->journal_ledger_list ) )
			continue;

		do {
			journal_ledger =
				list_get_pointer(
					transaction->journal_ledger_list );

			/* ------------------------------------ */
			/* Ignore the cash and accumulated	*/
			/* depreciation entries. We want the	*/
			/* other ones.				*/
			/* ------------------------------------ */
			if ( timlib_strcmp(
				journal_ledger->account_name,
				account_name ) == 0 )
			{
				continue;
			}

			tax_form_line_account =
				tax_form_line_account_seek(
					tax_form_line_list,
					journal_ledger->account_name );

			if ( !tax_form_line_account
			&&   unaccounted_journal_ledger_list )
			{
				list_append_pointer(
					unaccounted_journal_ledger_list,
					journal_ledger );
				continue;
			}

			if ( !tax_form_line_account->journal_ledger_list )
			{
				tax_form_line_account->
					journal_ledger_list =
						list_new();
			}

			journal_ledger->property_street_address =
				transaction->property_street_address;

			list_append_pointer(
				tax_form_line_account->
					journal_ledger_list,
				journal_ledger );

		} while( list_next( transaction->journal_ledger_list ) );

	} while( list_next( transaction_list ) );

	return tax_form_line_list;

} /* tax_process_set_journal_ledger_list() */

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

} /* tax_fetch_property_street_address_list() */

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

} /* tax_fetch_rental_property_list() */

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

} /* tax_output_rental_property_new() */

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

} /* tax_output_rental_property_new() */

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

} /* tax_form_line_rental_new() */

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

} /* tax_form_line_rental_get_empty_property_list() */

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

} /* tax_get_tax_form_line_rental_list() */

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

} /* tax_line_rental_list_accumulate_line_total() */

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
					ledger_debit_credit_get_amount(
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

} /* tax_form_line_address_rental_property_list_set() */

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
				( ledger_debit_credit_get_amount(
					debit_amount,
					credit_amount,
					accumulate_debit ) /
				  denominator );

	} while( list_next( rental_property_list ) );

} /* tax_form_line_distribute_rental_property_list_set() */

void tax_rental_journal_ledger_list_accumulate_line_total(
				LIST *rental_property_list,
				LIST *journal_ledger_list,
				boolean accumulate_debit )
{
	JOURNAL_LEDGER *ledger;
	double denominator;

	if ( !list_rewind( journal_ledger_list ) ) return;

	if ( !list_length( rental_property_list ) ) return;

	denominator =
		(double)
		list_length(
			rental_property_list );

	do {
		ledger = list_get_pointer( journal_ledger_list );

		if ( ledger->property_street_address
		&&   *ledger->property_street_address )
		{
			tax_form_line_address_rental_property_list_set(
				rental_property_list,
				ledger->debit_amount,
				ledger->credit_amount,
				ledger->property_street_address,
				accumulate_debit );
		}
		else
		{
			tax_form_line_distribute_rental_property_list_set(
				rental_property_list,
				ledger->debit_amount,
				ledger->credit_amount,
				accumulate_debit,
				denominator );
		}

	} while( list_next( journal_ledger_list ) );

} /* tax_rental_journal_ledger_list_accumulate_line_total() */

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

		tax_rental_journal_ledger_list_accumulate_line_total(
				rental_property_list,
				tax_form_line_account->journal_ledger_list,
				tax_form_line_account->accumulate_debit );

	} while( list_next( tax_form_line_account_list ) );

} /* tax_rental_property_list_accumulate_line_total() */

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

} /* tax_form_line_set_depreciation() */

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

} /* tax_rental_property_seek() */

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

} /* tax_rental_property_list_accumulate_depreciation() */

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

		if ( list_length( a->journal_ledger_list ) )
		{
			a->tax_form_account_total =
				tax_form_line_account_get_total(
					a->journal_ledger_list,
					a->accumulate_debit,
					tax_year );

			total += a->tax_form_account_total;
		}

	} while ( list_next( tax_form_line_account_list ) );

	return total;

} /* tax_form_line_get_total() */

double tax_form_line_account_get_total(	LIST *journal_ledger_list,
					boolean accumulate_debit,
					int tax_year )
{
	JOURNAL_LEDGER *journal_ledger;
	double total;
	double amount;
	char closing_transaction_date[ 16 ];
	char *closing_transaction_date_time;

	sprintf( closing_transaction_date, "%d-12-31", tax_year );

	closing_transaction_date_time =
		ledger_get_closing_transaction_date_time(
				closing_transaction_date );

	if ( !list_rewind( journal_ledger_list ) ) return 0.0;

	total = 0.0;

	do {
		journal_ledger = list_get( journal_ledger_list );

		if ( strcmp(	journal_ledger->transaction_date_time,
				closing_transaction_date_time ) == 0 )
		{
			continue;
		}

		amount = ledger_debit_credit_get_amount(
				journal_ledger->debit_amount,
				journal_ledger->credit_amount,
				accumulate_debit );

		total += amount;

	} while( list_next( journal_ledger_list ) );

	return total;

} /* tax_form_line_account_get_total() */

