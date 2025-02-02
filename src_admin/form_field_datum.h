/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_admin/form_field_datum.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FORM_FIELD_DATUM_H
#define FORM_FIELD_DATUM_H

#include "boolean.h"
#include "list.h"
#include "insert.h"

#define FORM_FIELD_DATUM_TABLE		"form_field_datum"

#define FORM_FIELD_DATUM_SELECT		"form_name,"		\
					"field_name,"		\
					"field_datum,"		\
					"message_datum"

typedef struct
{
	char *form_name;
	char *field_name;
	char *field_datum;
	char *message_datum;
	INSERT_DATUM *insert_datum;
} FORM_FIELD_DATUM;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FORM_FIELD_DATUM *form_field_datum_new(
		char *field_name,
		char *field_datum,
		char *message_datum,
		int primary_key_index );

/* Process */
/* ------- */
FORM_FIELD_DATUM *form_field_datum_calloc(
		void );

/* Returns either parameter */
/* ------------------------ */
char *form_field_datum_field(
		char *field_datum,
		char *message_datum );

/* Usage */
/* ----- */
LIST *form_field_datum_insert_list(
		LIST *form_field_datum_list );

/* Usage */
/* ----- */
LIST *form_field_datum_fetch_list(
		char *email_address,
		char *timestamp );

/* Usage */
/* ----- */
FORM_FIELD_DATUM *form_field_datum_parse(
		char *string_input );

/* Usage */
/* ----- */
FORM_FIELD_DATUM *form_field_datum_seek(
		LIST *form_field_datum_list,
		char *field_name );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *form_field_datum_insert_statement(
		const char *form_field_datum_table,
		LIST *form_field_datum_list );

#endif
