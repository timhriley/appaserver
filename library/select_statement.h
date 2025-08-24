/* -------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/select_statement.h				*/
/* -------------------------------------------------------------	*/
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------	*/

#ifndef SELECT_STATEMENT_H
#define SELECT_STATEMENT_H

#include "boolean.h"
#include "list.h"

#define SELECT_STATEMENT_SELECT		"select_statement"
#define SELECT_STATEMENT_TABLE		"select_statement"

typedef struct
{
	char *select_statement_title;
	char *login_name;
	char *statement;
} SELECT_STATEMENT;

/* Usage */
/* ----- */
SELECT_STATEMENT *select_statement_fetch(
			char *select_statement_title );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *select_statement_primary_where(
			char *select_statement_title );

/* Usage */
/* ----- */
SELECT_STATEMENT *select_statement_parse(
			char *select_statement_title,
			char *input );

/* Process */
/* ------- */
SELECT_STATEMENT *select_statement_calloc(
			void );

#endif
