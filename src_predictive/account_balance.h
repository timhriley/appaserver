/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/account_balance.h		*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#ifndef ACCOUNT_BALANCE_H
#define ACCOUNT_BALANCE_H

#include "entity.h"
#include "list.h"

/* Constants */
/* --------- */
#define ACCOUNT_BALANCE_TABLE		"account_balance"

/* Structures */
/* ---------- */
typedef struct
{
	ENTITY *financial_entity;
	char *account_number;
	char *date;
	double balance;
	double account_balance_change;
	int account_balance_change_percent;
	double balance_change_database;
	int balance_change_percent_database;
} ACCOUNT_BALANCE;

/* Operations */
/* ---------- */
ACCOUNT_BALANCE *account_balance_new(
			char *full_name,
			char *street_address,
			char *account_number,
			char *date );

/* Returns program memory */
/* ---------------------- */
char *account_balance_select(
			void );

/* Returns account_balance_steady_state() */
/* -------------------------------------- */
ACCOUNT_BALANCE *account_balance_parse(
			char *input );

void account_balance_update(
			double account_balance_change,
			int account_balance_change_percent,
			char *full_name,
			char *street_address,
			char *account_number,
			char *date );

FILE *account_balance_update_open(
			void );

ACCOUNT_BALANCE *account_balance_steady_state(
			char *full_name,
			char *street_address,
			char *account_number,
			char *date,
			double balance,
			double balance_change_database,
			int balance_change_percent_database,
			LIST *account_balance_list );

LIST *account_balance_list(
			char *full_name,
			char *street_address,
			char *account_number );

char *account_balance_sys_string(
			char *where );

LIST *account_balance_system_list(
			char *sys_string );

ACCOUNT_BALANCE *account_balance_seek(
			LIST *account_balance_list,
			char *date );

double account_balance_change(
			double balance,
			LIST *account_balance_list );

int account_balance_change_percent(
			double balance,
			LIST *account_balance_list );

double account_balance_latest(
			LIST *account_balance_list );

#endif
