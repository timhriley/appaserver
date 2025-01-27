/* -----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/post_change_bank_upload.c		*/
/* -----------------------------------------------------------------	*/
/* 									*/
/* Freely available software: see Appaserver.org			*/
/* -----------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "environ.h"
#include "piece.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "dictionary.h"
#include "bank_upload.h"

/* Constants */
/* --------- */
#define SELECT	"bank_date,bank_description,bank_amount,bank_running_balance";

/* Prototypes */
/* ---------- */
void post_change_bank_upload_update_execute(
				char *application_name,
				int first_posted_sequence_number,
				int prior_sequence_number,
				char *fund_name );

int bank_upload_get_prior_sequence_number(
				char *application_name,
				char *fund_name,
				int sequence_number );

BANK_UPLOAD *bank_upload_get_starting(
				char *application_name,
				DICTIONARY *dictionary );

boolean post_change_bank_upload_update(
				char *application_name,
				DICTIONARY *dictionary );

boolean post_change_bank_upload_delete(
				char *application_name,
				DICTIONARY *dictionary );

char *bank_upload_get_fund_name(
				char *application_name,
				char *bank_date,
				char *bank_description );

int get_bank_balance_sequence_number(
				char *application_name,
				char *bank_date,
				char *fund_name );

FILE *bank_upload_open_input_pipe(
				char *application_name,
				int bank_balance_sequence_number,
				char *fund_name );

FILE *bank_upload_open_output_pipe(
				void );

/* Global variables */
/* ---------------- */
enum bank_upload_exception bank_upload_exception = {0};

int main( int argc, char **argv )
{
	char *application_name;
	char *state;
	DICTIONARY *dictionary;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 4 )
	{
		fprintf( stderr,
			 "Usage: %s ignored state dictionary\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	state = argv[ 2 ];

	dictionary =
		dictionary_string2dictionary(
			argv[ 3 ] );

	if ( strcmp( state, "predelete" ) == 0 )
	{
		if ( !post_change_bank_upload_delete(
				application_name,
				dictionary ) )
		{
			fprintf( stderr, "Error occurred.\n" );
		}
	}
	else
	if ( strcmp( state, "update" ) == 0
	||   strcmp( state, "sort" ) == 0 )
	{
		if ( !post_change_bank_upload_update(
				application_name,
				dictionary ) )
		{
			fprintf( stderr, "Error occurred.\n" );
		}
	}

	return 0;
}

boolean post_change_bank_upload_update(
			char *application_name,
			DICTIONARY *dictionary )
{
	BANK_UPLOAD *posted_first_bank_upload;
	int prior_sequence_number;

	if ( ! ( posted_first_bank_upload =
			bank_upload_get_starting(
				application_name,
				dictionary ) ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot get posted_first_bank_upload.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return 0;
	}

	prior_sequence_number =
		bank_upload_get_prior_sequence_number(
			application_name,
			posted_first_bank_upload->fund_name,
			posted_first_bank_upload->sequence_number );

	post_change_bank_upload_update_execute(
		application_name,
		posted_first_bank_upload->sequence_number,
		prior_sequence_number,
		posted_first_bank_upload->fund_name );

	return 1;
}

void post_change_bank_upload_update_execute(
		char *application_name,
		int first_posted_sequence_number,
		int prior_sequence_number,
		char *fund_name )
{
	FILE *input_pipe;
	FILE *output_pipe;
	char input_bank_date[ 32 ];
	char input_bank_description[ 512 ];
	char input_bank_amount[ 32 ];
	char input_bank_running_balance[ 32 ];
	int sequence_number;
	char input_buffer[ 1024 ];
	boolean first_time = 1;
	double bank_running_balance;
	double bank_amount;

	if ( prior_sequence_number )
		sequence_number = prior_sequence_number;
	else
		sequence_number = first_posted_sequence_number;

	input_pipe =
		bank_upload_open_input_pipe(
			application_name,
			sequence_number,
			fund_name );

	output_pipe = bank_upload_open_output_pipe();

/*
SELECT="bank_date,bank_description,bank_amount,bank_running_balance"
*/
	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	input_bank_date,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	input_bank_description,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		piece(	input_bank_amount,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );
		bank_amount = atof( input_bank_amount );

		if ( first_time )
		{
			piece(	input_bank_running_balance,
				FOLDER_DATA_DELIMITER,
				input_buffer,
				3 );

			bank_running_balance =
				atof( input_bank_running_balance );

			first_time = 0;

			continue;
		}

		if ( bank_amount )
		{
			bank_running_balance += bank_amount;

			fprintf(output_pipe,
			 	"%s^%s^bank_running_balance^%.2lf\n",
			 	input_bank_date,
			 	input_bank_description,
			 	bank_running_balance );
		}
	}

	pclose( input_pipe );
	pclose( output_pipe );
}

int bank_upload_get_prior_sequence_number(
			char *application_name,
			char *fund_name,
			int sequence_number )
{
	char where[ 512 ];
	char *folder_name;
	char *select;
	char sys_string[ 1024 ];
	char *results;

	select = "max(sequence_number)";
	folder_name = "bank_upload";

	if ( fund_name && *fund_name && strcmp( fund_name, "fund" ) != 0 )
	{
		sprintf(	where,
				"fund = '%s' and		"
				"sequence_number < %d		",
				fund_name,
				sequence_number );
	}
	else
	{
		sprintf(	where,
				"sequence_number < %d",
				sequence_number );
	}

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 folder_name,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) )
		return 0;
	else
		return atoi( results );
}

BANK_UPLOAD *bank_upload_get_starting(
				char *application_name,
				DICTIONARY *dictionary )
{
	char key[ 128 ];
	BANK_UPLOAD *bank_upload;
	char *sequence_number_string;
	char *bank_date;
	char *bank_description;

	sprintf( key, "%s_1", "sequence_number" );

	if ( ! ( sequence_number_string =
			dictionary_fetch( key, dictionary ) ) )
	{
		return (BANK_UPLOAD *)0;
	}

	sprintf( key, "%s_1", "bank_date" );

	if ( ! ( bank_date = dictionary_fetch( key, dictionary ) ) )
	{
		return (BANK_UPLOAD *)0;
	}

	sprintf( key, "%s_1", "bank_description" );

	if ( ! ( bank_description =
			dictionary_fetch( key, dictionary ) ) )
	{
		return (BANK_UPLOAD *)0;
	}

	bank_upload =
		bank_upload_new(
			bank_date,
			bank_description );

	bank_upload->sequence_number = timlib_atoi( sequence_number_string );

	bank_upload->fund_name =
		bank_upload_get_fund_name(
			application_name,
			bank_upload->bank_date,
			bank_upload->bank_description );

	return bank_upload;
}

char *bank_upload_get_fund_name(
			char *application_name,
			char *bank_date,
			char *bank_description )
{
	char where[ 512 ];
	char buffer[ 512 ];
	char *folder_name;
	char *select;
	char sys_string[ 1024 ];

	select = "fund";
	folder_name = "bank_upload";

	if ( !attribute_exists(		application_name,
					folder_name,
					select /* attribute_name */ ) )
	{
		return (char *)0;
	}

	timlib_strcpy( buffer, bank_description, 512 );

	sprintf(	where,
			"bank_date = '%s' and bank_description = '%s'",
			bank_date,
			timlib_escape_field( buffer ) );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 folder_name,
		 where );

	return pipe2string( sys_string );
}

FILE *bank_upload_open_input_pipe(
			char *application_name,
			int bank_balance_sequence_number,
			char *fund_name )
{
	char where[ 512 ];
	char *folder_name;
	char *select;
	char *order;
	char sys_string[ 1024 ];

	select = SELECT;

	folder_name = "bank_upload";

	if ( fund_name && *fund_name && strcmp( fund_name, "fund" ) != 0 )
	{
		sprintf(	where,
				"fund = '%s' and sequence_number >= %d",
				fund_name,
				bank_balance_sequence_number );
	}
	else
	{
		sprintf(	where,
				"sequence_number >= %d",
				bank_balance_sequence_number );
	}

	order = "sequence_number";

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=%s	"
		 "			folder=%s	"
		 "			where=\"%s\"	"
		 "			order=%s	",
		 application_name,
		 select,
		 folder_name,
		 where,
		 order );

	return popen( sys_string, "r" );
}

FILE *bank_upload_open_output_pipe( void )
{
	char sys_string[ 1024 ];
	char *key = "bank_date,bank_description";

	sprintf( sys_string,
		 "update_statement.e	table=bank_upload	 "
		 "			key=%s			 "
		 "			carrot=y		|"
		 "sql.e						 ",
		 key );

	return popen( sys_string, "w" );
}

boolean post_change_bank_upload_delete(
			char *application_name,
			DICTIONARY *dictionary )
{
if ( application_name ){}
if ( dictionary ){}

#ifdef NOT_DEFINED
	BANK_UPLOAD *bank_upload;

	bank_upload =
		bank_upload_dictionary_extract(
			dictionary );

	/* --------------------- */
	/* Needs to be finished. */
	/* --------------------- */
#endif
	return 1;
}

