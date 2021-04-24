/* ----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/statement.c				*/
/* ----------------------------------------------------------------	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "timlib.h"
#include "environ.h"
#include "application.h"
#include "date_convert.h"
#include "predictive.h"
#include "piece.h"
#include "column.h"
#include "date.h"
#include "sql.h"
#include "list.h"
#include "boolean.h"
#include "statement.h"

STATEMENT *statement_new(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *process_name,
			boolean exists_logo_filename,
			LIST *filter_element_name_list,
			char *as_of_date,
			int prior_year_count,
			LIST *fund_name_list,
			char *subclassification_option_string,
			char *fund_aggregation_string,
			char *output_medium_string )
{
	STATEMENT *statement;

	if ( ! ( statement =
			calloc( 1, sizeof( STATEMENT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	statement->application_name = application_name;
	statement->session = session;
	statement->login_name = login_name;
	statement->role_name = role_name;
	statement->process_name = process_name;
	statement->exists_logo_filename = exists_logo_filename;
	statement->filter_element_name_list = filter_element_name_list;
	statement->as_of_date = as_of_date;
	statement->prior_year_count = prior_year_count;
	statement->fund_name_list = fund_name_list;

	statement->subclassification_option_string =
		subclassification_option_string;

	statement->fund_aggregation_string = fund_aggregation_string;
	statement->output_medium_string = output_medium_string;

	return statement;
}

enum subclassification_option statement_subclassification_option(
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

enum fund_aggregation statement_fund_aggregation(
			LIST *fund_name_list,
			char *fund_aggregation_string )
{
	char *fund_name;

	if ( !list_length( fund_name_list ) )
	{
		return no_fund;
	}

	fund_name = list_first( fund_name_list );

	if ( strcmp( fund_name, "fund" ) == 0 )
	{
		return no_fund;
	}

	if ( list_length( fund_name_list ) == 1 )
	{
		return single_fund;
	}

	if ( strcmp(
		fund_aggregation_string,
		"sequential" ) == 0 )
	{
		return sequential;
	}
	else
	if ( strcmp(
		fund_aggregation_string,
		"consolidated" ) == 0 )
	{
		return consolidated;
	}
	else
	{
		return sequential;
	}
}

enum output_medium statement_output_medium(
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
		"stdout" ) == 0 )
	{
		return output_stdout;
	}
	else
	{
		return output_table;
	}
}

LIST *statement_fund_element_list(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			LIST *filter_element_name_list,
			char *begin_date_string,
			char *transaction_date_time,
			char *fund_name,
			enum subclassification_option subclassification_option )
{
	boolean fetch_subclassification_list = 0;
	boolean fetch_account_list = 0;
	char transaction_date[ 16 ];
	LIST *list;

	if ( subclassification_option == subclassification_display )
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

	list =
		element_list(
			filter_element_name_list,
			fund_name,
			transaction_date_time,
			fetch_subclassification_list,
			fetch_account_list );

	element_list_set_account_action_string(
		list,
		application_name,
		session,
		login_name,
		role_name,
		begin_date_string,
		column(
			transaction_date,
			0,
			transaction_date_time ) );

	return list;
}

STATEMENT *statement_steady_state(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *process_name,
			boolean exists_logo_filename,
			LIST *filter_element_name_list,
			char *as_of_date,
			int prior_year_count,
			LIST *fund_name_list,
			char *subclassification_option_string,
			char *fund_aggregation_string,
			char *output_medium_string,
			STATEMENT *statement )
{
	if ( !*as_of_date
	||   strcmp( as_of_date, "as_of_date" ) == 0 )
	{
		statement->as_of_date =
			/* -------------------- */
			/* Returns heap memory. */
			/* -------------------- */
			transaction_date_max();
	}

	statement->statement_subclassification_option =
		statement_subclassification_option(
			subclassification_option_string );

	statement->statement_fund_aggregation =
		statement_fund_aggregation(
			fund_name_list,
			fund_aggregation_string );

	/* Set all the funds if nothing is selected */
	/* ---------------------------------------- */
	if ( statement->statement_fund_aggregation == no_fund )
	{
		statement->fund_name_list =
			statement_fund_name_list();

		statement->statement_fund_aggregation =
			statement_fund_aggregation(
				statement->fund_name_list,
				fund_aggregation_string );
	}

	statement->statement_output_medium =
		statement_output_medium(
			output_medium_string );

	statement->begin_date_string =
		statement_begin_date_string(
			statement->as_of_date );

	statement->title =
		statement_title(
			application_name,
			exists_logo_filename,
			process_name );

	statement->subtitle =
		statement_subtitle(
			statement->begin_date_string,
			statement->as_of_date,
			statement->statement_fund_aggregation );

	statement->statement_fund_list =
		statement_fund_list(
			application_name,
			session,
			login_name,
			role_name,
			filter_element_name_list,
			statement->begin_date_string,
			statement->as_of_date,
			prior_year_count,
			statement->fund_name_list,
			statement->statement_subclassification_option,
			statement->statement_fund_aggregation );

	statement_fund_list_steady_state(
		statement->statement_fund_list );

	return statement;
}

STATEMENT_PRIOR_YEAR *statement_prior_year_new(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			LIST *filter_element_name_list,
			char *prior_date_string,
			char *fund_name,
			enum subclassification_option subclassification_option )
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

	statement_prior_year->application_name = application_name;
	statement_prior_year->session = session;
	statement_prior_year->login_name = login_name;
	statement_prior_year->role_name = role_name;

	statement_prior_year->filter_element_name_list =
		filter_element_name_list;

	statement_prior_year->prior_date_string = prior_date_string;
	statement_prior_year->fund_name = fund_name;

	statement_prior_year->subclassification_option =
		subclassification_option;

	return statement_prior_year;
}

STATEMENT_PRIOR_YEAR *statement_prior_year_fetch(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			LIST *filter_element_name_list,
			char *as_of_date,
			int years_ago,
			char *fund_name,
			enum subclassification_option subclassification_option )
{
	STATEMENT_PRIOR_YEAR *statement_prior_year;
	DATE *prior_date;

	prior_date =
		date_yyyy_mm_dd_new(
			as_of_date );

	date_subtract_year( prior_date, years_ago );

	statement_prior_year =
		statement_prior_year_new(
			application_name,
			session,
			login_name,
			role_name,
			filter_element_name_list,
			date_yyyy_mm_dd_display( prior_date )
				/* prior_date_string */,
			fund_name,
			subclassification_option );

	statement_prior_year->prior_year_element_list =
		statement_fund_element_list(
			application_name,
			session,
			login_name,
			role_name,
			filter_element_name_list,
			(char *)0 /* begin_date_string */,
			strdup( 
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				transaction_date_time_closing(
					statement_prior_year->
						prior_date_string,
					0 /* not preclose_time_boolean */,
					transaction_closing_entry_exists(
						statement_prior_year->
							prior_date_string ) ) ),
			fund_name,
			subclassification_option );

	return statement_prior_year;
}

STATEMENT_FUND *statement_fund_new(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			LIST *filter_element_name_list,
			char *begin_date_string,
			char *as_of_date,
			int prior_year_count,
			char *fund_name,
			enum subclassification_option subclassification_option )
{
	STATEMENT_FUND *statement_fund;

	if ( ! ( statement_fund =
			calloc( 1, sizeof( STATEMENT_FUND ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	statement_fund->application_name = application_name;
	statement_fund->session = session;
	statement_fund->login_name = login_name;
	statement_fund->role_name = role_name;
	statement_fund->filter_element_name_list = filter_element_name_list;
	statement_fund->begin_date_string = begin_date_string;
	statement_fund->as_of_date = as_of_date;
	statement_fund->fund_name = fund_name;
	statement_fund->prior_year_count = prior_year_count;
	statement_fund->subclassification_option = subclassification_option;

	return statement_fund;
}

STATEMENT_FUND *statement_fund_fetch(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			LIST *filter_element_name_list,
			char *begin_date_string,
			char *as_of_date,
			int prior_year_count,
			char *fund_name,
			enum subclassification_option subclassification_option )
{
	STATEMENT_FUND *statement_fund;

	statement_fund =
		statement_fund_new(
			application_name,
			session,
			login_name,
			role_name,
			filter_element_name_list,
			begin_date_string,
			as_of_date,
			prior_year_count,
			fund_name,
			subclassification_option );

	statement_fund->preclose_element_list =
		statement_fund_element_list(
			application_name,
			session,
			login_name,
			role_name,
			filter_element_name_list,
			begin_date_string,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			transaction_date_time_closing(
				as_of_date,
				1 /* preclose_time_boolean */,
				transaction_closing_entry_exists(
					as_of_date ) ),
			fund_name,
			subclassification_option );

	if ( transaction_closing_entry_exists( as_of_date ) )
	{
		statement_fund->postclose_element_list =
			statement_fund_element_list(
				application_name,
				session,
				login_name,
				role_name,
				filter_element_name_list,
				begin_date_string,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				transaction_date_time_closing(
					as_of_date,
					0 /* not preclose_time_boolean */,
					1 /* exists_postclose */ ),
				fund_name,
				subclassification_option );
	}

	return statement_fund;
}

LIST *statement_fund_list(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			LIST *filter_element_name_list,
			char *begin_date_string,
			char *as_of_date,
			int prior_year_count,
			LIST *fund_name_list,
			enum subclassification_option subclassification_option,
			enum fund_aggregation fund_aggregation )
{
	LIST *statement_fund_list = list_new();

	if ( fund_aggregation == no_fund )
	{
		list_set(
			statement_fund_list,
			statement_fund_fetch(
				application_name,
				session,
				login_name,
				role_name,
				filter_element_name_list,
				begin_date_string,
				as_of_date,
				prior_year_count,
				(char *)0 /* fund_name */,
				subclassification_option ) );
	}
	else
	if ( fund_aggregation == single_fund )
	{
		list_set(
			statement_fund_list,
			statement_fund_fetch(
				application_name,
				session,
				login_name,
				role_name,
				filter_element_name_list,
				begin_date_string,
				as_of_date,
				prior_year_count,
				list_first( fund_name_list ) /* fund_name */,
				subclassification_option ) );
	}
	else
	if ( fund_aggregation == consolidated )
	{
		list_set(
			statement_fund_list,
			statement_fund_fetch(
				application_name,
				session,
				login_name,
				role_name,
				filter_element_name_list,
				begin_date_string,
				as_of_date,
				prior_year_count,
				(char *)0 /* fund_name */,
				subclassification_option ) );
	}
	else
	if ( fund_aggregation == sequential )
	{
		char *fund_name;

		if ( !list_rewind( fund_name_list ) )
		{
			fprintf(stderr,
			"Warning in %s/%s()/%d: fund_name_list is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			return (LIST *)0;
		}

		do {
			fund_name = list_get( fund_name_list );

			list_set(
				statement_fund_list,
				statement_fund_fetch(
					application_name,
					session,
					login_name,
					role_name,
					filter_element_name_list,
					begin_date_string,
					as_of_date,
					prior_year_count,
					fund_name,
					subclassification_option ) );

		} while ( list_next( fund_name_list ) );
	}

	return statement_fund_list;
}

char *statement_begin_date_string(
			char *as_of_date )
{
	char sys_string[ 1024 ];
	char *select;
	char *results;
	char transaction_date_string[ 16 ];
	DATE *prior_closing_transaction_date = {0};

	prior_closing_transaction_date =
		transaction_prior_closing_transaction_date(
			as_of_date );
	
	if ( prior_closing_transaction_date )
	{
		date_increment_days(
			prior_closing_transaction_date,
			1.0 );
	
		return date_get_yyyy_mm_dd_string(
				prior_closing_transaction_date );
	}

	/* No closing entries */
	/* ------------------ */
	select = "min( transaction_date_time )";

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=%s		",
		 environment_application_name(),
		 select,
		 JOURNAL_TABLE );

	results = pipe2string( sys_string );

	if ( !results || !*results ) return (char *)0;

	return strdup( column( transaction_date_string, 0, results )  );
}

LIST *statement_fund_name_list( void )
{
	char *sys_string;

	if ( !predictive_fund_attribute_exists() )
	{
		return (LIST *)0;
	}

	sys_string = "select.sh fund fund where fund";

	return pipe2list( sys_string );
}

char *statement_title(
			char *application_name,
			boolean exists_logo_filename,
			char *process_name )
{
	static char title[ 256 ];
	char buffer[ 128 ];

	if ( !exists_logo_filename )
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
			char *as_of_date,
			enum fund_aggregation fund_aggregation )
{
	static char subtitle[ 128 ];

	char begin_date_american[ 32 ];
	char end_date_american[ 32 ];

	*begin_date_american = '\0';

	date_convert_source_international(
		begin_date_american,
		american,
		begin_date_string );

	date_convert_source_international(
		end_date_american,
		american,
		as_of_date );

	if ( fund_aggregation != consolidated )
	{
		sprintf(subtitle,
	 		"Beginning: %s, Ending: %s",
			begin_date_american,
	 		end_date_american );
	}
	else
	{
		sprintf(subtitle,
	 		"Consolidated Funds, Beginning: %s, Ending: %s",
			begin_date_american,
	 		end_date_american );
	}

	return subtitle;
}

STATEMENT_FUND *statement_fund_steady_state(
			LIST *preclose_element_list,
			LIST *postclose_element_list,
			int prior_year_count,
			STATEMENT_FUND *statement_fund )
{
	if ( list_length( postclose_element_list ) )
	{
		element_list_set_total(
			postclose_element_list );

		element_list_set_percent_of_assets(
			postclose_element_list );

		element_list_set_percent_of_revenues(
			postclose_element_list );
	}

	element_list_set_total(
		preclose_element_list );

	element_list_set_percent_of_assets(
		preclose_element_list );

	element_list_set_percent_of_revenues(
		preclose_element_list );

	if ( prior_year_count > 0 )
	{
		STATEMENT_PRIOR_YEAR *statement_prior_year;
		int years_ago;

		statement_fund->prior_year_list = list_new();

		for(	years_ago = 1;
			years_ago <= prior_year_count;
			years_ago++ )
		{
			list_set(
				statement_fund->prior_year_list,
				statement_prior_year_fetch(
					statement_fund->application_name,
					statement_fund->session,
					statement_fund->login_name,
					statement_fund->role_name,
					statement_fund->
						filter_element_name_list,
					statement_fund->as_of_date,
					years_ago,
					statement_fund->fund_name,
					statement_fund->
						subclassification_option ) );
		}

		if ( list_rewind( statement_fund->prior_year_list ) )
		{
			do {
				statement_prior_year =
					list_get(
						statement_fund->
							prior_year_list );

				element_list_set_delta_prior(
					statement_prior_year->
						prior_year_element_list,
					preclose_element_list );

			} while ( list_next( statement_fund->
						prior_year_list ) );
		}
	} /* if prior_year_count > 0 */

	statement_fund->preclose_debit_total =
		element_list_debit_total(
			preclose_element_list );

	statement_fund->preclose_credit_total =
		element_list_credit_total(
			preclose_element_list );

	if ( list_length( postclose_element_list ) )
	{
		statement_fund->postclose_debit_total =
			element_list_debit_total(
				postclose_element_list );

		statement_fund->postclose_credit_total =
			element_list_credit_total(
				postclose_element_list );
	}

	return statement_fund;
}

void statement_fund_list_steady_state(
			LIST *statement_fund_list )
{
	STATEMENT_FUND *statement_fund;

	if ( !list_rewind( statement_fund_list ) ) return;

	do {
		statement_fund = list_get( statement_fund_list );

		statement_fund =
			statement_fund_steady_state(
				statement_fund->preclose_element_list,
				statement_fund->postclose_element_list,
				statement_fund->prior_year_count,
				statement_fund );

	} while ( list_next( statement_fund_list ) );
}

boolean statement_fund_exists_postclose(
			LIST *statement_fund_list )
{
	STATEMENT_FUND *statement_fund;

	if ( !list_length( statement_fund_list ) ) return 0;

	statement_fund = list_first( statement_fund_list );

	return (boolean)list_length( statement_fund->postclose_element_list );
}

LIST *statement_prior_year_heading_list(
			LIST *prior_year_list )
{
	LIST *heading_list;
	STATEMENT_PRIOR_YEAR *prior_year;

	if ( !list_rewind( prior_year_list ) ) return (LIST *)0;

	heading_list = list_new();

	do {
		prior_year = list_get( prior_year_list );

		list_set(
			heading_list,
			prior_year->prior_date_string );

	} while ( list_next( prior_year_list ) );

	return heading_list;
}

LIST *statement_prior_year_delta_list(
			char *account_name,
			LIST *prior_year_list )
{
	LIST *delta_list;
	STATEMENT_PRIOR_YEAR *prior_year;
	ACCOUNT *account;
	char delta_prior[ 16 ];

	if ( !list_rewind( prior_year_list ) ) return (LIST *)0;

	delta_list = list_new();

	do {
		prior_year = list_get( prior_year_list );

		if ( ( account =
			element_list_account_seek(
				account_name,
				prior_year->prior_year_element_list ) ) )
		{
			sprintf(delta_prior,
				"%d%c",
				account->delta_prior,
				'%' );

			list_set(
				delta_list,
				strdup( delta_prior ) );
		}
		else
		{
			list_set( delta_list, strdup( "" ) );
		}

	} while ( list_next( prior_year_list ) );

	return delta_list;
}

