/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/feeder.c				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "String.h"
#include "piece.h"
#include "date_convert.h"
#include "sql.h"
#include "sed.h"
#include "feeder.h"

LIST *feeder_phrase_list( void )
{
	LIST *list;
	FILE *input_pipe;
	char input[ 1024 ];

	input_pipe =
		popen(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			feeder_phrase_system_string(
				FEEDER_PHRASE_SELECT,
				FEEDER_PHRASE_TABLE ),
			"r" );

	list = list_new();

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			list,
			feeder_phrase_parse( input ) );
	}

	pclose( input_pipe );

	return list;
}

char *feeder_phrase_system_string(
			char *select,
			char *table )
{
	char system_string[ 1024 ];

	if ( !select || !table )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"select.sh \"%s\" %s",
		select,
		table );

	return strdup( system_string );
}

FEEDER_PHRASE *feeder_phrase_parse( char *input )
{
	char feeder_phrase[ 128 ];
	char buffer[ 128 ];
	FEEDER_PHRASE *phrase;

	if ( !input || !*input ) return (FEEDER_PHRASE *)0;

	/* See FEEDER_PHRASE_SELECT */
	/* ------------------------ */
	piece( feeder_phrase, SQL_DELIMITER, input, 0 );

	phrase = feeder_phrase_new( strdup( feeder_phrase ) );

	if ( piece( buffer, SQL_DELIMITER, input, 1 ) )
	{
		phrase->nominal_account = strdup( buffer );
	}

	if ( piece( buffer, SQL_DELIMITER, input, 2 ) )
	{
		phrase->full_name = strdup( buffer );
	}

	if ( piece( buffer, SQL_DELIMITER, input, 3 ) )
	{
		phrase->street_address = strdup( buffer );
	}

	if ( piece( buffer, SQL_DELIMITER, input, 4 ) )
	{
		phrase->feeder_phrase_ignore = ( *buffer == 'y' );
	}

	return phrase;
}

FEEDER_PHRASE *feeder_phrase_new( char *feeder_phrase )
{
	FEEDER_PHRASE *phrase;

	if ( !feeder_phrase || !*feeder_phrase )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: feeder_phrase is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	phrase = feeder_phrase_calloc();
	phrase->feeder_phrase = feeder_phrase;

	return phrase;
}

FEEDER_PHRASE *feeder_phrase_calloc( void )
{
	FEEDER_PHRASE *feeder_phrase;

	if ( ! ( feeder_phrase = calloc( 1, sizeof( FEEDER_PHRASE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return feeder_phrase;
}

char *feeder_load_row_trim_bank_date( char *description )
{
	static char sans_bank_date_description[ 512 ];
	char *replace;
	char *regular_expression;
	SED *sed;

	regular_expression = "[ ][0-9][0-9]/[0-9][0-9]$";
	replace = "";

	sed = sed_new( regular_expression, replace );

	timlib_strcpy(	sans_bank_date_description,
			description,
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
}

FEEDER_LOAD_ROW *feeder_load_row_new(
			int date_column /* one_based */,
			int description_column /* one_based */,
			int debit_column /* one_based */,
			int credit_column /* one_based */,
			int balance_column /* one_based */,
			char *input,
			int line_number )
{
	FEEDER_LOAD_ROW *feeder_load_row;
	char buffer[ 512 ];

	if ( !date_column
	||   !description_colun
	||   !input
	||   !*input
	||   !line_number )
	{
		return (FEEDER_LOAD_ROW *)0;
	}

	feeder_load_row = feeder_load_row_calloc();

	feeder_load_row->american_date =
		strdup(
			piece_quote_comma(
				buffer,
				input,
				date_column - 1 ) );

	feeder_load_row->international_date =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		feeder_load_row_international_date(
			feeder_load_row->american_date );

	if ( !feeder_load_row->international_date )
	{
		free( feeder_load_row->american_date );
		free( feeder_load_row );
		return (FEEDER_LOAD_ROW *)0;
	}

	feeder_load_row->description =
		strdup(
			piece_quote_comma(
				buffer,
				input,
				description_column - 1 ) );

	if ( !credit_column )
	{
		feeder_load_row->amount =
			atof(
				piece_quote_comma(
					buffer,
					input,
					debit_column - 1 ) );
	}
	else
	{
		feeder_load_row->debit =
			atof(
				piece_quote_comma(
					buffer,
					input,
					debit_column - 1 ) );

		feeder_load_row->credit =
			atof(
				piece_quote_comma(
					buffer,
					input,
					credit_column - 1 ) );

		feeder_load_row->amount =
			feeder_load_row_amount(
				feeder_load_row->debit,
				feeder_load_row->credit );
	}

	if ( balance_column )
	{
		feeder_load_row->balance =
			atof(
				piece_quote_comma(
					buffer,
					input,
					balance_column - 1 ) );
	}

	feeder_load_row->description_embedded =
		feeder_load_row_description_embedded(
			description,
			line_number );

	return feeder_load_row;
}

FEEDER_LOAD_ROW *feeder_load_row_calloc( void )
{
	FEEDER_LOAD_ROW *feeder_load_row;

	if ( ! ( feeder_load_row = calloc( 1, sizeof( FEEDER_LOAD_ROW ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return feeder_load_row;
}

double feeder_load_row_amount(
			double debit,
			double credit )
{
	return debit - credit;
}

char *feeder_load_row_description_embedded(
			char *description,
			int row_number )
{

	return
	/* ----------------------------------------- */
	/* Returns feeder_load_row_description_build */
	/* ----------------------------------------- */
	feeder_load_row_description_crop(
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_load_row_description_build(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			sed_trim_double_spaces(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				feeder_load_row_trim_bank_date(
					description ) ),
			line_number ),
		FEEDER_DESCRIPTION_SIZE );
}

char *feeder_load_row_description_build(
			char *sed_trim_double_spaces,
			int line_number )
{
	char build[ 512 ];

	if ( !sed_trim_double_spaces
	||   !line_number )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(build,
		"%s %d",
		sed_trim_double_spaces,
		line_number );

	return strdup( build );
}

char *feeder_load_row_description_crop(
			char *feeder_load_row_description_build,
			int feeder_description_size )
{
	*( feeder_load_row_description_build + feeder_description_size ) = '\0';

	return feeder_load_row_description_build;
}

double feeder_load_row_last_running_balance(
			boolean reverse_order,
			LIST *feeder_load_row_list )
{
	FEEDER_LOAD_ROW *feeder_load_row;

	if ( !list_length( feeder_load_row_list ) ) return 0.0;

	if ( !reverse_order )
		feeder_load_row = list_last( feeder_load_row_list );
	else
		feeder_load_row = list_first( feeder_load_row_list );

	return feeder_load_row->balance;
}

char *feeder_load_row_international_date( char *american_date )
{
	char international_date[ 128 ];

	date_convert_source_american(
		international_date,
		international,
		american_date );

	if ( date_convert_is_valid_international(
		international_date ) )
	{
		return strdup( international_date );
	}
	else
	{
		return (char *)0;
	}
}

#ifdef NOT_DEFINED
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
		feeder_upload_description_embedded(
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
}

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
}

/* Returns strdup() memory */
/* ----------------------- */
char *feeder_upload_description_embedded(
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

	bank_description_file =
		/* ------------------------- */
		/* Both Return static memory */
		/* ------------------------- */
		sed_trim_double_spaces(
			feeder_upload_trim_bank_date_from_description(
				bank_description_file ) );

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

	return strdup(
			feeder_description_crop(
				bank_description_embedded ) );
}

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
}

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
}

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
}

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
}


JOURNAL *feeder_check_number_existing_journal(
				LIST *existing_cash_journal_list,
				int check_number )
{
	return journal_check_number_seek(
			existing_cash_journal_list,
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
	JOURNAL *journal;

	if ( ! ( reoccurring_transaction =
			reoccurring_bank_upload_feeder_phrase(
				reoccurring_transaction_list,
				bank_description_embedded ) ) )
	{
		return (TRANSACTION *)0;
	}

	if ( reoccurring_transaction->feeder_phrase_ignore )
	{
		return (TRANSACTION *)0;
	}

	if ( !abs_bank_amount ) return (TRANSACTION *)0;

	transaction =
		transaction_new(
			reoccurring_transaction->full_name,
			reoccurring_transaction->street_address,
			transaction_time_append(
				bank_date /* transaction_date */ ) );

	transaction->transaction_amount = abs_bank_amount;

	transaction->journal_list = list_new();

	/* Set the debit account */
	/* --------------------- */
	journal =
		journal_new(
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time,
			reoccurring_transaction->debit_account );

	journal->debit_amount = transaction->transaction_amount;

	list_set(
		transaction->journal_list,
		journal );

	/* Set the credit account */
	/* ---------------------- */
	journal =
		journal_new(
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time,
			reoccurring_transaction->credit_account );

	journal->credit_amount =
		transaction->transaction_amount;

	list_set(
		transaction->journal_list,
		journal );

	return transaction;
}

/* Sets journal->match_sum_taken */
/* ----------------------------- */
LIST *feeder_match_sum_existing_journal_list(
				LIST *existing_cash_journal_list,
				double abs_bank_amount,
				boolean check_debit )
{
	FILE *output_pipe;
	char temp_output_file[ 128 ];
	JOURNAL *journal;
	char *pipe_delimited_transaction_list_string;
	LIST *name_string_list;
	char *name_string;
	LIST *return_list;
	char full_name[ 128 ];
	char street_address[ 128 ];
	char transaction_date_time [ 32 ];
	char sys_string[ 512 ];
	int count = 0;
	double send_amount;

	if ( !list_rewind( existing_cash_journal_list ) )
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
		journal =
			list_get( 
				existing_cash_journal_list );

		if ( journal->match_sum_taken )
			continue;

		if ( journal->check_number )
			continue;

		send_amount =
			(check_debit)
				? journal->debit_amount
				: journal->credit_amount;

		if ( !send_amount ) continue;

		fprintf(output_pipe,
		 	"%s^%s^%s|%.2lf\n",
			journal->full_name,
			journal->street_address,
			journal->transaction_date_time,
			send_amount );

		if ( ++count == FEEDER_KEYS_MATCH_SUM_MAX )
			break;

	} while ( list_next( existing_cash_journal_list ) );

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

		if ( ! ( journal =
				journal_transaction_date_time_seek(
					existing_cash_journal_list,
					full_name,
					street_address,
					transaction_date_time ) ) )
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

		journal->match_sum_taken = 1;

		list_set( return_list, journal );

	} while ( list_next( name_string_list ) );

	return return_list;
}

char *feeder_description_crop( char *bank_description )
{
	if ( strlen( bank_description ) > FEEDER_DESCRIPTION_SIZE )
	{
		*( bank_description + FEEDER_DESCRIPTION_SIZE ) = '\0';
	}
	return bank_description;
}
#endif
