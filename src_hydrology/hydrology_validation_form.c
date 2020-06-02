/* hydrology_validation_form.c						*/
/* ----------------------------------------------------------------	*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "list.h"
#include "validation_form.h"
#include "aggregate_level.h"
#include "date.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "application.h"
#include "appaserver_parameter_file.h"
#include "grace.h"
#include "process.h"
#include "environ.h"
#include "hydrology_library.h"

/* Constants */
/* --------- */
#define PROCESS_NAME		"hydrology_validation_form"
/* #define CHART_USING_EASYCHART	1 */

#define TARGET_FRAME		EDIT_FRAME

/* Type Definitions */
/* ---------------- */

/* Prototypes */
/* ---------- */
void get_chart_title_string(		char **title,
					char **sub_title,
					char *station,
					char *begin_measurement_date,
					char *end_measurement_date );

void get_table_title_string(		char **title,
					char *station,
					char *datatype,
					char *begin_measurement_date,
					char *end_measurement_date );

char *get_infrastructure_process( 		char *application_name,
						char *station,
						char *datatype );

void chart_hydrology_validation_form(		char *application_name,
						char *role_name,
						char *document_root_directory,
						char *station,
						char *datatype,
						char *begin_measurement_date,
						char *end_measurement_date,
						char *action_string,
						char *argv_0,
						char *post_dictionary_string );

LIST *hydrology_validation_get_primary_name_list();

int main( int argc, char **argv )
{
	char *person, *session, *application_name;
	char *role_name;
	char *station, *datatype;
	char *begin_date, *end_date;
	char *table_title;
	char action_string[ 1024 ];
	VALIDATION_FORM *validation_form;
	DOCUMENT *document;
	LIST *datatype_name_list;
	LIST *primary_name_list;
	char data_process[ 1024 ];
	char *post_dictionary_string;
	char *database_string = {0};
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *chart_yn;

	output_starting_argv_stderr( argc, argv );

	if ( argc != 11 )
	{
		fprintf( stderr, 
"Usage: %s application role session person station datatype begin_date end_date dictionary chart_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	role_name = argv[ 2 ];
	session = argv[ 3 ];
	person = argv[ 4 ];
	station = argv[ 5 ];
	datatype = argv[ 6 ];
	begin_date = argv[ 7 ];
	end_date = argv[ 8 ];
	post_dictionary_string = argv[ 9 ];
	chart_yn = argv[ 10 ];

	appaserver_parameter_file = new_appaserver_parameter_file();

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}

	hydrology_library_get_clean_begin_end_date(
					&begin_date,
					&end_date,
					application_name,
					station,
					datatype );

	if ( !appaserver_library_validate_begin_end_date(
					&begin_date,
					&end_date,
					(DICTIONARY *)0 /* post_dictionary */) )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf( "<p>ERROR: no data available for these dates.\n" );
		document_close();
		exit( 0 );
	}

	primary_name_list = 
		hydrology_validation_get_primary_name_list();

	sprintf( action_string,
		 "%s/post_hydrology_validation_form?%s+%s+%s+%s+%s+%s+%s+%s",
		 appaserver_parameter_file->apache_cgi_directory,
		 person,
		 application_name,
		 session,
		 station,
		 datatype,
		 begin_date,
		 end_date,
		 TARGET_FRAME );

	if ( *chart_yn == 'y' )
	{
		chart_hydrology_validation_form(application_name,
						role_name,
						appaserver_parameter_file->
							document_root,
						station,
						datatype,
						begin_date,
						end_date,
						action_string,
						argv[ 0 ],
						post_dictionary_string );
		exit( 0 );
	}

	datatype_name_list = list_new();
	list_append_string( datatype_name_list, datatype );

	get_table_title_string(		&table_title,
					station,
					datatype,
					begin_date,
					end_date );

	validation_form = 
			validation_form_new(	table_title,
						TARGET_FRAME,
						primary_name_list,
						datatype_name_list );

	document = document_new( table_title, application_name );
	document_set_output_content_type( document );

	document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(
			document->application_name,
			document->onload_control_string );

	validation_form_set_action( validation_form, action_string );

	validation_form->element_list = 
		validation_form_get_element_list( 
		   validation_form->primary_column_name_list,
		   validation_form->datatype_name_list );

	validation_form->table_border = 1;

	validation_form_output_heading(	validation_form->title,
					validation_form->action_string,
					validation_form->target_frame,
					validation_form->element_list,
					validation_form->table_border );

	/* Input specification: primary_data_1,...,n|datatype=value=validated */
	/* ------------------------------------------------------------------ */
	sprintf( data_process,
		 "hydrology_select_value4validation.sh %s %s %s %s %s",
		 application_name,
		 station,
		 datatype,
		 begin_date,
		 end_date );

	validation_form->row_list =
		validation_form_get_row_list(
			validation_form->primary_column_name_list,
			data_process );

	validation_form_output_body( validation_form );
	output_dictionary_string_as_hidden( post_dictionary_string );
	validation_form_output_trailer();

	document_close();

	process_increment_execution_count(
				application_name,
				PROCESS_NAME,
				appaserver_parameter_file_get_dbms() );
	exit( 0 );
} /* main() */

LIST *hydrology_validation_get_primary_name_list()
{
	LIST *list = list_new();
	list_add_string( list, "measurement_date" );
	list_add_string( list, "measurement_time" );
	return list;
}

void chart_hydrology_validation_form(		char *application_name,
						char *role_name,
						char *document_root_directory,
						char *station,
						char *datatype,
						char *begin_measurement_date,
						char *end_measurement_date,
						char *action_string,
						char *argv_0,
						char *post_dictionary_string )
{
	char *select_string;
	char where_clause[ 512 ];
	char data_process[ 1024 ];
	char *output_filename;
	char *ftp_output_filename;
	char *agr_filename;
	char *ftp_agr_filename;
	char *postscript_filename;
	char *chart_title;
	char *chart_sub_title;
	GRACE *grace;
	char *infrastructure_process;
	int datatype_entity_piece = 0;
	int datatype_piece = 1;
	int date_piece = 2;
	int time_piece = 3;
	int value_piece = 4;
	char graph_identifier[ 128 ];
	int page_width_pixels;
	int page_length_pixels;
	char *distill_landscape_flag;

	infrastructure_process =
		get_infrastructure_process( 	application_name,
						station,
						datatype );
	select_string =
	"station,datatype,measurement_date,measurement_time,measurement_value";

	sprintf(	where_clause,
			"station = '%s' and			"
			"datatype = '%s' and			"
			"measurement_date between '%s' and '%s'	",
			station,
			datatype,
			begin_measurement_date,
			end_measurement_date );

	sprintf( data_process,
		 "get_folder_data	application=%s			 "
		 "			folder=measurement		 "
		 "			select=%s			 "
		 "			where=\"%s\"			|"
		 "tr '^' '|'						 ",
		 application_name,
		 select_string,
		 where_clause );

	get_chart_title_string(
			&chart_title,
			&chart_sub_title,
			station,
			begin_measurement_date,
			end_measurement_date );

	grace = grace_new_unit_graph_grace(
				application_name,
				role_name,
				infrastructure_process,
				data_process,
				argv_0,
				datatype_entity_piece,
				datatype_piece,
				date_piece,
				time_piece,
				value_piece,
				strdup( chart_title ),
				strdup( chart_sub_title ),
				0 /* not datatype_type_xyhilo */,
				no_cycle_colors_if_multiple_datatypes );

	grace->grace_output =
		application_get_grace_output( application_name );

	sprintf(	graph_identifier,
			"%s_%s_%d",
			station,
			datatype,
			getpid() );

	grace_get_filenames(
			&agr_filename,
			&ftp_agr_filename,
			&postscript_filename,
			&output_filename,
			&ftp_output_filename,
			application_name,
			document_root_directory,
			graph_identifier,
			grace->grace_output );

	if ( !grace_set_begin_end_date(grace,
					begin_measurement_date,
					end_measurement_date ) )
	{
		document_quick_output_body(
			application_name,
			(char *)0 /* appaserver_mount_point */ );

		printf(
	"<h2>ERROR: Invalid date format format.</h2>" );
		document_close();
		exit( 1 );
	}

	if ( !grace_set_structures(
				&page_width_pixels,
				&page_length_pixels,
				&distill_landscape_flag,
				&grace->landscape_mode,
				grace,
				grace->graph_list,
				grace->anchor_graph_list,
				grace->begin_date_julian,
				grace->end_date_julian,
				grace->number_of_days,
				grace->grace_graph_type,
				0 /* not force_landscape_mode */ ) )
	{
		document_quick_output_body(
			application_name,
			(char *)0 /* appaserver_mount_point */ );

		printf( "<p>Warning: no graphs to display.\n" );
		document_close();
		exit( 0 );
	}

	if ( !grace_output_charts(
				output_filename, 
				postscript_filename,
				agr_filename,
				grace->title,
				grace->sub_title,
				grace->xaxis_ticklabel_format,
				grace->grace_graph_type,
				grace->x_label_size,
				page_width_pixels,
				page_length_pixels,
				application_get_grace_home_directory(
					application_name ),
				application_get_grace_execution_directory(
					application_name ),
				application_get_grace_free_option_yn(
					application_name ),
				grace->grace_output,
				application_get_distill_directory(
					application_name ),
				distill_landscape_flag,
				application_get_ghost_script_directory(
					application_name ),
				(LIST *)0 /* quantum_datatype_name_list */,
				grace->symbols,
				grace->world_min_x,
				grace->world_max_x,
				grace->xaxis_ticklabel_precision,
				grace->graph_list,
				grace->anchor_graph_list ) )
	{
		document_quick_output_body(
			application_name,
			(char *)0 /* appaserver_mount_point */ );

		printf( "<p>Warning: no graphs to display.\n" );
		document_close();
		exit( 0 );
	}
	else
	{
		document_quick_output_body(
			application_name,
			(char *)0 /* appaserver_mount_point */ );

		grace_output_graph_window(
				application_name,
				ftp_output_filename,
				ftp_agr_filename,
				(char *)0 /* appaserver_mount_point */,
				0 /* not with_document_output */,
				(char *)0 /* where_clause */ );

		printf( "<h3>Validate %s/%s from %s to %s</h3>\n",
				station, datatype,
				begin_measurement_date, end_measurement_date );

		printf( "<form method=post action='%s'>\n",
				action_string );
		printf( "<input type=submit value=Validate>\n" );

		output_dictionary_string_as_hidden(
				post_dictionary_string );

		printf( "</form>\n" );
	}
	document_close();
} /* chart_hydrology_validation_form() */

char *get_infrastructure_process(	char *application_name,
					char *station,
					char *datatype )
{
	char *datatype_table;
	char *station_datatype_table;
	char infrastructure_process[ 1024 ];

	datatype_table =
		get_table_name( application_name, "datatype" );

	station_datatype_table =
		get_table_name( application_name, "station_datatype" );

/* ------------------------------------------------------------------------ */
/* Sample input: 						            */
/* unit|expected_count|datatype|station|bar_graph_yn|scale_graph_zero_yn    */
/* ------------------------------------------------------------------------ */
/* celsius|24|air_temperature|BA|n|n		<-- block 0	            */
/* celsius|24|bottom_temperature|BA|n|n				            */
/* celsius|24|surface_temperature|BA|n|n			            */
/* celsius|24|temperature_cr10|BA|n|n				            */
/* feet|144|stage|BA|n|n			<-- block 1	            */
/* inches|24|rain|BA|y|y			<-- block 2	            */
/* mS/cm|24|conductivity|BA|n|n			<-- block 3	            */
/* volts|24|voltage|BA|n|n			<-- block 4	            */
/* ------------------------------------------------------------------------ */
/*
	sprintf(infrastructure_process,
		"echo \"select 	%s.units,				 "
		"		%s.expected_count,			 "
		"		%s.datatype,				 "
		"		%s.station,				 "
		"		%s.bar_graph_yn,			 "
		"		%s.scale_graph_to_zero_yn		 "
      		"	from %s,%s					 "
      		"	where %s.station = '%s'				 "
		"	  and %s.datatype = %s.datatype			 "
		"	  and %s.datatype = '%s'			 "
		"	order by units,expected_count,%s.datatype;\"	|"
		"sql '|'						 ",
		datatype_table,
		station_datatype_table,
		station_datatype_table,
		station_datatype_table,
		datatype_table,
		datatype_table,
		station_datatype_table,
		datatype_table,
		station_datatype_table,
		station,
		station_datatype_table,
		datatype_table,
		datatype_table,
		datatype,
		station_datatype_table );
*/

	sprintf(infrastructure_process,
		"echo \"select 	%s.units,				 "
		"		%s.datatype,				 "
		"		%s.station,				 "
		"		%s.bar_graph_yn,			 "
		"		%s.scale_graph_to_zero_yn		 "
      		"	from %s,%s					 "
      		"	where %s.station = '%s'				 "
		"	  and %s.datatype = %s.datatype			 "
		"	  and %s.datatype = '%s'			 "
		"	order by units,%s.datatype;\"			|"
		"sql '|'						 ",
		datatype_table,
		station_datatype_table,
		station_datatype_table,
		datatype_table,
		datatype_table,
		station_datatype_table,
		datatype_table,
		station_datatype_table,
		station,
		station_datatype_table,
		datatype_table,
		datatype_table,
		datatype,
		station_datatype_table );
	return strdup( infrastructure_process );

} /* get_infrastructure_process() */

void get_table_title_string(
			char **table_title,
			char *station,
			char *datatype,
			char *begin_measurement_date,
			char *end_measurement_date )
{
	char title[ 1024 ];
	char buffer1[ 512 ];
	char buffer2[ 512 ];

	sprintf(title, "Validate %s/%s<br>",
		format_initial_capital(	buffer1,
					station ),
		format_initial_capital(	buffer2,
					datatype ) );

	sprintf( title + strlen( title ),
		"Beginning: %s Ending: %s",
		 begin_measurement_date,
		 end_measurement_date );
	*table_title = strdup( title );
} /* get_table_title_string() */

void get_chart_title_string(
			char **chart_title,
			char **chart_sub_title,
			char *station,
			char *begin_measurement_date,
			char *end_measurement_date )
{
	char title[ 1024 ];
	char sub_title[ 1024 ];
	char buffer[ 512 ];

	sprintf(title, "Validate %s",
		format_initial_capital(	buffer,
					station ) );

	sprintf( sub_title,
		"Beginning: %s Ending: %s",
		 begin_measurement_date,
		 end_measurement_date );
	*chart_title = strdup( title );
	*chart_sub_title = strdup( sub_title );
} /* get_chart_title_string() */

