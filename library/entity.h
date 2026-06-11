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

typedef struct
{
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
LIST *entity_system_list(
		char *system_string );

/* Process */
/* ------- */

/* Safely returns */
/* -------------- */
FILE *entity_input_pipe(
		char *system_string );

/* Usage */
/* ----- */
ENTITY *entity_fetch(
		char *full_name,
		char *street_address );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *entity_primary_where(
		char *full_name,
		char *street_address );

/* Returns heap memory */
/* ------------------- */
char *entity_system_string(
		const char *entity_select,
		const char *entity_table,
		char *entity_primary_where );

/* Usage */
/* ----- */
ENTITY *entity_parse(
		char *input );


/* Usage */
/* ----- */
ENTITY *entity_new(
		char *full_name,
		char *street_address );

/* Process */
/* ------- */
ENTITY *entity_calloc(
		void );

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
		char *full_name );

/* Public */
/* ------ */
LIST *entity_full_street_list(
		LIST *full_name_list,
		LIST *street_address_list );

ENTITY *entity_full_name_seek(
		char *full_name,
		LIST *entity_list );

ENTITY *entity_getset(
		LIST *entity_list,
		char *full_name,
		char *street_address,
		boolean with_strdup );

ENTITY *entity_seek(
		char *full_name,
		char *street_address,
		LIST *entity_list );

boolean entity_list_exists(
		char *full_name,
		char *street_address,
		LIST *entity_list );

FILE *entity_insert_open(
		char *error_filename );

void entity_insert_pipe(
		FILE *entity_insert_open,
		char *full_name,
		char *street_address,
		char *email_address );

char *entity_name_display(
		char *full_name,
		char *street_address );

ENTITY *entity_full_name_entity(
		char *full_name );

#endif

