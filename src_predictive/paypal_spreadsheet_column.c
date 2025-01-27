/* -------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/paypal_spreadsheet_column.c		*/
/* -------------------------------------------------------------	*/
/* No warranty and freely available software. See Appaserver.org	*/
/* -------------------------------------------------------------	*/

#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "piece.h"
#include "timlib.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "paypal_spreadsheet_column.h"

int paypal_spreadsheet_column_piece(
		char *heading_line,
		char *heading_string )
{
	int p;
	char piece_buffer[ 128 ];

	for (	p = 0;
		piece_quote_comma(
			piece_buffer /* destination */,
			heading_line /* source */,
			p /* offset */ );
		p++ )
	{
		if ( string_strcmp(
			piece_buffer,
			heading_string ) == 0 )
		{
			return p;
		}
	}

	return -1;
}

PAYPAL_SPREADSHEET_COLUMN *paypal_spreadsheet_column_calloc( void )
{
	PAYPAL_SPREADSHEET_COLUMN *paypal_spreadsheet_column;

	if ( ! ( paypal_spreadsheet_column =
			calloc( 1, sizeof ( PAYPAL_SPREADSHEET_COLUMN ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return paypal_spreadsheet_column;
}

PAYPAL_SPREADSHEET_COLUMN *paypal_spreadsheet_column_new(
		char *heading_string,
		int paypal_spreadsheet_column_piece )
{
	PAYPAL_SPREADSHEET_COLUMN *paypal_spreadsheet_column;

	if ( !heading_string
	||   ( paypal_spreadsheet_column_piece == -1 ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	paypal_spreadsheet_column = paypal_spreadsheet_column_calloc();

	paypal_spreadsheet_column->heading_string = heading_string;

	paypal_spreadsheet_column->
		paypal_spreadsheet_column_piece =
			paypal_spreadsheet_column_piece;

	return paypal_spreadsheet_column;
}

PAYPAL_SPREADSHEET_COLUMN *paypal_spreadsheet_column_heading_seek(
		LIST *paypal_spreadsheet_column_list,
		char *heading_string )
{
	PAYPAL_SPREADSHEET_COLUMN *paypal_spreadsheet_column;

	if ( !heading_string )
	{
		char message[ 128 ];

		sprintf(message, "heading_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( paypal_spreadsheet_column_list ) )
	do {
		paypal_spreadsheet_column =
			list_get(
				paypal_spreadsheet_column_list );

		if ( string_strcmp(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			string_remove_control(
				paypal_spreadsheet_column->
					heading_string ),
			heading_string ) == 0 )
		{
			return paypal_spreadsheet_column;
		}

	} while( list_next( paypal_spreadsheet_column_list ) );

	return NULL;
}

LIST *paypal_spreadsheet_column_list(
		char *spreadsheet_filename,
		char *date_label )
{
	char *header_row_string;
	int p;
	char heading_string[ 1024 ];
	LIST *column_list;

	if ( ! ( header_row_string =
			paypal_spreadsheet_column_header_row_string(
				spreadsheet_filename,
				date_label ) ) )
	{
		return NULL;
	}

	column_list = list_new();

	for (	p = 0;
		piece_quote_comma(
			heading_string,
			header_row_string,
			p );
		p++ )
	{
		list_set(
			column_list,
			paypal_spreadsheet_column_new(
				strdup( heading_string ),
				p
				/* paypal_spreadsheet_column_piece */ ) );
	}
	return column_list;
}

char *paypal_spreadsheet_column_heading_data(
		LIST *paypal_spreadsheet_column_list,
		char *input_row,
		char *heading_string )
{
	char data[ 65536 ];
	PAYPAL_SPREADSHEET_COLUMN *paypal_spreadsheet_column;

	if ( ! ( paypal_spreadsheet_column =
			paypal_spreadsheet_column_heading_seek(
				paypal_spreadsheet_column_list,
				heading_string ) ) )
	{
		return NULL;
	}

	if ( !piece_quote(
			data /* destination */,
			input_row,
			paypal_spreadsheet_column->
				paypal_spreadsheet_column_piece ) )
	{
		return NULL;
	}
	return strdup( data );
}

char *paypal_spreadsheet_column_header_row_string(
		char *spreadsheet_filename,
		char *date_label )
{
	FILE *input_file;
	char header_buffer[ 65536 ];

	/* Safely returns */
	/* -------------- */
	input_file = appaserver_input_file( spreadsheet_filename );

	while( string_input( header_buffer, input_file, 65536 ) )
	{
		if ( paypal_spreadsheet_column_header_boolean(
			date_label,
			header_buffer ) )
		{
			fclose( input_file );
			return strdup( header_buffer );
		}
	}
	fclose( input_file );
	return NULL;
}

boolean paypal_spreadsheet_column_header_boolean(
		char *date_label,
		char *header_buffer )
{
	return ( instr( date_label, header_buffer, 1 ) > -1 );
}

char *paypal_spreadsheet_column_cell_extract(
		char *input_row,
		int paypal_spreadsheet_column_piece )
{
	char cell_extract[ 1024 ];

	if ( piece_quote_comma(
		cell_extract /* destination */,
		input_row /* source */,
		paypal_spreadsheet_column_piece /* offset */ ) )
	{
		return strdup( cell_extract );
	}
	else
	{
		return (char *)0;
	}
}

int paypal_spreadsheet_column_date_piece(
		char *spreadsheet_filename,
		char *date_label )
{
	char *header_row_string =
		paypal_spreadsheet_column_header_row_string(
			spreadsheet_filename,
			date_label );

	if ( !header_row_string ) return -1;

	return piece_quote_comma_seek( header_row_string, date_label );
}
