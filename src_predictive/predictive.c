/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/predictive.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit Appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "list.h"
#include "appaserver_error.h"
#include "predictive.h"

char *predictive_fund_where(
		const char *predictive_fund_table_name,
		const char *predictive_fund_column_name,
		char *fund_name )
{
	char where[ 128 ];

	if ( !fund_name
	||   !*fund_name
	||   strcmp(	fund_name,
			predictive_fund_table_name ) == 0
	||   strcmp(	fund_name,
			predictive_fund_column_name ) == 0
	||   !predictive_fund_boolean(
		predictive_fund_table_name,
		predictive_fund_column_name ) )
	{
		strcpy( where, "1 = 1" );
	}
	else
	{
		snprintf(
			where,
			sizeof ( where ),
		 	"%s = '%s'",
			predictive_fund_column_name,
		 	fund_name );
	}

	return strdup( where );
}

boolean predictive_fund_boolean(
		const char *predictive_fund_table_name,
		const char *predictive_fund_column_name )
{
	static boolean fund_boolean = -1;

	if ( fund_boolean == -1 )
	{
		char system_string[ 1024 ];

		snprintf(
			system_string,
			sizeof ( system_string ),
	 		"table_column_exists.sh %s %s",
			predictive_fund_table_name,
			predictive_fund_column_name );

		fund_boolean = ( system( system_string ) == 0 );
	}

	return fund_boolean;
}

char *predictive_title_passage_rule_string(
		enum predictive_title_passage_rule
			predictive_title_passage_rule )
{
	if ( predictive_title_passage_rule == title_passage_rule_null )
		return TITLE_PASSAGE_RULE_NULL;
	else
	if ( predictive_title_passage_rule == FOB_shipping )
		return TITLE_PASSAGE_RULE_SHIPPED_DATE;
	else
	if ( predictive_title_passage_rule == FOB_destination )
		return TITLE_PASSAGE_RULE_ARRIVED_DATE;
	else
		return TITLE_PASSAGE_RULE_NULL;
}

enum predictive_title_passage_rule predictive_resolve_title_passage_rule(
		char *title_passage_rule_string )
{
	if ( !title_passage_rule_string )
	{
		return title_passage_rule_null;
	}
	else
	if ( string_strcmp(
		title_passage_rule_string,
		TITLE_PASSAGE_RULE_SHIPPED_DATE ) == 0 )
	{
		return FOB_shipping;
	}
	else
	if ( string_strcmp(
		title_passage_rule_string,
		TITLE_PASSAGE_RULE_ARRIVED_DATE ) == 0 )
	{
		return FOB_destination;
	}
	else
	{
		return title_passage_rule_null;
	}
}

LIST *predictive_fund_name_list(
		const char *predictive_fund_table_name,
		const char *predictive_fund_column_name )
{
	char system_string[ 1024 ];

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh %s %s",
		predictive_fund_column_name,
		predictive_fund_table_name );

	return list_pipe( system_string );
}

char *predictive_fund_name(
		const char *predictive_fund_table_name,
		const char *predictive_fund_column_name,
		char *fund_name )
{
	if ( predictive_fund_boolean(
		predictive_fund_table_name,
		predictive_fund_column_name ) )
	{
		if ( !fund_name
		||   !*fund_name
		||   strcmp(	fund_name,
				PREDICTIVE_FUND_TABLE_NAME ) == 0
		||   strcmp(	fund_name,
				PREDICTIVE_FUND_COLUMN_NAME ) == 0 )
		{
			char message[ 1024 ];

			snprintf(
				message,
				sizeof ( message ),
				"Invalid fund_name=[%s]",
				fund_name );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		return fund_name;
	}
	else
	{
		return NULL;
	}
}
