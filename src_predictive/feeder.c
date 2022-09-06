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

char *feeder_load_row_trim_date( char *file_row_description )
{
	static char sans_bank_date_description[ 256 ];
	char *replace;
	char *regular_expression;
	SED *sed;

	regular_expression = "[ ][0-9][0-9]/[0-9][0-9]$";
	replace = "";

	sed = sed_new( regular_expression, replace );

	string_strcpy(	sans_bank_date_description,
			file_row_description,
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
			int date_column /* one_based */,
			int description_column /* one_based */,
			int debit_column /* one_based */,
			int credit_column /* one_based */,
			int balance_column /* one_based */,
			int reference_column /* one_based */,
			char *input,
			int row_number )
{
	FEEDER_LOAD_ROW *feeder_load_row;
	char buffer[ 512 ];

	if ( !date_column )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: date_column is zero.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !description_column )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: description_column is zero.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !debit_column )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: debit_column is zero.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !row_number )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: row_number is zero.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !input || !*input ) return (FEEDER_LOAD_ROW *)0;

	feeder_load_row = feeder_load_row_calloc();

	feeder_load_row->row_number = row_number;

	feeder_load_row->american_date =
		string_strdup(
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

	feeder_load_row->file_row_description =
		string_strdup(
			piece_quote_comma(
				buffer,
				input,
				description_column - 1 ) );

	if ( !*feeder_load_row->file_row_description )
	{
		free( feeder_load_row->file_row_description );
		free( feeder_load_row->american_date );
		free( feeder_load_row->international_date );
		free( feeder_load_row );
		return (FEEDER_LOAD_ROW *)0;
	}

	if ( !credit_column )
	{
		feeder_load_row->file_row_amount =
			string_atof(
				piece_quote_comma(
					buffer,
					input,
					debit_column - 1 ) );
	}
	else
	{
		feeder_load_row->debit =
			string_atof(
				piece_quote_comma(
					buffer,
					input,
					debit_column - 1 ) );

		feeder_load_row->credit =
			string_atof(
				piece_quote_comma(
					buffer,
					input,
					credit_column - 1 ) );

		feeder_load_row->file_row_amount =
			feeder_load_row_amount(
				feeder_load_row->debit,
				feeder_load_row->credit );
	}

	if ( !feeder_load_row->file_row_amount )
	{
		free( feeder_load_row->file_row_description );
		free( feeder_load_row->american_date );
		free( feeder_load_row->international_date );
		free( feeder_load_row );
		return (FEEDER_LOAD_ROW *)0;
	}

	if ( balance_column )
	{
		feeder_load_row->file_row_balance =
			string_atof(
				piece_quote_comma(
					buffer,
					input,
					balance_column - 1 ) );
	}

	if ( reference_column )
	{
		feeder_load_row->reference_string =
			string_strdup(
				piece_quote_comma(
					buffer,
					input,
					reference_column - 1 ) );
	}

	feeder_load_row->check_number =
		feeder_load_row_check_number(
			feeder_load_row->file_row_description );

	feeder_load_row->description_embedded =
		/* ------------------------------------------- */
		/* Returns heap memory or file_row_description */
		/* ------------------------------------------- */
		feeder_load_row_description_embedded(
			feeder_load_row->file_row_description,
			feeder_load_row->file_row_balance,
			feeder_load_row->reference_string,
			feeder_load_row->check_number );

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
			char *file_row_description,
			double balance,
			char *reference_string,
			int check_number )
{
	if ( check_number ) return file_row_description;

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
					file_row_description ) ),
			balance,
			reference_string ),
		FEEDER_DESCRIPTION_SIZE );
}

char *feeder_load_row_description_build(
			char *sed_trim_double_spaces,
			double balance,
			char *reference_string )
{
	char build[ 512 ];

	if ( !sed_trim_double_spaces )
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
	if ( reference_string )
	{
		sprintf(build,
			"%s %s",
			sed_trim_double_spaces,
			reference_string );
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

	if ( !american_date || !*american_date ) return (char *)0;

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
			char *feeder_load_filename,
			int date_column /* one_based */,
			int description_column /* one_based */,
			int debit_column /* one_based */,
			int credit_column /* one_based */,
			int balance_column /* one_based */,
			int reference_column /* one_based */,
			boolean reverse_order_boolean )
{
	FEEDER_LOAD_FILE *feeder_load_file;
	char input[ 1024 ];
	FEEDER_LOAD_ROW *feeder_load_row;

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
		fprintf(stderr,
"ERROR in %s/%s()/%d: feeder_load_file_input_open(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			feeder_load_filename );
		exit( 1 );
	}


	feeder_load_file->feeder_load_row_list = list_new();

	while ( string_input( input, feeder_load_file->input_open, 1024 ) )
	{
		if ( !*input )
		{
			++feeder_load_file->row_number;
			continue;
		}

		feeder_load_file->remove_character =
			remove_character( input, '\\' );

		feeder_load_row =
			feeder_load_row_new(
				date_column,
				description_column,
				debit_column,
				credit_column,
				balance_column,
				reference_column,
				feeder_load_file->remove_character
					/* input */,
				++feeder_load_file->row_number );

		if ( !feeder_load_row ) continue;

		if ( reverse_order_boolean )
		{
			list_set_first(
				feeder_load_file->feeder_load_row_list,
				feeder_load_row );
		}
		else
		{
			list_set(
				feeder_load_file->feeder_load_row_list,
				feeder_load_row );
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
			char *feeder_account,
			char *feeder_load_date_time,
			char *login_name,
			char *basename_filename,
			char *account_end_date,
			double account_end_calculate_balance )
{
	FEEDER_LOAD_EVENT *feeder_load_event;

	if ( !feeder_account
	||   !feeder_load_date_time
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

	feeder_load_event->feeder_account = feeder_account;
	feeder_load_event->feeder_load_date_time = feeder_load_date_time;
	feeder_load_event->login_name = login_name;
	feeder_load_event->basename_filename = basename_filename;
	feeder_load_event->account_end_date = account_end_date;

	feeder_load_event->account_end_calculate_balance =
		account_end_calculate_balance;

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
			char *feeder_account,
			char *feeder_load_date_time,
			char *login_name,
			char *basename_filename,
			char *account_end_date,
			double account_end_calculate_balance )
{
	FILE *insert_open;

	if ( !feeder_load_event_table
	||   !feeder_load_event_insert
	||   !feeder_account
	||   !feeder_load_date_time
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
		feeder_account,
		feeder_load_date_time,
		login_name,
		basename_filename,
		account_end_date,
		account_end_calculate_balance );

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
			char *feeder_account,
			char *feeder_load_date_time,
			char *login_name,
			char *basename_filename,
			char *account_end_date,
			double account_end_calculate_balance )
{
	if ( !insert_open
	||   !sql_delimiter
	||   !feeder_account
	||   !feeder_load_date_time
	||   !login_name
	||   !basename_filename
	||   !account_end_date )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		if ( insert_open ) pclose( insert_open );

		exit( 1 );
	}

	/* See FEEDER_LOAD_EVENT_INSERT */
	/* ---------------------------- */
	fprintf(insert_open,
		"%s%c%s%c%s%c%s%c%s%c%.2lf\n",
		feeder_account,
		sql_delimiter ,
	 	feeder_load_date_time,
		sql_delimiter,
		login_name,
		sql_delimiter,
		basename_filename,
		sql_delimiter,
		account_end_date,
		sql_delimiter,
		account_end_calculate_balance );
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
			int reference_column /* one_based */,
			boolean reverse_order_boolean,
			double parameter_account_end_balance )
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
			FEEDER_ROW_TABLE,
			feeder->feeder_load_file_minimum_date,
			account_uncleared_checks(
				ACCOUNT_UNCLEARED_CHECKS_KEY ) );

	feeder->feeder_load_file =
		feeder_load_file_fetch(
			feeder_load_filename,
			date_column,
			description_column,
			debit_column,
			credit_column,
			balance_column,
			reference_column,
			reverse_order_boolean );

	if ( !feeder->feeder_load_file
	||   !list_length(
		feeder->
			feeder_load_file->
			feeder_load_row_list ) )
	{
		return (FEEDER *)0;
	}

	feeder->feeder_row_list =
		feeder_row_list(
			feeder_account,
			feeder->feeder_phrase_list,
			feeder->feeder_exist_row_list,
			feeder->feeder_matched_journal_list,
			feeder->feeder_load_file->feeder_load_row_list );

	feeder->feeder_row_first_out_balance =
		feeder_row_first_out_balance(
			feeder->feeder_row_list );

	feeder->account_end_balance =
		feeder_account_end_balance(
			feeder_account,
			balance_column,
			parameter_account_end_balance,
			feeder->feeder_row_list,
			feeder->feeder_row_first_out_balance );

	feeder->prior_account_end_balance =
		feeder_prior_account_end_balance(
			feeder_account,
			feeder->feeder_load_file_minimum_date,
			balance_column,
			feeder->account_end_balance,
			feeder->feeder_row_list,
			feeder->feeder_row_first_out_balance );

	feeder_row_list_calculate_balance_set(
		feeder->feeder_row_list,
		feeder->prior_account_end_balance,
		feeder->feeder_row_first_out_balance );

	feeder->account_end_date =
		/* ------------------------------------------------------- */
		/* Returns feeder_row->feeder_load_row->international_date */
		/* or null.						   */
		/* ------------------------------------------------------- */
		feeder_account_end_date(
			feeder->feeder_row_list,
			feeder->feeder_row_first_out_balance );

	if ( feeder->account_end_date )
	{
		feeder->account_end_calculate_balance =
			feeder_account_end_calculate_balance(
				feeder->feeder_row_list,
				feeder->feeder_row_first_out_balance );

		feeder->feeder_load_date_time =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			date_now16( date_utc_offset() );

		feeder->feeder_load_event =
			feeder_load_event_new(
				feeder_account,
				feeder->feeder_load_date_time,
				login_name,
				feeder->feeder_load_file->basename_filename,
				feeder->account_end_date,
				feeder->account_end_calculate_balance );
	}

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
			char *file_row_description,
			LIST *feeder_phrase_list )
{
	FEEDER_PHRASE *feeder_phrase;
	char feeder_component[ 128 ];
	int piece_number;

	if ( !list_rewind( feeder_phrase_list ) ) return (FEEDER_PHRASE *)0;

	do {
		feeder_phrase = list_get( feeder_phrase_list );

		for(	piece_number = 0;
			piece(	feeder_component,
				feeder_phrase_delimiter(
					feeder_phrase->phrase ),
				feeder_phrase->phrase,
				piece_number );
			piece_number++ )
		{
			if ( string_exists_substr(
				file_row_description /* string */,
				feeder_component /* substring */ ) )
			{
				return feeder_phrase;
			}
		}

	} while ( list_next( feeder_phrase_list ) );

	return (FEEDER_PHRASE *)0;
}

char feeder_phrase_delimiter( char *phrase )
{
	if ( timlib_exists_character( phrase, '|' ) )
		return '|';
	else
	if ( timlib_exists_character( phrase, ',' ) )
		return ',';
	else
	if ( timlib_exists_character( phrase, ';' ) )
		return ';';
	else
		return 0;
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
	feeder_exist_row->file_row_description = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	feeder_exist_row->transaction_date_time = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	feeder_exist_row->file_row_balance = atof( buffer );

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
		&&   strcmp(	feeder_exist_row->file_row_description,
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
			char *feeder_row_table,
			char *feeder_load_file_minimum_date,
			char *account_uncleared_checks )
{
	LIST *list;
	char *subquery;
	char *where;
	LIST *system_list;
	JOURNAL *journal;

	if ( !feeder_account
	||   !feeder_row_table
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
			feeder_row_table );

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
			char *feeder_row_table )
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
		"	      %s.feeder_account in ('%s','%s'))		",
		feeder_row_table,
		feeder_row_table,
		journal_table,
		feeder_row_table,
		journal_table,
		feeder_row_table,
		journal_table,
		feeder_row_table,
		feeder_account,
		account_uncleared_checks );

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

int feeder_load_row_check_number( char *file_row_description )
{
	char buffer[ 512 ];

	if ( !file_row_description )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: file_row_description is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	string_strcpy( buffer, file_row_description, 512 );

	if ( *buffer == '#' && isdigit( * ( buffer + 1 ) ) )
	{
		return atoi( buffer + 1 );
	}
	else
	if ( string_strncmp( buffer, "check " ) == 0 )
	{
		search_replace_string( buffer, "check ", "" );
		return atoi( buffer );
	}

	return 0;
}

void feeder_row_list_insert(
			char *feeder_account,
			char *feeder_load_date_time,
			LIST *feeder_row_list,
			FEEDER_ROW *feeder_row_first_out_balance )
{
	FILE *insert_open;
	FEEDER_ROW *feeder_row;
	JOURNAL *journal;

	if ( !list_rewind( feeder_row_list ) ) return;

	insert_open =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		feeder_row_list_insert_open(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			feeder_row_list_insert_system_string(
				FEEDER_ROW_INSERT_COLUMNS,
				FEEDER_ROW_TABLE,
				SQL_DELIMITER ) );

	do {
		feeder_row = list_get( feeder_row_list );

		if ( feeder_row == feeder_row_first_out_balance )
		{
			break;
		}

		if ( feeder_row->feeder_exist_row_seek )
		{
			continue;
		}

		journal =
			feeder_row_journal(
				feeder_row->feeder_phrase_seek,
				feeder_row->transaction_date_time,
				feeder_row->feeder_matched_journal );

		if ( journal )
		{
			feeder_row_insert_pipe(
				insert_open,
				feeder_account,
				feeder_load_date_time,
				feeder_row->
					feeder_load_row->
					international_date,
				feeder_row->
					feeder_load_row->
					row_number,
				journal->full_name,
				journal->street_address,
				journal->transaction_date_time,
				feeder_row->
					feeder_load_row->
					description_embedded,
				feeder_row->
					feeder_load_row->
					file_row_amount,
				feeder_row->
					feeder_load_row->
					file_row_balance,
				feeder_row->calculate_balance,
				feeder_row->
					feeder_load_row->
					check_number,
				/* --------------- */
				/* Always succeeds */
				/* --------------- */
				feeder_row_phrase(
					feeder_row->feeder_phrase_seek ),
				SQL_DELIMITER );
		}

	} while ( list_next( feeder_row_list ) );

	pclose( insert_open );
}

char *feeder_row_list_insert_system_string(
			char *feeder_row_insert_columns,
			char *feeder_row_table,
			char sql_delimiter )
{
	char system_string[ 1024 ];

	if ( !feeder_row_insert_columns
	||   !feeder_row_table )
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
/*		"tee_appaserver_error.sh			|" */
	 	"sql 2>&1					|"
	 	"html_paragraph_wrapper.e			 ",
	 	feeder_row_table,
	 	feeder_row_insert_columns,
	 	sql_delimiter );

	return strdup( system_string );
}

FILE *feeder_row_list_insert_open(
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

void feeder_row_insert_pipe(
			FILE *insert_open,
			char *feeder_account,
			char *feeder_load_date_time,
			char *international_date,
			int row_number,
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *description_embedded,
			double file_row_amount,
			double file_row_balance,
			double calculate_balance,
			int check_number,
			char *phrase,
			char sql_delimiter )
{
	char check_number_string[ 16 ];

	if ( !insert_open
	||   !feeder_account
	||   !feeder_load_date_time
	||   !international_date
	||   !row_number
	||   !full_name
	||   !street_address
	||   !transaction_date_time
	||   !description_embedded
	||   !file_row_amount
	||   !phrase
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

	if ( check_number )
		sprintf( check_number_string, "%d", check_number );
	else
		*check_number_string = '\0';

	fprintf(insert_open,
		"%s%c%s%c%s%c%d%c%s%c%s%c%s%c%s%c%.2lf%c%.2lf%c%.2lf%c%s%c%s\n",
		feeder_account,
		sql_delimiter,
		feeder_load_date_time,
		sql_delimiter,
		international_date,
		sql_delimiter,
		row_number,
		sql_delimiter,
	 	full_name,
		sql_delimiter,
	 	street_address,
		sql_delimiter,
		transaction_date_time,
		sql_delimiter,
		description_embedded,
		sql_delimiter,
		file_row_amount,
		sql_delimiter,
		file_row_balance,
		sql_delimiter,
		calculate_balance,
		sql_delimiter,
		check_number_string,
		sql_delimiter,
		phrase );
}

char *feeder_row_transaction_date_time(
			char *international_date,
			char *date_hms,
			char *minimum_transaction_date_time )
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

	if ( minimum_transaction_date_time
	&&   strcmp(
		transaction_date_time,
		minimum_transaction_date_time ) < 0 )
	{
		DATE *date =
			date_19new(
				minimum_transaction_date_time );

		date_increment_seconds(
			date,
			1 /* seconds */ );

		strcpy(
			transaction_date_time,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			date_display19(	date ) );
	}

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
				FEEDER_ROW_TABLE,
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

		if ( american_date && *american_date )
		{
			if ( ! ( international_date =
					/* --------------------------- */
					/* Returns heap memory or null */
					/* --------------------------- */
					feeder_load_row_international_date(
						american_date ) ) )
			{
				continue;
			}

			if ( !minimum_date )
			{
				minimum_date = international_date;
				continue;
			}

			if ( strcmp( 
				international_date,
				minimum_date ) < 0 )
			{
				free( minimum_date );
				minimum_date = international_date;
			}
			else
			{
				free( international_date );
			}
		}
	}

	fclose( file );

	return minimum_date;
}

JOURNAL *feeder_row_journal(
			FEEDER_PHRASE *feeder_phrase_seek,
			char *transaction_date_time,
			FEEDER_MATCHED_JOURNAL *feeder_matched_journal )
{
	if ( !feeder_phrase_seek
	&&   !feeder_matched_journal )
	{
		return (JOURNAL *)0;
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

char *feeder_row_phrase( FEEDER_PHRASE *feeder_phrase_seek )
{
	if ( !feeder_phrase_seek )
		return "";
	else
		return feeder_phrase_seek->phrase;
}

FILE *feeder_row_list_display_pipe(
			char *system_string )
{
	return popen( system_string, "w" );
}

char *feeder_row_list_display_system_string( int balance_column )
{
	char system_string[ 1024 ];
	char *heading;
	char *format;

	if ( !balance_column )
	{
		heading =
"Row,Account<br>Entity/Transaction,date,description,amount,calculate_balance";

		format = "right,left,left,left,right,right";
	}
	else
	{
		heading =
"Row,Account<br>Entity/Transaction,date,description,amount,file_balance,calculate_balance,difference,status";

		format = "right,left,left,left,right,right,right,right,left";
	}

	sprintf(system_string,
		"html_table.e '' '%s' '^' %s",
		heading,
		format );

	return strdup( system_string );
}

boolean feeder_row_list_display(
			int balance_column,
			LIST *feeder_row_list,
			FEEDER_ROW *feeder_row_first_out_balance )
{
	FEEDER_ROW *feeder_row;
	FILE *display_pipe;
	char *system_string;

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_row_list_display_system_string(
			balance_column );

	display_pipe =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		feeder_row_list_display_pipe(
			system_string );

	if ( !list_rewind( feeder_row_list ) )
	{
		pclose( display_pipe );
		return 0;
	}

	do {
		feeder_row = list_get( feeder_row_list );

		if ( feeder_row == feeder_row_first_out_balance )
		{
			pclose( display_pipe );

			fflush( stdout );
			printf( "%s\n",
				/* ---------------------- */
				/* Returns program memory */
				/* ---------------------- */
				feeder_row_no_more_display() );
			fflush( stdout );

			display_pipe =
				/* --------------- */
				/* Always succeeds */
				/* --------------- */
				feeder_row_list_display_pipe(
					system_string );
		}

		display_pipe =
			/* ---------------------------- */
			/* Returns display_pipe or null */
			/* ---------------------------- */
			feeder_row_display_output(
				balance_column,
				display_pipe,
				feeder_row );

		if ( !display_pipe
		&&   !list_at_end( feeder_row_list ) )
		{
			display_pipe =
				/* --------------- */
				/* Always succeeds */
				/* --------------- */
				feeder_row_list_display_pipe(
					system_string );
		}

	} while ( list_next( feeder_row_list ) );

	if ( display_pipe ) pclose( display_pipe );

	return (boolean)list_length( feeder_row_list );
}

void feeder_row_transaction_insert(
			LIST *feeder_row_list,
			FEEDER_ROW *feeder_row_first_out_balance )
{
	LIST *transaction_list;

	if ( !list_length( feeder_row_list ) ) return;

	if ( ( transaction_list =
			feeder_row_extract_transaction_list(
				feeder_row_list,
				feeder_row_first_out_balance ) ) )
	{
		transaction_list_insert(
			transaction_list,
			0 /* not insert_journal_list_boolean */ );

		transaction_journal_list_insert(
			transaction_list,
			1 /* with_propagate */ );
	}
}

LIST *feeder_row_extract_transaction_list(
			LIST *feeder_row_list,
			FEEDER_ROW *feeder_row_first_out_balance )
{
	LIST *transaction_list;
	FEEDER_ROW *feeder_row;

	if ( !list_rewind( feeder_row_list ) )
	{
		return (LIST *)0;
	}

	transaction_list = list_new();

	do {
		feeder_row =
			list_get(
				feeder_row_list );

		if ( feeder_row == feeder_row_first_out_balance )
		{
			break;
		}

		if ( feeder_row->feeder_transaction
		&&   feeder_row->feeder_transaction->transaction )
		{
			list_set(
				transaction_list,
				feeder_row->
					feeder_transaction->
					transaction );
		}

	} while ( list_next( feeder_row_list ) );

	if ( !list_length( transaction_list ) )
	{
		return (LIST *)0;
	}
	else
	{
		return transaction_list;
	}
}

FILE *feeder_row_display_output(
			int balance_column,
			FILE *display_pipe,
			FEEDER_ROW *feeder_row )
{
	char *tmp;
	double difference;
	char *status;

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
		exit( 1 );
	}

	tmp =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_row_display_results(
			feeder_row->feeder_exist_row_seek,
			feeder_row->feeder_matched_journal,
			feeder_row->feeder_phrase_seek );

	if ( !balance_column )
	{
		fprintf(display_pipe,
			"%d^%s^%s^%s^%s^%s\n",
			feeder_row->feeder_load_row->row_number,
			tmp,
			feeder_row->feeder_load_row->american_date,
			feeder_row->feeder_load_row->description_embedded,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			string_commas_money(
				feeder_row->feeder_load_row->file_row_amount ),
			string_commas_money(
				feeder_row->calculate_balance ) );
	}
	else
	{
		difference =
			feeder_row->feeder_load_row->file_row_balance  -
			feeder_row->calculate_balance;

		if ( money_virtually_same( difference, 0.0 ) )
			status = "Okay";
		else
			status = "Out";

		fprintf(display_pipe,
			"%d^%s^%s^%s^%s^%s^%s^%s^%s\n",
			feeder_row->feeder_load_row->row_number,
			tmp,
			feeder_row->feeder_load_row->american_date,
			feeder_row->feeder_load_row->description_embedded,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			string_commas_money(
				feeder_row->feeder_load_row->file_row_amount ),
			string_commas_money(
				feeder_row->feeder_load_row->file_row_balance ),
			string_commas_money(
				feeder_row->calculate_balance ),
			string_commas_money( difference ),
			status );
	}

	free( tmp );

	if ( feeder_row->feeder_transaction
	&&   feeder_row->feeder_transaction->transaction )
	{
		pclose( display_pipe );
		display_pipe = (FILE *)0;

		journal_list_html_display(
			feeder_row->
				feeder_transaction->
				transaction->
				journal_list,
			feeder_row->
				feeder_transaction->
				transaction->
				transaction_date_time,
			feeder_row->
				feeder_transaction->
				transaction->
				memo );
	}

	return display_pipe;
}

char *feeder_row_display_results(
			FEEDER_EXIST_ROW *feeder_exist_row_seek,
			FEEDER_MATCHED_JOURNAL *feeder_matched_journal,
			FEEDER_PHRASE *feeder_phrase )
{
	char buffer[ 1024 ];

	if ( feeder_exist_row_seek )
	{
		sprintf(buffer,
		"<p style=\"color:black\">Existing transaction:</p> %s/%s",
			feeder_exist_row_seek->file_row_description,
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
	||   !transaction_date_time )
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
		amount = -amount;
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

void feeder_row_list_calculate_balance_set(
			LIST *feeder_row_list,
			double prior_account_end_balance,
			FEEDER_ROW *feeder_row_first_out_balance )
{
	FEEDER_ROW *feeder_row;

	if ( !list_rewind( feeder_row_list ) ) return;

	do {
		feeder_row = list_get( feeder_row_list );

		if ( !feeder_row->feeder_load_row )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: feeder_load_row is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( feeder_row == feeder_row_first_out_balance ) return;

		if ( feeder_row->feeder_exist_row_seek )
		{
			feeder_row->calculate_balance =
				feeder_row->
					feeder_exist_row_seek->
					file_row_balance;
		}
		else
		{
			feeder_row->calculate_balance =
				prior_account_end_balance +
				feeder_row->feeder_load_row->file_row_amount;

			prior_account_end_balance =
				feeder_row->
					calculate_balance;
		}

	} while ( list_next( feeder_row_list ) );
}

FEEDER_ROW *feeder_row_first_out_balance(
			LIST *feeder_row_list )
{
	FEEDER_ROW *feeder_row;

	if ( !list_rewind( feeder_row_list ) ) return (FEEDER_ROW *)0;

	do {
		feeder_row = list_get( feeder_row_list );

		if ( !feeder_row->feeder_exist_row_seek
		&&   !feeder_row->feeder_matched_journal
		&&   !feeder_row->feeder_phrase_seek )
		{
			return feeder_row;
		}

	} while ( list_next( feeder_row_list ) );

	return (FEEDER_ROW *)0;
}

double feeder_row_list_sum_amount(
			LIST *feeder_row_list,
			FEEDER_ROW *feeder_row_first_out_balance )
{
	FEEDER_ROW *feeder_row;
	double sum_amount;

	if ( !list_rewind( feeder_row_list ) ) return 0.0;

	sum_amount = 0.0;

	do {
		feeder_row = list_get( feeder_row_list );

		if ( !feeder_row->feeder_load_row )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: feeder_load_row is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( feeder_row == feeder_row_first_out_balance )
		{
			break;
		}

		if ( !feeder_row->feeder_exist_row_seek )
		{
			sum_amount +=
				feeder_row->
					feeder_load_row->
					file_row_amount;
		}

	} while ( list_next( feeder_row_list ) );

	return sum_amount;
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

char *feeder_account_end_date(
			LIST *feeder_row_list,
			FEEDER_ROW *feeder_row_first_out_balance )
{
	FEEDER_ROW *feeder_row;
	char *end_date;

	if ( !list_rewind( feeder_row_list ) ) return (char *)0;

	end_date = (char *)0;

	do {
		feeder_row = list_get( feeder_row_list );

		if ( !feeder_row->feeder_load_row )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: feeder_load_row is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( feeder_row == feeder_row_first_out_balance )
			break;

		end_date = feeder_row->feeder_load_row->international_date;

	} while ( list_next( feeder_row_list ) );

	return end_date;
}

double feeder_account_end_calculate_balance(
			LIST *feeder_row_list,
			FEEDER_ROW *feeder_row_first_out_balance )
{
	FEEDER_ROW *feeder_row;
	double end_calculate_balance;

	if ( !list_rewind( feeder_row_list ) ) return 0.0;

	end_calculate_balance = 0.0;

	do {
		feeder_row = list_get( feeder_row_list );

		if ( feeder_row == feeder_row_first_out_balance )
			break;

		end_calculate_balance =
			feeder_row->
				calculate_balance;

	} while ( list_next( feeder_row_list ) );

	return end_calculate_balance;
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

	if ( !feeder_load_file_minimum_date )
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

boolean feeder_row_error_display(
			int balance_column,
			LIST *feeder_row_list )
{
	return
	feeder_row_list_display(
		balance_column,
		feeder_row_error_extract_list(
			feeder_row_list ),
		(FEEDER_ROW *)0 );
}

LIST *feeder_row_error_extract_list(
			LIST *feeder_row_list )
{
	LIST *extract_list = {0};
	FEEDER_ROW *feeder_row;

	if ( !list_rewind( feeder_row_list ) )
		return (LIST *)0;

	do {
		feeder_row = list_get( feeder_row_list );

		if ( !feeder_row->feeder_exist_row_seek
		&&   !feeder_row->feeder_matched_journal
		&&   !feeder_row->feeder_phrase_seek )
		{
			if ( !extract_list ) extract_list = list_new();

			list_set( extract_list, feeder_row );
		}

	} while ( list_next( feeder_row_list ) );

	return extract_list;
}

char *feeder_row_no_more_display( void )
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
		"Journal Not Matched Amount Table",
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
			char *feeder_account,
			char *feeder_load_date_time )
{
	FEEDER_LOAD_EVENT *feeder_load_event;
	FILE *input_open;
	char input[ 1024 ];

	if ( !feeder_account
	||   !feeder_load_date_time )
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
					feeder_account,
					feeder_load_date_time ) ) );

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
			char *feeder_account,
			char *feeder_load_date_time )
{
	static char where[ 128 ];

	if ( !feeder_account
	||   !feeder_load_date_time )
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
		"feeder_load_date_time = '%s'",
		feeder_account,
		feeder_load_date_time );

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
	feeder_load_event->feeder_account = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 1 );
	feeder_load_event->feeder_load_date_time = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	feeder_load_event->login_name = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	feeder_load_event->basename_filename = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	feeder_load_event->account_end_date = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 5 );
	feeder_load_event->account_end_calculate_balance = atof( buffer );

	return feeder_load_event;
}

FEEDER_LOAD_EVENT *feeder_load_event_latest_fetch(
			char *feeder_load_event_table,
			char *feeder_account )
{
	char *latest_date_time;

	if ( ! ( latest_date_time =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			feeder_load_event_latest_date_time(
				feeder_load_event_latest_system_string(
					feeder_load_event_table,
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
		feeder_account,
		latest_date_time );
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
		"max( feeder_load_date_time )",
		table,
		where );

	return strdup( system_string );
}

char *feeder_load_event_latest_date_time(
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

double feeder_account_end_balance(
			char *feeder_account,
			int balance_column,
			double parameter_account_end_balance,
			LIST *feeder_row_list,
			FEEDER_ROW *feeder_row_first_out_balance )
{
	double account_end_balance = {0};
	FEEDER_LOAD_EVENT *feeder_load_event;

	if ( ( feeder_load_event =
		feeder_load_event_latest_fetch(
			FEEDER_LOAD_EVENT_TABLE,
			feeder_account ) ) )
	{
		account_end_balance =
			feeder_load_event->account_end_calculate_balance +
			feeder_row_list_sum_amount(
				feeder_row_list,
				feeder_row_first_out_balance );
	}
	else
	if ( balance_column )
	{
		FEEDER_ROW *last_feeder_row;

		last_feeder_row =
			list_last(
				feeder_row_list );

		account_end_balance =
			/* -------------------------------------- */
			/* Returns this->file_row_balance or null */
			/* -------------------------------------- */
			feeder_load_row_account_end_balance(
				(last_feeder_row)
					? last_feeder_row->feeder_load_row
					: (FEEDER_LOAD_ROW *)0 );
	}
	else
	if ( parameter_account_end_balance )
	{
		account_end_balance = parameter_account_end_balance;
	}

	return account_end_balance;
}

double feeder_prior_account_end_balance(
			char *feeder_account,
			char *feeder_load_file_minimum_date,
			boolean balance_column,
			double feeder_account_end_balance,
			LIST *feeder_row_list,
			FEEDER_ROW *feeder_row_first_out_balance )
{
	FEEDER_LOAD_EVENT *feeder_load_event;
	double prior_account_end_balance = {0};

	if ( !feeder_account
	||   !feeder_load_file_minimum_date
	||   !list_length( feeder_row_list ) )
	{
		return 0.0;
	}

	if ( ( feeder_load_event =
		feeder_load_event_latest_fetch(
			FEEDER_LOAD_EVENT_TABLE,
			feeder_account ) ) )
	{
		prior_account_end_balance =
			feeder_load_event->account_end_calculate_balance;
	}
	else
	{
		prior_account_end_balance =
			journal_prior_account_end_balance(
				feeder_load_file_minimum_date,
				feeder_account /* account_name */ );
	}

	if ( !prior_account_end_balance && balance_column )
	{
		FEEDER_ROW *first_feeder_row;

		first_feeder_row =
			list_first(
				feeder_row_list );

		prior_account_end_balance =
			/* -------------------------------------- */
			/* Returns this->file_row_balance or null */
			/* -------------------------------------- */
			feeder_load_row_prior_account_end_balance(
				(first_feeder_row)
					? first_feeder_row->feeder_load_row
					: (FEEDER_LOAD_ROW *)0 );
	}
	else
	if ( !prior_account_end_balance )
	{
		prior_account_end_balance =
			feeder_account_end_balance -
			feeder_row_list_sum_amount(
				feeder_row_list,
				feeder_row_first_out_balance );
	}

	return prior_account_end_balance;
}

double feeder_load_row_prior_account_end_balance(
			FEEDER_LOAD_ROW *first_feeder_load_row )
{
	if ( first_feeder_load_row )
		return
		first_feeder_load_row->file_row_balance -
		first_feeder_load_row->file_row_amount;
	else
		return 0.0;
}

double feeder_load_row_account_end_balance(
			FEEDER_LOAD_ROW *last_feeder_load_row )
{
	if ( last_feeder_load_row )
		return last_feeder_load_row->file_row_balance;
	else
		return 0.0;
}

LIST *feeder_row_list(	char *feeder_account,
			LIST *feeder_phrase_list,
			LIST *feeder_exist_row_list,
			LIST *feeder_matched_journal_list,
			LIST *feeder_load_row_list )
{
	LIST *list;
	FEEDER_LOAD_ROW *feeder_load_row;
	FEEDER_ROW *feeder_row;
	char *minimum_transaction_date_time = {0};
	DATE *date = feeder_row_date();

	if ( !feeder_account )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: feeder_account is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( feeder_load_row_list ) ) return (LIST *)0;

	list = list_new();

	do {
		feeder_load_row = list_get( feeder_load_row_list );

		feeder_row =
			feeder_row_new(
				date /* feeder_row_date in/out */,
				feeder_account,
				feeder_phrase_list,
				feeder_exist_row_list,
				feeder_matched_journal_list,
				feeder_load_row,
				minimum_transaction_date_time );

		if ( !feeder_row )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: feeder_row_new() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( feeder_row->feeder_matched_journal
		||   feeder_row->feeder_phrase_seek )
		{
			minimum_transaction_date_time =
				feeder_row->transaction_date_time;
		}

		list_set( list, feeder_row );

	} while ( list_next( feeder_load_row_list ) );

	return list;
}

DATE *feeder_row_date( void )
{
	DATE *date =
		date_time_new(
			2020 /* year */,
			1 /* month */,
			1 /* day */,
			0 /* hour */,
			0 /* minute */ );

	date_increment_seconds( date, 1 );

	return date;
}

FEEDER_ROW *feeder_row_new(
			DATE *feeder_row_date /* in/out */,
			char *feeder_account,
			LIST *feeder_phrase_list,
			LIST *feeder_exist_row_list,
			LIST *feeder_matched_journal_list,
			FEEDER_LOAD_ROW *feeder_load_row,
			char *minimum_transaction_date_time )
{
	FEEDER_ROW *feeder_row;

	if ( !feeder_row_date
	||   !feeder_account
	||   !feeder_load_row )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	feeder_row = feeder_row_calloc();

	feeder_row->feeder_load_row = feeder_load_row;

	if ( list_length( feeder_exist_row_list ) )
	{
		feeder_row->feeder_exist_row_seek =
			feeder_exist_row_seek(
				feeder_load_row->international_date,
				feeder_load_row->description_embedded,
				feeder_exist_row_list );
	}

	if ( feeder_row->feeder_exist_row_seek )
	{
		return feeder_row;
	}

	if ( list_length( feeder_matched_journal_list ) )
	{
		if ( feeder_load_row->check_number )
		{
			feeder_row->feeder_matched_journal =
				feeder_matched_journal_check_seek(
					feeder_load_row->check_number,
					feeder_load_row->file_row_amount,
					feeder_matched_journal_list );
		}

		if ( !feeder_row->feeder_matched_journal )
		{
			feeder_row->feeder_matched_journal =
				feeder_matched_journal_amount_seek(
					feeder_load_row->file_row_amount,
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
				feeder_load_row->file_row_description,
				feeder_phrase_list );
	}

	if ( feeder_row->feeder_phrase_seek )
	{
		feeder_row->transaction_date_time =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			feeder_row_transaction_date_time(
				feeder_load_row->international_date,
				date_hms( feeder_row_date ),
				minimum_transaction_date_time );

		date_increment_seconds(
			feeder_row_date,
			1 );

		feeder_row->memo =
			feeder_load_row->description_embedded;

		feeder_row->feeder_transaction =
			feeder_transaction_new(
				feeder_account,
				feeder_row->feeder_phrase_seek,
				feeder_load_row->file_row_amount,
				feeder_row->transaction_date_time,
				feeder_row->memo );
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

	if ( ! ( feeder_row = calloc( 1, sizeof( FEEDER_ROW ) ) ) )
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

char *feeder_parameter_account_end_balance_error(
			double parameter_account_end_balance,
			FEEDER_ROW *feeder_row_first_out_balance,
			double feeder_account_end_calculate_balance,
			int feeder_row_seek_matched_count )
{
	char error[ 128 ];

	if ( !parameter_account_end_balance
	||   dollar_virtually_same(
			parameter_account_end_balance,
			feeder_account_end_calculate_balance )
	||   feeder_row_first_out_balance
	||   !feeder_row_seek_matched_count )
	{
		return "";
	}

	sprintf(error,
"<h3>Warning: account end balance entered (%.2lf) doesn't match the calculated end balance (%.2lf)</h3>\n",
		parameter_account_end_balance,
		feeder_account_end_calculate_balance );

	return strdup( error );
}

int feeder_row_seek_matched_count(
			LIST *feeder_row_list,
			FEEDER_ROW *feeder_row_first_out_balance )
{
	int count = 0;
	FEEDER_ROW *feeder_row;

	if ( !list_rewind( feeder_row_list ) ) return 0;

	do {
		feeder_row = list_get( feeder_row_list );

		if ( feeder_row_first_out_balance
		&&   feeder_row == feeder_row_first_out_balance )
		{
			break;
		}

		if ( feeder_row->feeder_matched_journal
		||   feeder_row->feeder_phrase_seek )
		{
			count++;
		}

	} while ( list_next( feeder_row_list ) );

	return count;
}

char *feeder_row_system_string(
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
		"select.sh \"%s\" %s \"%s\" transaction_date_time",
		select,
		table,
		where );

	return strdup( system_string );
}

LIST *feeder_row_system_list( char *system_string )
{
	FILE *input_pipe;
	char input[ 2048 ];
	LIST *list;

	if ( !system_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: system_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	list = list_new();
	input_pipe = feeder_row_input_pipe( system_string );

	while ( string_input( input, input_pipe, 2048 ) )
	{
		list_set(
			list,
			feeder_row_parse(
				input ) );
	}

	pclose( input_pipe );

	return list;
}

FILE *feeder_row_input_pipe(
			char *feeder_row_system_string )
{
	if ( !feeder_row_system_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	popen( feeder_row_system_string, "r" );
}

FEEDER_ROW *feeder_row_parse( char *input )
{
	char buffer[ 512 ];
	FEEDER_ROW *feeder_row;

	if ( !input || !*input ) return (FEEDER_ROW *)0;

	feeder_row = feeder_row_calloc();

	/* See FEEDER_ROW_SELECT */
	/* --------------------- */
	piece( buffer, SQL_DELIMITER, input, 0 );
	feeder_row->feeder_account = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 1 );
	feeder_row->feeder_load_date_time = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	feeder_row->feeder_date = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	feeder_row->row_number = atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	feeder_row->full_name = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 5 );
	feeder_row->street_address = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 6 );
	feeder_row->transaction_date_time = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 7 );
	feeder_row->file_row_description = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 8 );
	feeder_row->file_row_amount = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 9 );
	feeder_row->file_row_balance = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 10 );
	feeder_row->calculate_balance = atof( buffer );

	if ( piece( buffer, SQL_DELIMITER, input, 11 ) )
	{
		feeder_row->check_number = atoi( buffer );
	}

	if ( piece( buffer, SQL_DELIMITER, input, 12 ) )
	{
		feeder_row->feeder_phrase = strdup( buffer );
	}

	return feeder_row;
}

FEEDER_AUDIT *feeder_audit_fetch(
			char *feeder_account_name,
			char *feeder_load_date_time )
{
	FEEDER_AUDIT *feeder_audit;
	double balance;

	if ( !feeder_account_name
	||   !feeder_load_date_time )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	feeder_audit = feeder_audit_calloc();

	if ( ! ( feeder_audit->feeder_account =
			account_fetch(
				feeder_account_name,
				1 /* fetch_subclassification */,
				1 /* fetch_element */ ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: account_fetch(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			feeder_account_name );
		exit( 1 );
	}

	feeder_audit->feeder_load_event_primary_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		feeder_load_event_primary_where(
			feeder_account_name,
			feeder_load_date_time );

	feeder_audit->feeder_row_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_row_system_string(
			FEEDER_ROW_SELECT,
			FEEDER_ROW_TABLE,
			feeder_audit->feeder_load_event_primary_where
				/* where */ );

	feeder_audit->feeder_row_list =
		feeder_row_system_list(
			feeder_audit->feeder_row_system_string );

	if ( !list_length( feeder_audit->feeder_row_list ) )
	{
		return (FEEDER_AUDIT *)0;
	}

	feeder_audit->first_feeder_row =
		list_first(
			feeder_audit->
				feeder_row_list );

	feeder_audit->journal_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		feeder_audit_journal_where(
			feeder_account_name,
			feeder_audit->
				first_feeder_row->
				transaction_date_time );

	feeder_audit->journal_system_list =
		journal_system_list(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			journal_system_string(
				JOURNAL_SELECT,
				JOURNAL_TABLE,
				feeder_audit->journal_where ),
			0 /* not fetch_account */,
			0 /* not fetch_subclassification */,
			0 /* not fetch_element */,
			0 /* not fetch_transaction */ );

	list_rewind( feeder_audit->feeder_row_list );

	feeder_audit->html_table =
		html_table_new(
			FEEDER_AUDIT_HTML_TITLE,
			(char *)0 /* sub_title */,
			(char *)0 /* sub_sub_title */ );

	feeder_audit->html_table->heading_list =
		feeder_audit_html_heading_list();

	do {
		feeder_audit->feeder_row =
			list_get(
				feeder_audit->
					feeder_row_list );

		if ( ! ( feeder_audit->journal =
				journal_seek(
					feeder_audit->
						feeder_row->
						transaction_date_time,
					feeder_account_name,
					feeder_audit->journal_system_list ) ) )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: journal_seek(%s) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				feeder_account_name );
			exit( 1 );
		}

		balance =
			journal_balance(
				feeder_audit->journal->balance,
				feeder_audit->
					feeder_account->
					subclassification->
					element->
					accumulate_debit );

		if ( !money_virtually_same(
			feeder_audit->feeder_row->calculate_balance,
			balance )
		||   list_at_last( feeder_audit->feeder_row_list ) )
		{
			list_set(
				feeder_audit->html_table->row_list,
				feeder_audit_html_row(
					(feeder_audit->prior_feeder_row)
						? feeder_audit->
							prior_feeder_row->
							transaction_date_time
						: (char *)0,
					feeder_audit->feeder_row,
					balance /* journal_balance */,
					list_at_last(
						feeder_audit->
							feeder_row_list ) ) );
		}

		feeder_audit->prior_feeder_row = feeder_audit->feeder_row;

	} while ( list_next( feeder_audit->feeder_row_list ) );

	return feeder_audit;
}

FEEDER_AUDIT *feeder_audit_calloc( void )
{
	FEEDER_AUDIT *feeder_audit;

	if ( ! ( feeder_audit = calloc( 1, sizeof( FEEDER_AUDIT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return feeder_audit;
}

char *feeder_audit_journal_where(
			char *feeder_account,
			char *transaction_date_time )
{
	static char where[ 128 ];
	char escape_account[ 64 ];

	if ( !feeder_account
	||   !transaction_date_time )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(where,
		"account = '%s' and transaction_date_time >= '%s'",
		string_escape_quote(
			escape_account,
			feeder_account ),
		transaction_date_time );

	return where;
}

LIST *feeder_audit_html_heading_list( void )
{
	LIST *list = list_new();

	list_set(
		list,
		html_heading_new(
			"full_name",
			0 /* not right_justify_boolean */ ) );

	list_set(
		list,
		html_heading_new(
			"description",
			0 /* not right_justify_boolean */ ) );

	list_set(
		list,
		html_heading_new(
			"prior_transaction_date_time",
			0 /* not right_justify_boolean */ ) );

	list_set(
		list,
		html_heading_new(
			"transaction_date_time",
			0 /* not right_justify_boolean */ ) );

	list_set(
		list,
		html_heading_new(
			"feeder_date",
			0 /* not right_justify_boolean */ ) );

	list_set(
		list,
		html_heading_new(
			"row_number",
			1 /* right_justify_boolean */ ) );

	list_set(
		list,
		html_heading_new(
			"amount",
			1 /* right_justify_boolean */ ) );

	list_set(
		list,
		html_heading_new(
			"check_number",
			1 /* right_justify_boolean */ ) );

	list_set(
		list,
		html_heading_new(
			"journal_balance",
			1 /* not right_justify_boolean */ ) );

	list_set(
		list,
		html_heading_new(
			"calculate_balance",
			1 /* right_justify_boolean */ ) );

	list_set(
		list,
		html_heading_new(
			"difference",
			1 /* right_justify_boolean */ ) );

	list_set(
		list,
		html_heading_new(
			"status",
			0 /* not right_justify_boolean */ ) );

	return list;
}

HTML_ROW *feeder_audit_html_row(
			char *prior_transaction_date_time,
			FEEDER_ROW *feeder_row,
			double journal_balance,
			boolean list_at_last )
{
	HTML_ROW *html_row;

	if ( !feeder_row )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: feeder_row is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	html_row = html_row_new();

	html_row->cell_list =
		feeder_audit_html_cell_list(
			feeder_row->full_name,
			feeder_row->file_row_description,
			prior_transaction_date_time,
			feeder_row->transaction_date_time,
			feeder_row->feeder_date,
			feeder_row->row_number,
			feeder_row->file_row_amount,
			feeder_row->check_number,
			feeder_row->calculate_balance,
			journal_balance,
			list_at_last );

	return html_row;
}

LIST *feeder_audit_html_cell_list(
			char *full_name,
			char *file_row_description,
			char *prior_transaction_date_time,
			char *transaction_date_time,
			char *feeder_date,
			int row_number,
			double file_row_amount,
			int check_number,
			double calculate_balance,
			double journal_balance,
			boolean list_at_last )
{
	LIST *list;
	char cell_string[ 128 ];
	double difference;

	if ( !full_name
	||   !file_row_description
	||   !transaction_date_time
	||   !feeder_date )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	list = list_new();

	list_set(
		list,
		html_cell_new(
			full_name,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		list,
		html_cell_new(
			file_row_description,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		list,
		html_cell_new(
			prior_transaction_date_time,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		list,
		html_cell_new(
			transaction_date_time,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		list,
		html_cell_new(
			feeder_date,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	sprintf(cell_string,
		"%d",
		row_number );

	list_set(
		list,
		html_cell_new(
			strdup( cell_string ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		list,
		html_cell_new(
			/* --------------------- */
			/* Returns heap memory   */
			/* Doesn't trim pennies  */
			/* --------------------- */
			timlib_commas_in_money(
				file_row_amount ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	if ( check_number )
	{
		sprintf(cell_string,
			"%d",
			check_number );

		list_set(
			list,
			html_cell_new(
				strdup( cell_string ),
				0 /* not large_boolean */,
				0 /* not bold_boolean */ ) );
	}
	else
	{
		list_set(
			list,
			html_cell_new(
				(char *)0,
				0 /* not large_boolean */,
				0 /* not bold_boolean */ ) );
	}

	difference = journal_balance - calculate_balance;

	list_set(
		list,
		html_cell_new(
			timlib_commas_in_money( journal_balance ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		list,
		html_cell_new(
			timlib_commas_in_money( calculate_balance ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		list,
		html_cell_new(
			timlib_commas_in_money( difference ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	if ( money_virtually_same( difference, 0.0 ) )
	{
		list_set(
			list,
			html_cell_new(
				"Okay",
				1 /* not large_boolean */,
				1 /* not bold_boolean */ ) );
	}
	else
	{
		if ( !list_at_last )
		{
			list_set(
				list,
				html_cell_new(
					"temporary difference",
					0 /* not large_boolean */,
					0 /* not bold_boolean */ ) );
		}
		else
		{
			list_set(
				list,
				html_cell_new(
					"permenant difference",
					1 /* not large_boolean */,
					1 /* not bold_boolean */ ) );
		}
	}

	return list;
}

