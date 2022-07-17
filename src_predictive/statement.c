/* ----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/statement.c				*/
/* ----------------------------------------------------------------	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include "timlib.h"
#include "application_constants.h"
#include "application.h"
#include "date_convert.h"
#include "predictive.h"
#include "transaction.h"
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

char *statement_begin_date_string(
			char *transaction_table,
			char *transaction_as_of_date )
{
	DATE *prior_closing_transaction_date = {0};

	prior_closing_transaction_date =
		transaction_prior_closing_transaction_date(
			PREDICTIVE_CLOSE_TIME,
			TRANSACTION_CLOSING_ENTRY_MEMO,
			transaction_table,
			transaction_as_of_date );
	
	if ( prior_closing_transaction_date )
	{
		date_increment_days(
			prior_closing_transaction_date,
			1.0 );
	
		return date_yyyy_mm_dd( prior_closing_transaction_date );
	}

	return
	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	transaction_minimum_transaction_date_string(
		transaction_table );
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

