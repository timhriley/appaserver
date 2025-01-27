/* $APPASERVER_HOME/utility/mysqldump_fork_count_drop.c */
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "boolean.h"
#include "column.h"
#include "timlib.h"
#include "appaserver_error.h"
#include "mysqldump.h"

/* Constants */
/* --------- */
#define PERCENTAGE_DROP_THRESHOLD	0.20

/* Prototypes */
/* ---------- */
void mysqldump_fork_count_drop(
				char *mysqldump_database_count_file,
				char *mysqldump_database_yesterday_count_file );

int main( int argc, char **argv )
{
	char *mysqldump_database_count_file;
	char *mysqldump_database_yesterday_count_file;

	if ( argc != 3 )
	{
		fprintf( stderr,
"Usage: %s mysqldump_database_count_file mysqldump_database_yesterday_count_file\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	appaserver_error_stderr( argc, argv );

	mysqldump_database_count_file = argv[ 1 ];
	mysqldump_database_yesterday_count_file = argv[ 2 ];

	mysqldump_fork_count_drop(
		mysqldump_database_count_file,
		mysqldump_database_yesterday_count_file );

	return 0;

} /* main() */

void mysqldump_fork_count_drop(
				char *mysqldump_database_count_file,
				char *mysqldump_database_yesterday_count_file )
{
	MYSQLDUMP *mysqldump;

	mysqldump =
		mysqldump_new(
			mysqldump_database_count_file,
			mysqldump_database_yesterday_count_file );

	mysqldump_set_percentage_drop(
		mysqldump->mysqldump_database_folder_list,
		mysqldump->mysqldump_database_yesterday_folder_list );

	mysqldump->reached_percentage_drop_name_list =
		mysqldump_get_reached_percentage_drop_name_list(
			mysqldump->mysqldump_database_folder_list,
			PERCENTAGE_DROP_THRESHOLD );

/* This was to check if the table no longer exists.
   ------------------------------------------------
	list_append_list(
		mysqldump->reached_percentage_drop_name_list,
		mysqldump_get_table_not_exists_drop_name_list(
			mysqldump->mysqldump_database_yesterday_folder_list ) );
*/

	if ( list_length( mysqldump->reached_percentage_drop_name_list ) )
	{
		printf( "%s\n",
			list_display(
				mysqldump->
					reached_percentage_drop_name_list ) );
	}

} /* mysqldump_fork_count_drop() */

