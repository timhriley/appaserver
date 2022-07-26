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
#include "account.h"
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

char *statement_title(	char *application_name,
			char *statement_logo_filename,
			char *process_name )
{
	static char title[ 256 ];
	char buffer[ 128 ];

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

	if ( !statement_logo_filename )
	{
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
		strcpy( title,
			format_initial_capital(
				buffer,
				process_name ) );
	}

	return title;
}

char *statement_subtitle(
			char *begin_date_string,
			char *as_of_date )
{
	static char subtitle[ 128 ];
	char begin_date_american[ 32 ];
	char end_date_american[ 32 ];

	if ( !begin_date_string
	||   !as_of_date )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	*begin_date_american = '\0';
	*end_date_american = '\0';

	date_convert_source_international(
		begin_date_american,
		american,
		begin_date_string );

	date_convert_source_international(
		end_date_american,
		american,
		as_of_date );

	sprintf(subtitle,
		"Beginning: %s, Ending: %s",
		begin_date_american,
	 	end_date_american );

	return subtitle;
}

STATEMENT *statement_fetch(
			LIST *filter_element_name_list,
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

LIST *statement_prior_year_latex_account_data_list(
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
			statement_prior_year_latex_account_data(
				prior_account ) );

	} while ( list_next( statement_prior_year_list ) );

	return data_list;
}

char *statement_prior_year_latex_account_data( ACCOUNT *prior_account )
{
	char account_data[ 32 ];

	if ( !prior_account
	||   !prior_account->journal_latest
	||   !prior_account->journal_latest->balance )
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
				prior_account->journal_latest->balance ) );
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

	statement_link->latex_filename =
		appaserver_link_get_tail_half(
			(char *)0 /* application_name */,
			statement_link->appaserver_link_file->filename_stem,
			statement_link->appaserver_link_file->begin_date_string,
			statement_link->appaserver_link_file->end_date_string,
			statement_link->appaserver_link_file->process_id,
			statement_link->appaserver_link_file->session,
			statement_link->appaserver_link_file->extension );

	statement_link->appaserver_link_file->extension = "dvi";

	statement_link->dvi_filename =
		appaserver_link_get_tail_half(
			(char *)0 /* application_name */,
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

