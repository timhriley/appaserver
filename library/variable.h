/* invest2:$root/library/tim/variable.h */
/* ------------------------------------ */
#ifndef VARIABLE_H
#define VARIABLE_H

#include "hash_table.h"

typedef struct
{
	HASH_TABLE *hash_table;
	
} VARIABLE;

/* Function prototypes */
/* ------------------- */
VARIABLE *variable_init();
int set_variable( VARIABLE *v, char *name, char *value );
void display_variable_table( VARIABLE *v, char *output_file );
int variable_free();

/* Both these do the same thing */
/* ---------------------------- */
char *get_variable( VARIABLE *v, char *name );
char *get_value( VARIABLE *v, char *name );

char *replace_all_variables( char *d, char *s, VARIABLE *v );
void variable_to_memory( char **key, char **data, VARIABLE *v );

#endif
