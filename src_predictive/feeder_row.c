/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/feeder_row.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "String.h"
#include "piece.h"
#include "appaserver.h"
#include "column.h"
#include "float.h"
#include "sql.h"
#include "sed.h"
#include "environ.h"
#include "process.h"
#include "application_constant.h"
#include "optional_column.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "date_convert.h"
#include "account.h"
#include "exchange.h"
#include "transaction.h"
#include "transaction_date.h"
#include "journal_propagate.h"
#include "feeder_load_event.h"
#include "feeder.h"
#include "feeder_row.h"

char *feeder_row_exist_where(
		char *feeder_account_name,
		char *feeder_match_minimum_date )
{
	static char where[ 128 ];

	if ( !feeder_account_name
	||   strcmp( feeder_account_name, "feeder_account" ) == 0
	||   !feeder_match_minimum_date )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		where,
		sizeof ( where ),
		"feeder_account = '%s' and "
		"feeder_date >= '%s'",
		feeder_account_name,
		feeder_match_minimum_date );

	return where;
}

FEEDER_ROW_EXIST *feeder_row_exist_parse( char *input )
{
	FEEDER_ROW_EXIST *feeder_row_exist;
	char buffer[ 256 ];

	if ( !input || !*input ) return NULL;

	feeder_row_exist = feeder_row_exist_calloc();

	/* See FEEDER_ROW_EXIST_SELECT */
	/* --------------------------- */
	piece( buffer, SQL_DELIMITER, input, 0 );
	feeder_row_exist->feeder_date = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 1 );
	feeder_row_exist->file_row_description = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	feeder_row_exist->transaction_date_time = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	feeder_row_exist->file_row_amount = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	feeder_row_exist->file_row_balance = atof( buffer );

	return feeder_row_exist;
}

FEEDER_ROW_EXIST *feeder_row_exist_calloc( void )
{
	FEEDER_ROW_EXIST *feeder_row_exist;

	if ( ! ( feeder_row_exist = calloc( 1, sizeof ( FEEDER_ROW_EXIST ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return feeder_row_exist;
}

FEEDER_ROW_EXIST *feeder_row_exist_seek(
		char *international_date,
		char *description_embedded,
		LIST *feeder_row_exist_list )
{
	FEEDER_ROW_EXIST *feeder_row_exist;

	if ( !international_date
	||   !description_embedded )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( feeder_row_exist_list ) )
	do {
		feeder_row_exist =
			list_get(
				feeder_row_exist_list );

		if ( strcmp(	feeder_row_exist->feeder_date,
				international_date ) == 0
		&&   strcmp(	feeder_row_exist->file_row_description,
				description_embedded ) == 0 )
		{
			return feeder_row_exist;
		}

	} while ( list_next( feeder_row_exist_list ) );

	return NULL;
}


void feeder_row_list_insert(
		char *fund_name,
		char *feeder_account_name,
		char *feeder_load_date_time,
		LIST *feeder_row_list,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	FILE *output_pipe;
	char *system_string;
	FEEDER_ROW *feeder_row;
	JOURNAL *journal;
	char *insert_string;

	if ( !list_rewind( feeder_row_list ) ) return;

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_row_list_insert_system_string(
			SQL_DELIMITER,
			FEEDER_ROW_INSERT,
			FEEDER_ROW_TABLE,
			PREDICTIVE_FUND_COLUMN,
			ENTITY_CONTACT_KEY_COLUMN,
			fund_boolean,
			contact_key_boolean );

	/* -------------- */
	/* Safely returns */
	/* -------------- */
	output_pipe = appaserver_output_pipe( system_string );

	free( system_string );

	do {
		feeder_row = list_get( feeder_row_list );

		if ( feeder_row->feeder_row_exist_seek )
		{
			continue;
		}

		if ( !feeder_row->feeder_phrase_seek
		&&   !feeder_row->feeder_matched_journal )
		{
			continue;
		}

		journal =
		   /* -------------- */
		   /* Safely returns */
		   /* -------------- */
		   feeder_row_journal(
			fund_name,
			feeder_row->feeder_matched_journal,
			feeder_row->feeder_phrase_seek,
			/* ------------------------------------------ */
			/* See feeder_row_transaction_date_time_set() */
			/* ------------------------------------------ */
			feeder_row->transaction_date_time );

		insert_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			feeder_row_insert_string(
				SQL_DELIMITER,
				fund_name,
				feeder_account_name,
				feeder_load_date_time,
				feeder_row->
					feeder_load_row->
					international_date
					/* feeder_date */,
				feeder_row->feeder_row_number,
				feeder_row->
					feeder_load_row->
					description_embedded
					/* file_row_description */,
				feeder_row->
					feeder_load_row->
					exchange_journal_amount
					/* file_row_amount */,
				feeder_row->
					feeder_load_row->
					file_row_balance,
				feeder_row->calculate_balance,
				feeder_row->
					feeder_load_row->
					check_number,
				fund_boolean,
				contact_key_boolean,
				journal->full_name,
				journal->contact_key,
				journal->transaction_date_time,
				/* -------------- */
				/* Safely returns */
				/* -------------- */
				feeder_row_phrase(
					feeder_row->feeder_phrase_seek ) );

		fprintf(output_pipe,
			"%s\n",
			insert_string );

		free( insert_string );

	} while ( list_next( feeder_row_list ) );

	pclose( output_pipe );
}

char *feeder_row_list_insert_system_string(
		const char sql_delimiter,
		const char *feeder_row_insert,
		const char *feeder_row_table,
		const char *predictive_fund_column,
		const char *entity_contact_key_column,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	char *field;
	char system_string[ 1024 ];

	field = 
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_row_list_insert_field(
			feeder_row_insert,
			predictive_fund_column,
			entity_contact_key_column,
			fund_boolean,
			contact_key_boolean );

	snprintf(
		system_string,
		sizeof ( system_string ),
	 	"insert_statement table=%s field=%s del='%c' 	|"
		"tee_appaserver.sh				|"
	 	"sql.e 2>&1					|"
	 	"html_paragraph_wrapper.e			 ",
	 	feeder_row_table,
	 	field,
	 	sql_delimiter );

	free( field );

	return strdup( system_string );
}

char *feeder_row_list_insert_field(
		const char *feeder_row_insert,
		const char *fund_column,
		const char *contact_key_column,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	OPTIONAL_COLUMN *optional_column;

	optional_column =
		optional_column_new(
			',' /* delimiter */,
			(char *)feeder_row_insert /* base_string */,
			(char *)fund_column /* component */,
			0 /* not escape_boolean */,
			fund_boolean /* set_boolean */ );

	optional_column =
		optional_column_new(
			',' /* delimiter */,
			optional_column->return_string /* base_string */,
			(char *)contact_key_column /* component */,
			0 /* not escape_boolean */,
			contact_key_boolean /* set_boolean */ );

	free( optional_column->prior_return_string );

	return optional_column->return_string /* heap memory */;
}

char *feeder_row_insert_string(
		const char sql_delimiter,
		char *fund_name,
		char *feeder_account_name,
		char *feeder_load_date_time,
		char *feeder_date,
		int feeder_row_number,
		char *file_row_description,
		double file_row_amount,
		double file_row_balance,
		double feeder_row_calculate_balance,
		int check_number,
		boolean fund_boolean,
		boolean contact_key_boolean,
		char *full_name,
		char *contact_key,
		char *transaction_date_time,
		char *phrase )
{
	char insert_string[ 1024 ];
	char check_number_string[ 16 ];
	OPTIONAL_COLUMN *optional_column;

	if ( !feeder_account_name
	||   !feeder_load_date_time
	||   !feeder_date
	||   !feeder_row_number
	||   !file_row_description
	||   !file_row_amount
	||   !full_name
	||   !transaction_date_time
	||   !phrase )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( check_number )
		sprintf( check_number_string, "%d", check_number );
	else
		*check_number_string = '\0';

	/* See FEEDER_ROW_INSERT */
	/* --------------------- */
	snprintf(
		insert_string,
		sizeof ( insert_string ),
		"%s^%s^%s^%d^%s^%.2lf^%.2lf^%.2lf^%s^%s^%s^%s",
		feeder_account_name,
		feeder_load_date_time,
		feeder_date,
		feeder_row_number,
		file_row_description,
		file_row_amount,
		file_row_balance,
		feeder_row_calculate_balance,
		check_number_string,
	 	full_name,
		transaction_date_time,
		phrase );

	optional_column =
		optional_column_new(
			sql_delimiter,
			insert_string /* base_string */,
			fund_name /* component */,
			1 /* not escape_boolean */,
			fund_boolean /* set_boolean */ );

	optional_column =
		optional_column_new(
			sql_delimiter,
			optional_column->return_string /* base_string */,
			contact_key /* component */,
			1 /* not escape_boolean */,
			contact_key_boolean /* set_boolean */ );

	free( optional_column->prior_return_string );

	return optional_column->return_string /* heap memory */;
}

char *feeder_row_transaction_date_time(
		char *international_date,
		char *minimum_transaction_date_time )
{
	char *increment_date_time;
	char transaction_date_time[ 32 ];

	if ( !international_date )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: international_date is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	increment_date_time =
		/* ------------------------------------ */
		/* Returns heap memory.			*/
		/* Increments second each invocation.   */
		/* ------------------------------------ */
		transaction_increment_date_time(
			international_date
				/* transaction_date_string */ );

	strcpy( transaction_date_time, increment_date_time ); 

	free( increment_date_time );

	if ( minimum_transaction_date_time
	&&   strcmp(
		transaction_date_time,
		minimum_transaction_date_time ) < 0 )
	{
		DATE *date =
			date_19new(
				minimum_transaction_date_time );

		date_increment_second(
			date,
			1 /* second */ );

		increment_date_time =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			date_display19(	date );

		strcpy(
			transaction_date_time,
			increment_date_time );

		free( increment_date_time );

		date_free( date );
	}

	return strdup( transaction_date_time );
}

LIST *feeder_row_exist_list(
		const char *feeder_row_table,
		char *feeder_account_name,
		char *feeder_match_minimum_date )
{
	LIST *list;
	char *tmp;
	FILE *input_pipe;
	char input[ 1024 ];

	if ( !feeder_account_name
	||   !feeder_match_minimum_date )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	list = list_new();

	input_pipe =
		appaserver_input_pipe(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			( tmp = feeder_row_exist_system_string(
				FEEDER_ROW_EXIST_SELECT,
				feeder_row_table,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				feeder_row_exist_where(
					feeder_account_name,
					feeder_match_minimum_date ) ) ) );

	while ( string_input( input, input_pipe, sizeof ( input ) ) )
	{
		list_set(
			list,
			feeder_row_exist_parse(
				input ) );
	}

	pclose( input_pipe );

	return list;
}

char *feeder_row_exist_system_string(
		const char *select,
		const char *table,
		char *where )
{
	char system_string[ 1024 ];

	if ( !where )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: where is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh \"%s\" %s \"%s\" feeder_date",
		select,
		table,
		where );

	return strdup( system_string );
}

JOURNAL *feeder_row_journal(
		char *fund_name,
		FEEDER_MATCHED_JOURNAL *feeder_matched_journal,
		FEEDER_PHRASE *feeder_phrase_seek,
		char *transaction_date_time )
{
	if ( !transaction_date_time )
	{
		char message[ 128 ];

		sprintf(message, "transaction_date_time is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( feeder_phrase_seek )
	{
		return
		journal_new(
			fund_name,
			feeder_phrase_seek->full_name,
			feeder_phrase_seek->contact_key,
			transaction_date_time,
			(char *)0 /* account_name */ );
	}
	else
	{
		return
		journal_new(
			fund_name,
			feeder_matched_journal->full_name,
			feeder_matched_journal->contact_key,
			transaction_date_time,
			(char *)0 /* account_name */ );
	}
}

char *feeder_row_phrase( FEEDER_PHRASE *feeder_phrase_seek )
{
	if ( !feeder_phrase_seek )
		return "";
	else
		return feeder_phrase_seek->phrase;
}

FILE *feeder_row_list_display_pipe( char *system_string )
{
	FILE *pipe;

	fflush( stdout );
	pipe = popen( system_string, "w" );
	fflush( stdout );
	return pipe;
}

char *feeder_row_list_display_system_string( void )
{
	char system_string[ 1024 ];
	char *heading;
	char *format;

	heading =
"Row,Account<br>Entity/Transaction,date,description,amount,file_balance,calculate_balance,difference,status";

	format = "right,left,left,left,right,right,right,right,left";

	sprintf(system_string,
		"html_table.e '' '%s' '^' %s",
		heading,
		format );

	return strdup( system_string );
}

void feeder_row_list_display(
		boolean reverse_order_boolean,
		LIST *feeder_row_list )
{
	FEEDER_ROW *feeder_row;
	FILE *display_pipe;
	char *system_string;

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_row_list_display_system_string();

	display_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		feeder_row_list_display_pipe(
			system_string );

	if ( !list_length( feeder_row_list ) )
	{
		pclose( display_pipe );
		return;
	}

	if ( reverse_order_boolean )
	{
		if ( list_go_last( feeder_row_list ) )
		do {
			feeder_row = list_get( feeder_row_list );
	
			if ( !display_pipe )
			{
				display_pipe =
					/* -------------- */
					/* Safely returns */
					/* -------------- */
					feeder_row_list_display_pipe(
						system_string );
			}
	
			display_pipe =
				/* ---------------------------- */
				/* Returns display_pipe or null */
				/* ---------------------------- */
				feeder_row_display_output(
					display_pipe,
					feeder_row );
	
		} while ( list_previous( feeder_row_list ) );
	}
	else
	{
		if ( list_rewind( feeder_row_list ) )
		do {
			feeder_row = list_get( feeder_row_list );
	
			if ( !display_pipe )
			{
				display_pipe =
					/* -------------- */
					/* Safely returns */
					/* -------------- */
					feeder_row_list_display_pipe(
						system_string );
			}
	
			display_pipe =
				/* ---------------------------- */
				/* Returns display_pipe or null */
				/* ---------------------------- */
				feeder_row_display_output(
					display_pipe,
					feeder_row );
	
		} while ( list_next( feeder_row_list ) );
	}

	if ( display_pipe ) pclose( display_pipe );

	fflush( stdout );
}

void feeder_row_transaction_insert(
		char *fund_name,
		boolean fund_boolean,
		boolean contact_key_boolean,
		LIST *feeder_row_list )
{
	LIST *transaction_list;

	if ( !list_length( feeder_row_list ) ) return;

	if ( ( transaction_list =
		feeder_row_extract_transaction_list(
			fund_name,
			feeder_row_list ) ) )
	{
		/* May reset transaction->transaction_date_time */
		/* -------------------------------------------- */
		transaction_list_insert(
			transaction_list,
			0 /* not insert_journal_list_boolean */ );

		transaction_journal_list_insert(
			fund_name,
			transaction_list,
			fund_boolean,
			contact_key_boolean,
			1 /* with_propagate */ );

		/* Set each feeder_row->transaction_date_time */
		/* ------------------------------------------ */
		feeder_row_transaction_date_time_set(
			feeder_row_list /* in/out */ );
	}
}

LIST *feeder_row_extract_transaction_list(
		char *fund_name,
		LIST *feeder_row_list )
{
	LIST *transaction_list = list_new();
	FEEDER_ROW *feeder_row;

	if ( list_rewind( feeder_row_list ) )
	do {
		feeder_row =
			list_get(
				feeder_row_list );

		if ( feeder_row->feeder_transaction
		&&   feeder_row->feeder_transaction->transaction )
		{
			feeder_row->
				feeder_transaction->
				transaction->
				fund_name =
					fund_name;

			list_set(
				transaction_list,
				feeder_row->
					feeder_transaction->
					transaction );
		}

	} while ( list_next( feeder_row_list ) );

	if ( !list_length( transaction_list ) )
	{
		list_free( transaction_list );
		transaction_list = NULL;
	}

	return transaction_list;
}

FILE *feeder_row_display_output(
		FILE *display_pipe,
		FEEDER_ROW *feeder_row )
{
	char *display_results;
	char *status_string;
	char feeder_row_calculate_balance_string[ 128 ];
	char feeder_load_row_file_row_balance_string[ 128 ];
	double difference;
	char difference_string[ 128 ];

	if ( !display_pipe
	||   !feeder_row
	||   !feeder_row->feeder_load_row )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		if ( display_pipe ) pclose( display_pipe );
		fflush( stdout );
		exit( 1 );
	}

	display_results =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_row_display_results(
			feeder_row->feeder_row_exist_seek,
			feeder_row->feeder_matched_journal,
			feeder_row->feeder_phrase_seek );

	strcpy(	feeder_row_calculate_balance_string,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		string_commas_money(
			feeder_row->calculate_balance ) );

	strcpy(	feeder_load_row_file_row_balance_string,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		string_commas_money(
			feeder_row->
				feeder_load_row->
				file_row_balance ) );

	difference =
		feeder_row->feeder_load_row->file_row_balance  -
		feeder_row->calculate_balance;

	strcpy(	difference_string,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		string_commas_money(
			difference ) );

	status_string =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		feeder_row_status_string(
			feeder_row->feeder_row_status );

	fprintf(display_pipe,
		"%d^%s^%s^%s^%s^%s^%s^%s^%s\n",
		feeder_row->feeder_row_number,
		display_results,
		feeder_row->feeder_load_row->international_date,
		feeder_row->feeder_load_row->description_embedded,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		string_commas_money(
			feeder_row->
				feeder_load_row->
				exchange_journal_amount ),
		feeder_load_row_file_row_balance_string,
		feeder_row_calculate_balance_string,
		difference_string,
		status_string );

	free( display_results );

	if ( feeder_row->feeder_transaction
	&&   feeder_row->feeder_transaction->transaction )
	{
		pclose( display_pipe );
		fflush( stdout );
		display_pipe = (FILE *)0;

		journal_list_html_display(
			feeder_row->
				feeder_transaction->
				transaction->
				journal_list,
			feeder_row->
				feeder_transaction->
				transaction->
				fund_name,
			feeder_row->
				feeder_transaction->
				transaction->
				transaction_date_time,
			feeder_row->
				feeder_transaction->
				transaction->
				memo,
			(char *)0 /* transaction_full_name */ );
	}

	return display_pipe;
}

char *feeder_row_status_string(
		enum feeder_row_status feeder_row_status )
{
	char *status_string = "";

	if ( feeder_row_status == feeder_row_status_okay )
		status_string = "Okay";
	else
	if ( feeder_row_status == feeder_row_status_out_of_balance )
		status_string = "Out";
	else
	if ( feeder_row_status == feeder_row_status_cannot_determine )
		status_string = "Unknown";

	return status_string;
}

char *feeder_row_display_results(
		FEEDER_ROW_EXIST *feeder_row_exist_seek,
		FEEDER_MATCHED_JOURNAL *feeder_matched_journal,
		FEEDER_PHRASE *feeder_phrase )
{
	char buffer[ STRING_64K ];

	if ( feeder_row_exist_seek )
	{
		snprintf(
			buffer,
			sizeof ( buffer ),
		"<p style=\"color:black\">Existing transaction:</p> %s/%s",
			feeder_row_exist_seek->file_row_description,
			feeder_row_exist_seek->transaction_date_time );
	}
	else
	if ( feeder_matched_journal )
	{
		if ( feeder_matched_journal->check_number )
		{
			snprintf(
				buffer,
				sizeof ( buffer ),
				"Matched check# %d/%s/%s; %s",
				feeder_matched_journal->check_number,
				feeder_matched_journal->full_name,
				feeder_matched_journal->transaction_date_time,
				feeder_matched_journal->
					check_update_statement );
		}
		else
		{
			snprintf(
				buffer,
				sizeof ( buffer ),
				"Matched amount: %.2lf/%s/%s",
				feeder_matched_journal->amount,
				feeder_matched_journal->full_name,
				feeder_matched_journal->transaction_date_time );
		}
	}
	else
	if ( feeder_phrase )
	{
		snprintf(
			buffer,
			sizeof ( buffer ),
			"Matched feeder phrase: %s<br>Entity: %s",
			feeder_phrase->phrase,
			feeder_phrase->full_name );
	}
	else
	{
		strcpy( buffer, "<p style=\"color:red\">No transaction</p>" );
	}

	return strdup( buffer );
}

void feeder_row_calculate_balance_set(
		LIST *feeder_row_list,
		double prior_account_end_balance )
{
	FEEDER_ROW *feeder_row;

	if ( list_rewind( feeder_row_list ) )
	do {
		feeder_row = list_get( feeder_row_list );

		if ( !feeder_row->feeder_load_row )
		{
			char message[ 128 ];

			sprintf(message,
				"feeder_row->feeder_load_row is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		feeder_row->calculate_balance =
			feeder_row_calculate_balance(
				feeder_row,
				prior_account_end_balance );

		prior_account_end_balance =
			feeder_row->
				calculate_balance;

	} while ( list_next( feeder_row_list ) );
}

double feeder_row_calculate_balance(
		FEEDER_ROW *feeder_row,
		double prior_account_end_balance )
{
	double calculate_balance;

	if ( feeder_row->feeder_row_exist_seek )
	{
		calculate_balance =
			feeder_row->
				feeder_row_exist_seek->
				file_row_balance;
	}
	else
	{
		calculate_balance =
			prior_account_end_balance +
			feeder_row->
				feeder_load_row->
				exchange_journal_amount;
	}

	return calculate_balance;
}

char *feeder_row_account_end_date( LIST *feeder_row_list )
{
	FEEDER_ROW *feeder_row;

	feeder_row =
		/* -------------------------------- */
		/* Returns the last element or null */
		/* -------------------------------- */
		list_last( feeder_row_list );

	if ( !feeder_row )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"list_last() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	if ( !feeder_row->feeder_load_row )
	{
		char message[ 128 ];

		sprintf(message,
			"feeder_row->feeder_load_row is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return feeder_row->feeder_load_row->international_date;
}

double feeder_row_account_end_balance( LIST *feeder_row_list )
{
	FEEDER_ROW *feeder_row;
	double account_end_balance = 0.0;

	if ( list_rewind( feeder_row_list ) )
	do {
		feeder_row = list_get( feeder_row_list );

		account_end_balance =
			feeder_row->
				calculate_balance;

	} while ( list_next( feeder_row_list ) );

	return account_end_balance;
}

void feeder_row_error_display(
		boolean reverse_order_boolean,
		LIST *feeder_row_list )
{
	feeder_row_list_display(
		reverse_order_boolean,
		feeder_row_error_extract_list(
			feeder_row_list )
			/* feeder_row_list */ );
}

LIST *feeder_row_error_extract_list( LIST *feeder_row_list )
{
	LIST *extract_list = {0};
	FEEDER_ROW *feeder_row;

	if ( list_rewind( feeder_row_list ) )
	do {
		feeder_row = list_get( feeder_row_list );

		if ( !feeder_row->feeder_row_exist_seek
		&&   !feeder_row->feeder_matched_journal
		&&   !feeder_row->feeder_phrase_seek )
		{
			if ( !extract_list ) extract_list = list_new();

			list_set( extract_list, feeder_row );
		}

	} while ( list_next( feeder_row_list ) );

	return extract_list;
}

LIST *feeder_row_list(
		char *fund_name,
		char *feeder_account_name,
		boolean reverse_order_boolean,
		boolean contact_key_boolean,
		char *financial_institution_full_name,
		char *financial_institution_contact_key,
		char *account_uncleared_checks_string,
		LIST *feeder_phrase_list,
		LIST *feeder_row_exist_list,
		LIST *feeder_matched_journal_list,
		LIST *feeder_load_row_list )
{
	LIST *list = list_new();
	FEEDER_LOAD_ROW *feeder_load_row;
	FEEDER_ROW *feeder_row;
	char *minimum_transaction_date_time = {0};

	if ( !feeder_account_name
	||   !financial_institution_full_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( feeder_load_row_list ) )
	do {
		feeder_load_row = list_get( feeder_load_row_list );

		feeder_row =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			feeder_row_new(
				fund_name,
				feeder_account_name,
				contact_key_boolean,
				financial_institution_full_name,
				financial_institution_contact_key,
				account_uncleared_checks_string,
				feeder_phrase_list,
				feeder_row_exist_list,
				feeder_matched_journal_list,
				feeder_load_row,
				minimum_transaction_date_time,
				feeder_row_feeder_row_number(
					reverse_order_boolean,
					list_length( feeder_load_row_list ) ) );

		if ( feeder_row->transaction_date_time )
		{
			/* Minimum transaction for each day */
			/* -------------------------------- */
			minimum_transaction_date_time =
				feeder_row->
					transaction_date_time;
		}

		list_set( list, feeder_row );

	} while ( list_next( feeder_load_row_list ) );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

FEEDER_ROW *feeder_row_new(
		char *fund_name,
		char *feeder_account_name,
		boolean contact_key_boolean,
		char *financial_institution_full_name,
		char *financial_institution_contact_key,
		char *account_uncleared_checks_string,
		LIST *feeder_phrase_list,
		LIST *feeder_row_exist_list,
		LIST *feeder_matched_journal_list,
		FEEDER_LOAD_ROW *feeder_load_row,
		char *minimum_transaction_date_time,
		int feeder_row_number )
{
	FEEDER_ROW *feeder_row;

	if ( !feeder_account_name
	||   !financial_institution_full_name
	||   !feeder_load_row
	||   !feeder_row_number )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	feeder_row = feeder_row_calloc();

	feeder_row->fund_name = fund_name;
	feeder_row->feeder_account_name = feeder_account_name;
	feeder_row->feeder_load_row = feeder_load_row;
	feeder_row->feeder_row_number = feeder_row_number;

	if ( list_length( feeder_row_exist_list ) )
	{
		feeder_row->feeder_row_exist_seek =
			feeder_row_exist_seek(
				feeder_load_row->international_date,
				feeder_load_row->description_embedded,
				feeder_row_exist_list );
	}

	if ( feeder_row->feeder_row_exist_seek )
	{
		return feeder_row;
	}

	if ( list_length( feeder_matched_journal_list ) )
	{
		if ( feeder_load_row->check_number )
		{
			feeder_row->feeder_matched_journal =
				/* ------------------- */
				/* Independent of fund */
				/* ------------------- */
				feeder_matched_journal_check_seek(
					feeder_account_name,
					contact_key_boolean,
					account_uncleared_checks_string,
					feeder_load_row->check_number,
					feeder_load_row->
						exchange_journal_amount,
					feeder_matched_journal_list );
		}

		if ( !feeder_row->feeder_matched_journal )
		{
			feeder_row->feeder_matched_journal =
				feeder_matched_journal_amount_seek(
					feeder_load_row->
						exchange_journal_amount,
					feeder_matched_journal_list );
		}

		if ( feeder_row->feeder_matched_journal )
		{
			feeder_row->feeder_matched_journal->taken = 1;
		}
	}

	if ( !feeder_row->feeder_matched_journal
	&&   list_length( feeder_phrase_list ) )
	{
		feeder_row->feeder_phrase_seek =
			feeder_phrase_seek(
				financial_institution_full_name,
				financial_institution_contact_key,
				feeder_load_row->description_space_trim,
				feeder_phrase_list );
	}

	if ( !feeder_row->feeder_matched_journal
	&&   !feeder_row->feeder_phrase_seek )
	{
		return feeder_row;
	}

	if ( feeder_row->feeder_phrase_seek )
	{
		feeder_row->transaction_date_time =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			feeder_row_transaction_date_time(
				feeder_load_row->international_date,
				minimum_transaction_date_time );

		feeder_row->feeder_transaction =
			feeder_transaction_new(
				fund_name,
				feeder_account_name,
				feeder_row->feeder_phrase_seek,
				feeder_load_row->exchange_journal_amount,
				feeder_row->transaction_date_time,
				feeder_load_row->description_embedded
					/* memo */ );
	}
	else
	if ( feeder_row->feeder_matched_journal )
	{
		feeder_row->transaction_date_time =
			feeder_row->
				feeder_matched_journal->
				transaction_date_time;
	}

	return feeder_row;
}

FEEDER_ROW *feeder_row_calloc( void )
{
	FEEDER_ROW *feeder_row;

	if ( ! ( feeder_row = calloc( 1, sizeof ( FEEDER_ROW ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return feeder_row;
}

int feeder_row_count( LIST *feeder_row_list )
{
	return list_length( feeder_row_list );
}

int feeder_row_insert_count( LIST *feeder_row_list )
{
	int insert_count = 0;
	FEEDER_ROW *feeder_row;

	if ( list_rewind( feeder_row_list ) )
	do {
		feeder_row = list_get( feeder_row_list );

		if ( feeder_row->feeder_matched_journal
		||   feeder_row->feeder_phrase_seek )
		{
			insert_count++;
		}
	} while ( list_next( feeder_row_list ) );

	return insert_count;
}

char *feeder_row_system_string(
		const char *table,
		char *select_string,
		char *where )
{
	char system_string[ 1024 ];

	if ( !select_string
	||   !where )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh \"%s\" %s \"%s\" feeder_row_number",
		select_string,
		table,
		where );

	return strdup( system_string );
}


FEEDER_ROW *feeder_row_parse(
		char *fund_name,
		boolean contact_key_boolean,
		char *input )
{
	char buffer[ 512 ];
	FEEDER_ROW *feeder_row;

	if ( !input || !*input ) return NULL;

	feeder_row = feeder_row_calloc();

	feeder_row->fund_name = fund_name;

	/* See entity_select_string() */
	/* -------------------------- */
	piece( buffer, SQL_DELIMITER, input, 0 );
	feeder_row->feeder_account_name = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 1 );
	feeder_row->feeder_load_date_time = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	feeder_row->feeder_row_number = atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer ) feeder_row->feeder_date = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	if ( *buffer ) feeder_row->full_name = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 5 );
	if ( *buffer ) feeder_row->transaction_date_time = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 6 );
	if ( *buffer ) feeder_row->file_row_description = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 7 );
	if ( *buffer ) feeder_row->file_row_amount = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 8 );
	if ( *buffer ) feeder_row->file_row_balance = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 9 );
	if ( *buffer ) feeder_row->calculate_balance = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 10 );
	if ( *buffer ) feeder_row->check_number = atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 11 );
	if ( *buffer ) feeder_row->feeder_phrase = strdup( buffer );

	if ( contact_key_boolean )
	{
		piece( buffer, SQL_DELIMITER, input, 12 );
		feeder_row->contact_key = strdup( buffer );
	}

	return feeder_row;
}

void feeder_row_check_journal_update(
		LIST *feeder_row_list )
{
	FILE *update_pipe;
	FEEDER_ROW *feeder_row;

	if ( !list_rewind( feeder_row_list ) ) return;

	update_pipe = feeder_row_check_journal_update_pipe();

	do {
		feeder_row = list_get( feeder_row_list );

		if ( feeder_row->feeder_matched_journal
		&&   feeder_row->
			feeder_matched_journal->
			check_update_statement )
		{
			fprintf(update_pipe,
				"%s\n",
				feeder_row->
					feeder_matched_journal->
					check_update_statement );
		}

	} while ( list_next( feeder_row_list ) );

	pclose( update_pipe );
}

FILE *feeder_row_check_journal_update_pipe( void )
{
	return popen( "sql", "w" );
}

char *feeder_row_maximum_transaction_date_time(
		const char *feeder_row_table,
		char *feeder_account_name,
		char *feeder_load_date_time )
{
	char system_string[ 1024 ];

	if ( !feeder_account_name )
	{
		char message[ 128 ];

		sprintf(message, "feeder_account_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !feeder_load_date_time ) return (char *)0;

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh \"%s\" %s \"%s\"",
		"max( transaction_date_time )",
		feeder_row_table,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		feeder_load_event_primary_where(
			feeder_account_name,
			feeder_load_date_time ) );

	return
	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	string_pipe_fetch( system_string );
}

char *feeder_row_minimum_transaction_date_time(
		const char *feeder_row_table,
		char *feeder_account_name,
		char *feeder_load_date_time )
{
	char system_string[ 1024 ];

	if ( !feeder_account_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !feeder_load_date_time ) return (char *)0;

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh \"%s\" %s \"%s\"",
		"min( transaction_date_time )",
		feeder_row_table,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		feeder_load_event_primary_where(
			feeder_account_name,
			feeder_load_date_time ) );

	return
	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	string_pipe_fetch( system_string );
}

char *feeder_row_latest_date_number_select( boolean reverse_order_boolean )
{
	if ( reverse_order_boolean )
		return "min( feeder_row_number )";
	else
		return "max( feeder_row_number )";
}

int feeder_row_latest_date_number(
		const char *feeder_row_table,
		char *feeder_account_name,
		boolean reverse_order_boolean,
		char *feeder_load_date_time )
{
	char *primary_where;
	char system_string[ 1024 ];

	if ( !feeder_account_name )
	{
		char message[ 128 ];

		sprintf(message, "feeder_account_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !feeder_load_date_time ) return 0;

	primary_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		feeder_load_event_primary_where(
			feeder_account_name,
			feeder_load_date_time );

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh \"%s\" %s \"%s\"",
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		feeder_row_latest_date_number_select( reverse_order_boolean),
		feeder_row_table,
		primary_where );

	return
	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	string_atoi( string_pipe_fetch( system_string ) );
}

FEEDER_ROW *feeder_row_fetch(
		char *fund_name,
		char *feeder_account_name,
		char *feeder_load_date_time,
		boolean contact_key_boolean,
		int feeder_row_number )
{
	char *select_string;
	char *primary_where;
	char *system_string;

	select_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		entity_select_string(
			FEEDER_ROW_SELECT /* ENTITY_SELECT */,
			ENTITY_CONTACT_KEY_COLUMN,
			contact_key_boolean );

	primary_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		feeder_row_primary_where(
			feeder_account_name,
			feeder_load_date_time,
			feeder_row_number );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_row_system_string(
			FEEDER_ROW_TABLE,
			select_string,
			primary_where );

	return
	feeder_row_parse(
		fund_name,
		contact_key_boolean,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		string_pipe_fetch( system_string ) );
}

char *feeder_row_primary_where(
		char *feeder_account_name,
		char *feeder_load_date_time,
		int feeder_row_number )
{
	static char where[ 128 ];

	if ( !feeder_account_name
	||   !feeder_load_date_time
	||   !feeder_row_number )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(where,
		"feeder_account = '%s' and		"
		"feeder_load_date_time = '%s' and	"
		"feeder_row_number = %d			",
		feeder_account_name,
		feeder_load_date_time,
		feeder_row_number );

	return where;
}

void feeder_row_journal_propagate(
		char *fund_name,
		char *feeder_account_name,
		char *feeder_load_date_time,
		char *account_uncleared_checks,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	char *minimum_transaction_date_time;
	JOURNAL_PROPAGATE *journal_propagate;

	if ( !feeder_account_name
	||   !feeder_load_date_time
	||   !account_uncleared_checks )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( ! ( minimum_transaction_date_time =
			feeder_row_minimum_transaction_date_time(
				FEEDER_ROW_TABLE,
				feeder_account_name,
				feeder_load_date_time ) ) )
	{
		return;
	}

	journal_propagate =
		journal_propagate_new(
			fund_name,
			minimum_transaction_date_time,
			feeder_account_name,
			fund_boolean,
			contact_key_boolean );

	if ( journal_propagate )
	{
		journal_list_update(
			journal_propagate->
				update_statement_list );
	}

	journal_propagate =
		journal_propagate_new(
			fund_name,
			minimum_transaction_date_time,
			account_uncleared_checks,
			fund_boolean,
			contact_key_boolean );

	if ( journal_propagate )
	{
		journal_list_update(
			journal_propagate->
				update_statement_list );
	}
}

void feeder_row_list_status_set( LIST *feeder_row_list )
{
	FEEDER_ROW *feeder_row;

	if ( list_rewind( feeder_row_list ) )
	do {
		feeder_row = list_get( feeder_row_list );

		if ( !feeder_row->feeder_load_row )
		{
			char message[ 128 ];

			sprintf(message,
				"feeder_row->feeder_load_row is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		feeder_row->feeder_row_status =
			feeder_row_status_evaluate(
				feeder_row->
					feeder_load_row->
					file_row_balance,
				feeder_row->calculate_balance );

	} while ( list_next( feeder_row_list ) );
}

enum feeder_row_status feeder_row_status_evaluate(
		double feeder_load_row_file_row_balance,
		double feeder_row_calculate_balance )
{
	enum feeder_row_status status;

	if ( float_money_virtually_same(
		feeder_load_row_file_row_balance,
		feeder_row_calculate_balance ) )
	{
		status = feeder_row_status_okay;
	}
	else
	{
		status = feeder_row_status_out_of_balance;
	}

	return status;
}

void feeder_row_transaction_date_time_set( LIST *feeder_row_list )
{
	FEEDER_ROW *feeder_row;

	if ( list_rewind( feeder_row_list ) )
	do {
		feeder_row = list_get( feeder_row_list );

		if ( !feeder_row->feeder_transaction ) continue;

		if (	feeder_row->
				feeder_transaction->
				transaction->
				transaction_date_time !=
			feeder_row->transaction_date_time )
		{
			feeder_row->transaction_date_time =
				feeder_row->
					feeder_transaction->
					transaction->
					transaction_date_time;
		}

	} while ( list_next( feeder_row_list ) );
}

void feeder_row_list_raw_display(
		FILE *stream,
		LIST *feeder_row_list )
{
	FEEDER_ROW *feeder_row;
	char *display;

	if ( !stream )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: stream is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( list_rewind( feeder_row_list ) )
	do {
		feeder_row = list_get( feeder_row_list );

		display =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			feeder_row_raw_display(
				feeder_row );

		fprintf(
			stream,
			"%s\n\n",
			display );

		free( display );

	} while ( list_next( feeder_row_list ) );
}

char *feeder_row_raw_display( FEEDER_ROW *feeder_row )
{
	char display[ 2048 ];
	char *ptr = display;

	if ( !feeder_row
	||   !feeder_row->feeder_load_row )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"feeder_row is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr += sprintf(
		ptr,
		"description_embedded=%s; ",
		feeder_row->feeder_load_row->description_embedded );

	ptr += sprintf(
		ptr,
		"row_number=%d; ",
		feeder_row->feeder_row_number );

	ptr += sprintf(
		ptr,
		"transaction_date_time=%s; ",
		feeder_row->transaction_date_time );

	ptr += sprintf(
		ptr,
		"feeder_row_exist_seek?=%d; ",
		(feeder_row->feeder_row_exist_seek)
			? 1 : 0 );

	ptr += sprintf(
		ptr,
		"feeder_matched_journal?=%d; ",
		(feeder_row->feeder_matched_journal)
			? 1 : 0 );

	ptr += sprintf(
		ptr,
		"feeder_phrase_seek?=%d; ",
		(feeder_row->feeder_phrase_seek)
			? 1 : 0 );

	ptr += sprintf(
		ptr,
		"feeder_transaction?=%d; ",
		(feeder_row->feeder_transaction)
			? 1 : 0 );

	ptr += sprintf(
		ptr,
		"feeder_load_row->exchange_journal_amount=%.2lf; ",
		feeder_row->feeder_load_row->exchange_journal_amount  );

	ptr += sprintf(
		ptr,
		"calculate_balance=%.2lf; ",
		feeder_row->calculate_balance );

	ptr += sprintf(
		ptr,
		"status=%s",
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		feeder_row_status_string(
			feeder_row->feeder_row_status ) );

	return strdup( display );
}

double feeder_row_exist_sum( LIST *feeder_row_list )
{
	FEEDER_ROW *feeder_row;
	double sum = 0.0;

	if ( list_rewind( feeder_row_list ) )
	do {
		feeder_row = list_get( feeder_row_list );

		if ( feeder_row->feeder_row_exist_seek )
		{
			sum +=
				feeder_row->
					feeder_row_exist_seek->
					file_row_amount;
		}

	} while ( list_next( feeder_row_list ) );

	return sum;
}

boolean feeder_row_list_status_out_of_balance_boolean( LIST *feeder_row_list )
{
	FEEDER_ROW *feeder_row;

	if ( list_rewind( feeder_row_list ) )
	do {
		feeder_row = list_get( feeder_row_list );

		if (	feeder_row->feeder_row_status ==
			feeder_row_status_out_of_balance )
		{
			return 1;
		}

	} while ( list_next( feeder_row_list ) );

	return 0;
}

boolean feeder_row_list_non_match_boolean( LIST *feeder_row_list )
{
	FEEDER_ROW *feeder_row;

	if ( list_rewind( feeder_row_list ) )
	do {
		feeder_row = list_get( feeder_row_list );

		if (	!feeder_row->feeder_row_exist_seek
		&&	!feeder_row->feeder_matched_journal
		&&	!feeder_row->feeder_phrase_seek )
		{
			return 1;
		}

	} while ( list_next( feeder_row_list ) );

	return 0;
}

int feeder_row_feeder_row_number(
		boolean reverse_order_boolean,
		int feeder_load_row_list_length )
{
	static int row_number;
	int return_row_number;

	row_number++;

	if ( reverse_order_boolean )
	{
		return_row_number =
			feeder_load_row_list_length -
			row_number +
			1;
	}
	else
	{
		return_row_number = row_number;
	}

	return return_row_number;
}

char *feeder_row_select_string(
		const char *feeder_row_select,
		const char *predictive_fund_column,
		const char *entity_contact_key_column,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean )
{
	OPTIONAL_COLUMN *optional_column;

	optional_column =
		optional_column_new(
			',' /* delimiter */,
			(char *)feeder_row_select /* base_string */,
			(char *)predictive_fund_column /* component */,
			0 /* not escape_boolean */,
			predictive_fund_boolean /* set_boolean */ );

	optional_column =
		optional_column_new(
			',' /* delimiter */,
			optional_column->return_string /* base_string */,
			(char *)entity_contact_key_column /* component */,
			0 /* not escape_boolean */,
			entity_contact_key_boolean /* set_boolean */ );

	return optional_column->return_string /* heap memory */;
}

