/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/predictive.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit Appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef PREDICTIVE_H
#define PREDICTIVE_H

#include "list.h"
#include "boolean.h"

#define TITLE_PASSAGE_RULE_SHIPPED_DATE 	"FOB_shipping"
#define TITLE_PASSAGE_RULE_ARRIVED_DATE 	"FOB_destination"
#define TITLE_PASSAGE_RULE_NULL			""

#define PREDICTIVE_FUND_TABLE_NAME		"fund"
#define PREDICTIVE_FUND_COLUMN_NAME		"fund_name"

enum predictive_title_passage_rule{
	title_passage_rule_null,
	FOB_shipping,
	FOB_destination };

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *predictive_fund_where(
		const char *predictive_fund_table_name,
		const char *predictive_fund_column_name,
		char *fund_name );

/* Usage */
/* ----- */
boolean predictive_fund_boolean(
		const char *predictive_fund_table_name,
		const char *predictive_fund_column_name );

/* Usage */
/* ----- */
enum predictive_title_passage_rule
	predictive_resolve_title_passage_rule(
		char *title_passage_rule_string );

/* Usage */
/* ----- */

/* Returns program memory */
/* ---------------------- */
char *predictive_title_passage_rule_string(
		enum predictive_title_passage_rule
			predictive_title_passage_rule );

/* Usage */
/* ----- */
LIST *predictive_fund_name_list(
		const char *predictive_fund_table_name,
		const char *predictive_fund_column_name );

#endif
