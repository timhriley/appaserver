/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/feeder_phrase.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FEEDER_PHRASE_H
#define FEEDER_PHRASE_H

#include "list.h"
#include "boolean.h"
#include "date.h"
#include "appaserver_user.h"
#include "html.h"
#include "transaction.h"
#include "journal.h"
#include "account.h"
#include "feeder_phrase.h"

#define FEEDER_PHRASE_TABLE		"feeder_phrase"

#define FEEDER_PHRASE_SELECT		"feeder_phrase,"		\
					"nominal_account,"		\
					"full_name,"			\
					"street_address"

#define FEEDER_PHRASE_DELIMITER		'|'

typedef struct
{
	char *phrase;
	char *nominal_account;
	char *full_name;
	char *street_address;
	int match_length;
} FEEDER_PHRASE;

/* Usage */
/* ----- */
LIST *feeder_phrase_list(
		char *feeder_phrase_select,
 		char *feeder_phrase_table );

/* Usage */
/* ----- */
FEEDER_PHRASE *feeder_phrase_parse(
		char *input );

/* Usage */
/* ----- */
FEEDER_PHRASE *feeder_phrase_new(
		char *phrase );

/* Process */
/* ------- */
FEEDER_PHRASE *feeder_phrase_calloc(
		void );

/* Usage */
/* ----- */
FEEDER_PHRASE *feeder_phrase_seek(
		char *financial_institution_full_name,
		char *financial_institution_street_address,
		char *feeder_load_row_description_space_trim,
		LIST *feeder_phrase_list );

/* Process */
/* ------- */
void feeder_phrase_zap_match_length(
		LIST *feeder_phrase_list );

/* Usage */
/* ----- */
void feeder_phrase_set_match_length(
		const char feeder_phrase_delimiter,
		char *description_space_trim,
		LIST *feeder_phrase_list );

/* Usage */
/* ----- */
FEEDER_PHRASE *feeder_phrase_extract(
		char *financial_institution_full_name,
		char *financial_institution_street_address,
		LIST *feeder_phrase_list );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *feeder_phrase_primary_where(
		char *feeder_phrase );

/* Usage */
/* ----- */

/* Returns feeder_phrase */
/* --------------------- */
FEEDER_PHRASE *feeder_phrase_entity_set(
		char *financial_institution_full_name,
		char *feeder_phrase_seek_full_name,
		FEEDER_PHRASE *feeder_phrase /* in/out */ );

/* Process */
/* ------- */

/* Returns either parameter */
/* ------------------------ */
char *feeder_phrase_full_name(
		char *financial_institution_full_name,
		char *feeder_phrase_full_name );

/* Returns either parameter */
/* ------------------------ */
char *feeder_phrase_street_address(
		char *financial_institution_street_address,
		char *feeder_phrase_street_address );

/* Usage */
/* ----- */
boolean feeder_phrase_match_boolean(
		char *description_space_trim,
		char *feeder_component );

#endif
