/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/statement.c				*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include "String.h"
#include "timlib.h"
#include "date_convert.h"
#include "application.h"
#include "application_constant.h"
#include "appaserver_error.h"
#include "element.h"
#include "column.h"
#include "account.h"
#include "latex.h"
#include "statement.h"

enum statement_subclassification_option
	statement_resolve_subclassification_option(
		char *subclassification_option_string )
{
	if ( strcmp(
		subclassification_option_string,
		"aggregate" ) == 0 )
	{
		return statement_subclassification_aggregate;
	}
	else
	if ( strcmp(
		subclassification_option_string,
		"display" ) == 0 )
	{
		return statement_subclassification_display;
	}
	else
	if ( strcmp(
		subclassification_option_string,
		"omit" ) == 0 )
	{
		return statement_subclassification_omit;
	}
	else
	{
		return statement_subclassification_display;
	}
}

enum statement_output_medium
	statement_resolve_output_medium(
		char *output_medium_string )
{
	if ( string_strcmp(
		output_medium_string,
		"table" ) == 0 )
	{
		return statement_output_table;
	}
	else
	if ( string_strcmp(
		output_medium_string,
		"PDF" ) == 0 )
	{
		return statement_output_PDF;
	}
	else
	if ( string_strcmp(
		output_medium_string,
		"spreadsheet" ) == 0 )
	{
		return statement_output_spreadsheet;
	}
	else
	if ( string_strcmp(
		output_medium_string,
		"stdout" ) == 0 )
	{
		return statement_output_stdout;
	}
	else
	{
		return statement_output_table;
	}
}

char *statement_caption_logo_filename(
		const char *statement_logo_filename_key )
{
	char *logo_filename =
	     /* ----------------------------------------------------------- */
	     /* Returns component of application_constant->dictionary or "" */
	     /* ----------------------------------------------------------- */
	     application_constant_fetch(
			(char *)statement_logo_filename_key );

	if ( *logo_filename )
		return logo_filename;
	else
		return (char *)0;
}

char *statement_caption_sub_title(
		char *begin_date_string,
		char *end_date_time )
{
	static char sub_title[ 128 ];
	char *begin_date_american = {0};
	char *end_date_american = {0};

	if ( begin_date_string )
	{
		begin_date_american =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			statement_date_american(
				begin_date_string );

		if ( !begin_date_american ) return NULL;
	}

	if ( end_date_time )
	{
		end_date_american =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			statement_date_american(
				end_date_time );

		if ( !end_date_american ) return NULL;
	}

	if ( !begin_date_american && !end_date_american )
	{
		*sub_title = '\0';
	}
	else
	if ( !end_date_american )
	{
		sprintf(sub_title,
			"Begin: %s",
			begin_date_american );
	}
	else
	if ( !begin_date_american )
	{
		sprintf(sub_title,
			"End: %s",
			end_date_american );
	}
	else
	{
		sprintf(sub_title,
			"Begin: %s, End: %s",
			begin_date_american,
			end_date_american );
	}

	return sub_title;
}

STATEMENT *statement_fetch(
		char *application_name,
		char *process_name,
		int prior_year_count,
		LIST *element_name_list,
		char *transaction_date_begin_date_string,
		char *end_date_time,
		boolean fetch_transaction,
		boolean latest_zero_balance_boolean )
{
	STATEMENT *statement;

	if ( !list_length( element_name_list )
	||   !end_date_time )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	statement = statement_calloc();

	statement->process_name = process_name;

	statement->transaction_date_begin_date_string =
		transaction_date_begin_date_string;

	statement->end_date_time =
		end_date_time;

	statement->element_statement_list =
		element_statement_list(
			element_name_list,
			end_date_time,
			1 /* fetch_subclassification */,
			1 /* fetch_account_list */,
			1 /* fetch_journal_latest */,
			fetch_transaction,
			latest_zero_balance_boolean );

	if ( !list_length( statement->element_statement_list ) )
	{
		free( statement );
		return (STATEMENT *)0;
	}

	element_list_sum_set(
		statement->element_statement_list );

	element_percent_of_asset_set(
		statement->element_statement_list );

	element_percent_of_income_set(
		statement->element_statement_list );

	statement->pdf_landscape_boolean =
		statement_pdf_landscape_boolean(
			prior_year_count );

	if ( application_name )
	{
		statement->statement_caption =
			statement_caption_new(
				application_name,
				process_name,
				transaction_date_begin_date_string,
				end_date_time );
	}

	statement->greater_year_message =
		/* ------------------------------ */
		/* Returns program memory or null */
		/* ------------------------------ */
		statement_greater_year_message(
			transaction_date_begin_date_string,
			end_date_time );

	return statement;
}

STATEMENT *statement_calloc( void )
{
	STATEMENT *statement;

	if ( ! ( statement = calloc( 1, sizeof ( STATEMENT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return statement;
}

LIST *statement_prior_year_account_data_list(
		char *account_name,
		LIST *statement_prior_year_list )
{
	LIST *data_list;
	STATEMENT_PRIOR_YEAR *statement_prior_year;
	ACCOUNT *prior_account;

	if ( !list_rewind( statement_prior_year_list ) ) return (LIST *)0;

	data_list = list_new();

	do {
		statement_prior_year = list_get( statement_prior_year_list );

		prior_account =
			account_element_list_seek(
				account_name,
				statement_prior_year->
					element_statement_list );

		list_set(
			data_list,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			statement_prior_year_account_data(
				prior_account ) );

	} while ( list_next( statement_prior_year_list ) );

	return data_list;
}

char *statement_prior_year_account_data( ACCOUNT *prior_account )
{
	char account_data[ 32 ];

	if ( !prior_account
	||   !prior_account->account_journal_latest
	||   !prior_account->account_journal_latest->balance )
	{
		*account_data = '\0';
	}
	else
	{
		sprintf(account_data,
			"%d%c %s",
			prior_account->delta_prior_percent,
			'%',
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			timlib_place_commas_in_dollars(
				prior_account->
					account_journal_latest->
					balance ) );
	}

	return strdup( account_data );
}

LIST *statement_prior_year_subclassification_data_list(
		char *subclassification_name,
		LIST *statement_prior_year_list )
{
	LIST *data_list;
	STATEMENT_PRIOR_YEAR *statement_prior_year;
	SUBCLASSIFICATION *prior_subclassification;

	if ( !list_rewind( statement_prior_year_list ) ) return (LIST *)0;

	data_list = list_new();

	do {
		statement_prior_year = list_get( statement_prior_year_list );

		prior_subclassification =
			subclassification_element_list_seek(
				subclassification_name,
				statement_prior_year->
					element_statement_list );

		list_set(
			data_list,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			statement_prior_year_subclassification_data(
				prior_subclassification ) );

	} while ( list_next( statement_prior_year_list ) );

	return data_list;
}

char *statement_prior_year_subclassification_data(
		SUBCLASSIFICATION *prior_subclassification )
{
	boolean cell_empty = {0};
	int delta_prior_percent = {0};
	double prior_year_amount = {0};

	if ( !prior_subclassification || !prior_subclassification->sum )
	{
		cell_empty = 1;
	}
	else
	{
		delta_prior_percent =
			prior_subclassification->delta_prior_percent;

		prior_year_amount =
			prior_subclassification->sum;
	}

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	statement_prior_year_cell_display(
		cell_empty,
		delta_prior_percent,
		prior_year_amount );
}

LIST *statement_prior_year_element_data_list(
			char *element_name,
			LIST *statement_prior_year_list )
{
	LIST *data_list;
	STATEMENT_PRIOR_YEAR *statement_prior_year;
	ELEMENT *prior_element;

	if ( !list_rewind( statement_prior_year_list ) ) return (LIST *)0;

	data_list = list_new();

	do {
		statement_prior_year = list_get( statement_prior_year_list );

		prior_element =
			element_seek(
				element_name,
				statement_prior_year->
					element_statement_list );

		list_set(
			data_list,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			statement_prior_year_element_data(
				prior_element ) );

	} while ( list_next( statement_prior_year_list ) );

	return data_list;
}

char *statement_prior_year_element_data(
			ELEMENT *prior_element )
{
	boolean cell_empty = {0};
	int delta_prior_percent = {0};
	double prior_year_amount = {0};

	if ( !prior_element || !prior_element->sum )
	{
		cell_empty = 1;
	}
	else
	{
		delta_prior_percent = prior_element->delta_prior_percent;
		prior_year_amount = prior_element->sum;
	}

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	statement_prior_year_cell_display(
		cell_empty,
		delta_prior_percent,
		prior_year_amount );
}

STATEMENT_LINK *statement_link_new(
		char *application_name,
		char *process_name,
		char *data_directory,
		char *transaction_date_begin_date_string,
		char *end_date_time,
		pid_t process_id )
{
	STATEMENT_LINK *statement_link;

	if ( !application_name
	||   !process_name
	||   !data_directory )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	statement_link = statement_link_calloc();

	statement_link->appaserver_link =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_link_new(
			application_http_prefix(
				application_ssl_support_boolean(
					application_name ) ),
			application_server_address(),
			data_directory,
			process_name /* filename_stem */,
			application_name,
			process_id,
			(char *)0 /* session_key */,
			transaction_date_begin_date_string,
			end_date_time,
			"tex" /* extension */ );

	statement_link->tex_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_filename(
			statement_link->appaserver_link->filename_stem,
			statement_link->appaserver_link->begin_date_string,
			statement_link->appaserver_link->end_date_string,
			statement_link->appaserver_link->process_id,
			statement_link->appaserver_link->session_key,
			statement_link->appaserver_link->extension );

	statement_link->tex_anchor_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_anchor_html(
			statement_link->
				appaserver_link->
				appaserver_link_prompt->
				filename /* prompt_filename */,
			process_name /* target_window */,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			statement_link_tex_prompt(
				process_name ) );

	statement_link->appaserver_link->extension = "pdf";

	statement_link->appaserver_link->appaserver_link_prompt =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_link_prompt_new(
			APPASERVER_LINK_PROMPT_DIRECTORY
				/* probably appaserver_data */,
			statement_link->appaserver_link->http_prefix,
			statement_link->appaserver_link->server_address,
			application_name,
			statement_link->appaserver_link->filename_stem,
			statement_link->appaserver_link->begin_date_string,
			statement_link->appaserver_link->end_date_string,
			statement_link->appaserver_link->process_id,
			statement_link->appaserver_link->session_key,
			statement_link->appaserver_link->extension );

	statement_link->pdf_anchor_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_anchor_html(
			statement_link->
				appaserver_link->
				appaserver_link_prompt->
				filename /* prompt_filename */,
			process_name /* target_window */,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			statement_link_pdf_prompt(
				process_name ) );

	statement_link->appaserver_link_working_directory =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_working_directory(
			data_directory,
			application_name );

	return statement_link;
}

STATEMENT_LINK *statement_link_calloc( void )
{
	STATEMENT_LINK *statement_link;

	if ( ! ( statement_link =
			calloc( 1, sizeof ( STATEMENT_LINK ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return statement_link;
}

char *statement_date_american( char *date_time )
{
	char date_string[ 16 ];
	char *date_american;

	date_american =
		/* ----------------------------------------------- */
		/* Returns heap memory, source_date_string or null */
		/* ----------------------------------------------- */
		date_convert_source_international(
			date_convert_american /* destination_enum */,
			column(
				date_string,
				0,
				date_time )
				/* source_date_string */ );

	if ( !date_american ) return (char *)0;

	if ( date_american == date_string )
		return strdup( date_american );
	else
		return date_american;
}

char *statement_date_convert(
		char *application_name,
		char *login_name,
		char *date_time )
{
	char date_string[ 16 ];
	enum date_convert_format_enum format_enum;
	char *date_convert;

	format_enum =
		/* -------------------------------------------- */
		/* Returns static enum date_convert_format_enum */
		/* -------------------------------------------- */
		date_convert_login_name_enum(
			application_name,
			login_name );

	date_convert =
		/* ----------------------------------------------- */
		/* Returns heap memory, source_date_string or null */
		/* ----------------------------------------------- */
		date_convert_source_international(
			format_enum /* destination_enum */,
			column(
				date_string,
				0,
				date_time )
				/* source_date_string */ );

	if ( !date_convert ) return (char *)0;

	if ( date_convert == date_string )
		return strdup( date_convert );
	else
		return date_convert;
}

char *statement_caption_combined(
		char *title,
		char *subtitle )
{
	char combined[ 256 ];

	if ( !title )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: title is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !subtitle )
	{
		strcpy( combined, title );
	}
	else
	{
		sprintf(combined,
			"%s %s",
			title,
			subtitle );
	}

	return strdup( combined );
}

char *statement_caption_title(
		char *application_name,
		boolean exists_logo_filename,
		char *process_name )
{
	static char title[ 256 ];

	if ( !application_name
	||   !process_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !exists_logo_filename )
	{
		char buffer[ 128 ];

		sprintf(title,
			"%s %s",
			application_title_string(
				application_name ),
			string_initial_capital(
				buffer,
				process_name ) );
	}
	else
	{
		string_initial_capital(
			title,
			process_name );
	}

	return title;
}

LIST *statement_prior_year_heading_list(
		LIST *statement_prior_year_list )
{
	LIST *heading_list;
	STATEMENT_PRIOR_YEAR *statement_prior_year;

	if ( !list_rewind( statement_prior_year_list ) ) return (LIST *)0;

	heading_list = list_new();

	do {
		statement_prior_year =
			list_get(
				statement_prior_year_list );

		list_set(
			heading_list,
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			statement_date_american(
				statement_prior_year->date_time ) );

	} while ( list_next( statement_prior_year_list ) );

	return heading_list;
}

void statement_html_output(
		HTML_TABLE *html_table,
		char *secondary_title )
{
	if ( !html_table )
	{
		char message[ 128 ];

		sprintf(message, "html_table is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( secondary_title )
	{
		printf( "<h1>%s</h1>\n", secondary_title );
	}

	html_table_output(
		html_table,
		HTML_TABLE_ROWS_BETWEEN_HEADING );
}

LIST *statement_prior_year_list(
		LIST *element_name_list,
		char *end_date_time,
		int prior_year_count,
		STATEMENT *statement )
{
	LIST *prior_year_list;
	int years_ago;

	if ( !list_length( element_name_list )
	||   !end_date_time
	||   !prior_year_count
	||   !statement
	||   !list_length( statement->element_statement_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	prior_year_list = list_new();

	for(	years_ago = 1;
		years_ago <= prior_year_count;
		years_ago++ )
	{
		list_set(
			prior_year_list,
			statement_prior_year_fetch(
				element_name_list,
				end_date_time,
				years_ago,
				statement ) );
	}

	return prior_year_list;
}

STATEMENT_PRIOR_YEAR *statement_prior_year_fetch(
		LIST *element_name_list,
		char *end_date_time,
		int years_ago,
		STATEMENT *statement )
{
	STATEMENT_PRIOR_YEAR *statement_prior_year;
	ELEMENT *current_element;
	ELEMENT *prior_element;

	if ( !list_length( element_name_list )
	||   !end_date_time
	||   !years_ago
	||   !statement )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_length( statement->element_statement_list ) ) return NULL;

	statement_prior_year = statement_prior_year_calloc();

	statement_prior_year->date_time =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_prior_year_date_time(
			end_date_time,
			years_ago );

	statement_prior_year->element_statement_list =
		element_statement_list(
			element_name_list,
			statement_prior_year->date_time,
			1 /* fetch_subclassification_list */,
			1 /* fetch_account_list */,
			1 /* fetch_journal_latest */,
			0 /* not fetch_transaction */,
			0 /* not latest_zero_balance_boolean */ );

	if ( !list_length( statement_prior_year->element_statement_list ) )
	{
		return NULL;
	}

	element_list_sum_set(
		statement_prior_year->
			element_statement_list );

	list_rewind( statement->element_statement_list );

	do {
		current_element =
			list_get(
				statement->
					element_statement_list );

		prior_element =
			element_seek(
				current_element->element_name,
				statement_prior_year->element_statement_list );

		if ( prior_element )
		{
			element_delta_prior_percent_set(
				prior_element /* in/out */,
				current_element );
		}

	} while ( list_next( statement->element_statement_list ) );

	return statement_prior_year;
}

STATEMENT_PRIOR_YEAR *statement_prior_year_calloc( void )
{
	STATEMENT_PRIOR_YEAR *statement_prior_year;

	if ( ! ( statement_prior_year =
			calloc( 1, sizeof ( STATEMENT_PRIOR_YEAR ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return statement_prior_year;
}

char *statement_prior_year_date_time(
		char *end_date_time,
		int years_ago )
{
	DATE *prior_date;

	if ( !end_date_time
	||   !years_ago )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	prior_date = date_19new( end_date_time );

	date_subtract_year( prior_date, years_ago );

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	date_display19( prior_date );
}

char *statement_caption_frame_title(
		char *process_name,
		char *date_now16 )
{
	static char title[ 128 ];
	char buffer[ 64 ];

	if ( !process_name
	||   !date_now16 )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(title,
		"<h1>%s</h1>\n<h2>%s</h2>\n",
		string_initial_capital(
			buffer,
			process_name ),
		date_now16 );

	return title;
}

boolean statement_pdf_landscape_boolean( int prior_year_count )
{
	return (boolean)( prior_year_count > 0 );
}

LATEX_ROW *statement_subclass_display_latex_element_label_row(
		char *element_name,
		int statement_prior_year_list_length,
		LIST *latex_column_list )
{
	LIST *cell_list;
	LATEX_COLUMN *latex_column;
	int i;

	if ( !element_name
	||   !list_rewind( latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_new(
			latex_column,
			0 /* not first_row_boolean */,
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum( element_name ),
			1 /* large_boolean */,
			1 /* bold_boolean */ ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	for (	i = 0;
		i < statement_prior_year_list_length + 4;
		i++ )
	{
		list_set(
			cell_list,
			latex_cell_small_new(
				latex_column,
				0 /* not first_row_boolean */,
				(char *)0 /* datum */ ) );

		latex_column = list_get( latex_column_list );
		list_next( latex_column_list );
	}

	return latex_row_new( cell_list );
}

LATEX_ROW *statement_subclass_display_latex_subclassification_label_row(
		char *subclassification_name,
		int statement_prior_year_list_length,
		LIST *latex_column_list )
{
	LIST *cell_list;
	LATEX_COLUMN *latex_column;
	int i;

	if ( !subclassification_name
	||   !list_rewind( latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_new(
			latex_column,
			0 /* not first_row_boolean */,
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum(
				subclassification_name ),
			0 /* not large_boolean */,
			1 /* bold_boolean */ ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	for (	i = 0;
		i < statement_prior_year_list_length + 4;
		i++ )
	{
		list_set(
			cell_list,
			latex_cell_small_new(
				latex_column,
				0 /* not first_row_boolean */,
				(char *)0 /* datum */ ) );

		latex_column = list_get( latex_column_list );
		list_next( latex_column_list );

	}

	return
	latex_row_new( cell_list );
}

LATEX_ROW *statement_subclass_display_latex_subclassification_sum_row(
		char *subclassification_name,
		double sum,
		int percent_of_asset,
		int percent_of_income,
		LIST *statement_prior_year_list,
		LIST *latex_column_list )
{
	LIST *cell_list;
	LATEX_COLUMN *latex_column;
	char *cell_label_datum;
	char *account_balance_string;
	char *account_percent_string;

	if ( !subclassification_name
	||   !list_rewind( latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	cell_label_datum =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_label_datum(
			subclassification_name );

	list_set(
		cell_list,
		latex_cell_new(
			latex_column,
			0 /* not first_row_boolean */,
			cell_label_datum,
			0 /* not large_boolean */,
			1 /* bold_boolean */ ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			(char *)0 /* datum */ ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	account_balance_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_account_balance_string(
			sum /* balance */,
			(char *)0 /* account_action_string */,
			0 /* not round_dollar_boolean */ );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			account_balance_string ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			(char *)0 /* datum */ ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	account_percent_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_account_percent_string(
			percent_of_asset,
			percent_of_income );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			account_percent_string ) );

	if ( list_length( statement_prior_year_list ) )
	{
		LIST *prior_year_subclassification_data_list =
			statement_prior_year_subclassification_data_list(
				subclassification_name,
				statement_prior_year_list );

		list_set_list(
			cell_list,
			latex_cell_list(
				__FUNCTION__,
				latex_column_list,
				prior_year_subclassification_data_list ) );
	}

	return
	latex_row_new( cell_list );
}

LATEX_ROW *statement_subclass_display_latex_element_sum_row(
		char *element_name,
		double sum,
		int percent_of_asset,
		int percent_of_income,
		LIST *statement_prior_year_list,
		LIST *latex_column_list )
{
	LIST *cell_list;
	LATEX_COLUMN *latex_column;
	char *cell_label_datum;
	char *account_balance_string;
	char *account_percent_string;

	if ( !element_name
	||   !list_rewind( latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	cell_label_datum =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_label_datum(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			statement_element_sum_name( element_name ) );

	list_set(
		cell_list,
		latex_cell_new(
			latex_column,
			0 /* not first_row_boolean */,
			cell_label_datum,
			1 /* large_boolean */,
			1 /* bold_boolean */ ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			(char *)0 /* datum */ ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			(char *)0 /* datum */ ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	account_balance_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_account_balance_string(
			sum /* balance */,
			(char *)0 /* account_action_string */,
			0 /* not round_dollar_boolean */ );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			account_balance_string ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	account_percent_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_account_percent_string(
			percent_of_asset,
			percent_of_income );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			account_percent_string ) );

	if ( list_length( statement_prior_year_list ) )
	{
		LIST *prior_year_element_data_list =
			statement_prior_year_element_data_list(
				element_name,
				statement_prior_year_list );

		list_set_list(
			cell_list,
			latex_cell_list(
				__FUNCTION__,
				latex_column_list,
				prior_year_element_data_list ) );
	}

	return
	latex_row_new( cell_list );
}

LIST *statement_subclass_display_latex_account_row_list(
		LIST *account_statement_list,
		LIST *statement_prior_year_list,
		LIST *latex_column_list )
{
	LIST *row_list;
	STATEMENT_ACCOUNT *statement_account;
	ACCOUNT *account;

	if ( !list_rewind( account_statement_list ) ) return (LIST *)0;

	row_list = list_new();

	do {
		account =
			list_get(
				account_statement_list );

		if ( !account->account_journal_latest )
		{
			char message[ 128 ];

			sprintf(message,
			"for account = [%s], account_journal_latest is empty.",
				account->account_name );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		statement_account =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			statement_account_new(
				(char *)0 /* end_date_time */,
				0 /* element_accumulate_debit */,
				account->account_journal_latest,
				(char *)0 /* account_action_string */,
				0 /* not round_dollar_boolean */,
				account );

		list_set(
			row_list,
			statement_subclass_display_latex_account_row(
				statement_account,
				statement_prior_year_list,
				latex_column_list ) );

	} while ( list_next( account_statement_list ) );

	return row_list;
}

LATEX_ROW *statement_subclass_omit_latex_element_label_row(
		char *element_name,
		int statement_prior_year_list_length,
		LIST *latex_column_list )
{
	LIST *cell_list;
	LATEX_COLUMN *latex_column;

	if ( !element_name
	||   !list_rewind( latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_new(
			latex_column,
			0 /* not first_row_boolean */,
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum( element_name ),
			1 /* large_boolean */,
			1 /* bold_boolean */ ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			(char *)0 /* datum */ ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			(char *)0 /* datum */ ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			(char *)0 /* datum */ ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	while ( statement_prior_year_list_length-- )
	{
		list_set(
			cell_list,
			latex_cell_small_new(
				latex_column,
				0 /* not first_row_boolean */,
				(char *)0 /* datum */ ) );

		latex_column = list_get( latex_column_list );
		list_next( latex_column_list );
	}

	return
	latex_row_new( cell_list );
}

LATEX_ROW *statement_subclass_display_latex_account_row(
		STATEMENT_ACCOUNT *statement_account,
		LIST *statement_prior_year_list,
		LIST *latex_column_list )
{
	LIST *cell_list;
	char *cell_label_datum;
	LATEX_COLUMN *latex_column;

	if ( !statement_account
	||   !statement_account->account
	||   !list_rewind( latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	cell_label_datum =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_label_datum(
			statement_account->
				account->
				account_name );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			cell_label_datum ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			statement_account->balance_string ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			(char *)0 /* datum */ ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );


	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			(char *)0 /* datum */ ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
			latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			statement_account->percent_string ) );

	if ( list_length( statement_prior_year_list ) )
	{
		LIST *prior_year_account_data_list =
			statement_prior_year_account_data_list(
				statement_account->
					account->
					account_name,
				statement_prior_year_list );

		list_set_list(
			cell_list,
			latex_cell_list(
				__FUNCTION__,
				latex_column_list,
				prior_year_account_data_list ) );
	}

	return
	latex_row_new( cell_list );
}

STATEMENT_ACCOUNT *statement_account_new(
		char *end_date_time,
		boolean element_accumulate_debit,
		ACCOUNT_JOURNAL *account_journal_latest,
		char *account_action_string,
		boolean round_dollar_boolean,
		ACCOUNT *account )
{
	STATEMENT_ACCOUNT *statement_account;

	if ( !account_journal_latest
	||   !account )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	statement_account = statement_account_calloc();

	statement_account->balance = account_journal_latest->balance;

	/* For trial_balance report */
	/* ------------------------ */
/*
	if ( statement_account->balance < 0.0 )
	{
		statement_account->balance =
			0.0 - statement_account->balance;

		element_accumulate_debit = 1 - element_accumulate_debit;
	}
*/

	statement_account->balance_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_account_balance_string(
			statement_account->balance,
			account_action_string,
			round_dollar_boolean );

	if ( element_accumulate_debit )
	{
		statement_account->debit_string =
			statement_account->balance_string;
	}
	else
	{
		statement_account->credit_string =
			statement_account->balance_string;
	}

	statement_account->asset_percent_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_account_asset_percent_string(
			account->percent_of_asset );

	statement_account->revenue_percent_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_account_revenue_percent_string(
			account->percent_of_income );

	statement_account->percent_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_account_percent_string(
			account->percent_of_asset,
			account->percent_of_income );

	if ( end_date_time )
	{
		statement_account->date_days_between =
			/* ---------------------- */
			/* Ignores trailing time. */
			/* ---------------------- */
			date_days_between(
				account_journal_latest->transaction_date_time,
				end_date_time );

		statement_account->within_days_between_boolean =
			( statement_account->date_days_between <=
		  	STATEMENT_DAYS_FOR_EMPHASIS );
	}

	statement_account->account = account;

	return statement_account;
}

STATEMENT_ACCOUNT *statement_account_calloc( void )
{
	STATEMENT_ACCOUNT *statement_account;

	if ( ! ( statement_account =
			calloc( 1, sizeof ( STATEMENT_ACCOUNT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return statement_account;
}

char *statement_account_asset_percent_string( int percent_of_asset )
{
	char percent_string[ 16 ];

	if ( percent_of_asset )
	{
		sprintf(percent_string,
			"%d%c",
			percent_of_asset,
			'%' );
	}
	else
	{
		*percent_string = '\0';
	}

	return strdup( percent_string );
}

char *statement_account_revenue_percent_string(
		int percent_of_income )
{
	char percent_string[ 16 ];

	if ( percent_of_income )
	{
		sprintf(percent_string,
			"%d%c",
			percent_of_income,
			'%' );
	}
	else
	{
		*percent_string = '\0';
	}

	return strdup( percent_string );
}

char *statement_account_percent_string(
		int percent_of_asset,
		int percent_of_income )
{
	char percent_string[ 16 ];

	if ( percent_of_asset )
	{
		sprintf(percent_string,
			"%d%c (A)",
			percent_of_asset,
			'%' );
	}
	else
	if ( percent_of_income )
	{
		sprintf(percent_string,
			"%d%c (I)",
			percent_of_income,
			'%' );
	}
	else
	{
		*percent_string = '\0';
	}

	return strdup( percent_string );
}

STATEMENT_SUBCLASS_DISPLAY_LATEX_LIST *
	statement_subclass_display_latex_list_new(
		LIST *element_statement_list,
		LIST *statement_prior_year_list )
{
	STATEMENT_SUBCLASS_DISPLAY_LATEX_LIST *
		statement_subclass_display_latex_list;
	ELEMENT *element;

	if ( !list_rewind( element_statement_list ) ) return NULL;

	statement_subclass_display_latex_list =
		statement_subclass_display_latex_list_calloc();

	statement_subclass_display_latex_list->column_list =
		statement_subclass_display_latex_list_column_list(
			statement_prior_year_list );

	statement_subclass_display_latex_list->list = list_new();

	do {
		element =
			list_get(
				element_statement_list );

		if ( !element->sum ) continue;

		list_set(
			statement_subclass_display_latex_list->list,
			statement_subclass_display_latex_new(
				element,
				statement_prior_year_list,
				statement_subclass_display_latex_list->
					column_list
					/* latex_column_list */ ) );

	} while ( list_next( element_statement_list ) );

	return statement_subclass_display_latex_list;
}

STATEMENT_SUBCLASS_DISPLAY_LATEX_LIST *
	statement_subclass_display_latex_list_calloc(
		void )
{
	STATEMENT_SUBCLASS_DISPLAY_LATEX_LIST *
		statement_subclass_display_latex_list;

	if ( ! ( statement_subclass_display_latex_list =
		   calloc(
			1,
			sizeof ( STATEMENT_SUBCLASS_DISPLAY_LATEX_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return statement_subclass_display_latex_list;
}

LIST *statement_subclass_display_latex_list_column_list(
		LIST *statement_prior_year_list )
{
	LIST *column_list;
	LATEX_COLUMN *latex_column;

	column_list = list_new();

	list_set(
		column_list,
		latex_column_new(
			(char *)0 /* heading_string */,
			latex_column_text,
			0 /* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			1 /* first_column_boolean */ ) );

	list_set(
		column_list,
		latex_column_new(
			STATEMENT_ACCOUNT_HEADING,
			latex_column_float,
			2 /* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			0 /* not first_column_boolean */ ) );

	list_set(
		column_list,
		latex_column_new(
			STATEMENT_SUBCLASSIFICATION_HEADING,
			latex_column_float,
			2 /* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			0 /* not first_column_boolean */ ) );

	list_set(
		column_list,
		latex_column_new(
			STATEMENT_ELEMENT_HEADING,
			latex_column_float,
			2 /* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			0 /* not first_column_boolean */ ) );

	latex_column =
		latex_column_new(
			STATEMENT_STANDARDIZED_ESCAPED_HEADING,
			latex_column_text,
			0 /* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			0 /* not first_column_boolean */ );

	latex_column->right_justify_boolean = 1;
	list_set( column_list, latex_column );

	if ( list_length( statement_prior_year_list ) )
	{
		LIST *prior_year_heading_list =
			statement_prior_year_heading_list(
				statement_prior_year_list );

		list_set_list(
			column_list,
			latex_column_text_list(
				prior_year_heading_list,
				0 /* not first_column_boolean */,
				1 /* right_justify_boolean */ ) );
	}

	return column_list;
}

STATEMENT_SUBCLASS_DISPLAY_LATEX *
	statement_subclass_display_latex_new(
		ELEMENT *element,
		LIST *statement_prior_year_list,
		LIST *latex_column_list )
{
	STATEMENT_SUBCLASS_DISPLAY_LATEX *statement_subclass_display_latex;
	SUBCLASSIFICATION *subclassification;
	boolean subclassification_name_duplicated;

	if ( !element
	||   !list_length( latex_column_list )
	||   !list_rewind( element->subclassification_statement_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	statement_subclass_display_latex =
		statement_subclass_display_latex_calloc();

	statement_subclass_display_latex->row_list = list_new();

	list_set(
		statement_subclass_display_latex->row_list,
		statement_subclass_display_latex_element_label_row(
			element->element_name,
			list_length( statement_prior_year_list )
				/* statement_prior_year_list_length */,
			latex_column_list ) );

	do {
		subclassification =
			list_get(
				element->
					subclassification_statement_list );

		if ( !subclassification->sum ) continue;

		subclassification_name_duplicated =
			statement_subclassification_name_duplicated(
				element->element_name,
				subclassification->subclassification_name );

		if ( !subclassification_name_duplicated )
		{
		 list_set(
		  statement_subclass_display_latex->row_list,
		  statement_subclass_display_latex_subclassification_label_row(
			subclassification->subclassification_name,
			list_length( statement_prior_year_list )
				/* statement_prior_year_list_length */,
			latex_column_list ) );
		}

		list_set_list(
			statement_subclass_display_latex->row_list,
			statement_subclass_display_latex_account_row_list(
				subclassification->account_statement_list,
				statement_prior_year_list,
				latex_column_list ) );

		if ( !subclassification_name_duplicated )
		{
		  list_set(
		    statement_subclass_display_latex->row_list,
		    statement_subclass_display_latex_subclassification_sum_row(
				subclassification->subclassification_name,
				subclassification->sum,
				subclassification->percent_of_asset,
				subclassification->percent_of_income,
				statement_prior_year_list,
				latex_column_list ) );
		}

	} while ( list_next(
			element->
				subclassification_statement_list ) );

	list_set(
		statement_subclass_display_latex->row_list,
		statement_subclass_display_latex_element_sum_row(
			element->element_name,
			element->sum,
			element->percent_of_asset,
			element->percent_of_income,
			statement_prior_year_list,
			latex_column_list ) );

	return statement_subclass_display_latex;
}

STATEMENT_SUBCLASS_DISPLAY_LATEX *
	statement_subclass_display_latex_calloc(
		void )
{
	STATEMENT_SUBCLASS_DISPLAY_LATEX *statement_subclass_display_latex;

	if ( ! ( statement_subclass_display_latex =
		   calloc( 1,
			   sizeof ( STATEMENT_SUBCLASS_DISPLAY_LATEX ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return statement_subclass_display_latex;
}

STATEMENT_SUBCLASS_OMIT_LATEX *
	statement_subclass_omit_latex_new(
		ELEMENT *element,
		LIST *statement_prior_year_list,
		LIST *latex_column_list )
{
	STATEMENT_SUBCLASS_OMIT_LATEX *statement_subclass_omit_latex;
	ACCOUNT *account;
	STATEMENT_ACCOUNT *statement_account;

	if ( !element
	||   !list_length( latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( element->account_statement_list ) )
		return NULL;

	statement_subclass_omit_latex = statement_subclass_omit_latex_calloc();

	statement_subclass_omit_latex->row_list = list_new();

	list_set(
		statement_subclass_omit_latex->row_list,
		statement_subclass_omit_latex_element_label_row(
			element->element_name,
			list_length( statement_prior_year_list ),
			latex_column_list ) );

	do {
		account = list_get( element->account_statement_list );

		if ( !account->account_journal_latest )
		{
			char message[ 128 ];

			sprintf(message,
			"for account = [%s], account_journal_latest is empty.",
				account->account_name );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		statement_account =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			statement_account_new(
				(char *)0 /* end_date_time */,
				0 /* element_accumulate_debit */,
				account->account_journal_latest,
				(char *)0 /* account_action_string */,
				0 /* not round_dollar_boolean */,
				account );

		list_set(
			statement_subclass_omit_latex->row_list,
			statement_subclass_omit_latex_row(
				statement_account,
				statement_prior_year_list,
				latex_column_list ) );

	} while ( list_next( element->account_statement_list ) );

	list_set(
		statement_subclass_omit_latex->row_list,
		statement_subclass_omit_latex_element_sum_row(
			element->element_name,
			element->sum,
			element->percent_of_asset,
			element->percent_of_income,
			statement_prior_year_list,
			latex_column_list ) );

	return statement_subclass_omit_latex;
}

STATEMENT_SUBCLASS_OMIT_LATEX *
	statement_subclass_omit_latex_calloc(
		void )
{
	STATEMENT_SUBCLASS_OMIT_LATEX *statement_subclass_omit_latex;

	if ( ! ( statement_subclass_omit_latex =
			calloc( 1,
				sizeof ( STATEMENT_SUBCLASS_OMIT_LATEX ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return statement_subclass_omit_latex;
}

STATEMENT_SUBCLASS_OMIT_LATEX_LIST *
	statement_subclass_omit_latex_list_new(
		LIST *element_statement_list,
		LIST *statement_prior_year_list )
{
	STATEMENT_SUBCLASS_OMIT_LATEX_LIST *
		statement_subclass_omit_latex_list;
	ELEMENT *element;

	if ( !list_rewind( element_statement_list ) )
	{
		char message[ 128 ];

		sprintf(message, "element_statement_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	statement_subclass_omit_latex_list =
		statement_subclass_omit_latex_list_calloc();

	statement_subclass_omit_latex_list->column_list =
		statement_subclass_omit_latex_list_column_list(
			statement_prior_year_list );

	statement_subclass_omit_latex_list->list = list_new();

	do {
		element = list_get( element_statement_list );

		if ( float_virtually_same( element->sum, 0.0 ) ) continue;

		list_set(
			statement_subclass_omit_latex_list->list,
			statement_subclass_omit_latex_new(
				element,
				statement_prior_year_list,
				statement_subclass_omit_latex_list->
					column_list 
					/* latex_column_list */ ) );

	} while ( list_next( element_statement_list ) );

	return statement_subclass_omit_latex_list;
}

STATEMENT_SUBCLASS_OMIT_LATEX_LIST *
	statement_subclass_omit_latex_list_calloc(
		void )
{
	STATEMENT_SUBCLASS_OMIT_LATEX_LIST *
		statement_subclass_omit_latex_list;

	if ( ! ( statement_subclass_omit_latex_list =
		   calloc(
		      1,
		      sizeof ( STATEMENT_SUBCLASS_OMIT_LATEX_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return statement_subclass_omit_latex_list;
}

LIST *statement_subclass_omit_latex_list_column_list(
		LIST *statement_prior_year_list )
{
	LIST *column_list;
	LATEX_COLUMN *latex_column;

	column_list = list_new();

	list_set(
		column_list,
		latex_column_new(
			(char *)0 /* heading_string */,
			latex_column_text,
			0 /* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			1 /* first_column_boolean */ ) );

	list_set(
		column_list,
		latex_column_new(
			STATEMENT_ACCOUNT_HEADING,
			latex_column_float,
			2 /* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			0 /* not first_column_boolean */ ) );

	list_set(
		column_list,
		latex_column_new(
			STATEMENT_ELEMENT_HEADING,
			latex_column_float,
			2 /* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			0 /* not first_column_boolean */ ) );

	latex_column =
		latex_column_new(
			STATEMENT_STANDARDIZED_ESCAPED_HEADING,
			latex_column_text,
			0 /* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			0 /* not first_column_boolean */ );

	latex_column->right_justify_boolean = 1;
	list_set( column_list, latex_column );

	if ( list_length( statement_prior_year_list ) )
	{
		LIST *prior_year_heading_list =
			statement_prior_year_heading_list(
				statement_prior_year_list );

		list_set_list(
			column_list,
			latex_column_text_list(
				prior_year_heading_list,
				0 /* not first_column_boolean */,
				1 /* right_justify_boolean */ ) );
	}

	return column_list;
}

char *statement_element_sum_name( char *element_name )
{
	static char name[ 64 ];

	if ( !element_name )
	{
		char message[ 128 ];

		sprintf(message, "element_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(name,
		"%s element",
		element_name );

	return name;
}

char *statement_cell_label_datum( char *name )
{
	char buffer[ 128 ];

	if ( !name ) return "";

	string_initial_capital( buffer, name );

	return strdup( buffer );
}

char *statement_link_pdf_prompt( char *process_name )
{
	static char prompt[ 128 ];

	if ( !process_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: process_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		prompt,
		sizeof ( prompt ),
		"%s PDF file",
		process_name );

	return
	string_initial_capital(
		prompt,
		prompt );
}

char *statement_link_tex_prompt( char *process_name )
{
	static char prompt[ 128 ];

	if ( !process_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: process_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		prompt,
		sizeof ( prompt ),
		"%s LATEX file",
		process_name );

	return
	string_initial_capital(
		prompt,
		prompt );
}

char *statement_prior_year_cell_display(
		boolean cell_empty,
		int delta_prior_percent,
		double prior_year_amount )
{
	char cell_display[ 32 ];

	if ( cell_empty )
	{
		*cell_display = '\0';
	}
	else
	{
		sprintf(cell_display,
			"%d%c %s",
			delta_prior_percent,
			'%',
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			timlib_place_commas_in_dollars(
				prior_year_amount ) );
	}

	return strdup( cell_display );
}

LIST *statement_subclass_display_latex_list_row_list(
	STATEMENT_SUBCLASS_DISPLAY_LATEX_LIST *
		statement_subclass_display_latex_list )
{
	LIST *row_list;
	STATEMENT_SUBCLASS_DISPLAY_LATEX *
		statement_subclass_display_latex;

	if ( !statement_subclass_display_latex_list )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: statement_subclass_display_latex_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind(
		statement_subclass_display_latex_list->list ) )
	{
		return (LIST *)0;
	}

	row_list = list_new();

	do {
		statement_subclass_display_latex =
			list_get(
				statement_subclass_display_latex_list->list );

		list_set_list(
			row_list,
			statement_subclass_display_latex->row_list );

	} while ( list_next(
			statement_subclass_display_latex_list->list ) );

	return row_list;
}

LIST *statement_subclass_omit_latex_list_row_list(
	STATEMENT_SUBCLASS_OMIT_LATEX_LIST *
		statement_subclass_omit_latex_list )
{
	LIST *row_list;
	STATEMENT_SUBCLASS_OMIT_LATEX *
		statement_subclass_omit_latex;

	if ( !statement_subclass_omit_latex_list )
	{
		char message[ 128 ];

		sprintf(message,
			"statement_subclass_omit_latex_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind(
		statement_subclass_omit_latex_list->list ) )
	{
		return (LIST *)0;
	}

	row_list = list_new();

	do {
		statement_subclass_omit_latex =
			list_get(
				statement_subclass_omit_latex_list->list );

		list_set_list(
			row_list,
			statement_subclass_omit_latex->row_list );

	} while ( list_next(
			statement_subclass_omit_latex_list->list ) );

	return row_list;
}

char *statement_account_balance_string(
		double balance,
		char *account_action_string,
		boolean round_dollar_boolean )
{
	char balance_string[ 1024 ];

	if ( account_action_string )
	{
		sprintf(balance_string,
			"<a href=\"%s\">%s</a>",
			account_action_string,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			statement_account_money_string(
				balance,
				round_dollar_boolean ) );
	}
	else
	{
		strcpy(	balance_string,
			statement_account_money_string(
				balance,
				round_dollar_boolean ) );
	}

	return strdup( balance_string );
}

char *statement_account_money_string(
		double balance,
		boolean round_dollar_boolean )
{
	if ( round_dollar_boolean )
	{
		return
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		string_commas_rounded_dollar( balance );
	}
	else
	{
		return
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		string_commas_dollar( balance );
	}
}

STATEMENT_SUBCLASS_OMIT_HTML *
	statement_subclass_omit_html_new(
		ELEMENT *element,
		LIST *statement_prior_year_list )
{
	STATEMENT_SUBCLASS_OMIT_HTML *statement_subclass_omit_html;
	ACCOUNT *account;
	STATEMENT_ACCOUNT *statement_account;

	if ( !element )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: element is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( element->account_statement_list ) )
		return (STATEMENT_SUBCLASS_OMIT_HTML *)0;

	statement_subclass_omit_html = statement_subclass_omit_html_calloc();
	statement_subclass_omit_html->row_list = list_new();

	list_set(
		statement_subclass_omit_html->row_list,
		statement_subclass_omit_html_element_label_row(
			element->element_name,
			list_length( statement_prior_year_list ) ) );

	do {
		account =
			list_get(
				element->account_statement_list );

		if ( !account->account_journal_latest )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: account_journal_latest is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		statement_account =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			statement_account_new(
				(char *)0 /* end_date_time */,
				0 /* element_accumulate_debit */,
				account->account_journal_latest,
				account->action_string,
				0 /* not round_dollar_boolean */,
				account );

		list_set(
			statement_subclass_omit_html->row_list,
			statement_subclass_omit_html_row(
				statement_account,
				statement_prior_year_list ) );

	} while ( list_next( element->account_statement_list ) );

	list_set(
		statement_subclass_omit_html->row_list,
		statement_subclass_omit_html_element_sum_row(
			element->element_name,
			element->sum,
			element->percent_of_asset,
			element->percent_of_income,
			statement_prior_year_list ) );

	return statement_subclass_omit_html;
}

STATEMENT_SUBCLASS_OMIT_HTML *
	statement_subclass_omit_html_calloc(
		void )
{
	STATEMENT_SUBCLASS_OMIT_HTML *statement_subclass_omit_html;

	if ( ! ( statement_subclass_omit_html =
			calloc( 1,
				sizeof ( STATEMENT_SUBCLASS_OMIT_HTML ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return statement_subclass_omit_html;
}

HTML_ROW *statement_subclass_omit_html_element_label_row(
		char *element_name,
		int statement_prior_year_list_length )
{
	LIST *cell_list;
	int i;

	if ( !element_name )
	{
		char message[ 128 ];

		sprintf(message, "element_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum( element_name ),
			1 /* large_boolean */,
			1 /* bold_boolean */ ) );

	for (	i = 0;
		i < statement_prior_year_list_length + 2;
		i++ )
	{
		list_set(
			cell_list,
			html_cell_new(
				(char *)0 /* datum */,
				0 /* not large_boolean */,
				0 /* not bold_boolean */ ) );
	}

	return html_row_new( cell_list );
}

HTML_ROW *statement_subclass_omit_html_element_sum_row(
		char *element_name,
		double sum,
		int percent_of_asset,
		int percent_of_income,
		LIST *statement_prior_year_list )
{
	LIST *cell_list;

	if ( !element_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: element_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	cell_list = list_new();

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				statement_element_sum_name( element_name ) ),
			1 /* large_boolean */,
			1 /* bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			(char *)0 /* datum */,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			statement_account_balance_string(
				sum /* balance */,
				(char *)0 /* account_action_string */,
				0 /* not round_dollar_boolean */ ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			statement_account_percent_string(
				percent_of_asset,
				percent_of_income ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	if ( list_length( statement_prior_year_list ) )
	{
		LIST *prior_year_element_data_list =
			statement_prior_year_element_data_list(
				element_name,
				statement_prior_year_list );

		list_set_list(
			cell_list,
			html_cell_list(
				prior_year_element_data_list ) );
	}

	return
	html_row_new( cell_list );
}

HTML_ROW *statement_subclass_omit_html_row(
		STATEMENT_ACCOUNT *statement_account,
		LIST *statement_prior_year_list )
{
	LIST *cell_list;

	if ( !statement_account )
	{
		char message[ 128 ];

		sprintf(message, "statement_account is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();


	list_set(
		cell_list,
		html_cell_new(
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum(
				statement_account->
					account->
					account_name ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			statement_account->balance_string,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			(char *)0 /* datum */,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			statement_account->percent_string,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	if ( list_length( statement_prior_year_list ) )
	{
		LIST *prior_year_account_data_list =
			statement_prior_year_account_data_list(
				statement_account->
					account->
					account_name,
				statement_prior_year_list );

		list_set_list(
			cell_list,
			html_cell_list(
				prior_year_account_data_list ) );
	}

	return html_row_new( cell_list );
}

STATEMENT_SUBCLASS_OMIT_HTML_LIST *
	statement_subclass_omit_html_list_new(
		LIST *element_statement_list,
		LIST *statement_prior_year_list )
{
	STATEMENT_SUBCLASS_OMIT_HTML_LIST *
		statement_subclass_omit_html_list;
	ELEMENT *element;

	if ( !list_rewind( element_statement_list ) )
	{
		char message[ 128 ];

		sprintf(message, "element_statement_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	statement_subclass_omit_html_list =
		statement_subclass_omit_html_list_calloc();

	statement_subclass_omit_html_list->column_list =
		statement_subclass_omit_html_list_column_list(
			statement_prior_year_list );

	statement_subclass_omit_html_list->list = list_new();

	do {
		element = list_get( element_statement_list );

		if ( !element->sum ) continue;

		list_set(
			statement_subclass_omit_html_list->list,
			statement_subclass_omit_html_new(
				element,
				statement_prior_year_list ) );

	} while ( list_next( element_statement_list ) );

	return statement_subclass_omit_html_list;
}

STATEMENT_SUBCLASS_OMIT_HTML_LIST *
	statement_subclass_omit_html_list_calloc(
		void )
{
	STATEMENT_SUBCLASS_OMIT_HTML_LIST *
		statement_subclass_omit_html_list;

	if ( ! ( statement_subclass_omit_html_list =
			calloc(
			    1,
			    sizeof ( STATEMENT_SUBCLASS_OMIT_HTML_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return statement_subclass_omit_html_list;
}

LIST *statement_subclass_omit_html_list_column_list(
		LIST *statement_prior_year_list )
{
	LIST *column_list = list_new();

	list_set(
		column_list,
		html_column_new(
			(char *)0 /* heading */,
			0 /* not right_justify_boolean */ ) );

	list_set(
		column_list,
		html_column_new(
			STATEMENT_ACCOUNT_HEADING,
			1 /* right_Justified_boolean */ ) );

	list_set(
		column_list,
		html_column_new(
			STATEMENT_ELEMENT_HEADING,
			1 /* right_justified_boolean */ ) );

	list_set(
		column_list,
		html_column_new(
			STATEMENT_STANDARDIZED_HEADING,
			1 /* right_justified_boolean */ ) );

	if ( list_length( statement_prior_year_list ) )
	{
		list_set_list(
			column_list,
			html_column_right_list(
				statement_prior_year_heading_list(
					statement_prior_year_list ) ) );
	}

	return column_list;
}

LIST *statement_subclass_omit_html_list_row_list(
	STATEMENT_SUBCLASS_OMIT_HTML_LIST *
		statement_subclass_omit_html_list )
{
	LIST *row_list;
	STATEMENT_SUBCLASS_OMIT_HTML *
		statement_subclass_omit_html;

	if ( !statement_subclass_omit_html_list )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: statement_subclass_omit_html_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind(
		statement_subclass_omit_html_list->list ) )
	{
		return (LIST *)0;
	}

	row_list = list_new();

	do {
		statement_subclass_omit_html =
			list_get(
				statement_subclass_omit_html_list->list );

		list_set_list(
			row_list,
			statement_subclass_omit_html->row_list );

	} while ( list_next(
			statement_subclass_omit_html_list->list ) );

	return row_list;
}

STATEMENT_SUBCLASS_AGGR_HTML *
	statement_subclass_aggr_html_new(
		ELEMENT *element,
		LIST *statement_prior_year_list )
{
	STATEMENT_SUBCLASS_AGGR_HTML *statement_subclass_aggr_html;
	SUBCLASSIFICATION *subclassification;

	if ( !element )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: element is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( element->subclassification_statement_list ) )
		return NULL;

	statement_subclass_aggr_html = statement_subclass_aggr_html_calloc();
	statement_subclass_aggr_html->row_list = list_new();

	list_set(
		statement_subclass_aggr_html->row_list,
		statement_subclass_aggr_html_element_label_row(
			element->element_name,
			list_length( statement_prior_year_list ) ) );

	do {
		subclassification =
			list_get(
				element->subclassification_statement_list );

		if ( !subclassification->sum ) continue;

		list_set(
			statement_subclass_aggr_html->row_list,
			statement_subclass_aggr_html_row(
				subclassification,
				statement_prior_year_list ) );

	} while ( list_next( element->subclassification_statement_list ) );

	list_set(
		statement_subclass_aggr_html->row_list,
		statement_subclass_aggr_html_element_sum_row(
			element->element_name,
			element->sum,
			element->percent_of_asset,
			element->percent_of_income,
			statement_prior_year_list ) );

	return statement_subclass_aggr_html;
}

STATEMENT_SUBCLASS_AGGR_HTML *
	statement_subclass_aggr_html_calloc(
		void )
{
	STATEMENT_SUBCLASS_AGGR_HTML *statement_subclass_aggr_html;

	if ( ! ( statement_subclass_aggr_html =
			calloc( 1,
				sizeof ( STATEMENT_SUBCLASS_AGGR_HTML ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return statement_subclass_aggr_html;
}

HTML_ROW *statement_subclass_aggr_html_element_label_row(
		char *element_name,
		int statement_prior_year_list_length )
{
	LIST *cell_list;
	int i;

	if ( !element_name )
	{
		char message[ 128 ];

		sprintf(message, "element_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum( element_name ),
			1 /* large_boolean */,
			1 /* bold_boolean */ ) );

	for (	i = 0;
		i < statement_prior_year_list_length + 2;
		i++ )
	{
		list_set(
			cell_list,
			html_cell_new(
				(char *)0 /* datum */,
				0 /* not large_boolean */,
				0 /* not bold_boolean */ ) );
	}

	return
	html_row_new( cell_list );
}

HTML_ROW *statement_subclass_aggr_html_element_sum_row(
		char *element_name,
		double sum,
		int percent_of_asset,
		int percent_of_income,
		LIST *statement_prior_year_list )
{
	LIST *cell_list;

	if ( !element_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: element_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	cell_list = list_new();

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				statement_element_sum_name( element_name ) ),
			1 /* large_boolean */,
			1 /* bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			(char *)0 /* datum */,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
			html_cell_new(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			statement_account_balance_string(
				sum /* balance */,
				(char *)0 /* account_action_string */,
				0 /* not round_dollar_boolean */ ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
			html_cell_new(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			statement_account_percent_string(
				percent_of_asset,
				percent_of_income ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	if ( list_length( statement_prior_year_list ) )
	{
		LIST *prior_year_element_data_list =
			statement_prior_year_element_data_list(
				element_name,
				statement_prior_year_list );

		list_set_list(
			cell_list,
			html_cell_list(
				prior_year_element_data_list ) );
	}

	return
	html_row_new( cell_list );
}

HTML_ROW *statement_subclass_aggr_html_row(
		SUBCLASSIFICATION *subclassification,
		LIST *statement_prior_year_list )
{
	LIST *cell_list;

	if ( !subclassification )
	{
		char message[ 128 ];

		sprintf(message, "subclassification is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum(
				subclassification->subclassification_name ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			statement_account_balance_string(
				subclassification->sum /* balance */,
				(char *)0 /* account_action_string */,
				0 /* not round_dollar_boolean */ ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			(char *)0 /* datum */,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			statement_account_percent_string(
				subclassification->percent_of_asset,
				subclassification->percent_of_income ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	if ( list_length( statement_prior_year_list ) )
	{
		LIST *prior_year_subclassification_data_list =
			statement_prior_year_subclassification_data_list(
				subclassification->subclassification_name,
				statement_prior_year_list );

		list_set_list(
			cell_list,
			html_cell_list(
				prior_year_subclassification_data_list ) );
	}

	return html_row_new( cell_list );
}

STATEMENT_SUBCLASS_AGGR_HTML_LIST *
	statement_subclass_aggr_html_list_new(
		LIST *element_statement_list,
		LIST *statement_prior_year_list )
{
	STATEMENT_SUBCLASS_AGGR_HTML_LIST *
		statement_subclass_aggr_html_list;
	ELEMENT *element;

	if ( !list_rewind( element_statement_list ) )
		return ( STATEMENT_SUBCLASS_AGGR_HTML_LIST *)0;

	statement_subclass_aggr_html_list =
		statement_subclass_aggr_html_list_calloc();

	statement_subclass_aggr_html_list->column_list =
		statement_subclass_aggr_html_list_column_list(
			statement_prior_year_list );

	statement_subclass_aggr_html_list->list = list_new();

	do {
		element = list_get( element_statement_list );

		if ( !element->sum ) continue;

		list_set(
			statement_subclass_aggr_html_list->list,
			statement_subclass_aggr_html_new(
				element,
				statement_prior_year_list ) );

	} while ( list_next( element_statement_list ) );

	return statement_subclass_aggr_html_list;
}

STATEMENT_SUBCLASS_AGGR_HTML_LIST *
	statement_subclass_aggr_html_list_calloc(
		void )
{
	STATEMENT_SUBCLASS_AGGR_HTML_LIST *
		statement_subclass_aggr_html_list;

	if ( ! ( statement_subclass_aggr_html_list =
			calloc(
			    1,
			    sizeof ( STATEMENT_SUBCLASS_AGGR_HTML_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return statement_subclass_aggr_html_list;
}

LIST *statement_subclass_aggr_html_list_column_list(
		LIST *statement_prior_year_list )
{
	LIST *column_list = list_new();

	list_set(
		column_list,
		html_column_new(
			(char *)0 /* heading */,
			0 /* not right_justify_boolean */ ) );

	list_set(
		column_list,
		html_column_new(
			STATEMENT_BALANCE_HEADING,
			1 /* right_Justified_boolean */ ) );

	list_set(
		column_list,
		html_column_new(
			STATEMENT_ELEMENT_HEADING,
			1 /* right_justified_boolean */ ) );

	list_set(
		column_list,
		html_column_new(
			STATEMENT_STANDARDIZED_HEADING,
			1 /* right_justified_boolean */ ) );

	if ( list_length( statement_prior_year_list ) )
	{
		list_set_list(
			column_list,
			html_column_right_list(
				statement_prior_year_heading_list(
					statement_prior_year_list ) ) );
	}

	return column_list;
}

LIST *statement_subclass_aggr_html_list_row_list(
	STATEMENT_SUBCLASS_AGGR_HTML_LIST *
		statement_subclass_aggr_html_list )
{
	LIST *row_list;
	STATEMENT_SUBCLASS_AGGR_HTML *
		statement_subclass_aggr_html;

	if ( !statement_subclass_aggr_html_list )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: statement_subclass_aggr_html_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind(
		statement_subclass_aggr_html_list->list ) )
	{
		return (LIST *)0;
	}

	row_list = list_new();

	do {
		statement_subclass_aggr_html =
			list_get(
				statement_subclass_aggr_html_list->list );

		list_set_list(
			row_list,
			statement_subclass_aggr_html->row_list );

	} while ( list_next(
			statement_subclass_aggr_html_list->list ) );

	return row_list;
}

STATEMENT_SUBCLASS_DISPLAY_HTML_LIST *
	statement_subclass_display_html_list_new(
		LIST *element_statement_list,
		LIST *statement_prior_year_list )
{
	STATEMENT_SUBCLASS_DISPLAY_HTML_LIST *
		statement_subclass_display_html_list;
	ELEMENT *element;

	if ( !list_rewind( element_statement_list ) )
		return (STATEMENT_SUBCLASS_DISPLAY_HTML_LIST *)0;

	statement_subclass_display_html_list =
		statement_subclass_display_html_list_calloc();

	statement_subclass_display_html_list->column_list =
		statement_subclass_display_html_list_column_list(
			statement_prior_year_list );

	statement_subclass_display_html_list->list = list_new();

	do {
		element = list_get( element_statement_list );

		if ( !float_virtually_same( element->sum, 0.0 ) )
		{
			list_set(
				statement_subclass_display_html_list->list,
				statement_subclass_display_html_new(
					element,
					statement_prior_year_list ) );
		}

	} while ( list_next( element_statement_list ) );

	return statement_subclass_display_html_list;
}

STATEMENT_SUBCLASS_DISPLAY_HTML_LIST *
	statement_subclass_display_html_list_calloc(
		void )
{
	STATEMENT_SUBCLASS_DISPLAY_HTML_LIST *
		statement_subclass_display_html_list;

	if ( ! ( statement_subclass_display_html_list =
		   calloc( 1,
			   sizeof ( STATEMENT_SUBCLASS_DISPLAY_HTML_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return statement_subclass_display_html_list;
}

LIST *statement_subclass_display_html_list_column_list(
		LIST *statement_prior_year_list )
{
	LIST *column_list = list_new();

	list_set(
		column_list,
		html_column_new(
			(char *)0 /* heading */,
			0 /* not right_justify_boolean */ ) );

	list_set(
		column_list,
		html_column_new(
			STATEMENT_ACCOUNT_HEADING,
			1 /* right_Justified_boolean */ ) );

	list_set(
		column_list,
		html_column_new(
			STATEMENT_SUBCLASSIFICATION_HEADING,
			1 /* right_justified_boolean */ ) );

	list_set(
		column_list,
		html_column_new(
			STATEMENT_ELEMENT_HEADING,
			1 /* right_justified_boolean */ ) );

	list_set(
		column_list,
		html_column_new(
			STATEMENT_STANDARDIZED_HEADING,
			1 /* right_justified_boolean */ ) );;

	if ( list_length( statement_prior_year_list ) )
	{
		list_set_list(
			column_list,
			html_column_right_list(
				statement_prior_year_heading_list(
					statement_prior_year_list ) ) );
	}

	return column_list;
}

LIST *statement_subclass_display_html_list_row_list(
	STATEMENT_SUBCLASS_DISPLAY_HTML_LIST *
		statement_subclass_display_html_list )
{
	LIST *row_list;
	STATEMENT_SUBCLASS_DISPLAY_HTML *
		statement_subclass_display_html;

	if ( !statement_subclass_display_html_list )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: statement_subclass_display_html_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( statement_subclass_display_html_list->list ) )
		return (LIST *)0;

	row_list = list_new();

	do {
		statement_subclass_display_html =
			list_get(
				statement_subclass_display_html_list->list );

		list_set_list(
			row_list,
			statement_subclass_display_html->row_list );

	} while ( list_next(
			statement_subclass_display_html_list->list ) );

	return row_list;
}

STATEMENT_SUBCLASS_DISPLAY_HTML *
	statement_subclass_display_html_new(
		ELEMENT *element,
		LIST *statement_prior_year_list )
{
	STATEMENT_SUBCLASS_DISPLAY_HTML *statement_subclass_display_html;
	SUBCLASSIFICATION *subclassification;
	boolean subclassification_name_duplicated;

	if ( !element )
	{
		char message[ 128 ];

		sprintf(message, "element is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( element->subclassification_statement_list ) )
		return NULL;

	statement_subclass_display_html =
		statement_subclass_display_html_calloc();

	statement_subclass_display_html->row_list = list_new();

	list_set(
		statement_subclass_display_html->row_list,
		statement_subclass_display_html_element_label_row(
			element->element_name,
			list_length( statement_prior_year_list )
				/* statement_prior_year_list_length */ ) );

	do {
		subclassification =
			list_get(
				element->subclassification_statement_list );

		if ( float_virtually_same( subclassification->sum, 0.0 ) )
			continue;

		subclassification_name_duplicated =
			statement_subclassification_name_duplicated(
				element->element_name,
				subclassification->subclassification_name );

		if ( !subclassification_name_duplicated )
		{
		  list_set(
		   statement_subclass_display_html->row_list,
		   statement_subclass_display_html_subclassification_label_row(
			subclassification->subclassification_name,
			list_length( statement_prior_year_list ) ) );
		}

		list_set_list(
			statement_subclass_display_html->row_list,
			statement_subclass_display_html_account_row_list(
				subclassification->account_statement_list,
				statement_prior_year_list ) );

		if ( !subclassification_name_duplicated )
		{
		  list_set(
		     statement_subclass_display_html->row_list,
		     statement_subclass_display_html_subclassification_sum_row(
			subclassification->subclassification_name,
			subclassification->sum,
			subclassification->percent_of_asset,
			subclassification->percent_of_income,
			statement_prior_year_list ) );
		}

	} while ( list_next( element->subclassification_statement_list ) );

	list_set(
		statement_subclass_display_html->row_list,
		statement_subclass_display_html_element_sum_row(
			element->element_name,
			element->sum,
			element->percent_of_asset,
			element->percent_of_income,
			statement_prior_year_list ) );

	return statement_subclass_display_html;
}

STATEMENT_SUBCLASS_DISPLAY_HTML *
	statement_subclass_display_html_calloc(
		void )
{
	STATEMENT_SUBCLASS_DISPLAY_HTML *
		statement_subclass_display_html;

	if ( ! ( statement_subclass_display_html =
		    calloc(
			1,
			sizeof ( STATEMENT_SUBCLASS_DISPLAY_HTML ) ) ) ) 
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return statement_subclass_display_html;
}

HTML_ROW *statement_subclass_display_html_element_label_row(
		char *element_name,
		int statement_prior_year_list_length )
{
	LIST *cell_list;
	int i;

	if ( !element_name )
	{
		char message[ 128 ];

		sprintf(message, "element_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum( element_name ),
			1 /* large_boolean */,
			1 /* bold_boolean */ ) );

	for (	i = 0;
		i < statement_prior_year_list_length + 4;
		i++ )
	{
		list_set(
			cell_list,
			html_cell_new(
				(char *)0 /* datum */,
				0 /* not large_boolean */,
				0 /* not bold_boolean */ ) );
	}

	return html_row_new( cell_list );
}

HTML_ROW *statement_subclass_display_html_subclassification_label_row(
		char *subclassification_name,
		int statement_prior_year_list_length )
{
	LIST *cell_list;
	int i;

	if ( !subclassification_name )
	{
		char message[ 128 ];

		sprintf(message, "subclassification_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum(
				subclassification_name ),
			0 /* not large_boolean */,
			1 /* not bold_boolean */ ) );

	for (	i = 0;
		i < statement_prior_year_list_length + 4;
		i++ )
	{
		list_set(
			cell_list,
			html_cell_new(
				(char *)0 /* datum */,
				0 /* not large_boolean */,
				0 /* not bold_boolean */ ) );
	}

	return
	html_row_new( cell_list );
}

HTML_ROW *statement_subclass_display_html_subclassification_sum_row(
		char *subclassification_name,
		double sum,
		int percent_of_asset,
		int percent_of_income,
		LIST *statement_prior_year_list )
{
	LIST *cell_list;

	if ( !subclassification_name )
	{
		char message[ 128 ];

		sprintf(message, "subclassification_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum(
				subclassification_name ),
			0 /* not large_boolean */,
			1 /* bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			(char *)0 /* datum */,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			statement_account_balance_string(
				sum /* balance */,
				(char *)0 /* account_action_string */,
				0 /* not round_dollar_boolean */ ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			(char *)0 /* datum */,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			statement_account_percent_string(
				percent_of_asset,
				percent_of_income ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	if ( list_length( statement_prior_year_list ) )
	{
		LIST *prior_year_subclassification_data_list =
			statement_prior_year_subclassification_data_list(
				subclassification_name,
				statement_prior_year_list );

		list_set_list(
			cell_list,
			html_cell_list(
				prior_year_subclassification_data_list ) );
	}

	return
	html_row_new( cell_list );
}

HTML_ROW *statement_subclass_display_html_element_sum_row(
		char *element_name,
		double sum,
		int percent_of_asset,
		int percent_of_income,
		LIST *statement_prior_year_list )
{
	LIST *cell_list;

	if ( !element_name )
	{
		char message[ 128 ];

		sprintf(message, "element_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				statement_element_sum_name(
					element_name ) ),
			1 /* large_boolean */,
			1 /* bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			(char *)0 /* datum */,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			(char *)0 /* datum */,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			statement_account_balance_string(
				sum /* balance */,
				(char *)0 /* account_action_string */,
				0 /* not round_dollar_boolean */ ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			statement_account_percent_string(
				percent_of_asset,
				percent_of_income ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	if ( list_length( statement_prior_year_list ) )
	{
		LIST *prior_year_element_data_list =
			statement_prior_year_element_data_list(
				element_name,
				statement_prior_year_list );

		list_set_list(
			cell_list,
			html_cell_list(
				prior_year_element_data_list ) );
	}

	return
	html_row_new( cell_list );
}

HTML_ROW *statement_subclass_display_html_account_row(
		STATEMENT_ACCOUNT *statement_account,
		LIST *statement_prior_year_list )
{
	LIST *cell_list;

	if ( !statement_account
	||   !statement_account->account )
	{
		char message[ 128 ];

		sprintf(message, "statement_account is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum(
				statement_account->
					account->
					account_name ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			statement_account->balance_string,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			(char *)0 /* datum */,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			(char *)0 /* datum */,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			statement_account->percent_string,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	if ( list_length( statement_prior_year_list ) )
	{
		list_set_list(
			cell_list,
			html_cell_list(
				statement_prior_year_account_data_list(
					statement_account->
						account->
						account_name,
					statement_prior_year_list ) ) );
	}

	return
	html_row_new( cell_list );
}

LIST *statement_subclass_display_html_account_row_list(
		LIST *account_statement_list,
		LIST *statement_prior_year_list )
{
	LIST *row_list;
	ACCOUNT *account;
	STATEMENT_ACCOUNT *statement_account;

	if ( !list_rewind( account_statement_list ) )
		return (LIST *)0;

	row_list = list_new();

	do {
		account = list_get( account_statement_list );

		if ( !account->account_journal_latest )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: account_journal_latest is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		statement_account =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			statement_account_new(
				(char *)0 /* end_date_time */,
				0 /* element_accumulate_debit */,
				account->account_journal_latest,
				account->action_string,
				0 /* not round_dollar_boolean */,
				account );

		list_set(
			row_list,
			statement_subclass_display_html_account_row(
				statement_account,
				statement_prior_year_list ) );

	} while ( list_next( account_statement_list ) );

	return row_list;
}

STATEMENT_SUBCLASS_AGGR_LATEX *
	statement_subclass_aggr_latex_new(
		ELEMENT *element,
		LIST *statement_prior_year_list,
		LIST *latex_column_list )
{
	STATEMENT_SUBCLASS_AGGR_LATEX *statement_subclass_aggr_latex;
	SUBCLASSIFICATION *subclassification;

	if ( !element
	||   !list_length( latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( element->subclassification_statement_list ) )
		return NULL;

	statement_subclass_aggr_latex = statement_subclass_aggr_latex_calloc();

	statement_subclass_aggr_latex->row_list = list_new();

	list_set(
		statement_subclass_aggr_latex->row_list,
		statement_subclass_aggr_latex_element_label_row(
			element->element_name,
			list_length( statement_prior_year_list )
				/* statement_prior_year_list_length */,
			latex_column_list ) );

	do {
		subclassification =
			list_get(
				element->subclassification_statement_list );

		if ( !float_virtually_same( subclassification->sum, 0.0 ) )
		{
			list_set(
				statement_subclass_aggr_latex->row_list,
				statement_subclass_aggr_latex_row(
					subclassification,
					statement_prior_year_list,
					latex_column_list ) );
		}

	} while ( list_next( element->subclassification_statement_list ) );

	list_set(
		statement_subclass_aggr_latex->row_list,
		statement_subclass_aggr_latex_element_sum_row(
			element->element_name,
			element->sum,
			element->percent_of_asset,
			element->percent_of_income,
			statement_prior_year_list,
			latex_column_list ) );

	return statement_subclass_aggr_latex;
}

STATEMENT_SUBCLASS_AGGR_LATEX *
	statement_subclass_aggr_latex_calloc(
		void )
{
	STATEMENT_SUBCLASS_AGGR_LATEX *statement_subclass_aggr_latex;

	if ( ! ( statement_subclass_aggr_latex =
			calloc( 1,
				sizeof ( STATEMENT_SUBCLASS_AGGR_LATEX ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return statement_subclass_aggr_latex;
}

LATEX_ROW *statement_subclass_aggr_latex_element_label_row(
		char *element_name,
		int statement_prior_year_list_length,
		LIST *latex_column_list )
{
	LIST *cell_list;
	LATEX_COLUMN *latex_column;
	int i;

	if ( !element_name
	||   !list_rewind( latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_new(
			latex_column,
			0 /* not first_row_boolean */,
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum( element_name ),
		1 /* large_boolean */,
		1 /* bold_boolean */ ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	for (	i = 0;
		i < statement_prior_year_list_length + 3;
		i++ )
	{
		list_set(
			cell_list,
			latex_cell_small_new(
				latex_column,
				0 /* not first_row_boolean */,
				(char *)0 /* datum */ ) );

		latex_column = list_get( latex_column_list );
		list_next( latex_column_list );
	}

	return
	latex_row_new( cell_list );
}

LATEX_ROW *statement_subclass_aggr_latex_element_sum_row(
		char *element_name,
		double sum,
		int percent_of_asset,
		int percent_of_income,
		LIST *statement_prior_year_list,
		LIST *latex_column_list )
{
	LIST *cell_list;
	LATEX_COLUMN *latex_column;

	if ( !element_name
	||   !list_rewind( latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_new(
			latex_column,
			0 /* not first_row_boolean */,
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				statement_element_sum_name( element_name ) ),
			1 /* large_boolean */,
			1 /* bold_boolean */ ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			(char *)0 /* datum */ ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			statement_account_balance_string(
				sum /* balance */,
				(char *)0 /* account_action_string */,
				0 /* not round_dollar_boolean */ ) ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			statement_account_percent_string(
				percent_of_asset,
				percent_of_income ) ) );

	if ( list_length( statement_prior_year_list ) )
	{
		list_set_list(
			cell_list,
			latex_cell_list(
				__FUNCTION__,
				latex_column_list,
				statement_prior_year_element_data_list(
					element_name,
					statement_prior_year_list ) ) );
	}

	return
	latex_row_new( cell_list );
}

LATEX_ROW *statement_subclass_aggr_latex_row(
		SUBCLASSIFICATION *subclassification,
		LIST *statement_prior_year_list,
		LIST *latex_column_list )
{
	LIST *cell_list;
	LATEX_COLUMN *latex_column;
	char *cell_label_datum;
	char *account_balance_string;
	char *account_percent_string;

	if ( !subclassification
	||   !list_rewind( latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	latex_column = list_get( latex_column_list );

	list_next( latex_column_list );

	cell_label_datum =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_label_datum(
			subclassification->subclassification_name );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			cell_label_datum ) );

	latex_column = list_get( latex_column_list );

	list_next( latex_column_list );

	account_balance_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_account_balance_string(
			subclassification->sum,
			(char *)0 /* account_action_string */,
			0 /* not round_dollar_boolean */ );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			account_balance_string ) );

	latex_column = list_get( latex_column_list );

	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			(char *)0 ) );

	latex_column = list_get( latex_column_list );

	list_next( latex_column_list );

	account_percent_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_account_percent_string(
			subclassification->percent_of_asset,
			subclassification->percent_of_income ),

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			account_percent_string ) );

	if ( list_length( statement_prior_year_list ) )
	{
		LIST *prior_year_subclassification_data_list =
			statement_prior_year_subclassification_data_list(
				subclassification->subclassification_name,
				statement_prior_year_list );

		list_set_list(
			cell_list,
			latex_cell_list(
				__FUNCTION__,
				latex_column_list,
				prior_year_subclassification_data_list ) );
	}

	return
	latex_row_new( cell_list );
}

STATEMENT_SUBCLASS_AGGR_LATEX_LIST *
	statement_subclass_aggr_latex_list_new(
		LIST *element_statement_list,
		LIST *statement_prior_year_list )
{
	STATEMENT_SUBCLASS_AGGR_LATEX_LIST *
		statement_subclass_aggr_latex_list;
	ELEMENT *element;

	if ( !list_rewind( element_statement_list ) )
	{
		char message[ 128 ];

		sprintf(message, "element_statement_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	statement_subclass_aggr_latex_list =
		statement_subclass_aggr_latex_list_calloc();

	statement_subclass_aggr_latex_list->column_list =
		statement_subclass_aggr_latex_list_column_list(
			statement_prior_year_list );

	statement_subclass_aggr_latex_list->list = list_new();

	do {
		element = list_get( element_statement_list );

		if ( float_virtually_same( element->sum, 0.0 ) ) continue;

		list_set(
			statement_subclass_aggr_latex_list->list,
			statement_subclass_aggr_latex_new(
				element,
				statement_prior_year_list,
				statement_subclass_aggr_latex_list->
					column_list
					/* latex_column_list */ ) );

	} while ( list_next( element_statement_list ) );

	return statement_subclass_aggr_latex_list;
}

STATEMENT_SUBCLASS_AGGR_LATEX_LIST *
	statement_subclass_aggr_latex_list_calloc(
		void )
{
	STATEMENT_SUBCLASS_AGGR_LATEX_LIST *
		statement_subclass_aggr_latex_list;

	if ( ! ( statement_subclass_aggr_latex_list =
		   calloc(
			1,
			sizeof ( STATEMENT_SUBCLASS_AGGR_LATEX_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return statement_subclass_aggr_latex_list;
}

LIST *statement_subclass_aggr_latex_list_row_list(
	STATEMENT_SUBCLASS_AGGR_LATEX_LIST *
		statement_subclass_aggr_latex_list )
{
	STATEMENT_SUBCLASS_AGGR_LATEX *statement_subclass_aggr_latex;
	LIST *row_list;

	if ( !list_rewind( statement_subclass_aggr_latex_list->list ) )
		return (LIST *)0;

	row_list = list_new();

	do {
		statement_subclass_aggr_latex =
			list_get(
				statement_subclass_aggr_latex_list->list );

		list_set_list(
			row_list,
			statement_subclass_aggr_latex->row_list );

	} while ( list_next( statement_subclass_aggr_latex_list->list ) );

	return row_list;
}

boolean statement_subclassification_name_duplicated(
		char *element_name,
		char *subclassification_name )
{
	if ( !element_name
	||   !subclassification_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return ( strcmp( subclassification_name, element_name ) == 0 );
}

STATEMENT_CAPTION *statement_caption_calloc( void )
{
	STATEMENT_CAPTION *statement_caption;

	if ( ! ( statement_caption =
			calloc( 1, sizeof ( STATEMENT_CAPTION ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return statement_caption;
}

STATEMENT_CAPTION *statement_caption_new(
		char *application_name,
		char *process_name,
		char *transaction_begin_date_string,
		char *end_date_time )
{
	STATEMENT_CAPTION *statement_caption;

	if ( !application_name
	||   !process_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	statement_caption = statement_caption_calloc();

	statement_caption->logo_filename =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		statement_caption_logo_filename(
			STATEMENT_LOGO_FILENAME_KEY );

	statement_caption->title =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		statement_caption_title(
			application_name,
			(statement_caption->logo_filename)
				? 1 : 0 /* exists_logo_filename */,
			process_name );

	statement_caption->sub_title =
		/* ----------------------------- */
		/* Returns static memory or null */
		/* ----------------------------- */
		statement_caption_sub_title(
			transaction_begin_date_string,
			end_date_time );

	statement_caption->combined =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_caption_combined(
			statement_caption->title,
			statement_caption->sub_title );

	statement_caption->date_now16 =
		date_now16( date_utc_offset() );

	statement_caption->frame_title =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		statement_caption_frame_title(
			process_name,
			statement_caption->date_now16 );

	return statement_caption;
}

STATEMENT_DELTA *statement_delta_new(
		double base_value,
		double change_value )
{
	STATEMENT_DELTA *statement_delta = statement_delta_calloc();

	statement_delta->difference =
		statement_delta_difference(
			base_value,
			change_value );

	statement_delta->percent =
		statement_delta_percent(
			base_value,
		statement_delta->difference );

	statement_delta->cell_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_delta_cell_string(
			statement_delta->difference,
			statement_delta->percent );

	return statement_delta;
}

STATEMENT_DELTA *statement_delta_calloc( void )
{
	STATEMENT_DELTA *statement_delta;

	if ( ! ( statement_delta =
			calloc( 1, sizeof ( STATEMENT_DELTA ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return statement_delta;
}

double statement_delta_difference(
		double base_value,
		double change_value )
{
	return change_value - base_value;
}

int statement_delta_percent(
		double base_value,
		double difference )
{
	double delta_percent;

	if ( float_virtually_same( base_value, 0.0 ) ) return 0;

	delta_percent = (difference / base_value) * 100.0;

	return float_round_int( delta_percent );
}

char *statement_delta_cell_string(
		double difference,
		int percent )
{
	char cell_string[ 32 ];

	sprintf(cell_string,
		"%d%c %s",
		percent,
		'%',
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		string_commas_dollar(
			difference ) );

	return strdup( cell_string );
}

LIST *statement_subclass_aggr_latex_list_column_list(
		LIST *statement_prior_year_list )
{
	LIST *column_list = list_new();
	LATEX_COLUMN *latex_column;

	list_set(
		column_list,
		latex_column_new(
			(char *)0 /* heading_string */,
			latex_column_text,
			0 /* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			1 /* first_column_boolean */ ) );

	list_set(
		column_list,
		latex_column_new(
			STATEMENT_BALANCE_HEADING,
			latex_column_float,
			2 /* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			0 /* not first_column_boolean */ ) );

	list_set(
		column_list,
		latex_column_new(
			STATEMENT_ELEMENT_HEADING,
			latex_column_float,
			2 /* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			0 /* not first_column_boolean */ ) );

	latex_column =
		latex_column_new(
			STATEMENT_STANDARDIZED_ESCAPED_HEADING,
			latex_column_text,
			0 /* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			0 /* not first_column_boolean */ );

	latex_column->right_justify_boolean = 1;
	list_set( column_list, latex_column );

	if ( list_length( statement_prior_year_list ) )
	{
		LIST *prior_year_heading_list =
			statement_prior_year_heading_list(
				statement_prior_year_list );

		list_set_list(
			column_list,
			latex_column_text_list(
				prior_year_heading_list,
				0 /* not first_column_boolean */,
				1 /* right_justify_boolean */ ) );
	}

	return column_list;
}

LATEX_ROW *statement_subclass_omit_latex_row(
		STATEMENT_ACCOUNT *statement_account,
		LIST *statement_prior_year_list,
		LIST *latex_column_list )
{
	LIST *cell_list;
	LATEX_COLUMN *latex_column;
	char *cell_label_datum;

	if ( !statement_account
	||   !statement_account->account
	||   !list_rewind( latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	cell_label_datum =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_label_datum(
			statement_account->account->account_name );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			cell_label_datum ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			statement_account->balance_string ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			(char *)0 /* datum */ ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			statement_account->percent_string ) );

	if ( list_length( statement_prior_year_list ) )
	{
		LIST *prior_year_account_data_list =
			statement_prior_year_account_data_list(
				statement_account->account->account_name,
				statement_prior_year_list );

		list_set_list(
			cell_list,
			latex_cell_list(
				__FUNCTION__,
				latex_column_list,
				prior_year_account_data_list ) );
	}

	return
	latex_row_new( cell_list );
}

LATEX_ROW *statement_subclass_omit_latex_element_sum_row(
		char *element_name,
		double sum,
		int percent_of_asset,
		int percent_of_income,
		LIST *statement_prior_year_list,
		LIST *latex_column_list )
{
	LIST *cell_list;
	LATEX_COLUMN *latex_column;

	if ( !element_name
	||   !list_rewind( latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_new(
			latex_column,
			0 /* not first_row_boolean */,
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				statement_element_sum_name( element_name ) ),
			1 /* large_boolean */,
			1 /* bold_boolean */ ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			(char *)0 /* datum */ ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			statement_account_balance_string(
				sum /* balance */,
				(char *)0 /* account_action_string */,
				0 /* not round_dollar_boolean */ ) ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			statement_account_percent_string(
				percent_of_asset,
				percent_of_income ) ) );

	if ( list_length( statement_prior_year_list ) )
	{
		LIST *prior_year_element_data_list =
			statement_prior_year_element_data_list(
				element_name,
				statement_prior_year_list );

		list_set_list(
			cell_list,
			latex_cell_list(
				__FUNCTION__,
				latex_column_list,
				prior_year_element_data_list ) );
	}

	return
	latex_row_new( cell_list );
}

char *statement_greater_year_message(
		char *transaction_date_begin_date_string,
		char *end_date_time )
{
	if ( date_greater_year_boolean(
		transaction_date_begin_date_string,
		end_date_time ) )
	{
		return STATEMENT_GREATER_YEAR_MESSAGE;
	}
	else
	{
		return NULL;
	}
}
