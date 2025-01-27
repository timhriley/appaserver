/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/delete_primary.h	 			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org 	*/
/* -------------------------------------------------------------------- */

#ifndef DELETE_PRIMARY_H
#define DELETE_PRIMARY_H

#include <stdio.h>
#include "list.h"
#include "boolean.h"
#include "session.h"
#include "delete.h"

typedef struct
{
	SESSION *session_fetch;
	char *folder_name;
	LIST *folder_attribute_fetch_primary_key_list;
	LIST *query_cell_list;
	DELETE *delete;
} DELETE_PRIMARY;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DELETE_PRIMARY *delete_primary_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *primary_data_list_string,
		boolean delete_omit_isa_boolean );

/* Process */
/* ------- */
DELETE_PRIMARY *delete_primary_calloc(
		void );

/* Usage */
/* ----- */
LIST *delete_primary_query_cell_list(
		char sql_delimiter,
		char *primary_data_list_string,
		LIST *folder_attribute_fetch_primary_key_list );

#endif
