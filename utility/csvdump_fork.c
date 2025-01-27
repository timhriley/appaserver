/* csvdump_fork.c */
/* -------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fork_control.h"
#include "list.h"
#include "boolean.h"
#include "column.h"
#include "timlib.h"

#define AUDIT_DELIMITER ','
#define DIFF_WARNING_MESSAGE \
	"Warning: backup count differs from database count!"

#define CVSDUMP_TEMPLATE "rm %s 2>/dev/null; touch %s; chmod o+r %s 2>/dev/null; nice -9 csvdump.sh %s | %s >> %s"

long int get_needed_megabytes(		char *output_directory,
					char *latest_date );

boolean filesystem_enough_space(	char *output_directory );

void output_audit_results(		char *audit_database_filename,
					char *audit_datafile_filename );

void remove_file(			char *filename );

void output_audit_datafile_count(	char *audit_datafile_filename,
					char *database,
					char *output_directory,
					LIST *table_name_list,
					char *date_stamp );

void output_audit_database_count(	char *audit_database_filename,
					LIST *table_name_list );

char *get_audit_database_filename(	void );

char *get_audit_datafile_filename(	void );

char *get_filename(			char *database,
					char *table_name,
					char *date_stamp );

LIST *get_filename_list(		LIST *table_name_list,
					char *database,
					char *date_stamp );

boolean zip_files(			LIST *table_name_list,
					char *database,
					char *output_directory,
					char *date_stamp );

LIST *get_table_name_list(		char *database );


LIST *get_process_list(			LIST *process_list,
					LIST *table_name_list,
					char *database,
					char *output_directory,
					char *date_stamp );

int main( int argc, char **argv )
{
	FORK_CONTROL *fork_control;
	char *output_directory;
	char *database;
	int processes_in_parallel;
	LIST *table_name_list;
	char *date_stamp;
	char *audit_database_filename;
	char *audit_datafile_filename;

	if ( argc != 4 )
	{
		fprintf( stderr,
"Usage: %s database output_directory processes_in_parallel\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	database = argv[ 1 ];
	output_directory = argv[ 2 ];

	if ( !filesystem_enough_space( output_directory ) )
	{
		fprintf( stderr, "Insufficient filesystem space.\n" );
		exit( 1 );
	}

	processes_in_parallel = atoi( argv[ 3 ] );

	if ( ! ( table_name_list = get_table_name_list( database ) )
	||   !   list_length( table_name_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s: cannot get table_name_list.\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	audit_database_filename = get_audit_database_filename();
	audit_datafile_filename = get_audit_datafile_filename();

	output_audit_database_count( audit_database_filename, table_name_list );

	fork_control = fork_control_new();

	fork_control->processes_in_parallel = processes_in_parallel;

	date_stamp = pipe2string( "date '+%Y%m%d'" );

	if ( ! ( fork_control->process_list =
			get_process_list(
				fork_control->process_list,
				table_name_list,
				database,
				output_directory,
				date_stamp ) )
	||   !list_length( fork_control->process_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s: cannot get process_list.\n",
			 argv[ 0 ] );
		exit( 1 );
	}

/*
fork_control_process_list_display( fork_control->process_list );
*/

	fork_control_execute(
		fork_control->process_list,
		fork_control->processes_in_parallel );

	output_audit_datafile_count(
				audit_datafile_filename,
				database,
				output_directory,
				table_name_list,
				date_stamp );

	if ( !zip_files(
		table_name_list,
		database,
		output_directory,
		date_stamp ) )
	{
		fprintf( stderr,
			 "ERROR in %s: cannot zip_files().\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	output_audit_results(
			audit_database_filename,
			audit_datafile_filename );

	remove_file( audit_database_filename );
	remove_file( audit_datafile_filename );

	return 0;
}

LIST *get_process_list(	LIST *process_list,
			LIST *table_name_list,
			char *database,
			char *output_directory,
			char *date_stamp )
{
	char *table_name;
	char command_line[ 1024 ];
	FORK_CONTROL_PROCESS *process;
	char *filename;
	char path_filename_extension[ 256 ];
	char *gzip_process;

	if ( !list_rewind( table_name_list ) ) return (LIST *)0;

	do {
		table_name = list_get_pointer( table_name_list );

		filename = get_filename( database, table_name, date_stamp );

		sprintf( path_filename_extension,
			 "%s/%s",
			 output_directory,
			 filename );
		gzip_process = "cat";

		sprintf(command_line,
		 	CVSDUMP_TEMPLATE,
		 	path_filename_extension,
		 	path_filename_extension,
		 	path_filename_extension,
		 	table_name,
		 	gzip_process,
		 	path_filename_extension );

		process = fork_control_process_new( strdup( command_line ) );

		list_append_pointer( process_list, process );

	} while( list_next( table_name_list ) );

	return process_list;
}

LIST *get_table_name_list( char *database )
{
	char sys_string[ 512 ];

	sprintf( sys_string,
		 "application_folder_list.sh %s n",
		 database );

	return pipe2list( sys_string );

}

LIST *get_filename_list(	LIST *table_name_list,
				char *database,
				char *date_stamp )
{
	LIST *filename_list;
	char *table_name;

	if ( !list_rewind( table_name_list ) ) return (LIST *)0;

	filename_list = list_new();

	do {
		table_name = list_get_pointer( table_name_list );

		list_append_pointer(	filename_list,
					strdup( get_filename(
						database,
						table_name,
						date_stamp ) ) );

	} while( list_next( table_name_list ) );

	return filename_list;
}

char *get_filename(	char *database,
			char *table_name,
			char *date_stamp )
{
	static char filename[ 128 ];

	sprintf(	filename,
			"%s_%s_%s.csv",
			database,
			table_name,
			date_stamp );

	return filename;
}

boolean zip_files(
		LIST *table_name_list,
		char *database,
		char *output_directory,
		char *date_stamp )
{
	char sys_string[ 65536 ];
	char *filename_list_string;
	char output_file[ 128 ];
	LIST *filename_list;

	if ( chdir( output_directory ) != 0 )
	{
		return 0;
	}

	if ( ! ( filename_list =
			get_filename_list(
				table_name_list,
				database,
				date_stamp ) ) )
	{
		return 0;
	}

	sprintf( output_file,
		 "%s_%s.zip",
		 database,
		 date_stamp );

	filename_list_string =
		list_display_delimited(
			filename_list, ' ' );

	sprintf( sys_string,
		 "zip %s %s",
		 output_file,
		 filename_list_string );
	if ( system( sys_string ) ){}

	sprintf( sys_string,
		 "rm -f %s",
		 filename_list_string );
	if ( system( sys_string ) ){}

	sprintf( sys_string,
		 "chmod o+r %s",
		 output_file );
	if ( system( sys_string ) ){}

	return 1;
}

char *get_audit_database_filename( void )
{
	static char audit_database_filename[ 128 ];

	sprintf( audit_database_filename,
		 "/tmp/csvdump_fork_audit_database_%d",
		 getpid() );

	return audit_database_filename;
}

char *get_audit_datafile_filename( void )
{
	static char audit_datafile_filename[ 128 ];

	sprintf( audit_datafile_filename,
		 "/tmp/csvdump_fork_audit_datafile_%d",
		 getpid() );

	return audit_datafile_filename;
}

void output_audit_database_count(	char *audit_database_filename,
					LIST *table_name_list )
{
	FILE *output_file;
	char sys_string[ 128 ];
	char *table_name;

	if ( !list_rewind( table_name_list ) ) return;

	if ( ! ( output_file = fopen( audit_database_filename, "w" ) ) )
	{
		fprintf( stderr,
			 "ERROR In %s/%s()/%d: cannot open %s for write.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 audit_database_filename );
		exit( 1 );
	}

	do {
		table_name = list_get_pointer( table_name_list );

		sprintf( sys_string,
			 "echo \"select count(*) from %s;\" | sql.e",
			 table_name );

		fprintf(	output_file,
				"%s%c%s\n",
				table_name,
				AUDIT_DELIMITER,
				pipe2string( sys_string ) );
		
	} while( list_next( table_name_list ) );

	fclose( output_file );
}

void output_audit_datafile_count(	char *audit_datafile_filename,
					char *database,
					char *output_directory,
					LIST *table_name_list,
					char *date_stamp )
{
	FILE *output_file;
	char sys_string[ 128 ];
	char *table_name;
	char *filename;
	char *cat_process;
	char full_filename[ 128 ];

	if ( !list_rewind( table_name_list ) ) return;

	if ( ! ( output_file = fopen( audit_datafile_filename, "w" ) ) )
	{
		fprintf( stderr,
			 "ERROR In %s/%s()/%d: cannot open %s for write.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 audit_datafile_filename );
		exit( 1 );
	}

	do {
		table_name = list_get_pointer( table_name_list );

		filename = get_filename( database, table_name, date_stamp );

		sprintf( full_filename,
			 "%s/%s",
			 output_directory,
			 filename );

		cat_process = "cat";

		sprintf( sys_string,
			 "%s %s | skip_lines.e 1 | wc -l",
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

void remove_file( char *filename )
{
	char sys_string[ 128 ];

	sprintf( sys_string, "rm -f %s", filename );
	if ( system( sys_string ) ){}
}

void output_audit_results(	char *audit_database_filename,
				char *audit_datafile_filename )
{
	char sys_string[ 1024 ];
	char *diff_results;

	sprintf(	sys_string,
			"diff %s %s",
			audit_database_filename,
			audit_datafile_filename );

	if ( ( diff_results = pipe2string( sys_string ) ) )
		printf( "%s\n", DIFF_WARNING_MESSAGE );

	sprintf(	sys_string,
			"join -t'%c' %s %s			|"
			"sort -t'%c' -r -n -k2			 ",
			AUDIT_DELIMITER,
			audit_database_filename,
			audit_datafile_filename,
			AUDIT_DELIMITER );

	printf( "table%cdatabase%cbackup\n",
		AUDIT_DELIMITER,
		AUDIT_DELIMITER );

	fflush( stdout );
	if ( system( sys_string ) ){}
	fflush( stdout );

	if ( diff_results )
		printf( "%s\n", DIFF_WARNING_MESSAGE );
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

