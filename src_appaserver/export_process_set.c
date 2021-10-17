/* --------------------------------------------------- 	*/
/* src_appaserver/export_process_set.c		       	*/
/* --------------------------------------------------- 	*/
/* 						       	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "environ.h"
#include "timlib.h"
#include "query.h"
#include "document.h"
#include "folder.h"
#include "application.h"
#include "list.h"
#include "appaserver_parameter_file.h"
#include "create_clone_filename.h"
#include "session.h"
#include "dictionary2file.h"
#include "dictionary_separate.h"

/* Constants */
/* --------- */
#define SHELL_HERE_LABEL			"all_done"
#define EXPORT_SUBSCHEMA_FILENAME_TEMPLATE	"%s/export_process_set.sh"

/* Prototypes */
/* ---------- */
void clone_table_javascript_files(
			char *export_process_filename,
			CREATE_CLONE_FILENAME *create_clone_filename,
			char *application_name,
			char *process_set_name,
			char *session,
			char *login_name,
			char *role_name );

LIST *get_javascript_filename_list(
			char *application_name,
			char *process_name );

LIST *get_folder_name_list(
			boolean exclude_roles );

LIST *export_process_dictionary_get_index_list(
			DICTIONARY *d,
			char *search_key_without_prefix );

LIST *get_prompt_list(
			char *application_name,
			char *process_name );

LIST *export_process_set_get_process_list(
			char *application_name,
			char *process_name );

LIST *get_drop_down_prompt_list(
			char *application_name,
			char *process_set_name );

void output_export_process_shell_script_initialize(
			char *export_process_filename,
			CREATE_CLONE_FILENAME *create_clone_filename );

void output_export_process_shell_script_process(
			char *export_process_filename,
			CREATE_CLONE_FILENAME *create_clone_filename,
			char *folder_name );

void output_export_process_shell_script_finish(
			char *export_process_filename );

void clone_table_process_set(
			char *export_process_filename,
			CREATE_CLONE_FILENAME *create_clone_filename,
			char *application_name,
			char *process_set_name,
			char *session,
			char *login_name,
			char *role_name );

void clone_table_generic(
			char *export_process_filename,
			CREATE_CLONE_FILENAME *create_clone_filename,
			char *application_name,
			char *process_name,
			char *folder_name,
			char with_delete_yn,
			char *session,
			char *login_name,
			char *role_name );

void clone_table_process(
			char *export_process_filename,
			CREATE_CLONE_FILENAME *create_clone_filename,
			char *application_name,
			char *process_set_name,
			char *session,
			char *login_name,
			char *role_name );

void clone_table_prompt(
			char *export_process_filename,
			CREATE_CLONE_FILENAME *create_clone_filename,
			char *application_name,
			char *process_set_name,
			char *session,
			char *login_name,
			char *role_name );

void clone_table_drop_down_prompt(
			char *export_process_filename,
			CREATE_CLONE_FILENAME *create_clone_filename,
			char *application_name,
			char *process_set_name,
			char *session,
			char *login_name,
			char *role_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *session;
	char *login_name;
	char *role_name;
	char *execution_process_name;
	char *process_set_name;
	DOCUMENT *document;
	LIST *process_set_name_list;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *dictionary_string;
	char export_process_filename[ 512 ];
	char *exclude_roles_yn = {0};
	boolean exclude_roles;
	CREATE_CLONE_FILENAME *create_clone_filename;
	DICTIONARY_SEPARATE *dictionary_separate;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 6 )
	{
		fprintf(stderr,
		"Usage: %s session login_name role process dictionary\n",
			argv[ 0 ] );
		exit( 1 );
	}

	session = argv[ 1 ];
	login_name = argv[ 2 ];
	role_name = argv[ 3 ];
	execution_process_name = argv[ 4 ];
	dictionary_string = argv[ 5 ];

	appaserver_parameter_file = appaserver_parameter_file_new();

	create_clone_filename =
		create_clone_filename_new(
			application_name,
			appaserver_parameter_file->
				appaserver_data_directory );

	dictionary_separate =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		dictionary_separate_string_new(
			dictionary_string );

	parameter_dictionary =
		dictionary_separate->
			working_post_dictionary;

	dictionary_get_index_data( 	&exclude_roles_yn,
					parameter_dictionary,
					"exclude_roles_yn",
					0 );

	exclude_roles = exclude_roles_yn && (*exclude_roles_yn == 'y' );

	process_set_name_list =
		export_process_dictionary_get_index_list(
						parameter_dictionary,
						"process_set" );

	create_clone_filename->folder_name_list =
		get_folder_name_list( exclude_roles );

	process_set_name_list =
		appaserver_library_trim_carrot_number(
			process_set_name_list );

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

	sprintf( export_process_filename,
		 EXPORT_SUBSCHEMA_FILENAME_TEMPLATE,
		 appaserver_parameter_file->appaserver_data_directory );

	if ( !list_length( process_set_name_list ) )
	{
		printf( "<P>No process sets selected.\n" );
		document_close();
		exit( 0 );
	}

	output_export_process_shell_script_initialize(
			export_process_filename,
			create_clone_filename );

	list_rewind( process_set_name_list );
	do {
		process_set_name = list_get_pointer( process_set_name_list );

		clone_table_process_set(export_process_filename,
					create_clone_filename,
					application_name,
					process_set_name,
					session,
					login_name,
					role_name );

		if ( !exclude_roles )
		{
			clone_table_generic(
					export_process_filename,
					create_clone_filename,
					application_name,
					process_set_name,
					"role_process_set_member",
					'n' /* with_delete */,
					session,
					login_name,
					role_name );
		}

		clone_table_generic(
					export_process_filename,
					create_clone_filename,
					application_name,
					process_set_name,
					"process_set_parameter",
					'y' /* with_delete */,
					session,
					login_name,
					role_name );

		clone_table_generic(
					export_process_filename,
					create_clone_filename,
					application_name,
					process_set_name,
					"javascript_process_sets",
					'y' /* with_delete */,
					session,
					login_name,
					role_name );

		clone_table_prompt(
					export_process_filename,
					create_clone_filename,
					application_name,
					process_set_name,
					session,
					login_name,
					role_name );

		clone_table_process(
					export_process_filename,
					create_clone_filename,
					application_name,
					process_set_name,
					session,
					login_name,
					role_name );

		clone_table_drop_down_prompt(
					export_process_filename,
					create_clone_filename,
					application_name,
					process_set_name,
					session,	
					login_name,
					role_name );

		clone_table_javascript_files(
					export_process_filename,
					create_clone_filename,
					application_name,
					process_set_name,
					session,
					login_name,
					role_name );

	} while( list_next( process_set_name_list ) );

	output_export_process_shell_script_finish(
			export_process_filename );

	printf( "<p>Created %s\n", export_process_filename );

	document_close();

	process_increment_execution_count(
				application_name,
				execution_process_name,
				appaserver_parameter_file_get_dbms() );
	exit( 0 );
} /* main() */

void clone_table_process_set(
			char *export_process_filename,
			CREATE_CLONE_FILENAME *create_clone_filename,
			char *application_name,
			char *process_set_name,
			char *session,
			char *login_name,
			char *role_name )
{
	char sys_string[ 4096 ];
	char *where_data;

	where_data = process_set_name;

	sprintf(sys_string,
"clone_folder %s y %s %s %s %s %s %s \"%s\" \"%s\" nohtml %s %s \"\" %s >/dev/null",
			application_name,
			session,
			login_name,
			role_name,
			application_name,
			"process_set",
			"process_set",
			where_data,
			where_data,
			"y" /* delete_yn */,
			"n" /* really_yn */,
			"y" /* output2file_yn */ );

	if ( system( sys_string ) ){};

	output_export_process_shell_script_process(
				export_process_filename,
				create_clone_filename,
				"process_set" );

} /* clone_table_process_set() */

void clone_table_generic(
			char *export_process_filename,
			CREATE_CLONE_FILENAME *create_clone_filename,
			char *application_name,
			char *process_set_name,
			char *folder_name,
			char with_delete_yn,
			char *session,
			char *login_name,
			char *role_name )
{
	char sys_string[ 1024 ];
	char *where_data;

	where_data = process_set_name;

	sprintf(sys_string,
"clone_folder %s y %s %s %s %s %s %s \"%s\" \"%s\" nohtml %c %s \"\" %s >/dev/null",
			application_name,
			session,
			login_name,
			role_name,
			application_name,
			folder_name,
			"process_set",
			where_data,
			where_data,
			with_delete_yn,
			"n" /* really_yn */,
			"y" /* output2file_yn */ );

	if ( system( sys_string ) ){};

	output_export_process_shell_script_process(
				export_process_filename,
				create_clone_filename,
				folder_name );

} /* clone_table_generic() */

void output_export_process_shell_script_initialize(
				char *export_process_filename,
				CREATE_CLONE_FILENAME *create_clone_filename )
{
	FILE *export_process_file;
	char *folder_name;

	export_process_file = fopen( export_process_filename, "w" );

	if ( !export_process_file )
	{
		printf( "<P>ERROR opening %s for write.\n",
			export_process_filename );
		document_close();
		exit( 0 );
	}

	environ_output_application_shell( export_process_file );

	list_rewind( create_clone_filename->folder_name_list );
	do {
		folder_name =
			list_get_pointer(
				create_clone_filename->folder_name_list );

		/* This used to execute `get_table_name` */
		/* ------------------------------------- */
		fprintf( export_process_file,
			 "%s=\"%s\"\n",
			 folder_name,
			 folder_name );

	} while( list_next( create_clone_filename->folder_name_list ) );

	fprintf(export_process_file,
		"(\ncat << %s\n",
		SHELL_HERE_LABEL );

	fclose( export_process_file );

} /* output_export_process_shell_script_initialize() */

void output_export_process_shell_script_process(
				char *export_process_filename,
				CREATE_CLONE_FILENAME *create_clone_filename,
				char *folder_name )
{
	char sys_string[ 1024 ];
	char clone_folder_filename[ 512 ];

	create_clone_filename_get_clone_folder_compressed_filename(
		clone_folder_filename,
		&create_clone_filename->directory_name,
		create_clone_filename->application_name,
		create_clone_filename->appaserver_data_directory,
		folder_name );

	sprintf(sys_string,
			"zcat %s					|"
			"sed 's/delete from /delete from \\$/'		|"
			"sed 's/insert into /insert into \\$/'		|"
			"cat >> %s",
			clone_folder_filename,
			export_process_filename );

	if ( system( sys_string ) ){};

	sprintf(sys_string,
			"rm %s",
			clone_folder_filename );

	if ( system( sys_string ) ){};

} /* output_export_process_shell_script_process() */

void output_export_process_shell_script_finish(
				char *export_process_filename )
{
	FILE *export_process_file;
	char sys_string[ 1024 ];

	export_process_file = fopen( export_process_filename, "a" );

	fprintf(export_process_file,
		"%s\n) | sql.e 2>&1 | grep -iv duplicate\n\nexit 0\n",
		SHELL_HERE_LABEL );

	fclose( export_process_file );

	sprintf( sys_string,
		 "chmod +x,g+w %s",
		 export_process_filename );
	if ( system( sys_string ) ){};

} /* output_export_process_shell_script_finish() */

void clone_table_process(
			char *export_process_filename,
			CREATE_CLONE_FILENAME *create_clone_filename,
			char *application_name,
			char *process_set_name,
			char *session,
			char *login_name,
			char *role_name )
{
	char sys_string[ 1024 ];
	char *where_data;
	char *process;
	LIST *process_list;

	process_list = 
		export_process_set_get_process_list(
			application_name, process_set_name );

	if ( !list_rewind( process_list ) ) return;
	
	do {
		process = list_get_pointer( process_list );

		where_data = process;
	
		sprintf(sys_string,
"clone_folder %s y %s %s %s %s %s %s \"%s\" \"%s\" nohtml %s %s \"\" %s >/dev/null",
				application_name,
				session,
				login_name,
				role_name,
				application_name,
				"process",
				"process",
				where_data,
				where_data,
				"y" /* delete_yn */,
				"n" /* really_yn */,
				"y" /* output2file_yn */ );
	
		if ( system( sys_string ) ){};

		output_export_process_shell_script_process(
			export_process_filename,
			create_clone_filename,
			"process" );

	} while( list_next( process_list ) );

} /* clone_table_process() */

void clone_table_prompt(
			char *export_process_filename,
			CREATE_CLONE_FILENAME *create_clone_filename,
			char *application_name,
			char *process_set_name,
			char *session,
			char *login_name,
			char *role_name )
{
	char sys_string[ 1024 ];
	char *where_data;
	char *prompt;
	LIST *prompt_list;

	prompt_list = 
		get_prompt_list( application_name, process_set_name );

	if ( !list_rewind( prompt_list ) ) return;
	
	do {
		prompt = list_get_pointer( prompt_list );

		where_data = prompt;
	
		sprintf(sys_string,
"clone_folder %s y %s %s %s %s %s %s \"%s\" \"%s\" nohtml %s %s \"\" %s >/dev/null",
				application_name,
				session,
				login_name,
				role_name,
				application_name,
				"prompt",
				"prompt",
				where_data,
				where_data,
				"n" /* delete_yn */,
				"n" /* really_yn */,
				"y" /* output2file_yn */ );
	
		if ( system( sys_string ) ){};

		output_export_process_shell_script_process(
			export_process_filename,
			create_clone_filename,
			"prompt" );

	} while( list_next( prompt_list ) );

} /* clone_table_prompt() */

void clone_table_drop_down_prompt(
			char *export_process_filename,
			CREATE_CLONE_FILENAME *create_clone_filename,
			char *application_name,
			char *process_set_name,
			char *session,
			char *login_name,
			char *role_name )
{
	char sys_string[ 1024 ];
	char *where_data;
	char *drop_down_prompt;
	LIST *drop_down_prompt_list;

	drop_down_prompt_list = 
		get_drop_down_prompt_list( application_name, process_set_name );

	if ( !list_rewind( drop_down_prompt_list ) ) return;
	
	do {
		drop_down_prompt = list_get_pointer( drop_down_prompt_list );

		where_data = drop_down_prompt;
	
		sprintf(sys_string,
"clone_folder %s y %s %s %s %s %s %s \"%s\" \"%s\" nohtml %s %s \"\" %s >/dev/null",
			application_name,
			session,
			login_name,
			role_name,
			application_name,
			"drop_down_prompt",
			"drop_down_prompt",
			where_data,
			where_data,
			"n" /* delete_yn */,
			"n" /* really_yn */,
			"y" /* output2file_yn */ );
	
		if ( system( sys_string ) ){};
	
		output_export_process_shell_script_process(
			export_process_filename,
			create_clone_filename,
			"drop_down_prompt" );

		sprintf(sys_string,
"clone_folder %s y %s %s %s %s %s %s \"%s\" \"%s\" nohtml %s %s \"\" %s >/dev/null",
			application_name,
			session,
			login_name,
			role_name,
			application_name,
			"drop_down_prompt_data",
			"drop_down_prompt",
			where_data,
			where_data,
			"n" /* delete_yn */,
			"n" /* really_yn */,
			"y" /* output2file_yn */ );

		if ( system( sys_string ) ){};

		output_export_process_shell_script_process(
			export_process_filename,
			create_clone_filename,
			"drop_down_prompt_data" );

	} while( list_next( drop_down_prompt_list ) );

} /* clone_table_drop_down_prompt() */

LIST *get_prompt_list( char *application_name, char *process_set_name )
{
	char sys_string[ 1024 ];
	char where_clause[ 512 ];

	sprintf(where_clause,
		"process_set = '%s' and prompt != 'null'",
		process_set_name );

	sprintf(sys_string,
		"get_folder_data	application=%s			"
		"			select=prompt			"
		"			folder=process_set_parameter	"
		"			where=\"%s\"			",
		application_name,
		where_clause );

	return pipe2list( sys_string );
} /* get_prompt_list() */

LIST *export_process_set_get_process_list(
					char *application_name,
					char *process_set_name )
{
	char sys_string[ 1024 ];
	char where_clause[ 512 ];

	sprintf(where_clause,
		"process_set = '%s' and process != 'null'",
		process_set_name );

	sprintf(sys_string,
		"get_folder_data	application=%s			 "
		"			select=process			 "
		"			folder=role_process_set_member	 "
		"			where=\"%s\"			|"
		"sort -u						 ",
		application_name,
		where_clause );

	return pipe2list( sys_string );
} /* export_process_set_get_process_list() */

LIST *get_drop_down_prompt_list(
			char *application_name,
			char *process_set_name )
{
	char sys_string[ 1024 ];
	char where_clause[ 512 ];

	sprintf(where_clause,
		"process_set = '%s' and drop_down_prompt != 'null'",
		process_set_name );

	sprintf(sys_string,
		"get_folder_data	application=%s			"
		"			select=drop_down_prompt		"
		"			folder=process_set_parameter	"
		"			where=\"%s\"			",
		application_name,
		where_clause );

	return pipe2list( sys_string );
} /* get_drop_down_prompt_list() */

LIST *export_process_dictionary_get_index_list(
					DICTIONARY *d,
					char *search_key_without_prefix )
{
	LIST *return_list = create_list();
	char *data;
	int index;
	int results;

	for( index = 1 ;; index++ )
	{
		results =
			dictionary_get_index_data(
					&data,
					d,
					search_key_without_prefix,
					index );
		if ( results < 1 ) return return_list;

		list_append_string( return_list, data );

	}
} /* export_process_dictionary_get_index_list() */

LIST *get_folder_name_list( boolean exclude_roles )
{
	LIST *folder_name_list;

	folder_name_list = list_new_list();

	list_append_pointer( folder_name_list, "process_set" );
	list_append_pointer( folder_name_list, "process_set_parameter" );
	list_append_pointer( folder_name_list, "javascript_process_sets" );
	list_append_pointer( folder_name_list, "javascript_files" );
	list_append_pointer( folder_name_list, "prompt" );
	list_append_pointer( folder_name_list, "process" );
	list_append_pointer( folder_name_list, "drop_down_prompt" );
	list_append_pointer( folder_name_list, "drop_down_prompt_data" );

	if ( !exclude_roles )
	{
		list_append_pointer(	folder_name_list,
					"role_process_set_member" );
	}
	return folder_name_list;
} /* get_folder_name_list() */

void clone_table_javascript_files(
			char *export_process_filename,
			CREATE_CLONE_FILENAME *create_clone_filename,
			char *application_name,
			char *process_set_name,
			char *session,
			char *login_name,
			char *role_name )
{
	char sys_string[ 1024 ];
	char *where_data;
	char *javascript_filename;
	LIST *javascript_filename_list;

	javascript_filename_list = 
		get_javascript_filename_list(
			application_name, process_set_name );

	if ( !list_rewind( javascript_filename_list ) ) return;
	
	do {
		javascript_filename =
			list_get_pointer(
				javascript_filename_list );

		where_data = javascript_filename;
	
		sprintf(sys_string,
"clone_folder %s y %s %s %s %s %s %s \"%s\" \"%s\" nohtml %s %s \"\" %s >/dev/null",
				application_name,
				session,
				login_name,
				role_name,
				application_name,
				"javascript_files",
				"javascript_filename",
				where_data,
				where_data,
				"n" /* delete_yn */,
				"n" /* really_yn */,
				"y" /* output2file_yn */ );
	
		if ( system( sys_string ) ){};

		output_export_process_shell_script_process(
				export_process_filename,
				create_clone_filename,
				"javascript_files" );

	} while( list_next( javascript_filename_list ) );

} /* clone_table_javascript_files() */

LIST *get_javascript_filename_list(	char *application_name,
					char *process_set_name )
{
	char sys_string[ 1024 ];
	char where_clause[ 512 ];

	sprintf(where_clause,
		"process_set = '%s'",
		process_set_name );

	sprintf(sys_string,
		"get_folder_data	application=%s			"
		"			select=javascript_filename	"
		"			folder=javascript_process_sets	"
		"			where=\"%s\"			",
		application_name,
		where_clause );

	return pipe2list( sys_string );
} /* get_javascript_filename_list() */

