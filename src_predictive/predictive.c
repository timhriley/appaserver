/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/predictive.c				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
#include "piece.h"
#include "column.h"
#include "date.h"
#include "sql.h"
#include "list.h"
#include "environ.h"
#include "transaction.h"
#include "predictive.h"

char *predictive_transaction_date_time( char *transaction_date )
{
	char transaction_date_time[ 32 ];
	char *transaction_time;
	DATE *date;
	static char prior_transaction_time[ 32 ] = {0};

	if ( !transaction_date
	||   !*transaction_date
	||   strcmp( transaction_date, "transaction_date" ) == 0 )
	{
		transaction_date =
			 date_get_now_yyyy_mm_dd(
				date_get_utc_offset() );
	}

	if ( !*prior_transaction_time )
	{
		transaction_time =
			date_get_now_hh_colon_mm_colon_ss(
				date_get_utc_offset() );

		sprintf( transaction_date_time,
			 "%s %s",
			 transaction_date,
			 transaction_time );

		strcpy( prior_transaction_time, transaction_time );
	}
	else
	{
		sprintf( transaction_date_time,
			 "%s %s",
			 transaction_date,
			 prior_transaction_time );

		date = date_yyyy_mm_dd_hms_new(	transaction_date_time );
		date_increment_seconds( date, 1 );
		transaction_time = date_get_hms( date );

		sprintf( transaction_date_time,
			 "%s %s",
			 transaction_date,
			 transaction_time );

		strcpy( prior_transaction_time,
			transaction_time );

		date_free( date );
	}

	return strdup( transaction_date_time );
}

char *predictive_fund_where( char *fund_name )
{
	char where[ 128 ];

	if ( !fund_name
	||   !*fund_name
	||   strcmp( fund_name, "fund" ) == 0 )
	{
		strcpy( where, "1 = 1" );
	}
	else
	if ( predictive_fund_attribute_exists() )
	{
		sprintf(where,
		 	"fund = '%s'",
		 	fund_name );
	}
	else
	{
		strcpy( where, "1 = 1" );
	}

	return strdup( where );
}

boolean predictive_fund_attribute_exists( void )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
	 	"folder_attribute_exists.sh %s account fund",
	 	environment_application() );

	return ( system( sys_string ) == 0 );
}

