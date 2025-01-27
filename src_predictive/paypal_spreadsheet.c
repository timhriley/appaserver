/* --------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/paypal_spreadsheet.c			*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit Appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "piece.h"
#include "date_convert.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "paypal_spreadsheet_column.h"
#include "paypal_spreadsheet.h"

PAYPAL_SPREADSHEET *paypal_spreadsheet_calloc( void )
{
	PAYPAL_SPREADSHEET *paypal_spreadsheet;

	if ( ! ( paypal_spreadsheet =
			calloc( 1, sizeof ( PAYPAL_SPREADSHEET ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return paypal_spreadsheet;
}

PAYPAL_SPREADSHEET *paypal_spreadsheet_fetch(
		char *spreadsheet_filename,
		char *date_label )
{
	PAYPAL_SPREADSHEET *paypal_spreadsheet;

	if ( !spreadsheet_filename
	||   !date_label )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	paypal_spreadsheet = paypal_spreadsheet_calloc();

	paypal_spreadsheet->spreadsheet_filename = spreadsheet_filename;
	paypal_spreadsheet->date_label = date_label;

	paypal_spreadsheet->
		paypal_spreadsheet_summary =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			paypal_spreadsheet_summary_new(
				spreadsheet_filename,
				date_label );

	if ( paypal_spreadsheet->
		paypal_spreadsheet_summary->
		row_count == 0 )
	{
		return paypal_spreadsheet;
	}

	paypal_spreadsheet->
		paypal_spreadsheet_column_list =
			paypal_spreadsheet_column_list(
				spreadsheet_filename,
				date_label );

	if ( !list_length( 
		paypal_spreadsheet->
			paypal_spreadsheet_column_list ) )
	{
		return paypal_spreadsheet;
	}

	return paypal_spreadsheet;
}


PAYPAL_SPREADSHEET_SUMMARY *paypal_spreadsheet_summary_calloc( void )
{
	PAYPAL_SPREADSHEET_SUMMARY *paypal_spreadsheet_summary;

	if ( ! ( paypal_spreadsheet_summary =
			calloc( 1,
				sizeof ( PAYPAL_SPREADSHEET_SUMMARY ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return paypal_spreadsheet_summary;
}

PAYPAL_SPREADSHEET_SUMMARY *paypal_spreadsheet_summary_new(
		char *spreadsheet_filename,
		char *date_label )
{
	FILE *input_file;
	char input[ 65536 ];
	char date_american_string[ 128 ];
	char *source_american;
	PAYPAL_SPREADSHEET_SUMMARY *paypal_spreadsheet_summary;

	if ( !spreadsheet_filename
	||   !date_label )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	paypal_spreadsheet_summary = paypal_spreadsheet_summary_calloc();

	paypal_spreadsheet_summary->
		paypal_spreadsheet_column_date_piece =
			paypal_spreadsheet_column_date_piece(
				spreadsheet_filename,
				date_label );

	if ( paypal_spreadsheet_summary->
		paypal_spreadsheet_column_date_piece == -1 )
	{
		return paypal_spreadsheet_summary;
	}

	/* Safely returns */
	/* -------------- */
	input_file = appaserver_input_file( spreadsheet_filename );

	while ( string_input( input, input_file, 65536 ) )
	{
		*date_american_string = '\0';

		piece_quote(
			date_american_string,
			input,
			paypal_spreadsheet_summary->
				paypal_spreadsheet_column_date_piece );

		if ( ! ( source_american =
			 /* ----------------------------------------------- */
			 /* Returns heap memory, source_date_string or null */
			 /* ----------------------------------------------- */
				date_convert_source_american(
					date_convert_international
						/* destination_enum */,
					date_american_string
						/* source_date_string */ ) ) )
		{
			continue;
		}

		paypal_spreadsheet_summary->row_count++;

		if ( !*paypal_spreadsheet_summary->minimum_date_international )
		{
			strcpy(
				paypal_spreadsheet_summary->
					minimum_date_international,
				source_american );

			strcpy(
				paypal_spreadsheet_summary->
					maximum_date_international,
				source_american );
		}
		else
		{
			if ( strcmp(
				source_american,
				paypal_spreadsheet_summary->
					minimum_date_international ) < 0 )
			{
				strcpy(	paypal_spreadsheet_summary->
						minimum_date_international,
					source_american );
			}
			else
			if ( strcmp(
				source_american,
				paypal_spreadsheet_summary->
					maximum_date_international ) > 0 )
			{
				strcpy(	paypal_spreadsheet_summary->
						maximum_date_international,
					source_american );
			}
		}

		if ( source_american != date_american_string )
		{
			free( source_american );
		}
	}

	fclose( input_file );
	return paypal_spreadsheet_summary;
}

