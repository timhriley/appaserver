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
					"full_name"

#define FEEDER_PHRASE_DELIMITER		'|'

typedef struct
{
	char *phrase;
	char *nominal_account;
	char *full_name;
	char *contact_key;
	int match_length;
} FEEDER_PHRASE;

/* Usage */
/* ----- */
LIST *feeder_phrase_list(
		const char *feeder_phrase_select,
 		const char *feeder_phrase_table,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */
FEEDER_PHRASE *feeder_phrase_parse(
		boolean entity_contact_key_boolean,
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
		char *financial_institution_contact_key,
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
		char *financial_institution_contact_key,
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
		char *financial_institution_contact_key,
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
char *feeder_phrase_contact_key(
		char *financial_institution_contact_key,
		char *feeder_phrase_contact_key );

/* Usage */
/* ----- */
boolean feeder_phrase_match_boolean(
		char *description_space_trim,
		char *feeder_component );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *feeder_phrase_insert(
		const char *feeder_phrase_table,
		const char *feeder_phrase_select,
		const char *entity_contact_key_column,
		boolean contact_key_boolean,
		char *feeder_phrase,
		char *feeder_account,
		char *new_full_name,
		char *entity_contact_key );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *feeder_phrase_insert_system_string(
		const char *feeder_phrase_table,
		const char *feeder_phrase_select,
		const char *entity_contact_key_column,
		const char sql_delimiter,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *feeder_phrase_insert_data_string(
		const char sql_delimiter,
		boolean entity_contact_key_boolean,
		char *feeder_phrase,
		char *feeder_account,
		char *new_full_name,
		char *entity_contact_key );

#endif
