/* ----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/trial_balance.c			*/
/* ----------------------------------------------------------------	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "String.h"
#include "float.h"
#include "transaction.h"
#include "statement.h"
#include "trial_balance.h"

TRIAL_BALANCE *trial_balance_calloc( void )
{
	TRIAL_BALANCE *trial_balance;

	if ( ! ( trial_balance = calloc( 1, sizeof( TRIAL_BALANCE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: () returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return trial_balance;
}

TRIAL_BALANCE *trial_balance_fetch(
			char *application_name,
			char *session_key,
			char *role_name,
			char *process_name,
			char *as_of_date,
			int prior_year_count,
			char *subclassifiction_option_string,
			char *output_medium_string )
{
	TRIAL_BALANCE *trial_balance;

	if ( !application_name
	||   !session_key
	||   !role_name
	||   !process_name
	||   !as_of_date
	||   !subclassifiction_option_string
	||   !output_medium_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	trial_balance = trial_balance_calloc();

	trial_balance->statement_subclassification_option =
		statement_resolve_subclassification_option(
			subclassifiction_option_string );

	trial_balance->statement_output_medium =
		statement_resolve_output_medium(
			output_medium_string );

	if ( ! ( trial_balance->transaction_as_of_date =
			transaction_as_of_date(
				as_of_date ) ) )
	{
		free( trial_balance );
		return (TRIAL_BALANCE *)0;
	}

	if ( ! ( trial_balance->statement_begin_date_string =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			statement_begin_date_string(
				TRANSACTION_TABLE,
				trial_balance->transaction_as_of_date ) ) )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: statement_begin_date_string(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			trial_balance->transaction_as_of_date );
		exit( 1 );
	}

	trial_balance->statement_logo_filename =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		statement_logo_filename(
			application_name,
			STATEMENT_LOGO_FILENAME_KEY );

	trial_balance->statement_title =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		statement_title(
			application_name,
			trial_balance->statement_logo_filename,
			process_name );

	trial_balance->statement_subtitle =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		statement_subtitle(
			trial_balance->statement_begin_date_string,
			trial_balance->transaction_as_of_date );

	return trial_balance;
}

