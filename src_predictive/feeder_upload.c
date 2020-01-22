/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/feeder_upload.c			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <unistd.h>
#include "timlib.h"
#include "sed.h"
#include "piece.h"
#include "bank_upload.h"
#include "feeder_upload.h"

LIST *feeder_upload_get_possible_description_list(
				char *bank_description_file,
				char *fund_name,
				double bank_amount,
				double bank_running_balance,
				int check_number )
{
	LIST *possible_description_list = list_new();

	list_append_pointer(
		possible_description_list,
		/* ----------------------- */
		/* Returns strdup() memory */
		/* ----------------------- */
		feeder_upload_get_description_embedded(
			bank_description_file,
			fund_name,
			bank_amount,
			bank_running_balance ) );

	list_append_pointer(
		possible_description_list,
		strdup(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			feeder_upload_get_description_bank_amount(
				bank_description_file,
				bank_amount ) ) );

	if ( check_number )
	{
		list_append_pointer(
			possible_description_list,
			strdup(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				feeder_upload_get_check_portion(
					check_number ) ) );
	}

	return possible_description_list;

} /* feeder_upload_get_possible_description_list() */

/* Returns static memory */
/* --------------------- */
char *feeder_upload_get_description_bank_amount(
			char *bank_description_file,
			double bank_amount )
{
	static char bank_description_bank_amount[ 1024 ];
	char *bank_amount_portion;

	bank_amount_portion =
		/* Returns static memory */
		/* --------------------- */
		feeder_upload_get_bank_amount_portion(
			bank_amount );

	sprintf( bank_description_bank_amount,
		 "%s%s",
		 bank_description_file,
		 bank_amount_portion );

	return bank_description_bank_amount;

} /* feeder_upload_get_description_bank_amount() */

/* Returns strdup() memory */
/* ----------------------- */
char *feeder_upload_get_description_embedded(
			char *bank_description_file,
			char *fund_name,
			double bank_amount,
			double bank_running_balance )
{
	char *bank_amount_portion;
	char bank_description_embedded[ 1024 ];
	char fund_portion[ 512 ];
	char running_balance_portion[ 512 ];

	*fund_portion = '\0';

	/* -------------------------------------- */
	/* Gets sed_trim_double_spaces()'s memory */
	/* -------------------------------------- */
	bank_description_file =
		/* ------------ */
		/* Returns self */
		/* ------------ */
		bank_upload_description_crop(
			/* ------------------------- */
			/* Both Return static memory */
			/* ------------------------- */
			sed_trim_double_spaces(
				feeder_upload_trim_bank_date_from_description(
					bank_description_file ) ) );

/*
	bank_description_file =
		feeder_upload_trim_bank_date_from_description(
			bank_description_file );
*/

	if ( fund_name && *fund_name && strcmp( fund_name, "fund" ) != 0 )
	{
		sprintf( fund_portion, " %s", fund_name );
	}

	sprintf( running_balance_portion, " %.2lf", bank_running_balance );

	bank_amount_portion =
		/* Returns static memory */
		/* --------------------- */
		feeder_upload_get_bank_amount_portion(
			bank_amount );

	sprintf( bank_description_embedded,
		 "%s%s%s%s",
		 bank_description_file,
	 	 fund_portion,
		 bank_amount_portion,
		 running_balance_portion );

	return strdup( bank_description_embedded );

} /* feeder_upload_get_description_embedded() */

/* Returns static memory */
/* --------------------- */
char *feeder_upload_get_bank_amount_portion(
				double bank_amount )
{
	static char bank_amount_portion[ 128 ];

	if ( bank_amount - (double)(int)bank_amount == 0.0 )
	{
		sprintf( bank_amount_portion, " %d", (int)bank_amount );
	}
	else
	{
		sprintf( bank_amount_portion, " %.2lf", bank_amount );

		/* They trim off the pennies if on the dime. */
		/* ----------------------------------------- */
		if ( * (bank_amount_portion +
			strlen( bank_amount_portion ) -
			1 ) == '0' )
		{
			*(bank_amount_portion +
			  strlen( bank_amount_portion ) -
			  1) = '\0';
		}
	}

	return bank_amount_portion;

} /* feeder_upload_get_bank_amount_portion() */

/* Returns static memory */
/* --------------------- */
char *feeder_upload_get_fund_portion(
				char *fund_name )
{
	static char fund_portion[ 128 ];

	*fund_portion = '\0';

	if ( fund_name && *fund_name && strcmp( fund_name, "fund" ) != 0 )
	{
		sprintf( fund_portion, " %s", fund_name );
	}

	return fund_portion;

} /* feeder_upload_get_fund_portion() */

/* Returns static memory */
/* --------------------- */
char *feeder_upload_get_check_portion(
				int check_number )
{
	static char check_portion[ 128 ];

	*check_portion = '\0';

	if ( check_number )
	{
		sprintf( check_portion, "CHECK %d", check_number );
	}

	return check_portion;

} /* feeder_upload_get_check_portion() */

char *feeder_upload_get_like_where(	char *where,
					char *bank_date,
					char *bank_description )
{
	char description_buffer[ 512 ];

	sprintf( where,
		 "bank_date = '%s' and			"
		 "bank_description like '%s%c'		",
		 bank_date,
		 escape_character(	description_buffer,
					bank_description,
					'\'' ),
		 '%' );

	return where;

} /* feeder_upload_get_like_where() */

/* Returns static memory */
/* --------------------- */
char *feeder_upload_trim_bank_date_from_description(
				char *bank_description_file )
{
	static char sans_bank_date_description[ 512 ];
	char *replace;
	char *regular_expression;
	SED *sed;

	regular_expression = "[ ][0-9][0-9]/[0-9][0-9]$";
	replace = "";

	sed = sed_new( regular_expression, replace );

	timlib_strcpy(	sans_bank_date_description,
			bank_description_file,
			512 );

	/* Why do I need to append the EOL character for sed() to work? */
	/* ------------------------------------------------------------ */
	sprintf( sans_bank_date_description +
		 strlen( sans_bank_date_description ),
		 "$" );

	if ( sed_will_replace( sans_bank_date_description, sed ) )
	{
		sed_search_replace( sans_bank_date_description, sed );
	}
	else
	{
		trim_right( sans_bank_date_description, 1 );
	}

	sed_free( sed );

	return timlib_rtrim( sans_bank_date_description );

} /* feeder_upload_trim_bank_date_from_description() */

TRANSACTION *feeder_phrase_match_new_transaction(
				char *full_name,
				char *street_address,
				char *bank_date,
				double bank_amount )
{
	TRANSACTION *transaction;

	transaction =
		ledger_transaction_new(
			full_name,
			street_address,
			ledger_get_transaction_date_time(
				bank_date /* transaction_date */ ),
			(char *)0 /* memo */ );

	transaction->transaction_amount =
		float_abs( bank_amount );

	return transaction;

} /* feeder_phrase_match_new_transaction() */

TRANSACTION *feeder_check_number_existing_transaction(
				LIST *transaction_list,
				int check_number )
{
	return ledger_check_number_seek_transaction(
			transaction_list,
			check_number );
}

JOURNAL_LEDGER *feeder_check_number_existing_journal_ledger(
				LIST *existing_cash_journal_ledger_list,
				int check_number )
{
	return ledger_check_number_seek_journal_ledger(
			existing_cash_journal_ledger_list,
			check_number );
}

TRANSACTION *feeder_phrase_match_build_transaction(
				LIST *reoccurring_transaction_list,
				char *bank_date,
				char *bank_description_embedded,
				double abs_bank_amount )
{
	REOCCURRING_TRANSACTION *reoccurring_transaction;
	TRANSACTION *transaction;
	JOURNAL_LEDGER *journal_ledger;

	if ( ! ( reoccurring_transaction =
				reoccurring_seek_bank_upload_feeder_phrase(
					reoccurring_transaction_list,
					bank_description_embedded ) ) )
	{
		return (TRANSACTION *)0;
	}

	transaction =
		ledger_transaction_new(
			reoccurring_transaction->full_name,
			reoccurring_transaction->street_address,
			ledger_get_transaction_date_time(
				bank_date /* transaction_date */ ),
			(char *)0 /* memo */ );

	transaction->transaction_amount = abs_bank_amount;

	transaction->journal_ledger_list = list_new();

	/* Set the debit account */
	/* --------------------- */
	journal_ledger =
		journal_ledger_new(
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time,
			reoccurring_transaction->debit_account );

	journal_ledger->debit_amount = transaction->transaction_amount;

	list_append_pointer(
		transaction->journal_ledger_list,
		journal_ledger );

	/* Set the credit account */
	/* ---------------------- */
	journal_ledger =
		journal_ledger_new(
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time,
			reoccurring_transaction->credit_account );

	journal_ledger->credit_amount =
		transaction->transaction_amount;

	list_append_pointer(
		transaction->journal_ledger_list,
		journal_ledger );

	return transaction;

} /* feeder_phrase_match_build_transaction() */

/* Sets journal_ledger->match_sum_taken */
/* ------------------------------------ */
LIST *feeder_match_sum_existing_journal_ledger_list(
				LIST *existing_cash_journal_ledger_list,
				double abs_bank_amount,
				char *bank_date,
				boolean check_debit )
{
	FILE *output_pipe;
	char temp_output_file[ 128 ];
	JOURNAL_LEDGER *journal_ledger;
	char *pipe_delimited_transaction_list_string;
	LIST *name_string_list;
	char *name_string;
	LIST *return_list;
	char full_name[ 128 ];
	char street_address[ 128 ];
	char transaction_date_time [ 32 ];
	char sys_string[ 512 ];
	int count = 0;

	if ( !list_rewind( existing_cash_journal_ledger_list ) )
		return (LIST *)0;

	sprintf( temp_output_file,
		 "/tmp/feeder_cash_%d",
		 getpid() );

	sprintf( sys_string,
		 "keys_match_sum.e %.2lf > %s",
		 abs_bank_amount,
		 temp_output_file );

	output_pipe = popen( sys_string, "w" );

	do {
		journal_ledger =
			list_get_pointer( 
				existing_cash_journal_ledger_list );

		if ( journal_ledger->match_sum_taken )
			continue;

		if ( journal_ledger->check_number )
			continue;

		fprintf(output_pipe,
		 	"%s^%s^%s|%.2lf\n",
			journal_ledger->full_name,
			journal_ledger->street_address,
			journal_ledger->transaction_date_time,
			(check_debit)
				? journal_ledger->debit_amount
				: journal_ledger->credit_amount );
		
		if ( ++count == FEEDER_KEYS_MATCH_SUM_MAX )
			break;

	} while ( list_next( existing_cash_journal_ledger_list ) );

	pclose( output_pipe );

	if ( !timlib_file_populated( temp_output_file ) )
	{
		sprintf( sys_string, "rm %s", temp_output_file );
		if ( system( sys_string ) ) {};

		return (LIST *)0;
	}

	sprintf( sys_string,
		 "cat %s",
		 temp_output_file );

	/* ------------------------------------------------------------ */
	/* Format: full_name^street_address^transaction_date_time[|...] */
	/* ------------------------------------------------------------ */
	pipe_delimited_transaction_list_string = pipe2string( sys_string );

	sprintf( sys_string, "rm %s", temp_output_file );
	if ( system( sys_string ) ) {};

	name_string_list =
		list_string2list( 
			pipe_delimited_transaction_list_string,
			'|' );

	if ( !list_rewind( name_string_list ) ) return (LIST *)0;

	return_list = list_new();

	do {
		name_string = list_get_pointer( name_string_list );

		if ( timlib_count_delimiters( '^', name_string ) != 2 )
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: unexpected input = [%s]\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 name_string );
			exit( 1 );
		}

		piece( full_name, '^', name_string, 0 );
		piece( street_address, '^', name_string, 1 );
		piece( transaction_date_time, '^', name_string, 2 );

		if ( ! ( journal_ledger =
				ledger_seek_journal_ledger(
					existing_cash_journal_ledger_list,
					full_name,
					street_address,
					transaction_date_time,
					(char *)0 /* account_name */ ) ) )
		{
			fprintf( stderr,
				 "ERROR in %s/%s()/%d: cannot seek %s/%s/%s\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 full_name,
				 street_address,
				 transaction_date_time );
			exit( 1 );
		}

		journal_ledger->match_sum_taken = 1;

		list_append_pointer( return_list, journal_ledger );

	} while ( list_next( name_string_list ) );

	return return_list;

} /* feeder_match_sum_existing_journal_ledger_list() */

