/* --------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_appaserver/add_column.c	 	*/
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
#include "attribute.h"
#include "application.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "session.h"
#include "boolean.h"
#include "related_folder.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void output_relation(		FILE *output_file,
				char *application_name,
				char *folder_name,
				char *attribute_name );

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
				char *post_change_javascript,
				char *on_focus_javascript_function,
				char lookup_histogram_output_yn,
				char lookup_time_chart_output_yn,
				boolean is_system_attribute,
				int primary_key_index,
				int display_order,
				char omit_insert_yn,
				char omit_insert_prompt_yn,
				char omit_update_yn,
				char additional_unique_index_yn,
				char additional_index_yn,
				char insert_required_yn,
				char lookup_required_yn );

char *get_sys_string(		char *table_name,
				char *attribute_name,
				char *database_datatype );

int main( int argc, char **argv )
{
	char *application_name;
	char *folder_name;
	char *attribute_name;
	char *table_name;
	char *really_yn;
	char *sys_string;
	DOCUMENT *document;
	ATTRIBUTE *attribute;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *login_name;
	char *database_datatype;
	boolean is_system_attribute;

	/* Exits if fails. */
	/* --------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 8 )
	{
		fprintf(stderr,
"Usage: %s ignored ignored login_name ignored folder attribute really_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	/* session = argv[ 2 ]; */
	login_name = argv[ 3 ];
	/* role_name = argv[ 4 ]; */
	folder_name = argv[ 5 ];
	attribute_name = argv[ 6 ];

	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( !*attribute_name
	||   strcmp( attribute_name, "attribute" ) == 0 )
	{
		document_quick_output_body(
				application_name,
				appaserver_parameter_file->
					appaserver_mount_point );

		printf( "<h3>Please select a Folder Attribute.</h3>\n" );
		document_close();
		exit( 0 );
	}

	really_yn = argv[ 7 ];

	table_name = get_table_name( application_name, folder_name );

	attribute =
		attribute_load_folder_attribute(
		application_name,
		folder_name,
		attribute_name );

	if ( !attribute )
	{
		char msg[ 1024 ];
		sprintf( msg, 
		"ERROR in add_column: cannot get attribute %s.%s\n",
			 folder_name,
			 attribute_name );
		appaserver_output_error_message(
			application_name, msg, login_name );
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
		attribute_get_database_datatype(
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
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(
			document->application_name,
			document->onload_control_string );

	sys_string = get_sys_string(	table_name,
					attribute_name,
					database_datatype );

	printf( "<BR><p>%s\n", sys_string );

	if ( toupper( *really_yn ) == 'Y' )
	{
		char *results;

		fflush( stdout );
		results = pipe2string( sys_string );
		if ( results )
			printf( "<p>%s\n", results );
		printf( "<BR><h3>Process complete</h3>\n" );
	}
	else
	{
		char process_filename[ 1024 ];
		char sys_string[ 1024 ];

		sprintf( process_filename,
			 "%s/add_column_%s.sh",
			 appaserver_parameter_file->
				appaserver_data_directory,
			 attribute_name );

		is_system_attribute = attribute->appaserver;

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
				attribute->post_change_javascript,
				attribute->on_focus_javascript_function,
				(attribute->lookup_histogram_output)
					? 'y' : 'n',
				(attribute->lookup_time_chart_output)
					? 'y' : 'n',
				is_system_attribute,
				attribute->primary_key_index,
				attribute->display_order,
				(attribute->omit_insert) ? 'y' : 'n',
				(attribute->omit_insert_prompt) ? 'y' : 'n',
				(attribute->omit_update) ? 'y' : 'n',
				(attribute->additional_unique_index)
					? 'y' : 'n',
				(attribute->additional_index) ? 'y' : 'n',
				(attribute->insert_required) ? 'y' : 'n',
				(attribute->lookup_required) ? 'y' : 'n' ) )
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

	sprintf( buffer,
		 "echo \"alter table %s add %s %s;\" | sql.e 2>&1",
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
				char *post_change_javascript,
				char *on_focus_javascript_function,
				char lookup_histogram_output_yn,
				char lookup_time_chart_output_yn,
				boolean is_system_attribute,
				int primary_key_index,
				int display_order,
				char omit_insert_yn,
				char omit_insert_prompt_yn,
				char omit_update_yn,
				char additional_unique_index_yn,
				char additional_index_yn,
				char insert_required_yn,
				char lookup_required_yn )
{
	FILE *output_file;
	char buffer[ 2048 ];

	if ( ! ( output_file = fopen( process_filename, "w" ) ) )
		return 0;

	environ_output_application_shell( output_file );

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
"echo \"alter table $table_name add %s %s;\" | sql.e\n",
		 attribute_name,
		 database_datatype );

	fprintf( output_file,
"folder_attribute_table=`get_table_name $application folder_attribute`\n" );
	fprintf( output_file,
"echo \"insert into $folder_attribute_table				\\\n"
"	(folder,attribute,primary_key_index,display_order,		\\\n"
"	 omit_insert_yn,omit_insert_prompt_yn,omit_update_yn,		\\\n"
"	 additional_unique_index_yn,					\\\n"
"	 additional_index_yn,						\\\n"
"	 insert_required_yn,						\\\n"
"	 lookup_required_yn )						\\\n"
"	values								\\\n"
"	('%s','%s',%d,%d,'%c','%c','%c','%c','%c','%c','%c');\" 	 |\n"
"sql.e\n",
		 folder_name,
		 attribute_name,
		 primary_key_index,
		 display_order,
		 omit_insert_yn,
		 omit_insert_prompt_yn,
		 omit_update_yn,
		 additional_unique_index_yn,
		 additional_index_yn,
		 insert_required_yn,
		 lookup_required_yn );

	fprintf( output_file,
"attribute_table=`get_table_name $application attribute`\n" );

	strcpy( buffer, hint_message );

	fprintf( output_file,
"echo \"insert into $attribute_table					\\\n"
"	(attribute,attribute_datatype,width,float_decimal_places,	\\\n"
"	 hint_message,appaserver_yn,					\\\n"
"	 post_change_javascript,					\\\n"
"	 on_focus_javascript_function,					\\\n"
"	 lookup_histogram_output_yn,					\\\n"
"	 lookup_time_chart_output_yn )					\\\n"
"	values								\\\n"
"	('%s','%s',%d,%d,'%s','%c','%s','%s','%c','%c');\"		 |\n"
"sql.e\n",
		 attribute_name,
		 attribute_datatype,
		 width,
		 float_decimal_places,
		 escape_single_quotes( buffer ),
		 (is_system_attribute) ? 'y' : 'n',
		 post_change_javascript,
		 on_focus_javascript_function,
		 lookup_histogram_output_yn,
		 lookup_time_chart_output_yn );

	output_relation(	output_file,
				application_name,
				folder_name,
				attribute_name );

	fprintf( output_file,
") 2>&1 | grep -vi duplicate\n" );
	fprintf( output_file,
"exit 0\n" );

	fclose( output_file );
	return 1;
} /* output_process_script() */

void output_relation(	FILE *output_file,
			char *application_name,
			char *folder_name,
			char *attribute_name )
{
	LIST *mto1_related_folder_list;
	RELATED_FOLDER *related_folder;

	mto1_related_folder_list =
		related_folder_get_mto1_related_folder_list(
			list_new_list(),
			application_name,
			(char *)0 /* session */,
			folder_name,
			(char *)0 /* role_name */,
			0 /* isa_flag */,
			related_folder_no_recursive,
			1 /* override_row_restrictions */,
			(LIST *)0 /* root_primary_attribute_name_list */,
			0 /* recursive_level */ );

	if ( ( related_folder =
	       related_folder_attribute_consumes_related_folder(
			(LIST **)0 /* foreign_attribute_name_list */,
			(LIST *)0 /* done_attribute_name_list */,
			(LIST *)0 /* omit_update_attribute_name_list */,
			mto1_related_folder_list,
			attribute_name,
			(LIST *)0 /* include_attribute_name_list */ ) ) )
	{
		fprintf( output_file,
"relation_table=`get_table_name $application relation`\n" );
	fprintf( output_file,
"echo \"insert into $relation_table					\\\n"
"	(folder,related_folder,related_attribute)			\\\n"
"	values								\\\n"
"	('%s','%s','%s');\"						 |\n"
"sql.e\n",
			related_folder->
				one2m_folder->
					folder_name,
			related_folder->folder->folder_name,
			related_folder->related_attribute_name );
	}

} /* output_relation() */

