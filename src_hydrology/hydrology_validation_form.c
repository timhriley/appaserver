/* $APPASERVER_HOME/src_hydrology/hydrology_validation_form.c		*/
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
#include "form.h"
#include "hydrology_library.h"

/* Constants */
/* --------- */
#define PROCESS_NAME		"hydrology_validation_form"

#define TARGET_FRAME		EDIT_FRAME

#define DEFAULT_ATTRIBUTE_WIDTH 		10
#define NO_VERIFY_PUSH_BUTTON_PREFIX		"no_verify_push_button_"
#define NO_VERIFY_LABEL				"Not Validated"

/* Data Structures */
/* --------------- */
typedef struct
{
	char *value;
	int validated;
} VALIDATION_DATATYPE;

typedef struct
{
	char *primary_key;
	HASH_TABLE *primary_data_hash_table;
	HASH_TABLE *datatype_data_hash_table;
} ROW;

typedef struct
{
	char *title;
	char *target_frame;
	char *action_string;
	LIST *row_list;
	LIST *primary_column_name_list;
	LIST *datatype_name_list;
	LIST *element_list;
	int table_border;
} VALIDATION_FORM;

/* Operations */
/* ---------- */
VALIDATION_FORM *validation_form_new( 	char *title, 
					char *target_frame,
					LIST *primary_column_name_list,
					LIST *datatype_name_list );
ROW *validation_form_row_new( 		char *primary_key );
LIST *validation_form_get_element_list( LIST *primary_column_name_list,
					LIST *datatype_name_list );
void validation_form_set_action( 	VALIDATION_FORM *validation_form, 
					char *action_string );
void validation_form_output_heading(	char *title,
					char *action_string,
					char *target_frame,
					LIST *element_list,
					int table_border );
void validation_form_output_table_heading( 
					LIST *element_list );
void validation_form_output_body( 	VALIDATION_FORM *validation_form );
void validation_form_output_trailer( 	void );
void validation_form_set_primary_data_hash_table(
					HASH_TABLE *primary_data_hash_table,
					LIST *primary_column_name_list,
					char *primary_data_comma_list );

VALIDATION_DATATYPE *validation_datatype_new(
					char *value, int validated );

LIST *validation_form_get_row_list(	LIST *primary_column_name_list,
					char *select_sys_string );
int row_compare( 			ROW *row1, ROW *row2 );

char *hydrology_validation_form_data_process(
			char *application_name,
			char *station,
			char *datatype,
			char *begin_date,
			char *end_date );

char *hydrology_validation_form_action_string(
			char *apache_cgi_directory,
			char *login_name,
			char *application_name,
			char *session_key,
			char *station,
			char *datatype,
			char *begin_date,
			char *end_date,
			char *target_frame );

void validation_form_chart_title_string(
			char **title,
			char **sub_title,
			char *station,
			char *begin_measurement_date,
			char *end_measurement_date );

void hydrology_validation_form_table_title_string(
			char **title,
			char *station,
			char *datatype,
			char *begin_measurement_date,
			char *end_measurement_date );

char *hydrology_validation_form_infrastructure_process(
			char *application_name,
			char *station,
			char *datatype );

void chart_hydrology_validation_form(
			char *application_name,
			char *role_name,
			char *document_root_directory,
			char *station,
			char *datatype,
			char *begin_measurement_date,
			char *end_measurement_date,
			char *action_string,
			char *argv_0,
			char *post_dictionary_string );

LIST *hydrology_validation_primary_name_list(
			void );

LIST *hydrology_validation_form_datatype_name_list(
			LIST *datatype );

int main( int argc, char **argv )
{
	char *login_name;
	char *session_key;
	char *application_name;
	char *role_name;
	char *station;
	char *datatype;
	char *begin_date;
	char *end_date;
	char *table_title;
	char *action_string;
	VALIDATION_FORM *validation_form;
	DOCUMENT *document;
	char data_process[ 1024 ];
	char *post_dictionary_string;
	char *database_string = {0};
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *chart_yn;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 11 )
	{
		fprintf( stderr, 
"Usage: %s ignored role session login_name station datatype begin_date end_date dictionary chart_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	role_name = argv[ 2 ];
	session = argv[ 3 ];
	login_name = argv[ 4 ];
	station = argv[ 5 ];
	datatype = argv[ 6 ];
	begin_date = argv[ 7 ];
	end_date = argv[ 8 ];
	post_dictionary_string = argv[ 9 ];
	chart_yn = argv[ 10 ];

	appaserver_parameter_file = new_appaserver_parameter_file();

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
		document_quick_output( application_name );
		printf( "<p>ERROR: no data available for these dates.\n" );
		document_close();
		exit( 0 );
	}

	action_string =
		hydrology_validation_form_action_string(
			appaserver_parameter_file->apache_cgi_directory,
			login_name,
			application_name,
			session_key,
			station,
			datatype,
			begin_date,
			end_date,
			TARGET_FRAME );

	if ( *chart_yn == 'y' )
	{
		chart_hydrology_validation_form(
			application_name,
			role_name,
			appaserver_parameter_file->document_root,
			station,
			datatype,
			begin_date,
			end_date,
			action_string,
			argv[ 0 ],
			post_dictionary_string );
		exit( 0 );
	}

	document = document_new( application_name, (MENU *)0 );
	document_output_output_content_type();
	document_begin( stdout, document );

	hydrology_validation_form_table_title_string(
		&table_title,
		station,
		datatype,
		begin_date,
		end_date );

	validation_form = 
		validation_form_new(
			table_title,
			TARGET_FRAME,
			hydrology_validation_primary_name_list(),
			hydrology_validation_form_datatype_name_list(
				datatype ),
			action_string,
			1 /* table_border */,
			hydrology_validation_form_data_process(
				application_name,
				station,
				datatype,
				begin_date,
				end_date ) );

	validation_form_output_heading(
		validation_form->table_title,
		validation_form->action_string,
		validation_form->target_frame,
		validation_form->element_list,
		validation_form->table_border );

	validation_form_output_body( validation_form );
	output_dictionary_string_as_hidden( post_dictionary_string );
	validation_form_output_trailer();

	document_close();

	process_increment_execution_count(
				application_name,
				PROCESS_NAME,
				appaserver_parameter_file_get_dbms() );
	return 0;
}

LIST *hydrology_validation_primary_name_list()
{
	LIST *list = list_new();
	list_set( list, "measurement_date" );
	list_set( list, "measurement_time" );
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
		hydrology_validation_form_infrastructure_process(
			application_name,
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
		application_grace_output( application_name );

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
				application_grace_home_directory(
					application_name ),
				application_grace_execution_directory(
					application_name ),
				application_grace_free_option_yn(
					application_name ),
				grace->grace_output,
				application_distill_directory(
					application_name ),
				distill_landscape_flag,
				application_ghost_script_directory(
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
}

char *hydrology_validation_form_infrastructure_process(
			char *application_name,
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

}

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
}

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
}

LIST *hydrology_validation_form_datatype_name_list( LIST *datatype )
{
	LIST *datatype_name_list = list_new();
	list_set( datatype_name_list, datatype );
	return datatype_name_list;
}

char *hydrology_validation_form_action_string(
			char *apache_cgi_directory,
			char *login_name,
			char *application_name,
			char *session_key,
			char *station,
			char *datatype,
			char *begin_date,
			char *end_date,
			char *target_frame )
{
	char action_string[ 1024 ];

	sprintf( action_string,
		 "%s/post_hydrology_validation_form?%s+%s+%s+%s+%s+%s+%s+%s",
		 apache_cgi_directory,
		 login_name,
		 application_name,
		 session,
		 station,
		 datatype,
		 begin_date,
		 end_date,
		 target_frame );

	return strdup( action_string );
}

char *hydrology_validation_form_data_process(
			char *application_name,
			char *station,
			char *datatype,
			char *begin_date,
			char *end_date )
{
	char data_process[ 1024 ];

	/* Input specification: primary_data_1,...,n|datatype=value=validated */
	/* ------------------------------------------------------------------ */
	sprintf( data_process,
		 "hydrology_select_value4validation.sh %s '%s' '%s' %s %s",
		 application_name,
		 station,
		 datatype,
		 begin_date,
		 end_date );

	return strdup( data_process );
}

VALIDATION_FORM *validation_form_new( 	char *title, 
					char *target_frame,
					LIST *primary_column_name_list,
					LIST *datatype_name_list )
{
	VALIDATION_FORM *v;

	v = (VALIDATION_FORM *)calloc( 1, sizeof( VALIDATION_FORM ) );
	v->title = title;
	v->target_frame = target_frame;
	v->primary_column_name_list = primary_column_name_list;
	v->datatype_name_list = datatype_name_list;
	return v;
}

ROW *validation_form_row_new( char *primary_key )
{
	ROW *row;

	row = (ROW *)calloc( 1, sizeof( ROW ) );
	row->primary_key = strdup( primary_key );
	row->primary_data_hash_table = hash_table_new( hash_table_small );
	row->datatype_data_hash_table = hash_table_new( hash_table_small );
	return row;
}

LIST *validation_form_get_element_list( LIST *primary_column_name_list,
					LIST *datatype_name_list )
{
	LIST *element_list = list_new();
	char buffer[ 1024 ];
	ELEMENT_APPASERVER *element;
	char *primary_column_name;
	char *datatype_name;

	if ( !list_reset( primary_column_name_list ) )
	{
		fprintf(stderr,
"ERROR in validation_form_get_element_list: primary_column_name_list is empty\n" );
		exit( 1 );
	}

	if ( !list_reset( datatype_name_list ) )
	{
		fprintf(stderr,
"ERROR in validation_form_get_element_list: datatype_name_list is empty\n" );
		exit( 1 );
	}

	/* Create a line break */
	/* ------------------- */
	element = element_appaserver_new( linebreak, "" );
	list_append( 	element_list, 
			element, 
			sizeof( ELEMENT_APPASERVER ) );

	/* For each primary column */
	/* ----------------------- */
	do {
		primary_column_name = 
			list_get_string( primary_column_name_list );

		/* Create a prompt element for the primary column */
		/* ---------------------------------------------- */
		element = 
			element_appaserver_new( 
				prompt_data,
				strdup( format_initial_capital( 
				   buffer, 
				   primary_column_name ) ) );

		list_append( 	element_list, 
				element, 
				sizeof( ELEMENT_APPASERVER ) );
	} while( list_next( primary_column_name_list ) );

	/* For each datatype */
	/* ------------------ */
	do {
		datatype_name = list_get_string( datatype_name_list );

		/* Create a prompt element for the value */
		/* ------------------------------------- */
		element = 
			element_appaserver_new( 
				prompt_data,
				strdup( format_initial_capital( 
				   buffer, 
				   datatype_name ) ) );
		list_append(	element_list, 
				element, 
				sizeof( ELEMENT_APPASERVER ) );

	} while( list_next( datatype_name_list ) );

	return element_list;
}

void validation_form_set_action( 	VALIDATION_FORM *validation_form, 
					char *action_string )
{
	validation_form->action_string = strdup( action_string );
}

void validation_form_output_heading(	char *title,
					char *action_string,
					char *target_frame,
					LIST *element_list,
					int table_border )
{
	printf( "<h1>%s</h1>\n", title );

	/* Output form tag */
	/* --------------- */
	printf( "<form method=post" );
	printf( " action=%s", action_string );
	printf( " target=\"%s\"", target_frame );
	printf( ">\n" );

	/* Output table tag */
	/* ---------------- */
	printf(
"	<table cellspacing=0 cellpadding=0" );

	if ( table_border ) printf( " border" );

	printf( ">\n" );

	validation_form_output_table_heading( element_list );

}

void validation_form_output_table_heading( LIST *element_list )
{
	ELEMENT_APPASERVER *element;
	char buffer[ 1024 ];
	char *heading;

	if ( element_list
	&&   list_reset( element_list ) )
	{
		printf( "<tr>" );
		do {
			element = list_get( element_list );
			heading = element_get_heading( (char **)0, element, 0 );
			if ( heading && *heading )
			{
				printf( "<th><p>%s", 
					format_initial_capital( 
						buffer, 
						heading ) );
			}
		} while( next_item( element_list ) );
		printf( "\n" );
	}
}

void validation_form_output_body( VALIDATION_FORM *validation_form )
{
	ELEMENT_APPASERVER *element;
	int row_int;
	char *data;
	VALIDATION_DATATYPE *datatype;
	ROW *row;
	char *primary_column_name;
	char *datatype_name;

	if ( !list_reset( validation_form->element_list ) )
	{
		fprintf(stderr,
	"ERROR: validation_form_output_body has empty element list.\n" );
		exit( 1 );
	}

	if ( !list_rewind( validation_form->row_list ) )
	{
		fprintf(stderr,
	"ERROR in validation_form_output_body(): row list is empty.\n" );
		exit( 1 );
	}

	if ( !list_length( validation_form->element_list ) )
	{
		fprintf(stderr,
	"ERROR in validation_form_output_body(): element list is empty.\n" );
		exit( 1 );
	}

	row_int = 1;
	do {
		row = list_get_pointer( validation_form->row_list );
		list_rewind( validation_form->element_list );
		list_rewind( validation_form->primary_column_name_list );

		/* Output the line break */
		/* --------------------- */
		element = (ELEMENT_APPASERVER *)
			list_get( validation_form->element_list );
		element_simple_output( element, row_int );
		list_next( validation_form->element_list );

		/* Output the primary columns */
		/* -------------------------- */
		do {
			primary_column_name = 
				list_get_string( validation_form->
						 primary_column_name_list );
			data =  hash_table_get_pointer(
					row->primary_data_hash_table,
					primary_column_name );

			if ( !data )
			{
				fprintf( stderr,
			"Warning in %s/%s()/%d: cannot find data for %s\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__,
					 primary_column_name );

				data = "";
			}

			element =
				list_get_pointer(
					validation_form->element_list );
			element_set_data( element, data );
			element_simple_output( element, row_int );
			list_next( validation_form->element_list );
		} while( list_next( validation_form->
				    primary_column_name_list ) );

		/* Output the datatypes */
		/* -------------------- */
		list_rewind( validation_form->datatype_name_list );
		do {
			datatype_name = 
				list_get_pointer(
					validation_form->datatype_name_list );

			datatype = hash_table_get_pointer( 
					row->datatype_data_hash_table,
					datatype_name );

			/* Output the datatype text item */
			/* ----------------------------- */
			if ( datatype )
			{
				element =
					list_get_pointer(
						validation_form->element_list );
				data = datatype->value;
				element_set_data( element, data );
				element_simple_output( element, row_int );
			}
			else
			{
				element_blank_data_output( stdout );
			}

			list_next( validation_form->element_list );

		} while( list_next( validation_form->
				    datatype_name_list ) );

		row_int++;
	} while( next_item( validation_form->row_list ) );

}

void validation_form_output_trailer( void )
{
	printf( "<tr><td>" );

	form_output_submit_button(
			(char *)0,
			"Validate",
			0 /* form_number */ );

	printf( "</table></form>\n" );
}

void validation_form_set_primary_data_hash_table(
			HASH_TABLE *primary_data_hash_table,
			LIST *primary_column_name_list,
			char *primary_data_comma_list )
{
	char primary_data[ 1024 ];
	char *primary_column_name;
	int i;

	if ( 	list_length( primary_column_name_list ) !=
		count_characters( ',', primary_data_comma_list ) + 1 )
	{
		fprintf( stderr,
"validation_form_set_primary_data_hash_table() ERROR: column list = (%s) does not match data list = (%s)\n",
			 list_display( primary_column_name_list ),
			 primary_data_comma_list );
		exit( 1 );
	}

	for( 	i = 0, list_rewind( primary_column_name_list );
		piece( primary_data, ',', primary_data_comma_list, i );
		list_next( primary_column_name_list ), i++ )
	{
		primary_column_name = 
			list_get_string( primary_column_name_list );

		hash_table_set_string( 	primary_data_hash_table,
					strdup( primary_column_name ),
					strdup( primary_data ) );
	}
}

VALIDATION_DATATYPE *validation_datatype_new( char *value, int validated )
{
	VALIDATION_DATATYPE *d;

	d = (VALIDATION_DATATYPE *)calloc( 1, sizeof( VALIDATION_DATATYPE ) );
	d->value = value;
	d->validated = validated;
	return d;
}


int row_compare( ROW *row1, ROW *row2 )
{
	return strcmp( row1->primary_key, row2->primary_key );
}

LIST *validation_form_get_row_list(	LIST *primary_column_name_list,
					char *select_sys_string )
{
	FILE *p;
	char input_buffer[ 4096 ];
	char right_half[ 4096 ];
	char buffer[ 1024 ];
	char primary_data_comma_list[ 4096 ];
	char value[ 128 ];
	int validated;
	ROW *row;
	LIST *row_list = list_new();
	char datatype_name[ 1024 ];

	p = popen( select_sys_string, "r" );

	/* Input specification: primary_data_1,...,n|datatype=value=validated */
	/* ------------------------------------------------------------------ */
	while( get_line( input_buffer, p ) )
	{
		piece( primary_data_comma_list, '|', input_buffer, 0 );
		piece( right_half, '|', input_buffer, 1 );
		piece( datatype_name, '=', right_half, 0 );
		piece( value, '=', right_half, 1 );
		validated = atoi( piece( buffer, '=', right_half, 2 ) );

		row = validation_form_row_new( 
				primary_data_comma_list );

		list_append_pointer( row_list, (char *)row );

		validation_form_set_primary_data_hash_table(
			row->primary_data_hash_table,
			primary_column_name_list,
			primary_data_comma_list );

		hash_table_set(	row->datatype_data_hash_table,
				strdup( datatype_name ),
				validation_datatype_new(
					strdup( value ),
					validated ) );
	} /* while( get_line() ) */
	pclose( p );
	return row_list;
}

