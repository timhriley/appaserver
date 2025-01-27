/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/mysqldump.c					*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit Appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include "String.h"
#include "piece.h"
#include "date.h"
#include "application.h"
#include "application_log.h"
#include "appaserver.h"
#include "appaserver_parameter.h"
#include "basename.h"
#include "environ.h"
#include "mysqldump.h"

MYSQLDUMP *mysqldump_new(
		char *argv_0,
		char *config_filename )
{
	MYSQLDUMP *mysqldump;

	if ( !argv_0
	||   !config_filename )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	mysqldump = mysqldump_calloc();

	environ_umask( APPLICATION_UMASK );

	mysqldump->mysqldump_input =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		mysqldump_input_fetch(
			argv_0,
			config_filename );

	mysqldump->subdirectory_exists_boolean =
		mysqldump_subdirectory_exists_boolean(
			mysqldump->mysqldump_input->son );

	if ( !mysqldump->subdirectory_exists_boolean )
	{
		mysqldump->mkdir_system_string =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			mysqldump_mkdir_system_string(
				MYSQLDUMP_MKDIR_SCRIPT,
				mysqldump->mysqldump_input->database,
				mysqldump->
					mysqldump_input->
					backup_directory );

		if ( system( mysqldump->mkdir_system_string ) ){}
	}
	else
	{
		mysqldump->mysqldump_last_run_filename =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			mysqldump_last_run_filename(
				MYSQLDUMP_COUNT_PREFIX,
				mysqldump->mysqldump_input->date_yyyymmdd,
				mysqldump->mysqldump_input->son );

		if ( mysqldump->mysqldump_last_run_filename )
		{
			mysqldump->mysqldump_last_run_yyyymmdd =
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				mysqldump_last_run_yyyymmdd(
					mysqldump->
						mysqldump_last_run_filename );

			mysqldump->mysqldump_last_run_list =
				mysqldump_last_run_list(
					MYSQLDUMP_DELIMITER,
					mysqldump->
						mysqldump_last_run_filename );
		}
	}

	mysqldump->mysqldump_this_run_list =
		mysqldump_this_run_list(
			MYSQLDUMP_DELIMITER,
			mysqldump->mysqldump_input->database,
			mysqldump->
				mysqldump_input->
				mysqldump_this_run_filename,
			mysqldump->mysqldump_last_run_list );

	mysqldump->mysqldump_this_run_threshold_display =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		mysqldump_this_run_threshold_display(
			MYSQLDUMP_DROP_THRESHOLD,
			mysqldump->mysqldump_input->database,
			mysqldump->mysqldump_this_run_list );

	mysqldump->mysqldump_this_run_display_system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		mysqldump_this_run_display_system_string(
			mysqldump->
				mysqldump_input->
				database,
			mysqldump->
				mysqldump_input->
				mysqldump_this_run_filename );

	mysqldump->mysqldump_separate_block_list =
		mysqldump_separate_list(
			mysqldump->mysqldump_input->database,
			mysqldump->mysqldump_input->mysql_user,
			mysqldump->mysqldump_input->password_filename,
			mysqldump->mysqldump_input->date_yyyymmdd,
			mysqldump->mysqldump_input->separate_block_table_list,
			0 /* not is_line */ );

	mysqldump->mysqldump_separate_line_list =
		mysqldump_separate_list(
			mysqldump->mysqldump_input->database,
			mysqldump->mysqldump_input->mysql_user,
			mysqldump->mysqldump_input->password_filename,
			mysqldump->mysqldump_input->date_yyyymmdd,
			mysqldump->mysqldump_input->separate_line_table_list,
			1 /* is_line */ );

	mysqldump->mysqldump_consolidate_list =
		mysqldump_consolidate_list(
			mysqldump->mysqldump_input->database,
			mysqldump->mysqldump_input->mysql_user,
			mysqldump->mysqldump_input->password_filename,
			mysqldump->mysqldump_input->date_yyyymmdd,
			mysqldump->mysqldump_input->separate_block_table_list,
			mysqldump->mysqldump_input->separate_line_table_list,
			mysqldump->mysqldump_input->exclude_table_list,
			mysqldump->mysqldump_this_run_list );

	mysqldump->fork_control =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		mysqldump_fork_control(
			mysqldump->mysqldump_input->parallel_count,
			mysqldump->mysqldump_separate_block_list,
			mysqldump->mysqldump_separate_line_list,
			mysqldump->mysqldump_consolidate_list );

	mysqldump->remove_password_system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		mysqldump_remove_password_system_string(
			mysqldump->mysqldump_input->password_filename );

	mysqldump->mysqldump_consolidate_tar_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		mysqldump_consolidate_tar_filename(
			mysqldump->mysqldump_input->database,
			mysqldump->mysqldump_input->date_yyyymmdd );

	mysqldump->mysqldump_consolidate_tar_system_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		mysqldump_consolidate_tar_system_string(
			mysqldump->mysqldump_consolidate_list,
			mysqldump->mysqldump_consolidate_tar_filename );

	mysqldump->mysqldump_consolidate_rm_system_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		mysqldump_consolidate_rm_system_string(
			mysqldump->mysqldump_consolidate_list );

	mysqldump->log_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		mysqldump_log_system_string(
			MYSQLDUMP_LOG_SCRIPT,
			APPLICATION_LOG_EXTENSION,
			mysqldump->mysqldump_input->database,
			mysqldump->mysqldump_input->log_directory,
			mysqldump->mysqldump_input->date_yyyymmdd,
			mysqldump->mysqldump_input->son );

	mysqldump->rotate_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		mysqldump_rotate_system_string(
			MYSQLDUMP_ROTATE_SCRIPT,
			mysqldump->mysqldump_input->date_yyyymmdd,
			mysqldump->mysqldump_input->son,
			mysqldump->mysqldump_input->father,
			mysqldump->mysqldump_input->grandfather,
			mysqldump->mysqldump_input->greatgrandfather,
			mysqldump->mysqldump_input->greatgreatgrandfather );

	if ( mysqldump->mysqldump_input->offsite_rsync_destination )
	{
		mysqldump->rsync_system_string =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			mysqldump_rsync_system_string(
				mysqldump->mysqldump_input->son,
				mysqldump->
					mysqldump_input->
					offsite_rsync_destination );
	}

	if ( mysqldump->mysqldump_input->offsite_scp_destination )
	{
		mysqldump->scp_system_string =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			mysqldump_scp_system_string(
				mysqldump->mysqldump_input->database,
				mysqldump->
					mysqldump_input->
					offsite_scp_destination,
				mysqldump->
					mysqldump_consolidate_tar_filename );
	}

	return mysqldump;
}

MYSQLDUMP *mysqldump_calloc( void )
{
	MYSQLDUMP *mysqldump;

	if ( ! ( mysqldump = calloc( 1, sizeof ( MYSQLDUMP ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return mysqldump;
}

boolean mysqldump_subdirectory_exists_boolean( char *son )
{
	char system_string[ 1024 ];
	char *results;

	if ( !son )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: son is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"stat.e %s 2>/dev/null | wc -l",
		son );

	/* Returns heap memory or null */
	/* --------------------------- */
	results = string_pipe_fetch( system_string );

	return (boolean)atoi( results );
}

char *mysqldump_log_system_string(
		const char *mysqldump_log_script,
		const char *application_log_extension,
		char *database,
		char *log_directory,
		char *date_yyyymmdd,
		char *son )
{
	char system_string[ 1024 ];

	if ( !database )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: database is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !log_directory )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: log_directory is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !date_yyyymmdd )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: date_yyyymmdd is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !son )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: son is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"%s %s %s %s %s %s",
		mysqldump_log_script,
		database,
		log_directory,
		date_yyyymmdd,
		son,
		application_log_extension );

	return strdup( system_string );
}

char *mysqldump_rotate_system_string(
		const char *mysqldump_rotate_script,
		char *date_yyyymmdd,
		char *son,
		char *father,
		char *grandfather,
		char *greatgrandfather,
		char *greatgreatgrandfather )
{
	char system_string[ 1024 ];

	if ( !date_yyyymmdd
	||   !son
	||   !father
	||   !grandfather
	||   !greatgrandfather
	||   !greatgreatgrandfather )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"%s %s %s %s %s %s %s",
		mysqldump_rotate_script,
		date_yyyymmdd,
		son,
		father,
		grandfather,
		greatgrandfather,
		greatgreatgrandfather );

	return strdup( system_string );
}

char *mysqldump_rsync_system_string(
		char *son,
		char *offsite_rsync_destination )
{
	char system_string[ 1024 ];

	if ( !son
	||   !offsite_rsync_destination )
	{
		return (char *)0;
	}

	sprintf(system_string,
		"rsync -aq --delete %s/ %s 1>&2",
		son,
		offsite_rsync_destination );

	return strdup( system_string );
}

char *mysqldump_scp_system_string(
		char *database,
		char *offsite_scp_destination,
		char *mysqldump_consolidate_tar_filename )
{
	char system_string[ 1024 ];

	if ( !database
	||   !offsite_scp_destination
	||   !mysqldump_consolidate_tar_filename )
	{
		return (char *)0;
	}

	sprintf(system_string,
		"rsync -aq %s %s%s.tar.gz 1>&2",
		mysqldump_consolidate_tar_filename,
		offsite_scp_destination,
		database );

	return strdup( system_string );
}

char *mysqldump_filename(
		char *date_yyyymmdd,
		char *table_name,
		boolean is_gzip )
{
	char filename[ 128 ];

	if ( !date_yyyymmdd
	||   !table_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( is_gzip )
	{
		sprintf(filename,
			"%s_%s.sql.gz",
			table_name,
			date_yyyymmdd );
	}
	else
	{
		sprintf(filename,
			"%s_%s.sql",
			table_name,
			date_yyyymmdd );
	}

	return strdup( filename );
}

char *mysqldump_system_string(
		char *database,
		char *mysql_user,
		char *password_filename,
		char *table_name,
		boolean is_line )
{
	char system_string[ 1024 ];
	char *ptr = system_string;

	if ( !database
	||   !mysql_user
	||   !password_filename
	||   !table_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf( ptr,
		"nice mysqldump "
		"--defaults-extra-file=%s "
		"-u %s "
		"--force "
		"--quick "
		"--add-drop-table "
		"--complete-insert ",
		password_filename,
		mysql_user );

		if ( is_line )
			ptr += sprintf( ptr, "--skip-extended-insert " );
		else
			ptr += sprintf( ptr, "--extended-insert " );

	ptr += sprintf( ptr,
		"%s %s",
		database,
		table_name );

	return strdup( system_string );
}

FORK_CONTROL *mysqldump_fork_control(
		int parallel_count,
		LIST *mysqldump_separate_block_list,
		LIST *mysqldump_separate_line_list,
		LIST *mysqldump_consolidate_list )
{
	FORK_CONTROL *fork_control;
	MYSQLDUMP_SEPARATE *mysqldump_separate;
	MYSQLDUMP_CONSOLIDATE *mysqldump_consolidate;

	fork_control = fork_control_new( parallel_count );

	if ( list_rewind( mysqldump_separate_block_list ) )
	do {
		mysqldump_separate =
			list_get(
				mysqldump_separate_block_list );

		fork_control_process_set(
			fork_control->
				fork_control_process_list /* in/out */,
			mysqldump_separate->system_string
				/* command_line */ );

	} while ( list_next( mysqldump_separate_block_list ) );

	if ( list_rewind( mysqldump_separate_line_list ) )
	do {
		mysqldump_separate =
			list_get(
				mysqldump_separate_line_list );

		fork_control_process_set(
			fork_control->
				fork_control_process_list /* in/out */,
			mysqldump_separate->system_string
				/* command_line */ );

	} while ( list_next( mysqldump_separate_line_list ) );

	if ( list_rewind( mysqldump_consolidate_list ) )
	do {
		mysqldump_consolidate =
			list_get(
				mysqldump_consolidate_list );

		fork_control_process_set(
			fork_control->
				fork_control_process_list /* in/out */,
			mysqldump_consolidate->system_string
				/* command_line */ );

	} while ( list_next( mysqldump_consolidate_list ) );

	return fork_control;
}

char *mysqldump_remove_password_system_string(
		char *password_filename )
{
	static char system_string[ 128 ];

	if ( !password_filename )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: password_filename is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"rm %s",
		password_filename );

	return system_string;
}

char *mysqldump_mkdir_system_string(
		const char *mysqldump_mkdir_script,
		char *database,
		char *backup_directory )
{
	static char system_string[ 256 ];

	if ( !database
	||   !backup_directory )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"%s %s %s",
		mysqldump_mkdir_script,
		database,
		backup_directory );

	return system_string;
}

char *mysqldump_last_run_filename(
		char *mysqldump_count_prefix,
		char *date_yyyymmdd,
		char *son )
{
	char system_string[ 1024 ];

	if ( !mysqldump_count_prefix
	||   !date_yyyymmdd
	||   !son )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"ls -1 %s/%s_*.dat 2>/dev/null | grep -v %s | tail -1",
		son,
		mysqldump_count_prefix,
		date_yyyymmdd );

	/* Returns heap memory or null */
	/* --------------------------- */
	return string_pipe_fetch( system_string );
}

char *mysqldump_last_run_yyyymmdd( char *mysqldump_last_run_filename )
{
	int length;
	static char yyyymmdd[ 9 ];

	if ( !mysqldump_last_run_filename )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: mysqldump_last_run_filename is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	length = strlen( mysqldump_last_run_filename );

	if ( length < 12 )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: strlen(%s) has length < 12\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			mysqldump_last_run_filename );
		exit( 1 );
	}

	return
	string_strncpy(
		yyyymmdd /* destination */,
		mysqldump_last_run_filename + ( length - 12 ) /* source */,
		8 /* count */ );
}

LIST *mysqldump_last_run_list(
		char mysqldump_delimiter,
		char *mysqldump_last_run_filename )
{
	LIST *list;
	FILE *input_file;
	char input[ 256 ];
	char table_name[ 128 ];
	char row_count_string[ 128 ];

	if ( !mysqldump_delimiter
	||   !mysqldump_last_run_filename )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parmameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	input_file =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_file(
			mysqldump_last_run_filename );

	list = list_new();

	while ( string_input( input, input_file, sizeof ( input ) ) )
	{
		piece(	table_name,
			mysqldump_delimiter,
			input,
			0 );

		if ( !piece(
			row_count_string,
			mysqldump_delimiter,
			input,
			1 ) )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: invalid input=[%s].\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				input );

			fclose( input_file );
			exit( 1 );
		}

		list_set(
			list,
			mysqldump_last_run_new(
				strdup( table_name ),
				string_atoi( row_count_string ) ) );
	}

	fclose( input_file );

	return list;
}

MYSQLDUMP_LAST_RUN *mysqldump_last_run_new(
		char *table_name,
		int row_count )
{
	MYSQLDUMP_LAST_RUN *mysqldump_last_run;

	if ( !table_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: table_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	mysqldump_last_run = mysqldump_last_run_calloc();

	mysqldump_last_run->table_name = table_name;
	mysqldump_last_run->row_count = row_count;

	return mysqldump_last_run;
}

MYSQLDUMP_LAST_RUN *mysqldump_last_run_calloc( void )
{
	MYSQLDUMP_LAST_RUN *mysqldump_last_run;

	if ( ! ( mysqldump_last_run =
			calloc( 1,
				sizeof ( MYSQLDUMP_LAST_RUN ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return mysqldump_last_run;
}

int mysqldump_last_run_row_count(
		LIST *mysqldump_last_run_list,
		char *table_name )
{
	MYSQLDUMP_LAST_RUN *mysqldump_last_run;
	int row_count = 0;

	if ( !table_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: table_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ( mysqldump_last_run =
		mysqldump_last_run_seek(
			mysqldump_last_run_list,
			table_name ) ) )
	{
		row_count = mysqldump_last_run->row_count;
	}

	return row_count;
}

MYSQLDUMP_LAST_RUN *mysqldump_last_run_seek(
		LIST *mysqldump_last_run_list,
		char *table_name )
{
	MYSQLDUMP_LAST_RUN *mysqldump_last_run;

	if ( !table_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: table_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( list_rewind( mysqldump_last_run_list ) )
	do {
		mysqldump_last_run =
			list_get(
				mysqldump_last_run_list );

		if ( strcmp(
			mysqldump_last_run->table_name,
			table_name ) == 0 )
		{
			return mysqldump_last_run;
		}

	} while ( list_next( mysqldump_last_run_list ) );

	return NULL;
}

LIST *mysqldump_this_run_list(
		char mysqldump_delimiter,
		char *database,
		char *mysqldump_this_run_filename,
		LIST *mysqldump_last_run_list )
{
	LIST *list;
	char *system_string;
	FILE *input_pipe;
	char input[ 256 ];
	char table_name[ 128 ];
	char row_count_string[ 128 ];

	if ( !mysqldump_delimiter
	||   !database
	||   !mysqldump_this_run_filename )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	list = list_new();

	environment_database_set( database );

	system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		mysqldump_this_run_system_string(
			mysqldump_this_run_filename );

	input_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_pipe(
			system_string );

	while ( string_input( input, input_pipe, 256 ) )
	{
		piece(
			table_name,
			mysqldump_delimiter,
			input,
			0 );

		if ( !piece(
			row_count_string,
			mysqldump_delimiter,
			input,
			1 ) )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: piece(%s,1) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				input );

			pclose( input_pipe );
			exit( 1 );
		}

		list_set(
			list,
			mysqldump_this_run_new(
				mysqldump_last_run_list,
				strdup( table_name ),
				string_atoi( row_count_string ) ) );
	}

	pclose( input_pipe );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

MYSQLDUMP_THIS_RUN *mysqldump_this_run_new(
		LIST *mysqldump_last_run_list,
		char *table_name,
		int row_count )
{
	MYSQLDUMP_THIS_RUN *mysqldump_this_run;

	if ( !table_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: table_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	mysqldump_this_run = mysqldump_this_run_calloc();

	mysqldump_this_run->table_name = table_name;
	mysqldump_this_run->row_count = row_count;

	mysqldump_this_run->percentage_drop =
		mysqldump_this_run_percentage_drop(
			mysqldump_last_run_list,
			table_name,
			row_count );

	return mysqldump_this_run;
}

MYSQLDUMP_THIS_RUN *mysqldump_this_run_calloc( void )
{
	MYSQLDUMP_THIS_RUN *mysqldump_this_run;

	if ( ! ( mysqldump_this_run =
			calloc( 1,
				sizeof ( MYSQLDUMP_THIS_RUN ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return mysqldump_this_run;
}

double mysqldump_this_run_percentage_drop(
		LIST *mysqldump_last_run_list,
		char *table_name,
		int row_count )
{
	int last_run_row_count;
	int row_count_increase;

	if ( ! ( last_run_row_count =
			mysqldump_last_run_row_count(
				mysqldump_last_run_list,
				table_name ) ) )
	{
		return 0.0;
	}

	row_count_increase =
		row_count -
		last_run_row_count;

	if ( row_count_increase >= 0 ) return 0.0;

	return
	(double)-row_count_increase /
	(double)last_run_row_count;
}

char *mysqldump_this_run_threshold_display(
		const double mysqldump_drop_threshold,
		char *database,
		LIST *mysqldump_this_run_list )
{
	LIST *name_list =
		mysqldump_this_run_threshold_name_list(
			mysqldump_drop_threshold,
			mysqldump_this_run_list );

	if ( list_length( name_list ) )
	{
		char display[ 4096 ];

		snprintf(
			display,
			sizeof ( display ),
			"%s: Row count dropped below %d percent: %s",
			database,
			(int)(mysqldump_drop_threshold * 100.0),
			list_display_delimited(
				name_list,
				',' ) );

		return strdup( display );
	}
	else
	{
		return NULL;
	}
}

LIST *mysqldump_this_run_threshold_name_list(
		const double mysqldump_drop_threshold,
		LIST *mysqldump_this_run_list )
{
	LIST *list = list_new();
	MYSQLDUMP_THIS_RUN *mysqldump_this_run;

	if ( list_rewind( mysqldump_this_run_list ) )
	do {
		mysqldump_this_run =
			list_get(
				mysqldump_this_run_list );

		if (	mysqldump_this_run->percentage_drop >=
			mysqldump_drop_threshold )
		{
			list_set(
				list,
				mysqldump_this_run->table_name );
		}

	} while ( list_next( mysqldump_this_run_list ) );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

LIST *mysqldump_separate_list(
		char *database,
		char *mysql_user,
		char *password_filename,
		char *date_yyyymmdd,
		LIST *separate_table_list,
		boolean is_line )
{
	LIST *list = list_new();
	char *table_name;

	if ( !database
	||   !mysql_user
	||   !password_filename
	||   !date_yyyymmdd )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( list_rewind( separate_table_list ) )
	do {
		table_name = list_get( separate_table_list );

		list_set(
			list,
			mysqldump_separate_new(
				database,
				mysql_user,
				password_filename,
				date_yyyymmdd,
				table_name,
				is_line ) );

	} while ( list_next( separate_table_list ) );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

MYSQLDUMP_SEPARATE *mysqldump_separate_new(
		char *database,
		char *mysql_user,
		char *password_filename,
		char *date_yyyymmdd,
		char *table_name,
		boolean is_line )
{
	MYSQLDUMP_SEPARATE *mysqldump_separate;

	if ( !database
	||   !mysql_user
	||   !password_filename
	||   !date_yyyymmdd
	||   !table_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	mysqldump_separate = mysqldump_separate_calloc();

	mysqldump_separate->mysqldump_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		mysqldump_system_string(
			database,
			mysql_user,
			password_filename,
			table_name,
			is_line );

	mysqldump_separate->mysqldump_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		mysqldump_filename(
			date_yyyymmdd,
			table_name,
			1 /* is_gzip */ );

	mysqldump_separate->system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		mysqldump_separate_system_string(
			mysqldump_separate->mysqldump_system_string,
			mysqldump_separate->mysqldump_filename );

	return mysqldump_separate;
}

MYSQLDUMP_SEPARATE *mysqldump_separate_calloc( void )
{
	MYSQLDUMP_SEPARATE *mysqldump_separate;

	if ( ! ( mysqldump_separate =
			calloc( 1,
				sizeof ( MYSQLDUMP_SEPARATE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return mysqldump_separate;
}

char *mysqldump_separate_system_string(
		char *mysqldump_system_string,
		char *mysqldump_filename )
{
	char system_string[ 2048 ];

	if ( !mysqldump_system_string
	||   !mysqldump_filename )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"%s | gzip > %s",
		mysqldump_system_string,
		mysqldump_filename );

	return strdup( system_string );
}

LIST *mysqldump_consolidate_list(
		char *database,
		char *mysql_user,
		char *password_filename,
		char *date_yyyymmdd,
		LIST *separate_block_table_list,
		LIST *separate_line_table_list,
		LIST *exclude_table_list,
		LIST *mysqldump_this_run_list )
{
	LIST *list = list_new();
	MYSQLDUMP_THIS_RUN *mysqldump_this_run;

	if ( !database
	||   !mysql_user
	||   !password_filename
	||   !date_yyyymmdd )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( list_rewind( mysqldump_this_run_list ) )
	do {
		mysqldump_this_run =
			list_get(
				mysqldump_this_run_list );

		if ( list_exists_string(
				mysqldump_this_run->table_name,
				separate_block_table_list ) )
		{
			continue;
		}

		if ( list_exists_string(
				mysqldump_this_run->table_name,
				separate_line_table_list ) )
		{
			continue;
		}

		if ( list_exists_string(
				mysqldump_this_run->table_name,
				exclude_table_list ) )
		{
			continue;
		}

		list_set(
			list,
			mysqldump_consolidate_new(
				database,
				mysql_user,
				password_filename,
				date_yyyymmdd,
				mysqldump_this_run->table_name ) );

	} while ( list_next( mysqldump_this_run_list ) );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

MYSQLDUMP_CONSOLIDATE *mysqldump_consolidate_new(
		char *database,
		char *mysql_user,
		char *password_filename,
		char *date_yyyymmdd,
		char *table_name )
{
	MYSQLDUMP_CONSOLIDATE *mysqldump_consolidate;

	if ( !database
	||   !mysql_user
	||   !password_filename
	||   !date_yyyymmdd
	||   !table_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	mysqldump_consolidate = mysqldump_consolidate_calloc();

	mysqldump_consolidate->mysqldump_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		mysqldump_system_string(
			database,
			mysql_user,
			password_filename,
			table_name,
			1 /* is_line */ );

	mysqldump_consolidate->mysqldump_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		mysqldump_filename(
			date_yyyymmdd,
			table_name,
			0 /* not is_gzip */ );

	mysqldump_consolidate->system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		mysqldump_consolidate_system_string(
			mysqldump_consolidate->mysqldump_system_string,
			mysqldump_consolidate->mysqldump_filename );
	
	return mysqldump_consolidate;
}

MYSQLDUMP_CONSOLIDATE *mysqldump_consolidate_calloc( void )
{
	MYSQLDUMP_CONSOLIDATE *mysqldump_consolidate;

	if ( ! ( mysqldump_consolidate =
			calloc( 1,
				sizeof ( MYSQLDUMP_CONSOLIDATE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return mysqldump_consolidate;
}

char *mysqldump_consolidate_system_string(
		char *mysqldump_system_string,
		char *mysqldump_filename )
{
	char system_string[ 2048 ];

	if ( !mysqldump_system_string
	||   !mysqldump_filename )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"%s > %s",
		mysqldump_system_string,
		mysqldump_filename );

	return strdup( system_string );
}

char *mysqldump_consolidate_tar_filename(
		char *database,
		char *date_yyyymmdd )
{
	char filename[ 128 ];

	if ( !database
	||   !date_yyyymmdd )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(filename,
		"%s_%s.tar.gz",
		database,
		date_yyyymmdd );

	return strdup( filename );
}

char *mysqldump_consolidate_tar_system_string(
		LIST *mysqldump_consolidate_list,
		char *mysqldump_consolidate_tar_filename )
{
	char system_string[ 65536 ];
	char *ptr = system_string;
	MYSQLDUMP_CONSOLIDATE *mysqldump_consolidate;

	if ( !mysqldump_consolidate_tar_filename )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: mysqldump_consolidate_tar_filename is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( mysqldump_consolidate_list ) ) return (char *)0;

	ptr += sprintf(
		ptr,
		"tar -czf %s",
		mysqldump_consolidate_tar_filename );

	do {
		mysqldump_consolidate =
			list_get(
				mysqldump_consolidate_list );

		ptr += sprintf( ptr,
			" %s",
			mysqldump_consolidate->mysqldump_filename );

	} while ( list_next( mysqldump_consolidate_list ) );

	return strdup( system_string );
}

char *mysqldump_consolidate_rm_system_string(
		LIST *mysqldump_consolidate_list )
{
	char system_string[ 65536 ];
	char *ptr = system_string;
	MYSQLDUMP_CONSOLIDATE *mysqldump_consolidate;

	if ( !list_rewind( mysqldump_consolidate_list ) ) return (char *)0;

	ptr += sprintf( ptr, "rm" );

	do {
		mysqldump_consolidate =
			list_get(
				mysqldump_consolidate_list );

		ptr += sprintf( ptr,
			" %s",
			mysqldump_consolidate->mysqldump_filename );

	} while ( list_next( mysqldump_consolidate_list ) );

	return strdup( system_string );
}

MYSQLDUMP_INPUT *mysqldump_input_fetch(
		char *argv_0,
		char *config_filename )
{
	MYSQLDUMP_INPUT *mysqldump_input;

	if ( !argv_0
	||   !config_filename )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	mysqldump_input = mysqldump_input_calloc();

	mysqldump_input_parse(
		config_filename,
		&mysqldump_input->database /* out */,
		&mysqldump_input->parallel_count /* out */,
		&mysqldump_input->separate_block_table_list_string /* out */,
		&mysqldump_input->separate_line_table_list_string /* out */,
		&mysqldump_input->exclude_table_list_string /* out */,
		&mysqldump_input->offsite_rsync_destination /* out */,
		&mysqldump_input->offsite_scp_destination  /* out */ );

	mysqldump_input_appaserver_parse(
		mysqldump_input->database,
		&mysqldump_input->mysql_user /* out */,
		&mysqldump_input->password /* out */,
		&mysqldump_input->log_directory /* out */,
		&mysqldump_input->backup_directory /* out */ );

	mysqldump_input->password_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		mysqldump_input_password_filename(
			MYSQLDUMP_PASSWORD_FILE_SCRIPT
				/* mysqldump_password_file.sh */,
			mysqldump_input->password );

	mysqldump_input->date_yyyymmdd =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		mysqldump_input_date_yyyymmdd();

	mysqldump_input->basename =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		mysqldump_input_basename(
			argv_0 );

	mysqldump_input->son =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		mysqldump_input_son(
			mysqldump_input->database,
			mysqldump_input->backup_directory );

	mysqldump_input->father =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		mysqldump_input_father(
			mysqldump_input->database,
			mysqldump_input->backup_directory );

	mysqldump_input->grandfather =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		mysqldump_input_grandfather(
			mysqldump_input->database,
			mysqldump_input->backup_directory );

	mysqldump_input->greatgrandfather =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		mysqldump_input_greatgrandfather(
			mysqldump_input->database,
			mysqldump_input->backup_directory );

	mysqldump_input->greatgreatgrandfather =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		mysqldump_input_greatgreatgrandfather(
			mysqldump_input->database,
			mysqldump_input->backup_directory );

	if ( mysqldump_input->separate_block_table_list_string )
	{
		mysqldump_input->separate_block_table_list =
			mysqldump_input_separate_block_table_list(
				mysqldump_input->
					separate_block_table_list_string );
	}

	if ( mysqldump_input->separate_line_table_list_string )
	{
		mysqldump_input->separate_line_table_list =
			mysqldump_input_separate_line_table_list(
				mysqldump_input->
					separate_line_table_list_string );
	}

	if ( mysqldump_input->exclude_table_list_string )
	{
		mysqldump_input->exclude_table_list =
			mysqldump_input_exclude_table_list(
				mysqldump_input->exclude_table_list_string );
	}

	mysqldump_input->mysqldump_this_run_filename =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		mysqldump_this_run_filename(
			MYSQLDUMP_COUNT_PREFIX,
			mysqldump_input->date_yyyymmdd,
			mysqldump_input->son );

	return mysqldump_input;
}

MYSQLDUMP_INPUT *mysqldump_input_calloc( void )
{
	MYSQLDUMP_INPUT *mysqldump_input;

	if ( ! ( mysqldump_input = calloc( 1, sizeof ( MYSQLDUMP_INPUT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return mysqldump_input;
}

void mysqldump_input_parse(
		char *config_filename,
		char **database /* out */,
		int *parallel_count /* out */,
		char **separate_block_table_list_string /* out */,
		char **separate_line_table_list_string /* out */,
		char **exclude_table_list_string /* out */,
		char **offsite_rsync_destination /* out */,
		char **offsite_scp_destination /* out */ )
{
	FILE *input_file;
	char input[ 256 ];
	char label[ 128 ];
	char value[ 128 ];

	input_file =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_file(
			config_filename );

	while ( string_input( input, input_file, 256 ) )
	{
		if ( !*input ) continue;
		if ( *input == '#' ) continue;
		piece( label, '=', input, 0 );
		if ( !piece( value, '=', input, 1 ) ) continue;
		if ( !*value ) continue;

		if ( strcmp( label, "database" ) == 0 )
			*database = strdup( value );
		else
		if ( strcmp( label, "parallel_count" ) == 0 )
			*parallel_count = atoi( value );
		else
		if ( strcmp( label, "separate_block_table_list" ) == 0 )
			*separate_block_table_list_string = strdup( value );
		else
		if ( strcmp( label, "separate_line_table_list" ) == 0 )
			*separate_line_table_list_string = strdup( value );
		else
		if ( strcmp( label, "exclude_table_list" ) == 0 )
			*exclude_table_list_string = strdup( value );
		else
		if ( strcmp( label, "offsite_rsync_destination" ) == 0 )
			*offsite_rsync_destination = strdup( value );
		else
		if ( strcmp( label, "offsite_scp_destination" ) == 0 )
			*offsite_scp_destination = strdup( value );
	}

	fclose( input_file );
}

void mysqldump_input_appaserver_parse(
		char *database,
		char **mysql_user,
		char **password,
		char **log_directory,
		char **backup_directory )
{
	APPASERVER_PARAMETER *appaserver_parameter;

	appaserver_parameter =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_parameter_application(
			database /* application_name */ );

	*mysql_user = appaserver_parameter->mysql_user;
	*password = appaserver_parameter->password;
	*log_directory = appaserver_parameter->log_directory;
	*backup_directory = appaserver_parameter->backup_directory;
}

char *mysqldump_input_date_yyyymmdd( void )
{
	return
	/* --------------------- */
	/* Returns static memory */
	/* --------------------- */
	date_display_yyyymmdd(
		date_now_new(
			date_utc_offset() ) );
}

char *mysqldump_input_basename( char *argv_0 )
{
	if ( !argv_0 )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: argv_0 is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	/* --------------------- */
	/* Returns static memory */
	/* --------------------- */
	basename_base_name(
		argv_0,
		0 /* not strip_extension */ );
}

char *mysqldump_input_son(
		char *database,
		char *backup_directory )
{
	static char son[ 128 ];

	if ( !database )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: database is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !backup_directory )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: backup_directory is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(son,
		"%s/%s/son",
		backup_directory,
		database );

	return son;
}

char *mysqldump_input_father(
		char *database,
		char *backup_directory )
{
	static char father[ 128 ];

	if ( !database
	||   !backup_directory )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(father,
		"%s/%s/father",
		backup_directory,
		database );

	return father;
}

char *mysqldump_input_grandfather(
		char *database,
		char *backup_directory )
{
	static char grandfather[ 128 ];

	if ( !database
	||   !backup_directory )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(grandfather,
		"%s/%s/grandfather",
		backup_directory,
		database );

	return grandfather;
}

char *mysqldump_input_greatgrandfather(
		char *database,
		char *backup_directory )
{
	static char greatgrandfather[ 128 ];

	if ( !database
	||   !backup_directory )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(greatgrandfather,
		"%s/%s/greatgrandfather",
		backup_directory,
		database );

	return greatgrandfather;
}

char *mysqldump_input_greatgreatgrandfather(
		char *database,
		char *backup_directory )
{
	static char greatgreatgrandfather[ 128 ];

	if ( !database
	||   !backup_directory )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(greatgreatgrandfather,
		"%s/%s/greatgreatgrandfather",
		backup_directory,
		database );

	return greatgreatgrandfather;
}

LIST *mysqldump_input_separate_block_table_list(
		char *separate_block_table_list_string )
{
	if ( !separate_block_table_list_string ) return NULL;

	return
	list_string_list(
		separate_block_table_list_string,
		',' /* delimiter */ );
}

LIST *mysqldump_input_separate_line_table_list(
		char *separate_line_table_list_string )
{
	if ( !separate_line_table_list_string ) return NULL;

	return
	list_string_list(
		separate_line_table_list_string,
		',' /* delimiter */ );
}

LIST *mysqldump_input_exclude_table_list(
		char *exclude_table_list_string )
{
	if ( !exclude_table_list_string ) return NULL;

	return
	list_string_list(
		exclude_table_list_string,
		',' /* delimiter */ );
}

char *mysqldump_input_password_filename(
		const char *mysqldump_password_file_script,
		char *mysql_password )
{
	char *password_filename;
	FILE *output_file;

	password_filename =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_pipe_fetch(
			(char *)mysqldump_password_file_script );

	if ( !password_filename )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: string_pipe_fetch(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			mysqldump_password_file_script );
		exit( 1 );
	}

	output_file =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_output_file(
			password_filename );

	fprintf(output_file,
		"[mysqldump]\npassword=%s\n",
		mysql_password );

	fclose( output_file );

	return password_filename;
}

char *mysqldump_this_run_filename(
		const char *mysqldump_count_prefix,
		char *mysqldump_input_date_yyyymmdd,
		char *mysqldump_input_son )
{
	static char filename[ 128 ];

	if ( !mysqldump_input_date_yyyymmdd
	||   !mysqldump_input_son )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		filename,
		sizeof( filename ),
		"%s/%s_%s.dat",
		mysqldump_input_son,
		(char *)mysqldump_count_prefix,
		mysqldump_input_date_yyyymmdd );

	return filename;
}

char *mysqldump_this_run_system_string( char *mysqldump_this_run_filename )
{
	static char system_string[ 256 ];

	if ( !mysqldump_this_run_filename )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: filename is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"table_count.e | "
		"sort_delimited_numbers.e '=' y | "
		"tee %s",
		mysqldump_this_run_filename );

	return system_string;
}

char *mysqldump_this_run_display_system_string(
		char *database,
		char *mysqldump_this_run_filename )
{
	static char system_string[ 128 ];

	if ( !database
	||   !mysqldump_this_run_filename )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"/bin/echo -e \"\\nDatabase: %s\"; cat %s",
		database,
		mysqldump_this_run_filename );

	return system_string;
}

