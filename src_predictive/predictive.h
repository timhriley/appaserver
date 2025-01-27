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

enum title_passage_rule{			title_passage_rule_null,
						FOB_shipping,
						FOB_destination };


/* Public */
/* ------ */
char *predictive_fund_where(
			char *fund_name );

boolean predictive_fund_attribute_exists(
			void );

boolean predictive_fund_exists(
			void );

enum title_passage_rule
	predictive_title_passage_rule_resolve(
		char *title_passage_rule_string );

char *predictive_title_passage_rule_string(
		enum title_passage_rule title_passage_rule );

#endif
