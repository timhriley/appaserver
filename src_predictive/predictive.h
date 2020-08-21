/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/predictive.h			*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#ifndef PREDICTIVE_H
#define PREDICTIVE_H

#include "list.h"
#include "boolean.h"

/* Constants */
/* --------- */
#define PREDICTIVE_ELEMENT_ASSET		"asset"
#define PREDICTIVE_ELEMENT_LIABILITY		"liability"
#define PREDICTIVE_ELEMENT_EQUITY		"equity"
#define PREDICTIVE_ELEMENT_REVENUE		"revenue"
#define PREDICTIVE_ELEMENT_EXPENSE		"expense"
#define PREDICTIVE_ELEMENT_GAIN			"gain"
#define PREDICTIVE_ELEMENT_LOSS			"loss"

/* Structures */
/* ---------- */

/* Operations */
/* ---------- */

char *predictive_transaction_date_time(
			char *transaction_date );

char *predictive_fund_where(
			char *fund_name );

boolean predictive_fund_attribute_exists(
			void );

/* Returns the existing transaction_date_time */
/* ------------------------------------------ */
char *predictive_exists_closing_entry(
			char *as_of_date );

char *predictive_existing_closing_date_time(
			char *as_of_date );

char *predictive_exists_closing_date_time(
			char *as_of_date );

#endif
