/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_canvass/canvass_street.h	   		*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#ifndef CANVASS_STREET_H
#define CANVASS_STREET_H

#include "boolean.h"
#include "list.h"
#include "street.h"

#define CANVASS_STREET_SELECT		"street_name,"		\
					"city,"			\
					"state_code,"		\
					"action"

#define CANVASS_STREET_TABLE		"canvass_street"

typedef struct
{
	STREET *street;
	boolean include_boolean;
	boolean start_boolean;

	/* Set externally */
	/* -------------- */
	double votes_per_election;
} CANVASS_STREET;

/* Usage */
/* ----- */
LIST *canvass_street_list(
		char *canvass_name,
		LIST *street_list );

/* Usage */
/* ----- */
CANVASS_STREET *canvass_street_parse(
		LIST *street_list,
		char *input );

/* Process */
/* ------- */
CANVASS_STREET *canvass_street_calloc(
		void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *canvass_street_where(
		char *canvass_name );

/* Usage */
/* ----- */
void canvass_street_list_votes_per_election_set(
		LIST *canvass_street_list );

/* Usage */
/* ----- */
double canvass_street_votes_per_election(
		char *street_name );

/* Process */
/* ------- */
int canvass_street_recent_vote_sum(
		char *street_name );

int canvass_street_address_count(
		char *street_name );

/* Usage */
/* ----- */
void canvass_street_output(
		CANVASS_STREET *canvass_street,
		int start_distance_yards );

#endif
