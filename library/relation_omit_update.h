/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/relation_omit_update.h 			*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#ifndef RELATION_OMIT_UPDATE_H
#define RELATION_OMIT_UPDATE_H

#include "boolean.h"
#include "list.h"
#include "query.h"

typedef struct
{
	LIST *relation_one2m_list;
	boolean viewonly_boolean;
} RELATION_OMIT_UPDATE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
RELATION_OMIT_UPDATE *relation_omit_update_new(
		char *folder_name,
		LIST *relation_mto1_recursive_list
			/* relation_mto1_list_set_one_to_many_list() */,
		LIST *relation_one2m_list,
		LIST *query_fetch_row_list );

/* Process */
/* ------- */
RELATION_OMIT_UPDATE *relation_omit_update_calloc(
		void );

/* Usage */
/* ----- */
LIST *relation_omit_update_one2m_list(
		LIST *relation_mto1_recursive_list,
		LIST *relation_one2m_list );

/* Usage */
/* ----- */
boolean relation_omit_update_set_viewonly_boolean(
		LIST *query_fetch_row_list /* in/out */,
		LIST *relation_omit_update_one2m_list );

/* Usage */
/* ----- */
boolean relation_omit_update_row_set_viewonly_boolean(
		QUERY_ROW *query_row /* in/out */,
		LIST *relation_omit_update_one2m_list );

#endif
