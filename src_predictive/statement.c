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
#include "piece.h"
#include "date.h"
#include "sql.h"
#include "list.h"
#include "boolean.h"
#include "statement.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
STATEMENT *statement_new(
			char *as_of_date,
			int prior_year_count,
			char *fund_name,
			char *subclassification_option_string,
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

	statement->as_of_date = as_of_date;
	statement->prior_year_count = prior_year_count;
	statement->fund_name = fund_name;

	statement->subclassification_option_string =
		subclassification_option_string;

	statement->output_medium_string =
		output_medium_string;

	return statement;
}

enum subclassification_option statement_subclassification_option(
			char *subclassification_option_string )
{
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

LIST *statement_element_list(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *transaction_date_time,
			char *fund_name,
			enum subclassification_option subclassification_option,
			enum fund_aggregation fund_aggregation )
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
			(LIST *)0 /* filter_element_name_list */,
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
		beginning_date,
		transaction_date_time );

	return list;
}

STATEMENT *statement_steady_state(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *as_of_date,
			int prior_year_count,
			char *fund_name,
			char *subclassification_option_string,
			char *fund_aggregation_string,
			char *output_medium_string,
			STATEMENT *statement )
{
	statement->statement_subclassification_option =
		statement_subclassification_option(
			subclassification_option_string );

	statement->statement_fund_aggregation =
		statement_fund_aggregation(
			fund_aggregation_string );

	statement->statement_output_medium =
		statement_output_medium(
			output_medium_string );

	return statement;
}

STATEMENT_PRIOR_YEAR *statement_prior_year_new(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *as_of_date,
			int years_ago,
			char *fund_name,
			enum subclassification_option subclassification_option )
			enum fund_aggregation fund_aggregation )
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
	statement_prior_year->as_of_date = as_of_date;
	statement_prior_year->fund_name = fund_name;
	statement_prior_year->years_ago = years_ago;

	statement_prior_year->subclassification_option =
		subclassification_option;

	statement_prior_year->fund_aggregation =
		fund_aggregation;

	return statement_prior_year;
}

STATEMENT_PRIOR_YEAR *statement_prior_year_fetch(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *as_of_date,
			int years_ago,
			char *fund_name,
			enum subclassification_option subclassification_option,
			enum fund_aggregation fund_aggregation )
{
	STATEMENT_PRIOR_YEAR *statement_prior_year;
	DATE *current_date;

	statement_prior_year =
		statement_prior_year_new(
			application_name,
			session,
			login_name,
			role_name,
			as_of_date,
			years_ago,
			fund_name,
			subclassification_option,
			fund_aggregation );

	current_date =
		date_yyyy_mm_dd_new(
			as_of_date );

	date_subtract_year( current_date, years_ago );

	statement_prior_year->prior_year_element_list =
		statement_element_list(
			application_name,
			session,
			login_name,
			role_name,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			transaction_date_time_closing(
				date_yyyy_mm_dd_display(
					current_date ),
				0 /* not preclose_time_boolean */ ),
			fund_name,
			subclassification_option,
			fund_aggregation );

	return statement_prior_year;
}

STATEMENT_FUND *statement_fund_new(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *as_of_date,
			int prior_year_count,
			char *fund_name,
			enum subclassification_option subclassification_option )
			enum fund_aggregation fund_aggregation )
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
	statement_fund->as_of_date = as_of_date;
	statement_fund->fund_name = fund_name;
	statement_fund->years_ago = years_ago;
	statement_fund->subclassification_option = subclassification_option;
	statement_fund->fund_aggregation = fund_aggregation;

	return statement_fund;
}

STATEMENT_FUND *statement_fund_fetch(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *as_of_date,
			int prior_year_count,
			char *fund_name,
			LIST *fund_name_list,
			enum subclassification_option subclassification_option )
{
	STATEMENT_FUND *statement_fund;
	int years_ago;

	statement_fund =
		statement_fund_new(
			application_name,
			session,
			login_name,
			role_name,
			as_of_date,
			prior_year_count,
			fund_name,
			subclassification_option,
			fund_aggregation );

	statement_fund->begin_date_string =
		statement_begin_date_string(
			fund_name,
			as_of_date );

	statement_fund->preclose_element_list =
		statement_element_list(
			application_name,
			session,
			login_name,
			role_name,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			transaction_date_time_closing(
				as_of_date,
				1 /* preclose_time_boolean */ ),
			fund_name,
			subclassification_option );

	statement_fund->current_element_list =
		statement_element_list(
			application_name,
			session,
			login_name,
			role_name,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			transaction_date_time_closing(
				as_of_date,
				0 /* not preclose_time_boolean */ ),
			fund_name,
			subclassification_option );

	if ( !prior_year_count )
	{
		return statement_fund;
	}

	statement_fund->prior_year_list = list_new();

	for(	years_ago = 1;
		years_ago < prior_year_count;
		years_ago++ )
	{
		list_set(
			statement->prior_year_list,
			statement_prior_year_fetch(
				application_name,
				session,
				login_name,
				role_name,
				as_of_date,
				years_ago,
				fund_name,
				statement->
					statement_subclassification_option,
				statement->
					statement_fund_aggregation ) );
	}

	return statement_fund;
}

LIST *statement_fund_list(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *as_of_date,
			int prior_year_count,
			LIST *fund_name_list,
			enum subclassification_option,
			enum fund_aggregation )
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
				as_of_date,
				prior_year_count,
				(char *)0 /* fund_name */,
				(LIST *)0 /* fund_name_list */,
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
				as_of_date,
				prior_year_count,
				list_first( fund_name_list ) /* fund_name */,
				(LIST *)0 /* fund_name_list */,
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
				as_of_date,
				prior_year_count,
				(char *)0 /* fund_name */,
				fund_name_list,
				subclassification_option,
				fund_aggregation ) );
	}
	else
	if ( fund_aggregation == sequential )
	{
		STATEMENT_FUND *statement_fund;
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
					as_of_date,
					prior_year_count,
					fund_name,
					(LIST *)0 /* fund_name_list */,
					subclassification_option ) );

		} while ( list_next( fund_name_list ) );
	}

	return statement_fund_list;
}

char *statement_begin_date_string(
			char *fund_name,
			char *as_of_date )
{
	char where[ 512 ];
	char sys_string[ 1024 ];
	char *select;
	char folder[ 128 ];
	char *results;
	char transaction_date_string[ 16 ];
	DATE *prior_closing_transaction_date = {0};

	if ( fund_name && !*fund_name )
		fund_name = (char *)0;
	else
	if ( fund_name && strcmp( fund_name, "fund" ) == 0 )
		fund_name = (char *)0;

	/* Get the prior closing entry then return its tomorrow. */
	/* ----------------------------------------------------- */
	if ( as_of_date )
	{
		prior_closing_transaction_date =
			transaction_prior_closing_transaction_date(
				fund_name,
				as_of_date );
	
		if ( prior_closing_transaction_date )
		{
			date_increment_days(
				prior_closing_transaction_date,
				1.0 );
	
			return date_get_yyyy_mm_dd_string(
					prior_closing_transaction_date );
		}
	}

	/* No closing entries */
	/* ------------------ */
	select = "min( transaction_date_time )";

	if ( fund_name && *fund_name && strcmp( fund_name, "fund" ) != 0 )
	{
		/* Get the first entry for the fund. */
		/* --------------------------------- */
		sprintf(folder,
		 	"%s,%s",
		 	JOURNAL_TABLE,
		 	ACCOUNT_TABLE_NAME );

		sprintf(where,
		 	"fund = '%s' and				"
			"%s.account = %s.account			",
		 	fund_name,
			JOURNAL_TABLE,
			ACCOUNT_TABLE_NAME );
	}
	else
	{
		/* Get the first entry. */
		/* -------------------- */
		strcpy( folder, JOURNAL_TABLE );

		strcpy( where, "1 = 1" );
	}

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=%s		"
		 "			where=\"%s\"		",
		 environment_application_name(),
		 select,
		 folder,
		 where );

	results = pipe2string( sys_string );

	if ( !results || !*results ) return (char *)0;

	return strdup( column( transaction_date_string, 0, results )  );
}

