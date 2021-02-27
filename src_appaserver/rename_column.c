/* --------------------------------------------------- 	*/
/* rename_column.c				       	*/
/* --------------------------------------------------- 	*/
/* 						       	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "environ.h"
#include "timlib.h"
#include "piece.h"
#include "document.h"
#include "attribute.h"
#include "column.h"
#include "application.h"
#include "appaserver_parameter_file.h"
#include "appaserver.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
boolean output_process_script(
					char *process_filename,
					char *folder_name,
					char *old_attribute_name,
					char *attribute_name,
					char *database_datatype );

char *get_sys_string(			char *table_name,
					char *old_attribute_name,
					char *new_attribute_name,
					char *database_datatype );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *folder_name;
	char *old_attribute_name;
	char *new_attribute_name;
	char *really_yn;
	char *database_datatype;
	char *table_name;
	char *sys_string;
	ATTRIBUTE *attribute;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 7 )
	{
		fprintf(stderr,
"Usage: %s ignored process old_attribute folder new_attribute really_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	process_name = argv[ 2 ];
	old_attribute_name = argv[ 3 ];
	folder_name = argv[ 4 ];
	new_attribute_name = argv[ 5 ];
	really_yn = argv[ 6 ];

	appaserver_parameter_file = appaserver_parameter_file_new();

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

	table_name = get_table_name( application_name, folder_name );

	attribute =
		attribute_load_folder_attribute(
		application_name,
		folder_name,
		new_attribute_name );

	if ( !attribute )
	{
		char msg[ 1024 ];
		sprintf( msg, 
		"ERROR in add_column: cannot get attribute %s.%s",
			 folder_name,
			 new_attribute_name );
		appaserver_output_error_message(
			application_name, msg, (char *)0 /* login_name */ );
		exit( 1 );
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

	sys_string = get_sys_string(	table_name,
					old_attribute_name,
					new_attribute_name,
					database_datatype );

	printf( "<BR><p>%s\n", sys_string );

	if ( toupper( *really_yn ) == 'Y' )
	{
		fflush( stdout );
		if ( system( sys_string ) ){};
		printf( "<BR><h3>Process complete.</h3>\n" );
		process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	}
	else
	{
		char process_filename[ 1024 ];
		char sys_string[ 1024 ];

		sprintf( process_filename,
			 "%s/rename_column.sh",
			 appaserver_parameter_file->
				appaserver_data_directory );

		printf( "<BR><h3>Process not executed.</h3>\n" );

		if ( !output_process_script(
				process_filename,
				folder_name,
				old_attribute_name,
				attribute->attribute_name,
				database_datatype ) )
		{
			printf( "<BR><h2>ERROR: Cannot create %s</h2>\n",
				process_filename );
		}
		else
		{
			sprintf( sys_string,
				 "chmod +x,g+w %s",
				 process_filename );
			if ( system( sys_string ) ){};
			printf( "<BR><p>Created %s</p>\n", process_filename );
		}
	}

	document_close();
	exit( 0 );
}

char *get_sys_string(	char *table_name,
			char *old_attribute_name,
			char *new_attribute_name,
			char *database_datatype )
{
	char buffer[ 1024 ];

	sprintf( buffer,
"echo \"alter table %s change %s %s %s;\" | sql.e 2>&1 | html_paragraph_wrapper",
		 table_name,
		 old_attribute_name,
		 new_attribute_name,
		 database_datatype );

	return strdup( buffer );
}

boolean output_process_script(
				char *process_filename,
				char *folder_name,
				char *old_attribute_name,
				char *attribute_name,
				char *database_datatype )
{
	FILE *output_file;

	if ( ! ( output_file = fopen( process_filename, "w" ) ) )
		return 0;

	fprintf( output_file,
"#!/bin/sh\n" );

	fprintf( output_file,
"if [ \"$APPASERVER_DATABASE\" != \"\" ]\n"
"then\n"
"	application=$APPASERVER_DATABASE\n"
"elif [ \"$DATABASE\" != \"\" ]\n"
"then\n"
"	application=$DATABASE\n"
"fi\n"
"\n"
"if [ \"$application\" = \"\" ]\n"
"then\n"
"	echo \"Error in `basename.e $0 n`: you must first:\" 1>&2\n"
"	echo \"$ . set_database\" 1>&2\n"
"	exit 1\n"
"fi\n" );

	fprintf( output_file,
"table_name=`get_table_name $application %s`\n",
		 folder_name );

	fprintf( output_file,
"echo \"alter table $table_name change %s %s %s;\" | sql.e 2>&1 | grep -v 'Unknown column'\n",
		 old_attribute_name,
		 attribute_name,
		 database_datatype );

	fprintf( output_file,
"exit 0\n" );

	fclose( output_file );
	return 1;
}

