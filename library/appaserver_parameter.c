/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/appaserver_parameter.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "piece.h"
#include "environ.h"
#include "application.h"
#include "dictionary.h"
#include "boolean.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"

static APPASERVER_PARAMETER *global_appaserver_parameter = {0};

char *appaserver_parameter_mount_point( void )
{
	if ( !global_appaserver_parameter )
		global_appaserver_parameter =
			appaserver_parameter_new();

	return global_appaserver_parameter->mount_point;
}

char *appaserver_parameter_cgi_directory( void )
{
	if ( !global_appaserver_parameter )
		global_appaserver_parameter =
			appaserver_parameter_new();

	return
	/* -------------------------- */
	/* Returns source_destination */
	/* -------------------------- */
	string_trim_trailing_character(
		global_appaserver_parameter->
			apache_cgi_directory,
		'/' );
}

char *appaserver_parameter_log_directory( void )
{
	if ( !global_appaserver_parameter )
		global_appaserver_parameter =
			appaserver_parameter_new();

	return
	/* -------------------------- */
	/* Returns source_destination */
	/* -------------------------- */
	string_trim_trailing_character(
		global_appaserver_parameter->
			log_directory,
		'/' );
}

char *appaserver_parameter_data_directory( void )
{
	if ( !global_appaserver_parameter )
		global_appaserver_parameter =
			appaserver_parameter_new();

	return
	/* -------------------------- */
	/* Returns source_destination */
	/* -------------------------- */
	string_trim_trailing_character(
		global_appaserver_parameter->
			data_directory,
		'/' );
}

char *appaserver_parameter_cgi_home( void )
{
	if ( !global_appaserver_parameter )
		global_appaserver_parameter =
			appaserver_parameter_new();

	return global_appaserver_parameter->cgi_home;
}

char *appaserver_parameter_document_root( void )
{
	if ( !global_appaserver_parameter )
		global_appaserver_parameter =
			appaserver_parameter_new();

	return global_appaserver_parameter->document_root;
}

char *appaserver_parameter_upload_directory( void )
{
	if ( !global_appaserver_parameter )
		global_appaserver_parameter =
			appaserver_parameter_new();

	return global_appaserver_parameter->upload_directory;
}

char *appaserver_parameter_backup_directory( void )
{
	if ( !global_appaserver_parameter )
		global_appaserver_parameter =
			appaserver_parameter_new();

	return global_appaserver_parameter->backup_directory;
}

char *appaserver_parameter_filename( char *application_name )
{
	char filename[ 128 ];

	if ( application_name )
	{
		sprintf(filename,
			APPASERVER_PARAMETER_APPLICATION_NAME,
			application_name );

		if ( application_file_exists_boolean( filename ) )
			return strdup( filename );
	}

	strcpy( filename, APPASERVER_PARAMETER_GENERIC_NAME );

	if ( application_file_exists_boolean( filename ) )
		return strdup( filename );

	fprintf(stderr,
		"Warning in %s/%s()/%d: cannot find %s.\n",
		__FILE__,
		__FUNCTION__,
		__LINE__,
		filename );

	return (char *)0;
}

FILE *appaserver_parameter_open_file( char *filename )
{
	FILE *file;

	if ( ! ( file = fopen( filename, "r" ) ) )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: fopen(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			filename );
		return (FILE *)0;
	}

	return file;
}

APPASERVER_PARAMETER *appaserver_parameter_new( void )
{
	char *application;

	application =
		environment_get(
			"APPASERVER_DATABASE" );

	if ( !application || !*application )
	{
		application =
			environment_get(
				"DATABASE" );
	}

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	appaserver_parameter_application(
		application );
}

APPASERVER_PARAMETER *appaserver_parameter_calloc( void )
{
	APPASERVER_PARAMETER *appaserver_parameter;

	if ( ! ( appaserver_parameter =
			calloc( 1, sizeof ( APPASERVER_PARAMETER ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return appaserver_parameter;
}

APPASERVER_PARAMETER *appaserver_parameter_fetch(
		APPASERVER_PARAMETER *appaserver_parameter,
		DICTIONARY *dictionary )
{
	APPASERVER_PARAMETER *s = appaserver_parameter;
	DICTIONARY *d = dictionary;
	char *a;

	a = "mysql_user";
	if ( ! ( s->mysql_user = dictionary_fetch( a, d ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: Cannot fetch %s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 a );
		exit( 1 );
	}

	a = "mysql_flags";
	if ( ! ( s->flags = dictionary_fetch( a, d ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: Cannot fetch %s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 a );
		exit( 1 );
	}

	a = "mysql_password";
	if ( ! ( s->password = dictionary_fetch( a, d ) ) )
	{
		a = "password";
		if ( ! ( s->password = dictionary_fetch( a, d ) ) )
		{
			fprintf( stderr,
			 	"Error in %s/%s()/%d: Cannot fetch %s\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__,
			 	a );
			exit( 1 );
		}
		s->mysql_password_syntax = 1;
	}

	a = "mysql_host";
	s->MYSQL_HOST = dictionary_fetch( a, d );

	a = "mysql_tcp_port";
	s->MYSQL_TCP_PORT = dictionary_fetch( a, d );

	a = "appaserver_mount_point";
	if ( ! ( s->mount_point =
			dictionary_fetch( a, d ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: Cannot fetch %s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 a );
		exit( 1 );
	}

	/* cgi home from the browser's perspective. */
	/* ---------------------------------------- */
	a = "apache_cgi_directory";
	if ( ! ( s->apache_cgi_directory = dictionary_fetch( a, d ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: Cannot fetch %s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 a );
		exit( 1 );
	}

	/* cgi home from the file system's perspective. */
	/* -------------------------------------------- */
	a = "cgi_home";
	s->cgi_home = dictionary_fetch( a, d );

	a = "document_root";
	s->document_root = dictionary_fetch( a, d );

	a = "log_directory";
	s->log_directory = dictionary_fetch( a, d );

	a = "data_directory";
	s->data_directory = dictionary_fetch( a, d );

	a = "upload_directory";
	s->upload_directory = dictionary_fetch( a, d );

	if ( !s->upload_directory )
	{
		s->upload_directory = s->data_directory;
	}

	a = "backup_directory";
	s->backup_directory = dictionary_fetch( a, d );

	if ( !s->backup_directory )
	{
		s->backup_directory = "/var/backups/appaserver";
	}

	return appaserver_parameter;
}

DICTIONARY *appaserver_parameter_dictionary( FILE *file )
{
	char buffer[ 4096 ];
	char key[ 1024 ];
	char data[ 3072 ];
	DICTIONARY *dictionary = dictionary_new();

	while( string_input( buffer, file, 1024 ) )
	{
		if ( *buffer == '[' ) continue;

		piece( key, '=', buffer, 0 );

		if ( piece( data, '=', buffer, 1 ) )
		{
			dictionary_set(
				dictionary,
				strdup( key ),
				strdup( data ) );
		}
	}

	return dictionary;
}

APPASERVER_PARAMETER *appaserver_parameter_application(
		char *application_name )
{
	APPASERVER_PARAMETER *appaserver_parameter;

	appaserver_parameter = appaserver_parameter_calloc();
	appaserver_parameter->application_name = application_name;

	appaserver_parameter->filename =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		appaserver_parameter_filename(
			application_name );

	if ( !appaserver_parameter->filename )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: appaserver_parameter_filename(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			application_name );

		exit( 1 );
	}

	appaserver_parameter->file =
		appaserver_parameter_open_file(
			appaserver_parameter->filename );

	if ( !appaserver_parameter->file )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: appaserver_parameter_open_file(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			appaserver_parameter->filename );

		exit( 1 );
	}

	appaserver_parameter->dictionary =
		appaserver_parameter_dictionary(
			appaserver_parameter->file );

	fclose( appaserver_parameter->file );

	return
	/* ---------------------------- */
	/* Returns appaserver_parameter */
	/* ---------------------------- */
	appaserver_parameter_fetch(
		appaserver_parameter,
		appaserver_parameter->dictionary );
}

char *appaserver_parameter_application_data_directory(
		char *application_name,
		char *data_directory )
{
	char application_data_directory[ 128 ];

	if ( !application_name
	||   !data_directory )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		application_data_directory,
		sizeof ( application_data_directory ),
		"%s/%s",
		data_directory,
		application_name );

	return strdup( application_data_directory );
}
