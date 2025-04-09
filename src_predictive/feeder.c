/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/feeder.c				*/
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
#include "float.h"
#include "date_convert.h"
#include "sql.h"
#include "sed.h"
#include "environ.h"
#include "process.h"
#include "application_constant.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "feeder.h"

FILE *feeder_phrase_pipe_open( char *system_string )
{
	if ( !system_string )
	{
		char message[ 128 ];

		sprintf(message, "system_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
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
				feeder_phrase_table,
				/* ---------------------- */
				/* Returns program memory */
				/* ---------------------- */
				feeder_phrase_where() ) );

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
		char *table,
		char *where )
{
	char system_string[ 1024 ];

	if ( !select || !table || !where )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		select,
		table,
		where );

	return strdup( system_string );
}

FEEDER_PHRASE *feeder_phrase_parse( char *input )
{
	char buffer[ 128 ];
	FEEDER_PHRASE *feeder_phrase;

	if ( !input || !*input ) return NULL;

	/* See FEEDER_PHRASE_SELECT */
	/* ------------------------ */
	piece( buffer, SQL_DELIMITER, input, 0 );
	feeder_phrase = feeder_phrase_new( strdup( buffer ) );

	/* Returns null if not enough delimiters */
	/* ------------------------------------- */
	piece( buffer, SQL_DELIMITER, input, 1 );
	if ( *buffer ) feeder_phrase->nominal_account = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	if ( *buffer ) feeder_phrase->full_name = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer ) feeder_phrase->street_address = strdup( buffer );

	return feeder_phrase;
}

FEEDER_PHRASE *feeder_phrase_new( char *phrase )
{
	FEEDER_PHRASE *feeder_phrase;

	if ( !phrase || !*phrase )
	{
		char message[ 128 ];

		sprintf(message, "phrase is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	feeder_phrase = feeder_phrase_calloc();
	feeder_phrase->phrase = phrase;

	return feeder_phrase;
}

FEEDER_PHRASE *feeder_phrase_calloc( void )
{
	FEEDER_PHRASE *feeder_phrase;

	if ( ! ( feeder_phrase = calloc( 1, sizeof ( FEEDER_PHRASE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return feeder_phrase;
}

char *feeder_load_row_trim_date( char *description_space_trim )
{
	static char sans_bank_date_description[ 256 ];
	char *replace;
	char *regular_expression;
	SED *sed;

	regular_expression = "[ ][0-9][0-9]/[0-9][0-9]$";
	replace = "";

	sed = sed_new( regular_expression, replace );

	string_strcpy(	sans_bank_date_description,
			description_space_trim,
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
		/* ---------------------- */
		/* Trim the EOL character */
		/* ---------------------- */

		/* Returns buffer with end shortened (maybe) */
		/* ----------------------------------------- */
		string_trim_right( sans_bank_date_description, 1 );
	}

	sed_free( sed );

	return
	/* ----------------------------------------- */
	/* Returns buffer with end shortened (maybe) */
	/* ----------------------------------------- */
	string_rtrim( sans_bank_date_description );
}

FEEDER_LOAD_ROW *feeder_load_row_new(
		int date_column /* one_based */,
		int description_column /* one_based */,
		int debit_column /* one_based */,
		int credit_column /* one_based */,
		int balance_column /* one_based */,
		int reference_column /* one_based */,
		char *input,
		int feeder_row_number )
{
	FEEDER_LOAD_ROW *feeder_load_row;
	char buffer[ 512 ];

	if ( !date_column )
	{
		char message[ 128 ];

		sprintf(message, "date_column is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !description_column )
	{
		char message[ 128 ];

		sprintf(message, "description_column is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !debit_column )
	{
		char message[ 128 ];

		sprintf(message, "debit_column is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !feeder_row_number )
	{
		char message[ 128 ];

		sprintf(message, "feeder_row_number is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !input || !*input ) return NULL;

	feeder_load_row = feeder_load_row_calloc();

	feeder_load_row->feeder_row_number = feeder_row_number;

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
		return NULL;
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
		return NULL;
	}

	feeder_load_row->description_space_trim =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_load_row_description_space_trim(
			feeder_load_row->file_row_description );

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
		return NULL;
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
			feeder_load_row->description_space_trim );

	feeder_load_row->description_embedded =
		/* ------------------------------------------- */
		/* Returns heap memory or file_row_description */
		/* ------------------------------------------- */
		feeder_load_row_description_embedded(
			feeder_load_row->description_space_trim,
			feeder_load_row->file_row_balance,
			feeder_load_row->reference_string,
			feeder_load_row->check_number );

	return feeder_load_row;
}

FEEDER_LOAD_ROW *feeder_load_row_calloc( void )
{
	FEEDER_LOAD_ROW *feeder_load_row;

	if ( ! ( feeder_load_row =
			calloc( 1, sizeof ( FEEDER_LOAD_ROW ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
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
		char *description_space_trim,
		double balance,
		char *reference_string,
		int check_number )
{
	if ( check_number ) return description_space_trim;

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
			feeder_load_row_trim_date(
				description_space_trim ),
			balance,
			reference_string ),
		FEEDER_DESCRIPTION_SIZE );
}

char *feeder_load_row_description_build(
		char *feeder_load_row_trim_date,
		double balance,
		char *reference_string )
{
	char build[ 512 ];

	if ( !feeder_load_row_trim_date )
	{
		char message[ 128 ];

		sprintf(message, "feeder_load_row_trim_date is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( balance )
	{
		sprintf(build,
			"%s %.2lf",
			feeder_load_row_trim_date,
			balance );
	}
	else
	if ( reference_string )
	{
		sprintf(build,
			"%s %s",
			feeder_load_row_trim_date,
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
	char *international_date;

	if ( !american_date || !*american_date ) return (char *)0;

	international_date =
		/* ----------------------------------------------- */
		/* Returns heap memory, source_date_string or null */
		/* ----------------------------------------------- */
		date_convert_source_american(
			date_convert_international /* destination_enum */,
			american_date );

	if ( !international_date )
		return (char *)0;
	else
	if ( international_date == american_date )
		return strdup( international_date );
	else
		return international_date;
}

FILE *feeder_load_file_input_open( char *feeder_load_file_filename )
{
	FILE *input_open;

	if ( !feeder_load_file_filename )
	{
		char message[ 128 ];

		sprintf(message, "feeder_load_file_filename is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( ! ( input_open = fopen( feeder_load_file_filename, "r" ) ) )
	{
		char message[ 256 ];

		sprintf(message,
			"fopen(%s) returned empty.",
			feeder_load_file_filename );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return input_open;
}

FEEDER_LOAD_FILE *feeder_load_file_fetch(
		char *application_name,
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
	FILE *input_open;
	int feeder_row_number = 0;
	FEEDER_LOAD_ROW *feeder_load_row;

	if ( !application_name
	||   !feeder_load_filename
	||   !date_column
	||   !description_column
	||   !debit_column )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	feeder_load_file = feeder_load_file_calloc();

	feeder_load_file->feeder_load_filename = feeder_load_filename;

	feeder_load_file->filename =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		feeder_load_file_filename(
			application_name,
			feeder_load_filename,
			appaserver_parameter_upload_directory() );

	input_open =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		feeder_load_file_input_open(
			feeder_load_file->filename );

	feeder_load_file->feeder_load_row_list = list_new();

	while ( string_input( input, input_open, 1024 ) )
	{
		if ( !*input || *input == '#' )
		{
			++feeder_row_number;
			continue;
		}

		string_trim_character(
			input /* source_destination */,
			'\\' );

		feeder_load_row =
			feeder_load_row_new(
				date_column,
				description_column,
				debit_column,
				credit_column,
				balance_column,
				reference_column,
				input,
				++feeder_row_number );

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

	fclose( input_open );

	return feeder_load_file;
}

FEEDER_LOAD_FILE *feeder_load_file_calloc( void )
{
	FEEDER_LOAD_FILE *feeder_load_file;

	if ( ! ( feeder_load_file = calloc( 1, sizeof ( FEEDER_LOAD_FILE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return feeder_load_file;
}

FEEDER_LOAD_EVENT *feeder_load_event_new(
		char *feeder_account_name,
		char *feeder_load_date_time,
		char *login_name,
		char *feeder_load_filename,
		char *feeder_row_account_end_date,
		double feeder_row_account_end_balance )
{
	FEEDER_LOAD_EVENT *feeder_load_event;

	if ( !feeder_account_name
	||   !feeder_load_date_time
	||   !login_name
	||   !feeder_load_filename
	||   !feeder_row_account_end_date )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	feeder_load_event = feeder_load_event_calloc();

	feeder_load_event->feeder_account_name = feeder_account_name;
	feeder_load_event->feeder_load_date_time = feeder_load_date_time;
	feeder_load_event->login_name = login_name;
	feeder_load_event->feeder_load_filename = feeder_load_filename;

	feeder_load_event->feeder_row_account_end_date =
		feeder_row_account_end_date;

	feeder_load_event->feeder_row_account_end_balance =
		feeder_row_account_end_balance;

	return feeder_load_event;
}

FEEDER_LOAD_EVENT *feeder_load_event_calloc( void )
{
	FEEDER_LOAD_EVENT *feeder_load_event;

	if ( ! ( feeder_load_event =
			calloc( 1, sizeof ( FEEDER_LOAD_EVENT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return feeder_load_event;
}

void feeder_load_event_insert(
		const char *feeder_load_event_table,
		const char *feeder_load_event_insert,
		char *feeder_account_name,
		char *feeder_load_date_time,
		char *login_name,
		char *feeder_load_filename,
		char *feeder_row_account_end_date,
		double feeder_row_account_end_balance )
{
	FILE *insert_open;

	if ( !feeder_account_name
	||   !feeder_load_date_time
	||   !login_name
	||   !feeder_load_filename
	||   !feeder_row_account_end_date )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
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
		feeder_account_name,
		feeder_load_date_time,
		login_name,
		feeder_load_filename,
		feeder_row_account_end_date,
		feeder_row_account_end_balance );

	pclose( insert_open );
}

FILE *feeder_load_event_insert_open( char *system_string )
{
	if ( !system_string )
	{
		char message[ 128 ];

		sprintf(message, "system_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	popen( system_string, "w" );
}

char *feeder_load_event_insert_system_string(
		const char *feeder_load_event_table,
		const char *feeder_load_event_insert,
		const char sql_delimiter )
{
	char system_string[ 1024 ];

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
		char *feeder_account_name,
		char *feeder_load_date_time,
		char *login_name,
		char *feeder_load_filename,
		char *feeder_row_account_end_date,
		double feeder_row_account_end_calculate_balance )
{
	if ( !insert_open
	||   !sql_delimiter
	||   !feeder_account_name
	||   !feeder_load_date_time
	||   !login_name
	||   !feeder_load_filename
	||   !feeder_row_account_end_date )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		if ( insert_open ) pclose( insert_open );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	/* See FEEDER_LOAD_EVENT_INSERT */
	/* ---------------------------- */
	fprintf(insert_open,
		"%s%c%s%c%s%c%s%c%s%c%.2lf\n",
		feeder_account_name,
		sql_delimiter,
	 	feeder_load_date_time,
		sql_delimiter,
		login_name,
		sql_delimiter,
		feeder_load_filename,
		sql_delimiter,
		feeder_row_account_end_date,
		sql_delimiter,
		feeder_row_account_end_calculate_balance );
}

FEEDER *feeder_fetch(
		char *application_name,
		char *login_name,
		char *feeder_account_name,
		char *feeder_load_filename,
		int date_column /* one_based */,
		int description_column /* one_based */,
		int debit_column /* one_based */,
		int credit_column /* one_based */,
		int balance_column /* one_based */,
		int reference_column /* one_based */,
		boolean reverse_order_boolean,
		double parameter_end_balance,
		boolean parameter_end_balance_not_null_boolean )
{
	FEEDER *feeder;

	if ( !application_name
	||   !login_name
	||   !feeder_account_name
	||   !*feeder_account_name
	||   strcmp( feeder_account_name, "feeder_account" ) == 0
	||   !date_column
	||   !description_column
	||   !debit_column )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !feeder_load_filename
	||   !*feeder_load_filename
	||   strcmp( feeder_load_filename, "filename" ) == 0 )
	{
		return NULL;
	}

	feeder = feeder_calloc();

	feeder->feeder_account_name = feeder_account_name;

	if ( ! ( feeder->feeder_load_file_minimum_date =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			feeder_load_file_minimum_date(
				application_name,
				feeder_load_filename,
				date_column /* one_based */ ) ) )
	{
		return NULL;
	}

	feeder->feeder_load_file =
		feeder_load_file_fetch(
			application_name,
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
		return NULL;
	}

	if ( !balance_column
	&&   parameter_end_balance_not_null_boolean )
	{
		feeder_load_row_balance_set(
			feeder->
				feeder_load_file->
				feeder_load_row_list /* in/out */,
			parameter_end_balance );
	}

	feeder->account_uncleared_checks =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		account_uncleared_checks(
			ACCOUNT_UNCLEARED_CHECKS_KEY );

	feeder->feeder_phrase_list =
		feeder_phrase_list(
			FEEDER_PHRASE_SELECT,
			FEEDER_PHRASE_TABLE );

	feeder->feeder_exist_row_list =
		feeder_exist_row_list(
			feeder_account_name,
			feeder->feeder_load_file_minimum_date );

	feeder->match_days_ago =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		feeder_match_days_ago(
			FEEDER_LOAD_TRANSACTION_DAYS_AGO,
			FEEDER_MATCH_DEFAULT_DAYS_AGO );

	feeder->match_minimum_date =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		feeder_match_minimum_date(
			feeder->feeder_load_file_minimum_date,
			feeder->match_days_ago );

	feeder->feeder_matched_journal_list =
		feeder_matched_journal_list(
			feeder_account_name,
			FEEDER_ROW_TABLE,
			feeder->match_minimum_date,
			feeder->account_uncleared_checks );

	feeder->feeder_row_list =
		feeder_row_list(
			feeder_account_name,
			feeder->account_uncleared_checks,
			feeder->feeder_phrase_list,
			feeder->feeder_exist_row_list,
			feeder->feeder_matched_journal_list,
			feeder->feeder_load_file->feeder_load_row_list );

	feeder->feeder_row_first_out_balance =
		feeder_row_first_out_balance(
			feeder->feeder_row_list );

	feeder->feeder_row_count =
		feeder_row_count(
			feeder->feeder_row_list );

	if ( !feeder->feeder_row_count ) return feeder;

	feeder->prior_account_end_balance =
		feeder_prior_account_end_balance(
			feeder_account_name );

	feeder_row_calculate_balance_set(
		feeder->feeder_row_list /* in/out */,
		feeder->feeder_row_first_out_balance,
		feeder->prior_account_end_balance );

	feeder_row_status_set(
		feeder->feeder_row_list /* in/out */,
		balance_column,
		parameter_end_balance_not_null_boolean,
		feeder->feeder_row_first_out_balance );

	feeder->feeder_load_date_time =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		date_now19( date_utc_offset() );

	feeder->feeder_row_account_end_date =
		/* ------------------------------------------------------- */
		/* Returns feeder_row->feeder_load_row->international_date */
		/* or null.						   */
		/* ------------------------------------------------------- */
		feeder_row_account_end_date(
			feeder->feeder_row_list,
			feeder->feeder_row_first_out_balance );

	if ( feeder->feeder_row_account_end_date )
	{
		feeder->feeder_row_account_end_balance =
			feeder_row_account_end_balance(
				feeder->feeder_row_list,
				feeder->feeder_row_first_out_balance );

		feeder->feeder_load_event =
			feeder_load_event_new(
				feeder_account_name,
				feeder->feeder_load_date_time,
				login_name,
				feeder->feeder_load_file->feeder_load_filename,
				feeder->feeder_row_account_end_date,
				feeder->feeder_row_account_end_balance );
	}

	feeder->parameter_end_balance_error =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		feeder_parameter_end_balance_error(
			parameter_end_balance,
			parameter_end_balance_not_null_boolean,
			feeder->feeder_row_first_out_balance,
			feeder->feeder_row_account_end_date,
			feeder->feeder_row_account_end_balance );

	return feeder;
}

FEEDER *feeder_calloc( void )
{
	FEEDER *feeder;

	if ( ! ( feeder = calloc( 1, sizeof ( FEEDER ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return feeder;
}

FEEDER_PHRASE *feeder_phrase_seek(
		char *description_space_trim,
		LIST *feeder_phrase_list )
{
	FEEDER_PHRASE *feeder_phrase = {0};
	char feeder_component[ 128 ];
	int piece_number;

	if ( list_rewind( feeder_phrase_list ) )
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
			if ( string_exists_substring(
				description_space_trim /* string */,
				feeder_component /* substring */ ) )
			{
				return feeder_phrase;
			}
		}

	} while ( list_next( feeder_phrase_list ) );

	return NULL;
}

char feeder_phrase_delimiter( char *phrase )
{
	if ( string_exists_character( phrase, '|' ) )
		return '|';
	else
	if ( string_exists_character( phrase, ',' ) )
		return ',';
	else
	if ( string_exists_character( phrase, ';' ) )
		return ';';
	else
		return 0;
}

char *feeder_exist_row_where(
		char *feeder_account_name,
		char *feeder_load_file_minimum_date )
{
	static char where[ 128 ];

	if ( !feeder_account_name
	||   strcmp( feeder_account_name, "feeder_account" ) == 0
	||   !feeder_load_file_minimum_date )
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
		"feeder_account = '%s' and "
		"feeder_date >= '%s'",
		feeder_account_name,
		feeder_load_file_minimum_date );

	return where;
}

FEEDER_EXIST_ROW *feeder_exist_row_parse( char *input )
{
	FEEDER_EXIST_ROW *feeder_exist_row;
	char buffer[ 256 ];

	if ( !input || !*input ) return NULL;

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
	feeder_exist_row->file_row_amount = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	feeder_exist_row->file_row_balance = atof( buffer );

	return feeder_exist_row;
}

FEEDER_EXIST_ROW *feeder_exist_row_calloc( void )
{
	FEEDER_EXIST_ROW *feeder_exist_row;

	if ( ! ( feeder_exist_row = calloc( 1, sizeof ( FEEDER_EXIST_ROW ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
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
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( feeder_exist_row_list ) )
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

	return NULL;
}

FEEDER_TRANSACTION *feeder_transaction_calloc( void )
{
	FEEDER_TRANSACTION *feeder_transaction;

	if ( ! ( feeder_transaction =
			calloc( 1, sizeof ( FEEDER_TRANSACTION ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return feeder_transaction;
}

LIST *feeder_matched_journal_list(
		char *feeder_account_name,
		char *feeder_row_table,
		char *feeder_match_minimum_date,
		char *account_uncleared_checks )
{
	LIST *list;
	char *subquery;
	char *where;
	LIST *system_list;
	JOURNAL *journal;

	if ( !feeder_account_name
	||   !feeder_row_table
	||   !feeder_match_minimum_date
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

	subquery =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_matched_journal_subquery(
			feeder_account_name,
			account_uncleared_checks,
			JOURNAL_TABLE,
			feeder_row_table );

	where =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_matched_journal_where(
			feeder_account_name,
			account_uncleared_checks,
			subquery,
			feeder_match_minimum_date );

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
				sizeof ( FEEDER_MATCHED_JOURNAL ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return feeder_matched_journal;
}

char *feeder_matched_journal_subquery(
		char *feeder_account_name,
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
		feeder_account_name,
		account_uncleared_checks );

	return strdup( subquery );
}

char *feeder_matched_journal_where(
		char *feeder_account_name,
		char *account_uncleared_checks,
		char *subquery,
		char *feeder_match_minimum_date )
{
	char where[ 2048 ];

	if ( !feeder_account_name
	||   !account_uncleared_checks
	||   !subquery
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

	sprintf(where,
		"account in ('%s','%s') and		"
		"transaction_date_time >= '%s' and	"
		"%s					",
		feeder_account_name,
		account_uncleared_checks,
		feeder_match_minimum_date,
		subquery );

	return strdup( where );
}

int feeder_load_row_check_number( char *description_space_trim )
{
	char buffer[ 512 ];
	char *substr = "CHECK #:";

	int position;

	if ( !description_space_trim )
	{
		char message[ 128 ];

		sprintf(message, "description_space_trim is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	string_strcpy( buffer, description_space_trim, 512 );

	/* If like: VETERANS AFFAIRS DES:PAYMENT CHECK #:1827 */
	/* -------------------------------------------------- */
	position =
		/* ------------------------ */
		/* Returns -1 if not found. */
		/* ------------------------ */
		string_instr( 
			substr,
			buffer /* string */,
			1 /* occurrence */ );

	if ( position > -1 )
	{
		return atoi( buffer + position + strlen( substr ) );
	}
	else
	if ( *buffer == '#' && isdigit( * ( buffer + 1 ) ) )
	{
		return atoi( buffer + 1 );
	}
	else
	if ( string_strncmp( buffer, "check " ) == 0 )
	{
		string_search_replace( buffer, "check ", "" );
		string_search_replace( buffer, "CHECK ", "" );
		return atoi( buffer );
	}

	return 0;
}

void feeder_row_list_insert(
		char *feeder_account_name,
		char *feeder_load_date_time,
		LIST *feeder_row_list,
		FEEDER_ROW *feeder_row_first_out_balance )
{
	FILE *insert_open;
	FEEDER_ROW *feeder_row;
	JOURNAL *journal;

	if ( !list_rewind( feeder_row_list ) ) return;

	insert_open =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
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
			feeder_row->feeder_matched_journal,
			feeder_row->feeder_phrase_seek,
			/* ------------------------------------------ */
			/* See feeder_row_transaction_date_time_set() */
			/* ------------------------------------------ */
			feeder_row->transaction_date_time );

		feeder_row_insert(
			insert_open,
			feeder_account_name,
			feeder_load_date_time,
			feeder_row->
				feeder_load_row->
				international_date,
			feeder_row->
				feeder_load_row->
				feeder_row_number,
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
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			feeder_row_phrase(
				feeder_row->feeder_phrase_seek ),
			SQL_DELIMITER );

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

FILE *feeder_row_list_insert_open( char *insert_system_string )
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

void feeder_row_insert(
		FILE *insert_open,
		char *feeder_account_name,
		char *feeder_load_date_time,
		char *international_date,
		int feeder_row_number,
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
	||   !feeder_account_name
	||   !feeder_load_date_time
	||   !international_date
	||   !feeder_row_number
	||   !full_name
	||   !street_address
	||   !transaction_date_time
	||   !description_embedded
	||   !file_row_amount
	||   !phrase
	||   !sql_delimiter )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		if ( insert_open ) pclose( insert_open );

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

	fprintf(insert_open,
		"%s%c%s%c%s%c%d%c%s%c%s%c%s%c%s%c%.2lf%c%.2lf%c%.2lf%c%s%c%s\n",
		feeder_account_name,
		sql_delimiter,
		feeder_load_date_time,
		sql_delimiter,
		international_date,
		sql_delimiter,
		feeder_row_number,
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

FILE *feeder_exist_row_input_open( char *feeder_exist_row_system_string )
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
		char *feeder_account_name,
		char *feeder_load_file_minimum_date )
{
	LIST *list;
	FILE *input_open;
	char input[ 1024 ];

	if ( !feeder_account_name
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
					feeder_account_name,
					feeder_load_file_minimum_date ) ) );

	while ( string_input( input, input_open, 1024 ) )
	{
		list_set(
			list,
			feeder_exist_row_parse(
				input ) );
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

char *feeder_load_file_minimum_date(
		char *application_name,
		char *feeder_load_filename,
		int date_column )
{
	FILE *input_open;
	char input[ 1024 ];
	char buffer[ 128 ];
	char *american_date;
	char *international_date;
	char *minimum_date = {0};

	input_open =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		feeder_load_file_input_open(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			feeder_load_file_filename(
				application_name,
				feeder_load_filename,
				appaserver_parameter_upload_directory() ) );

	while ( string_input( input, input_open, 1024 ) )
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

	fclose( input_open );

	return minimum_date;
}

JOURNAL *feeder_row_journal(
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
	boolean result;

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
		list_tail( feeder_row_list );
	else
		list_rewind( feeder_row_list );

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

		if ( reverse_order_boolean )
			result = list_prior( feeder_row_list );
		else
			result = list_next( feeder_row_list );

	} while ( result );

	if ( display_pipe ) pclose( display_pipe );

	fflush( stdout );
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
		/* May reset transaction->transaction_date_time */
		/* -------------------------------------------- */
		transaction_list_insert(
			transaction_list,
			0 /* not insert_journal_list_boolean */,
			0 /* not transaction_lock_boolean */ );

		transaction_journal_list_insert(
			transaction_list,
			1 /* with_propagate */ );

		/* Set each feeder_row->transaction_date_time */
		/* ------------------------------------------ */
		feeder_row_transaction_date_time_set(
			feeder_row_list /* in/out */ );
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
		FILE *display_pipe,
		FEEDER_ROW *feeder_row )
{
	char *display_results;
	char *status_string;
	char calculate_balance_string[ 128 ];
	char file_row_balance_string[ 128 ];
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
			feeder_row->feeder_exist_row_seek,
			feeder_row->feeder_matched_journal,
			feeder_row->feeder_phrase_seek );


	strcpy(	calculate_balance_string,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		string_commas_money(
			feeder_row->calculate_balance ) );

	strcpy(	file_row_balance_string,
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
		feeder_row->feeder_load_row->feeder_row_number,
		display_results,
		feeder_row->feeder_load_row->american_date,
		feeder_row->feeder_load_row->description_embedded,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		string_commas_money(
			feeder_row->feeder_load_row->file_row_amount ),
		file_row_balance_string,
		calculate_balance_string,
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

FEEDER_MATCHED_JOURNAL *
	feeder_matched_journal_check_seek(
		char *feeder_account_name,
		char *account_uncleared_checks,
		int check_number,
		double amount,
		LIST *feeder_matched_journal_list )
{
	FEEDER_MATCHED_JOURNAL *feeder_matched_journal;

	if ( !feeder_account_name
	||   !account_uncleared_checks
	||   !check_number
	||   !amount )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( feeder_matched_journal_list ) )
	do {
		feeder_matched_journal =
			list_get(
				feeder_matched_journal_list );

		if ( feeder_matched_journal->taken ) continue;

		if ( feeder_matched_journal->check_number == check_number
		&&   float_money_virtually_same(
			feeder_matched_journal->amount,
			amount ) )
		{
			feeder_matched_journal->check_update_statement =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				feeder_matched_journal_check_update_statement(
					JOURNAL_TABLE,
					feeder_account_name,
					account_uncleared_checks,
					feeder_matched_journal->
						full_name,
					feeder_matched_journal->
						street_address,
					feeder_matched_journal->
						transaction_date_time );

			return feeder_matched_journal;
		}

	} while ( list_next( feeder_matched_journal_list ) );

	return NULL;
}

FEEDER_MATCHED_JOURNAL *
	feeder_matched_journal_amount_seek(
		double amount,
		LIST *feeder_matched_journal_list )
{
	FEEDER_MATCHED_JOURNAL *feeder_matched_journal;

	if ( !amount )
	{
		char message[ 128 ];

		sprintf(message, "amount is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( feeder_matched_journal_list ) )
	do {
		feeder_matched_journal =
			list_get(
				feeder_matched_journal_list );

		if ( feeder_matched_journal->taken ) continue;
		if ( feeder_matched_journal->check_number ) continue;

		if ( float_money_virtually_same(
			feeder_matched_journal->amount,
			amount ) )
		{
			return feeder_matched_journal;
		}

	} while ( list_next( feeder_matched_journal_list ) );

	return NULL;
}

FEEDER_TRANSACTION *feeder_transaction_new(
		char *feeder_account_name,
		FEEDER_PHRASE *feeder_phrase_seek,
		double amount,
		char *transaction_date_time,
		char *memo )
{
	FEEDER_TRANSACTION *feeder_transaction;

	if ( !feeder_account_name
	||   !feeder_phrase_seek
	||   !amount
	||   !transaction_date_time )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	feeder_transaction = feeder_transaction_calloc();

	if ( amount < 0.0 )
	{
		feeder_transaction->debit_account =
			feeder_phrase_seek->
				nominal_account;

		feeder_transaction->credit_account = feeder_account_name;
		amount = -amount;
	}
	else
	{
		feeder_transaction->debit_account = feeder_account_name;

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
		char message[ 128 ];

		sprintf(message,
			"transaction_binary(%s/%.2lf) returned empty.",
			transaction_date_time,
			amount );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return feeder_transaction;
}

void feeder_row_calculate_balance_set(
		LIST *feeder_row_list,
		FEEDER_ROW *feeder_row_first_out_balance,
		double feeder_prior_account_end_balance )
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

		if ( feeder_row == feeder_row_first_out_balance ) break;

		feeder_row->calculate_balance =
			feeder_row_calculate_balance(
				feeder_row,
				feeder_prior_account_end_balance );

		feeder_prior_account_end_balance =
			feeder_row->
				calculate_balance;

	} while ( list_next( feeder_row_list ) );
}

double feeder_row_calculate_balance(
		FEEDER_ROW *feeder_row,
		double feeder_prior_account_end_balance )
{
	double calculate_balance;

	if ( feeder_row->feeder_exist_row_seek )
	{
		calculate_balance =
			feeder_row->
				feeder_exist_row_seek->
				file_row_balance;
	}
	else
	{
		calculate_balance =
			feeder_prior_account_end_balance +
			feeder_row->feeder_load_row->file_row_amount;
	}

	return calculate_balance;
}

FEEDER_ROW *feeder_row_first_out_balance( LIST *feeder_row_list )
{
	FEEDER_ROW *feeder_row;

	if ( list_rewind( feeder_row_list ) )
	do {
		feeder_row = list_get( feeder_row_list );

		if ( !feeder_row->feeder_exist_row_seek
		&&   !feeder_row->feeder_matched_journal
		&&   !feeder_row->feeder_phrase_seek )
		{
			return feeder_row;
		}

	} while ( list_next( feeder_row_list ) );

	return NULL;
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

	if ( !journal->transaction )
	{
		char message[ 256 ];

		snprintf(
			message,
			sizeof ( message ),
			"journal->transaction is empty for (%s/%s).",
			journal->full_name,
			journal->transaction_date_time );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
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

char *feeder_row_account_end_date(
		LIST *feeder_row_list,
		FEEDER_ROW *feeder_row_first_out_balance )
{
	FEEDER_ROW *feeder_row;
	char *account_end_date = {0};

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

		if ( feeder_row == feeder_row_first_out_balance )
			break;

		account_end_date =
			feeder_row->
				feeder_load_row->
				international_date;

	} while ( list_next( feeder_row_list ) );

	return account_end_date;
}

double feeder_row_account_end_balance(
		LIST *feeder_row_list,
		FEEDER_ROW *feeder_row_first_out_balance )
{
	FEEDER_ROW *feeder_row;
	double account_end_balance = 0.0;

	if ( list_rewind( feeder_row_list ) )
	do {
		feeder_row = list_get( feeder_row_list );

		if ( feeder_row == feeder_row_first_out_balance )
			break;

		account_end_balance =
			feeder_row->
				calculate_balance;

	} while ( list_next( feeder_row_list ) );

	return account_end_balance;
}

int feeder_match_days_ago(
		const char *feeder_load_transaction_days_ago,
		const int feeder_match_default_days_ago )
{
	char *days_ago_string;

	days_ago_string =
	/* ------------------------------------------------------------------ */
	/* Returns component of application_constant->dictionary->get() or "" */
	/* ------------------------------------------------------------------ */
	     application_constant_fetch(
			(char *)feeder_load_transaction_days_ago );

	if ( *days_ago_string )
		return atoi( days_ago_string );
	else
		return feeder_match_default_days_ago;
}

char *feeder_match_minimum_date(
	char *feeder_load_file_minimum_date,
	int feeder_match_days_ago )
{
	DATE *date;

	if ( !feeder_load_file_minimum_date )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"feeder_load_file_minimum_date is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( ! ( date =
			date_yyyy_mm_dd_new(
				feeder_load_file_minimum_date ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"Cannot determine minimum date using [%s].",
			feeder_load_file_minimum_date );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	date =
		date_decrement_days(
			date,
			(double)feeder_match_days_ago );

	return
	/* ------------------------- */
	/* Returns heap memory or "" */
	/* ------------------------- */
	date_display_yyyy_mm_dd( date );
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

FEEDER_LOAD_EVENT *feeder_load_event_fetch(
		char *feeder_account_name,
		char *feeder_load_date_time )
{
	FEEDER_LOAD_EVENT *feeder_load_event;
	FILE *input_open;
	char input[ 1024 ];

	if ( !feeder_account_name
	||   !feeder_load_date_time )
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
					feeder_account_name,
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
		char *feeder_account_name,
		char *feeder_load_date_time )
{
	static char where[ 128 ];

	if ( !feeder_account_name
	||   !feeder_load_date_time )
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

	snprintf(
		where,
		sizeof ( where ),
		"feeder_account = '%s' and "
		"feeder_load_date_time = '%s'",
		feeder_account_name,
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

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh \"%s\" %s \"%s\"",
		feeder_load_event_select,
		feeder_load_event_table,
		feeder_load_event_primary_where );

	return strdup( system_string );
}

FILE *feeder_load_event_input_open( char *feeder_load_event_system_string )
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

	if ( !input || !*input ) return NULL;

	feeder_load_event = feeder_load_event_calloc();

	/* See FEEDER_LOAD_EVENT_SELECT */
	/* ---------------------------- */
	piece( buffer, SQL_DELIMITER, input, 0 );
	feeder_load_event->feeder_account_name = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 1 );
	feeder_load_event->feeder_load_date_time = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	if ( *buffer ) feeder_load_event->login_name = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer )
		feeder_load_event->feeder_load_filename =
			strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	if ( *buffer )
	{
		feeder_load_event->feeder_row_account_end_date =
			strdup( buffer );
	}

	piece( buffer, SQL_DELIMITER, input, 5 );
	if ( *buffer )
	{
		feeder_load_event->feeder_row_account_end_balance =
			atof( buffer );
	}

	return feeder_load_event;
}

FEEDER_LOAD_EVENT *feeder_load_event_latest_fetch(
		char *feeder_load_event_table,
		char *feeder_account_name )
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
						feeder_account_name ) ) ) ) )
	{
		return NULL;
	}

	return
	feeder_load_event_fetch(
		feeder_account_name,
		latest_date_time );
}

char *feeder_load_event_account_where( char *feeder_account_name )
{
	static char where[ 128 ];

	if ( !feeder_account_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"feeder_account_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		where,
		sizeof ( where ),
		"feeder_account = '%s'",
		feeder_account_name );

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

	snprintf(
		system_string,
		sizeof ( system_string ),
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

double feeder_prior_account_end_balance(
		char *feeder_account_name )
{
	FEEDER_LOAD_EVENT *feeder_load_event;
	double prior_account_end_balance = {0};

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

	if ( ( feeder_load_event =
		feeder_load_event_latest_fetch(
			FEEDER_LOAD_EVENT_TABLE,
			feeder_account_name ) ) )
	{
		prior_account_end_balance =
			feeder_load_event->feeder_row_account_end_balance;
	}
	else
	{
		prior_account_end_balance =
			journal_first_account_balance(
				feeder_account_name /* account_name */ );
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

LIST *feeder_row_list(
		char *feeder_account_name,
		char *account_uncleared_checks,
		LIST *feeder_phrase_list,
		LIST *feeder_exist_row_list,
		LIST *feeder_matched_journal_list,
		LIST *feeder_load_row_list )
{
	LIST *list = list_new();
	FEEDER_LOAD_ROW *feeder_load_row;
	FEEDER_ROW *feeder_row;
	char *minimum_transaction_date_time = {0};

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

	if ( list_rewind( feeder_load_row_list ) )
	do {
		feeder_load_row = list_get( feeder_load_row_list );

		feeder_row =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			feeder_row_new(
				feeder_account_name,
				account_uncleared_checks,
				feeder_phrase_list,
				feeder_exist_row_list,
				feeder_matched_journal_list,
				feeder_load_row,
				minimum_transaction_date_time );

		if ( feeder_row->transaction_date_time )
		{
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
		char *feeder_account_name,
		char *account_uncleared_checks,
		LIST *feeder_phrase_list,
		LIST *feeder_exist_row_list,
		LIST *feeder_matched_journal_list,
		FEEDER_LOAD_ROW *feeder_load_row,
		char *minimum_transaction_date_time )
{
	FEEDER_ROW *feeder_row;

	if ( !feeder_account_name
	||   !feeder_load_row )
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

	feeder_row->feeder_load_row = feeder_load_row;

	feeder_row->feeder_row_status = feeder_row_status_out_of_balance;

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
					feeder_account_name,
					account_uncleared_checks,
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
				feeder_account_name,
				feeder_row->feeder_phrase_seek,
				feeder_load_row->file_row_amount,
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

char *feeder_parameter_end_balance_error(
		double parameter_end_balance,
		boolean parameter_end_balance_not_null_boolean,
		FEEDER_ROW *feeder_row_first_out_balance,
		char *feeder_row_account_end_date,
		double feeder_row_account_end_balance )
{
	char error[ 256 ];

	if ( !parameter_end_balance_not_null_boolean
	||   float_dollar_virtually_same(
			parameter_end_balance,
			feeder_row_account_end_balance )
	||   feeder_row_first_out_balance
	||   !feeder_row_account_end_date )
	{
		return "";
	}

	sprintf(error,
"<h3>Warning: account end balance entered (%.2lf) doesn't match the calculated end balance (%.2lf).</h3>\n",
		parameter_end_balance,
		feeder_row_account_end_balance );

	return strdup( error );
}

int feeder_row_count( LIST *feeder_row_list )
{
	return list_length( feeder_row_list );
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
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\" feeder_row_number",
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
	FEEDER_ROW *feeder_row;

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
		if ( !*input ) continue;

		feeder_row =
			feeder_row_parse(
				input );

		if ( feeder_row ) list_set( list, feeder_row );
	}

	pclose( input_pipe );

	return list;
}

FILE *feeder_row_input_pipe( char *feeder_row_system_string )
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

	if ( !input || !*input ) return NULL;

	feeder_row = feeder_row_calloc();

	/* See FEEDER_ROW_SELECT */
	/* --------------------- */
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
	if ( *buffer ) feeder_row->street_address = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 6 );
	if ( *buffer ) feeder_row->transaction_date_time = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 7 );
	if ( *buffer ) feeder_row->file_row_description = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 8 );
	if ( *buffer ) feeder_row->file_row_amount = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 9 );
	if ( *buffer ) feeder_row->file_row_balance = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 10 );
	if ( *buffer ) feeder_row->calculate_balance = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 11 );
	if ( *buffer ) feeder_row->check_number = atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 12 );
	if ( *buffer ) feeder_row->feeder_phrase = strdup( buffer );

	return feeder_row;
}

FEEDER_AUDIT *feeder_audit_fetch(
		char *application_name,
		char *login_name,
		char *feeder_account_name,
		boolean reverse_order_boolean )
{
	FEEDER_AUDIT *feeder_audit;

	if ( !application_name
	||   !login_name
	||   !feeder_account_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	feeder_audit = feeder_audit_calloc();

	if ( ! ( feeder_audit->feeder_load_event =
			feeder_load_event_latest_fetch(
				FEEDER_LOAD_EVENT_TABLE,
				feeder_account_name ) ) )
	{
		return feeder_audit;
	}

	feeder_audit->feeder_row_final_number =
		feeder_row_final_number(
			FEEDER_ROW_TABLE,
			feeder_account_name,
			reverse_order_boolean,
			feeder_audit->
				feeder_load_event->
				feeder_load_date_time );

	if ( !feeder_audit->feeder_row_final_number )
	{
		return feeder_audit;
	}

	if ( ! ( feeder_audit->feeder_row_fetch =
			feeder_row_fetch(
				feeder_account_name,
				feeder_audit->
					feeder_load_event->
					feeder_load_date_time,
				feeder_audit->
					feeder_row_final_number
					/* feeder_row_number */ ) ) )
	{
		char message[ 256 ];

		snprintf(
			message,
			sizeof ( message ),
			"feeder_row_fetch(%s,%s,%d) returned empty.",
				feeder_account_name,
				feeder_audit->
					feeder_load_event->
					feeder_load_date_time,
				feeder_audit->
					feeder_row_final_number );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	feeder_audit->end_transaction_date_time =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		feeder_audit_end_transaction_date_time(
			TRANSACTION_DATE_CLOSE_TIME,
			feeder_audit->feeder_row_fetch->feeder_date );

	feeder_audit->journal_latest =
		journal_latest(
			JOURNAL_TABLE,
			feeder_account_name,
			feeder_audit->end_transaction_date_time,
			0 /* not fetch_transaction */,
			1 /* zero_balance_boolean */ );

	if ( !feeder_audit->journal_latest ) return feeder_audit;

	feeder_audit->account_fetch =
		account_fetch(
			feeder_account_name,
			1 /* fetch_subclassification */,
			1 /* fetch_element */ );

	feeder_audit->credit_balance_negate =
		feeder_audit_credit_balance_negate(
			feeder_audit->
				journal_latest->
				balance
				/* journal_balance */,
			feeder_audit->
				account_fetch->
				subclassification->
				element->
				accumulate_debit );

	feeder_audit->balance_difference =
		feeder_audit_balance_difference(
			feeder_audit->
				feeder_row_fetch->
				calculate_balance,
		feeder_audit->credit_balance_negate
			/* journal_balance */ );

	feeder_audit->difference_zero =
		feeder_audit_difference_zero(
			feeder_audit->balance_difference );

	feeder_audit->html_table =
		html_table_new(
			FEEDER_AUDIT_HTML_TITLE,
			(char *)0 /* sub_title */,
			(char *)0 /* sub_sub_title */ );

	feeder_audit->html_table->column_list =
		feeder_audit_html_column_list();

	feeder_audit->html_table->row_list = list_new();

	list_set(
		feeder_audit->html_table->row_list,
		feeder_audit_html_row(
			application_name,
			login_name,
			feeder_audit->feeder_row_fetch
				/* feeder_row */,
			feeder_audit->journal_latest
				/* journal */,
			feeder_audit->credit_balance_negate
				/* journal_balance */,
			feeder_audit->balance_difference,
			feeder_audit->difference_zero ) );

	return feeder_audit;
}

double feeder_audit_credit_balance_negate(
		double journal_balance,
		boolean element_accumulate_debit )
{
	if ( element_accumulate_debit )
		return journal_balance;
	else
		return -journal_balance;
}

double feeder_audit_balance_different(
		double calculate_balance,
		double journal_balance )
{
	return calculate_balance - journal_balance;
}

boolean feeder_audit_difference_zero( double balance_difference )
{
	return
	float_money_virtually_same(
		balance_difference,
		0.0 );
}

FEEDER_AUDIT *feeder_audit_calloc( void )
{
	FEEDER_AUDIT *feeder_audit;

	if ( ! ( feeder_audit = calloc( 1, sizeof ( FEEDER_AUDIT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return feeder_audit;
}

LIST *feeder_audit_html_column_list( void )
{
	LIST *list = list_new();

	list_set(
		list,
		html_column_new(
			"final_row_number",
			1 /* right_justify_boolean */ ) );

	list_set(
		list,
		html_column_new(
			"final_feeder_date",
			0 /* not right_justify_boolean */ ) );

	list_set(
		list,
		html_column_new(
			"feeder_row_full_name",
			0 /* not right_justify_boolean */ ) );

	list_set(
		list,
		html_column_new(
			"journal_full_name",
			0 /* not right_justify_boolean */ ) );

	list_set(
		list,
		html_column_new(
			"feeder_description",
			0 /* not right_justify_boolean */ ) );

	list_set(
		list,
		html_column_new(
			"feeder_row_transaction_date_time",
			0 /* not right_justify_boolean */ ) );

	list_set(
		list,
		html_column_new(
			"journal_transaction_date_time",
			0 /* not right_justify_boolean */ ) );

	list_set(
		list,
		html_column_new(
			"feeder_row_amount",
			1 /* right_justify_boolean */ ) );

	list_set(
		list,
		html_column_new(
			"calculate_balance",
			1 /* right_justify_boolean */ ) );

	list_set(
		list,
		html_column_new(
			"journal_balance",
			1 /* right_justify_boolean */ ) );

	list_set(
		list,
		html_column_new(
			"difference",
			1 /* right_justify_boolean */ ) );

	list_set(
		list,
		html_column_new(
			"status",
			0 /* not right_justify_boolean */ ) );

	return list;
}

HTML_ROW *feeder_audit_html_row(
		char *application_name,
		char *login_name,
		FEEDER_ROW *feeder_row,
		JOURNAL *journal,
		double journal_balance,
		double feeder_audit_balance_difference,
		boolean feeder_audit_difference_zero )
{
	LIST *cell_list;

	if ( !application_name
	||   !login_name
	||   !feeder_row
	||   !journal )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list =
		feeder_audit_html_cell_list(
			feeder_row->feeder_row_number,
			feeder_row->full_name,
			feeder_row->file_row_description,
			feeder_row->transaction_date_time,
			date_convert_return_date_string(
				date_convert_international
					/* source_enum */,
				date_convert_login_name_enum(
					application_name,
					login_name )
					/* destination_enum */,
				feeder_row->feeder_date
					/* source_date_string */)
				/* feeder_date */,
			feeder_row->file_row_amount,
			feeder_row->calculate_balance,
			journal->full_name,
			journal->transaction_date_time,
			journal_balance,
			feeder_audit_balance_difference,
			feeder_audit_difference_zero );

	return
	html_row_new( cell_list );
}

double feeder_audit_balance_difference(
		double calculate_balance,
		double journal_balance )
{
	return calculate_balance - journal_balance;
}

LIST *feeder_audit_html_cell_list(
		int feeder_row_number,
		char *feeder_row_full_name,
		char *file_row_description,
		char *feeder_row_transaction_date_time,
		char *feeder_date,
		double file_row_amount,
		double calculate_balance,
		char *journal_full_name,
		char *journal_transaction_date_time,
		double journal_balance,
		double balance_difference,
		boolean difference_zero )
{
	LIST *list;
	char cell_string[ 128 ];

	if ( !feeder_row_number
	||   !feeder_row_full_name
	||   !file_row_description
	||   !feeder_row_transaction_date_time
	||   !feeder_date
	||   !journal_full_name
	||   !journal_transaction_date_time )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	list = list_new();

	sprintf(cell_string,
		"%d",
		feeder_row_number );

	list_set(
		list,
		html_cell_new(
			strdup( cell_string ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		list,
		html_cell_new(
			feeder_date,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		list,
		html_cell_new(
			feeder_row_full_name,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		list,
		html_cell_new(
			journal_full_name,
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
			feeder_row_transaction_date_time,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		list,
		html_cell_new(
			journal_transaction_date_time,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		list,
		html_cell_new(
			strdup(
				/* ---------------------- */
				/* Returns static memory. */
				/* Doesn't trim pennies.  */
				/* ---------------------- */
				string_commas_money(
					file_row_amount ) ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		list,
		html_cell_new(
			strdup(
				/* ---------------------- */
				/* Returns static memory. */
				/* Doesn't trim pennies.  */
				/* ---------------------- */
				string_commas_money(
					calculate_balance ) ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		list,
		html_cell_new(
			strdup(
				/* ---------------------- */
				/* Returns static memory. */
				/* Doesn't trim pennies.  */
				/* ---------------------- */
				string_commas_money(
					journal_balance ) ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		list,
		html_cell_new(
			strdup(
				/* ---------------------- */
				/* Returns static memory. */
				/* Doesn't trim pennies.  */
				/* ---------------------- */
				string_commas_money(
					balance_difference ) ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	if ( difference_zero )
	{
		list_set(
			list,
			html_cell_new(
				"Okay",
				0 /* not large_boolean */,
				0 /* not bold_boolean */ ) );
	}
	else
	{
		list_set(
			list,
			html_cell_new(
				"permenant difference",
				1 /* large_boolean */,
				1 /* bold_boolean */ ) );
	}

	return list;
}

char *feeder_matched_journal_check_update_statement(
		char *journal_table,
		char *feeder_account,
		char *account_uncleared_checks,
		char *full_name,
		char *street_address,
		char *transaction_date_time )
{
	char update_statement[ 1024 ];

	if ( !journal_table
	||   !feeder_account
	||   !account_uncleared_checks
	||   !full_name
	||   !street_address
	||   !transaction_date_time )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(update_statement,
		"update %s set account = '%s' where %s;",
		journal_table,
		feeder_account,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		journal_primary_where(
			full_name,
			street_address,
			transaction_date_time,
			account_uncleared_checks ) );

	return strdup( update_statement );
}

void feeder_row_check_journal_update(
		LIST *feeder_row_list,
		FEEDER_ROW *feeder_row_first_out_balance )
{
	FILE *update_pipe;
	FEEDER_ROW *feeder_row;

	if ( !list_rewind( feeder_row_list ) ) return;

	update_pipe = feeder_row_check_journal_update_pipe();

	do {
		feeder_row = list_get( feeder_row_list );

		if ( feeder_row == feeder_row_first_out_balance )
			break;

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
		char *feeder_row_table,
		char *feeder_account_name,
		char *feeder_load_date_time )
{
	char system_string[ 1024 ];

	if ( !feeder_row_table
	||   !feeder_account_name )
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

	sprintf(system_string,
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
		char *feeder_row_table,
		char *feeder_account_name,
		char *feeder_load_date_time )
{
	char system_string[ 1024 ];

	if ( !feeder_row_table
	||   !feeder_account_name )
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

	sprintf(system_string,
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

char *feeder_row_final_number_select( boolean reverse_order_boolean )
{
	if ( reverse_order_boolean )
		return "min( feeder_row_number )";
	else
		return "max( feeder_row_number )";
}

int feeder_row_final_number(
		char *feeder_row_table,
		char *feeder_account_name,
		boolean reverse_order_boolean,
		char *feeder_load_date_time )
{
	char system_string[ 1024 ];

	if ( !feeder_row_table
	||   !feeder_account_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !feeder_load_date_time ) return 0;

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		feeder_row_final_number_select(
			reverse_order_boolean ),
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
	string_atoi( string_pipe_fetch( system_string ) );
}

FEEDER_ROW *feeder_row_fetch(
		char *feeder_account_name,
		char *feeder_load_date_time,
		int feeder_row_number )
{
	return
	feeder_row_parse(
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		string_pipe_fetch(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			feeder_row_system_string(
				FEEDER_ROW_SELECT,
				FEEDER_ROW_TABLE,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				feeder_row_primary_where(
					feeder_account_name,
					feeder_load_date_time,
					feeder_row_number ) ) ) );
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
		char *feeder_account_name,
		char *feeder_load_date_time,
		char *account_uncleared_checks )
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
			minimum_transaction_date_time,
			feeder_account_name );

	if ( journal_propagate )
	{
		journal_list_update(
			journal_propagate->
				update_statement_list );
	}

	journal_propagate =
		journal_propagate_new(
			minimum_transaction_date_time,
			account_uncleared_checks );

	if ( journal_propagate )
	{
		journal_list_update(
			journal_propagate->
				update_statement_list );
	}
}

char *feeder_phrase_where( void )
{
	return "full_name is not null";
}

char *feeder_load_file_filename(
		char *application_name,
		char *feeder_load_filename,
		char *upload_directory )
{
	static char filename[ 128 ];

	if ( !application_name
	||   !feeder_load_filename
	||   !upload_directory )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(filename,
		"%s/%s/%s",
		upload_directory,
		application_name,
		feeder_load_filename );

	return filename;
}

void feeder_load_row_balance_set(
		LIST *feeder_load_row_list /* in/out */,
		double parameter_end_balance )
{
	FEEDER_LOAD_ROW *feeder_load_row;

	if ( list_tail( feeder_load_row_list ) )
	do {
		feeder_load_row =
			list_get(
				feeder_load_row_list );

		feeder_load_row->file_row_balance = parameter_end_balance;

		parameter_end_balance -=
			feeder_load_row->file_row_amount;

	} while ( list_previous( feeder_load_row_list ) );
}

void feeder_row_status_set(
		LIST *feeder_row_list /* in/out */,
		int balance_column,
		boolean parameter_end_balance_not_null_boolean,
		FEEDER_ROW *feeder_row_first_out_balance )
{
	FEEDER_ROW *feeder_row;

	if ( list_rewind( feeder_row_list ) )
	do {
		feeder_row = list_get( feeder_row_list );

		if ( feeder_row == feeder_row_first_out_balance ) break;

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
				balance_column,
				parameter_end_balance_not_null_boolean,
				feeder_row->
					feeder_load_row->
					file_row_balance,
				feeder_row->calculate_balance );

	} while ( list_next( feeder_row_list ) );
}

enum feeder_row_status feeder_row_status_evaluate(
		int balance_column,
		boolean parameter_end_balance_not_null_boolean,
		double file_row_balance,
		double calculate_balance )
{
	if ( !balance_column
	&&   !parameter_end_balance_not_null_boolean )
	{
		return feeder_row_status_cannot_determine;
	}

	if ( float_money_virtually_same(
		file_row_balance,
		calculate_balance ) )
	{
		return feeder_row_status_okay;
	}
	else
	{
		return feeder_row_status_out_of_balance;
	}
}

void feeder_execute(
		char *process_name,
		FEEDER *feeder )
{
	if ( !process_name
	||   !feeder )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	/* ---------------------------- */
	/* May reset			*/
	/* feeder_row->			*/
	/*	feeder_transaction->	*/
	/*	transaction_date_time	*/
	/* ---------------------------- */
	feeder_row_transaction_insert(
		feeder->feeder_row_list,
		feeder->feeder_row_first_out_balance );

	feeder_row_check_journal_update(
		feeder->feeder_row_list,
		feeder->feeder_row_first_out_balance );

	feeder_row_journal_propagate(
		feeder->feeder_account_name,
		feeder->
			feeder_load_event->
			feeder_load_date_time,
		feeder->account_uncleared_checks );

	feeder_load_event_insert(
		FEEDER_LOAD_EVENT_TABLE,
		FEEDER_LOAD_EVENT_INSERT,
		feeder->
			feeder_load_event->
			feeder_account_name,
		feeder->
			feeder_load_event->
			feeder_load_date_time,
		feeder->
			feeder_load_event->
			login_name,
		feeder->
			feeder_load_event->
			feeder_load_filename,
		feeder->
			feeder_load_event->
			feeder_row_account_end_date,
		feeder->
			feeder_load_event->
			feeder_row_account_end_balance );

	feeder_row_list_insert(
		feeder->feeder_account_name,
		feeder->
			feeder_load_event->
			feeder_load_date_time,
		feeder->feeder_row_list,
		feeder->feeder_row_first_out_balance );

	printf( "%s\n",
		feeder->parameter_end_balance_error );

	printf( "<h3>Execute feeder row count: %d</h3>\n",
		feeder->feeder_row_count );

	process_execution_count_increment(
		process_name );
}

void feeder_display(
		boolean reverse_order_boolean,
		FEEDER *feeder )
{
	if ( !feeder )
	{
		char message[ 128 ];

		sprintf(message, "feeder is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( feeder->feeder_row_first_out_balance )
	{
		printf( "<h1>Feeder Error Table</h1>\n" );

		feeder_row_error_display(
			reverse_order_boolean,
			feeder->feeder_row_list );
	}

	printf( "<h1>Transaction Table</h1>\n" );

	feeder_row_list_display(
		reverse_order_boolean,
		feeder->feeder_row_list );

	printf( "%s\n",
		feeder->parameter_end_balance_error );

	printf( "<h3>Non-execute feeder row count: %d</h3>\n",
		feeder->feeder_row_count );
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

char *feeder_audit_end_transaction_date_time(
		const char *transaction_date_close_time,
		char *feeder_date )
{
	static char end_transaction_date_time[ 32 ];

	if ( !feeder_date )
	{
		char message[ 128 ];

		sprintf(message, "date_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		end_transaction_date_time,
		sizeof ( end_transaction_date_time ),
		"%s %s",
		feeder_date,
		transaction_date_close_time );

	return end_transaction_date_time;
}

char *feeder_row_display( FEEDER_ROW *feeder_row )
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
		"row_number=%d, date=%s, description=%s, amount=%.2lf",
		feeder_row->feeder_load_row->feeder_row_number,
		feeder_row->feeder_load_row->international_date,
		feeder_row->feeder_load_row->file_row_description,
		feeder_row->feeder_load_row->file_row_amount );

	ptr += sprintf(
		ptr,
		", status=%s\n",
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		feeder_row_status_string(
			feeder_row->feeder_row_status ) );

	return strdup( display );
}

char *feeder_load_row_description_space_trim( char *file_row_description )
{
	if ( !file_row_description )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"file_row_description is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	strdup(
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		sed_trim_double_spaces(
			file_row_description ) );
}
