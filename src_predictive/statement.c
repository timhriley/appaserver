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
#include "account.h"
#include "subclassification.h"
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

	list =
		element_list(
			filter_element_name_list,
			fund_name,
			transaction_date_time,
			fetch_subclassification_list,
			fetch_account_list );

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
			char *as_of_date,
			int years_ago,
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

	statement_prior_year->as_of_date = as_of_date;
	statement_prior_year->years_ago = years_ago;
	statement_prior_year->fund_name = fund_name;

	statement_prior_year->subclassification_option =
		subclassification_option;

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
		element_list_balance_total(
			postclose_element_list );

		element_list_percent_of_asset_set(
			postclose_element_list );

		element_list_percent_of_revenue_set(
			postclose_element_list );
	}

	element_list_balance_total(
		preclose_element_list );

	element_list_percent_of_asset_set(
		preclose_element_list );

	element_list_percent_of_revenue_set(
		preclose_element_list );

	if ( prior_year_count > 0 )
	{
		statement_fund->prior_year_list =
			statement_fund_steady_state_prior_year_list(
				prior_year_count,
				preclose_element_list,
				statement_fund );
	}

	statement_fund->preclose_debit_total =
		element_debit_total(
			preclose_element_list );

	statement_fund->preclose_credit_total =
		element_credit_total(
			preclose_element_list );

	if ( list_length( postclose_element_list ) )
	{
		statement_fund->postclose_debit_total =
			element_debit_total(
				postclose_element_list );

		statement_fund->postclose_credit_total =
			element_credit_total(
				postclose_element_list );
	}

	return statement_fund;
}

LIST *statement_fund_steady_state_prior_year_list(
			int prior_year_count,
			LIST *preclose_element_list,
			STATEMENT_FUND *statement_fund )
{
	STATEMENT_PRIOR_YEAR *statement_prior_year;
	LIST *prior_year_list;
	DATE *prior_date;
	int years_ago;

	if ( prior_year_count < 1 ) return (LIST *)0;

	prior_year_list = list_new();

	for(	years_ago = 1;
		years_ago <= prior_year_count;
		years_ago++ )
	{
		list_set(
			prior_year_list,
			statement_prior_year_new(
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

	list_rewind( prior_year_list );

	prior_date =
		date_yyyy_mm_dd_new(
			statement_fund->as_of_date );

	do {
		statement_prior_year = list_get( prior_year_list );

		date_subtract_year( prior_date, 1 );

		statement_prior_year->prior_date_string =
			strdup(
				date_yyyy_mm_dd_display(
					prior_date ) );

		statement_prior_year->begin_date_string =
			statement_begin_date_string(
				statement_prior_year->
					prior_date_string );

		statement_prior_year->prior_year_element_list =
			statement_fund_element_list(
				statement_prior_year->
					application_name,
				statement_prior_year->
					session,
				statement_prior_year->
					login_name,
				statement_prior_year->
					role_name,
				statement_prior_year->
				       filter_element_name_list,
				statement_prior_year->
					begin_date_string,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				transaction_date_time_closing(
					statement_prior_year->
						prior_date_string,
					1 /* preclose_time_boolean */,
					transaction_closing_entry_exists(
						statement_prior_year->
							prior_date_string ) ),
				statement_prior_year->
					fund_name,
				statement_prior_year->
				     subclassification_option );

		element_list_balance_total(
			statement_prior_year->
				prior_year_element_list );

		element_list_delta_prior_set(
			statement_prior_year->
				prior_year_element_list,
			preclose_element_list );

	} while ( list_next( prior_year_list ) );

	return prior_year_list;
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

LIST *statement_account_delta_list(
			char *account_name,
			LIST *prior_year_list )
{
	LIST *delta_list;
	STATEMENT_PRIOR_YEAR *prior_year;
	ACCOUNT *account;
	char delta_prior[ 1024 ];

	if ( !list_rewind( prior_year_list ) ) return (LIST *)0;

	delta_list = list_new();

	do {
		prior_year = list_get( prior_year_list );

		if ( ( account =
			element_account_seek(
				account_name,
				prior_year->prior_year_element_list ) ) )
		{
			if ( account->account_total )
			{
				sprintf(delta_prior,
					"^%d%c %s",
					account->delta_prior,
					'%',
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					timlib_place_commas_in_money(
						account->account_total ) );
			}
			else
			{
				*delta_prior = '\0';
			}

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

LIST *statement_html_account_delta_list(
			char *account_name,
			LIST *prior_year_list )
{
	LIST *delta_list;
	STATEMENT_PRIOR_YEAR *prior_year;
	ACCOUNT *account;
	char delta_prior[ 1024 ];

	if ( !list_rewind( prior_year_list ) ) return (LIST *)0;

	delta_list = list_new();

	do {
		prior_year = list_get( prior_year_list );

		if ( ( account =
			element_account_seek(
				account_name,
				prior_year->prior_year_element_list ) ) )
		{
			if ( account->account_total )
			{
				sprintf(delta_prior,
					"%d%c<br><a href=\"%s\">%s</a>",
					account->delta_prior,
					'%',
			 		account->account_action_string,
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					timlib_place_commas_in_money(
						account->account_total ) );
			}
			else
			{
				*delta_prior = '\0';
			}

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

LIST *statement_html_element_delta_list(
			char *element_name,
			LIST *prior_year_list )
{
	LIST *delta_list;
	STATEMENT_PRIOR_YEAR *prior_year;
	ELEMENT *element;
	char delta_prior[ 1024 ];

	if ( !list_rewind( prior_year_list ) ) return (LIST *)0;

	delta_list = list_new();

	do {
		prior_year = list_get( prior_year_list );

		if ( ( element =
			element_seek(
				element_name,
				prior_year->prior_year_element_list ) ) )
		{
			if ( element->element_balance_total )
			{
				sprintf(delta_prior,
					"%d%c<br>%s",
					element->element_delta_prior,
					'%',
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					timlib_place_commas_in_money(
					   element->
						element_balance_total ) );
			}
			else
			{
				*delta_prior = '\0';
			}

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

LIST *statement_html_subclassification_delta_list(
			char *subclassification_name,
			LIST *prior_year_list )
{
	LIST *delta_list;
	STATEMENT_PRIOR_YEAR *prior_year;
	SUBCLASSIFICATION *subclassification;
	char delta_prior[ 1024 ];

	if ( !list_rewind( prior_year_list ) ) return (LIST *)0;

	delta_list = list_new();

	do {
		prior_year = list_get( prior_year_list );

		if ( ( subclassification =
			element_subclassification_seek(
				subclassification_name,
				prior_year->prior_year_element_list ) ) )
		{
			if ( subclassification->subclassification_total )
			{
				sprintf(delta_prior,
					"%d%c<br>%s",
					subclassification->delta_prior,
					'%',
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					timlib_place_commas_in_money(
					   subclassification->
						subclassification_total ) );
			}
			else
			{
				*delta_prior = '\0';
			}

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

LIST *statement_PDF_account_delta_list(
			char *account_name,
			LIST *prior_year_list )
{
	LIST *delta_list;
	STATEMENT_PRIOR_YEAR *prior_year;
	ACCOUNT *account;
	char delta_prior[ 1024 ];

	if ( !list_rewind( prior_year_list ) ) return (LIST *)0;

	delta_list = list_new();

	do {
		prior_year = list_get( prior_year_list );

		if ( ( account =
			element_account_seek(
				account_name,
				prior_year->prior_year_element_list ) ) )
		{
			if ( account->account_total )
			{
				sprintf(delta_prior,
					"%d%c %s",
					account->delta_prior,
					'%',
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					timlib_place_commas_in_dollars(
						account->account_total ) );
			}
			else
			{
				*delta_prior = '\0';
			}

#ifdef NOT_DEFINED
			sprintf(delta_prior,
			"\\vtop{\\hbox{\\strut %d%c}\\hbox{\\strut %s}}",
				account->delta_prior,
				'%',
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				timlib_place_commas_in_dollars(
					account->account_total ) );
#endif

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

LIST *statement_PDF_subclassification_delta_list(
			char *subclassification_name,
			LIST *prior_year_list )
{
	LIST *delta_list;
	STATEMENT_PRIOR_YEAR *prior_year;
	SUBCLASSIFICATION *subclassification;
	char delta_prior[ 1024 ];

	if ( !list_rewind( prior_year_list ) ) return (LIST *)0;

	delta_list = list_new();

	do {
		prior_year = list_get( prior_year_list );

		if ( ( subclassification =
			element_subclassification_seek(
				subclassification_name,
				prior_year->prior_year_element_list ) ) )
		{
			if ( subclassification->subclassification_total )
			{
				sprintf(delta_prior,
					"%d%c %s",
					subclassification->delta_prior,
					'%',
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					timlib_place_commas_in_dollars(
					    subclassification->
						subclassification_total ) );
			}
			else
			{
				*delta_prior = '\0';
			}

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

LIST *statement_PDF_element_delta_list(
			char *element_name,
			LIST *prior_year_list )
{
	LIST *delta_list;
	STATEMENT_PRIOR_YEAR *prior_year;
	ELEMENT *element;
	char delta_prior[ 1024 ];

	if ( !list_rewind( prior_year_list ) ) return (LIST *)0;

	delta_list = list_new();

	do {
		prior_year = list_get( prior_year_list );

		if ( ( element =
			element_seek(
				element_name,
				prior_year->prior_year_element_list ) ) )
		{
			if ( element->element_balance_total )
			{
				sprintf(delta_prior,
					"%d%c %s",
					element->element_delta_prior,
					'%',
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					timlib_place_commas_in_dollars(
					    element->
						element_balance_total ) );
			}
			else
			{
				*delta_prior = '\0';
			}

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

boolean statement_fund_exists_prior_year(
			LIST *statement_fund_list )
{
	STATEMENT_FUND *statement_fund;

	if ( !list_length( statement_fund_list ) ) return 0;

	statement_fund = list_first( statement_fund_list );

	return (boolean)list_length( statement_fund->prior_year_list );
}

STATEMENT_FUND *statement_fund_seek(
			char *fund_name,
			LIST *statement_fund_list )
{
	STATEMENT_FUND *statement_fund;

	if ( !list_rewind( statement_fund_list ) ) return (STATEMENT_FUND *)0;

	do {
		statement_fund = list_get( statement_fund_list );

		/* If no fund then return the first one. */
		/* ------------------------------------- */
		if ( !fund_name
		||   !*fund_name
		||   strcmp( fund_name, "fund" ) == 0 )
		{
			return statement_fund;
		}

		if ( string_strcmp(	statement_fund->fund_name,
					fund_name ) == 0 )
		{
			return statement_fund;
		}

	} while ( list_next( statement_fund_list ) );

	return (STATEMENT_FUND *)0;
}

double statement_fund_net_income(
			double revenue_total,
			double expense_total,
			double gain_total,
			double loss_total )
{
	return (	revenue_total -
			expense_total +
			gain_total -
			loss_total );
}

void statement_fund_list_net_income_set(
			LIST *statement_fund_list )
{
	STATEMENT_FUND *statement_fund;
	ELEMENT *element;

	if ( !list_rewind( statement_fund_list ) ) return;

	do {
		statement_fund = list_get( statement_fund_list );

		if ( ( element =
			element_seek(
				ELEMENT_REVENUE,
				statement_fund->preclose_element_list ) ) )
		{
			statement_fund->revenue_total =
				element->element_balance_total;
		}

		if ( ( element =
			element_seek(
				ELEMENT_EXPENSE,
				statement_fund->preclose_element_list ) ) )
		{
			statement_fund->expense_total =
				element->element_balance_total;
		}

		if ( ( element =
			element_seek(
				ELEMENT_GAIN,
				statement_fund->preclose_element_list ) ) )
		{
			statement_fund->gain_total =
				element->element_balance_total;
		}

		if ( ( element =
			element_seek(
				ELEMENT_LOSS,
				statement_fund->preclose_element_list ) ) )
		{
			statement_fund->loss_total =
				element->element_balance_total;
		}

		statement_fund->net_income =
			statement_fund_net_income(
				statement_fund->revenue_total,
				statement_fund->expense_total,
				statement_fund->gain_total,
				statement_fund->loss_total );

		statement_fund->net_income_percent =
			statement_fund_net_income_percent(
				statement_fund->net_income,
				statement_fund->revenue_total );

	} while ( list_next( statement_fund_list ) );
}

int statement_fund_net_income_percent(
			double net_income,
			double revenue_total )
{
	double percent_of_revenue;

	percent_of_revenue =
		( net_income /
  		revenue_total ) * 100.0;

	return float_round_int( percent_of_revenue );
}

void statement_html_display_subclassification(
			LIST *statement_fund_list,
			char *title,
			char *subtitle,
			boolean is_statement_of_activities )
{
	STATEMENT_FUND *statement_fund;

	if ( !list_rewind( statement_fund_list ) ) return;

	do {
		statement_fund = list_get( statement_fund_list );

		statement_html_display_subclassification_fund(
			statement_fund,
			title,
			subtitle,
			is_statement_of_activities );

	} while ( list_next( statement_fund_list ) );
}

void statement_html_display_subclassification_fund(
			STATEMENT_FUND *statement_fund,
			char *title,
			char *subtitle,
			boolean is_statement_of_activities )
{
	HTML_TABLE *html_table;

	html_table =
		html_table_new(
			title,
			subtitle,
			statement_fund->fund_name );

	html_table->number_left_justified_columns = 1;
	html_table->number_right_justified_columns = 99;

	html_table_heading(
		html_table->title,
		html_table->sub_title,
		html_table->sub_sub_title );

	html_table_output_data_heading(
		statement_html_heading_list(
			statement_fund->prior_year_list,
			1 /* include_account */,
			1 /* include_subclassification */ ),
		html_table->number_left_justified_columns,
		html_table->number_right_justified_columns,
		(LIST *)0 /* justify_list */ );

	statement_html_display_subclassification_element_list(
		html_table,
		statement_fund->preclose_element_list,
		statement_fund->prior_year_list );

	if ( statement_fund->net_income )
	{
		statement_html_display_subclassification_net_income(
			html_table,
			statement_fund->net_income,
			statement_fund->net_income_percent,
			is_statement_of_activities );
	}

	html_table_close();
}

void statement_html_display_subclassification_element_list(
			HTML_TABLE *html_table,
			LIST *preclose_element_list,
			LIST *prior_year_list )
{
	ELEMENT *element;

	if ( !list_rewind( preclose_element_list ) ) return;

	do {
		element = list_get( preclose_element_list );

		if ( !element->element_balance_total ) continue;

		statement_html_display_subclassification_element(
			html_table,
			element,
			prior_year_list );

	} while ( list_next( preclose_element_list ) );
}

void statement_html_display_subclassification_element(
			HTML_TABLE *html_table,
			ELEMENT *element,
			LIST *prior_year_list )
{
	SUBCLASSIFICATION *subclassification;
	ACCOUNT *account;
	char buffer[ 128 ];
	char format_buffer[ 128 ];
	char element_title[ 128 ];

	if ( !html_table )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty html_table.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( element->subclassification_list ) ) return;

	/* Set the element as centered */
	/* --------------------------- */
	sprintf(element_title,
		"<h2>%s</h2>",
		format_initial_capital(
			format_buffer,
			element->element_name ) );

	html_table_set_data(
		html_table->data_list,
		strdup( element_title ) );

	html_table_output_data(
		html_table->data_list,
		html_table->
			number_left_justified_columns,
		html_table->
			number_right_justified_columns,
		html_table->background_shaded,
		html_table->justify_list );

	list_free_string_list( html_table->data_list );
	html_table->data_list = list_new();

	do {
		subclassification = list_get( element->subclassification_list );

		if ( !list_length( subclassification->account_list ) )
			continue;

		/* ------------------------------------ */
		/* If more than one account,		*/
		/* then set the subclassification name.	*/
		/* ------------------------------------ */
		if ( list_length( subclassification->account_list ) > 1 )
		{
			sprintf(buffer,
				"<h3>%s</h3>",
				format_initial_capital(
					format_buffer,
					subclassification->
						subclassification_name ) );

			html_table_set_data(
				html_table->data_list,
				strdup( buffer ) );
	
			/* Output the subclassification name */
			/* --------------------------------- */
			html_table_output_data(
				html_table->data_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );

			list_free_string_list( html_table->data_list );
			html_table->data_list = list_new();
		}

		list_rewind( subclassification->account_list );

		do {
			account =
				list_get(
					subclassification->account_list );

			if ( !account->account_total ) continue;

			html_table_set_data(
				html_table->data_list,
				strdup(
					format_initial_capital(
						format_buffer,
						account->account_name ) ) );
	
			html_table_set_data(
				html_table->data_list,
				strdup(
					place_commas_in_money(
						account->account_total ) ) );

			/* Skip the subclassification column */
			/* --------------------------------- */
			html_table_set_data(
				html_table->data_list,
				strdup( "" ) );

			/* Skip the element column */
			/* ----------------------- */
			html_table_set_data(
				html_table->data_list,
				strdup( "" ) );

			/* Set the account percent of revenue */
			/* ---------------------------------- */
			sprintf( buffer,
				 "%d%c",
				 account->percent_of_revenue,
				 '%' );

			html_table_set_data(
				html_table->data_list,
				strdup( buffer ) );

			if ( list_length( prior_year_list ) )
			{
				list_append_list(
					html_table->data_list,
					statement_html_account_delta_list(
						account->account_name,
						prior_year_list ) );
			}

			html_table_output_data(
				html_table->data_list,
				html_table->
					number_left_justified_columns,
				html_table->
					number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );
	
			list_free_string_list( html_table->data_list );
			html_table->data_list = list_new();

		} while( list_next( subclassification->account_list ) );

		/* Set the subclassification label */
		/* ------------------------------- */
		html_table_set_data(
			html_table->data_list,
			strdup(
				statement_html_subclassification_label(
					subclassification->
						subclassification_name ) ) );

		/* Skip the account column */
		/* ----------------------- */
		html_table_set_data( html_table->data_list, strdup( "" ) );

		/* Set the subclassification total */
		/* ------------------------------- */
		html_table_set_data(
			html_table->data_list,
			strdup(
				place_commas_in_money(
					subclassification->
						subclassification_total ) ) );

		/* Skip the element column */
		/* ----------------------- */
		html_table_set_data(
			html_table->data_list,
			strdup( "" ) );

		sprintf( buffer,
			 "%d%c",
			 subclassification->percent_of_revenue,
			 '%' );

		/* Set the subclassification percent of revenue */
		/* -------------------------------------------- */
		html_table_set_data(
			html_table->data_list,
			strdup( buffer ) );

		if ( list_length( prior_year_list ) )
		{
			list_append_list(
				html_table->data_list,
				statement_html_subclassification_delta_list(
					subclassification->
						subclassification_name,
					prior_year_list ) );
		}

		/* Output the row */
		/* -------------- */
		html_table_output_data(
			html_table->data_list,
			html_table->
				number_left_justified_columns,
			html_table->
				number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );

		list_free_string_list( html_table->data_list );
		html_table->data_list = list_new();

	} while( list_next( element->subclassification_list ) );

	sprintf(element_title,
		"<h2>Total %s</h2>",
		format_initial_capital(
			format_buffer,
			element->element_name ) );

	/* Set the element total title */
	/* --------------------------- */
	html_table_set_data(
		html_table->data_list,
		strdup( element_title ) );
	
	/* Skip the account column */
	/* ----------------------- */
	html_table_set_data( html_table->data_list, strdup( "" ) );

	/* Skip the subclassification column */
	/* --------------------------------- */
	html_table_set_data( html_table->data_list, strdup( "" ) );

	/* Set the element total column */
	/* ---------------------------- */
	html_table_set_data(
		html_table->data_list,
		strdup(
			place_commas_in_money(
				element->element_balance_total ) ) );

	sprintf( buffer,
		 "%d%c",
		 element->percent_of_revenue,
		 '%' );

	/* Set the percent of revenue column */
	/* --------------------------------- */
	html_table_set_data(
		html_table->data_list,
		strdup( buffer ) );

	if ( list_length( prior_year_list ) )
	{
		list_append_list(
			html_table->data_list,
			statement_html_element_delta_list(
				element->element_name,
				prior_year_list ) );
	}

	/* Output the element row */
	/* ---------------------- */
	html_table_output_data(
		html_table->data_list,
		html_table->
			number_left_justified_columns,
		html_table->
			number_right_justified_columns,
		html_table->background_shaded,
		html_table->justify_list );

	list_free_string_list( html_table->data_list );
	html_table->data_list = list_new();
}

void statement_html_display_subclassification_net_income(
			HTML_TABLE *html_table,
			double net_income,
			int net_income_percent,
			boolean is_statement_of_activities )
{
	char buffer[ 128 ];

	html_table_set_data(
		html_table->data_list,
		strdup(
			statement_html_net_income_label(
				is_statement_of_activities ) ) );

	/* Skip the account */
	/* ---------------- */
	html_table_set_data( html_table->data_list, strdup( "" ) );

	/* Skip the subclassification */
	/* -------------------------- */
	html_table_set_data( html_table->data_list, strdup( "" ) );

	/* Set the net income */
	/* ------------------ */
	html_table_set_data(
		html_table->data_list,
		strdup(
			place_commas_in_money(
				net_income ) ) );

	sprintf( buffer,
		 "%d%c",
		 net_income_percent,
		 '%' );

	/* Set the percent of revenue */
	/* --------------------------- */
	html_table_set_data(
		html_table->data_list,
		strdup( buffer ) );

	/* Output the net income row */
	/* ------------------------- */
	html_table_output_data(
		html_table->data_list,
		html_table->
			number_left_justified_columns,
		html_table->
			number_right_justified_columns,
		html_table->background_shaded,
		html_table->justify_list );

	list_free_string_list( html_table->data_list );
	html_table->data_list = list_new();
}

LIST *statement_html_heading_list(
			LIST *prior_year_list,
			boolean include_account,
			boolean include_subclassification )
{
	LIST *heading_list = list_new();

	list_set( heading_list, "" );

	if ( include_account )
	{
		list_set( heading_list, "Account" );
	}

	if ( include_subclassification )
	{
		list_set( heading_list, "Subclassification" );
	}

	list_set( heading_list, "Element" );
	list_set( heading_list, "Percent" );

	if ( list_length( prior_year_list ) )
	{
		list_append_list(
			heading_list,
			statement_prior_year_heading_list(
				prior_year_list ) );
	}

	return heading_list;
}

char *statement_html_net_income_label(
			boolean is_statement_of_activities )
{
	if ( is_statement_of_activities )
	{
		return "<h2>Change in Net Assets</h2>";
	}
	else
	{
		return "<h2>Net Income</h2>";
	}
}

char *statement_html_subclassification_label(
			char *subclassification_name )
{
	static char subclassification_label[ 256 ];
	char format_buffer[ 128 ];

	if ( strcmp(	subclassification_name,
			ACCOUNT_CHANGE_IN_NET_ASSETS ) == 0 )
	{
		strcpy( subclassification_label,
			"<h3>Change in Net Assets</h3>" );
	}
	else
	{
		sprintf(subclassification_label,
			"<h3>Total %s</h3>",
			format_initial_capital(
				format_buffer,
				subclassification_name ) );
	}

	return subclassification_label;
}

void statement_html_omit_subclassification(
			LIST *statement_fund_list,
			char *title,
			char *subtitle,
			boolean is_statement_of_activities )
{
	STATEMENT_FUND *statement_fund;

	if ( !list_rewind( statement_fund_list ) ) return;

	do {
		statement_fund = list_get( statement_fund_list );

		statement_html_omit_subclassification_fund(
			statement_fund,
			title,
			subtitle,
			is_statement_of_activities );

	} while ( list_next( statement_fund_list ) );
}

void statement_html_omit_subclassification_fund(
			STATEMENT_FUND *statement_fund,
			char *title,
			char *subtitle,
			boolean is_statement_of_activities )
{
	HTML_TABLE *html_table;

	html_table =
		html_table_new(
			title,
			subtitle,
			statement_fund->fund_name );

	html_table->number_left_justified_columns = 1;
	html_table->number_right_justified_columns = 99;

	html_table_heading(
		html_table->title,
		html_table->sub_title,
		html_table->sub_sub_title );

	html_table_output_data_heading(
		statement_html_heading_list(
			statement_fund->prior_year_list,
			1 /* include_account */,
			0 /* not include_subclassification */ ),
		html_table->number_left_justified_columns,
		html_table->number_right_justified_columns,
		(LIST *)0 /* justify_list */ );

	statement_html_omit_subclassification_element_list(
		html_table,
		statement_fund->preclose_element_list,
		statement_fund->prior_year_list );

	if ( statement_fund->net_income )
	{
		statement_html_omit_subclassification_net_income(
			html_table,
			statement_fund->net_income,
			statement_fund->net_income_percent,
			is_statement_of_activities );
	}

	html_table_close();
}

void statement_html_omit_subclassification_element_list(
			HTML_TABLE *html_table,
			LIST *preclose_element_list,
			LIST *prior_year_list )
{
	ELEMENT *element;

	if ( !list_rewind( preclose_element_list ) ) return;

	do {
		element = list_get( preclose_element_list );

		if ( !element->element_balance_total ) continue;

		statement_html_omit_subclassification_element(
			html_table,
			element,
			prior_year_list );

	} while ( list_next( preclose_element_list ) );
}

void statement_html_omit_subclassification_element(
			HTML_TABLE *html_table,
			ELEMENT *element,
			LIST *prior_year_list )
{
	ACCOUNT *account;
	char buffer[ 128 ];
	char format_buffer[ 128 ];
	char element_title[ 128 ];

	if ( !html_table )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty html_table.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( element->account_list ) ) return;

	sprintf(element_title,
		"<h2>%s</h2>",
		format_initial_capital(
			format_buffer,
			element->element_name ) );

	/* Set the element title */
	/* --------------------- */
	html_table_set_data(
		html_table->data_list,
		strdup( element_title ) );

	/* Output the element title */
	/* ------------------------ */
	html_table_output_data(
		html_table->data_list,
		html_table->
			number_left_justified_columns,
		html_table->
			number_right_justified_columns,
		html_table->background_shaded,
		html_table->justify_list );

	list_free_string_list( html_table->data_list );
	html_table->data_list = list_new();

	do {
		account = list_get( element->account_list );

		if ( !account->account_total ) continue;

		/* Set the account name */
		/* -------------------- */
		html_table_set_data(
			html_table->data_list,
			strdup(
				format_initial_capital(
					format_buffer,
					account->account_name ) ) );
	
		/* Set the account total */
		/* --------------------- */
		html_table_set_data(
			html_table->data_list,
			strdup(
				place_commas_in_money(
					account->account_total ) ) );

		/* Skip the element column */
		/* ----------------------- */
		html_table_set_data( html_table->data_list, strdup( "" ) );

		sprintf( buffer,
			 "%d%c",
			 account->percent_of_revenue,
			 '%' );

		/* Set the account percent of revenue */
		/* ---------------------------------- */
		html_table_set_data(
			html_table->data_list,
			strdup( buffer ) );

		if ( list_length( prior_year_list ) )
		{
			list_append_list(
				html_table->data_list,
				statement_html_account_delta_list(
					account->account_name,
					prior_year_list ) );
		}

		/* Output the row */
		/* -------------- */
		html_table_output_data(
			html_table->data_list,
			html_table->
				number_left_justified_columns,
			html_table->
				number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );
	
		list_free_string_list( html_table->data_list );
		html_table->data_list = list_new();

	} while( list_next( element->account_list ) );

	sprintf(element_title,
		"<h2>Total %s</h2>",
		element->element_name );

	/* Set the element total title */
	/* --------------------------- */
	html_table_set_data(
		html_table->data_list,
		strdup( element_title ) );
	
	/* Skip the account column */
	/* ----------------------- */
	html_table_set_data( html_table->data_list, strdup( "" ) );

	/* Set the element total */
	/* --------------------- */
	html_table_set_data(
		html_table->data_list,
		strdup(
			place_commas_in_money(
				element->element_balance_total ) ) );

	sprintf( buffer,
		 "%d%c",
		 element->percent_of_revenue,
		 '%' );

	/* Set the percent of revenue */
	/* -------------------------- */
	html_table_set_data(
		html_table->data_list,
		strdup( buffer ) );

	if ( list_length( prior_year_list ) )
	{
		list_append_list(
			html_table->data_list,
			statement_html_element_delta_list(
				element->element_name,
				prior_year_list ) );
	}

	/* Output the element total row */
	/* ---------------------------- */
	html_table_output_data(
		html_table->data_list,
		html_table->
			number_left_justified_columns,
		html_table->
			number_right_justified_columns,
		html_table->background_shaded,
		html_table->justify_list );

	list_free_string_list( html_table->data_list );
	html_table->data_list = list_new();
}

void statement_html_omit_subclassification_net_income(
			HTML_TABLE *html_table,
			double net_income,
			int net_income_percent,
			boolean is_statement_of_activities )
{
	char buffer[ 128 ];

	/* Set the element as centered */
	/* --------------------------- */
	html_table_set_data(
		html_table->data_list,
		strdup(
			statement_html_net_income_label(
				is_statement_of_activities )  ) );

	/* Skip the account */
	/* ---------------- */
	html_table_set_data( html_table->data_list, strdup( "" ) );

	/* Set the net income */
	/* ------------------ */
	html_table_set_data(
		html_table->data_list,
		strdup(
			place_commas_in_money(
				net_income ) ) );

	sprintf( buffer,
		 "%d%c",
		 net_income_percent,
		 '%' );

	/* Percent of revenue */
	/* ------------------ */
	html_table_set_data(
		html_table->data_list,
		strdup( buffer ) );

	/* Output the net income row */
	/* ------------------------- */
	html_table_output_data(
		html_table->data_list,
		html_table->
			number_left_justified_columns,
		html_table->
			number_right_justified_columns,
		html_table->background_shaded,
		html_table->justify_list );

	list_free_string_list( html_table->data_list );
	html_table->data_list = list_new();
}

void statement_html_aggregate_subclassification(
			LIST *statement_fund_list,
			char *title,
			char *subtitle,
			boolean is_statement_of_activities )
{
	STATEMENT_FUND *statement_fund;

	if ( !list_rewind( statement_fund_list ) ) return;

	do {
		statement_fund = list_get( statement_fund_list );

		statement_html_aggregate_subclassification_fund(
			statement_fund,
			title,
			subtitle,
			is_statement_of_activities );

	} while ( list_next( statement_fund_list ) );
}

void statement_html_aggregate_subclassification_fund(
			STATEMENT_FUND *statement_fund,
			char *title,
			char *subtitle,
			boolean is_statement_of_activities )
{
	HTML_TABLE *html_table;

	html_table =
		html_table_new(
			title,
			subtitle,
			statement_fund->fund_name );

	html_table->number_left_justified_columns = 1;
	html_table->number_right_justified_columns = 99;

	html_table_heading(
		html_table->title,
		html_table->sub_title,
		html_table->sub_sub_title );

	html_table_output_data_heading(
		statement_html_heading_list(
			statement_fund->prior_year_list,
			0 /* not include_account */,
			1 /* include_subclassification */ ),
		html_table->number_left_justified_columns,
		html_table->number_right_justified_columns,
		(LIST *)0 /* justify_list */ );

	statement_html_aggregate_subclassification_element_list(
		html_table,
		statement_fund->preclose_element_list,
		statement_fund->prior_year_list );

	if ( statement_fund->net_income )
	{
		statement_html_aggregate_subclassification_net_income(
			html_table,
			statement_fund->net_income,
			statement_fund->net_income_percent,
			is_statement_of_activities );
	}

	html_table_close();
}

void statement_html_aggregate_subclassification_element_list(
			HTML_TABLE *html_table,
			LIST *preclose_element_list,
			LIST *prior_year_list )
{
	ELEMENT *element;

	if ( !list_rewind( preclose_element_list ) ) return;

	do {
		element = list_get( preclose_element_list );

		if ( !element->element_balance_total ) continue;

		statement_html_aggregate_subclassification_element(
			html_table,
			element,
			prior_year_list );

	} while ( list_next( preclose_element_list ) );
}

void statement_html_aggregate_subclassification_element(
			HTML_TABLE *html_table,
			ELEMENT *element,
			LIST *prior_year_list )
{
	SUBCLASSIFICATION *subclassification;
	char buffer[ 128 ];
	char format_buffer[ 128 ];
	char element_title[ 128 ];

	if ( !html_table )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty html_table.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( element->subclassification_list ) ) return;

	sprintf(element_title,
		"<h2>%s</h2>",
		format_initial_capital(
			format_buffer,
			element->element_name ) );

	/* Set the element title */
	/* --------------------- */
	html_table_set_data(
		html_table->data_list,
		strdup( element_title ) );

	/* Output the element title */
	/* ------------------------ */
	html_table_output_data(
		html_table->data_list,
		html_table->
			number_left_justified_columns,
		html_table->
			number_right_justified_columns,
		html_table->background_shaded,
		html_table->justify_list );

	list_free_string_list( html_table->data_list );
	html_table->data_list = list_new();

	do {
		subclassification = list_get( element->subclassification_list );

		if ( !subclassification->subclassification_total ) continue;

		/* Set the subclassification label */
		/* ------------------------------- */
		html_table_set_data(
			html_table->data_list,
			strdup(
				statement_html_subclassification_label(
					subclassification->
						subclassification_name ) ) );
	
		/* Set the subclassification total */
		/* ------------------------------- */
		html_table_set_data(
			html_table->data_list,
			strdup(
				place_commas_in_money(
					subclassification->
						subclassification_total ) ) );

		/* Skip the element column */
		/* ----------------------- */
		html_table_set_data( html_table->data_list, strdup( "" ) );

		sprintf( buffer,
			 "%d%c",
			 subclassification->percent_of_revenue,
			 '%' );

		/* Set the subclassification percent of revenue */
		/* -------------------------------------------- */
		html_table_set_data(
			html_table->data_list,
			strdup( buffer ) );

		if ( list_length( prior_year_list ) )
		{
			list_append_list(
				html_table->data_list,
				statement_html_subclassification_delta_list(
					subclassification->
						subclassification_name,
					prior_year_list ) );
		}

		/* Output the row */
		/* -------------- */
		html_table_output_data(
			html_table->data_list,
			html_table->
				number_left_justified_columns,
			html_table->
				number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );
	
		list_free_string_list( html_table->data_list );
		html_table->data_list = list_new();

	} while( list_next( element->subclassification_list ) );

	sprintf(element_title,
		"<h2>Total %s</h2>",
		element->element_name );

	/* Set the element total title */
	/* --------------------------- */
	html_table_set_data(
		html_table->data_list,
		strdup( element_title ) );
	
	/* Skip the subclassification column */
	/* --------------------------------- */
	html_table_set_data( html_table->data_list, strdup( "" ) );

	/* Set the element total */
	/* --------------------- */
	html_table_set_data(
		html_table->data_list,
		strdup(
			place_commas_in_money(
				element->element_balance_total ) ) );

	sprintf( buffer,
		 "%d%c",
		 element->percent_of_revenue,
		 '%' );

	/* Set the percent of revenue */
	/* -------------------------- */
	html_table_set_data(
		html_table->data_list,
		strdup( buffer ) );

	if ( list_length( prior_year_list ) )
	{
		list_append_list(
			html_table->data_list,
			statement_html_element_delta_list(
				element->element_name,
				prior_year_list ) );
	}

	/* Output the element total row */
	/* ---------------------------- */
	html_table_output_data(
		html_table->data_list,
		html_table->
			number_left_justified_columns,
		html_table->
			number_right_justified_columns,
		html_table->background_shaded,
		html_table->justify_list );

	list_free_string_list( html_table->data_list );
	html_table->data_list = list_new();
}

void statement_html_aggregate_subclassification_net_income(
			HTML_TABLE *html_table,
			double net_income,
			int net_income_percent,
			boolean is_statement_of_activities )
{
	char buffer[ 128 ];

	/* Set the element as centered */
	/* --------------------------- */
	html_table_set_data(
		html_table->data_list,
		strdup(
			statement_html_net_income_label(
				is_statement_of_activities )  ) );

	/* Skip the subclassification */
	/* -------------------------- */
	html_table_set_data( html_table->data_list, strdup( "" ) );

	/* Set the net income */
	/* ------------------ */
	html_table_set_data(
		html_table->data_list,
		strdup(
			place_commas_in_money(
				net_income ) ) );

	sprintf( buffer,
		 "%d%c",
		 net_income_percent,
		 '%' );

	/* Percent of revenue */
	/* ------------------ */
	html_table_set_data(
		html_table->data_list,
		strdup( buffer ) );

	/* Output the net income row */
	/* ------------------------- */
	html_table_output_data(
		html_table->data_list,
		html_table->
			number_left_justified_columns,
		html_table->
			number_right_justified_columns,
		html_table->background_shaded,
		html_table->justify_list );

	list_free_string_list( html_table->data_list );
	html_table->data_list = list_new();
}

