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
#include <ctype.h>
#include "String.h"
#include "piece.h"
#include "float.h"
#include "timlib.h"
#include "date_convert.h"
#include "sql.h"
#include "session.h"
#include "basename.h"
#include "sed.h"
#include "environ.h"
#include "application_constants.h"
#include "account.h"
#include "journal.h"
#include "feeder.h"

FILE *feeder_phrase_pipe_open( char *system_string )
{
	if ( !system_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: system_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return popen( system_string, "r" );
}

LIST *feeder_phrase_list(
			char *feeder_phrase_select,
			char *feeder_phrase_table )
{
	LIST *list;
	FILE *pipe_open;
	char input[ 1024 ];

	pipe_open =
		feeder_phrase_pipe_open(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			feeder_phrase_system_string(
				feeder_phrase_select,
				feeder_phrase_table ) );

	list = list_new();

	while ( string_input( input, pipe_open, 1024 ) )
	{
		list_set(
			list,
			feeder_phrase_parse( input ) );
	}

	pclose( pipe_open );

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
	char buffer[ 128 ];
	FEEDER_PHRASE *feeder_phrase;

	if ( !input || !*input ) return (FEEDER_PHRASE *)0;

	/* See FEEDER_PHRASE_SELECT */
	/* ------------------------ */
	piece( buffer, SQL_DELIMITER, input, 0 );
	feeder_phrase = feeder_phrase_new( strdup( buffer ) );

	if ( piece( buffer, SQL_DELIMITER, input, 1 ) )
	{
		feeder_phrase->nominal_account = strdup( buffer );
	}

	if ( piece( buffer, SQL_DELIMITER, input, 2 ) )
	{
		feeder_phrase->full_name = strdup( buffer );
	}

	if ( piece( buffer, SQL_DELIMITER, input, 3 ) )
	{
		feeder_phrase->street_address = strdup( buffer );
	}

	if ( piece( buffer, SQL_DELIMITER, input, 4 ) )
	{
		feeder_phrase->feeder_phrase_ignore = ( *buffer == 'y' );
	}

	return feeder_phrase;
}

FEEDER_PHRASE *feeder_phrase_new( char *phrase )
{
	FEEDER_PHRASE *feeder_phrase;

	if ( !phrase || !*phrase )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: phrase is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	feeder_phrase = feeder_phrase_calloc();
	feeder_phrase->phrase = phrase;

	return feeder_phrase;
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

char *feeder_load_row_trim_date( char *feeder_description )
{
	static char sans_bank_date_description[ 256 ];
	char *replace;
	char *regular_expression;
	SED *sed;

	regular_expression = "[ ][0-9][0-9]/[0-9][0-9]$";
	replace = "";

	sed = sed_new( regular_expression, replace );

	string_strcpy(	sans_bank_date_description,
			feeder_description,
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

	return string_rtrim( sans_bank_date_description );
}

FEEDER_LOAD_ROW *feeder_load_row_new(
			DATE *feeder_load_date /* in/out */,
			char *feeder_account,
			int date_column /* one_based */,
			int description_column /* one_based */,
			int debit_column /* one_based */,
			int credit_column /* one_based */,
			int balance_column /* one_based */,
			LIST *feeder_phrase_list,
			LIST *feeder_exist_row_list,
			LIST *feeder_matched_journal_list,
			char *input,
			int row_number )
{
	FEEDER_LOAD_ROW *feeder_load_row;
	char buffer[ 512 ];

	if ( !feeder_load_date )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: feeder_load_date is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !feeder_account )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: feeder_account is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !date_column )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: date_column is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !description_column )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: description_column is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !debit_column )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: debit_column is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !row_number )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: row_number is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !input || !*input ) return (FEEDER_LOAD_ROW *)0;

	feeder_load_row = feeder_load_row_calloc();

	feeder_load_row->row_number = row_number;

	feeder_load_row->american_date =
		strdup(
			piece_quote_comma(
				buffer,
				input,
				date_column - 1 ) );

	if ( ! ( feeder_load_row->international_date =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			feeder_load_row_international_date(
				feeder_load_row->american_date ) ) )
	{
		free( feeder_load_row->american_date );
		free( feeder_load_row );
		return (FEEDER_LOAD_ROW *)0;
	}

	feeder_load_row->feeder_description =
		string_strdup(
			piece_quote_comma(
				buffer,
				input,
				description_column - 1 ) );

	if ( !*feeder_load_row->feeder_description )
	{
		free( feeder_load_row->feeder_description );
		free( feeder_load_row->american_date );
		free( feeder_load_row );
		return (FEEDER_LOAD_ROW *)0;
	}

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

	if ( !feeder_load_row->amount )
	{
		free( feeder_load_row->feeder_description );
		free( feeder_load_row->american_date );
		free( feeder_load_row );
		return (FEEDER_LOAD_ROW *)0;
	}

	if ( balance_column )
	{
		feeder_load_row->balance =
			atof(
				piece_quote_comma(
					buffer,
					input,
					balance_column - 1 ) );

		if ( !feeder_load_row->balance )
		{
			fprintf(stderr,
			"Warning in %s/%s()/%d: balance is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

			return (FEEDER_LOAD_ROW *)0;
		}
	}

	feeder_load_row->check_number =
		feeder_load_row_check_number(
			feeder_load_row->feeder_description );

	feeder_load_row->description_embedded =
		/* ----------------------------------------- */
		/* Returns heap memory or feeder_description */
		/* ----------------------------------------- */
		feeder_load_row_description_embedded(
			feeder_load_row->feeder_description,
			feeder_load_row->balance,
			row_number,
			feeder_load_row->check_number );

	if ( list_length( feeder_exist_row_list ) )
	{
		feeder_load_row->feeder_exist_row_seek =
			feeder_exist_row_seek(
				feeder_load_row->international_date,
				feeder_load_row->description_embedded,
				feeder_exist_row_list );
	}

	if ( feeder_load_row->feeder_exist_row_seek )
	{
		return feeder_load_row;
	}

	if ( list_length( feeder_matched_journal_list ) )
	{
		feeder_load_row->feeder_matched_journal =
			feeder_matched_journal_check_seek(
				feeder_load_row->check_number,
				feeder_load_row->amount,
				feeder_matched_journal_list );

		if ( !feeder_load_row->feeder_matched_journal )
		{
			feeder_load_row->feeder_matched_journal =
				feeder_matched_journal_amount_seek(
					feeder_load_row->amount,
					feeder_matched_journal_list );
		}

		if ( feeder_load_row->feeder_matched_journal )
		{
			feeder_load_row->feeder_matched_journal->taken = 1;
		}
	}

	if ( !feeder_load_row->feeder_matched_journal )
	{
		feeder_load_row->feeder_phrase_seek =
			feeder_phrase_seek(
				feeder_load_row->feeder_description,
				feeder_phrase_list );
	}

	if ( feeder_load_row->feeder_phrase_seek )
	{
		feeder_load_row->transaction_date_time =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			feeder_load_row_transaction_date_time(
				feeder_load_row->international_date,
				date_hms( feeder_load_date ) );

		date_increment_seconds(
			feeder_load_date,
			1 );

		feeder_load_row->memo =
			feeder_load_row->description_embedded;

		feeder_load_row->feeder_transaction =
			feeder_transaction_new(
				feeder_account,
				feeder_load_row->feeder_phrase_seek,
				feeder_load_row->amount,
				feeder_load_row->transaction_date_time,
				feeder_load_row->memo );
	}
	else
	if ( feeder_load_row->feeder_matched_journal )
	{
		feeder_load_row->transaction_date_time =
			feeder_load_row->
				feeder_matched_journal->
				transaction_date_time;
	}

	return feeder_load_row;
}

FEEDER_LOAD_ROW *feeder_load_row_calloc( void )
{
	FEEDER_LOAD_ROW *feeder_load_row;

	if ( ! ( feeder_load_row =
			calloc( 1, sizeof( FEEDER_LOAD_ROW ) ) ) )
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
			char *feeder_description,
			double balance,
			int row_number,
			int check_number )
{
	if ( check_number ) return feeder_description;

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
				feeder_load_row_trim_date(
					feeder_description ) ),
			balance,
			row_number ),
		FEEDER_DESCRIPTION_SIZE );
}

char *feeder_load_row_description_build(
			char *sed_trim_double_spaces,
			double balance,
			int row_number )
{
	char build[ 512 ];

	if ( !sed_trim_double_spaces
	||   !row_number )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( balance )
	{
		sprintf(build,
			"%s %.2lf",
			sed_trim_double_spaces,
			balance );
	}
	else
	{
		sprintf(build,
			"%s %d",
			sed_trim_double_spaces,
			row_number );
	}

	return strdup( build );
}

char *feeder_load_row_description_crop(
			char *description_build,
			int feeder_description_size )
{
	if ( string_strlen( description_build ) > feeder_description_size )
	{
		*( description_build + feeder_description_size ) = '\0';
	}

	return description_build;
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

FILE *feeder_load_file_input_open(
			char *feeder_load_filename )
{
	if ( !feeder_load_filename )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: feeder_load_filename is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	fopen( feeder_load_filename, "r" );
}

FEEDER_LOAD_FILE *feeder_load_file_fetch(
			DATE *feeder_load_date /* in/out */,
			char *feeder_account,
			char *feeder_load_filename,
			int date_column /* one_based */,
			int description_column /* one_based */,
			int debit_column /* one_based */,
			int credit_column /* one_based */,
			int balance_column /* one_based */,
			boolean reverse_order_boolean,
			LIST *feeder_phrase_list,
			LIST *feeder_exist_row_list,
			LIST *feeder_matched_journal_list )
{
	FEEDER_LOAD_FILE *feeder_load_file;
	char input[ 1024 ];

	if ( !feeder_load_filename
	||   !date_column
	||   !description_column
	||   !debit_column )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	feeder_load_file = feeder_load_file_calloc();

	feeder_load_file->basename_filename =
		feeder_load_file_basename_filename(
			feeder_load_filename );

	if ( ! ( feeder_load_file->input_open =
			feeder_load_file_input_open(
				feeder_load_filename ) ) )
	{
		free( feeder_load_file );
		return (FEEDER_LOAD_FILE *)0;
	}

	feeder_load_file->feeder_load_row_list = list_new();

	while ( string_input( input, feeder_load_file->input_open, 1024 ) )
	{
		feeder_load_file->remove_character =
			remove_character( input, '\\' );

		if ( !feeder_load_file->remove_character
		||   !*feeder_load_file->remove_character )
		{
			++feeder_load_file->row_number;
			continue;
		}

		feeder_load_file->feeder_load_row =
			feeder_load_row_new(
				feeder_load_date /* in/out */,
				feeder_account,
				date_column,
				description_column,
				debit_column,
				credit_column,
				balance_column,
				feeder_phrase_list,
				feeder_exist_row_list,
				feeder_matched_journal_list,
				feeder_load_file->remove_character
					/* input */,
				++feeder_load_file->row_number );

		if ( !feeder_load_file->feeder_load_row ) continue;

		if ( reverse_order_boolean )
		{
			list_set_first(
				feeder_load_file->feeder_load_row_list,
				feeder_load_file->feeder_load_row );
		}
		else
		{
			list_set(
				feeder_load_file->feeder_load_row_list,
				feeder_load_file->feeder_load_row );
		}
	}

	fclose( feeder_load_file->input_open );

	return feeder_load_file;
}

FEEDER_LOAD_FILE *feeder_load_file_calloc( void )
{
	FEEDER_LOAD_FILE *feeder_load_file;

	if ( ! ( feeder_load_file = calloc( 1, sizeof( FEEDER_LOAD_FILE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return feeder_load_file;
}

FEEDER_LOAD_EVENT *feeder_load_event_new(
			char *feeder_load_date_string,
			char *feeder_account,
			char *login_name,
			char *basename_filename,
			char *account_end_date,
			double account_end_balance )
{
	FEEDER_LOAD_EVENT *feeder_load_event;

	if ( !feeder_load_date_string
	||   !feeder_account
	||   !login_name
	||   !basename_filename
	||   !account_end_date )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	feeder_load_event = feeder_load_event_calloc();

	feeder_load_event->feeder_load_date_string = feeder_load_date_string;
	feeder_load_event->feeder_account = feeder_account;
	feeder_load_event->login_name = login_name;
	feeder_load_event->basename_filename = basename_filename;
	feeder_load_event->account_end_date = account_end_date;
	feeder_load_event->account_end_balance = account_end_balance;

	return feeder_load_event;
}

FEEDER_LOAD_EVENT *feeder_load_event_calloc( void )
{
	FEEDER_LOAD_EVENT *feeder_load_event;

	if ( ! ( feeder_load_event =
			calloc( 1, sizeof( FEEDER_LOAD_EVENT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return feeder_load_event;
}

void feeder_load_event_insert(
			char *feeder_load_event_table,
			char *feeder_load_event_insert,
			char *feeder_load_date_string,
			char *feeder_account )
{
	FILE *insert_open;

	if ( !feeder_load_event_table
	||   !feeder_load_event_insert
	||   !feeder_load_date_string
	||   !feeder_account )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	insert_open =
		feeder_load_event_insert_open(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			feeder_load_event_insert_system_string(
				feeder_load_event_table,
				feeder_load_event_insert,
				SQL_DELIMITER ) );

	feeder_load_event_insert_pipe(
		insert_open,
		SQL_DELIMITER,
		feeder_load_date_string,
		feeder_account );

	pclose( insert_open );
}

FILE *feeder_load_event_insert_open(
			char *system_string )
{
	if ( !system_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: system_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	popen( system_string, "w" );
}

char *feeder_load_event_insert_system_string(
			char *feeder_load_event_table,
			char *feeder_load_event_insert,
			char sql_delimiter )
{
	char system_string[ 1024 ];

	if ( !feeder_load_event_table
	||   !feeder_load_event_insert
	||   !sql_delimiter )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
	 	"insert_statement table=%s field=%s del='%c' 		  |"
	 	"sql 2>&1						  |"
		"grep -vi duplicate					  |"
	 	"html_paragraph_wrapper.e				   ",
	 	feeder_load_event_table,
	 	feeder_load_event_insert,
	 	sql_delimiter );

	return strdup( system_string );
}

void feeder_load_event_insert_pipe(
			FILE *insert_open,
			char sql_delimiter,
			char *feeder_load_date_string,
			char *feeder_account )
{
	if ( !insert_open
	||   !sql_delimiter
	||   !feeder_load_date_string
	||   !feeder_account )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		if ( insert_open ) pclose( insert_open );

		exit( 1 );
	}

	fprintf(insert_open,
		"%s%c%s\n",
	 	feeder_load_date_string,
		sql_delimiter,
		feeder_account );
}

void feeder_load_event_update(
			char *feeder_load_event_table,
			char *feeder_load_event_primary_key,
			char *feeder_load_date_string,
			char *feeder_account,
			char *login_name,
			char *basename_filename,
			char *account_end_date,
			double account_end_balance  )
{
	FILE *update_open;

	if ( !feeder_load_event_table
	||   !feeder_load_event_primary_key
	||   !feeder_load_date_string
	||   !feeder_account
	||   !login_name
	||   !basename_filename
	||   !account_end_date )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	update_open =
		feeder_load_event_update_open(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			feeder_load_event_update_system_string(
				feeder_load_event_table,
				feeder_load_event_primary_key ) );

	feeder_load_event_update_pipe(
		update_open,
		feeder_load_date_string,
		feeder_account,
		login_name,
		basename_filename,
		account_end_date,
		account_end_balance );

	pclose( update_open );
}

char *feeder_load_event_update_system_string(
			char *table,
			char *primary_key )
{
	char system_string[ 1024 ];

	if ( !table || !primary_key )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"update_statement.e table=%s key=%s carrot=y | sql",
		table,
		primary_key );

	return strdup( system_string );
}

FILE *feeder_load_event_update_open(
			char *system_string )
{
	return popen( system_string, "w" );
}

void feeder_load_event_update_pipe(
			FILE *update_open,
			char *feeder_load_date_string,
			char *feeder_account,
			char *login_name,
			char *basename_filename,
			char *account_end_date,
			double account_end_balance )
{
	if ( !update_open
	||   !feeder_load_date_string
	||   !feeder_account
	||   !login_name
	||   !basename_filename
	||   !account_end_date )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		if ( update_open ) pclose( update_open );

		exit( 1 );
	}

	fprintf(update_open,
		"%s^%s^login_name^%s\n",
		feeder_load_date_string,
		feeder_account,
		login_name );

	fprintf(update_open,
		"%s^%s^feeder_load_filename^%s\n",
		feeder_load_date_string,
		feeder_account,
		basename_filename );

	fprintf(update_open,
		"%s^%s^account_end_date^%s\n",
		feeder_load_date_string,
		feeder_account,
		account_end_date );

	fprintf(update_open,
		"%s^%s^account_end_balance^%.2lf\n",
		feeder_load_date_string,
		feeder_account,
		account_end_balance );
}

char *feeder_load_file_basename_filename( char *feeder_load_filename )
{
	char filename[ 256 ];
	int session_characters_to_trim;
	char *start_trimming_here;

	string_strcpy(
		filename,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		basename_filename( feeder_load_filename ),
		256 );

	/* Trim off _$SESSION */
	/* ------------------ */
	session_characters_to_trim = SESSION_MAX_SESSION_SIZE + 1;

	start_trimming_here =
		filename + strlen( filename ) - session_characters_to_trim;

	*start_trimming_here = '\0';

	return strdup( filename );
}

FEEDER *feeder_fetch(
			char *login_name,
			char *feeder_account,
			char *feeder_load_filename,
			int date_column /* one_based */,
			int description_column /* one_based */,
			int debit_column /* one_based */,
			int credit_column /* one_based */,
			int balance_column /* one_based */,
			boolean reverse_order_boolean,
			double account_end_balance )
{
	FEEDER *feeder;

	if ( !login_name
	||   !feeder_account
	||   strcmp( feeder_account, "feeder_account" ) == 0
	||   !feeder_load_filename
	||   !date_column
	||   !description_column
	||   !debit_column )
	{
		return (FEEDER *)0;
	}

	feeder = feeder_calloc();

	feeder->feeder_phrase_list =
		feeder_phrase_list(
			FEEDER_PHRASE_SELECT,
			FEEDER_PHRASE_TABLE );

	if ( ! ( feeder->feeder_load_file_minimum_date =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			feeder_load_file_minimum_date(
				feeder_load_filename,
				date_column /* one_based */ ) ) )
	{
		free( feeder );
		return (FEEDER *)0;
	}

	feeder->feeder_exist_row_list =
		feeder_exist_row_list(
			feeder_account,
			feeder->feeder_load_file_minimum_date );

	feeder->feeder_matched_journal_list =
		feeder_matched_journal_list(
			feeder_account,
			FEEDER_LOAD_ROW_TABLE,
			feeder->feeder_load_file_minimum_date,
			account_uncleared_checks(
				ACCOUNT_UNCLEARED_CHECKS_KEY ) );

	feeder->feeder_load_date = date_now_new( date_utc_offset() );

	feeder->feeder_load_date_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		date_yyyy_mm_dd(
			feeder->feeder_load_date );

	feeder->feeder_load_file =
		feeder_load_file_fetch(
			feeder->feeder_load_date /* in/out */,
			feeder_account,
			feeder_load_filename,
			date_column,
			description_column,
			debit_column,
			credit_column,
			balance_column,
			reverse_order_boolean,
			feeder->feeder_phrase_list,
			feeder->feeder_exist_row_list,
			feeder->feeder_matched_journal_list );

	if ( !feeder->feeder_load_file
	||   !list_length(
		feeder->
			feeder_load_file->
			feeder_load_row_list ) )
	{
		return (FEEDER *)0;
	}

	feeder->feeder_load_row_prior_row_balance =
		feeder_load_row_prior_row_balance(
			feeder_account,
			account_end_balance,
			feeder->feeder_load_file->feeder_load_row_list );

	if ( !balance_column )
	{
		feeder_load_row_balance_set(
			feeder->feeder_load_file->feeder_load_row_list,
			feeder->feeder_load_row_prior_row_balance );
	}

	feeder->feeder_load_row_first_out_balance =
		feeder_load_row_first_out_balance(
			feeder->feeder_load_row_prior_row_balance,
			feeder->feeder_load_file->feeder_load_row_list,
			account_end_balance );

	feeder->account_end_date =
		/* ---------------------------------------- */
		/* Returns this->international_date or null */
		/* ---------------------------------------- */
		feeder_account_end_date(
			(FEEDER_LOAD_ROW *)list_last(
			feeder->feeder_load_file->feeder_load_row_list )
				/* last_feeder_load_row */ );

	feeder->account_end_balance =
		/* ----------------------------- */
		/* Returns this->balance or null */
		/* ----------------------------- */
		feeder_account_end_balance(
			(FEEDER_LOAD_ROW *)list_last(
			feeder->feeder_load_file->feeder_load_row_list )
				/* last_feeder_load_row */ );

	feeder->feeder_load_event =
		feeder_load_event_new(
			feeder->feeder_load_date_string,
			feeder_account,
			login_name,
			feeder->feeder_load_file->basename_filename,
			feeder->account_end_date,
			feeder->account_end_balance );

	return feeder;
}

FEEDER *feeder_calloc( void )
{
	FEEDER *feeder;

	if ( ! ( feeder = calloc( 1, sizeof( FEEDER ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return feeder;
}

char *feeder_load_file_sha256sum( char *feeder_load_filename )
{
	if ( !feeder_load_filename )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: feeder_load_filename is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	timlib_sha256sum( feeder_load_filename );
}

FEEDER_PHRASE *feeder_phrase_seek(
			char *feeder_description,
			LIST *feeder_phrase_list )
{
	FEEDER_PHRASE *feeder_phrase;
	char feeder_component[ 128 ];
	int piece_number;

	if ( !list_rewind( feeder_phrase_list ) ) return (FEEDER_PHRASE *)0;

	do {
		feeder_phrase = list_get( feeder_phrase_list );

		if ( feeder_phrase->feeder_phrase_ignore ) continue;


		for(	piece_number = 0;
			piece(	feeder_component,
				timlib_delimiter( feeder_phrase->phrase ),
				feeder_phrase->phrase,
				piece_number );
			piece_number++ )
		{
			if ( string_exists_substr(
				feeder_description /* string */,
				feeder_component /* substring */ ) )
			{
				return feeder_phrase;
			}
		}

	} while ( list_next( feeder_phrase_list ) );

	return (FEEDER_PHRASE *)0;
}

char *feeder_exist_row_where(
			char *feeder_account,
			char *feeder_load_file_minimum_date )
{
	static char where[ 128 ];

	if ( !feeder_account
	||   strcmp( feeder_account, "feeder_account" ) == 0
	||   !feeder_load_file_minimum_date )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(where,
		"feeder_account = '%s' and "
		"feeder_date >= '%s'",
		feeder_account,
		feeder_load_file_minimum_date );

	return where;
}

FEEDER_EXIST_ROW *feeder_exist_row_parse( char *input )
{
	FEEDER_EXIST_ROW *feeder_exist_row;
	char buffer[ 128 ];

	if ( !input || !*input ) return (FEEDER_EXIST_ROW *)0;

	feeder_exist_row = feeder_exist_row_calloc();

	/* See FEEDER_EXIST_ROW_SELECT */
	/* --------------------------- */
	piece( buffer, SQL_DELIMITER, input, 0 );
	feeder_exist_row->feeder_date = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 1 );
	feeder_exist_row->feeder_description = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	feeder_exist_row->transaction_date_time = strdup( buffer );

	return feeder_exist_row;
}

FEEDER_EXIST_ROW *feeder_exist_row_calloc( void )
{
	FEEDER_EXIST_ROW *feeder_exist_row;

	if ( ! ( feeder_exist_row = calloc( 1, sizeof( FEEDER_EXIST_ROW ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return feeder_exist_row;
}

FEEDER_EXIST_ROW *feeder_exist_row_seek(
			char *international_date,
			char *description_embedded,
			LIST *feeder_exist_row_list )
{
	FEEDER_EXIST_ROW *feeder_exist_row;

	if ( !international_date
	||   !description_embedded )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( feeder_exist_row_list ) )
		return (FEEDER_EXIST_ROW *)0;

	do {
		feeder_exist_row =
			list_get(
				feeder_exist_row_list );

		if ( strcmp(	feeder_exist_row->feeder_date,
				international_date ) == 0
		&&   strcmp(	feeder_exist_row->feeder_description,
				description_embedded ) == 0 )
		{
			return feeder_exist_row;
		}

	} while ( list_next( feeder_exist_row_list ) );

	return (FEEDER_EXIST_ROW *)0;
}

FEEDER_TRANSACTION *feeder_transaction_calloc( void )
{
	FEEDER_TRANSACTION *feeder_transaction;

	if ( ! ( feeder_transaction =
			calloc( 1, sizeof( FEEDER_TRANSACTION ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return feeder_transaction;
}

LIST *feeder_matched_journal_list(
			char *feeder_account,
			char *feeder_load_row_table,
			char *feeder_load_file_minimum_date,
			char *account_uncleared_checks )
{
	LIST *list;
	char *subquery;
	char *where;
	LIST *system_list;
	JOURNAL *journal;

	if ( !feeder_account
	||   !feeder_load_row_table
	||   !feeder_load_file_minimum_date
	||   !account_uncleared_checks )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	subquery =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_matched_journal_subquery(
			feeder_account,
			account_uncleared_checks,
			JOURNAL_TABLE,
			feeder_load_row_table,
			feeder_load_file_minimum_date );

	where =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_matched_journal_where(
			feeder_account,
			account_uncleared_checks,
			subquery,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			feeder_matched_journal_minimum_date(
				feeder_load_file_minimum_date,
				feeder_matched_journal_days_ago(
				       FEEDER_LOAD_TRANSACTION_DAYS_AGO ) ) );

	system_list =
		journal_system_list(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			journal_system_string(
				JOURNAL_SELECT,
				JOURNAL_TABLE,
				where ),
		0 /* not fetch_account */,
		0 /* not fetch_subclassification */,
		0 /* not fetch_element */,
		1 /* fetch_transaction */ );

	if ( !list_rewind( system_list ) ) return (LIST *)0;

	list = list_new();

	do {
		journal = list_get( system_list );

		list_set(
			list,
			feeder_matched_journal_new(
				journal ) );

	} while ( list_next( system_list ) );

	return list;
}

FEEDER_MATCHED_JOURNAL *feeder_matched_journal_calloc( void )
{
	FEEDER_MATCHED_JOURNAL *feeder_matched_journal;

	if ( ! ( feeder_matched_journal =
			calloc(	1,
				sizeof( FEEDER_MATCHED_JOURNAL ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return feeder_matched_journal;
}

char *feeder_matched_journal_subquery(
			char *feeder_account,
			char *account_uncleared_checks,
			char *journal_table,
			char *feeder_load_row_table,
			char *feeder_load_file_minimum_date )
{
	char subquery[ 1024 ];

	sprintf(subquery,
		"not exists						"
		"(select 1 from %s					"
		"	where %s.full_name =				"
		"	      %s.full_name and				"
		"	      %s.street_address =			"
		"	      %s.street_address and			"
		"	      %s.transaction_date_time =		"
		"	      %s.transaction_date_time and		"
		"	      %s.feeder_account in ('%s','%s') and	"
		"	      feeder_date >= '%s')			",
		feeder_load_row_table,
		feeder_load_row_table,
		journal_table,
		feeder_load_row_table,
		journal_table,
		feeder_load_row_table,
		journal_table,
		feeder_load_row_table,
		feeder_account,
		account_uncleared_checks,
		feeder_load_file_minimum_date );

	return strdup( subquery );
}

char *feeder_matched_journal_where(
			char *feeder_account,
			char *account_uncleared_checks,
			char *subquery,
			char *minimum_date )
{
	char where[ 2048 ];

	if ( !feeder_account
	||   !account_uncleared_checks
	||   !subquery
	||   !minimum_date )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(where,
		"account in ('%s','%s') and		"
		"transaction_date_time >= '%s' and	"
		"%s					",
		feeder_account,
		account_uncleared_checks,
		minimum_date,
		subquery );

	return strdup( where );
}

int feeder_load_row_check_number( char *feeder_description )
{
	char buffer[ 512 ];

	if ( !feeder_description )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: feeder_description is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	string_strcpy( buffer, feeder_description, 512 );

	if ( *buffer == '#' && isdigit( * ( buffer + 1 ) ) )
	{
		return atoi( buffer + 1 );
	}
	else
	if ( string_strncmp( buffer, "check" ) == 0 )
	{
		search_replace_string( buffer, "check", "" );
		return atoi( buffer );
	}

	return 0;
}

void feeder_load_row_list_insert(
			char *feeder_account,
			char *feeder_load_date_string,
			LIST *feeder_load_row_list )
{
	FILE *insert_open;
	FEEDER_LOAD_ROW *feeder_load_row;
	JOURNAL *journal;

	if ( !list_rewind( feeder_load_row_list ) ) return;

	insert_open =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		feeder_load_row_list_insert_open(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			feeder_load_row_list_insert_system_string(
				FEEDER_LOAD_ROW_INSERT_COLUMNS,
				FEEDER_LOAD_ROW_TABLE,
				SQL_DELIMITER ) );

	do {
		feeder_load_row =
			list_get(
				feeder_load_row_list );

		if ( feeder_load_row->
			feeder_exist_row_seek )
		{
			continue;
		}

		journal =
			/* Always succeeds */
			/* --------------- */
			feeder_load_row_journal(
				feeder_load_row->
					feeder_phrase_seek,
				feeder_load_row->
					transaction_date_time,
				feeder_load_row->
					feeder_matched_journal );

		feeder_load_row_insert_pipe(
			insert_open,
			journal->full_name,
			journal->street_address,
			journal->transaction_date_time,
			feeder_account,
			feeder_load_row->international_date,
			feeder_load_row->description_embedded,
			feeder_load_row->amount,
			feeder_load_row->balance,
			feeder_load_row->row_number,
			/* --------------- */
			/* Always succeeds */
			/* --------------- */
			feeder_load_row_phrase(
				feeder_load_row->feeder_phrase_seek ),
			feeder_load_date_string,
			SQL_DELIMITER );

	} while ( list_next( feeder_load_row_list ) );

	pclose( insert_open );
}

char *feeder_load_row_list_insert_system_string(
			char *feeder_load_row_insert_columns,
			char *feeder_load_row_table,
			char sql_delimiter )
{
	char system_string[ 1024 ];

	if ( !feeder_load_row_insert_columns
	||   !feeder_load_row_table )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
	 	"insert_statement table=%s field=%s del='%c' 	|"
		"tee_appaserver_error.sh			|"
	 	"sql 2>&1					|"
	 	"html_paragraph_wrapper.e			 ",
	 	feeder_load_row_table,
	 	feeder_load_row_insert_columns,
	 	sql_delimiter );

	return strdup( system_string );
}

FILE *feeder_load_file_row_list_insert_open(
			char *insert_system_string )
{
	if ( !insert_system_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return popen( insert_system_string, "w" );
}

void feeder_load_row_insert_pipe(
			FILE *insert_open,
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *feeder_account,
			char *international_date,
			char *description_embedded,
			double amount,
			double balance,
			int row_number,
			char *phrase,
			char *feeder_load_date_string,
			char sql_delimiter )
{
	if ( !insert_open
	||   !full_name
	||   !street_address
	||   !transaction_date_time
	||   !feeder_account
	||   !international_date
	||   !description_embedded
	||   !amount
	||   !row_number
	||   !phrase
	||   !feeder_load_date_string
	||   !sql_delimiter )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		if ( insert_open ) pclose( insert_open );

		exit( 1 );
	}

	fprintf(insert_open,
		"%s%c%s%c%s%c%s%c%s%c%s%c%.2lf%c%.2lf%c%d%c%s%c%s\n",
	 	full_name,
		sql_delimiter,
	 	street_address,
		sql_delimiter,
		transaction_date_time,
		sql_delimiter,
		feeder_account,
		sql_delimiter,
		international_date,
		sql_delimiter,
		description_embedded,
		sql_delimiter,
		amount,
		sql_delimiter,
		balance,
		sql_delimiter,
		row_number,
		sql_delimiter,
		phrase,
		sql_delimiter,
		feeder_load_date_string );
}

char *feeder_load_row_transaction_date_time(
			char *international_date,
			char *date_hms )
{
	char transaction_date_time[ 32 ];

	if ( !international_date
	||   !date_hms )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(transaction_date_time,
		"%s %s",
		international_date,
		date_hms );

	return strdup( transaction_date_time );
}

FILE *feeder_exist_row_input_open(
			char *feeder_exist_row_system_string )
{
	if ( !feeder_exist_row_system_string )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: feeder_exist_row_system_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	popen( feeder_exist_row_system_string, "r" );
}

LIST *feeder_exist_row_list(
			char *feeder_account,
			char *feeder_load_file_minimum_date )
{
	LIST *list;
	FILE *input_open;
	char input[ 1024 ];

	if ( !feeder_account
	||   !feeder_load_file_minimum_date )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	list = list_new();

	input_open =
		feeder_exist_row_input_open(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			feeder_exist_row_system_string(
				FEEDER_EXIST_ROW_SELECT,
				FEEDER_LOAD_ROW_TABLE,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				feeder_exist_row_where(
					feeder_account,
					feeder_load_file_minimum_date ) ) );

	while ( string_input( input, input_open, 1024 ) )
	{
		list_set(
			list,
			feeder_exist_row_parse( input ) );
	}

	pclose( input_open );

	return list;
}

char *feeder_exist_row_system_string(
			char *select,
			char *table,
			char *where )
{
	char system_string[ 1024 ];

	if ( !select
	||   !table
	||   !where )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\" feeder_date",
		select,
		table,
		where );

	return strdup( system_string );
}

boolean feeder_load_event_sha256sum_exists(
			char *feeder_load_event_table,
			char *feeder_load_file_sha256sum )
{
	char where[ 128 ];
	char system_string[ 1024 ];

	sprintf(where,
		"file_sha256sum = '%s'",
		feeder_load_file_sha256sum );

	sprintf(system_string,
		"select.sh \"count(1)\" %s \"%s\"",
		feeder_load_event_table,
		where );

	return (boolean)atoi( pipe2string( system_string ) );
}

char *feeder_load_file_minimum_date(
			char *feeder_load_filename,
			int date_column )
{
	FILE *file;
	char input[ 1024 ];
	char buffer[ 128 ];
	char *american_date;
	char *international_date;
	char *minimum_date = {0};
	
	if ( ! ( file = fopen( feeder_load_filename, "r" ) ) )
	{
		return (char *)0;
	}

	while ( string_input( input, file, 1024 ) )
	{
		american_date =
			piece_quote_comma(
				buffer,
				input,
				date_column - 1 );

		if ( american_date )
		{
			if ( ( international_date =
				/* --------------------------- */
				/* Returns heap memory or null */
				/* --------------------------- */
				feeder_load_row_international_date(
					american_date ) ) )
			{
				if ( !minimum_date )
				{
					minimum_date = international_date;
				}
				else
				if ( international_date < minimum_date )
				{
					minimum_date = international_date;
				}
			}
		}
	}

	fclose( file );

	return minimum_date;
}

JOURNAL *feeder_load_row_journal(
			FEEDER_PHRASE *feeder_phrase_seek,
			char *transaction_date_time,
			FEEDER_MATCHED_JOURNAL *feeder_matched_journal )
{
	if ( !feeder_phrase_seek
	&&   !feeder_matched_journal )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: both parameters are empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !transaction_date_time )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: transaction_date_time is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( feeder_phrase_seek )
	{
		return
		journal_new(
			feeder_phrase_seek->full_name,
			feeder_phrase_seek->street_address,
			transaction_date_time,
			(char *)0 /* account_name */ );
	}
	else
	{
		return
		journal_new(
			feeder_matched_journal->full_name,
			feeder_matched_journal->street_address,
			transaction_date_time,
			(char *)0 /* account_name */ );
	}
}

char *feeder_load_row_phrase(
			FEEDER_PHRASE *feeder_phrase_seek )
{
	if ( !feeder_phrase_seek )
		return "";
	else
		return feeder_phrase_seek->phrase;
}

FILE *feeder_load_row_list_display_pipe(
			char *system_string )
{
	return popen( system_string, "w" );
}

char *feeder_load_row_list_display_system_string( void )
{
	char system_string[ 1024 ];
	char *heading;

	heading = "Row,Account<br>Entity/Transaction,date,description,amount";

	sprintf(system_string,
		"html_table.e '' '%s' '^' right,left,left,left,right",
		heading );

	return strdup( system_string );
}

boolean feeder_load_row_list_display(
			LIST *feeder_load_row_list,
			FEEDER_LOAD_ROW *feeder_load_row_first_out_balance )
{
	FEEDER_LOAD_ROW *feeder_load_row;
	FILE *display_pipe;
	char *system_string;

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_load_row_list_display_system_string();

	display_pipe =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		feeder_load_row_list_display_pipe(
			system_string );

	if ( !list_rewind( feeder_load_row_list ) )
	{
		pclose( display_pipe );
		return 0;
	}

	do {
		feeder_load_row = list_get( feeder_load_row_list );

		if ( feeder_load_row == feeder_load_row_first_out_balance )
		{
			fflush( stdout );
			printf( "%s\n",
				/* ---------------------- */
				/* Returns program memory */
				/* ---------------------- */
				feeder_load_row_no_more() );
			fflush( stdout );
		}

		display_pipe =
			/* ---------------------------- */
			/* Returns display_pipe or null */
			/* ---------------------------- */
			feeder_load_row_display_output(
				display_pipe,
				feeder_load_row );

		if ( !display_pipe
		&&   list_still_more( feeder_load_row_list ) )
		{
			display_pipe =
				/* --------------- */
				/* Always succeeds */
				/* --------------- */
				feeder_load_row_list_display_pipe(
					system_string );
		}

	} while ( list_next( feeder_load_row_list ) );

	if ( display_pipe ) pclose( display_pipe );

	return (boolean)list_length( feeder_load_row_list );
}

void feeder_load_row_transaction_insert(
			LIST *feeder_load_row_list,
			char *appaserver_error_filename )
{
	LIST *transaction_list;

	if ( !appaserver_error_filename )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: appaserver_error_filename is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( feeder_load_row_list ) ) return;

	if ( ( transaction_list =
			feeder_load_row_extract_transaction_list(
				feeder_load_row_list ) ) )
	{
		transaction_list_insert(
			transaction_list,
			appaserver_error_filename,
			1 /* insert_journal_list_boolean */ );
	}
}

LIST *feeder_load_row_extract_transaction_list(
			LIST *feeder_load_row_list )
{
	LIST *transaction_list;
	FEEDER_LOAD_ROW *feeder_load_row;

	if ( !list_rewind( feeder_load_row_list ) )
	{
		return (LIST *)0;
	}

	transaction_list = list_new();

	do {
		feeder_load_row =
			list_get(
				feeder_load_row_list );

		if ( feeder_load_row->feeder_transaction
		&&   feeder_load_row->feeder_transaction->transaction )
		{
			list_set(
				transaction_list,
				feeder_load_row->
					feeder_transaction->
					transaction );
		}

	} while ( list_next( feeder_load_row_list ) );

	if ( !list_length( transaction_list ) )
	{
		return (LIST *)0;
	}
	else
	{
		return transaction_list;
	}
}

FILE *feeder_load_row_display_output(
			FILE *display_pipe,
			FEEDER_LOAD_ROW *feeder_load_row )
{
	char *tmp;

	if ( !display_pipe
	||   !feeder_load_row )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		if ( display_pipe ) pclose( display_pipe );
		exit( 1 );
	}

	fprintf(display_pipe,
		"%d^%s^%s^%s^%.2lf\n",
		feeder_load_row->row_number,
		( tmp =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			feeder_load_row_display_results(
				feeder_load_row->feeder_exist_row_seek,
				feeder_load_row->feeder_matched_journal,
				feeder_load_row->feeder_phrase_seek ) ),
		feeder_load_row->american_date,
		feeder_load_row->description_embedded,
		feeder_load_row->amount );

	free( tmp );

	if ( feeder_load_row->feeder_transaction
	&&   feeder_load_row->feeder_transaction->transaction )
	{
		pclose( display_pipe );
		display_pipe = (FILE *)0;

		journal_list_html_display(
			feeder_load_row->
				feeder_transaction->
				transaction->
				journal_list,
			feeder_load_row->
				feeder_transaction->
				transaction->
				memo );
	}

	return display_pipe;
}

char *feeder_load_row_display_results(
			FEEDER_EXIST_ROW *feeder_exist_row_seek,
			FEEDER_MATCHED_JOURNAL *feeder_matched_journal,
			FEEDER_PHRASE *feeder_phrase )
{
	char buffer[ 256 ];

	if ( feeder_exist_row_seek )
	{
		sprintf(buffer,
			"Existing transaction: %s/%s",
			feeder_exist_row_seek->feeder_description,
			feeder_exist_row_seek->transaction_date_time );
	}
	else
	if ( feeder_matched_journal )
	{
		if ( feeder_matched_journal->check_number )
		{
			sprintf(buffer,
				"Matched check# %d/%s/%s",
				feeder_matched_journal->check_number,
				feeder_matched_journal->full_name,
				feeder_matched_journal->transaction_date_time );
		}
		else
		{
			sprintf(buffer,
				"Matched amount: %.2lf/%s/%s",
				feeder_matched_journal->amount,
				feeder_matched_journal->full_name,
				feeder_matched_journal->transaction_date_time );
		}
	}
	else
	if ( feeder_phrase )
	{
		sprintf(buffer,
			"Matched feeder phrase: %s/%s",
			feeder_phrase->phrase,
			feeder_phrase->full_name );
	}
	else
	{
		strcpy( buffer, "<p style=\"color:red\">No transaction</p>" );
	}

	return strdup( buffer );
}

FEEDER_MATCHED_JOURNAL *
	feeder_matched_journal_check_seek(
			int check_number,
			double amount,
			LIST *feeder_matched_journal_list )
{
	FEEDER_MATCHED_JOURNAL *feeder_matched_journal;

	if ( !amount )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: amount is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( feeder_matched_journal_list )
	||   !check_number )
	{
		return (FEEDER_MATCHED_JOURNAL *)0;
	}

	do {
		feeder_matched_journal =
			list_get(
				feeder_matched_journal_list );

		if ( feeder_matched_journal->taken ) continue;

		if ( feeder_matched_journal->check_number == check_number
		&&   feeder_matched_journal->amount == amount )
		{
			return feeder_matched_journal;
		}

	} while ( list_next( feeder_matched_journal_list ) );

	return (FEEDER_MATCHED_JOURNAL *)0;
}

FEEDER_MATCHED_JOURNAL *
	feeder_matched_journal_amount_seek(
			double amount,
			LIST *feeder_matched_journal_list )
{
	FEEDER_MATCHED_JOURNAL *feeder_matched_journal;

	if ( !amount )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: amount is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( feeder_matched_journal_list ) )
		return (FEEDER_MATCHED_JOURNAL *)0;

	do {
		feeder_matched_journal =
			list_get(
				feeder_matched_journal_list );

		if ( feeder_matched_journal->taken ) continue;

		if ( feeder_matched_journal->amount == amount )
		{
			return feeder_matched_journal;
		}

	} while ( list_next( feeder_matched_journal_list ) );

	return (FEEDER_MATCHED_JOURNAL *)0;
}

FEEDER_TRANSACTION *feeder_transaction_new(
			char *feeder_account,
			FEEDER_PHRASE *feeder_phrase_seek,
			double amount,
			char *transaction_date_time,
			char *memo )
{
	FEEDER_TRANSACTION *feeder_transaction;

	if ( !feeder_account
	||   !feeder_phrase_seek
	||   !amount
	||   !transaction_date_time
	||   !memo )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	feeder_transaction = feeder_transaction_calloc();

	if ( amount < 0.0 )
	{
		feeder_transaction->debit_account =
			feeder_phrase_seek->
				nominal_account;

		feeder_transaction->credit_account = feeder_account;
	}
	else
	{
		feeder_transaction->debit_account = feeder_account;

		feeder_transaction->credit_account =
			feeder_phrase_seek->
				nominal_account;
	}

	feeder_transaction->transaction =
		transaction_binary(
			feeder_phrase_seek->full_name,
			feeder_phrase_seek->street_address,
			transaction_date_time,
			amount /* transaction_amount */,
			memo,
			feeder_transaction->debit_account
				/* debit_account_name */,
			feeder_transaction->credit_account
				/* credit_account_name */ );

	if ( !feeder_transaction->transaction )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: transaction_binary(%s/%.2lf) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			transaction_date_time,
			amount );
		exit( 1 );
	}

	return feeder_transaction;
}

double feeder_load_row_prior_row_balance(
			char *feeder_account,
			double account_end_balance,
			LIST *feeder_load_row_list )
{
	FEEDER_LOAD_EVENT *feeder_load_event;

	if ( ( feeder_load_event =
		feeder_load_event_latest_fetch(
			feeder_account ) ) )
	{
		return feeder_load_event->account_end_balance;
	}

	return
		account_end_balance -
		feeder_load_row_list_sum_amount(
			feeder_load_row_list );
}

void feeder_load_row_balance_set(
			LIST *feeder_load_row_list,
			double prior_row_balance )
{
	FEEDER_LOAD_ROW *feeder_load_row;

	if ( !list_rewind( feeder_load_row_list ) ) return;

	do {
		feeder_load_row = list_get( feeder_load_row_list );

		if ( !feeder_load_row->feeder_exist_row_seek
		&&   !feeder_load_row->feeder_matched_journal
		&&   !feeder_load_row->feeder_phrase_seek )
		{
			return;
		}

		feeder_load_row->balance =
			prior_row_balance +
			feeder_load_row->amount;

		prior_row_balance = feeder_load_row->balance;

	} while ( list_next( feeder_load_row_list ) );
}

FEEDER_LOAD_ROW *feeder_load_row_first_out_balance(
			double prior_row_balance,
			LIST *feeder_load_row_list,
			double account_end_balance )
{
	FEEDER_LOAD_ROW *feeder_load_row;
	FEEDER_LOAD_ROW *first_feeder_load_row = {0};

	if ( !list_rewind( feeder_load_row_list ) )
		return (FEEDER_LOAD_ROW *)0;

	do {
		feeder_load_row = list_get( feeder_load_row_list );

		if ( !first_feeder_load_row )
		{
			first_feeder_load_row = feeder_load_row;
		}

		if ( feeder_load_row->feeder_exist_row_seek ) continue;

		if ( !feeder_load_row->feeder_matched_journal
		&&   !feeder_load_row->feeder_phrase_seek )
		{
			return feeder_load_row;
		}

		if ( !double_virtually_same_places(
			feeder_load_row->balance,
			prior_row_balance + feeder_load_row->amount,
			2 ) )
		{
			return feeder_load_row;
		}

		prior_row_balance = feeder_load_row->balance;

	} while ( list_next( feeder_load_row_list ) );

	if ( account_end_balance )
	{
		if ( !double_virtually_same_places(
			account_end_balance,
			prior_row_balance,
			2 ) )
		{
			return first_feeder_load_row;
		}
	}

	return (FEEDER_LOAD_ROW *)0;
}

double feeder_load_ending_balance(
			FEEDER_LOAD_ROW *last_feeder_load_row )
{
	if ( !last_feeder_load_row )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: last_feeder_load_row is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return last_feeder_load_row->balance;
}

double feeder_load_row_list_sum_amount(
			LIST *feeder_load_row_list )
{
	FEEDER_LOAD_ROW *feeder_load_row;
	double sum_amount;

	if ( !list_rewind( feeder_load_row_list ) ) return 0.0;

	sum_amount = 0.0;

	do {
		feeder_load_row = list_get( feeder_load_row_list );
		sum_amount += feeder_load_row->amount;

	} while ( list_next( feeder_load_row_list ) );

	return sum_amount;
}

FILE *feeder_load_row_list_insert_open( char *system_string )
{
	if ( !system_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: system_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return popen( system_string, "w" );
}

FEEDER_MATCHED_JOURNAL *feeder_matched_journal_new( JOURNAL *journal )
{
	FEEDER_MATCHED_JOURNAL *feeder_matched_journal;

	if ( !journal )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: journal is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	feeder_matched_journal = feeder_matched_journal_calloc();

	feeder_matched_journal->amount =
		feeder_matched_journal_amount(
			journal->debit_amount,
			journal->credit_amount );

	feeder_matched_journal->full_name = journal->full_name;
	feeder_matched_journal->street_address = journal->street_address;

	feeder_matched_journal->transaction_date_time =
		journal->transaction_date_time;

	feeder_matched_journal->account_name = journal->account_name;
	feeder_matched_journal->debit_amount = journal->debit_amount;
	feeder_matched_journal->credit_amount = journal->credit_amount;

	feeder_matched_journal->check_number =
		journal->transaction->check_number;

	return feeder_matched_journal;
}

double feeder_matched_journal_amount(
			double debit_amount,
			double credit_amount )
{
	if ( debit_amount )
		return debit_amount;
	else
		return 0.0 - credit_amount;
}

char *feeder_account_end_date( FEEDER_LOAD_ROW *last_feeder_load_row )
{
	if ( !last_feeder_load_row )
		return (char *)0;
	else
		return last_feeder_load_row->international_date;
}

double feeder_account_end_balance(
			FEEDER_LOAD_ROW *last_feeder_load_row )
{
	if ( !last_feeder_load_row )
		return 0.0;
	else
		return last_feeder_load_row->balance;
}

int feeder_matched_journal_days_ago(
			char *feeder_load_transaction_days_ago )
{
	char *days_ago_string;

	if ( !feeder_load_transaction_days_ago )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	days_ago_string =
		application_constants_quick_fetch(
			environment_application_name(),
			feeder_load_transaction_days_ago );

	if ( !days_ago_string )
		return 6;
	else
		return atoi( days_ago_string );
}

char *feeder_matched_journal_minimum_date(
			char *feeder_load_file_minimum_date,
			int feeder_match_journal_days_ago )
{
	DATE *date;

	if ( !feeder_load_file_minimum_date
	||   !feeder_match_journal_days_ago )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ! ( date =
			date_yyyy_mm_dd_new(
				feeder_load_file_minimum_date ) ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: date_yyyy_mm_dd_new(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			feeder_load_file_minimum_date );
		exit( 1 );
	}

	date =
		date_decrement_days(
			date,
			(double)feeder_match_journal_days_ago );

	/* Returns heap memory or "" */
	/* ------------------------- */
	return
	date_display_yyyy_mm_dd( date );
}

boolean feeder_load_row_error_display(
			LIST *feeder_load_row_list )
{
	return
	feeder_load_row_list_display(
		feeder_load_row_error_extract_list(
			feeder_load_row_list ),
		(FEEDER_LOAD_ROW *)0 );
}

LIST *feeder_load_row_error_extract_list(
			LIST *feeder_load_row_list )
{
	LIST *extract_list = {0};
	FEEDER_LOAD_ROW *feeder_load_row;

	if ( !list_rewind( feeder_load_row_list ) )
		return (LIST *)0;

	do {
		feeder_load_row =
			list_get(
				feeder_load_row_list );

		if ( !feeder_load_row->feeder_exist_row_seek
		&&   !feeder_load_row->feeder_matched_journal
		&&   !feeder_load_row->feeder_phrase_seek )
		{
			if ( !extract_list ) extract_list = list_new();

			list_set(
				extract_list,
				feeder_load_row );
		}
	} while ( list_next( feeder_load_row_list ) );

	return extract_list;
}

char *feeder_load_row_no_more( void )
{
	return
	"<h1>^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^</h1>\n"
	"<h1>Execute will stop loading here.</h1>\n";
}

char *feeder_matched_journal_not_taken_system_string( void )
{
	char system_string[ 1024 ];
	char *heading;

	heading = "Name,Transaction date/time,debit,credit";

	sprintf(system_string,
		"html_table.e '%s' '%s' '^' left,left,right,right",
		"Journal Not Matched",
		heading );

	return strdup( system_string );
}

void feeder_matched_journal_not_taken_display(
			LIST *feeder_matched_journal_list )
{
	FILE *output_pipe;
	FEEDER_MATCHED_JOURNAL *feeder_matched_journal;

	if ( !list_rewind( feeder_matched_journal_list ) ) return;

	output_pipe =
		popen(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			feeder_matched_journal_not_taken_system_string(),
			"w" );

	do {
		feeder_matched_journal =
			list_get(
				feeder_matched_journal_list );

		if ( feeder_matched_journal->taken ) continue;

		fprintf(output_pipe,
			"%s^%s^%s^%s\n",
			feeder_matched_journal->full_name,
			feeder_matched_journal->transaction_date_time,
			(feeder_matched_journal->debit_amount)
				? /* --------------------- */
				  /* Returns static memory */
				  /* --------------------- */
				  timlib_place_commas_in_money(
					feeder_matched_journal->debit_amount )
				: "",
			(feeder_matched_journal->credit_amount)
				? /* --------------------- */
				  /* Returns static memory */
				  /* --------------------- */
				  timlib_place_commas_in_money(
					feeder_matched_journal->credit_amount )
				: "" );

	} while ( list_next( feeder_matched_journal_list ) );

	pclose( output_pipe );
}

FEEDER_LOAD_EVENT *feeder_load_event_fetch(
			char *feeder_load_date_string,
			char *feeder_account )
{
	FEEDER_LOAD_EVENT *feeder_load_event;
	FILE *input_open;
	char input[ 1024 ];

	if ( !feeder_load_date_string
	||   !feeder_account )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	input_open =
		feeder_load_event_input_open(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			feeder_load_event_system_string(
				FEEDER_LOAD_EVENT_SELECT,
				FEEDER_LOAD_EVENT_TABLE,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				feeder_load_event_primary_where(
					feeder_load_date_string,
					feeder_account ) ) );

	feeder_load_event =
		feeder_load_event_parse(
			string_input(
				input,
				input_open,
				1024 ) );

	pclose( input_open );

	return feeder_load_event;
}

char *feeder_load_event_primary_where(
			char *feeder_load_date_string,
			char *feeder_account )
{
	static char where[ 128 ];

	if ( !feeder_load_date_string
	||   !feeder_account )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(where,
		"feeder_load_date = '%s' and "
		"feeder_account = '%s'",
		feeder_load_date_string,
		feeder_account );

	return where;
}

char *feeder_load_event_system_string(
			char *feeder_load_event_select,
			char *feeder_load_event_table,
			char *feeder_load_event_primary_where )
{
	char system_string[ 1024 ];

	if ( !feeder_load_event_select
	||   !feeder_load_event_table
	||   !feeder_load_event_primary_where )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		feeder_load_event_select,
		feeder_load_event_table,
		feeder_load_event_primary_where );

	return strdup( system_string );
}

FILE *feeder_load_event_input_open(
			char *feeder_load_event_system_string )
{
	if ( !feeder_load_event_system_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	popen( feeder_load_event_system_string, "r" );
}

FEEDER_LOAD_EVENT *feeder_load_event_parse( char *input )
{
	char buffer[ 128 ];
	FEEDER_LOAD_EVENT *feeder_load_event;

	if ( !input || !*input ) return (FEEDER_LOAD_EVENT *)0;

	feeder_load_event = feeder_load_event_calloc();

	/* See FEEDER_LOAD_EVENT_SELECT */
	/* ---------------------------- */
	piece( buffer, SQL_DELIMITER, input, 0 );
	feeder_load_event->feeder_load_date_string = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 1 );
	feeder_load_event->feeder_account = strdup( buffer );

	if ( piece( buffer, SQL_DELIMITER, input, 2 ) )
	{
		feeder_load_event->login_name = strdup( buffer );
	}

	if ( piece( buffer, SQL_DELIMITER, input, 3 ) )
	{
		feeder_load_event->basename_filename = strdup( buffer );
	}

	if ( piece( buffer, SQL_DELIMITER, input, 4 ) )
	{
		feeder_load_event->account_end_date = strdup( buffer );
	}

	if ( piece( buffer, SQL_DELIMITER, input, 5 ) )
	{
		feeder_load_event->account_end_balance = atof( buffer );
	}

	return feeder_load_event;
}

FEEDER_LOAD_EVENT *feeder_load_event_latest_fetch(
			char *feeder_account )
{
	char *latest_load_date_string;

	if ( ! ( latest_load_date_string =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			feeder_load_event_latest_load_date_string(
				feeder_load_event_latest_system_string(
					FEEDER_LOAD_EVENT_TABLE,
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					feeder_load_event_account_where(
						feeder_account ) ) ) ) )
	{
		return (FEEDER_LOAD_EVENT *)0;
	}


	return
	feeder_load_event_fetch(
		latest_load_date_string,
		feeder_account );
}

char *feeder_load_event_account_where(
			char *feeder_account )
{
	static char where[ 128 ];

	if ( !feeder_account )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: feeder_account is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(where,
		"feeder_account = '%s'",
		feeder_account );

	return where;
}

char *feeder_load_event_latest_system_string(
			char *table,
			char *where )
{
	char system_string[ 1024 ];

	if ( !table
	||   !where )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		"max( feeder_load_date )",
		table,
		where );

	return strdup( system_string );
}

char *feeder_load_event_latest_load_date_string(
			char *feeder_load_event_latest_system_string )
{
	if ( !feeder_load_event_latest_system_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	string_pipe_fetch(
		feeder_load_event_latest_system_string );
}

