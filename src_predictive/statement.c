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
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			LIST *filter_element_name_list,
			char *transaction_begin_date_string,
			char *transaction_as_of_date,
			int prior_year_count,
			enum subclassification_option subclassification_option )
{
	STATEMENT *statement;

	if ( !application_name
	||   !session
	||   !login_name
	||   !role_name
	||   !list_length( filter_element_name_list )
	||   !statement_begin_date_string
	||   !transaction_as_of_date )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	statement = statement_calloc();

	statement->transaction_closing_entry_exists =
		transaction_closing_entry_exists(
			transaction_as_of_date );

	statement->transaction_date_time_nominal =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		transaction_date_time_closing(
			PREDICTIVE_CLOSE_TIME,
			PREDICTIVE_PRECLOSE_TIME,
			transaction_as_of_date /* transaction_date */,
			1 /* preclose_time_boolean */,
			statement->transaction_closing_entry_exists );

	statement->transaction_date_time_fixed =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		transaction_date_time_closing(
			PREDICTIVE_CLOSE_TIME,
			PREDICTIVE_PRECLOSE_TIME,
			transaction_as_of_date,
			0 /* not preclose_time_boolean */,
			statement->transaction_closing_entry_exists );

	statement->preclose_element_list =
		statement_element_list(
			application_name,
			session,
			login_name,
			role_name,
			filter_element_name_list,
			begin_date_string,
			statement_fund->transaction_date_time_nominal,
			statement_fund->transaction_date_time_nominal,
			subclassification_option );

	if ( with_postclose
	&&   transaction_closing_entry_exists( as_of_date ) )
	{
		statement_fund->postclose_element_list =
			statement_element_list(
				application_name,
				session,
				login_name,
				role_name,
				filter_element_name_list,
				begin_date_string,
				statement_fund->transaction_date_time_fixed,
				statement_fund->transaction_date_time_fixed,
				subclassification_option );
	}

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

LIST *statement_element_list(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			LIST *filter_element_name_list,
			char *begin_date_string,
			char *transaction_date_time_nominal,
			char *transaction_date_time_fixed,
			enum subclassification_option subclassification_option )
{
	boolean fetch_subclassification_list = 0;
	boolean fetch_account_list = 0;
	char transaction_date[ 16 ];
	LIST *element_list;
	ELEMENT *element;

	if ( subclassification_option == subclassification_display
	||   subclassification_option == subclassification_aggregate )
	{
		fetch_subclassification_list = 1;
	}
	else
	/* ------------------------------ */
	/* Must be subclassification_omit */
	/* ------------------------------ */
	{
		fetch_account_list = 1;
	}

	element_list =
		element_list(
			filter_element_name_list,
			transaction_date_time_nominal,
			transaction_date_time_fixed,
			fetch_subclassification_list,
			fetch_account_list );

	if ( ( element =
			element_seek(
				ELEMENT_EQUITY,
				element_list ) )
	&&     begin_date_string )
	{
		element->equity_element =
			equity_element_fetch(
				element->element_name,
				element->element_current_balance,
				begin_date_string );

		if ( !element->equity_element )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: equity_element_fetch() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}
	}

	element_list_account_action_string_set(
		list,
		application_name,
		session,
		login_name,
		role_name,
		begin_date_string,
		column(
			transaction_date,
			0,
			transaction_date_time_fixed ) );

	return element_list;
}

