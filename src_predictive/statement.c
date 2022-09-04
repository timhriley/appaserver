/* ----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/statement.c				*/
/* ----------------------------------------------------------------	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include "String.h"
#include "timlib.h"
#include "date_convert.h"
#include "application.h"
#include "application_constants.h"
#include "appaserver_library.h"
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
		return subclassification_aggregate;
	}
	else
	if ( strcmp(
		subclassification_option_string,
		"display" ) == 0 )
	{
		return subclassification_display;
	}
	else
	if ( strcmp(
		subclassification_option_string,
		"omit" ) == 0 )
	{
		return subclassification_omit;
	}
	else
	{
		return subclassification_display;
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
			char *application_name,
			char *statement_logo_filename_key )
{
	char *logo_filename =
		/* ------------------------ */
		/* If not found, returns "" */
		/* ------------------------ */
		application_constants_quick_fetch(
			application_name,
			statement_logo_filename_key );

	if ( *logo_filename )
		return logo_filename;
	else
		return (char *)0;
}

char *statement_caption_subtitle(
			char *begin_date_string,
			char *end_date_string )
{
	static char subtitle[ 128 ];

	if ( !begin_date_string || !end_date_string ) return (char *)0;

	sprintf(subtitle,
		"Beginning: %s, Ending: %s",
		begin_date_string,
	 	end_date_string );

	return subtitle;
}

STATEMENT *statement_fetch(
			char *application_name,
			char *process_name,
			LIST *element_name_list,
			char *transaction_begin_date_string,
			char *transaction_as_of_date,
			char *transaction_date_time_closing,
			boolean fetch_transaction )
{
	STATEMENT *statement;

	if ( !list_length( element_name_list )
	||   !transaction_date_time_closing )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	statement = statement_calloc();

	statement->element_statement_list =
		element_statement_list(
			element_name_list,
			transaction_date_time_closing,
			1 /* fetch_subclassification */,
			1 /* fetch_account_list */,
			1 /* fetch_journal_latest */,
			fetch_transaction );

	if ( !list_length( statement->element_statement_list ) )
	{
		free( statement );
		return (STATEMENT *)0;
	}

	element_list_sum(
		statement->element_statement_list );

	element_percent_of_asset_set(
		statement->element_statement_list );

	element_percent_of_revenue_set(
		statement->element_statement_list );

	if ( application_name )
	{
		statement->statement_caption =
			statement_caption_new(
				application_name,
				process_name,
				transaction_begin_date_string,
				transaction_as_of_date );
	}

	statement->transaction_begin_date_string =
		transaction_begin_date_string;

	statement->transaction_as_of_date =
		transaction_as_of_date;

	statement->transaction_date_time_closing =
		transaction_date_time_closing;

	return statement;
}

STATEMENT *statement_calloc( void )
{
	STATEMENT *statement;

	if ( ! ( statement = calloc( 1, sizeof( STATEMENT ) ) ) )
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
			char *document_root_directory,
			char *transaction_begin_date_string,
			char *transaction_as_of_date,
			char *preclose_key,
			pid_t process_id )
{
	STATEMENT_LINK *statement_link;

	if ( !application_name
	||   !process_name
	||   !document_root_directory
	||   !transaction_begin_date_string
	||   !transaction_as_of_date )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	statement_link = statement_link_calloc();

	statement_link->filename_stem =
		/* ----------------------------------- */
		/* Returns heap memory or process_name */
		/* ----------------------------------- */
		statement_link_filename_stem(
			process_name,
			preclose_key );

	statement_link->appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
				appaserver_library_get_server_address(),
				( application_prepend_http_protocol_yn(
					application_name ) == 'y' ),
				document_root_directory,
				statement_link->filename_stem,
				application_name,
				process_id,
				(char *)0 /* session */,
				(char *)0 /* extension */ );

	statement_link->appaserver_link_file->extension = "tex";

	statement_link->appaserver_link_file->begin_date_string =
		transaction_begin_date_string;

	statement_link->appaserver_link_file->end_date_string =
		transaction_as_of_date;

	statement_link->tex_filename =
		strdup(
		     appaserver_link_get_tail_half(
			(char *)0 /* application_name */,
			statement_link->appaserver_link_file->filename_stem,
			statement_link->appaserver_link_file->begin_date_string,
			statement_link->appaserver_link_file->end_date_string,
			statement_link->appaserver_link_file->process_id,
			statement_link->appaserver_link_file->session,
			statement_link->appaserver_link_file->extension ) );

	statement_link->appaserver_link_file->extension = "dvi";

	statement_link->dvi_filename =
		strdup(
		     appaserver_link_get_tail_half(
			(char *)0 /* application_name */,
			statement_link->appaserver_link_file->filename_stem,
			statement_link->appaserver_link_file->begin_date_string,
			statement_link->appaserver_link_file->end_date_string,
			statement_link->appaserver_link_file->process_id,
			statement_link->appaserver_link_file->session,
			statement_link->appaserver_link_file->extension ) );

	statement_link->appaserver_link_file->extension = "pdf";

	statement_link->ftp_output_filename =
		appaserver_link_get_link_prompt(
			statement_link->
				appaserver_link_file->
				link_prompt->
				prepend_http_boolean,
			statement_link->
				appaserver_link_file->
				link_prompt->
				http_prefix,
			statement_link->
				appaserver_link_file->
				link_prompt->server_address,
			statement_link->appaserver_link_file->application_name,
			statement_link->appaserver_link_file->filename_stem,
			statement_link->appaserver_link_file->begin_date_string,
			statement_link->appaserver_link_file->end_date_string,
			statement_link->appaserver_link_file->process_id,
			statement_link->appaserver_link_file->session,
			statement_link->appaserver_link_file->extension );

	statement_link->working_directory =
		appaserver_link_source_directory(
			document_root_directory,
			application_name );

	return statement_link;
}

STATEMENT_LINK *statement_link_calloc( void )
{
	STATEMENT_LINK *statement_link;

	if ( ! ( statement_link =
			calloc( 1, sizeof( STATEMENT_LINK ) ) ) )
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

char *statement_link_filename_stem(
			char *process_name,
			char *preclose_key )
{
	char filename_stem[ 128 ];

	if ( !process_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: process_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !preclose_key ) return process_name;

	sprintf(filename_stem,
		"%s_%s",
		process_name,
		preclose_key );

	return strdup( filename_stem );
}

char *statement_date_american( char *date_time_string )
{
	char date_string[ 16 ];
	char date_american[ 16 ];

*date_american = '\0';

	if ( !date_convert_source_international(
		date_american,
		american,
		column(
			date_string,
			0,
			date_time_string ) ) )
	{
		return (char *)0;
	}

	return strdup( date_american );
}

char *statement_caption_string(
			char *title,
			char *subtitle )
{
	char string[ 256 ];

	if ( !title || !subtitle )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(string,
		"%s %s",
		title,
		subtitle );

	return strdup( string );
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
			format_initial_capital(
				buffer,
				process_name ) );
	}
	else
	{
		format_initial_capital(
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
				statement_prior_year->date_time_string ) );

	} while ( list_next( statement_prior_year_list ) );

	return heading_list;
}

void statement_html_output(
			HTML_TABLE *html_table,
			char *secondary_title )
{
	if ( !html_table )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: html_table is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( secondary_title )
	{
		printf( "<h1>%s</h1>\n", secondary_title );
	}

	html_table_output(
		html_table,
		HTML_TABLE_ROWS_BETWEEN_HEADING );
}

void statement_latex_output(
			LATEX *latex,
			char *ftp_output_filename,
			char *prompt,
			char *process_name,
			char *statement_date_time_string )
{
	if ( !latex )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: latex is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !ftp_output_filename )
	{
		if ( latex ) fclose( latex->output_stream );

		fprintf(stderr,
			"ERROR in %s/%s()/%d: ftp_output_filename is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !prompt )
	{
		if ( latex ) fclose( latex->output_stream );

		fprintf(stderr,
			"ERROR in %s/%s()/%d: prompt is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !process_name )
	{
		if ( latex ) fclose( latex->output_stream );

		fprintf(stderr,
			"ERROR in %s/%s()/%d: process_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !statement_date_time_string )
	{
		if ( latex ) fclose( latex->output_stream );

		fprintf(stderr,
		"ERROR in %s/%s()/%d: statement_date_time_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	latex_longtable_output(
		latex->output_stream,
		latex->landscape_flag,
		latex->table_list,
		latex->logo_filename,
		0 /* not omit_page_numbers */,
		statement_date_time_string /* footline */ );

	fclose( latex->output_stream );

	latex_tex2pdf(
		latex->tex_filename,
		latex->working_directory );

	appaserver_library_output_ftp_prompt(
		ftp_output_filename,
		prompt,
		process_name /* target_frame */,
		(char *)0 /* mime_type */ );
}

LIST *statement_prior_year_list(
			LIST *element_name_list,
			char *transaction_date_time_closing,
			int prior_year_count,
			STATEMENT *statement )
{
	LIST *prior_year_list;
	int years_ago;

	if ( !list_length( element_name_list )
	||   !transaction_date_time_closing
	||   !prior_year_count
	||   !statement
	||   !list_length( statement->element_statement_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
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
				transaction_date_time_closing,
				years_ago,
				statement ) );
	}

	return prior_year_list;
}

STATEMENT_PRIOR_YEAR *statement_prior_year_fetch(
			LIST *element_name_list,
			char *transaction_date_time_closing,
			int years_ago,
			STATEMENT *statement )
{
	STATEMENT_PRIOR_YEAR *statement_prior_year;
	ELEMENT *current_element;
	ELEMENT *prior_element;

	if ( !list_length( element_name_list )
	||   !transaction_date_time_closing
	||   !years_ago
	||   !statement )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( statement->element_statement_list ) )
		return (STATEMENT_PRIOR_YEAR *)0;

	statement_prior_year = statement_prior_year_calloc();

	statement_prior_year->date_time_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_prior_year_date_time_string(
			transaction_date_time_closing,
			years_ago );

	statement_prior_year->element_statement_list =
		element_statement_list(
			element_name_list,
			statement_prior_year->date_time_string,
			1 /* fetch_subclassification_list */,
			1 /* fetch_account_list */,
			1 /* fetch_journal_latest */,
			0 /* not fetch_transaction */ );

	if ( !list_length( statement_prior_year->element_statement_list ) )
	{
		return (STATEMENT_PRIOR_YEAR *)0;
	}

	element_list_sum( statement_prior_year->element_statement_list );

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
			calloc( 1, sizeof( STATEMENT_PRIOR_YEAR ) ) ) )
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

char *statement_prior_year_date_time_string(
			char *transaction_date_time_closing,
			int years_ago )
{
	DATE *prior_date;

	if ( !transaction_date_time_closing
	||   !years_ago )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	prior_date = date_19new( transaction_date_time_closing );

	date_subtract_year( prior_date, years_ago );

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	date_display19( prior_date );
}

char *statement_caption_date_time_string(
			char *date_time_string )
{
	return date_time_string;
}

char *statement_caption_frame_title(
			char *process_name,
			char *statement_date_time_string )
{
	static char title[ 128 ];
	char buffer[ 64 ];

	if ( !process_name
	||   !statement_date_time_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(title,
		"<h1>%s</h1>\n<h2>%s</h2>\n",
		format_initial_capital(
			buffer,
			process_name ),
		statement_date_time_string );

	return title;
}

boolean statement_pdf_landscape_boolean(
	int statement_prior_year_list_length )
{
	return (boolean)( statement_prior_year_list_length > 0 );
}

LATEX_ROW *statement_subclass_display_latex_element_label_row(
			char *element_name,
			int statement_prior_year_list_length )
{
	LATEX_ROW *latex_row;
	int i;

	if ( !element_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: element_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	latex_row = latex_row_new();

	latex_column_data_set(
		latex_row->column_data_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label( element_name ),
		1 /* large_boolean */,
		1 /* bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	for (	i = 0;
		i < statement_prior_year_list_length;
		i++ )
	{
		latex_column_data_set(
			latex_row->column_data_list,
			"",
			0 /* not large_boolean */,
			0 /* not bold_boolean */ );
	}

	return latex_row;
}

LATEX_ROW *statement_subclass_display_latex_subclassification_label_row(
			char *subclassification_name,
			int statement_prior_year_list_length )
{
	LATEX_ROW *latex_row;
	int i;

	if ( !subclassification_name ) return (LATEX_ROW *)0;

	latex_row = latex_row_new();

	latex_column_data_set(
		latex_row->column_data_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label( subclassification_name ),
		0 /* not large_boolean */,
		1 /* bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	for (	i = 0;
		i < statement_prior_year_list_length;
		i++ )
	{
		latex_column_data_set(
			latex_row->column_data_list,
			"",
			0 /* not large_boolean */,
			0 /* not bold_boolean */ );
	}

	return latex_row;
}

LATEX_ROW *statement_subclass_display_latex_subclassification_sum_row(
			char *subclassification_name,
			double sum,
			int percent_of_asset,
			int percent_of_revenue,
			LIST *statement_prior_year_list )
{
	LATEX_ROW *latex_row;

	if ( !subclassification_name )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: subclassification_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	latex_row = latex_row_new();

	latex_column_data_set(
		latex_row->column_data_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label( subclassification_name ),
		0 /* not large_boolean */,
		1 /* bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_account_balance_string(
			sum /* balance */,
			(char *)0 /* account_action_string */,
			0 /* not round_dollar_boolean */ ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_account_percent_string(
			percent_of_asset,
			percent_of_revenue ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	if ( list_length( statement_prior_year_list ) )
	{
		latex_column_data_list_set(
			latex_row->column_data_list,
			statement_prior_year_subclassification_data_list(
				subclassification_name,
				statement_prior_year_list ) );
	}

	return latex_row;
}

LATEX_ROW *statement_subclass_display_latex_element_sum_row(
			char *element_name,
			double sum,
			int percent_of_asset,
			int percent_of_revenue,
			LIST *statement_prior_year_list )
{
	LATEX_ROW *latex_row;

	if ( !element_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: element_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	latex_row = latex_row_new();

	latex_column_data_set(
		latex_row->column_data_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			/* ----------------------------- */
			/* Returns static memory or null */
			/* ----------------------------- */
			statement_element_name( element_name ) ),
		1 /* not large_boolean */,
		1 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_account_balance_string(
			sum /* balance */,
			(char *)0 /* account_action_string */,
			0 /* not round_dollar_boolean */ ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_account_percent_string(
			percent_of_asset,
			percent_of_revenue ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	if ( list_length( statement_prior_year_list ) )
	{
		latex_column_data_list_set(
			latex_row->column_data_list,
			statement_prior_year_element_data_list(
				element_name,
				statement_prior_year_list ) );
	}

	return latex_row;
}

LIST *statement_subclass_display_latex_account_row_list(
			LIST *account_statement_list,
			LIST *statement_prior_year_list )
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
			fprintf(stderr,
		"ERROR in %s/%s()/%d: account_journal_latest is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		statement_account =
			statement_account_new(
				(char *)0 /* transaction_as_of_date */,
				0 /* element_accumulate_debit */,
				account->account_journal_latest,
				(char *)0 /* account_action_string */,
				0 /* not round_dollar_boolean */,
				account );

		list_set(
			row_list,
			statement_subclass_display_latex_account_row(
				statement_account,
				statement_prior_year_list ) );

	} while ( list_next( account_statement_list ) );

	return row_list;
}

LATEX_ROW *statement_subclass_omit_latex_element_label_row(
			char *element_name,
			int statement_prior_year_list_length )
{
	LATEX_ROW *latex_row;
	int i;

	if ( !element_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: element_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	latex_row = latex_row_new();

	latex_column_data_set(
		latex_row->column_data_list,
		statement_cell_data_label( element_name ),
		1 /* not large_boolean */,
		1 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	for (	i = 0;
		i < statement_prior_year_list_length;
		i++ )
	{
		latex_column_data_set(
			latex_row->column_data_list,
			"",
			0 /* not large_boolean */,
			0 /* not bold_boolean */ );
	}

	return latex_row;
}

LATEX_ROW *statement_subclass_omit_latex_element_sum_row(
			char *element_name,
			double sum,
			int percent_of_asset,
			int percent_of_revenue,
			LIST *statement_prior_year_list )
{
	LATEX_ROW *latex_row;

	if ( !element_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: element_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	latex_row = latex_row_new();

	latex_column_data_set(
		latex_row->column_data_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			/* ----------------------------- */
			/* Returns static memory or null */
			/* ----------------------------- */
			statement_element_name( element_name ) ),
		1 /* large_boolean */,
		1 /* bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_account_balance_string(
			sum /* balance */,
			(char *)0 /* account_action_string */,
			0 /* not round_dollar_boolean */ ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_account_percent_string(
			percent_of_asset,
			percent_of_revenue ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	if ( list_length( statement_prior_year_list ) )
	{
		latex_column_data_list_set(
			latex_row->column_data_list,
			statement_prior_year_element_data_list(
				element_name,
				statement_prior_year_list ) );
	}

	return latex_row;
}

LATEX_ROW *statement_subclass_display_latex_account_row(
			STATEMENT_ACCOUNT *statement_account,
			LIST *statement_prior_year_list )
{
	LATEX_ROW *latex_row;

	if ( !statement_account
	||   !statement_account->account )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: statement_account is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	latex_row = latex_row_new();

	latex_column_data_set(
		latex_row->column_data_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			statement_account->account->account_name ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		statement_account->balance_string,
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		statement_account->percent_string,
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	if ( list_length( statement_prior_year_list ) )
	{
		latex_column_data_list_set(
			latex_row->column_data_list,
			statement_prior_year_account_data_list(
				statement_account->account->account_name,
				statement_prior_year_list ) );
	}

	return latex_row;
}

LATEX_ROW *statement_subclass_omit_latex_account_row(
			STATEMENT_ACCOUNT *statement_account,
			LIST *statement_prior_year_list )
{
	LATEX_ROW *latex_row;

	if ( !statement_account
	||   !statement_account->account )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: statement_account is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	latex_row = latex_row_new();

	latex_column_data_set(
		latex_row->column_data_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			statement_account->account->account_name ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		statement_account->balance_string,
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		statement_account->percent_string,
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	if ( list_length( statement_prior_year_list ) )
	{
		latex_column_data_list_set(
			latex_row->column_data_list,
			statement_prior_year_account_data_list(
				statement_account->account->account_name,
				statement_prior_year_list ) );
	}

	return latex_row;
}

STATEMENT_ACCOUNT *statement_account_new(
			char *transaction_as_of_date,
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
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	statement_account = statement_account_calloc();

	statement_account->balance = account_journal_latest->balance;

	/* For non trial_balance reports */
	/* ----------------------------- */
	statement_account->balance_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_account_balance_string(
			statement_account->balance,
			account_action_string,
			round_dollar_boolean );

	/* For trial_balance report */
	/* ------------------------ */
	if ( statement_account->balance < 0.0 )
	{
		statement_account->balance =
			0.0 - statement_account->balance;

		element_accumulate_debit = 1 - element_accumulate_debit;
	}

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
			account->percent_of_revenue );

	statement_account->percent_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_account_percent_string(
			account->percent_of_asset,
			account->percent_of_revenue );

	if ( transaction_as_of_date )
	{
		statement_account->date_days_between =
			/* ---------------------- */
			/* Ignores trailing time. */
			/* ---------------------- */
			date_days_between(
				account_journal_latest->transaction_date_time,
				transaction_as_of_date );

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
			calloc( 1, sizeof( STATEMENT_ACCOUNT ) ) ) )
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

char *statement_account_asset_percent_string(
			int percent_of_asset )
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
			int percent_of_revenue )
{
	char percent_string[ 16 ];

	if ( percent_of_revenue )
	{
		sprintf(percent_string,
			"%d%c",
			percent_of_revenue,
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
			int percent_of_revenue )
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
	if ( percent_of_revenue )
	{
		sprintf(percent_string,
			"%d%c (R)",
			percent_of_revenue,
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

	if ( !list_rewind( element_statement_list ) )
	{
		return (STATEMENT_SUBCLASS_DISPLAY_LATEX_LIST *)0;
	}

	statement_subclass_display_latex_list =
		statement_subclass_display_latex_list_calloc();

	statement_subclass_display_latex_list->heading_list =
		statement_subclass_display_latex_list_heading_list(
			statement_prior_year_list );

	statement_subclass_display_latex_list->list = list_new();

	do {
		element =
			list_get(
				element_statement_list );

		if ( element->sum )
		{
			list_set(
				statement_subclass_display_latex_list->list,
				statement_subclass_display_latex_new(
					element,
					statement_prior_year_list ) );
		}

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
			sizeof( STATEMENT_SUBCLASS_DISPLAY_LATEX_LIST ) ) ) )
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

LIST *statement_subclass_display_latex_list_heading_list(
			LIST *statement_prior_year_list )
{
	LIST *heading_list;

	heading_list = list_new();

	list_set(
		heading_list,
		latex_table_heading_new(
			"",
			0 /* right_justified_flag */,
			(char *)0 /* paragraph_size */ ) );

	list_set(
		heading_list,
		latex_table_heading_new(
			"Account",
			1 /* right_justified_flag */,
			(char *)0 /* paragraph_size */ ) );

	list_set(
		heading_list,
		latex_table_heading_new(
			"Subclassification",
			1 /* right_justified_flag */,
			(char *)0 /* paragraph_size */ ) );

	list_set(
		heading_list,
		latex_table_heading_new(
			"Element",
			1 /* right_justified_flag */,
			(char *)0 /* paragraph_size */ ) );

	list_set(
		heading_list,
		latex_table_heading_new(
			"Standardized",
			1 /* right_justified_flag */,
			(char *)0 /* paragraph_size */ ) );

	if ( list_length( statement_prior_year_list ) )
	{
		list_set_list(
			heading_list,
			latex_table_right_heading_list(
				statement_prior_year_heading_list(
					statement_prior_year_list ) ) );
	}

	return heading_list;
}

STATEMENT_SUBCLASS_DISPLAY_LATEX *
	statement_subclass_display_latex_new(
			ELEMENT *element,
			LIST *statement_prior_year_list )
{
	STATEMENT_SUBCLASS_DISPLAY_LATEX *statement_subclass_display_latex;
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
		return ( STATEMENT_SUBCLASS_DISPLAY_LATEX *)0;

	statement_subclass_display_latex =
		statement_subclass_display_latex_calloc();

	statement_subclass_display_latex->row_list = list_new();

	list_set(
		statement_subclass_display_latex->row_list,
		statement_subclass_display_latex_element_label_row(
			element->element_name,
			list_length( statement_prior_year_list ) ) );

	do {
		subclassification =
			list_get(
				element->
					subclassification_statement_list );

		if ( !subclassification->sum ) continue;

		list_set(
		  statement_subclass_display_latex->row_list,
		  statement_subclass_display_latex_subclassification_label_row(
			/* -------------------------------------- */
			/* Returns subclassification_name or null */
			/* -------------------------------------- */
			statement_subclass_display_name(
				element->element_name,
				subclassification->subclassification_name ),
			list_length( statement_prior_year_list ) ) );

		list_set_list(
			statement_subclass_display_latex->row_list,
			statement_subclass_display_latex_account_row_list(
				subclassification->account_statement_list,
				statement_prior_year_list ) );

		list_set(
		    statement_subclass_display_latex->row_list,
		    statement_subclass_display_latex_subclassification_sum_row(
				subclassification->subclassification_name,
				subclassification->sum,
				subclassification->percent_of_asset,
				subclassification->percent_of_revenue,
				statement_prior_year_list ) );

	} while ( list_next(
			element->subclassification_statement_list ) );

	list_set(
		statement_subclass_display_latex->row_list,
		statement_subclass_display_latex_element_sum_row(
			element->element_name,
			element->sum,
			element->percent_of_asset,
			element->percent_of_revenue,
			statement_prior_year_list ) );

	return statement_subclass_display_latex;
}

STATEMENT_SUBCLASS_DISPLAY_LATEX *
	statement_subclass_display_latex_calloc(
			void )
{
	STATEMENT_SUBCLASS_DISPLAY_LATEX *statement_subclass_display_latex;

	if ( ! ( statement_subclass_display_latex =
		   calloc( 1,
			   sizeof( STATEMENT_SUBCLASS_DISPLAY_LATEX ) ) ) )
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
			LIST *statement_prior_year_list )
{
	STATEMENT_SUBCLASS_OMIT_LATEX *statement_subclass_omit_latex;
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
		return (STATEMENT_SUBCLASS_OMIT_LATEX *)0;

	statement_subclass_omit_latex = statement_subclass_omit_latex_calloc();

	statement_subclass_omit_latex->row_list = list_new();

	list_set(
		statement_subclass_omit_latex->row_list,
		statement_subclass_omit_latex_element_label_row(
			element->element_name,
			list_length( statement_prior_year_list ) ) );

	do {
		account = list_get( element->account_statement_list );

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
			statement_account_new(
				(char *)0 /* transaction_as_of_date */,
				0 /* element_accumulate_debit */,
				account->account_journal_latest,
				(char *)0 /* account_action_string */,
				0 /* not round_dollar_boolean */,
				account );

		list_set(
			statement_subclass_omit_latex->row_list,
			statement_subclass_omit_latex_account_row(
				statement_account,
				statement_prior_year_list ) );

	} while ( list_next( element->account_statement_list ) );

	list_set(
		statement_subclass_omit_latex->row_list,
		statement_subclass_omit_latex_element_sum_row(
			element->element_name,
			element->sum,
			element->percent_of_asset,
			element->percent_of_revenue,
			statement_prior_year_list ) );

	return statement_subclass_omit_latex;
}

STATEMENT_SUBCLASS_OMIT_LATEX *
	statement_subclass_omit_latex_calloc(
			void )
{
	STATEMENT_SUBCLASS_OMIT_LATEX *statement_subclass_omit_latex;

	if ( ! ( statement_subclass_omit_latex =
			calloc( 1, sizeof( STATEMENT_SUBCLASS_OMIT_LATEX ) ) ) )
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
		fprintf(stderr,
		"ERROR in %s/%s()/%d: element_statement_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	statement_subclass_omit_latex_list =
		statement_subclass_omit_latex_list_calloc();

	statement_subclass_omit_latex_list->heading_list =
		statement_subclass_omit_latex_list_heading_list(
			statement_prior_year_list );

	statement_subclass_omit_latex_list->list = list_new();

	do {
		element = list_get( element_statement_list );

		if ( element->sum )
		{
			list_set(
				statement_subclass_omit_latex_list->list,
				statement_subclass_omit_latex_new(
					element,
					statement_prior_year_list ) );
		}

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
			      sizeof( STATEMENT_SUBCLASS_OMIT_LATEX_LIST ) ) ) )
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

LIST *statement_subclass_omit_latex_list_heading_list(
			LIST *statement_prior_year_list )
{
	LIST *heading_list;

	heading_list = list_new();

	list_set(
		heading_list,
		latex_table_heading_new(
			"",
			0 /* right_justified_flag */,
			(char *)0 /* paragraph_size */ ) );

	list_set(
		heading_list,
		latex_table_heading_new(
			"Account",
			1 /* right_justified_flag */,
			(char *)0 /* paragraph_size */ ) );

	list_set(
		heading_list,
		latex_table_heading_new(
			"Element",
			1 /* right_justified_flag */,
			(char *)0 /* paragraph_size */ ) );

	list_set(
		heading_list,
		latex_table_heading_new(
			"Standardized",
			1 /* right_justified_flag */,
			(char *)0 /* paragraph_size */ ) );

	if ( list_length( statement_prior_year_list ) )
	{
		list_set_list(
			heading_list,
			latex_table_right_heading_list(
				statement_prior_year_heading_list(
					statement_prior_year_list ) ) );
	}

	return heading_list;
}

char *statement_element_name( char *element_name )
{
	static char name[ 64 ];

	if ( !element_name ) return (char *)0;

	sprintf(name,
		"%s element",
		element_name );

	return name;
}

char *statement_cell_data_label( char *name )
{
	char buffer[ 128 ];

	if ( !name ) return "";

	format_initial_capital( buffer, name );

	return strdup( buffer );
}

char *statement_pdf_prompt( char *process_name )
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

	format_initial_capital( prompt, process_name );

	return prompt;
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

LIST *statement_subclass_display_latex_list_extract_row_list(
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

LIST *statement_subclass_omit_latex_list_extract_row_list(
	STATEMENT_SUBCLASS_OMIT_LATEX_LIST *
			statement_subclass_omit_latex_list )
{
	LIST *row_list;
	STATEMENT_SUBCLASS_OMIT_LATEX *
		statement_subclass_omit_latex;

	if ( !statement_subclass_omit_latex_list )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: statement_subclass_omit_latex_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
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
		timlib_place_commas_in_dollars( balance );
	}
	else
	{
		return
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		timlib_place_commas_in_money( balance );
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
			statement_account_new(
				(char *)0 /* transaction_as_of_date */,
				0 /* element_accumulate_debit */,
				account->account_journal_latest,
				account->action_string,
				0 /* not round_dollar_boolean */,
				account );

		list_set(
			statement_subclass_omit_html->row_list,
			statement_subclass_omit_html_account_row(
				statement_account,
				statement_prior_year_list ) );

	} while ( list_next( element->account_statement_list ) );

	list_set(
		statement_subclass_omit_html->row_list,
		statement_subclass_omit_html_element_sum_row(
			element->element_name,
			element->sum,
			element->percent_of_asset,
			element->percent_of_revenue,
			statement_prior_year_list ) );

	return statement_subclass_omit_html;
}

STATEMENT_SUBCLASS_OMIT_HTML *
	statement_subclass_omit_html_calloc(
			void )
{
	STATEMENT_SUBCLASS_OMIT_HTML *statement_subclass_omit_html;

	if ( ! ( statement_subclass_omit_html =
			calloc( 1, sizeof( STATEMENT_SUBCLASS_OMIT_HTML ) ) ) )
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
	HTML_ROW *html_row;
	int i;

	if ( !element_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: element_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	html_row = html_row_new();

	html_cell_data_set(
		html_row->cell_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label( element_name ),
		1 /* large_boolean */,
		1 /* bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	for (	i = 0;
		i < statement_prior_year_list_length;
		i++ )
	{
		html_cell_data_set(
			html_row->cell_list,
			"",
			0 /* not large_boolean */,
			0 /* not bold_boolean */ );
	}

	return html_row;
}

HTML_ROW *statement_subclass_omit_html_element_sum_row(
			char *element_name,
			double sum,
			int percent_of_asset,
			int percent_of_revenue,
			LIST *statement_prior_year_list )
{
	HTML_ROW *html_row;

	if ( !element_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: element_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	html_row = html_row_new();

	html_cell_data_set(
		html_row->cell_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			/* ----------------------------- */
			/* Returns static memory or null */
			/* ----------------------------- */
			statement_element_name( element_name ) ),
		1 /* large_boolean */,
		1 /* bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_account_balance_string(
			sum /* balance */,
			(char *)0 /* account_action_string */,
			0 /* not round_dollar_boolean */ ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_account_percent_string(
			percent_of_asset,
			percent_of_revenue ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	if ( list_length( statement_prior_year_list ) )
	{
		html_cell_data_list_set(
			html_row->cell_list,
			statement_prior_year_element_data_list(
				element_name,
				statement_prior_year_list ) );
	}

	return html_row;
}

HTML_ROW *statement_subclass_omit_html_account_row(
			STATEMENT_ACCOUNT *statement_account,
			LIST *statement_prior_year_list )
{
	HTML_ROW *html_row;

	if ( !statement_account )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: statement_account is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}


	html_row = html_row_new();

	html_cell_data_set(
		html_row->cell_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			statement_account->
				account->
				account_name ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		statement_account->balance_string,
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		statement_account->percent_string,
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	if ( list_length( statement_prior_year_list ) )
	{
		html_cell_data_list_set(
			html_row->cell_list,
			statement_prior_year_account_data_list(
				statement_account->
					account->
					account_name,
				statement_prior_year_list ) );
	}

	return html_row;
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
		return ( STATEMENT_SUBCLASS_OMIT_HTML_LIST *)0;

	statement_subclass_omit_html_list =
		statement_subclass_omit_html_list_calloc();

	statement_subclass_omit_html_list->heading_list =
		statement_subclass_omit_html_list_heading_list(
			statement_prior_year_list );

	statement_subclass_omit_html_list->list = list_new();

	do {
		element = list_get( element_statement_list );

		if ( element->sum )
		{
			list_set(
				statement_subclass_omit_html_list->list,
				statement_subclass_omit_html_new(
					element,
					statement_prior_year_list ) );
		}

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
			    sizeof( STATEMENT_SUBCLASS_OMIT_HTML_LIST ) ) ) )
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

LIST *statement_subclass_omit_html_list_heading_list(
			LIST *statement_prior_year_list )
{
	LIST *heading_list = list_new();

	list_set(
		heading_list,
		html_heading_new(
			"",
			0 /* right_justify_boolean */ ) );

	list_set(
		heading_list,
		html_heading_new(
			"Account",
			1 /* right_Justified_boolean */ ) );

	list_set(
		heading_list,
		html_heading_new(
			"Element",
			1 /* right_justified_boolean */ ) );

	list_set(
		heading_list,
		html_heading_new(
			"Standardized",
			1 /* right_justified_boolean */ ) );

	html_heading_list_right_justify_set(
		heading_list,
		statement_prior_year_heading_list(
			statement_prior_year_list ) );

	return heading_list;
}

LIST *statement_subclass_omit_html_list_extract_row_list(
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
		return (STATEMENT_SUBCLASS_AGGR_HTML *)0;

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

		if ( subclassification->sum )
		{
			list_set(
				statement_subclass_aggr_html->row_list,
				statement_subclass_aggr_html_row(
					subclassification,
					statement_prior_year_list ) );
		}

	} while ( list_next( element->subclassification_statement_list ) );

	list_set(
		statement_subclass_aggr_html->row_list,
		statement_subclass_aggr_html_element_sum_row(
			element->element_name,
			element->sum,
			element->percent_of_asset,
			element->percent_of_revenue,
			statement_prior_year_list ) );

	return statement_subclass_aggr_html;
}

STATEMENT_SUBCLASS_AGGR_HTML *
	statement_subclass_aggr_html_calloc(
			void )
{
	STATEMENT_SUBCLASS_AGGR_HTML *statement_subclass_aggr_html;

	if ( ! ( statement_subclass_aggr_html =
			calloc( 1, sizeof( STATEMENT_SUBCLASS_AGGR_HTML ) ) ) )
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
	HTML_ROW *html_row;
	int i;

	if ( !element_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: element_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	html_row = html_row_new();

	html_cell_data_set(
		html_row->cell_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label( element_name ),
		1 /* large_boolean */,
		1 /* bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	for (	i = 0;
		i < statement_prior_year_list_length;
		i++ )
	{
		html_cell_data_set(
			html_row->cell_list,
			"",
			0 /* not large_boolean */,
			0 /* not bold_boolean */ );
	}

	return html_row;
}

HTML_ROW *statement_subclass_aggr_html_element_sum_row(
			char *element_name,
			double sum,
			int percent_of_asset,
			int percent_of_revenue,
			LIST *statement_prior_year_list )
{
	HTML_ROW *html_row;

	if ( !element_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: element_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	html_row = html_row_new();

	html_cell_data_set(
		html_row->cell_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			/* ----------------------------- */
			/* Returns static memory or null */
			/* ----------------------------- */
			statement_element_name( element_name ) ),
		1 /* large_boolean */,
		1 /* bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_account_balance_string(
			sum /* balance */,
			(char *)0 /* account_action_string */,
			0 /* not round_dollar_boolean */ ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_account_percent_string(
			percent_of_asset,
			percent_of_revenue ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	if ( list_length( statement_prior_year_list ) )
	{
		html_cell_data_list_set(
			html_row->cell_list,
			statement_prior_year_element_data_list(
				element_name,
				statement_prior_year_list ) );
	}

	return html_row;
}

HTML_ROW *statement_subclass_aggr_html_row(
			SUBCLASSIFICATION *subclassification,
			LIST *statement_prior_year_list )
{
	HTML_ROW *html_row;

	if ( !subclassification )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: subclassification is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}


	html_row = html_row_new();

	html_cell_data_set(
		html_row->cell_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			subclassification->subclassification_name ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_account_balance_string(
			subclassification->sum /* balance */,
			(char *)0 /* account_action_string */,
			0 /* not round_dollar_boolean */ ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_account_percent_string(
			subclassification->percent_of_asset,
			subclassification->percent_of_revenue ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	if ( list_length( statement_prior_year_list ) )
	{
		html_cell_data_list_set(
			html_row->cell_list,
			statement_prior_year_subclassification_data_list(
				subclassification->subclassification_name,
				statement_prior_year_list ) );
	}

	return html_row;
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

	statement_subclass_aggr_html_list->heading_list =
		statement_subclass_aggr_html_list_heading_list(
			statement_prior_year_list );

	statement_subclass_aggr_html_list->list = list_new();

	do {
		element = list_get( element_statement_list );

		if ( element->sum )
		{
			list_set(
				statement_subclass_aggr_html_list->list,
				statement_subclass_aggr_html_new(
					element,
					statement_prior_year_list ) );
		}

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
			    sizeof( STATEMENT_SUBCLASS_AGGR_HTML_LIST ) ) ) )
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

LIST *statement_subclass_aggr_html_list_heading_list(
			LIST *statement_prior_year_list )
{
	LIST *heading_list = list_new();

	list_set(
		heading_list,
		html_heading_new(
			"",
			0 /* right_justify_boolean */ ) );

	list_set(
		heading_list,
		html_heading_new(
			"Balance",
			1 /* right_Justified_boolean */ ) );

	list_set(
		heading_list,
		html_heading_new(
			"Element",
			1 /* right_justified_boolean */ ) );

	list_set(
		heading_list,
		html_heading_new(
			"Standardized",
			1 /* right_justified_boolean */ ) );

	html_heading_list_right_justify_set(
		heading_list,
		statement_prior_year_heading_list(
			statement_prior_year_list ) );

	return heading_list;
}

LIST *statement_subclass_aggr_html_list_extract_row_list(
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

	statement_subclass_display_html_list->heading_list =
		statement_subclass_display_html_list_heading_list(
			statement_prior_year_list );

	statement_subclass_display_html_list->list = list_new();

	do {
		element = list_get( element_statement_list );

		if ( element->sum )
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
			   sizeof( STATEMENT_SUBCLASS_DISPLAY_HTML_LIST ) ) ) )
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

LIST *statement_subclass_display_html_list_heading_list(
			LIST *statement_prior_year_list )
{
	LIST *heading_list = list_new();

	list_set(
		heading_list,
		html_heading_new(
			"",
			0 /* right_justify_boolean */ ) );

	list_set(
		heading_list,
		html_heading_new(
			"Account",
			1 /* right_Justified_boolean */ ) );

	list_set(
		heading_list,
		html_heading_new(
			"Subclassification",
			1 /* right_justified_boolean */ ) );

	list_set(
		heading_list,
		html_heading_new(
			"Element",
			1 /* right_justified_boolean */ ) );

	list_set(
		heading_list,
		html_heading_new(
			"Standardized",
			1 /* right_justified_boolean */ ) );;

	html_heading_list_right_justify_set(
		heading_list,
		statement_prior_year_heading_list(
			statement_prior_year_list ) );

	return heading_list;
}

LIST *statement_subclass_display_html_list_extract_row_list(
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
		return (STATEMENT_SUBCLASS_DISPLAY_HTML *)0;

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

		if ( !subclassification->sum ) continue;

		list_set(
		   statement_subclass_display_html->row_list,
		   statement_subclass_display_html_subclassification_label_row(
			/* -------------------------------------- */
			/* Returns subclassification_name or null */
			/* -------------------------------------- */
			statement_subclass_display_name(
				element->element_name,
				subclassification->subclassification_name ),
			list_length( statement_prior_year_list ) ) );

		list_set_list(
			statement_subclass_display_html->row_list,
			statement_subclass_display_html_account_row_list(
				subclassification->account_statement_list,
				statement_prior_year_list ) );

		list_set(
		     statement_subclass_display_html->row_list,
		     statement_subclass_display_html_subclassification_sum_row(
			subclassification->subclassification_name,
			subclassification->sum,
			subclassification->percent_of_asset,
			subclassification->percent_of_revenue,
			statement_prior_year_list ) );

	} while ( list_next( element->subclassification_statement_list ) );

	list_set(
		statement_subclass_display_html->row_list,
		statement_subclass_display_html_element_sum_row(
			element->element_name,
			element->sum,
			element->percent_of_asset,
			element->percent_of_revenue,
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
			calloc( 1,
				sizeof( STATEMENT_SUBCLASS_DISPLAY_HTML ) ) ) ) 
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
	HTML_ROW *html_row;
	int i;

	if ( !element_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: element_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	html_row = html_row_new();

	html_cell_data_set(
		html_row->cell_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label( element_name ),
		1 /* large_boolean */,
		1 /* bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	for (	i = 0;
		i < statement_prior_year_list_length;
		i++ )
	{
		html_cell_data_set(
			html_row->cell_list,
			"",
			0 /* not large_boolean */,
			0 /* not bold_boolean */ );
	}

	return html_row;
}

HTML_ROW *statement_subclass_display_html_subclassification_label_row(
			char *subclassification_name,
			int statement_prior_year_list_length )
{
	HTML_ROW *html_row;
	int i;

	if ( !subclassification_name ) return (HTML_ROW *)0;

	html_row = html_row_new();

	html_cell_data_set(
		html_row->cell_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label( subclassification_name ),
		0 /* not large_boolean */,
		1 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	for (	i = 0;
		i < statement_prior_year_list_length;
		i++ )
	{
		html_cell_data_set(
			html_row->cell_list,
			"",
			0 /* not large_boolean */,
			0 /* not bold_boolean */ );
	}

	return html_row;
}

HTML_ROW *statement_subclass_display_html_subclassification_sum_row(
			char *subclassification_name,
			double sum,
			int percent_of_asset,
			int percent_of_revenue,
			LIST *statement_prior_year_list )
{
	HTML_ROW *html_row;

	if ( !subclassification_name )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: subclassification_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	html_row = html_row_new();

	html_cell_data_set(
		html_row->cell_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label( subclassification_name ),
		0 /* not large_boolean */,
		1 /* bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_account_balance_string(
			sum /* balance */,
			(char *)0 /* account_action_string */,
			0 /* not round_dollar_boolean */ ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_account_percent_string(
			percent_of_asset,
			percent_of_revenue ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	if ( list_length( statement_prior_year_list ) )
	{
		html_cell_data_list_set(
			html_row->cell_list,
			statement_prior_year_subclassification_data_list(
				subclassification_name,
				statement_prior_year_list ) );
	}

	return html_row;
}

HTML_ROW *statement_subclass_display_html_element_sum_row(
			char *element_name,
			double sum,
			int percent_of_asset,
			int percent_of_revenue,
			LIST *statement_prior_year_list )
{
	HTML_ROW *html_row;

	if ( !element_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: element_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	html_row = html_row_new();

	html_cell_data_set(
		html_row->cell_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			/* ----------------------------- */
			/* Returns static memory or null */
			/* ----------------------------- */
			statement_element_name( element_name ) ),
		1 /* large_boolean */,
		1 /* bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_account_balance_string(
			sum /* balance */,
			(char *)0 /* account_action_string */,
			0 /* not round_dollar_boolean */ ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_account_percent_string(
			percent_of_asset,
			percent_of_revenue ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	if ( list_length( statement_prior_year_list ) )
	{
		html_cell_data_list_set(
			html_row->cell_list,
			statement_prior_year_element_data_list(
				element_name,
				statement_prior_year_list ) );
	}

	return html_row;
}

HTML_ROW *statement_subclass_display_html_account_row(
			STATEMENT_ACCOUNT *statement_account,
			LIST *statement_prior_year_list )
{
	HTML_ROW *html_row = html_row_new();

	if ( !statement_account
	||   !statement_account->account )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: statement_account is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}


	html_cell_data_set(
		html_row->cell_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			statement_account->
				account->
				account_name ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		statement_account->balance_string,
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		statement_account->percent_string,
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	if ( list_length( statement_prior_year_list ) )
	{
		html_cell_data_list_set(
			html_row->cell_list,
			statement_prior_year_account_data_list(
				statement_account->
					account->
					account_name,
				statement_prior_year_list ) );
	}

	return html_row;
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
			statement_account_new(
				(char *)0 /* transaction_as_of_date */,
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
			LIST *statement_prior_year_list )
{
	STATEMENT_SUBCLASS_AGGR_LATEX *statement_subclass_aggr_latex;
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
		return (STATEMENT_SUBCLASS_AGGR_LATEX *)0;

	statement_subclass_aggr_latex = statement_subclass_aggr_latex_calloc();

	statement_subclass_aggr_latex->row_list = list_new();

	list_set(
		statement_subclass_aggr_latex->row_list,
		statement_subclass_aggr_latex_element_label_row(
			element->element_name,
			list_length( statement_prior_year_list )
				/* statement_prior_year_list_length */ ) );

	do {
		subclassification =
			list_get(
				element->subclassification_statement_list );

		if ( subclassification->sum )
		{
			list_set(
				statement_subclass_aggr_latex->row_list,
				statement_subclass_aggr_latex_row(
					subclassification,
					statement_prior_year_list ) );
		}

	} while ( list_next( element->subclassification_statement_list ) );

	list_set(
		statement_subclass_aggr_latex->row_list,
		statement_subclass_aggr_latex_element_sum_row(
			element->element_name,
			element->sum,
			element->percent_of_asset,
			element->percent_of_revenue,
			statement_prior_year_list ) );

	return statement_subclass_aggr_latex;
}

STATEMENT_SUBCLASS_AGGR_LATEX *
	statement_subclass_aggr_latex_calloc(
			void )
{
	STATEMENT_SUBCLASS_AGGR_LATEX *statement_subclass_aggr_latex;

	if ( ! ( statement_subclass_aggr_latex =
			calloc( 1, sizeof( STATEMENT_SUBCLASS_AGGR_LATEX ) ) ) )
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
			int statement_prior_year_list_length )
{
	LATEX_ROW *latex_row;
	int i;

	if ( !element_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: element_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	latex_row = latex_row_new();

	latex_column_data_set(
		latex_row->column_data_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label( element_name ),
		1 /* large_boolean */,
		1 /* bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	for (	i = 0;
		i < statement_prior_year_list_length;
		i++ )
	{
		latex_column_data_set(
			latex_row->column_data_list,
			"",
			0 /* not large_boolean */,
			0 /* not bold_boolean */ );
	}

	return latex_row;
}

LATEX_ROW *statement_subclass_aggr_latex_element_sum_row(
			char *element_name,
			double sum,
			int percent_of_asset,
			int percent_of_revenue,
			LIST *statement_prior_year_list )
{
	LATEX_ROW *latex_row;

	if ( !element_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: element_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	latex_row = latex_row_new();

	latex_column_data_set(
		latex_row->column_data_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			/* ----------------------------- */
			/* Returns static memory or null */
			/* ----------------------------- */
			statement_element_name( element_name ) ),
		1 /* not large_boolean */,
		1 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_account_balance_string(
			sum /* balance */,
			(char *)0 /* account_action_string */,
			0 /* not round_dollar_boolean */ ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_account_percent_string(
			percent_of_asset,
			percent_of_revenue ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	if ( list_length( statement_prior_year_list ) )
	{
		latex_column_data_list_set(
			latex_row->column_data_list,
			statement_prior_year_element_data_list(
				element_name,
				statement_prior_year_list ) );
	}

	return latex_row;
}

LATEX_ROW *statement_subclass_aggr_latex_row(
			SUBCLASSIFICATION *subclassification,
			LIST *statement_prior_year_list )
{
	LATEX_ROW *latex_row;

	if ( !subclassification )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: subclassification is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}


	latex_row = latex_row_new();

	latex_column_data_set(
		latex_row->column_data_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label(
			subclassification->subclassification_name ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		statement_account_balance_string(
			subclassification->sum,
			(char *)0 /* account_action_string */,
			0 /* not round_dollar_boolean */ ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		statement_account_percent_string(
			subclassification->percent_of_asset,
			subclassification->percent_of_revenue ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	if ( list_length( statement_prior_year_list ) )
	{
		latex_column_data_list_set(
			latex_row->column_data_list,
			statement_prior_year_subclassification_data_list(
				subclassification->subclassification_name,
				statement_prior_year_list ) );
	}

	return latex_row;
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
		return (STATEMENT_SUBCLASS_AGGR_LATEX_LIST *)0;

	statement_subclass_aggr_latex_list =
		statement_subclass_aggr_latex_list_calloc();

	statement_subclass_aggr_latex_list->heading_list =
		statement_subclass_aggr_latex_list_heading_list(
			statement_prior_year_list );

	statement_subclass_aggr_latex_list->list = list_new();

	do {
		element = list_get( element_statement_list );

		if ( element->sum )
		{
			list_set(
				statement_subclass_aggr_latex_list->list,
				statement_subclass_aggr_latex_new(
					element,
					statement_prior_year_list ) );
		}

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
			sizeof( STATEMENT_SUBCLASS_AGGR_LATEX_LIST ) ) ) )
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

LIST *statement_subclass_aggr_latex_list_heading_list(
			LIST *statement_prior_year_list )
{
	LIST *heading_list = list_new();

	list_set(
		heading_list,
		latex_table_heading_new(
			"",
			0 /* right_justify_flag */,
			(char *)0 /* paragraph_size */ ) );

	list_set(
		heading_list,
		latex_table_heading_new(
			"Balance",
			1 /* right_Justified_flag */,
			(char *)0 /* paragraph_size */ ) );

	list_set(
		heading_list,
		latex_table_heading_new(
			"Element",
			1 /* right_justified_flag */,
			(char *)0 /* paragraph_size */ ) );

	list_set(
		heading_list,
		latex_table_heading_new(
			"Standardized",
			1 /* right_justified_flag */,
			(char *)0 /* paragraph_size */ ) );

	list_set_list(
		heading_list,
		latex_table_right_heading_list(
			statement_prior_year_heading_list(
				statement_prior_year_list ) ) );

	return heading_list;
}

LIST *statement_subclass_aggr_latex_list_extract_row_list(
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

char *statement_subclass_display_name(
			char *element_name,
			char *subclassification_name )
{
	if ( !element_name )
		return subclassification_name;
	else
	if ( !subclassification_name )
		return (char *)0;
	else
	if ( strcmp( subclassification_name, element_name ) == 0 )
		return (char *)0;
	else
		return subclassification_name;
}

STATEMENT_CAPTION *statement_caption_calloc( void )
{
	STATEMENT_CAPTION *statement_caption;

	if ( ! ( statement_caption =
			calloc( 1, sizeof( STATEMENT_CAPTION ) ) ) )
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
			char *begin_date_string,
			char *end_date_string )
{
	STATEMENT_CAPTION *statement_caption;

	if ( !application_name
	||   !process_name
	||   !begin_date_string
	||   !end_date_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	statement_caption = statement_caption_calloc();

	statement_caption->begin_date_string = begin_date_string;
	statement_caption->end_date_string = end_date_string;

	statement_caption->logo_filename =
		statement_caption_logo_filename(
			application_name,
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
	
	statement_caption->subtitle =
		/* ----------------------------- */
		/* Returns static memory or null */
		/* ----------------------------- */
		statement_caption_subtitle(
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			statement_date_american(
				begin_date_string ),
			statement_date_american(
				end_date_string  ) );
	
	if ( !statement_caption->subtitle )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: statement_caption_subtitle() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	
	statement_caption->string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_caption_string(
			statement_caption->title,
			statement_caption->subtitle );
	
	statement_caption->date_time_string =
		/* ------------------------ */
		/* Returns date_time_string */
		/* ------------------------ */
		statement_caption_date_time_string(
			date_get_now_yyyy_mm_dd_hh_mm(
				date_utc_offset()
					/* date_time_string */ ) );
	
	statement_caption->frame_title =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		statement_caption_frame_title(
			process_name,
			statement_caption->date_time_string );

	return statement_caption;
}
