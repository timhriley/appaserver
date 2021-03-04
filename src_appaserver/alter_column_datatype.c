/* --------------------------------------------------- 	*/
/* alter_column_datatype.c			       	*/
/* --------------------------------------------------- 	*/
/* 						       	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "timlib.h"
#include "document.h"
#include "application.h"
#include "folder.h"
#include "attribute.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "session.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
boolean output_process_script(
				char *process_filename,
				char *application_name,
				char *folder_name,
				char *attribute_name,
				char *database_datatype,
				char *attribute_datatype,
				int width,
				int float_decimal_places,
				char *hint_message,
				boolean is_system_attribute,
				char *post_change_javascript,
				char *on_focus_javascript_function,
				boolean lookup_histogram_output,
				boolean lookup_time_chart_output );

char *get_sys_string(	char *table_name, 
			char *attribute_name, 
			char *datatype );

int main( int argc, char **argv )
{
	char *application_name;
	char *folder_name;
	char *attribute_name;
	char *database_datatype;
	char *table_name;
	char *really_yn;
	char *sys_string;
	DOCUMENT *document;
	ATTRIBUTE *attribute;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 8 )
	{
		fprintf(stderr,
"Usage: %s ignored ignored ignored ignored folder attribute really_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	folder_name = argv[ 5 ];
	attribute_name = argv[ 6 ];
	really_yn = argv[ 7 ];

	appaserver_parameter_file = appaserver_parameter_file_new();

	table_name = get_table_name( application_name, folder_name );

	attribute =
		attribute_load_folder_attribute(
			application_name,
			folder_name,
			attribute_name );

	if ( !attribute )
	{
		document_quick_output_body(
					application_name,
					appaserver_parameter_file->
						appaserver_mount_point );
		printf( "<h3>Please choose an attribute.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( !attribute->datatype || !*attribute->datatype )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: attribute = (%s) has empty attribute_datatype.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 attribute->attribute_name );
		exit( 1 );
	}

	database_datatype =
		attribute_database_datatype(
			attribute->datatype,
			attribute->width,
			attribute->float_decimal_places,
			attribute->primary_key_index );

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

	sys_string = 
		get_sys_string(	table_name, 
				attribute_name, 
				database_datatype );

	printf( "<BR><p>%s\n", sys_string );

	if ( toupper( *really_yn ) == 'Y' )
	{
		fflush( stdout );
		system( sys_string );
		printf( "<BR><h3>Process complete</h3>\n" );
	}
	else
	{
		char process_filename[ 1024 ];
		char sys_string[ 1024 ];

		sprintf( process_filename,
			 "%s/alter_column_datatype.sh",
			 appaserver_parameter_file->
				appaserver_data_directory );

		if ( !output_process_script(
				process_filename,
				application_name,
				folder_name,
				attribute_name,
				database_datatype,
				attribute->datatype,
				attribute->width,
				attribute->float_decimal_places,
				attribute->hint_message,
				attribute->appaserver,
				attribute->post_change_javascript,
				attribute->on_focus_javascript_function,
				attribute->lookup_histogram_output,
				attribute->lookup_time_chart_output ) )
		{
			printf( "<BR><h2>ERROR: Cannot create %s</h2>\n",
				process_filename );
		}
		else
		{
			sprintf( sys_string,
				 "chmod +x,g+w %s",
				 process_filename );
			system( sys_string );
			printf( "<BR><p>Created %s</p>\n", process_filename );
		}
	}

	document_close();
	exit( 0 );
} /* main() */


char *get_sys_string(	char *table_name, 
			char *attribute_name, 
			char *database_datatype )
{
	char buffer[ 1024 ];

	sprintf(buffer,
"echo \"alter table %s modify %s %s;\" | sql.e 2>&1 | html_paragraph_wrapper",
		table_name,
		attribute_name,
		database_datatype );

	return strdup( buffer );
} /* get_sys_string() */

boolean output_process_script(
				char *process_filename,
				char *application_name,
				char *folder_name,
				char *attribute_name,
				char *database_datatype,
				char *attribute_datatype,
				int width,
				int float_decimal_places,
				char *hint_message,
				boolean is_system_attribute,
				char *post_change_javascript,
				char *on_focus_javascript_function,
				boolean lookup_histogram_output,
				boolean lookup_time_chart_output )
{
	FILE *output_file;
	char buffer[ 2048 ];

	if ( ! ( output_file = fopen( process_filename, "w" ) ) )
		return 0;

	strcpy( buffer, hint_message );

	fprintf( output_file,
"#!/bin/sh\n" );
	fprintf( output_file,
"if [ \"$#\" -ne 1 ]\n" );
	fprintf( output_file,
"then\n" );
	fprintf( output_file,
"\techo \"Usage: $0 application\" 1>&2\n" );
	fprintf( output_file,
"\texit 1\n" );
	fprintf( output_file,
"fi\n" );

	fprintf( output_file,
"application=$1\n\n" );

	if ( !is_system_attribute )
	{
		fprintf(output_file,
	"if [ \"$application\" != %s ]\n", application_name );
		fprintf(output_file,
	"then\n" );
		fprintf(output_file,
	"\texit 0\n" );
		fprintf(output_file,
	"fi\n\n" );
	}

	fprintf( output_file,
"(\n" );

	fprintf( output_file,
"table_name=`get_table_name $application %s`\n",
		 folder_name );
	fprintf( output_file,
"echo \"alter table $table_name modify %s %s;\" | sql.e\n",
		 	attribute_name,
		 	database_datatype );
	fprintf( output_file,
"attribute_table=`get_table_name $application attribute`\n" );

/*
	fprintf( output_file,
"echo \"delete from $attribute_table where attribute = '%s';\"		 |\n"
"sql.e\n",
		 attribute_name );
*/

	fprintf(output_file,
"echo \"delete from $attribute_table where attribute = '%s';\" | sql.e\n",
		attribute_name );

	fprintf(output_file,
"echo \"insert into $attribute_table					\\\n"
"	(attribute,							\\\n"
"	hint_message,							\\\n"
"	appaserver_yn,							\\\n"
"	attribute_datatype,						\\\n"
"	width,								\\\n"
"	float_decimal_places,						\\\n"
"	post_change_javascript,						\\\n"
"	on_focus_javascript_function,					\\\n"
"	lookup_histogram_output_yn,					\\\n"
"	lookup_time_chart_output_yn)					\\\n"
"	values('%s','%s','%c','%s',%d,%d,'%s','%s','%c','%c');\"	 |\n"
"sql.e									  \n",
		attribute_name,
		escape_single_quotes( buffer ),
		(is_system_attribute) ? 'y' : 'n',
		attribute_datatype,
		width,
		float_decimal_places,
		post_change_javascript,
		on_focus_javascript_function,
		(lookup_histogram_output) ? 'y' : 'n',
		(lookup_time_chart_output) ? 'y' : 'n' );

	fprintf( output_file,
") 2>&1 | grep -vi duplicate\n" );
	fprintf( output_file,
"exit 0\n" );

	fclose( output_file );
	return 1;
} /* output_process_script() */

