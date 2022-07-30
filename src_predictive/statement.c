/* ----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/statement.c				*/
/* ----------------------------------------------------------------	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
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
	if ( strcmp(
		output_medium_string,
		"table" ) == 0 )
	{
		return output_table;
	}
	else
	if ( strcmp(
		output_medium_string,
		"PDF" ) == 0 )
	{
		return output_PDF;
	}
	else
	if ( strcmp(
		output_medium_string,
		"spreadsheet" ) == 0 )
	{
		return output_spreadsheet;
	}
	else
	if ( strcmp(
		output_medium_string,
		"stdout" ) == 0 )
	{
		return output_stdout;
	}
	else
	{
		return output_table;
	}
}

char *statement_logo_filename(
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

char *statement_subtitle(
			char *begin_date_string,
			char *as_of_date )
{
	static char subtitle[ 128 ];

	if ( !begin_date_string || !as_of_date ) return (char *)0;

	sprintf(subtitle,
		"Beginning: %s, Ending: %s",
		begin_date_string,
	 	as_of_date );

	return subtitle;
}

STATEMENT *statement_fetch(
			char *application_name,
			char *process_name,
			LIST *filter_element_name_list,
			char *transaction_begin_date_string,
			char *transaction_as_of_date,
			char *transaction_date_time_closing )
{
	STATEMENT *statement;

	if ( !list_length( filter_element_name_list )
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
			filter_element_name_list,
			transaction_date_time_closing,
			1 /* fetch_subclassification */,
			1 /* fetch_account_list */,
			1 /* fetch_journal_latest */,
			1 /* fetch_memo */ );

	if ( !list_length( statement->element_statement_list ) )
	{
		free( statement );
		return (STATEMENT *)0;
	}

	statement->logo_filename =
		statement_logo_filename(
			application_name,
			STATEMENT_LOGO_FILENAME_KEY );

	statement->title =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		statement_title(
			application_name,
			(statement->logo_filename)
				? 1 : 0 /* exists_logo_filename */,
			process_name );

	statement->subtitle =
		/* ----------------------------- */
		/* Returns static memory or null */
		/* ----------------------------- */
		statement_subtitle(
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			statement_date_american(
				transaction_begin_date_string )
					/* begin_date_string */,
			statement_date_american(
				transaction_as_of_date )
					/* as_of_date */ );

	if ( !statement->subtitle )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: statement_subtitle() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	statement->caption =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_caption(
			statement->title,
			statement->subtitle );

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
			account_element_account_seek(
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
			prior_account->delta_prior,
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

char *statement_caption(
			char *title,
			char *subtitle )
{
	char caption[ 256 ];

	if ( !title || !subtitle )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(caption,
		"%s %s",
		title,
		subtitle );

	return strdup( caption );
}

char *statement_title(	char *application_name,
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
				statement_prior_year->date_string ) );

	} while ( list_next( statement_prior_year_list ) );

	return heading_list;
}

void statement_html_output(
			HTML_TABLE *html_table,
			char *title )
{
	if ( !html_table
	||   !title )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	printf( "<h1>%s</h1>\n", title );

	html_table_output( html_table );
}

void statement_latex_output(
			LATEX *latex,
			char *ftp_output_filename,
			char *prompt,
			char *process_name,
			char *date_time_string )
{
	if ( !latex
	||   !ftp_output_filename
	||   !prompt
	||   !process_name
	||   !date_time_string )
	{
		if ( latex ) fclose( latex->output_stream );

		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
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
		date_time_string /* footline */ );

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
			LIST *filter_element_name_list,
			char *transaction_date_time_closing,
			int prior_year_count,
			STATEMENT *statement )
{
	LIST *prior_year_list;
	int years_ago;

	if ( !list_length( filter_element_name_list )
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
				filter_element_name_list,
				transaction_date_time_closing,
				years_ago,
				statement ) );
	}

	return prior_year_list;
}

STATEMENT_PRIOR_YEAR *statement_prior_year_fetch(
			LIST *filter_element_name_list,
			char *transaction_date_time_closing,
			int years_ago,
			STATEMENT *statement )
{
	STATEMENT_PRIOR_YEAR *statement_prior_year;
	ELEMENT *current_element;
	ELEMENT *prior_element;

	if ( !list_length( filter_element_name_list )
	||   !transaction_date_time_closing
	||   !years_ago
	||   !statement
	||   !list_rewind( statement->element_statement_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	statement_prior_year = statement_prior_year_calloc();

	statement_prior_year->date_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_prior_year_date_string(
			transaction_date_time_closing,
			years_ago );

	statement_prior_year->element_statement_list =
		element_statement_list(
			filter_element_name_list,
			statement_prior_year->date_string,
			1 /* fetch_subclassification_list */,
			1 /* fetch_account_list */,
			1 /* fetch_journal_latest */,
			0 /* not fetch_transaction */ );

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
			element_prior_year_set(
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

char *statement_prior_year_date_string(
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

