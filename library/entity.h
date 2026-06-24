/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/entity.h					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef ENTITY_H
#define ENTITY_H

#include "boolean.h"
#include "list.h"

#define ENTITY_TABLE			"entity"
#define ENTITY_FULL_NAME_COLUMN		"full_name"
#define ENTITY_CONTACT_KEY_COLUMN	"contact_key"
#define ENTITY_CONTACT_KEY_DEFAULT	"unknown"

#define ENTITY_FINANCIAL_INSTITUTION_TABLE \
					"financial_institution"

#define ENTITY_SELECT			"full_name,"		\
					"street_address,"	\
					"city,"			\
					"state_code,"		\
					"zip_code,"		\
					"land_phone_number,"	\
					"cell_phone_number,"	\
					"email_address"

#define ENTITY_CONTACT_KEY_UNKNOWN	"unknown"
#define ENTITY_CONTACT_KEY_NULL		"null"

typedef struct
{
	boolean entity_contact_key_boolean;
	char *full_name;
	char *contact_key /* optional */;
	char *street_address;
	char *city;
	char *state_code;
	char *zip_code;
	char *land_phone_number;
	char *cell_phone_number;
	char *email_address;
} ENTITY;

/* Usage */
/* ----- */
ENTITY *entity_fetch(
		char *full_name,
		char *contact_key,
		boolean entity_contact_key_boolean );
/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *entity_select_string(
		const char *entity_select,
		const char *entity_contact_key_column,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *entity_primary_where(
		boolean entity_contact_key_boolean,
		char *full_name,
		char *contact_key );

/* Usage */
/* ----- */
ENTITY *entity_parse(
		boolean entity_contact_key_boolean,
		char *input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
ENTITY *entity_new(
		boolean entity_contact_key_boolean,
		char *full_name,
		char *contact_key );

/* Process */
/* ------- */
ENTITY *entity_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *entity_contact_key_where(
		boolean entity_contact_key_boolean,
		char *contact_key );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *entity_escape_full_name(
		char *full_name );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *entity_escape_contact_key(
		char *contact_key );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *entity_fetch_contact_key(
		boolean entity_contact_key_boolean,
		char *full_name );

/* Usage */
/* ----- */
LIST *entity_full_contact_list(
		LIST *full_name_list,
		LIST *contact_key_list );

/* Usage */
/* ----- */
ENTITY *entity_full_name_seek(
		char *full_name,
		LIST *entity_list );

/* Usage */
/* ----- */
ENTITY *entity_getset(
		boolean entity_contact_key_boolean,
		LIST *entity_list,
		char *full_name,
		char *contact_key,
		boolean strdup_boolean );

/* Usage */
/* ----- */
ENTITY *entity_seek(
		char *full_name,
		char *contact_key,
		LIST *entity_list );

/* Usage */
/* ----- */
boolean entity_list_exist_boolean(
		char *full_name,
		char *contact_key,
		LIST *entity_list );

/* Usage */
/* ----- */

/* Returns error message (heap memory) or null */
/* ------------------------------------------- */
char *entity_insert(
		const char *entity_table,
		const char *entity_full_name_column,
		const char *entity_contact_key_column,
		boolean entity_contact_key_boolean,
		char *full_name,
		char *contact_key,
		boolean ignore_duplicate_boolean );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *entity_insert_system_string(
		const char *entity_table,
		const char *entity_full_name_column,
		const char *entity_contact_key_column,
		const char sql_delimiter,
		boolean entity_contact_key_boolean,
		boolean ignore_duplicate_boolean );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *entity_insert_column_string(
		const char *entity_insert,
		const char *entity_contact_key_column,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *entity_insert_data_string(
		const char sql_delimiter,
		boolean entity_contact_key_boolean,
		char *full_name,
		char *contact_key );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *entity_name_display(
		char *full_name,
		char *contact_key );

/* Usage */
/* ----- */
ENTITY *entity_full_name_entity(
		boolean entity_contact_key_boolean,
		char *full_name /* stack memory */ );

/* Usage */
/* ----- */

/* Returns static boolean */
/* ---------------------- */
boolean entity_contact_key_boolean(
		const char *entity_table,
		const char *entity_contact_key_column );

/* Usage */
/* ----- */

/* Returns parameter, heap memory, or null */
/* --------------------------------------- */
char *entity_contact_key(
		const char *entity_table,
		const char *entity_contact_key_column,
		const char *entity_contact_key_default,
		char *full_name,
		char *contact_key,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */
boolean entity_contact_key_populated_boolean(
		const char *entity_contact_key_column,
		char *contact_key );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LIST *entity_primary_key_list(
		const char *entity_full_name_column,
		const char *entity_contact_key_column,
		boolean entity_contact_key_boolean );

#endif

