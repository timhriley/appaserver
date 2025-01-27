/* $APPASERVER_HOME/library/query_isa.h					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef QUERY_ISA_H
#define QUERY_ISA_H

#include "list.h"
#include "boolean.h"

typedef struct
{
	char *where;
} QUERY_ISA;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
QUERY_ISA *query_isa_where_new(
			char *application_name,
			LIST *relation_mto1_isa_list );

/* Process */
/* ------- */
QUERY_ISA *query_isa_calloc(
			void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *query_isa_where(
			char *application_name,
			LIST *relation_mto1_isa_list );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *query_isa_where_join(
			char *application_name,
			char *many_folder_name,
			LIST *relation_foreign_key_list,
			char *one_folder_name,
			LIST *one_folder_primary_key_list );

#endif
