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
#include "timlib.h"
#include "date_convert.h"
#include "sql.h"
#include "session.h"
#include "basename.h"
#include "sed.h"
#include "account.h"
#include "environ.h"
#include "feeder.h"

LIST *feeder_phrase_list( void )
{
	LIST *list;
	FILE *input_pipe;
	char input[ 1024 ];

	input_pipe =
		popen(
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
	char buffer[ 128 ];
	FEEDER_PHRASE *feeder_phrase;

	if ( !input || !*input ) return (FEEDER_PHRASE *)0;

	/* See FEEDER_PHRASE_SELECT */
	/* ------------------------ */
	piece( buffer, SQL_DELIMITER, input, 0 );
	feeder_phrase = feeder_phrase_new( strdup( buffer ) );

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

char *feeder_load_file_row_trim_bank_date( char *description )
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

FEEDER_LOAD_FILE_ROW *feeder_load_file_row_new(
			DATE *feeder_load_file_date /* in/out */,
			int date_column /* one_based */,
			int description_column /* one_based */,
			int debit_column /* one_based */,
			int credit_column /* one_based */,
			int balance_column /* one_based */,
			LIST *feeder_phrase_list,
			LIST *feeder_load_table_row_list,
			char *input,
			int line_number )
{
	FEEDER_LOAD_FILE_ROW *feeder_load_file_row;
	char buffer[ 512 ];

	if ( !date_column
	||   !description_column
	||   !input
	||   !*input
	||   !line_number )
	{
		return (FEEDER_LOAD_FILE_ROW *)0;
	}

	feeder_load_file_row = feeder_load_file_row_calloc();

	feeder_load_file_row->line_number = line_number;

	feeder_load_file_row->american_date =
		strdup(
			piece_quote_comma(
				buffer,
				input,
				date_column - 1 ) );

	if ( ! ( feeder_load_file_row->international_date =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			feeder_load_file_row_international_date(
				feeder_load_file_row->american_date ) ) )
	{
		free( feeder_load_file_row->american_date );
		free( feeder_load_file_row );
		return (FEEDER_LOAD_FILE_ROW *)0;
	}

	feeder_load_file_row->feeder_description =
		strdup(
			piece_quote_comma(
				buffer,
				input,
				description_column - 1 ) );

	if ( !credit_column )
	{
		feeder_load_file_row->amount =
			atof(
				piece_quote_comma(
					buffer,
					input,
					debit_column - 1 ) );
	}
	else
	{
		feeder_load_file_row->debit =
			atof(
				piece_quote_comma(
					buffer,
					input,
					debit_column - 1 ) );

		feeder_load_file_row->credit =
			atof(
				piece_quote_comma(
					buffer,
					input,
					credit_column - 1 ) );

		feeder_load_file_row->amount =
			feeder_load_file_row_amount(
				feeder_load_row->debit,
				feeder_load_row->credit );
	}

	if ( balance_column )
	{
		feeder_load_file_row->balance =
			atof(
				piece_quote_comma(
					buffer,
					input,
					balance_column - 1 ) );
	}

	feeder_load_file_row->check_number =
		feeder_load_file_row_check_number(
			feeder_load_file_row->feeder_description );

	feeder_load_file_row->description_embedded =
		/* ---------------------------------- */
		/* Returns heap memory or description */
		/* ---------------------------------- */
		feeder_load_file_row_description_embedded(
			feeder_load_file_row->feeder_description,
			feeder_load_file_row->balance,
			line_number,
			feeder_load_file_row->check_number );

	feeder_load_file_row->feeder_load_exist_row_seek =
		feeder_load_exist_row_seek(
			feeder_load_file_row->international_date,
			feeder_load_file_row->description_embedded,
			feeder_load_exist_row_list );

	if ( feeder_load_file_row->feeder_load_exist_row_seek )
	{
		return feeder_load_file_row;
	}

	feeder_load_file_row->feeder_phrase_seek =
		feeder_phrase_seek(
			feeder_load_file_row->feeder_description,
			feeder_phrase_list );

	if ( feeder_load_file_row->feeder_phrase_seek )
	{
		feeder_load_file_row->transaction_date_time =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			feeder_load_file_row_transaction_date_time(
				feeder_load_file_row->international_date,
				date_hms(
					feeder_load_file_row->
						feeder_load_file_date ) );

		date_increment_seconds(
			feeder_load_file_row->feeder_load_file_date,
			1 );
	}

	return feeder_load_file_row;
}

FEEDER_LOAD_FILE_ROW *feeder_load_file_row_calloc( void )
{
	FEEDER_LOAD_FILE_ROW *feeder_load_file_row;

	if ( ! ( feeder_load_file_row =
			calloc( 1, sizeof( FEEDER_LOAD_FILE_ROW ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return feeder_load_file_row;
}

double feeder_load_file_row_amount(
			double debit,
			double credit )
{
	return debit - credit;
}

char *feeder_load_file_row_description_embedded(
			char *description,
			double balance,
			int line_number,
			int check_number )
{
	if ( check_number ) return description;

	return
	/* ---------------------------------------------- */
	/* Returns feeder_load_file_row_description_build */
	/* ---------------------------------------------- */
	feeder_load_file_row_description_crop(
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_load_file_row_description_build(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			sed_trim_double_spaces(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				feeder_load_row_trim_bank_date(
					description ) ),
			balance,
			line_number ),
		FEEDER_DESCRIPTION_SIZE );
}

char *feeder_load_row_file_description_build(
			char *sed_trim_double_spaces,
			double balance,
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
			line_number );
	}

	return strdup( build );
}

char *feeder_load_file_row_description_crop(
			char *description_build,
			int feeder_description_size )
{
	if ( string_strlen( description_build ) > feeder_description_size )
	{
		*( description_build + feeder_description_size ) = '\0';
	}

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

char *feeder_load_file_row_international_date( char *american_date )
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

FEEDER_LOAD_FILE *feeder_load_file_fetch(
			char *feeder_load_filename,
			int date_column /* one_based */,
			int description_column /* one_based */,
			int debit_column /* one_based */,
			int credit_column /* one_based */,
			int balance_column /* one_based */,
			boolean reverse_order )
{
	FEEDER_LOAD_FILE *feeder_load_file;

	if ( !feeder_load_filename
	||   !date_column
	||   !description_column )
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

	feeder_load_file->sha256sum =
		feeder_load_file_sha256sum(
			feeder_load_filename );

	feeder_load_file->feeder_load_event_sha256sum_exists =
		feeder_load_event_sha256sum_exists(
			FEEDER_LOAD_EVENT_TABLE,
			feeder_load_file->sha256sum );

	feeder_load_file->feeder_load_file_date =
		date_now_new(
			date_utc_offset() );

	if ( ! ( feeder_load_file->file =
			fopen( feeder_load_filename, "r" ) ) )
	{
		free( feeder_load_file );
		return (FEEDER_LOAD_FILE *)0;
	}

	feeder_load_file->feeder_load_row_list = list_new();

	while ( string_input( input, file, 1024 ) )
	{
		feeder_load_file->remove_character =
			remove_character( input, '\\' );

		feeder_load_file->feeder_load_row =
			feeder_load_row_new(
				feeder_load_file->feeder_load_file_date
					/* in/out */,
				date_column,
				description_column,
				debit_column,
				credit_column,
				balance_column,
				feeder_phrase_list,
				feeder_load_exist_row_list,
				feeder_load_file->remove_character,
				++line_number );

		if ( !feeder_load_file->feeder_load_row ) continue;

		if ( reverse_order )
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

	fclose( feeder_load_file->file );

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
			char *feeder_load_date_time,
			char *login_name,
			char *basename_filename,
			char *feeder_account,
			double ending_balance,
			char *feeder_load_sha256sum )
{
	FEEDER_LOAD_EVENT *feeder_load_event;

	if ( !feeder_load_date_time
	||   !login_name
	||   !basename_filename
	||   !feeder_account
	||   !feeder_load_sha256sum )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	feeder_load_event = feeder_load_event_calloc();

	feeder_load_event->feeder_load_date_time = feeder_load_date_time;
	feeder_load_event->login_name = login_name;
	feeder_load_event->basename_filename = basename_filename;
	feeder_load_event->feeder_account = feeder_account;
	feeder_load_event->ending_balance = ending_balance;
	feeder_load_event->sha256sum = feeder_load_sha256sum;

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
			char *feeder_load_select,
			char *feeder_load_date_time,
			char *login_name,
			char *basename_filename,
			char *sha256sum,
			char *feeder_account,
			double ending_balance )
{
	FILE *insert_pipe;

	if ( !feeder_load_event_table
	||   !feeder_load_select
	||   !feeder_load_date_time
	||   !login_name
	||   !basename_filename
	||   !sha256sum
	||   !feeder_account )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	system_string =

	insert_pipe =
		popen(
			/* Returns heap memory */
			/* ------------------- */
			feeder_load_event_insert_system_string(
				feeder_load_event_table,
				feeder_load_select,
				SQL_DELIMITER ),
			"w" );

	feeder_load_event_insert_pipe(
		insert_pipe,
		SQL_DELIMITER,
		feeder_load_date_time,
		login_name,
		basename_filename,
		sha256sum,
		feeder_account,
		ending_balance );

	pclose( insert_pipe );
}

char *feeder_load_event_insert_system_string(
			char *feeder_load_event_table,
			char *feeder_load_select,
			char sql_delimiter )
{
	char system_string[ 1024 ];

	if ( !feeder_load_event_table
	||   !feeder_load_select
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
	 	"html_paragraph_wrapper.e				   ",
	 	feeder_load_event_table,
	 	feeder_load_select,
	 	sql_delimiter );

	return strdup( system_string );
}

void feeder_load_event_insert_pipe(
			FILE *insert_pipe,
			char sql_delimiter,
			char *feeder_load_date_time,
			char *login_name,
			char *basename_filename,
			char *sha256sum,
			char *feeder_account,
			double ending_balance )
{
	if ( !insert_pipe
	||   !sql_delimiter
	||   !feeder_load_date_time
	||   !login_name
	||   !basename_filename
	||   !sha256sum
	||   !feeder_account )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		pclose( insert_pipe );
		exit( 1 );
	}

	fprintf(insert_pipe,
		"%s%c%s%c%s%c%s%c%s%c%.2lf\n",
	 	feeder_load_date_time,
		sql_delimiter,
	 	login_name,
		sql_delimiter,
		basename_filename,
		sql_delimiter,
		sha256sum,
		sql_delimiter,
		feeder_account,
		sql_delimiter,
		ending_balance );
}

char *feeder_load_basename_filename( char *feeder_load_filename )
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

FEEDER_LOAD *feeder_load_new(
			char *process_name,
			char *login_name,
			char *feeder_account,
			char *feeder_load_filename,
			int date_column /* one_based */,
			int description_column /* one_based */,
			int debit_column /* one_based */,
			int credit_column /* one_based */,
			int balance_column /* one_based */,
			boolean reverse_order,
			double ending_balance )
{
	FEEDER_LOAD *feeder_load;

	if ( !process_name
	||   !login_name
	||   !feeder_account
	||   strcmp( feeder_account, "feeder_account" ) == 0
	||   !feeder_load_filename
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

	feeder_load = feeder_load_calloc();

	feeder_load->feeder_load_file =
		feeder_load_file_fetch(
			feeder_load_filename,
			date_column,
			description_column,
			debit_column,
			credit_column,
			balance_column,
			reverse_order );

	if ( !feeder_load->feeder_load_file
	||   !list_length(
		feeder_load->
			feeder_load_file->
			feeder_load_row_list ) )
	{
		free( feeder_load );
		return (FEEDER_LOAD *)0;
	}

	feeder_load->feeder_phrase_list = feeder_phrase_list();

	feeder_load->feeder_load_row_last_running_balance =
		feeder_load_row_last_running_balance(
			reverse_order,
			feeder_load->feeder_load_file->feeder_load_row_list );

	feeder_load->basename_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_load_basename_filename(
			feeder_load_filename );

	feeder_load->sha256sum =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_load_sha256sum(
			feeder_load_filename );

	feeder_load->feeder_load_event =
		feeder_load_event_new(
			date_display_19(
				date_now_new(
					date_utc_offset()
						/* feeder_load_date_time */ ) ),
			login_name,
			feeder_account,
			feeder_load->basename_filename,
			ending_balance,
			feeder_load->feeder_load_row_last_running_balance,
			feeder_load->sha256sum );

	return feeder_load;
}

FEEDER_LOAD *feeder_load_calloc( void )
{
	FEEDER_LOAD *feeder_load;

	if ( ! ( feeder_load = calloc( 1, sizeof( FEEDER_LOAD ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return feeder_load;
}

char *feeder_load_sha256sum( char *feeder_load_filename )
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

		for(	piece_number = 0;
			piece(	feeder_component,
				'|',
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

LIST *feeder_load_table_row_list(
			char *feeder_account,
			char *feeder_load_file_minimum_date )
{
	LIST *list;
	char *system_string;
	FILE *input_pipe;
	char input[ 512 ];

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


	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_load_table_row_system_string(
			FEEDER_LOAD_TABLE_ROW_SELECT,
			FEEDER_LOAD_ROW_TABLE,
			/* ------------------- */
			/* Returns static memory */
			/* --------------------- */
			feeder_load_table_row_where(
				feeder_account,
				feeder_load_file_minimum_date ) );

	list = list_new()

	input_pipe = popen( system_string, "r" );

	while ( string_input( input, input_pipe, 512 ) )
	{
		list_set(
			list,
			feeder_load_table_row_parse(
				input ) );
	}

	pclose( input_pipe );
	free( system_string );

	return list;
}

char *feeder_load_table_row_where(
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

char *feeder_load_table_row_system_string(
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
		"select.sh \"%s\" %s \"%s\"",
		select,
		table,
		where );

	return strdup( system_string );
}

FEEDER_LOAD_TABLE_ROW *feeder_load_table_row_parse( char *input )
{
	FEEDER_LOAD_TABLE_ROW *feeder_load_table_row;
	char buffer[ 128 ];

	if ( !input || !*input ) return (FEEDER_LOAD_TABLE_ROW *)0;

	feeder_load_table_row = feeder_load_table_row_calloc();

	/* See FEEDER_LOAD_TABLE_ROW_SELECT */
	/* -------------------------------- */
	piece( buffer, SQL_DELIMITER, input, 0 );
	feeder_load_table_row->feeder_date = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 1 );
	feeder_load_table_row->feeder_description = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	feeder_load_table_row->transaction_date_time = strdup( buffer );

	return feeder_load_table_row;
}

FEEDER_LOAD_TABLE_ROW *feeder_load_table_row_calloc( void )
{
	FEEDER_LOAD_TABLE_ROW *feeder_load_table_row;

	if ( ! ( feeder_load_table_row =
			calloc( 1, sizeof( FEEDER_LOAD_TABLE_ROW ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return feeder_load_table_row;
}

FEEDER_LOAD_TABLE_ROW *feeder_load_table_row_seek(
			char *international_date,
			char *description_embedded,
			LIST *feeder_load_table_row_list )
{
	FEEDER_LOAD_TABLE_ROW *feeder_load_table_row;

	if ( !international_date
	||   !description_embeded )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( feeder_load_table_row_list ) )
		return (FEEDER_LOAD_TABLE_ROW *)0;

	do {
		feeder_load_table_row =
			list_get(
				feeder_load_table_row_list );

		if ( strcmp(	feeder_load_table_row->feeder_date,
				internation_date ) == 0
		&&   strcmp(	feeder_load_table_row->feeder_description,
				description_embedded ) == 0 )
		{
			return feeder_load_table_row;
		}

	} while ( list_next( feeder_load_table_row_list ) );

	return (FEEDER_LOAD_TABLE_ROW *)0;
}

FEEDER_TRANSACTION *feeder_transaction_new(
			DATE *feeder_load_date,
			char *feeder_account,
			FEEDER_LOAD_FILE_ROW *feeder_load_file_row,
			LIST *feeder_journal_list )
{
	FEEDER_TRANSACTION *feeder_transaction;

	if ( !feeder_load_date
	||   !feeder_account
	||   !feeder_load_file_row
	||   !feeder_load_file_row->amount )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	feeder_transaction = feeder_transaction_calloc();

	if ( list_length( feeder_journal_list ) )
	{
		feeder_transaction->feeder_journal_row_seek =
			feeder_journal_row_seek(
				feeder_load_file_row,
				feeder_journal_list );

		if ( feeder_transaction->feeder_journal_row_seek )
		{
			feeder_transaction->feeder_journal_row_seek->taken = 1;
			free( feeder_transaction );
			return (FEEDER_TRANSACTION *)0;
		}
	}

	if ( !feeder_load_file_row->feeder_phrase )
	{
		free( feeder_transaction );
		return (FEEDER_TRANSACTION *)0;
	}

	if ( feeder_load_file_row->amount < 0.0 )
	{
		feeder_transaction->debit_account =
			feeder_load_file_row->
				feeder_phrase->
				feeder_account;

		feeder_transaction->credit_account = feeder_account;
	}
	else
	{
		feeder_transaction->debit_account = feeder_account;

		feeder_transaction->credit_account =
			feeder_load_file_row->
				feeder_phrase->
				feeder_account;
	}

	feeder_transaction->transaction =
		transaction_binary(
			feeder_load_file_row->
				feeder_phrase->
				full_name,
			feeder_load_file_row->
				feeder_phrase->
				street_address,
			date_display_19( feeder_load_date )
				/* transaction_date_time */,
			feeder_load_file_row->amount
				/* transaction_amount */,
			(char *)0 /* memo */,
			debit_account,
			credit_account );

	date_increment_seconds( feeder_load_date, 1 );
	feeder_transaction->feeder_load_file_row = feeder_load_file_row;

	return feeder_transaction;
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

FEEDER_TRANSACTION *feeder_transaction_row_seek(
			FEEDER_LOAD_FILE_ROW *feeder_load_file_row,
			LIST *feeder_transaction_list )
{
	FEEDER_TRANSACTION *feeder_transaction;

	if ( !list_rewind( feeder_transaction_list ) )
		return (FEEDER_TRANSACTION *)0;

	do {
		feeder_transaction =
			list_get(
				feeder_transaction_list );

		if (	feeder_transaction->feeder_load_file_row ==
			feeder_load_file_row )
		{
			return feeder_transaction;
		}

	} while ( list_next( feeder_transaction_list ) );

	return (FEEDER_TRANSACTION *)0;
}

FEEDER_TRANSACTION_LIST *feeder_transaction_list_new(
			DATE *feeder_load_date,
			char *feeder_account,
			LIST *feeder_load_row_list,
			LIST *feeder_journal_list )
{
	FEEDER_TRANSATION_LIST *feeder_transaction_list;
	FEEDER_LOAD_FILE_ROW *feeder_load_file_row;

	if ( !feeder_load_date
	||   !feeder_account )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( feeder_load_row_list ) )
	{
		return (FEEDER_TRANSACTION_LIST *)0;
	}

	feeder_transaction_list = feeder_transaction_list_calloc();

	feeder_transaction_list->list = list_new();

	do {
		feeder_load_file_row =
			list_get(
				feeder_load_file_row_list );

		list_set(
			feeder_transaction_list->list,
			feeder_transaction_new(
				feeder_load_date /* in/out */,
				feeder_account,
				feeder_load_file_row,
				feeder_journal_list ) );

	} while ( list_next( feeder_load_file_row_list ) );

	return feeder_transaction_list;
}

FEEDER_TRANSACTION_LIST *feeder_transaction_list_calloc( void )
{
	FEEDER_TRANSACTION_LIST *feeder_transaction_list;

	if ( ! ( feeder_transaction_list =
			calloc( 1, sizeof( FEEDER_TRANSACTION_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return feeder_transaction_list;
}

FEEDER_JOURNAL *feeder_journal_new(
			LIST *feeder_load_row_list,
			JOURNAL *journal )
{
	FEEDER_JOURNAL *feeder_journal;

	if ( !list_length( feeder_load_row_list ) )
		return (FEEDER_JOURNAL *)0;

	if ( !journal )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: journal is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	feeder_journal = feeder_journal_calloc();

	if ( journal->check_number )
	{
		feeder_journal->feeder_load_file_row =
			feeder_load_file_row_check_number_seek(
				journal->check_number,
				feeder_load_file_row_list );
	}

	if ( !feeder_journal->feeder_load_file_row )
	{
		feeder_journal->feeder_load_file_row =
			feeder_load_file_row_amount_seek(
				journal->debit_amount,
				journal->credit_amount,
				feeder_load_file_row_list );
	}

	if ( !feeder_journal->feeder_load_file_row )
	{
		free( feeder_journal );
		return (FEEDER_JOURNAL *)0;
	}

	feeder_journal->journal = journal;

	return feeder_journal;
}

FEEDER_JOURNAL *feeder_journal_calloc( void )
{
	FEEDER_JOURNAL *feeder_journal;

	if ( ! ( feeder_journal = calloc( 1, sizeof( FEEDER_JOURNAL ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return feeder_journal;
}

FEEDER_JOURNAL *feeder_journal_row_seek(
			FEEDER_LOAD_FILE_ROW *feeder_load_file_row,
			LIST *feeder_journal_list )
{
	FEEDER_JOURNAL *feeder_journal;

	if ( !feeder_load_file_row )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: feeder_load_file_row is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( feeder_journal_list ) )
		return (FEEDER_JOURNAL *)0;

	do {
		feeder_journal = list_get( feeder_journal_list );

		if ( feeder_journal->taken ) continue;

		if (	feeder_journal->feeder_load_file_row ==
			feeder_load_file_row )
		{
			return feeder_journal;
		}

	} while ( list_next( feeder_journal_list ) );

	return (FEEDER_JOURNAL *)0;
}

FEEDER_NOT_MATCHED_JOURNAL_LIST *
	feeder_not_matched_journal_list_new(
			char *feeder_account,
			LIST *feeder_load_file_row_list )
{
	FEEDER_NOT_MATCHED_JOURNAL_LIST *feeder_not_matched_journal_list;

	if ( !feeder_account )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: feeder_account is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	feeder_not_matched_journal_list =
		feeder_not_matched_journal_list_calloc();

	if ( !list_length( feeder_load_file_row_list ) )
	{
		feeder_not_matched_journal_list;
	}

	feeder_not_matched_journal_list->subquery =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_not_matched_journal_list_subquery(
			feeder_account,
			account_uncleared_checks(
				ACCOUNT_UNCLEARED_CHECKS_KEY ),
			JOURNAL_TABLE,
			FEEDER_LOAD_ROW_TABLE );

	feeder_not_matched_journal_list->where =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_not_matched_journal_list_where(
			feeder_account,
			account_uncleared_checks(
				ACCOUNT_UNCLEARED_CHECKS_KEY ),
			feeder_not_matched_journal_list->subquery );

	feeder_not_matched_journal_list->journal_system_list =
		journal_system_list(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			journal_system_string(
				JOURNAL_SELECT,
				JOURNAL_TABLE,
			feeder_not_matched_journal_list->where ),
		1 /* fetch_check_number */,
		0 /* not fetch_memo */ );

	if ( !list_rewind( feeder_not_matched_journal_list->
				journal_system_list  ) )
	{
		feeder_not_matched_journal_list;
	}

	feeder_not_matched_journal_list->feeder_journal_list = list_new();

	do {
		feeder_not_matched_journal_list->journal =
			list_get(
				feeder_not_matched_journal_list->
					journal_system_list );

		list_set(
			feeder_not_matched_journal_list->feeder_journal_list,
			feeder_journal_new(
				feeder_load_file_row_list,
				feeder_not_matched_journal_list->journal ) );

	} while ( list_next(
			feeder_not_matched_journal_list->
				journal_system_list ) );

	return feeder_not_matched_journal_list;
}

FEEDER_NOT_MATCHED_JOURNAL_LIST *
	feeder_not_matched_journal_list_calloc(
			void )
{
	FEEDER_NOT_MATCHED_JOURNAL_LIST *feeder_not_matched_journal_list;

	if ( ! ( feeder_not_matched_journal_list =
			calloc(	1,
				sizeof( FEEDER_NOT_MATCHED_JOURNAL_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return feeder_not_matched_journal_list;
}

char *feeder_not_matched_journal_list_subquery(
			char *feeder_account,
			char *account_uncleared_checks,
			char *journal_table,
			char *feeder_load_row_table )
{
	char subquery[ 512 ];

	sprintf(subquery,
		"not exists						"
		"(select 1 from %s					"
		"	where %s.full_name =				"
		"	      %s.full_name and				"
		"	      %s.street_address =			"
		"	      %s.street_address and			"
		"	      %s.transaction_date_time =		"
		"	      %s.transaction_date_time and		"
		"	      %s.feeder_account in ('%s','%s')		",
		feeder_load_row_table,
		feeder_load_row_table,
		journal_table,
		feeder_load_row_table,
		journal_table,
		feeder_load_row_table,
		journal_table,
		feeder_load_row_table,
		feeder_account,
		account_uncleared_checks_key );

	return strdup( subquery );
}

char *feeder_not_matched_journal_list_where(
			char *feeder_account,
			char *account_uncleared_checks,
			char *subquery )
{
	char where[ 1024 ];
	char *timriley_where;

	if ( !feeder_account
	||   !account_uncleared_checks
	||   !subquery )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( strcmp( environment_application(), "timriley" ) == 0 )
	{
		timriley_where =
			"transaction.transaction_date_time >= '2019-01-01'";
	}
	else
	{
		timriley_where = "1 = 1";
	}

	sprintf(where,
		"(account in ('%s','%s') and		"
		"%s and %s				",
		feeder_account,
		account_uncleared_checks,
		subquery,
		timriley_where );

	return strdup( where );
}

int feeder_load_file_row_check_number( char *feeder_description )
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

void feeder_load_file_row_list_insert(
			char *feeder_account,
			char *feeder_load_date_time,
			LIST *feeder_load_file_row_list )
{
	FILE *insert_pipe;
	FEEDER_LOAD_FILE_ROW *feeder_load_file_row;
	JOURNAL *journal;

	if ( !list_rewind( feeder_load_file_row_list ) ) return;

	insert_pipe =
		/* Always succeeds */
		/* --------------- */
		feeder_load_file_row_list_insert_pipe(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			feeder_load_file_row_list_insert_system_string(
				FEEDER_LOAD_ROW_INSERT,
				FEEDER_LOAD_ROW_TABLE,
				SQL_DELIMITER ) );

	do {
		feeder_load_file_row =
			list_get(
				feeder_load_file_row_list );

		if ( feeder_load_file_row->
			feeder_load_exist_row_seek )
		{
			continue;
		}

		journal =
			/* Always succeeds */
			/* --------------- */
			feeder_load_file_row_journal(
				feeder_load_file_row->
					feeder_phrase_seek,
				feeder_load_file_row->
					transaction_date_time,
				feeder_load_file_row->
					feeder_not_matched_journal );

		feeder_load_file_row_insert_pipe(
			insert_pipe,
			journal->full_name,
			journal->street_address,
			journal->transaction_date_time,
			feeder_account,
			feeder_load_file_row->international_date,
			feeder_load_file_row->description_embedded,
			feeder_load_file_row->amount,
			feeder_load_file_row->balance,
			feeder_load_file_row->row_number,
			/* --------------- */
			/* Always succeeds */
			/* --------------- */
			feeder_load_file_row_phrase(
				feeder_load_file_row->feeder_phrase ),
			feeder_load_date_time,
			SQL_DELIMITER );

	} while ( list_next( feeder_load_file_row_list ) );

	pclose( insert_pipe );
}

char *feeder_load_file_row_list_insert_system_string(
			char *feeder_load_row_insert,
			char *feeder_load_row_table,
			char sql_delimiter )
{
	char system_string[ 1024 ];

	if ( !feeder_load_row_insert
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
	 	"insert_statement table=%s field=%s del='%c' 		  |"
	 	"sql 2>&1						  |"
	 	"html_paragraph_wrapper.e				   ",
	 	feeder_load_row_table,
	 	feeder_load_row_insert
	 	sql_delimiter );

	return strdup( system_string );
}

FILE *feeder_load_file_row_list_insert_pipe(
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

void feeder_load_file_row_insert_pipe(
			FILE *insert_pipe,
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
			char *feeder_load_date_time,
			char sql_delimiter )
{
	if ( !insert_pipe
	||   !full_name
	||   !street_address
	||   !transaction_date_time
	||   !feeder_account
	||   !international_date
	||   !description_embedded
	||   !amount
	||   !row_number
	||   !phrase
	||   !feeder_load_date_time
	||   !sql_delimiter )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		pclose( insert_pipe );
		exit( 1 );
	}

	fprintf(insert_pipe,
		"%s%c%s%c%s%c%s%c%s%c%s%c%.2lf%c%.2lf%c%d%c%s%c%s\n",
	 	full_name,
		sql_delimiter,
	 	street_address,
		sql_delimiter,
		transation_date_time,
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
		feeder_load_date_time );
}

FEEDER_LOAD_FILE_ROW *
	feeder_load_file_row_check_number_seek(
				int check_number,
				LIST *feeder_load_file_row_list )
{
	FEEDER_LOAD_FILE_ROW *feeder_load_file_row;

	if ( !check_number ) return (FEEDER_LOAD_FILE_ROW *)0;

	if ( !list_rewind( feeder_load_file_row_list ) )
		return (FEEDER_LOAD_FILE_ROW *)0;

	do {
		feeder_load_file_row =
			list_get(
				feeder_load_file_row_list );

		if ( check_number == feeder_load_file_row->check_number )
		{
			return feeder_load_file_row;
		}

	} while ( list_next( feeder_load_file_row_list ) );

	return (FEEDER_LOAD_FILE_ROW *)0;
}

FEEDER_LOAD_FILE_ROW *
	feeder_load_file_row_amount_seek(
				double debit_amount,
				double credit_amount,
				LIST *feeder_load_file_row_list )
{
	FEEDER_LOAD_FILE_ROW *feeder_load_file_row;

	if ( !debit_amount && !credit_amount )
		return (FEEDER_LOAD_FILE_ROW *)0;

	if ( !list_rewind( feeder_load_file_row_list ) )
		return (FEEDER_LOAD_FILE_ROW *)0;

	do {
		feeder_load_file_row =
			list_get(
				feeder_load_file_row_list );

		if ( debit_amount
		&&   debit_amount == feeder_load_file_row->amount )
		{
			return feeder_load_file_row;
		}

		if ( credit_amount
		&&   credit_amount == -feeder_load_file_row->amount )
		{
			return feeder_load_file_row;
		}

	} while ( list_next( feeder_load_file_row_list ) );

	return (FEEDER_LOAD_FILE_ROW *)0;
}

char *feeder_load_file_row_transaction_date_time(
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
		date_html );

	return strdup( transaction_date_time );
}

LIST *feeder_load_exist_row_list(
			char *feeder_account,
			char *feeder_load_file_minimum_date )
{
	LIST *list;
	FILE *pipe;
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

	pipe =
		popen(
			/* Returns heap memory */
			/* ------------------- */
			feeder_load_exist_row_system_string(
				FEEDER_LOAD_EXIST_ROW_SELECT,
				FEEDER_LOAD_ROW_TABLE,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				feeder_load_exist_row_where(
					feeder_account,
					feeder_load_file_minimum_date ) ),
				“r” );

	while ( string_input( input, pipe, 1024 ) )
	{
		list_set(
			list,
			feeder_load_exist_row_parse( input ) );
	}

	pclose( pipe );

	return list;
}

char *feeder_load_exist_row_where(
			char *feeder_account,
			char *feeder_load_file_minimum_date )
{
	static char where[ 128 ];

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

	sprintf(where,
		"feeder_account = '%s' and "
		"feeder_date >= '%s'",
		feeder_account,
		feeder_load_file_minimum_date );

	return where;
}

char *feeder_load_exist_row_system_string(
			char *select,
			char *table,
			char *where )
{
	char system_string[ 1024 ];

	if ( !feeder_load_exist_row_select
	||   !feeder_load_row_table
	||   !feeder_load_exist_row_where )
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

FEEDER_LOAD_EXIST_ROW *feeder_load_exist_row_parse( char *input )
{
	char buffer[ 128 ];
	FEEDER_LOAD_EXIST_ROW *feeder_load_exist_row;

	if ( !input || !*input ) return (FEEDER_LOAD_EXIST_ROW *)0;

	feeder_load_exist_row = feeder_load_exist_row_calloc();

	/* See FEEDER_LOAD_EXIST_ROW_SELECT */
	/* -------------------------------- */
	if ( piece( buffer, SQL_DELIMITER, input, 0 ) )
	{
		feeder_load_exist_row->feeder_date = strdup( buffer );
	}

	if ( piece( buffer, SQL_DELIMITER, input, 1 ) )
	{
		feeder_load_exist_row->feeder_description = strdup( buffer );
	}

	if ( piece( buffer, SQL_DELIMITER, input, 2 ) )
	{
		feeder_load_exist_row->transaction_date_time = strdup( buffer );
	}

	if ( !feeder_load_exist_row->feeder_date )
		return (FEEDER_LOAD_EXIST_ROW *)0;
	else
		return feeder_load_exist_row;
}

FEEDER_LOAD_EXIST_ROW *feeder_load_exist_row_calloc( void )
{
	FEEDER_LOAD_EXIST_ROW *feeder_load_exist_row;

	if ( ! ( feeder_load_exist_row =
			calloc( 1, sizeof( FEEDER_LOAD_EXIST_ROW ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return feeder_load_exist_row;
}

FEEDER_LOAD_EXIST_ROW *feeder_load_exist_row_seek(
			char *international_date,
			char *description_embedded,
			LIST *feeder_load_exist_row_list )
{
	FEEDER_LOAD_EXIST_ROW *feeder_load_exist_row;

	if ( !list_rewind( feeder_load_exist_row_list ) )
		return (FEEDER_LOAD_EXIST_ROW *)0;

	do {
		feeder_load_exist_row =
			list_get(
				feeder_load_exist_row_list );

		if ( strcmp(	feeder_load_exist_row->feeder_date,
				international_date ) == 0
		&&   strcmp(	feeder_load_exist_row->feeder_description,
				description_embedded ) == 0 )
		{
			return feeder_load_exist_row;
		}

	} while ( list_next( feeder_load_exist_row_list ) );

	return (FEEDER_LOAD_EXIST_ROW *)0;
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
				feeder_load_file_row_international_date(
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

JOURNAL *feeder_load_file_row_journal(
			FEEDER_PHRASE *feeder_phrase_seek,
			char *transaction_date_time,
			FEEDER_NOT_MATCHED_JOURNAL *
				feeder_not_matched_journal )
{
	if ( !feeder_phrase_seek
	&&   !feeder_not_matched_journal )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: both parameters are empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( feeder_phrase_seek )
	{
		if ( !transaction_date_time )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: transaction_date_time is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}


		return
		journal_new(
			feeder_phrase_seek->full_name,
			feeder_phrase_seek->street_address,
			transaction_date_time,
			feeder_phrase_seek->nominal_account );
	}
	else
	{
		return
		journal_new(
			not_matched_journal->full_name,
			not_matched_journal->street_address,
			not_matched_journal->transaction_date_time,
			not_matched_journal->account_name );
	}
}

char *feeder_load_file_row_phrase(
			FEEDER_PHRASE *feeder_phrase )
{
	if ( !feeder_phrase )
		return "";
	else
		return feeder_phrase->phrase;
}

