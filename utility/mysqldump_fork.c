/* $APPASERVER_HOME/utility/mysqldump_fork.c */
/* ----------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fork_control.h"
#include "list.h"
#include "boolean.h"
#include "column.h"
#include "folder.h"
#include "timlib.h"

/* Constants */
/* --------- */
/* -------------------------------------------------------------------- */
/* Synchronize with:							*/
/* $APPASERVER_HOME/src_appaserver/appaserver_mysqldump_database.sh	*/
/* -------------------------------------------------------------------- */
#define MYSQLDUMP_FORK_BACKUP_DIRECTORY   "/var/backups/appaserver"

#define INSUFFICIENT_SPACE_MESSAGE "Insufficient filesystem space."
#define AUDIT_DELIMITER ','
#define DIFF_WARNING_MESSAGE \
	"Warning: backup count differs from database count."
#define COUNT_DROP_WARNING_MESSAGE \
	"Warning: backup count dropped significantly from last run"

#define MYSQL_DUMP_TEMPLATE "rm %s 2>/dev/null; touch %s; chmod o-r %s 2>/dev/null; nice -9 mysqldump --defaults-extra-file=%s -u%s --extended-insert=TRUE --force --quick --add-drop-table %s %s | %s >> %s"

#define MYSQL_LINE_DUMP_TEMPLATE "rm %s 2>/dev/null; touch %s; chmod o-r %s 2>/dev/null; nice -9 mysqldump --defaults-extra-file=%s -u%s --extended-insert=FALSE --force --quick --add-drop-table %s %s | %s >> %s"

/* Prototypes */
/* ---------- */
long int get_needed_megabytes(		char *output_directory,
					char *latest_date );

boolean filesystem_enough_space(	char *output_directory );

void output_count_results(	char *mysqldump_database_count_file,
				char *mysqldump_datafile_count_file,
				char *mysqldump_database_yesterday_file );

void build_datafile_count_file(		char *mysqldump_datafile_count_file,
					char *output_directory,
					LIST *table_name_list,
					LIST *big_table_name_list,
					LIST *exclude_table_name_list,
					char *now_yyyymmdd );

boolean output_database_count(		char *mysqldump_database_count_file,
					LIST *table_name_list );

char *get_mysqldump_filename(		char *database,
					char *date_yyyymmdd,
					char *inner_key );

/* Returns static memory */
/* --------------------- */
char *get_filename(			char *table_name,
					char *now_yyyymmdd );

LIST *get_filename_list(
					LIST *table_name_list,
					char *now_yyyymmdd,
					LIST *big_table_name_list,
					LIST *exclude_table_name_list );

boolean tar_small_files(
					LIST *table_name_list,
					char *database,
					char *output_directory,
					char *now_yyyymmdd,
					LIST *big_table_name_list,
					LIST *exclude_table_name_list );

LIST *get_process_list(			LIST *process_list,
					LIST *table_name_list,
					char *mysqluser,
					char *database,
					char *mysql_password_file,
					char *output_directory,
					char *now_yyyymmdd,
					LIST *big_table_name_list,
					LIST *exclude_table_name_list,
					boolean each_line_insert );

int main( int argc, char **argv )
{
	FORK_CONTROL *fork_control;
	char *mysqluser;
	char *database;
	char *mysql_password_file;
	char *output_directory;
	int processes_in_parallel;
	LIST *table_name_list;
	boolean each_line_insert;
	LIST *big_table_name_list = {0};
	LIST *exclude_table_name_list = {0};
	char *now_yyyymmdd;
	char *yesterday_yyyymmdd;
	char *mysqldump_database_count_file;
	char *mysqldump_datafile_count_file;
	char *mysqldump_database_yesterday_count_file;

	if ( argc < 7 )
	{
		fprintf( stderr,
"Usage: %s mysqluser database mysql_password_file output_directory processes_in_parallel each_line_insert_yn [big_table_name_list] [exclude_table_name_list]\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	mysqluser = argv[ 1 ];
	database = argv[ 2 ];
	mysql_password_file = argv[ 3 ];
	output_directory = argv[ 4 ];

	if ( !filesystem_enough_space( output_directory ) )
	{
		fprintf( stderr, "%s.\n", INSUFFICIENT_SPACE_MESSAGE );
		exit( 1 );
	}

	processes_in_parallel = atoi( argv[ 5 ] );
	each_line_insert = ( *argv[ 6 ] == 'y' );

	if ( argc >= 8
	&&   *argv[ 7 ]
	&&   strcmp(	argv[ 7 ],
			"big_table_name_list" ) != 0 )
	{
		big_table_name_list = list_string2list( argv[ 7 ], ',' );
	}

	if ( argc >= 9
	&&   *argv[ 8 ]
	&&   strcmp(	argv[ 8 ],
			"exclude_table_name_list" ) != 0 )
	{
		exclude_table_name_list = list_string2list( argv[ 8 ], ',' );
	}

	if ( ! ( table_name_list = folder_get_table_name_list( database ) )
	||   !   list_length( table_name_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s: cannot get table_name_list.\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	table_name_list =
		list_subtract_list(
			table_name_list,
			exclude_table_name_list );

	now_yyyymmdd = pipe2string( "now.sh yyyymmdd" );
	yesterday_yyyymmdd = pipe2string( "now.sh yyyymmdd -1" );

	mysqldump_database_count_file =
		get_mysqldump_filename(
			database,
			now_yyyymmdd,
			"database" /* inner_key */ );

	mysqldump_datafile_count_file =
		get_mysqldump_filename(
			database,
			now_yyyymmdd,
			"datafile" /* inner_key */ );

	mysqldump_database_yesterday_count_file =
		get_mysqldump_filename(
			database,
			yesterday_yyyymmdd,
			"database" /* inner_key */ );

	output_database_count(
		mysqldump_database_count_file,
		table_name_list );

	fork_control = fork_control_new();

	fork_control->processes_in_parallel = processes_in_parallel;

	now_yyyymmdd = pipe2string( "date '+%Y%m%d'" );

	if ( ! ( fork_control->process_list =
			get_process_list(
				fork_control->process_list,
				table_name_list,
				mysqluser,
				database,
				mysql_password_file,
				output_directory,
				now_yyyymmdd,
				big_table_name_list,
				exclude_table_name_list,
				each_line_insert ) )
	||   !list_length( fork_control->process_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s: cannot get process_list.\n",
			 argv[ 0 ] );
		exit( 1 );
	}

/* fork_control_process_list_display( fork_control->process_list ); */

	fork_control_execute(
		fork_control->process_list,
		fork_control->processes_in_parallel );

	/* Must happen before tar_small_files() */
	/* ------------------------------------ */
	if ( each_line_insert )
	{
		build_datafile_count_file(
					mysqldump_datafile_count_file,
					output_directory,
					table_name_list,
					big_table_name_list,
					exclude_table_name_list,
					now_yyyymmdd );
	}

	if ( !tar_small_files(
		table_name_list,
		database,
		output_directory,
		now_yyyymmdd,
		big_table_name_list,
		exclude_table_name_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s: cannot tar_small_files.\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	if ( each_line_insert
	&&    timlib_file_exists( mysqldump_database_count_file )
	&&    timlib_file_exists( mysqldump_datafile_count_file )
	&&    timlib_file_exists( mysqldump_database_yesterday_count_file ) )
	{
		output_count_results(
				mysqldump_database_count_file,
				mysqldump_datafile_count_file,
				mysqldump_database_yesterday_count_file );
	}

	return 0;

} /* main() */

LIST *get_process_list(	LIST *process_list,
			LIST *table_name_list,
			char *mysqluser,
			char *database,
			char *mysql_password_file,
			char *output_directory,
			char *now_yyyymmdd,
			LIST *big_table_name_list,
			LIST *exclude_table_name_list,
			boolean each_line_insert )
{
	char *table_name;
	char command_line[ 2048 ];
	FORK_CONTROL_PROCESS *process;
	char *filename;
	char path_filename_extension[ 512 ];
	char *gzip_process;

	if ( !list_rewind( table_name_list ) ) return (LIST *)0;

	do {
		table_name = list_get_pointer( table_name_list );

		if ( list_exists_string(
			exclude_table_name_list,
			table_name ) )
		{
			continue;
		}

/*
#define MYSQL_DUMP_TEMPLATE "rm %s 2>/dev/null; touch %s && chmod o-r %s && nice -9 mysqldump --defaults-extra-file=%s -u%s --extended-insert=FALSE --force --quick --add-drop-table %s %s | %s >> %s"
*/

		/* Returns static memory */
		/* --------------------- */
		filename = get_filename( table_name, now_yyyymmdd );

		if ( list_exists_string( big_table_name_list, table_name ) )
		{
			sprintf( path_filename_extension,
				 "%s/%s.gz",
				 output_directory,
				 filename );
			gzip_process = "gzip";
		}
		else
		{
			sprintf( path_filename_extension,
				 "%s/%s",
				 output_directory,
				 filename );
			gzip_process = "cat";
		}

		if ( each_line_insert )
		{
			sprintf(command_line,
			 	MYSQL_LINE_DUMP_TEMPLATE,
			 	path_filename_extension,
			 	path_filename_extension,
			 	path_filename_extension,
			 	mysql_password_file,
			 	mysqluser,
			 	database,
			 	table_name,
			 	gzip_process,
			 	path_filename_extension );
		}
		else
		{
			sprintf(command_line,
			 	MYSQL_DUMP_TEMPLATE,
			 	path_filename_extension,
			 	path_filename_extension,
			 	path_filename_extension,
			 	mysql_password_file,
			 	mysqluser,
			 	database,
			 	table_name,
			 	gzip_process,
			 	path_filename_extension );
		}

		process = fork_control_process_new( strdup( command_line ) );

		list_append_pointer( process_list, process );

	} while( list_next( table_name_list ) );

	return process_list;

}

LIST *get_filename_list(	LIST *table_name_list,
				char *now_yyyymmdd,
				LIST *big_table_name_list,
				LIST *exclude_table_name_list )
{
	LIST *filename_list;
	char *table_name;

	if ( !list_rewind( table_name_list ) ) return (LIST *)0;

	filename_list = list_new();

	do {
		table_name = list_get_pointer( table_name_list );

		if ( list_exists_string(
			exclude_table_name_list,
			table_name ) )
		{
			continue;
		}

		if ( list_length( big_table_name_list ) )
		{
			if ( list_exists_string(
					big_table_name_list,
					table_name ) )
			{
				continue;
			}
		}

		list_append_pointer(	filename_list,
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					strdup( get_filename(
						table_name,
						now_yyyymmdd ) ) );

	} while( list_next( table_name_list ) );

	return filename_list;

}

char *get_filename(	char *table_name,
			char *now_yyyymmdd )
{
	static char filename[ 128 ];

	sprintf(	filename,
			"%s_%s.sql",
			table_name,
			now_yyyymmdd );

	return filename;

}

boolean tar_small_files(
		LIST *table_name_list,
		char *database,
		char *output_directory,
		char *now_yyyymmdd,
		LIST *big_table_name_list,
		LIST *exclude_table_name_list )
{
	char sys_string[ 65536 ];
	char *filename_list_string;
	char output_file[ 256 ];
	LIST *small_filename_list;

	if ( chdir( output_directory ) != 0 )
	{
		return 0;
	}

	if ( ! ( small_filename_list =
			get_filename_list(
				table_name_list,
				now_yyyymmdd,
				big_table_name_list,
				exclude_table_name_list ) ) )
	{
		return 0;
	}

	sprintf( output_file,
		 "%s_%s.tar.gz",
		 database,
		 now_yyyymmdd );

	filename_list_string =
		list_display_delimited(
			small_filename_list, ' ' );

	sprintf( sys_string,
		 "tar czf %s %s",
		 output_file,
		 filename_list_string );
	if ( system( sys_string ) ) {};

	sprintf( sys_string,
		 "rm -f %s",
		 filename_list_string );
	if ( system( sys_string ) ) {};

	sprintf( sys_string,
		 "chmod o-r %s",
		 output_file );
	if ( system( sys_string ) ) {};

	return 1;

}

char *get_mysqldump_filename(		char *database,
					char *date_yyyymmdd,
					char *inner_key )
{
	char filename[ 128 ];

	sprintf( filename,
		 "%s/%s/son/mysqldump_%s_count_%s_%s.dat",
		 MYSQLDUMP_FORK_BACKUP_DIRECTORY,
		 database,
		 inner_key,
		 database,
		 date_yyyymmdd );

	return strdup( filename );

}

boolean output_database_count(		char *mysqldump_database_count_file,
					LIST *table_name_list )
{
	FILE *output_file;
	char sys_string[ 128 ];
	char *table_name;
	char *results;

	if ( !list_rewind( table_name_list ) ) return 1;

	if ( ! ( output_file = fopen( mysqldump_database_count_file, "w" ) ) )
	{
		fprintf( stderr,
			 "Warning In %s/%s()/%d: cannot open %s for write.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 mysqldump_database_count_file );

		return 0;
	}

	do {
		table_name = list_get_pointer( table_name_list );

		sprintf( sys_string,
			 "echo \"select count(*) from %s;\" | sql.e",
			 table_name );

		results = pipe2string( sys_string );

		fprintf(	output_file,
				"%s%c%s\n",
				table_name,
				AUDIT_DELIMITER,
				results );
		
	} while( list_next( table_name_list ) );

	fclose( output_file );
	return 1;

}

void build_datafile_count_file(		char *mysqldump_datafile_count_file,
					char *output_directory,
					LIST *table_name_list,
					LIST *big_table_name_list,
					LIST *exclude_table_name_list,
					char *now_yyyymmdd )
{
	FILE *output_file;
	char sys_string[ 1024 ];
	char *table_name;
	char *filename;
	char *cat_process;
	char full_filename[ 256 ];

	if ( !list_rewind( table_name_list ) ) return;

	if ( ! ( output_file = fopen( mysqldump_datafile_count_file, "w" ) ) )
	{
		fprintf( stderr,
			 "ERROR In %s/%s()/%d: cannot open %s for write.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 mysqldump_datafile_count_file );
		exit( 1 );
	}

	do {
		table_name = list_get_pointer( table_name_list );

		if ( list_exists_string(
			exclude_table_name_list,
			table_name ) )
		{
			continue;
		}

		/* Returns static memory */
		/* --------------------- */
		filename = get_filename( table_name, now_yyyymmdd );

		if ( list_exists_string( big_table_name_list, table_name ) )
		{
			sprintf( full_filename,
				 "%s/%s.gz",
				 output_directory,
				 filename );

			cat_process = "zcat";
		}
		else
		{
			sprintf( full_filename,
				 "%s/%s",
				 output_directory,
				 filename );

			cat_process = "cat";
		}

		sprintf( sys_string,
			 "%s %s | grep -i '^insert into' | wc -l",
			 cat_process,
			 full_filename );

		fprintf(	output_file,
				"%s%c%s\n",
				table_name,
				AUDIT_DELIMITER,
				pipe2string( sys_string ) );
		
	} while( list_next( table_name_list ) );

	fclose( output_file );

}

void output_count_results(	char *mysqldump_database_count_file,
				char *mysqldump_datafile_count_file,
				char *mysqldump_database_yesterday_count_file )
{
	char sys_string[ 1024 ];
	char *diff_results = {0};
	char *drop_results = {0};

	if ( !timlib_file_exists( mysqldump_database_count_file )
	||   !timlib_file_exists( mysqldump_datafile_count_file )
	||   !timlib_file_exists( mysqldump_database_yesterday_count_file ) )
	{
		return;
	}

	/* Output the warnings messages at the top. */
	/* ---------------------------------------- */
	sprintf(	sys_string,
			"diff %s %s",
			mysqldump_database_count_file,
			mysqldump_datafile_count_file );
	
	if ( ( diff_results = pipe2string( sys_string ) ) )
	{
		printf( "%s\n", DIFF_WARNING_MESSAGE );
	}

	/* Output the table of counts. */
	/* --------------------------  */
	sprintf(	sys_string,
			"join -t'%c' %s %s			|"
			"sort -t'%c' -r -n -k2			 ",
			AUDIT_DELIMITER,
			mysqldump_database_count_file,
			mysqldump_datafile_count_file,
			AUDIT_DELIMITER );

	printf( "table%cdatabase%cbackup\n",
		AUDIT_DELIMITER,
		AUDIT_DELIMITER );

	fflush( stdout );
	if ( system( sys_string ) ) {};
	fflush( stdout );

	/* Output the drop count. */
	/* ---------------------- */
	sprintf(	sys_string,
			"mysqldump_fork_count_drop.e %s %s",
			mysqldump_database_count_file,
			mysqldump_database_yesterday_count_file );

	if ( ( drop_results = pipe2string( sys_string ) ) )
	{
		printf( "%s: %s\n",
			COUNT_DROP_WARNING_MESSAGE,
			drop_results );
	}

}

long int get_needed_megabytes(	char *output_directory,
				char *latest_date )
{
	char sys_string[ 1024 ];
	long int needed_megabytes = 0;
	char input_buffer[ 512 ];
	char megabytes_string[ 64 ];
	FILE *input_pipe;

	sprintf( sys_string,
		 "ls -l --block-size=M %s/*%s*",
		 output_directory,
		 latest_date );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer,input_pipe ) )
	{
		column( megabytes_string, 4, input_buffer );
		needed_megabytes += strtol( megabytes_string, (char **)0, 10 );
	}

	pclose( input_pipe );
	return needed_megabytes;

}

boolean filesystem_enough_space( char *output_directory )
{
	long int available_megabytes = 0;
	long int needed_megabytes;
	char *latest_date;

	timlib_directory_filesystem(
		&available_megabytes,
		output_directory );

	if ( ! ( latest_date =
			timlib_get_latest_filename_datestamp(
				output_directory ) ) )
	{
		fprintf( stderr,
		"Warning in %s(): cannot determine space requirement.\n",
			 __FUNCTION__ );
		return 1;
	}

	needed_megabytes =
		get_needed_megabytes(
			output_directory,
			latest_date );

	if ( needed_megabytes < available_megabytes )
		return 1;
	else
		return 0;

}

