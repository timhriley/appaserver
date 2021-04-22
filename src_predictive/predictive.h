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

/* Structures */
/* ---------- */

/* Operations */
/* ---------- */

/* ------------------------------------ */
/* Returns heap memory			*/
/* Increments seconds each invocation   */
/* ------------------------------------ */
char *predictive_transaction_date_time(
			char *transaction_date );

char *predictive_fund_where(
			char *fund_name );

boolean predictive_fund_attribute_exists(
			void );

boolean predictive_fund_exists(
			void );

#endif
