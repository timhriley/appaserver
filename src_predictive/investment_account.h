/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/investment_account.h		*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#ifndef INVESTMENT_ACCOUNT_H
#define INVESTMENT_ACCOUNT_H

#include "entity.h"
#include "list.h"

/* Constants */
/* --------- */
#define INVESTMENT_ACCOUNT_TABLE	"investment_account"

/* Structures */
/* ---------- */
typedef struct
{
	ENTITY *financial_entity;
	char *account_number;
	char *investment_classification;
	char *investment_purpose;
	int certificate_maturity_months;
	char *certificate_maturity_date;
	double interest_rate;
	double account_balance_latest;
	double balance_latest_database;
	LIST *account_balance_list;
} INVESTMENT_ACCOUNT;

/* Operations */
/* ---------- */
INVESTMENT_ACCOUNT *investment_account_new(
			char *full_name,
			char *street_address,
			char *account_number );

/* Returns investment_account_steady_state() */
/* ----------------------------------------- */
INVESTMENT_ACCOUNT *investment_account_fetch(
			char *full_name,
			char *street_address,
			char *account_number );

/* Returns program memory */
/* ---------------------- */
char *investment_account_select(
			void );

/* Returns investment_account_steady_state() */
/* -------------------------------------- */
INVESTMENT_ACCOUNT *investment_account_parse(
			char *input );

void investment_account_update(
			double balance_latest,
			char *full_name,
			char *street_address,
			char *account_number );

FILE *investment_account_update_open(
			void );

INVESTMENT_ACCOUNT *investment_account_steady_state(
			char *full_name,
			char *street_address,
			char *account_number,
			char *investment_classification,
			char *investment_purpose,
			int certificate_maturity_months,
			char *certificate_maturity_date,
			double interest_rate,
			double balance_latest_database,
			LIST *account_balance_list );

char *investment_account_sys_string(
			char *where );

LIST *investment_account_system_list(
			char *sys_string );

/* Safely returns heap memory. */
/* --------------------------- */
char *investment_account_primary_where(
			char *full_name,
			char *street_address,
			char *account_number );

double investment_account_balance_latest(
			LIST *account_balance_list );

#endif
