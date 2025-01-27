/* library/parse_insert_statement.c */
/* -------------------------------- */

#include <stdio.h>
#include <string.h>
#include "parse_insert_statement.h"


PARSE_INSERT_STATEMENT *new_parse_insert_statement( char *insert_statement )
{
	PARSE_INSERT_STATEMENT *p;
	LIST *column_list;
	LIST *value_list;

	p = (PARSE_INSERT_STATEMENT *)
		calloc( 1, sizeof( PARSE_INSERT_STATEMENT ) );
	p->insert_statement = strdup( insert_statement );

	column_list = 
		parse_insert_statement_get_column_list( insert_statement );

	value_list = 
		parse_insert_statement_get_value_list( insert_statement );

	p->dictionary = 
		dictionary_two_lists2dictionary( column_list, value_list );
	return p;
} /* new_parse_insert_statement() */

LIST *parse_insert_statement_get_column_list( char *insert_statement )
{
	LIST *column_list;
	char right_half[ 4096 ], column_list_string[ 4096 ];

	column_list = list_new();

	piece( right_half, '(', insert_statement, 1 );
	piece( column_list_string, ')', right_half, 0 );

	return column_list;
}

LIST *parse_insert_statement_get_value_list( char *insert_statement )
{
}
