/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_canvass/street.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include "boolean.h"
#include "list.h"

#ifndef STREET_H
#define STREET_H

#define STREET_SELECT			"street_name,"		\
					"city,"			\
					"state_code,"		\
					"zip_code,"		\
					"house_count,"		\
					"apartment_count,"	\
					"longitude,"		\
					"latitude,"		\
					"county_district"

#define STREET_TABLE			"street"

typedef struct
{
	char *street_name;
	char *city;
	char *state_code;
	char *zip_code;
	int house_count;
	int apartment_count;
	char *longitude_string;
	char *latitude_string;
	int county_district;
	int total_count;
} STREET;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
STREET *street_fetch(
		char *street_name,
		char *city,
		char *state_code );

/* Process */
/* ------- */

/* Usage */
/* ----- */
STREET *street_parse(
		char *street_name,
		char *city,
		char *state_code,
		char *input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
STREET *street_new(
		char *street_name,
		char *city,
		char *state_code );

/* Process */
/* ------- */
STREET *street_calloc(
		void );

/* Usage */
/* ----- */
LIST *street_list(
		char *city,
		char *state_code );

/* Returns static memory */
/* --------------------- */
char *street_where(
		char *city,
		char *state_code );

int street_total_count(
		int house_count,
		int apartment_count );

/* Usage */
/* ----- */
STREET *street_seek(
		LIST *street_list,
		char *street_name,
		char *city,
		char *state_code );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *street_primary_where(
		char *street_name,
		char *city,
		char *state_code );

#endif
