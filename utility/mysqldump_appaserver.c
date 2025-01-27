/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/utility/mysqldump_appaserver.c			*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mysqldump.h"

int main( int argc, char **argv )
{
	char *config_filename;
	MYSQLDUMP *mysqldump;

	if ( argc != 2 )
	{
		fprintf(stderr,
			"Usage: %s config_filename\n",
			argv[ 0 ] );
		exit( 1 );
	}

	config_filename = argv[ 1 ];

	mysqldump =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		mysqldump_new(
			argv[ 0 ],
			config_filename );

	if ( chdir( mysqldump->mysqldump_input->son ) != 0 )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: chdir(%s) failed.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			mysqldump->mysqldump_input->son );
		exit( 1 );
	}

	fork_control_execute(
		mysqldump->fork_control->parallel_count,
		mysqldump->
			fork_control->
			fork_control_process_list );

	if ( system(
		mysqldump->
			remove_password_system_string ) ){}

	if ( mysqldump->mysqldump_consolidate_tar_system_string )
	{
		if ( system(
			mysqldump->
				mysqldump_consolidate_tar_system_string ) ){}
	}

	if ( mysqldump->mysqldump_consolidate_rm_system_string )
	{
		if ( system(
			mysqldump->
				mysqldump_consolidate_rm_system_string ) ){}
	}

	if ( system( mysqldump->log_system_string ) ){}

	if ( system( mysqldump->rotate_system_string ) ){}

	if ( mysqldump->rsync_system_string )
	{
		if ( system( mysqldump->rsync_system_string ) ){}
	}

	if ( mysqldump->scp_system_string )
	{
		if ( system( mysqldump->scp_system_string ) ){}
	}

	if ( mysqldump->mysqldump_this_run_threshold_display )
	{
		printf( "%s\n",
			mysqldump->
				mysqldump_this_run_threshold_display );
	}

	if ( system(
		mysqldump->
			mysqldump_this_run_display_system_string ) ){}

	return 0;
}

