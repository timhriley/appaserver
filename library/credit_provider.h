/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/credit_provider.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef CREDIT_PROVIDER_H
#define CREDIT_PROVIDER_H

#include "boolean.h"

#define MAX_CC_SIZE  16
#define MIN_CC_SIZE  10  /* Carte Blanche Intern'l is 10 digits */

typedef struct {
        char cc_id;             /* cc id number */
        char cc_size;           /* string len of cc number   */
        char first_digit;       /* first digit of cc number  */
        char secnd_digit;       /* second digit of cc number */
        char third_digit;       /* third digit of cc number  */
} CC_ID_TYPE;

typedef struct
{
	char *number_stripped;
	boolean check_digit_boolean;
	int array_offset;
	char *name;
	char *sql_statement;
} CREDIT_PROVIDER;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CREDIT_PROVIDER *credit_provider_new(
		char *full_name,
		char *street_address,
		char *credit_card_number );

/* Process */
/* ------- */
CREDIT_PROVIDER *credit_provider_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *credit_provider_number_stripped(
		char *credit_card_number );

boolean credit_provider_check_digit_boolean(
		char *credit_provider_number_stripped );

int credit_provider_array_offset(
		char *credit_provider_number_stripped );

/* Returns component of global structure or null */
/* --------------------------------------------- */
char *credit_provider_name(
		int credit_provider_array_offset );

/* Returns heap memory */
/* ------------------- */
char *credit_provider_sql_statement(
		const char *entity_self_table,
		char *full_name,
		char *street_address,
		char *credit_provider_name );

#endif
