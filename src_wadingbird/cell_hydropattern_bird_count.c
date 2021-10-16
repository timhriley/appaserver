/* ---------------------------------------------------	*/
/* src_wadingbird/cell_hydropattern_bird_count.c	*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "grace.h"
#include "appaserver_parameter_file.h"
#include "document.h"
#include "timlib.h"
#include "piece.h"
#include "column.h"
#include "date.h"
#include "wadingbird_library.h"
#include "environ.h"
#include "date_convert.h"
#include "application.h"
#include "process.h"
#include "appaserver_link.h"

/* Structures */
/* ---------- */

/* Constants */
/* --------- */
#define HYDROPATTERN_COUNT		 4
#define BIRD_COUNT_BUCKET_SIZE	 	25
#define BIRD_ESTIMATION_BUCKET_SIZE    100
#define BUCKET_COUNT    		 8

/* Prototypes */
/* ---------- */
void fill_bucket_legends(
			char **bucket_1_legend,
			char **bucket_2_legend,
			char **bucket_3_legend,
			char **bucket_4_legend,
			char **bucket_5_legend,
			char **bucket_6_legend,
			char **bucket_7_legend,
			char **bucket_8_legend,
			int bucket_size,
			char *label );

void output_chart(	char *application_name,
			char *document_root_directory,
			char *initial_takeoff_date,
			LIST *cell_list,
			char *bird_count_estimation );

void output_hydropattern_grace_header(
			FILE *output_pipe,
			char *date_string,
			char *bucket_1_legend,
			char *bucket_2_legend,
			char *bucket_3_legend,
			char *bucket_4_legend,
			char *bucket_5_legend,
			char *bucket_6_legend,
			char *bucket_7_legend,
			char *bucket_8_legend );

void populate_bird_count(
			LIST *cell_list,
			char *application_name,
			char *initial_takeoff_date,
			char *common_name,
			char *bird_count_estimation );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char buffer[ 128 ];
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	char *initial_takeoff_date;
	char *common_name_list_string;
	char *bird_count_estimation;
	char *output_medium;
	HYDROPATTERN_MERGE *hydropattern_merge;

	if ( argc != 7 )
	{
		fprintf( stderr, 
"Usage: %s application process initial_takeoff_date common_name bird_count_estimation output_medium\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	initial_takeoff_date = argv[ 3 ];
	common_name_list_string = argv[ 4 ];
	bird_count_estimation = argv[ 5 ];
	output_medium = argv[ 6 ];

	if ( strcmp( bird_count_estimation, "bird_count_estimation" ) == 0 )
		bird_count_estimation = "count";

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	add_dot_to_path();
	add_local_bin_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

	appaserver_parameter_file = new_appaserver_parameter_file();

	document = document_new( "", application_name );
	document_set_output_content_type( document );

	document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(
			document->application_name,
			document->onload_control_string );

	printf( "<h2>%s for %s<br>\n",
		format_initial_capital( buffer, process_name ),
		initial_takeoff_date );
	fflush( stdout );
	if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) );
	printf( "</h2>\n" );
	fflush( stdout );

	if ( !*initial_takeoff_date
	||   strcmp( initial_takeoff_date,
		     "initial_takeoff_date" ) == 0 )
	{
		printf(
	"<h3>Please select a systematic reconnaissance flight.</h3>\n" );
		document_close();
		exit( 0 );
	}

	hydropattern_merge = wadingbird_get_hydropattern_merge(
				application_name,
				initial_takeoff_date );

	if ( !hydropattern_merge->preferred_hydropattern_seat )
	{
		printf(
"<h3>Error: No preferred seat position is assigned for this SRF.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( !wadingbird_exists_cell_hydropattern_for_seat(
				application_name,
				initial_takeoff_date,
				hydropattern_merge->
					preferred_hydropattern_seat ) )
	{
		printf(
"<h3>Error: No cell hydropatterns exist for the preferred seat position for this SRF. The preferred seat position assigned to this SRF is %s.</h3>\n",
			hydropattern_merge->preferred_hydropattern_seat );
		document_close();
		exit( 0 );
	}

	if ( !list_length( hydropattern_merge->cell_list ) )
	{
		printf(
"<h3>Warning: no cells have hydropatterns from this seat position.</h3>\n" );
		document_close();
		exit( 0 );
	}

	populate_bird_count(
			hydropattern_merge->cell_list,
			application_name,
			initial_takeoff_date,
			common_name_list_string,
			bird_count_estimation );

	if ( strcmp( output_medium, "table" ) == 0
	||   strcmp( output_medium, "output_medium" ) == 0 )
	{
		FILE *output_pipe;
		char sys_string[ 1024 ];
		SRF_CELL *cell;
		char heading[ 128 ];

		sprintf(	heading,
				"cell_number,hydropattern,bird_%s",
				bird_count_estimation );

		sprintf( sys_string,
			 "html_table.e '' %s '|' %s",
			 heading,
			 "right,left,right,left" );

		output_pipe = popen( sys_string, "w" );

		if ( list_rewind( hydropattern_merge->cell_list ) )
		{
			do {
				cell = list_get_pointer(
					hydropattern_merge->cell_list );

				fprintf( output_pipe,
					 "%d|%s|%d\n",
					 cell->cell_number,
					 cell->hydropattern,
					 cell->bird_count );
			} while( list_next( 
				hydropattern_merge->cell_list ) );
		}
		pclose( output_pipe );
	}
	else
	if ( strcmp( output_medium, "text_file" ) == 0 )
	{
		FILE *output_pipe;
		char sys_string[ 1024 ];
		SRF_CELL *cell;
		FILE *output_file;
		char *output_filename;
		char *ftp_filename;
		APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
		int process_id = getpid();
		APPASERVER_LINK *appaserver_link;
	
		appaserver_parameter_file = appaserver_parameter_file_new();

		appaserver_link =
			appaserver_link_new(
				application_http_prefix( application_name ),
				appaserver_library_server_address(),
				( application_prepend_http_protocol_yn(
					application_name ) == 'y' ),
	 			appaserver_parameter_file->
					document_root,
				process_name /* filename_stem */,
				application_name,
				process_id,
				(char *)0 /* session */,
				(char *)0 /* begin_date_string */,
				(char *)0 /* end_date_string */,
				"csv" );

		output_filename = appaserver_link->output->filename;

		ftp_filename = appaserver_link->prompt->filename;

		if ( ! ( output_file = fopen( output_filename, "w" ) ) )
		{
			printf(
			"<h3>ERROR: Cannot open output file %s</h3>\n",
				output_filename );
			document_close();
			exit( 0 );
		}
		else
		{
			fclose( output_file );
		}
	
		sprintf( sys_string,
			 "cat > %s",
			 output_filename );
		output_pipe = popen( sys_string, "w" );

		if ( list_rewind( hydropattern_merge->cell_list ) )
		{
			do {
				cell = list_get_pointer(
					hydropattern_merge->cell_list );

				fprintf( output_pipe,
					 "%d,%s,%d\n",
					 cell->cell_number,
					 cell->hydropattern,
					 cell->bird_count );
			} while( list_next( 
				hydropattern_merge->cell_list ) );
		}
		pclose( output_pipe );

		appaserver_library_output_ftp_prompt(
			ftp_filename,
	"&lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

	}
	else
	if ( strcmp( output_medium, "chart" ) == 0
	||   strcmp( output_medium, "map" ) == 0 )
	{
		output_chart(	application_name,
				appaserver_parameter_file->
					document_root,
				initial_takeoff_date,
				hydropattern_merge->cell_list,
				bird_count_estimation );
	}

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );

	exit( 0 );
} /* main() */

void populate_bird_count(
			LIST *cell_list,
			char *application_name,
			char *initial_takeoff_date,
			char *common_name_list_string,
			char *bird_count_estimation )
{
	char sys_string[ 2048 ];
	char where[ 2048 ];
	SRF_CELL *cell;
	char input_buffer[ 1024 ];
	char cell_number_string[ 128 ];
	char bird_count_string[ 128 ];
	FILE *input_pipe;
	char *select;
	char *folder;

	if ( !*common_name_list_string
	||   strcmp( common_name_list_string, "common_name" ) == 0 )
	{
		sprintf(where,
			"initial_takeoff_date = '%s'",
			initial_takeoff_date );
	}
	else
	{
		sprintf(where,
			"initial_takeoff_date = '%s' and common_name in (%s)",
			initial_takeoff_date,
			timlib_get_in_clause( common_name_list_string ) );
	}

	if ( strcmp( bird_count_estimation, "count" ) == 0 )
		select = "cell_number,bird_count";
	else
		select = "cell_number,bird_estimation";

	folder = "bird_observation";

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=%s		"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 folder,
		 where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	cell_number_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		if ( ( cell = wadingbird_merge_seek_cell(
					cell_list,
					atoi( cell_number_string ) ) ) )
		{
			piece(	bird_count_string,
				FOLDER_DATA_DELIMITER,
				input_buffer,
				1 );
			cell->bird_count += atoi( bird_count_string );
		}
	}
	pclose( input_pipe );
} /* populate_bird_count() */

void output_hydropattern_grace_header(	FILE *output_pipe,
					char *date_string,
					char *bucket_1_legend,
					char *bucket_2_legend,
					char *bucket_3_legend,
					char *bucket_4_legend,
					char *bucket_5_legend,
					char *bucket_6_legend,
					char *bucket_7_legend,
					char *bucket_8_legend )
{
	fprintf( output_pipe,
"# Grace project file\n"
"#\n"
"@version 50121\n"
"@page size 612, 792\n"
"@page scroll 5%c\n"
"@page inout 5%c\n"
"@link page off\n"
"@map font 0 to \"Times-Roman\", \"Times-Roman\"\n"
"@map font 1 to \"Times-Italic\", \"Times-Italic\"\n"
"@map font 2 to \"Times-Bold\", \"Times-Bold\"\n"
"@map font 3 to \"Times-BoldItalic\", \"Times-BoldItalic\"\n"
"@map font 4 to \"Helvetica\", \"Helvetica\"\n"
"@map font 5 to \"Helvetica-Oblique\", \"Helvetica-Oblique\"\n"
"@map font 6 to \"Helvetica-Bold\", \"Helvetica-Bold\"\n"
"@map font 7 to \"Helvetica-BoldOblique\", \"Helvetica-BoldOblique\"\n"
"@map font 8 to \"Courier\", \"Courier\"\n"
"@map font 9 to \"Courier-Oblique\", \"Courier-Oblique\"\n"
"@map font 10 to \"Courier-Bold\", \"Courier-Bold\"\n"
"@map font 11 to \"Courier-BoldOblique\", \"Courier-BoldOblique\"\n"
"@map font 12 to \"Symbol\", \"Symbol\"\n"
"@map font 13 to \"ZapfDingbats\", \"ZapfDingbats\"\n"
"@map color 0 to (255, 255, 255), \"white\"\n"
"@map color 1 to (0, 0, 0), \"black\"\n"
"@map color 2 to (99, 0, 0), \"brown1\"\n"
"@map color 3 to (132, 115, 8), \"brown2\"\n"
"@map color 4 to (255, 255, 0), \"yellow\"\n"
"@map color 5 to (0, 255, 255), \"cyan\"\n"
"@map color 6 to (0, 0, 255), \"blue\"\n"
"@map color 7 to (255, 0, 255), \"magenta\"\n"
"@map color 8 to (255, 0, 0), \"red\"\n"
"@map color 9 to (0, 255, 0), \"green\"\n"
"@map color 10 to (220, 220, 220), \"grey\"\n"
"@map color 11 to (148, 0, 211), \"violet\"\n"
"@map color 12 to (114, 33, 188), \"indigo\"\n"
"@map color 13 to (103, 7, 72), \"maroon\"\n"
"@map color 14 to (64, 224, 208), \"turquoise\"\n"
"@map color 15 to (0, 139, 0), \"green4\"\n"
"@reference date 0\n"
"@date wrap off\n"
"@date wrap year 1950\n"
"@default linewidth 1.0\n"
"@default linestyle 1\n"
"@default color 1\n"
"@default pattern 1\n"
"@default font 0\n"
"@default char size 1.000000\n"
"@default symbol size 1.000000\n"
"@default sformat \"%c.8g\"\n"
"@background color 0\n"
"@page background fill on\n"
"@timestamp off\n"
"@timestamp 0.03, 0.03\n"
"@timestamp color 1\n"
"@timestamp rot 0\n"
"@timestamp font 0\n"
"@timestamp char size 1.000000\n"
"@timestamp def \"Mon Jan 11 12:50:34 2010\"\n"
"@r0 off\n"
"@link r0 to g0\n"
"@r0 type above\n"
"@r0 linestyle 1\n"
"@r0 linewidth 1.0\n"
"@r0 color 1\n"
"@r0 line 0, 0, 0, 0\n"
"@r1 off\n"
"@link r1 to g0\n"
"@r1 type above\n"
"@r1 linestyle 1\n"
"@r1 linewidth 1.0\n"
"@r1 color 1\n"
"@r1 line 0, 0, 0, 0\n"
"@r2 off\n"
"@link r2 to g0\n"
"@r2 type above\n"
"@r2 linestyle 1\n"
"@r2 linewidth 1.0\n"
"@r2 color 1\n"
"@r2 line 0, 0, 0, 0\n"
"@r3 off\n"
"@link r3 to g0\n"
"@r3 type above\n"
"@r3 linestyle 1\n"
"@r3 linewidth 1.0\n"
"@r3 color 1\n"
"@r3 line 0, 0, 0, 0\n"
"@r4 off\n"
"@link r4 to g0\n"
"@r4 type above\n"
"@r4 linestyle 1\n"
"@r4 linewidth 1.0\n"
"@r4 color 1\n"
"@r4 line 0, 0, 0, 0\n"
"@g0 on\n"
"@g0 hidden false\n"
"@g0 type XY\n"
"@g0 stacked false\n"
"@g0 bar hgap 0.000000\n"
"@g0 fixedpoint off\n"
"@g0 fixedpoint type 0\n"
"@g0 fixedpoint xy 0.000000, 0.000000\n"
"@g0 fixedpoint format general general\n"
"@g0 fixedpoint prec 8, 8\n"
"@with g0\n"
"@    world 460000, 2770000, 580000, 2850000\n"
"@    stack world 0, 0, 0, 0\n"
"@    znorm 1\n"
"@    view 0.090000, 0.500000, 0.950000, 1.150000\n"
"@    title \"WadingBird Cell Hydropattern\"\n"
"@    title font 0\n"
"@    title size 1.500000\n"
"@    title color 1\n"
"@    subtitle \"%s\"\n"
"@    subtitle font 0\n"
"@    subtitle size 1.000000\n"
"@    subtitle color 1\n"
"@    xaxes scale Normal\n"
"@    yaxes scale Normal\n"
"@    xaxes invert off\n"
"@    yaxes invert off\n"
"@    xaxis  on\n"
"@    xaxis  type zero false\n"
"@    xaxis  offset 0.000000 , 0.000000\n"
"@    xaxis  bar on\n"
"@    xaxis  bar color 1\n"
"@    xaxis  bar linestyle 1\n"
"@    xaxis  bar linewidth 1.0\n"
"@    xaxis  label \"\"\n"
"@    xaxis  label layout para\n"
"@    xaxis  label place auto\n"
"@    xaxis  label char size 1.000000\n"
"@    xaxis  label font 0\n"
"@    xaxis  label color 1\n"
"@    xaxis  label place normal\n"
"@    xaxis  tick on\n"
"@    xaxis  tick major 20000\n"
"@    xaxis  tick minor ticks 1\n"
"@    xaxis  tick default 6\n"
"@    xaxis  tick place rounded true\n"
"@    xaxis  tick in\n"
"@    xaxis  tick major size 1.000000\n"
"@    xaxis  tick major color 1\n"
"@    xaxis  tick major linewidth 1.0\n"
"@    xaxis  tick major linestyle 1\n"
"@    xaxis  tick major grid off\n"
"@    xaxis  tick minor color 1\n"
"@    xaxis  tick minor linewidth 1.0\n"
"@    xaxis  tick minor linestyle 1\n"
"@    xaxis  tick minor grid off\n"
"@    xaxis  tick minor size 0.500000\n"
"@    xaxis  ticklabel on\n"
"@    xaxis  ticklabel format general\n"
"@    xaxis  ticklabel prec 5\n"
"@    xaxis  ticklabel formula \"\"\n"
"@    xaxis  ticklabel append \"\"\n"
"@    xaxis  ticklabel prepend \"\"\n"
"@    xaxis  ticklabel angle 0\n"
"@    xaxis  ticklabel skip 0\n"
"@    xaxis  ticklabel stagger 0\n"
"@    xaxis  ticklabel place normal\n"
"@    xaxis  ticklabel offset auto\n"
"@    xaxis  ticklabel offset 0.000000 , 0.010000\n"
"@    xaxis  ticklabel start type auto\n"
"@    xaxis  ticklabel start 0.000000\n"
"@    xaxis  ticklabel stop type auto\n"
"@    xaxis  ticklabel stop 0.000000\n"
"@    xaxis  ticklabel char size 1.000000\n"
"@    xaxis  ticklabel font 0\n"
"@    xaxis  ticklabel color 1\n"
"@    xaxis  tick place both\n"
"@    xaxis  tick spec type both\n"
"@    xaxis  tick spec 12\n"
"@    xaxis  tick major 0, 460000\n"
"@    xaxis  ticklabel 0, \"0\"\n"
"@    xaxis  tick minor 1, 470000\n"
"@    xaxis  tick major 2, 480000\n"
"@    xaxis  ticklabel 2, \"27\"\n"
"@    xaxis  tick minor 3, 490000\n"
"@    xaxis  tick major 4, 500000\n"
"@    xaxis  ticklabel 4, \"54\"\n"
"@    xaxis  tick minor 5, 510000\n"
"@    xaxis  tick major 6, 520000\n"
"@    xaxis  ticklabel 6, \"81\"\n"
"@    xaxis  tick minor 7, 530000\n"
"@    xaxis  tick major 8, 540000\n"
"@    xaxis  ticklabel 8, \"108\"\n"
"@    xaxis  tick minor 9, 550000\n"
"@    xaxis  tick major 10, 560000\n"
"@    xaxis  ticklabel 10, \"135\"\n"
"@    xaxis  tick minor 11, 570000\n"
"@    yaxis  on\n"
"@    yaxis  type zero false\n"
"@    yaxis  offset 0.000000 , 0.000000\n"
"@    yaxis  bar on\n"
"@    yaxis  bar color 1\n"
"@    yaxis  bar linestyle 1\n"
"@    yaxis  bar linewidth 1.0\n"
"@    yaxis  label \"\"\n"
"@    yaxis  label layout para\n"
"@    yaxis  label place auto\n"
"@    yaxis  label char size 1.000000\n"
"@    yaxis  label font 0\n"
"@    yaxis  label color 1\n"
"@    yaxis  label place normal\n"
"@    yaxis  tick on\n"
"@    yaxis  tick major 20000\n"
"@    yaxis  tick minor ticks 1\n"
"@    yaxis  tick default 6\n"
"@    yaxis  tick place rounded true\n"
"@    yaxis  tick in\n"
"@    yaxis  tick major size 1.000000\n"
"@    yaxis  tick major color 1\n"
"@    yaxis  tick major linewidth 1.0\n"
"@    yaxis  tick major linestyle 1\n"
"@    yaxis  tick major grid off\n"
"@    yaxis  tick minor color 1\n"
"@    yaxis  tick minor linewidth 1.0\n"
"@    yaxis  tick minor linestyle 1\n"
"@    yaxis  tick minor grid off\n"
"@    yaxis  tick minor size 0.500000\n"
"@    yaxis  ticklabel on\n"
"@    yaxis  ticklabel format general\n"
"@    yaxis  ticklabel prec 5\n"
"@    yaxis  ticklabel formula \"\"\n"
"@    yaxis  ticklabel append \"\"\n"
"@    yaxis  ticklabel prepend \"\"\n"
"@    yaxis  ticklabel angle 0\n"
"@    yaxis  ticklabel skip 0\n"
"@    yaxis  ticklabel stagger 0\n"
"@    yaxis  ticklabel place normal\n"
"@    yaxis  ticklabel offset auto\n"
"@    yaxis  ticklabel offset 0.000000 , 0.010000\n"
"@    yaxis  ticklabel start type auto\n"
"@    yaxis  ticklabel start 0.000000\n"
"@    yaxis  ticklabel stop type auto\n"
"@    yaxis  ticklabel stop 0.000000\n"
"@    yaxis  ticklabel char size 1.000000\n"
"@    yaxis  ticklabel font 0\n"
"@    yaxis  ticklabel color 1\n"
"@    yaxis  tick place both\n"
"@    yaxis  tick spec type both\n"
"@    yaxis  tick spec 11\n"
"@    yaxis  tick minor 0, 2760000\n"
"@    yaxis  tick minor 1, 2770000\n"
"@    yaxis  tick major 2, 2780000\n"
"@    yaxis  ticklabel 2, \"89\"\n"
"@    yaxis  tick minor 3, 2790000\n"
"@    yaxis  tick major 4, 2800000\n"
"@    yaxis  ticklabel 4, \"79\"\n"
"@    yaxis  tick minor 5, 2810000\n"
"@    yaxis  tick major 6, 2820000\n"
"@    yaxis  ticklabel 6, \"69\"\n"
"@    yaxis  tick minor 7, 2830000\n"
"@    yaxis  tick major 8, 2840000\n"
"@    yaxis  ticklabel 8, \"59\"\n"
"@    yaxis  tick minor 9, 2850000\n"
"@    yaxis  tick minor 10, 2860000\n"
"@    altxaxis  off\n"
"@    altyaxis  off\n"
"@    legend on\n"
"@    legend loctype view\n"
"@    legend 0.2, 0.4\n"
"@    legend box color 1\n"
"@    legend box pattern 1\n"
"@    legend box linewidth 1.0\n"
"@    legend box linestyle 1\n"
"@    legend box fill color 9\n"
"@    legend box fill pattern 1\n"
"@    legend font 0\n"
"@    legend char size 1.000000\n"
"@    legend color 1\n"
"@    legend length 4\n"
"@    legend vgap 1\n"
"@    legend hgap 1\n"
"@    legend invert false\n"
"@    frame type 0\n"
"@    frame linestyle 1\n"
"@    frame linewidth 1.0\n"
"@    frame color 1\n"
"@    frame pattern 1\n"
"@    frame background color 0\n"
"@    frame background pattern 0\n"
"@    s0 hidden false\n"
"@    s0 type xy\n"
"@    s0 symbol 2\n"
"@    s0 symbol size 0.800000\n"
"@    s0 symbol color 2\n"
"@    s0 symbol pattern 1\n"
"@    s0 symbol fill color 2\n"
"@    s0 symbol fill pattern 1\n"
"@    s0 symbol linewidth 1.0\n"
"@    s0 symbol linestyle 1\n"
"@    s0 symbol char 65\n"
"@    s0 symbol char font 0\n"
"@    s0 symbol skip 0\n"
"@    s0 line type 0\n"
"@    s0 line linestyle 1\n"
"@    s0 line linewidth 1.0\n"
"@    s0 line color 1\n"
"@    s0 line pattern 1\n"
"@    s0 baseline type 0\n"
"@    s0 baseline off\n"
"@    s0 dropline off\n"
"@    s0 fill type 0\n"
"@    s0 fill rule 0\n"
"@    s0 fill color 1\n"
"@    s0 fill pattern 1\n"
"@    s0 avalue off\n"
"@    s0 avalue type 2\n"
"@    s0 avalue char size 1.000000\n"
"@    s0 avalue font 0\n"
"@    s0 avalue color 1\n"
"@    s0 avalue rot 0\n"
"@    s0 avalue format general\n"
"@    s0 avalue prec 3\n"
"@    s0 avalue prepend \"\"\n"
"@    s0 avalue append \"\"\n"
"@    s0 avalue offset 0.000000 , 0.000000\n"
"@    s0 errorbar on\n"
"@    s0 errorbar place both\n"
"@    s0 errorbar color 4\n"
"@    s0 errorbar pattern 1\n"
"@    s0 errorbar size 1.000000\n"
"@    s0 errorbar linewidth 1.0\n"
"@    s0 errorbar linestyle 1\n"
"@    s0 errorbar riser linewidth 1.0\n"
"@    s0 errorbar riser linestyle 1\n"
"@    s0 errorbar riser clip off\n"
"@    s0 errorbar riser clip length 0.100000\n"
"@    s0 comment \"/home/kevin/projects/wading_bird/srf_grid_utm_xy.dat\"\n"
"@    s0 legend  \"DD\"\n"
"@    s1 hidden false\n"
"@    s1 type xy\n"
"@    s1 symbol 2\n"
"@    s1 symbol size 0.800000\n"
"@    s1 symbol color 3\n"
"@    s1 symbol pattern 1\n"
"@    s1 symbol fill color 3\n"
"@    s1 symbol fill pattern 1\n"
"@    s1 symbol linewidth 1.0\n"
"@    s1 symbol linestyle 1\n"
"@    s1 symbol char 65\n"
"@    s1 symbol char font 0\n"
"@    s1 symbol skip 0\n"
"@    s1 line type 0\n"
"@    s1 line linestyle 1\n"
"@    s1 line linewidth 1.0\n"
"@    s1 line color 1\n"
"@    s1 line pattern 1\n"
"@    s1 baseline type 0\n"
"@    s1 baseline off\n"
"@    s1 dropline off\n"
"@    s1 fill type 0\n"
"@    s1 fill rule 0\n"
"@    s1 fill color 1\n"
"@    s1 fill pattern 1\n"
"@    s1 avalue off\n"
"@    s1 avalue type 2\n"
"@    s1 avalue char size 1.000000\n"
"@    s1 avalue font 0\n"
"@    s1 avalue color 1\n"
"@    s1 avalue rot 0\n"
"@    s1 avalue format general\n"
"@    s1 avalue prec 3\n"
"@    s1 avalue prepend \"\"\n"
"@    s1 avalue append \"\"\n"
"@    s1 avalue offset 0.000000 , 0.000000\n"
"@    s1 errorbar on\n"
"@    s1 errorbar place both\n"
"@    s1 errorbar color 4\n"
"@    s1 errorbar pattern 1\n"
"@    s1 errorbar size 1.000000\n"
"@    s1 errorbar linewidth 1.0\n"
"@    s1 errorbar linestyle 1\n"
"@    s1 errorbar riser linewidth 1.0\n"
"@    s1 errorbar riser linestyle 1\n"
"@    s1 errorbar riser clip off\n"
"@    s1 errorbar riser clip length 0.100000\n"
"@    s1 comment \"srf_grid.agr\"\n"
"@    s1 legend  \"DT\"\n"
"@    s2 hidden false\n"
"@    s2 type xy\n"
"@    s2 symbol 2\n"
"@    s2 symbol size 0.800000\n"
"@    s2 symbol color 4\n"
"@    s2 symbol pattern 1\n"
"@    s2 symbol fill color 4\n"
"@    s2 symbol fill pattern 1\n"
"@    s2 symbol linewidth 1.0\n"
"@    s2 symbol linestyle 1\n"
"@    s2 symbol char 65\n"
"@    s2 symbol char font 0\n"
"@    s2 symbol skip 0\n"
"@    s2 line type 0\n"
"@    s2 line linestyle 1\n"
"@    s2 line linewidth 1.0\n"
"@    s2 line color 1\n"
"@    s2 line pattern 1\n"
"@    s2 baseline type 0\n"
"@    s2 baseline off\n"
"@    s2 dropline off\n"
"@    s2 fill type 0\n"
"@    s2 fill rule 0\n"
"@    s2 fill color 1\n"
"@    s2 fill pattern 1\n"
"@    s2 avalue off\n"
"@    s2 avalue type 2\n"
"@    s2 avalue char size 1.000000\n"
"@    s2 avalue font 0\n"
"@    s2 avalue color 1\n"
"@    s2 avalue rot 0\n"
"@    s2 avalue format general\n"
"@    s2 avalue prec 3\n"
"@    s2 avalue prepend \"\"\n"
"@    s2 avalue append \"\"\n"
"@    s2 avalue offset 0.000000 , 0.000000\n"
"@    s2 errorbar on\n"
"@    s2 errorbar place both\n"
"@    s2 errorbar color 4\n"
"@    s2 errorbar pattern 1\n"
"@    s2 errorbar size 1.000000\n"
"@    s2 errorbar linewidth 1.0\n"
"@    s2 errorbar linestyle 1\n"
"@    s2 errorbar riser linewidth 1.0\n"
"@    s2 errorbar riser linestyle 1\n"
"@    s2 errorbar riser clip off\n"
"@    s2 errorbar riser clip length 0.100000\n"
"@    s2 comment \"srf_grid.agr\"\n"
"@    s2 legend  \"WD\"\n"
"@    s3 hidden false\n"
"@    s3 type xy\n"
"@    s3 symbol 2\n"
"@    s3 symbol size 0.800000\n"
"@    s3 symbol color 5\n"
"@    s3 symbol pattern 1\n"
"@    s3 symbol fill color 5\n"
"@    s3 symbol fill pattern 1\n"
"@    s3 symbol linewidth 1.0\n"
"@    s3 symbol linestyle 1\n"
"@    s3 symbol char 65\n"
"@    s3 symbol char font 0\n"
"@    s3 symbol skip 0\n"
"@    s3 line type 0\n"
"@    s3 line linestyle 1\n"
"@    s3 line linewidth 1.0\n"
"@    s3 line color 1\n"
"@    s3 line pattern 1\n"
"@    s3 baseline type 0\n"
"@    s3 baseline off\n"
"@    s3 dropline off\n"
"@    s3 fill type 0\n"
"@    s3 fill rule 0\n"
"@    s3 fill color 1\n"
"@    s3 fill pattern 1\n"
"@    s3 avalue off\n"
"@    s3 avalue type 2\n"
"@    s3 avalue char size 1.000000\n"
"@    s3 avalue font 0\n"
"@    s3 avalue color 1\n"
"@    s3 avalue rot 0\n"
"@    s3 avalue format general\n"
"@    s3 avalue prec 3\n"
"@    s3 avalue prepend \"\"\n"
"@    s3 avalue append \"\"\n"
"@    s3 avalue offset 0.000000 , 0.000000\n"
"@    s3 errorbar on\n"
"@    s3 errorbar place both\n"
"@    s3 errorbar color 4\n"
"@    s3 errorbar pattern 1\n"
"@    s3 errorbar size 1.000000\n"
"@    s3 errorbar linewidth 1.0\n"
"@    s3 errorbar linestyle 1\n"
"@    s3 errorbar riser linewidth 1.0\n"
"@    s3 errorbar riser linestyle 1\n"
"@    s3 errorbar riser clip off\n"
"@    s3 errorbar riser clip length 0.100000\n"
"@    s3 comment \"srf_grid.agr\"\n"
"@    s3 legend  \"WT\"\n"
"@    s4 hidden false\n"
"@    s4 type xy\n"
"@    s4 symbol 2\n"
"@    s4 symbol size 0.800000\n"
"@    s4 symbol color 6\n"
"@    s4 symbol pattern 1\n"
"@    s4 symbol fill color 6\n"
"@    s4 symbol fill pattern 1\n"
"@    s4 symbol linewidth 1.0\n"
"@    s4 symbol linestyle 1\n"
"@    s4 symbol char 65\n"
"@    s4 symbol char font 0\n"
"@    s4 symbol skip 0\n"
"@    s4 line type 0\n"
"@    s4 line linestyle 1\n"
"@    s4 line linewidth 1.0\n"
"@    s4 line color 1\n"
"@    s4 line pattern 1\n"
"@    s4 baseline type 0\n"
"@    s4 baseline off\n"
"@    s4 dropline off\n"
"@    s4 fill type 0\n"
"@    s4 fill rule 0\n"
"@    s4 fill color 1\n"
"@    s4 fill pattern 1\n"
"@    s4 avalue off\n"
"@    s4 avalue type 2\n"
"@    s4 avalue char size 1.000000\n"
"@    s4 avalue font 0\n"
"@    s4 avalue color 1\n"
"@    s4 avalue rot 0\n"
"@    s4 avalue format general\n"
"@    s4 avalue prec 3\n"
"@    s4 avalue prepend \"\"\n"
"@    s4 avalue append \"\"\n"
"@    s4 avalue offset 0.000000 , 0.000000\n"
"@    s4 errorbar on\n"
"@    s4 errorbar place both\n"
"@    s4 errorbar color 4\n"
"@    s4 errorbar pattern 1\n"
"@    s4 errorbar size 1.000000\n"
"@    s4 errorbar linewidth 1.0\n"
"@    s4 errorbar linestyle 1\n"
"@    s4 errorbar riser linewidth 1.0\n"
"@    s4 errorbar riser linestyle 1\n"
"@    s4 errorbar riser clip off\n"
"@    s4 errorbar riser clip length 0.100000\n"
"@    s4 comment \"srf_grid.agr\"\n"
"@    s4 legend  \"WW\"\n"
"@    s5 hidden false\n"
"@    s5 type xy\n"
"@    s5 symbol 1\n"
"@    s5 symbol size 0.100000\n"
"@    s5 symbol color 0\n"
"@    s5 symbol fill color 0\n"
"@    s5 symbol pattern 1\n"
"@    s5 symbol fill pattern 1\n"
"@    s5 symbol linewidth 1.0\n"
"@    s5 symbol linestyle 1\n"
"@    s5 symbol char 65\n"
"@    s5 symbol char font 0\n"
"@    s5 symbol skip 0\n"
"@    s5 line type 0\n"
"@    s5 line linestyle 1\n"
"@    s5 line linewidth 1.0\n"
"@    s5 line color 1\n"
"@    s5 line pattern 1\n"
"@    s5 baseline type 0\n"
"@    s5 baseline off\n"
"@    s5 dropline off\n"
"@    s5 fill type 0\n"
"@    s5 fill rule 0\n"
"@    s5 fill color 1\n"
"@    s5 fill pattern 1\n"
"@    s5 avalue off\n"
"@    s5 avalue type 2\n"
"@    s5 avalue char size 1.000000\n"
"@    s5 avalue font 0\n"
"@    s5 avalue color 1\n"
"@    s5 avalue rot 0\n"
"@    s5 avalue format general\n"
"@    s5 avalue prec 3\n"
"@    s5 avalue prepend \"\"\n"
"@    s5 avalue append \"\"\n"
"@    s5 avalue offset 0.000000 , 0.000000\n"
"@    s5 errorbar on\n"
"@    s5 errorbar place both\n"
"@    s5 errorbar color 4\n"
"@    s5 errorbar pattern 1\n"
"@    s5 errorbar size 1.000000\n"
"@    s5 errorbar linewidth 1.0\n"
"@    s5 errorbar linestyle 1\n"
"@    s5 errorbar riser linewidth 1.0\n"
"@    s5 errorbar riser linestyle 1\n"
"@    s5 errorbar riser clip off\n"
"@    s5 errorbar riser clip length 0.100000\n"
"@    s5 comment \"srf_grid.agr\"\n"
"@    s5 legend  \"%s\"\n"
"@    s6 hidden false\n"
"@    s6 type xy\n"
"@    s6 symbol 1\n"
"@    s6 symbol size 0.200000\n"
"@    s6 symbol color 0\n"
"@    s6 symbol fill color 0\n"
"@    s6 symbol pattern 1\n"
"@    s6 symbol fill pattern 1\n"
"@    s6 symbol linewidth 1.0\n"
"@    s6 symbol linestyle 1\n"
"@    s6 symbol char 65\n"
"@    s6 symbol char font 0\n"
"@    s6 symbol skip 0\n"
"@    s6 line type 0\n"
"@    s6 line linestyle 1\n"
"@    s6 line linewidth 1.0\n"
"@    s6 line color 1\n"
"@    s6 line pattern 1\n"
"@    s6 baseline type 0\n"
"@    s6 baseline off\n"
"@    s6 dropline off\n"
"@    s6 fill type 0\n"
"@    s6 fill rule 0\n"
"@    s6 fill color 1\n"
"@    s6 fill pattern 1\n"
"@    s6 avalue off\n"
"@    s6 avalue type 2\n"
"@    s6 avalue char size 1.000000\n"
"@    s6 avalue font 0\n"
"@    s6 avalue color 1\n"
"@    s6 avalue rot 0\n"
"@    s6 avalue format general\n"
"@    s6 avalue prec 3\n"
"@    s6 avalue prepend \"\"\n"
"@    s6 avalue append \"\"\n"
"@    s6 avalue offset 0.000000 , 0.000000\n"
"@    s6 errorbar on\n"
"@    s6 errorbar place both\n"
"@    s6 errorbar color 4\n"
"@    s6 errorbar pattern 1\n"
"@    s6 errorbar size 1.000000\n"
"@    s6 errorbar linewidth 1.0\n"
"@    s6 errorbar linestyle 1\n"
"@    s6 errorbar riser linewidth 1.0\n"
"@    s6 errorbar riser linestyle 1\n"
"@    s6 errorbar riser clip off\n"
"@    s6 errorbar riser clip length 0.100000\n"
"@    s6 comment \"srf_grid.agr\"\n"
"@    s6 legend  \"%s\"\n"
"@    s7 hidden false\n"
"@    s7 type xy\n"
"@    s7 symbol 1\n"
"@    s7 symbol size 0.400000\n"
"@    s7 symbol color 0\n"
"@    s7 symbol fill color 0\n"
"@    s7 symbol pattern 1\n"
"@    s7 symbol fill pattern 1\n"
"@    s7 symbol linewidth 1.0\n"
"@    s7 symbol linestyle 1\n"
"@    s7 symbol char 65\n"
"@    s7 symbol char font 0\n"
"@    s7 symbol skip 0\n"
"@    s7 line type 0\n"
"@    s7 line linestyle 1\n"
"@    s7 line linewidth 1.0\n"
"@    s7 line color 1\n"
"@    s7 line pattern 1\n"
"@    s7 baseline type 0\n"
"@    s7 baseline off\n"
"@    s7 dropline off\n"
"@    s7 fill type 0\n"
"@    s7 fill rule 0\n"
"@    s7 fill color 1\n"
"@    s7 fill pattern 1\n"
"@    s7 avalue off\n"
"@    s7 avalue type 2\n"
"@    s7 avalue char size 1.000000\n"
"@    s7 avalue font 0\n"
"@    s7 avalue color 1\n"
"@    s7 avalue rot 0\n"
"@    s7 avalue format general\n"
"@    s7 avalue prec 3\n"
"@    s7 avalue prepend \"\"\n"
"@    s7 avalue append \"\"\n"
"@    s7 avalue offset 0.000000 , 0.000000\n"
"@    s7 errorbar on\n"
"@    s7 errorbar place both\n"
"@    s7 errorbar color 4\n"
"@    s7 errorbar pattern 1\n"
"@    s7 errorbar size 1.000000\n"
"@    s7 errorbar linewidth 1.0\n"
"@    s7 errorbar linestyle 1\n"
"@    s7 errorbar riser linewidth 1.0\n"
"@    s7 errorbar riser linestyle 1\n"
"@    s7 errorbar riser clip off\n"
"@    s7 errorbar riser clip length 0.100000\n"
"@    s7 comment \"srf_grid.agr\"\n"
"@    s7 legend  \"%s\"\n"
"@    s8 hidden false\n"
"@    s8 type xy\n"
"@    s8 symbol 1\n"
"@    s8 symbol size 0.60000\n"
"@    s8 symbol color 0\n"
"@    s8 symbol fill color 0\n"
"@    s8 symbol pattern 1\n"
"@    s8 symbol fill pattern 1\n"
"@    s8 symbol linewidth 1.0\n"
"@    s8 symbol linestyle 1\n"
"@    s8 symbol char 65\n"
"@    s8 symbol char font 0\n"
"@    s8 symbol skip 0\n"
"@    s8 line type 0\n"
"@    s8 line linestyle 1\n"
"@    s8 line linewidth 1.0\n"
"@    s8 line color 1\n"
"@    s8 line pattern 1\n"
"@    s8 baseline type 0\n"
"@    s8 baseline off\n"
"@    s8 dropline off\n"
"@    s8 fill type 0\n"
"@    s8 fill rule 0\n"
"@    s8 fill color 1\n"
"@    s8 fill pattern 1\n"
"@    s8 avalue off\n"
"@    s8 avalue type 2\n"
"@    s8 avalue char size 1.000000\n"
"@    s8 avalue font 0\n"
"@    s8 avalue color 1\n"
"@    s8 avalue rot 0\n"
"@    s8 avalue format general\n"
"@    s8 avalue prec 3\n"
"@    s8 avalue prepend \"\"\n"
"@    s8 avalue append \"\"\n"
"@    s8 avalue offset 0.000000 , 0.000000\n"
"@    s8 errorbar on\n"
"@    s8 errorbar place both\n"
"@    s8 errorbar color 4\n"
"@    s8 errorbar pattern 1\n"
"@    s8 errorbar size 1.000000\n"
"@    s8 errorbar linewidth 1.0\n"
"@    s8 errorbar linestyle 1\n"
"@    s8 errorbar riser linewidth 1.0\n"
"@    s8 errorbar riser linestyle 1\n"
"@    s8 errorbar riser clip off\n"
"@    s8 errorbar riser clip length 0.100000\n"
"@    s8 comment \"srf_grid.agr\"\n"
"@    s8 legend  \"%s\"\n"
"@    s9 hidden false\n"
"@    s9 type xy\n"
"@    s9 symbol 1\n"
"@    s9 symbol size 0.100000\n"
"@    s9 symbol color 8\n"
"@    s9 symbol fill color 8\n"
"@    s9 symbol pattern 1\n"
"@    s9 symbol fill pattern 1\n"
"@    s9 symbol linewidth 1.0\n"
"@    s9 symbol linestyle 1\n"
"@    s9 symbol char 65\n"
"@    s9 symbol char font 0\n"
"@    s9 symbol skip 0\n"
"@    s9 line type 0\n"
"@    s9 line linestyle 1\n"
"@    s9 line linewidth 1.0\n"
"@    s9 line color 1\n"
"@    s9 line pattern 1\n"
"@    s9 baseline type 0\n"
"@    s9 baseline off\n"
"@    s9 dropline off\n"
"@    s9 fill type 0\n"
"@    s9 fill rule 0\n"
"@    s9 fill color 1\n"
"@    s9 fill pattern 1\n"
"@    s9 avalue off\n"
"@    s9 avalue type 2\n"
"@    s9 avalue char size 1.000000\n"
"@    s9 avalue font 0\n"
"@    s9 avalue color 1\n"
"@    s9 avalue rot 0\n"
"@    s9 avalue format general\n"
"@    s9 avalue prec 3\n"
"@    s9 avalue prepend \"\"\n"
"@    s9 avalue append \"\"\n"
"@    s9 avalue offset 0.000000 , 0.000000\n"
"@    s9 errorbar on\n"
"@    s9 errorbar place both\n"
"@    s9 errorbar color 4\n"
"@    s9 errorbar pattern 1\n"
"@    s9 errorbar size 1.000000\n"
"@    s9 errorbar linewidth 1.0\n"
"@    s9 errorbar linestyle 1\n"
"@    s9 errorbar riser linewidth 1.0\n"
"@    s9 errorbar riser linestyle 1\n"
"@    s9 errorbar riser clip off\n"
"@    s9 errorbar riser clip length 0.100000\n"
"@    s9 comment \"srf_grid.agr\"\n"
"@    s9 legend  \"%s\"\n"
"@    s10 hidden false\n"
"@    s10 type xy\n"
"@    s10 symbol 1\n"
"@    s10 symbol size 0.200000\n"
"@    s10 symbol color 8\n"
"@    s10 symbol fill color 8\n"
"@    s10 symbol pattern 1\n"
"@    s10 symbol fill pattern 1\n"
"@    s10 symbol linewidth 1.0\n"
"@    s10 symbol linestyle 1\n"
"@    s10 symbol char 65\n"
"@    s10 symbol char font 0\n"
"@    s10 symbol skip 0\n"
"@    s10 line type 0\n"
"@    s10 line linestyle 1\n"
"@    s10 line linewidth 1.0\n"
"@    s10 line color 1\n"
"@    s10 line pattern 1\n"
"@    s10 baseline type 0\n"
"@    s10 baseline off\n"
"@    s10 dropline off\n"
"@    s10 fill type 0\n"
"@    s10 fill rule 0\n"
"@    s10 fill color 1\n"
"@    s10 fill pattern 1\n"
"@    s10 avalue off\n"
"@    s10 avalue type 2\n"
"@    s10 avalue char size 1.000000\n"
"@    s10 avalue font 0\n"
"@    s10 avalue color 1\n"
"@    s10 avalue rot 0\n"
"@    s10 avalue format general\n"
"@    s10 avalue prec 3\n"
"@    s10 avalue prepend \"\"\n"
"@    s10 avalue append \"\"\n"
"@    s10 avalue offset 0.000000 , 0.000000\n"
"@    s10 errorbar on\n"
"@    s10 errorbar place both\n"
"@    s10 errorbar color 4\n"
"@    s10 errorbar pattern 1\n"
"@    s10 errorbar size 1.000000\n"
"@    s10 errorbar linewidth 1.0\n"
"@    s10 errorbar linestyle 1\n"
"@    s10 errorbar riser linewidth 1.0\n"
"@    s10 errorbar riser linestyle 1\n"
"@    s10 errorbar riser clip off\n"
"@    s10 errorbar riser clip length 0.100000\n"
"@    s10 comment \"srf_grid.agr\"\n"
"@    s10 legend  \"%s\"\n"
"@    s11 hidden false\n"
"@    s11 type xy\n"
"@    s11 symbol 1\n"
"@    s11 symbol size 0.400000\n"
"@    s11 symbol color 8\n"
"@    s11 symbol fill color 8\n"
"@    s11 symbol pattern 1\n"
"@    s11 symbol fill pattern 1\n"
"@    s11 symbol linewidth 1.0\n"
"@    s11 symbol linestyle 1\n"
"@    s11 symbol char 65\n"
"@    s11 symbol char font 0\n"
"@    s11 symbol skip 0\n"
"@    s11 line type 0\n"
"@    s11 line linestyle 1\n"
"@    s11 line linewidth 1.0\n"
"@    s11 line color 1\n"
"@    s11 line pattern 1\n"
"@    s11 baseline type 0\n"
"@    s11 baseline off\n"
"@    s11 dropline off\n"
"@    s11 fill type 0\n"
"@    s11 fill rule 0\n"
"@    s11 fill color 1\n"
"@    s11 fill pattern 1\n"
"@    s11 avalue off\n"
"@    s11 avalue type 2\n"
"@    s11 avalue char size 1.000000\n"
"@    s11 avalue font 0\n"
"@    s11 avalue color 1\n"
"@    s11 avalue rot 0\n"
"@    s11 avalue format general\n"
"@    s11 avalue prec 3\n"
"@    s11 avalue prepend \"\"\n"
"@    s11 avalue append \"\"\n"
"@    s11 avalue offset 0.000000 , 0.000000\n"
"@    s11 errorbar on\n"
"@    s11 errorbar place both\n"
"@    s11 errorbar color 4\n"
"@    s11 errorbar pattern 1\n"
"@    s11 errorbar size 1.000000\n"
"@    s11 errorbar linewidth 1.0\n"
"@    s11 errorbar linestyle 1\n"
"@    s11 errorbar riser linewidth 1.0\n"
"@    s11 errorbar riser linestyle 1\n"
"@    s11 errorbar riser clip off\n"
"@    s11 errorbar riser clip length 0.100000\n"
"@    s11 comment \"srf_grid.agr\"\n"
"@    s11 legend  \"%s\"\n"
"@    s12 hidden false\n"
"@    s12 type xy\n"
"@    s12 symbol 1\n"
"@    s12 symbol size 0.600000\n"
"@    s12 symbol color 8\n"
"@    s12 symbol fill color 8\n"
"@    s12 symbol pattern 1\n"
"@    s12 symbol fill pattern 1\n"
"@    s12 symbol linewidth 1.0\n"
"@    s12 symbol linestyle 1\n"
"@    s12 symbol char 65\n"
"@    s12 symbol char font 0\n"
"@    s12 symbol skip 0\n"
"@    s12 line type 0\n"
"@    s12 line linestyle 1\n"
"@    s12 line linewidth 1.0\n"
"@    s12 line color 1\n"
"@    s12 line pattern 1\n"
"@    s12 baseline type 0\n"
"@    s12 baseline off\n"
"@    s12 dropline off\n"
"@    s12 fill type 0\n"
"@    s12 fill rule 0\n"
"@    s12 fill color 1\n"
"@    s12 fill pattern 1\n"
"@    s12 avalue off\n"
"@    s12 avalue type 2\n"
"@    s12 avalue char size 1.000000\n"
"@    s12 avalue font 0\n"
"@    s12 avalue color 1\n"
"@    s12 avalue rot 0\n"
"@    s12 avalue format general\n"
"@    s12 avalue prec 3\n"
"@    s12 avalue prepend \"\"\n"
"@    s12 avalue append \"\"\n"
"@    s12 avalue offset 0.000000 , 0.000000\n"
"@    s12 errorbar on\n"
"@    s12 errorbar place both\n"
"@    s12 errorbar color 4\n"
"@    s12 errorbar pattern 1\n"
"@    s12 errorbar size 1.000000\n"
"@    s12 errorbar linewidth 1.0\n"
"@    s12 errorbar linestyle 1\n"
"@    s12 errorbar riser linewidth 1.0\n"
"@    s12 errorbar riser linestyle 1\n"
"@    s12 errorbar riser clip off\n"
"@    s12 errorbar riser clip length 0.100000\n"
"@    s12 comment \"srf_grid.agr\"\n"
"@    s12 legend  \"%s\"\n",
		'%',
		'%',
		'%',
		date_string,
		bucket_1_legend,
		bucket_2_legend,
		bucket_3_legend,
		bucket_4_legend,
		bucket_5_legend,
		bucket_6_legend,
		bucket_7_legend,
		bucket_8_legend );

} /* output_hydropattern_grace_header() */

void output_chart(	char *application_name,
			char *document_root_directory,
			char *initial_takeoff_date,
			LIST *cell_list,
			char *bird_count_estimation )
{
	char *agr_filename;
	char *ftp_agr_filename;
	char *output_filename;
	char *ftp_output_filename;
	char *postscript_filename;
	char graph_identifier[ 16 ];
	FILE *output_pipe;
	SRF_CELL *cell;
	int hydropattern_numeric_code;
	char *grace_output;
	int dataset;
	int bucket_size;
	int bird_count_lower_bound;
	int bird_count_upper_bound;
	char *bucket_1_legend;
	char *bucket_2_legend;
	char *bucket_3_legend;
	char *bucket_4_legend;
	char *bucket_5_legend;
	char *bucket_6_legend;
	char *bucket_7_legend;
	char *bucket_8_legend;

	bucket_size = (
		(double)wadingbird_get_max_bird_count(
			cell_list ) / 8.0 ) + 1;

	if ( strcmp( bird_count_estimation, "count" ) == 0 )
	{
/*
		bucket_size = BIRD_COUNT_BUCKET_SIZE;
*/
	}
	else
	{
		bucket_size *= 4;
/*
		bucket_size = BIRD_ESTIMATION_BUCKET_SIZE;
*/
	}

/*
fprintf( stderr, "got bucket_size = %d\n", bucket_size );
*/

	fill_bucket_legends(
		&bucket_1_legend,
		&bucket_2_legend,
		&bucket_3_legend,
		&bucket_4_legend,
		&bucket_5_legend,
		&bucket_6_legend,
		&bucket_7_legend,
		&bucket_8_legend,
		bucket_size,
		bird_count_estimation );

	sprintf( graph_identifier, "%d", getpid() );

	grace_output = application_grace_output( application_name );

	grace_get_filenames(
		&agr_filename,
		&ftp_agr_filename,
		&postscript_filename,
		&output_filename,
		&ftp_output_filename,
		application_name,
		document_root_directory,
		graph_identifier,
		grace_output );

	output_pipe =
		grace_open_output_pipe(
			output_filename,
			postscript_filename,
			agr_filename,
			application_grace_home_directory(
				application_name ),
			application_grace_execution_directory(
				application_name ),
			application_grace_free_option_yn(
				application_name ),
			application_grace_output(
				application_name ) );

	output_hydropattern_grace_header(
			output_pipe,
			initial_takeoff_date,
			bucket_1_legend,
			bucket_2_legend,
			bucket_3_legend,
			bucket_4_legend,
			bucket_5_legend,
			bucket_6_legend,
			bucket_7_legend,
			bucket_8_legend );

	if ( !list_length( cell_list ) )
	{
		printf( "<h3>Error: empty cell list.</h3>\n" );
		document_close();
		pclose( output_pipe );
		exit( 0 );
	}

	/* Output the hydropatterns */
	/* ------------------------ */
	for(	hydropattern_numeric_code = 1;
		hydropattern_numeric_code <= HYDROPATTERN_COUNT + 1;
		hydropattern_numeric_code++ )
	{
		list_rewind( cell_list );

		fprintf(output_pipe,
			"@target G0.S%d\n"
			"@type xy\n",
			hydropattern_numeric_code - 1 );

		do {
			cell = list_get_pointer( cell_list );

			if (	cell->hydropattern_numeric_code ==
				hydropattern_numeric_code )
			{
				fprintf(output_pipe,
				 	"%d %d\n",
				 	cell->utm_easting,
				 	cell->utm_northing );
			}
		} while( list_next( cell_list ) );

		fprintf(output_pipe,
			"&\n" );
	}

	/* Output the bird sightings */
	/* ------------------------- */

	for (	dataset = HYDROPATTERN_COUNT + 1;
		dataset <= HYDROPATTERN_COUNT + BUCKET_COUNT;
		dataset++ )
	{
		list_rewind( cell_list );
	
		fprintf(output_pipe,
			"@target G0.S%d\n"
			"@type xy\n",
			dataset );
	
		do {
			cell = list_get_pointer( cell_list );

			bird_count_lower_bound =
			( (dataset - (HYDROPATTERN_COUNT + 1)) * bucket_size ) +
			1;

			bird_count_upper_bound =
			( (dataset - (HYDROPATTERN_COUNT + 1)) * bucket_size ) +
			bucket_size;

			if ( dataset == BUCKET_COUNT + HYDROPATTERN_COUNT )
				bird_count_upper_bound = INT_MAX;

			if ( cell->bird_count >= bird_count_lower_bound
			&&   cell->bird_count <= bird_count_upper_bound )
			{
				fprintf(output_pipe,
			 		"%d %d\n",
			 		cell->utm_easting,
			 		cell->utm_northing );
			}
		} while( list_next( cell_list ) );
	
		fprintf(output_pipe,
			"&\n" );
	}

	pclose( output_pipe );

	if ( strcmp( grace_output, "postscript_pdf" ) == 0 )
	{
		if ( !grace_convert_postscript_to_pdf(
			output_filename, 
			postscript_filename,
			application_distill_directory(
				application_name ),
			"" /* distill_landscape_flag */,
			application_ghost_script_directory(
				application_name ) ) )
		{
			return;
		}
	}

	grace_output_graph_window(
			application_name,
			ftp_output_filename,
			ftp_agr_filename,
			(char *)0 /* appaserver_mount_point */,
			0 /* not with_document_output */,
			(char *)0 /* where_clause */ );
} /* output_chart() */

void fill_bucket_legends(
		char **bucket_1_legend,
		char **bucket_2_legend,
		char **bucket_3_legend,
		char **bucket_4_legend,
		char **bucket_5_legend,
		char **bucket_6_legend,
		char **bucket_7_legend,
		char **bucket_8_legend,
		int bucket_size,
		char *bird_count_estimation )
{
	char buffer[ 128 ];
	int bird_count_lower_bound;
	int bird_count_upper_bound;
	int i;

	for( i = 1; i <= BUCKET_COUNT; i++ )
	{
		if ( i < BUCKET_COUNT )
		{
			bird_count_lower_bound =
				( (i - 1) * bucket_size) + 1;
			bird_count_upper_bound =
				i * bucket_size;

			sprintf( buffer,
			 	"%d-%d Bird %s",
			 	bird_count_lower_bound,
			 	bird_count_upper_bound,
				bird_count_estimation );
		}
		else
		{
			bird_count_lower_bound =
				(i - 1) * bucket_size;

			sprintf( buffer,
			 	"> %d Bird %s",
			 	bird_count_lower_bound,
				bird_count_estimation );
		}

		if ( i == 1 ) *bucket_1_legend = strdup( buffer );
		if ( i == 2 ) *bucket_2_legend = strdup( buffer );
		if ( i == 3 ) *bucket_3_legend = strdup( buffer );
		if ( i == 4 ) *bucket_4_legend = strdup( buffer );
		if ( i == 5 ) *bucket_5_legend = strdup( buffer );
		if ( i == 6 ) *bucket_6_legend = strdup( buffer );
		if ( i == 7 ) *bucket_7_legend = strdup( buffer );
		if ( i == 8 ) *bucket_8_legend = strdup( buffer );
	}

} /* fill_bucket_legends() */
