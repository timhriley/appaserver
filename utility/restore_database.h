/* --------------------------------------------------- 	*/
/* utility/restore_database.h			       	*/
/* --------------------------------------------------- 	*/
/* 						       	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

#ifndef RESTORE_DATABASE_H
#define RESTORE_DATABASE_H

/* Includes */
/* -------- */
#include "list.h"

typedef struct
{
	char *table_name;
	char *data_process_string;
	LIST *column_name_list;
	char delimiter;
} RESTORE_TABLE;

typedef struct
{
	char *configuration_filename;
	char *sql_statement_process_string;
	LIST *restore_table_list;
} RESTORE_DATABASE;

RESTORE_DATABASE *restore_database_new_restore_database(
				char *configuration_filename );

int restore_database_load_configuration_file(
				LIST **restore_table_list,
				char *configuration_filename );

char *restore_database_get_sql_statement_process_string(
				char *data_process_string,
				char *table_name,
				LIST *column_name_list,
				char delimiter );

LIST *restore_database_get_column_name_list(
				char *describe_process );

#endif
