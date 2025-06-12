/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/preupdate_change.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef PREUPDATE_CHANGE_H
#define PREUPDATE_CHANGE_H

#include <stdio.h>
#include "list.h"
#include "boolean.h"

enum preupdate_change_state {
	from_null_to_something,
	from_something_to_null,
	from_something_to_something_else,
	no_change_null,
	no_change_something };

typedef struct
{
	enum preupdate_change_state state_evaluate;
	boolean no_change_boolean;
	char *prior_datum;
	char *new_datum;
} PREUPDATE_CHANGE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PREUPDATE_CHANGE *preupdate_change_new(
		const char *appaserver_insert_state,
		const char *appaserver_predelete_state,
		char *state,
		char *preupdate_datum,
		char *postupdate_datum,
		const char *preupdate_placeholder_name );

/* Process */
/* ------- */
PREUPDATE_CHANGE *preupdate_change_calloc(
		void );

boolean preupdate_change_no_change_boolean(
		enum preupdate_change_state state_evaluate );

/* Usage */
/* ----- */
enum preupdate_change_state preupdate_change_state_evaluate(
		const char *appaserver_insert_state,
		const char *appaserver_predelete_state,
		char *state,
		char *preupdate_datum,
		char *postupdate_datum,
		const char *preupdate_placeholder_name );

/* Usage */
/* ----- */

/* Returns preupdate_attribute_datum or null */
/* ----------------------------------------- */
char *preupdate_change_prior_datum(
		char *preupdate_attribute_datum,
		enum preupdate_change_state
			preupdate_change_state );

/* Usage */
/* ----- */

/* Returns attribute_datum or null */
/* ------------------------------- */
char *preupdate_change_new_datum(
		char *attribute_datum,
		enum preupdate_change_state
			preupdate_change_state );

/* Usage */
/* ----- */

/* Returns program memory */
/* ---------------------- */
char *preupdate_change_state_display(
		enum preupdate_change_state state_evaluate );

#endif

