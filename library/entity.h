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

#define ENTITY_FINANCIAL_INSTITUTION_TABLE \
					"financial_institution"

#define ENTITY_SELECT			"full_name,"		\
					"street_address,"	\
					"city,"			\
					"state_code,"		\
					"zip_code,"		\
					"land_phone_number,"	\
					"email_address"

#define ENTITY_INSERT_COLUMNS		"full_name,"		\
					"street_address,"	\
					"email_address"

#define ENTITY_STREET_ADDRESS_UNKNOWN	"unknown"

typedef struct
{
	char *full_name;
	char *street_address;
	char *city;
	char *state_code;
	char *zip_code;
	char *land_phone_number;
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
boolean entity_login_name_boolean(
		const char *entity_table,
		const char *appaserver_user_primary_key );

/* Usage */
/* ----- */
ENTITY *entity_login_name_fetch(
		const char *entity_table,
		const char *appaserver_user_primary_key,
		char *login_name );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *entity_login_name_where(
		const char *appaserver_user_primary_key,
		char *login_name );

/* Public */
/* ------ */
LIST *entity_full_street_list(
		LIST *full_name_list,
		LIST *street_address_list );

/* Returns static memory */
/* --------------------- */
char *entity_escape_full_name(
		char *full_name );
char *entity_escape_name(
		char *full_name );
char *entity_name_escape(
		char *full_name );

/* Returns heap memory or null */
/* --------------------------- */
char *entity_street_address(
		char *full_name );

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

