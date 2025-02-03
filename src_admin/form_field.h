/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_admin/form_field.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FORM_FIELD_H
#define FORM_FIELD_H

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
LIST *form_field_datum_insert_statement_list(
		const char *form_field_datum_table,
		LIST *form_field_insert_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *form_field_datum_insert_sql_statement(
		const char *form_field_datum_table,
		LIST *form_field_datum_list );

typedef struct
{
	LIST *form_field_datum_list;
} FORM_FIELD_INSERT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FORM_FIELD_INSERT *form_field_insert_new(
		char *email_address,
		char *form_name,
		char *timestamp,
		LIST *form_field_datum_list /* in/out */ );

/* Process */
/* ------- */
FORM_FIELD_INSERT *form_field_insert_calloc(
		void );

typedef struct
{
	LIST *list;
} FORM_FIELD_INSERT_LIST;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FORM_FIELD_INSERT_LIST *form_field_insert_list_new(
		void );

/* Process */
/* ------- */
FORM_FIELD_INSERT_LIST *form_field_insert_list_calloc(
		void );

#endif
