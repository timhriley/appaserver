/* html/library/parse_insert_statement.h */
/* ------------------------------------- */

#ifndef PARSE_INSERT_STATEMENT_H
#define PARSE_INSERT_STATEMENT_H

#include "list.h"
#include "dictionary.h"

typedef struct
{
	DICTIONARY *column_dictionary;
} PARSE_INSERT_STATEMENT;

PARSE_INSERT_STATEMENT *new_parse_insert_statement( char *insert_statement );
LIST *parse_insert_statement_get_column_list( 	    char *insert_statement );
LIST *parse_insert_statement_get_value_list( 	    char *insert_statement );
#endif
