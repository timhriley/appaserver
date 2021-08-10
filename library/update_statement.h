/* $APPASERVER_HOME/library/update_statement.h		*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef UPDATE_STATEMENT_H
#define UPDATE_STATEMENT_H

#include "list.h"

typedef struct
{
	char *table_name;
	LIST *attribute_list;
	LIST *data_list;
	LIST *primary_attribute_list;
	LIST *primary_data_list;
} UPDATE_STATEMENT;

UPDATE_STATEMENT *update_statement_new(
			char *table_name );

void update_statement_set_attribute_list(
			UPDATE_STATEMENT *u,
			LIST *attribute_list );

void update_statement_set_data_list(
			UPDATE_STATEMENT *u,
			LIST *data_list );

void update_statement_set_primary_attribute_list(
			UPDATE_STATEMENT *u,
			LIST *primary_attribute_list );

void update_statement_set_primary_data_list(
			UPDATE_STATEMENT *u,
			LIST *primary_data_list );

int update_statement_get(
			char *destination,
			UPDATE_STATEMENT *u );

int update_statement_where_clause(	
			char *destination,
			LIST *primary_attribute_list,
			LIST *primary_data_list );

int update_statement_set_clause(	
			char *destination,
			LIST *attribute_list,
			LIST *data_list );
#endif
