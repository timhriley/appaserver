/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/import_predictbooks.c		*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include "environ.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "document.h"
#include "sql.h"
#include "import_predict.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *process_name;
	boolean execute_boolean;
	IMPORT_PREDICT *import_predict;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 6 )
	{
		fprintf(stderr,
		"Usage: %s session login role process execute_yn\n",
			argv[ 0 ] );

		exit ( 1 );
	}

	session_key = argv[ 1 ];
	login_name = argv[ 2 ];
	role_name = argv[ 3 ];
	process_name = argv[ 4 ];
	execute_boolean = (*argv[ 5 ] == 'y');

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		process_name /* title */ );

	import_predict =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		import_predict_new(
			IMPORT_PREDICT_SQLGZ,
			application_name,
			session_key,
			login_name,
			role_name,
			appaserver_parameter_mount_point() );

	if ( import_predict->import_predict_input->template_boolean )
	{
		printf(	"%s\n",
			IMPORT_PREDICT_TEMPLATE_MESSAGE );
	}

	if ( !import_predict->import_predict_input->entity_self )
	{
		printf( "%s\n",
			IMPORT_PREDICT_ENTITY_SELF_MESSAGE );
		document_close();
		exit( 0 );
	}

	if ( import_predict->import_predict_input->exists_boolean
	&&   !execute_boolean )
	{
		printf( "%s\n",
			IMPORT_PREDICT_EXISTS_MESSAGE );
	}

	if ( execute_boolean )
	{
		if ( system( import_predict->system_string ) ){}

		if ( system(
			import_predict->
				delete_role_system_string ) ){}

		printf( "%s\n",
			IMPORT_PREDICT_ONCE_MESSAGE );

		printf(	"%s\n",
			IMPORT_PREDICT_REFRESH_MESSAGE );

		printf(	"%s\n",
			import_predict->menu_anchor_tag );

		printf( "<h3>Reminder:</h3>\n" );

		printf( "%s\n",
			IMPORT_PREDICT_SHORTCUT_MESSAGE );

		printf( "<h3>Next step:</h3>\n" );

		printf( "%s\n",
			IMPORT_PREDICT_NEXT_STEP_MESSAGE );

		printf( "<h3>Process complete</h3>\n" );
	}
	else
	{
		printf( "%s\n",
			IMPORT_PREDICT_SHORTCUT_MESSAGE );

		printf( "<h3>Hint message complete</h3>\n" );
	}

	document_close();

	return 0;
}

