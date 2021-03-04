/* ------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/grant_select_to_user.c	*/
/* ------------------------------------------------------	*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "application_constants.h"
#include "appaserver_user.h"
#include "appaserver_parameter_file.h"
#include "document.h"
#include "element.h"
#include "timlib.h"
#include "piece.h"
#include "date.h"
#include "environ.h"
#include "application.h"
#include "process.h"

/* Structures */
/* ---------- */

/* Constants */
/* --------- */
#define CREATE_USER_COMMAND	"create user"
#define DROP_USER_COMMAND 	"drop user"

#define ENCRYPTED_PASSWORD_LENGTH	41

/* Prototypes */
/* ---------- */
/*
char *get_ip_address(	char *application_name );
*/

char *get_revoke_only_sys_string(
			char *login_host_name,
			char really_yn );

char *get_sys_string(	char **generated_password,
			char *application_name,
			char *login_host_name,
			char really_yn );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *login_name;
	char buffer[ 128 ];
	char *connect_from_host;
	char login_host_name[ 256 ];
	char revoke_only_yn;
	char really_yn;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *sys_string;
	char *generated_password = {0};

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 7 )
	{
		fprintf( stderr, 
"Usage: %s ignored process login_name connect_from_host revoke_only_yn really_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 2 ];
	login_name = argv[ 3 ];
	connect_from_host = argv[ 4 ];
	revoke_only_yn = *argv[ 5 ];
	really_yn = *argv[ 6 ];

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

	printf( "<h2>%s\n", format_initial_capital( buffer, process_name ) );
	fflush( stdout );
	if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ){};
	printf( "</h2>\n" );
	fflush( stdout );

	sprintf( login_host_name,
		 "'%s'@'%s'",
		 login_name,
		 connect_from_host );

	if ( revoke_only_yn == 'y' )
	{
		sys_string = get_revoke_only_sys_string(
					login_host_name,
					really_yn );
	}
	else
	{
		sys_string = get_sys_string(	&generated_password,
						application_name,
						login_host_name,
						really_yn );
	}

	if ( system( sys_string ) ){};

	if ( really_yn == 'y' )
	{
		printf( "<p>Process complete.\n" );

		if ( revoke_only_yn != 'y' )
		{
			printf(
"<p>Password generated: %s\n", generated_password );

			printf(
"<p>Place this password in %s:/etc/appaserver_%s.config\n",
			connect_from_host,
			application_name );

			printf(
"<p>You may need to edit /etc/mysql/mysql.conf.d/mysqld.cnf and comment out \"bind-address = 127.0.0.1\"\n" );
		}
	}
	else
	{
		printf( "<p>Process not executed.\n" );
	}

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
}

char *get_sys_string(	char **generated_password,
			char *application_name,
			char *login_host_name,
			char really_yn )
{
	static char sys_string[ 2048 ];
	char execution_process[ 512 ];
	char sed_string[ 512 ];
	char testing_processes[ 1024 ];
	char grant_update_process[ 512 ];
	char flush_privileges_process[ 512 ];

	/* Grant update to PROCESS to increment execution_count */
	/* ---------------------------------------------------- */
	if ( really_yn == 'y' )
	{
		sprintf( grant_update_process,
"echo \"grant update (execution_count ) on %s.process to %s;\" |"
"sql.e '^'",
			application_name,
		 	login_host_name );
	}
	else
	{
		sprintf( grant_update_process,
"echo \"grant update (execution_count) on %s.process to %s;\" |"
"html_paragraph_wrapper.e | html_table.e 'Grant Update' '' '|'",
			application_name,
		 	login_host_name );
	}

	if ( really_yn == 'y' )
	{
		sprintf( flush_privileges_process,
"echo \"flush privileges;\" |"
"sql.e '^'" );
	}
	else
	{
		sprintf( flush_privileges_process,
"echo \"flush privileges;\" |"
"html_paragraph_wrapper.e | html_table.e 'Flush Privileges' '' '|'" );
	}

	if ( really_yn == 'y' )
	{
		strcpy( testing_processes, "cat" );
		strcpy( execution_process, "sql.e" );
		*generated_password = timlib_generate_password();
	}
	else
	{
		sprintf(testing_processes,
		 	"queue_top_bottom_lines.e 50			|"
		 	"html_table.e 'Grant Statements' '' '|'	 " );

		*generated_password = "**************************";
		strcpy( execution_process, "cat" );
	}

	sprintf(	sed_string,
			"sed \"s/.*/grant select on %s.& to %s;/\"",
			application_name,
			login_host_name );

	sprintf( sys_string,
	 "(echo \"%s if exists %s;\"					;"
	 "echo \"%s %s identified by '%s';\"				;"
	 "application_folder_list.sh %s n				|"
	 "grep -v '^null$'						|"
	 "sed 's/^application$/%s_application/'				|"
	 "%s								|"
	 "sort -u)							|"
	 "%s								|"
	 "%s								|"
	 "cat								;"
	 "%s								;"
	 "%s								 ",
		 DROP_USER_COMMAND,
		 login_host_name,
		 CREATE_USER_COMMAND,
		 login_host_name,
		 *generated_password,
		 application_name,
		 application_name,
		 sed_string,
		 testing_processes,
		 execution_process,
		 grant_update_process,
		 flush_privileges_process );

	return sys_string;

}

char *get_revoke_only_sys_string(
			char *login_host_name,
			char really_yn )
{
	static char sys_string[ 1024 ];

	if ( really_yn == 'y' )
	{
		sprintf( sys_string,
	"echo \"revoke all on *.* from %s;\" | sql.e;"
	"echo \"drop user %s;\" | sql.e '^' mysql mysql;"
	"echo \"flush privileges;\" | sql.e '^' mysql mysql",
			 login_host_name,
			 login_host_name );
	}
	else
	{
		sprintf( sys_string,
"echo \"revoke all on *.* from %s; | sql.e\" | html_paragraph_wrapper;"
"echo \"drop user %s | sql.e '^' mysql mysql\" | html_paragraph_wrapper;"
"echo \"flush privileges; | sql.e '^' mysql mysql\" | html_paragraph_wrapper",
			 login_host_name,
			 login_host_name );
	}

	return sys_string;

}
