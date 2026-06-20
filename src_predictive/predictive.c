/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/predictive.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit Appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "list.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "predictive.h"

char *predictive_fund_where(
		char *fund_name,
		boolean fund_boolean )
{
	static char where[ 128 ];

	if ( !fund_name
	||   !*fund_name
	||   strcmp(	fund_name,
			PREDICTIVE_FUND_TABLE ) == 0
	||   strcmp(	fund_name,
			PREDICTIVE_FUND_COLUMN ) == 0
	||   !fund_boolean )
	{
		strcpy( where, "1 = 1" );
	}
	else
	{
		snprintf(
			where,
			sizeof ( where ),
		 	"%s = '%s'",
			PREDICTIVE_FUND_COLUMN,
		 	fund_name );
	}

	return where;
}

boolean predictive_fund_boolean(
		const char *predictive_fund_table,
		const char *predictive_fund_column )
{
	static boolean fund_boolean = -1;

	if ( fund_boolean == -1 )
	{
		fund_boolean =
			appaserver_table_column_boolean(
				predictive_fund_table,
				predictive_fund_column );
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
		const char *predictive_fund_table,
		const char *predictive_fund_column )
{
	char system_string[ 1024 ];

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh %s %s",
		predictive_fund_column,
		predictive_fund_table );

	return list_pipe( system_string );
}

char *predictive_fund_name(
		char *fund_name,
		boolean predictive_fund_boolean )
{
	if ( predictive_fund_boolean )
	{
		if ( !fund_name
		||   !*fund_name
		||   strcmp(	fund_name,
				PREDICTIVE_FUND_TABLE ) == 0
		||   strcmp(	fund_name,
				PREDICTIVE_FUND_COLUMN ) == 0 )
		{
			return NULL;
		}

		return fund_name;
	}
	else
	{
		return NULL;
	}
}
