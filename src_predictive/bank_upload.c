/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/bank_upload.c			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <unistd.h>
#include "timlib.h"
#include "date.h"
#include "date_convert.h"
#include "piece.h"
#include "column.h"
#include "session.h"
#include "appaserver_library.h"
#include "html_table.h"
#include "basename.h"
#include "sed.h"
#include "feeder_upload.h"
#include "bank_upload.h"

BANK_UPLOAD *bank_upload_calloc( void )
{
	BANK_UPLOAD *p =
		(BANK_UPLOAD *)
			calloc( 1, sizeof( BANK_UPLOAD ) );

	if ( !p )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}

	return p;

} /* bank_upload_calloc() */

void bank_upload_free( BANK_UPLOAD *b )
{
	if ( b->bank_date ) free( b->bank_date );

	if ( b->bank_description ) free( b->bank_description );

	if ( b->bank_description_embedded &&
	     b->bank_description_embedded != b->bank_description )
	{
		free( b->bank_description_embedded );
	}

	free( b );

} /* bank_upload_free() */

BANK_UPLOAD_STRUCTURE *bank_upload_structure_calloc( void )
{
	BANK_UPLOAD_STRUCTURE *p =
		(BANK_UPLOAD_STRUCTURE *)
			calloc( 1, sizeof( BANK_UPLOAD_STRUCTURE ) );

	if ( !p )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}

	return p;

} /* bank_upload_structure_calloc() */

BANK_UPLOAD_STRUCTURE *bank_upload_structure_new(
				char *application_name,
				char *fund_name,
				char *feeder_account,
				char *input_filename,
				boolean reverse_order,
				int date_piece_offset,
				int description_piece_offset,
				int debit_piece_offset,
				int credit_piece_offset,
				int balance_piece_offset )
{
	extern enum bank_upload_exception bank_upload_exception;
	BANK_UPLOAD_STRUCTURE *p;
	DATE *uncleared_checks_back_date;

	p = bank_upload_structure_calloc();

	p->fund_name = fund_name;
	p->feeder_account = feeder_account;

	p->file.input_filename = input_filename;
	p->file.date_piece_offset = date_piece_offset;
	p->file.description_piece_offset = description_piece_offset;
	p->file.debit_piece_offset = debit_piece_offset;
	p->file.credit_piece_offset = credit_piece_offset;
	p->file.balance_piece_offset = balance_piece_offset;

	if ( ! ( p->starting_sequence_number =
			bank_upload_get_starting_sequence_number(
				application_name,
				p->file.input_filename,
				p->file.date_piece_offset ) ) )
	{
		char *msg;

		msg = "<h2>ERROR: cannot get sequence number</h2>";

		fprintf( stderr,
			 "%s/%s()/%d: %s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 msg );

		printf( "%s\n", msg );

		bank_upload_exception = sequence_number_not_generated;

		return (BANK_UPLOAD_STRUCTURE *)0;
	}

	p->file.error_line_list = list_new();

/* Sets:
		bank_upload->bank_date
		bank_upload->bank_description
		bank_upload->bank_description_embedded
		bank_upload->sequence_number
		bank_upload->bank_amount
		bank_upload->bank_running_balance
*/
	p->file.bank_upload_list =
		bank_upload_fetch_file_list(
			p->file.error_line_list,
			&p->file.file_sha256sum,
			&p->file.minimum_bank_date,
			application_name,
			p->file.input_filename,
			reverse_order,
			p->file.date_piece_offset,
			p->file.description_piece_offset,
			p->file.debit_piece_offset,
			p->file.credit_piece_offset,
			p->file.balance_piece_offset,
			p->starting_sequence_number,
			p->fund_name );

	if ( !p->file.file_sha256sum )
	{
		char *msg;

		msg = "<h2>ERROR: cannot read file.</h2>";

		fprintf( stderr,
			 "%s/%s()/%d: %s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 msg );

		printf( "%s\n", msg );

		bank_upload_exception = internal_read_permission;

		return (BANK_UPLOAD_STRUCTURE *)0;
	}

	p->file.file_row_count =
		bank_upload_get_file_row_count(
			p->file.bank_upload_list );

	p->existing_cash_journal_ledger_list =
		bank_upload_existing_cash_journal_ledger_list(
			application_name,
			p->fund_name );

	uncleared_checks_back_date =
		date_yyyy_mm_dd_new(
			p->file.minimum_bank_date );

	date_increment_days(
		uncleared_checks_back_date,
		-15.0,
		0 /* utc_offset */ );

	p->reoccurring_structure = reoccurring_structure_new();

	p->reoccurring_structure->
		reoccurring_transaction_list =
			reoccurring_fetch_reoccurring_transaction_list(
				application_name );

	p->bank_upload_date_time =
		date_get_now19(
			date_get_utc_offset() );

	return p;

} /* bank_upload_structure_new() */

BANK_UPLOAD *bank_upload_new(	char *bank_date,
				char *bank_description )
{
	BANK_UPLOAD *p = bank_upload_calloc();

	p->bank_date = bank_date;
	p->bank_description = bank_description;

	return p;

} /* bank_upload_new() */

/* Sets:
		bank_upload->bank_date
		bank_upload->bank_description
		bank_upload->bank_description_embedded
		bank_upload->sequence_number
		bank_upload->bank_amount
		bank_upload->bank_running_balance
*/
LIST *bank_upload_fetch_file_list(
				LIST *error_line_list,
				char **file_sha256sum,
				char **minimum_bank_date,
				char *application_name,
				char *input_filename,
				boolean reverse_order,
				int date_piece_offset,
				int description_piece_offset,
				int debit_piece_offset,
				int credit_piece_offset,
				int balance_piece_offset,
				int starting_sequence_number,
				char *fund_name )
{
	char sys_string[ 1024 ];
	char input_string[ 4096 ];
	char bank_date[ 128 ];
	char bank_date_international[ 128 ];
	char bank_description[ 1024 ];
	char bank_amount[ 128 ];
	char bank_balance[ 128 ];
	static char local_minimum_bank_date[ 16 ] = {0};
	FILE *input_pipe;
	BANK_UPLOAD *bank_upload;
	LIST *bank_upload_list;
	int line_number = 0;

	if ( file_sha256sum )
	{
		*file_sha256sum = timlib_get_sha256sum( input_filename );
	}

	if ( reverse_order )
	{
		sprintf( sys_string,
			 "cat \"%s\" | reverse_lines.e",
			 input_filename );
	}
	else
	{
		sprintf( sys_string,
			 "cat \"%s\"",
			 input_filename );
	}

	input_pipe = popen( sys_string, "r" );

	if ( minimum_bank_date )
	{
		*minimum_bank_date = local_minimum_bank_date;
	}

	bank_upload_list = list_new();
	*bank_balance = '\0';

	while( timlib_get_line( input_string, input_pipe, 4096 ) )
	{
		line_number++;

		trim( input_string );
		if ( !*input_string ) continue;

		/* Why is BofA escaping the closing double-quote? */
		/* ---------------------------------------------- */
		timlib_remove_character( input_string, '\\' );

		/* Get bank_date */
		/* ------------- */
		if ( !piece_quote_comma(
				bank_date,
				input_string,
				date_piece_offset ) )
		{
			continue;
		}

		if ( timlib_exists_string( bank_date, "date" )
		||   timlib_exists_string( bank_date, "description" )
		||   timlib_exists_string( bank_date, "balance" )
		||   timlib_exists_string( bank_date, "total" ) )
		{
			continue;
		}

		if ( !bank_upload_get_bank_date_international(
				bank_date_international,
				bank_date ) )
		{
			char msg[ 128 ];

			sprintf( msg,
				 "<p>Error in line %d: Bad date = (%s)",
				 line_number,
				 bank_date );

			list_append_pointer( error_line_list, strdup( msg ) );

			continue;
		}

		if ( !*local_minimum_bank_date )
		{
			strcpy(	local_minimum_bank_date,
				bank_date_international );
		}

		/* =============== */
		/* Get bank_amount */
		/* =============== */

		/* The bank_amount is either a single column or two columns. */
		/* --------------------------------------------------------- */
		if ( !piece_quote_comma(
				bank_amount,
				input_string,
				debit_piece_offset ) )
		{
			char msg[ 128 ];

			sprintf( msg,
"<p>Error in line %d: Cannot parse bank amount using piece=%d in input = (%s)",
				 line_number,
				 debit_piece_offset,
				 input_string );

			list_append_pointer( error_line_list, strdup( msg ) );

			continue;
		}

		if ( !atof( bank_amount ) )
		{
			/* See if there's a second column. */
			/* ------------------------------- */
			if ( credit_piece_offset < 0 ) continue;

			if ( !piece_quote_comma(
					bank_amount,
					input_string,
					credit_piece_offset ) )
			{
				char msg[ 128 ];

				sprintf( msg,
		"<p>Error in line %d: Cannot parse bank amount using piece=%d",
				 	line_number,
				 	credit_piece_offset );

				list_append_pointer(
					error_line_list,
					strdup( msg ) );

				continue;
			}
		}

		if ( !atof( bank_amount ) )
		{
			char msg[ 128 ];

			sprintf( msg,
		"<p>Error in line %d: Cannot parse bank amount using piece=%d",
			 	line_number,
			 	credit_piece_offset );

			list_append_pointer( error_line_list, strdup( msg ) );

			continue;
		}

		/* Get bank_description */
		/* -------------------- */
		if ( !piece_quote_comma(
				bank_description,
				input_string,
				description_piece_offset ) )
		{
			char msg[ 128 ];

			sprintf( msg,
		"<p>Error in line %d: Cannot parse description using piece=%d",
				 line_number,
				 description_piece_offset );

			list_append_pointer( error_line_list, strdup( msg ) );

			continue;
		}

		/* Trim off trailing date and crop to database size. */
		/* ------------------------------------------------- */
		strcpy( bank_description,
			/* ------------ */
			/* Returns self */
			/* ------------ */
			bank_upload_description_crop(
				/* ------------------------- */
				/* Both Return static memory */
				/* ------------------------- */
				sed_trim_double_spaces(
				  feeder_upload_trim_bank_date_from_description(
					bank_description ) ) ) );

		/* Get bank_balance */
		/* ---------------- */
		if ( balance_piece_offset >= 0 )
		{
			piece_quote_comma(
				bank_balance,
				input_string,
				balance_piece_offset );
		}

		bank_upload = bank_upload_calloc();

		bank_upload->bank_date = strdup( bank_date_international );

		bank_upload->bank_description =
		bank_upload->bank_description_embedded =
			strdup( bank_description );

		bank_upload->check_number =
			bank_upload_parse_check_number(
				bank_upload->bank_description );

		bank_upload->sequence_number = starting_sequence_number;
		bank_upload->bank_amount = atof( bank_amount );;
		bank_upload->bank_running_balance = atof( bank_balance );;

		/* ------------------------------- */
		/* Build bank_description_embedded */
		/* ------------------------------- */
		bank_upload->bank_description_embedded =
			/* ----------------------- */
			/* Returns strdup() memory */
			/* ----------------------- */
			feeder_upload_get_description_embedded(
					bank_upload->bank_description
						/* bank_description_file */,
					fund_name,
					bank_upload->bank_amount,
					bank_upload->bank_running_balance );

		if ( bank_upload_exists(
			application_name,
			bank_upload->bank_date,
			bank_upload->bank_description_embedded,
			local_minimum_bank_date ) )
		{
			bank_upload->existing_bank_upload = 1;
		}

		list_append_pointer( bank_upload_list, bank_upload );
		starting_sequence_number++;
	}

	pclose( input_pipe );

	return bank_upload_list;

} /* bank_upload_fetch_file_list() */

void bank_upload_event_insert(		char *application_name,
					char *bank_upload_date_time,
					char *login_name,
					char *bank_upload_filename,
					char *file_sha256sum,
					char *fund_name,
					char *feeder_account )
{
	char sys_string[ 1024 ];
	FILE *insert_pipe;
	char *insert_upload_event;
	boolean exists_fund;
	char *table_name;
	char *insert_bank_upload_filename;

	insert_bank_upload_filename =
		bank_upload_get_insert_bank_upload_filename(
			bank_upload_filename );

	exists_fund = ledger_fund_attribute_exists( application_name );

/*
#define INSERT_BANK_UPLOAD_EVENT		\
	"bank_upload_date_time,login_name,bank_upload_filename,file_sha256sum,feeder_account"

#define INSERT_BANK_UPLOAD_EVENT_FUND		\
	"bank_upload_date_time,login_name,bank_upload_filename,file_sha256sum,feeder_account,fund"
*/

	if ( exists_fund )
	{
		insert_upload_event = INSERT_BANK_UPLOAD_EVENT_FUND;
	}
	else
	{
		insert_upload_event = INSERT_BANK_UPLOAD_EVENT;
	}

	table_name =
		get_table_name(	application_name,
				"bank_upload_event" );

	sprintf( sys_string,
	 	 "insert_statement table=%s field=%s del='%c' 		  |"
	 	 "sql.e 2>&1						  |"
	 	 "html_paragraph_wrapper.e				   ",
	 	 table_name,
	 	 insert_upload_event,
	 	 FOLDER_DATA_DELIMITER );

	insert_pipe = popen( sys_string, "w" );

	if ( exists_fund )
	{
		fprintf(insert_pipe,
	 		"%s^%s^%s^%s^%s^%s\n",
	 		bank_upload_date_time,
	 		(login_name) ? login_name : "",
			(insert_bank_upload_filename)
				? insert_bank_upload_filename
				: "",
			(file_sha256sum) ? file_sha256sum : "",
			(feeder_account) ? feeder_account : "",
			(fund_name) ? fund_name : "" );
	}
	else
	{
		fprintf(insert_pipe,
	 		"%s^%s^%s^%s^%s\n",
	 		bank_upload_date_time,
	 		(login_name) ? login_name : "",
			(insert_bank_upload_filename)
				? insert_bank_upload_filename
				: "",
			(file_sha256sum) ? file_sha256sum : "",
			(feeder_account) ? feeder_account : "" );
	}

	pclose( insert_pipe );

} /* bank_upload_event_insert() */

void bank_upload_archive_insert(	char *application_name,
					char *fund_name,
					LIST *bank_upload_list,
					char *bank_upload_date_time )
{
	char sys_string[ 1024 ];
	FILE *bank_upload_archive_insert_pipe = {0};
	BANK_UPLOAD *bank_upload;
	char *table_name;

	if ( !list_rewind( bank_upload_list ) );

/*
#define INSERT_BANK_UPLOAD_ARCHIVE	\
	"bank_date,bank_description,sequence_number,bank_amount,bank_running_balance,bank_upload_date_time"
*/

	table_name =
		get_table_name(	application_name,
				"bank_upload_archive" );

	sprintf( sys_string,
		 "insert_statement table=%s field=%s del='%c' 		  |"
		 "sql.e 2>&1						  |"
		 "grep -vi duplicate					  |"
		 "html_paragraph_wrapper.e				  |"
		 "cat							   ",
		 	table_name,
		 	INSERT_BANK_UPLOAD_ARCHIVE,
		 	FOLDER_DATA_DELIMITER );

	bank_upload_archive_insert_pipe = popen( sys_string, "w" );

	do {
		bank_upload = list_get_pointer( bank_upload_list );

		if ( bank_upload->existing_bank_upload ) continue;

		if ( !bank_upload->bank_description_embedded )
		{
			bank_upload->bank_description_embedded =
				/* ----------------------- */
				/* Returns strdup() memory */
				/* ----------------------- */
				feeder_upload_get_description_embedded(
					bank_upload->bank_description,
					fund_name,
					bank_upload->bank_amount,
					bank_upload->bank_running_balance );
		}

		fprintf(bank_upload_archive_insert_pipe,
			"%s^%s^%d^%.2lf^%.2lf^%s\n",
		 	bank_upload->bank_date,
			bank_upload_description_crop(
				bank_upload->bank_description_embedded ),
			bank_upload->sequence_number,
		 	bank_upload->bank_amount,
			bank_upload->bank_running_balance,
			bank_upload_date_time );

	} while( list_next( bank_upload_list ) );

	pclose( bank_upload_archive_insert_pipe );

} /* bank_upload_archive_insert() */

void bank_upload_transaction_direct_insert(
					char *application_name,
					char *bank_date,
					char *bank_description_embedded,
					char *full_name,
					char *street_address,
					char *transaction_date_time )
{
	char sys_string[ 1024 ];
	char *table_name;
	FILE *insert_pipe;
	char *field;

	field =
"bank_date,bank_description,full_name,street_address,transaction_date_time";

	table_name =
		get_table_name(	application_name,
				"bank_upload_transaction" );

	sprintf( sys_string,
	 	 "insert_statement table=%s field=%s del='%c' 		  |"
	 	 "sql.e 2>&1						  |"
	 	 "html_paragraph_wrapper.e				   ",
	 	 table_name,
	 	 field,
	 	 FOLDER_DATA_DELIMITER );

	insert_pipe = popen( sys_string, "w" );

	fprintf(insert_pipe,
		"%s^%s^%s^%s^%s\n",
		bank_date,
		bank_upload_description_crop(
			bank_description_embedded ),
		full_name,
		street_address,
		transaction_date_time );

	pclose( insert_pipe );

} /* bank_upload_transaction_direct_insert() */

/* Returns table_insert_count */
/* -------------------------- */
int bank_upload_insert(			char *fund_name,
					LIST *bank_upload_list,
					char *bank_upload_date_time )
{
	char sys_string[ 1024 ];
	FILE *bank_upload_insert_pipe = {0};
	int table_insert_count = 0;
	char error_filename[ 128 ] = {0};
	BANK_UPLOAD *bank_upload;
	int error_file_lines;
	char *table_name = BANK_UPLOAD_TABLE_NAME;

	if ( !list_rewind( bank_upload_list ) ) return 0;

/*
#define INSERT_BANK_UPLOAD		\
	"bank_date,bank_description,sequence_number,bank_amount,bank_upload_date_time"
*/

	sprintf(	error_filename,
			"/tmp/%s_%d.err",
			table_name,
			getpid() );

	sprintf( sys_string,
	 	 "insert_statement.e table=%s field=%s del='%c'		  |"
	 	 "sql.e 2>&1						  |"
		 "grep -vi duplicate					  |"
	 	 "cat > %s						   ",
	 	table_name,
	 	INSERT_BANK_UPLOAD,
	 	FOLDER_DATA_DELIMITER,
		error_filename );

	bank_upload_insert_pipe = popen( sys_string, "w" );

	do {
		bank_upload = list_get_pointer( bank_upload_list );

		if ( bank_upload->existing_bank_upload ) continue;

		if ( !bank_upload->bank_description_embedded )
		{
			bank_upload->bank_description_embedded =
				/* ----------------------- */
				/* Returns strdup() memory */
				/* ----------------------- */
				feeder_upload_get_description_embedded(
					bank_upload->bank_description,
					fund_name,
					bank_upload->bank_amount,
					bank_upload->bank_running_balance );
		}

		/* Output insert into BANK_UPLOAD */
		/* ------------------------------ */
		fprintf(bank_upload_insert_pipe,
			"%s^%s^%d^%.2lf^%s\n",
		 	bank_upload->bank_date,
			bank_upload_description_crop(
				bank_upload->bank_description_embedded ),
			bank_upload->sequence_number,
		 	bank_upload->bank_amount,
			bank_upload_date_time );

		table_insert_count++;

	} while( list_next( bank_upload_list ) );

	pclose( bank_upload_insert_pipe );

	sprintf( sys_string,
		 "wc -l %s",
		 error_filename );

	error_file_lines = atoi( pipe2string( sys_string ) );
	table_insert_count -= error_file_lines;

	if ( error_file_lines )
	{
		printf( "<h3>Error records.</h3>\n" );
		fflush( stdout );

		sprintf( sys_string,
			 "cat %s | html_paragraph_wrapper.e",
			 error_filename );

		if ( system( sys_string ) ) {};
	}

	sprintf( sys_string,
		 "rm -f %s",
		 error_filename );

	if ( system( sys_string ) ) {};

	return table_insert_count;

} /* bank_upload_insert() */

int bank_upload_get_starting_sequence_number(
			char *application_name,
			char *input_filename,
			int date_piece_offset )
{
	int line_count;
	char sys_string[ 1024 ];

	line_count =
		bank_upload_get_line_count(
			input_filename,
			date_piece_offset );

	sprintf( sys_string,
		 "reference_number.sh %s %d",
		 application_name,
		 line_count );

	return atoi( pipe2string( sys_string ) );

} /* bank_upload_get_starting_sequence_number() */

int bank_upload_get_line_count(	char *input_filename,
				int date_piece_offset )
{
	char input_string[ 4096 ];
	char bank_date[ 128 ];
	FILE *input_file;
	int line_count = 0;

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 input_filename );
		exit( 1 );
	}

	while( timlib_get_line( input_string, input_file, 4096 ) )
	{
		trim( input_string );
		if ( !*input_string ) continue;

		if ( !piece_quote_comma(
				bank_date,
				input_string,
				date_piece_offset ) )
		{
			continue;
		}

		if ( timlib_exists_string( bank_date, "date" ) )
		{
			continue;
		}

		line_count++;
	}

	fclose( input_file );
	return line_count;

} /* bank_upload_get_line_count() */

boolean bank_upload_get_bank_date_international(
				char *bank_date_international,
				char *bank_date )
{
	date_convert_source_american(
		bank_date_international,
		international,
		bank_date );

	return date_convert_is_valid_international(
		bank_date_international );

} /* bank_upload_get_bank_date_international() */

char *bank_upload_get_select( void )
{
	char *select;

	select =	"bank_date,		"
			"bank_description,	"
			"sequence_number,	"
			"bank_amount,		"
			"bank_running_balance	";

	return select;
}

char *bank_upload_get_where(	char *where,
				char *bank_date,
				char *bank_description )
{
	char description_buffer[ 512 ];

	sprintf( where,
		 "bank_date = '%s' and			"
		 "bank_description = '%s'		",
		 bank_date,
		 escape_character(	description_buffer,
					bank_description,
					'\'' ) );

	return where;

} /* bank_upload_get_where() */

BANK_UPLOAD *bank_upload_fetch(		char *application_name,
					char *bank_date,
					char *bank_description_embedded )
{
	BANK_UPLOAD *bank_upload;
	char *select;
	char where[ 512 ];
	char sys_string[ 1024 ];
	char *results;

	select = bank_upload_get_select();

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=bank_upload	"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 bank_upload_get_where(
			where,
			bank_date,
			bank_description_embedded ) );

	if ( ! ( results = pipe2string( sys_string ) ) )
		return (BANK_UPLOAD *)0;

	bank_upload = bank_upload_calloc();

	bank_upload_fetch_parse(
			&bank_upload->bank_date,
			&bank_upload->bank_description,
			&bank_upload->sequence_number,
			&bank_upload->bank_amount,
			&bank_upload->bank_running_balance,
			&bank_upload->check_number,
			results );

	bank_upload->bank_description_embedded =
		bank_upload->bank_description;

	return bank_upload;

} /* bank_upload_fetch() */

LIST *bank_upload_fetch_bank_upload_table_list(
					char *application_name,
					int starting_sequence_number,
					char *begin_date )
{
	LIST *bank_upload_list;
	BANK_UPLOAD *bank_upload;
	char *select;
	char where[ 128 ];
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	FILE *input_pipe;

	bank_upload_list = list_new();
	select = bank_upload_get_select();

	if ( starting_sequence_number )
	{
		sprintf(	where,
				"sequence_number >= %d",
				starting_sequence_number );
	}
	else
	if ( begin_date )
	{
		sprintf(	where,
				"bank_date >= '%s'",
				begin_date );
	}
	else
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot generate where.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=bank_upload	"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		bank_upload = bank_upload_calloc();

		bank_upload_fetch_parse(
				&bank_upload->bank_date,
				&bank_upload->bank_description,
				&bank_upload->sequence_number,
				&bank_upload->bank_amount,
				&bank_upload->bank_running_balance,
				&bank_upload->check_number,
				input_buffer );

		list_append_pointer( bank_upload_list, bank_upload );
	}

	pclose( input_pipe );

	return bank_upload_list;

} /* bank_upload_fetch_bank_upload_table_list() */

void bank_upload_fetch_parse(	char **bank_date,
				char **bank_description,
				int *sequence_number,
				double *bank_amount,
				double *bank_running_balance,
				int *check_number,
				char *input_buffer )
{
	char buffer[ 1024 ];

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
	if ( *buffer )
		*bank_date = strdup( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
	if ( *buffer )
		*bank_description = strdup( buffer );

	*check_number =
		bank_upload_parse_check_number(
			*bank_description );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
	if ( *buffer )
		*sequence_number = atoi( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
	if ( *buffer )
		*bank_amount = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
	if ( *buffer )
		*bank_running_balance = atof( buffer );

} /* bank_upload_fetch_parse() */

LIST *bank_upload_existing_cash_journal_ledger_list(
					char *application_name,
					char *fund_name )
{
	LIST *journal_ledger_list;
	char sys_string[ 2048 ];
	char *cash_account_name;
	char *uncleared_checks_account;
	char where[ 1024 ];
	char *join_where;
	char *subquery_join;
	char *select;
	char check_number_select[ 512 ];
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	char local_full_name[ 128 ];
	char local_street_address[ 128 ];
	char local_transaction_date_time[ 128 ];
	char local_account_name[ 128 ];
	char *folder_list_string;
	JOURNAL_LEDGER *ledger;
	char check_number[ 16 ];
	char *timriley_where;

	cash_account_name =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			LEDGER_CASH_KEY,
			0 /* not warning_only */,
			__FUNCTION__ );

	uncleared_checks_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			LEDGER_UNCLEARED_CHECKS_KEY,
			0 /* not warning_only */,
			__FUNCTION__ );

/*
"full_name,street_address,transaction_date_time,account,transaction_count,previous_balance,debit_amount,credit_amount,balance,check_number";
*/
	select = ledger_journal_ledger_get_select();
	sprintf( check_number_select, "%s,check_number", select );

	folder_list_string = "journal_ledger,transaction";

	join_where = ledger_get_journal_ledger_transaction_join_where();

	subquery_join =
		bank_upload_transaction_journal_ledger_subquery();

	if ( strcmp( application_name, "timriley" ) == 0 )
	{
		timriley_where =
			"transaction.transaction_date_time >= '2019-01-01'";
	}
	else
	{
		timriley_where = "1 = 1";
	}

	sprintf(where,
		"(account = '%s' or account = '%s') and		"
		"%s and %s and %s				",
		cash_account_name,
		uncleared_checks_account,
		join_where,
		subquery_join,
		timriley_where );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=%s		"
		 "			where=\"%s\"		"
		 "			order=\"%s\"		",
		 application_name,
		 check_number_select,
		 folder_list_string,
		 where,
		 "transaction_date_time" );

	input_pipe = popen( sys_string, "r" );

	journal_ledger_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	local_full_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	local_street_address,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		piece(	local_transaction_date_time,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		piece(	local_account_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			3 );

		piece(	check_number,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			9 );

		ledger = journal_ledger_new(
				strdup( local_full_name ),
				strdup( local_street_address ),
				strdup( local_transaction_date_time ),
				strdup( local_account_name ) );

		ledger_journal_ledger_partial_parse(
			&ledger->transaction_count,
			&ledger->database_transaction_count,
			&ledger->previous_balance,
			&ledger->database_previous_balance,
			&ledger->debit_amount,
			&ledger->credit_amount,
			&ledger->balance,
			&ledger->database_balance,
			input_buffer );

		if ( *check_number )
		{
			ledger->check_number = atoi( check_number );
		}

		list_append_pointer( journal_ledger_list, ledger );
	}

	pclose( input_pipe );

	return journal_ledger_list;

} /* bank_upload_existing_cash_journal_ledger_list() */

/* ---------------------------------------------------------------------*/
/* Sets bank_upload->feeder_check_number_existing_journal_ledger	*/
/* or									*/
/* Sets bank_upload->feeder_match_sum_xisting_journal_ledger_list	*/
/* or									*/
/* Sets bank_upload->feeder_phrase_match_build_transaction		*/
/* --------------------------------------------------------------------	*/
void bank_upload_set_transaction(
				LIST *bank_upload_list,
				LIST *reoccurring_transaction_list,
				LIST *existing_cash_journal_ledger_list )
{
	bank_upload_check_number_existing_journal_ledger(
		bank_upload_list,
		existing_cash_journal_ledger_list );

	bank_upload_feeder_phrase_match_build_transaction(
		bank_upload_list,
		reoccurring_transaction_list );

	bank_upload_match_sum_existing_journal_ledger_list(
		bank_upload_list,
		existing_cash_journal_ledger_list );

} /* bank_upload_set_transaction() */

void bank_upload_check_number_existing_journal_ledger(
			LIST *bank_upload_list,
			LIST *existing_cash_journal_ledger_list )
{
	BANK_UPLOAD *bank_upload;
	JOURNAL_LEDGER *journal_ledger;

	if ( !list_rewind( bank_upload_list ) ) return;

	do {
		bank_upload = list_get_pointer( bank_upload_list );

		if ( bank_upload->existing_bank_upload ) continue;

		if ( !bank_upload->check_number ) continue;

		if ( bank_upload->feeder_match_sum_existing_journal_ledger_list
		||   bank_upload->feeder_phrase_match_build_transaction )
		{
			continue;
		}

		if ( ( journal_ledger =
				feeder_check_number_existing_journal_ledger(
					existing_cash_journal_ledger_list,
					bank_upload->check_number ) ) )
		{
			bank_upload->
				feeder_check_number_existing_journal_ledger =
					journal_ledger;

			journal_ledger->match_sum_taken = 1;

			journal_ledger->check_number =
				bank_upload->check_number;
		}

	} while( list_next( bank_upload_list ) );

} /* bank_upload_check_number_existing_journal_ledger() */

void bank_upload_feeder_phrase_match_build_transaction(
				LIST *bank_upload_list,
				LIST *reoccurring_transaction_list )
{
	BANK_UPLOAD *bank_upload;

	if ( !list_rewind( bank_upload_list ) ) return;

	do {
		bank_upload = list_get_pointer( bank_upload_list );

		if ( bank_upload->existing_bank_upload ) continue;

		if ( bank_upload->feeder_check_number_existing_journal_ledger
		||   bank_upload->feeder_match_sum_existing_journal_ledger_list)
		{
			continue;
		}

		bank_upload->
			feeder_phrase_match_build_transaction =
				feeder_phrase_match_build_transaction(
					reoccurring_transaction_list,
				 	bank_upload->bank_date,
					bank_upload->
						bank_description_embedded,
					timlib_abs_double(
						bank_upload->bank_amount )
						/* abs_bank_amount */ );

	} while( list_next( bank_upload_list ) );

} /* bank_upload_feeder_phrase_match_build_transaction() */

/* Insert into TRANSACTION and JOURNAL_LEDGER */
/* ------------------------------------------ */
/* Note: this is the bottleneck.	      */
/* ------------------------------------------ */
void bank_upload_transaction_insert(	char *application_name,
					LIST *bank_upload_list )
{
	BANK_UPLOAD *bank_upload;
	TRANSACTION *transaction;

	if ( !list_rewind( bank_upload_list ) ) return;

	do {
		bank_upload = list_get_pointer( bank_upload_list );

		if ( bank_upload->existing_transaction ) continue;

		if ( !bank_upload->feeder_phrase_match_build_transaction )
			continue;

		transaction =
			bank_upload->
				feeder_phrase_match_build_transaction;

		/* Here is the bottleneck. */
		/* ----------------------- */
		transaction->transaction_date_time =
			ledger_transaction_journal_ledger_insert(
				application_name,
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				transaction->transaction_amount,
				transaction->memo,
				transaction->check_number,
				transaction->lock_transaction,
				transaction->journal_ledger_list );

	} while( list_next( bank_upload_list ) );

} /* bank_upload_transaction_insert() */

void bank_upload_table_display(
				char *application_name,
				LIST *bank_upload_list )
{
	BANK_UPLOAD *bank_upload;
	FILE *output_pipe;
	char sys_string[ 1024 ];
	char *heading;
	LIST *match_sum_journal_ledger_list;

	if ( !list_rewind( bank_upload_list ) ) return;

	heading =
	"Account<br>Entity/Transaction,bank_date,bank_description,amount";

	sprintf( sys_string,
		 "html_table.e '' '%s' '^' left,left,left,right",
		 heading );

	output_pipe = popen( sys_string, "w" );

	do {
		bank_upload = list_get( bank_upload_list );

		match_sum_journal_ledger_list =
			bank_upload->
			feeder_match_sum_existing_journal_ledger_list;

		fprintf( output_pipe,
			 "%s^%s^%s^%.2lf\n",
			 bank_upload_get_account_html(
				application_name,
				bank_upload->existing_bank_upload,
				bank_upload->existing_transaction,
				bank_upload->
					feeder_phrase_match_build_transaction,
				bank_upload->
				    feeder_check_number_existing_journal_ledger,
				match_sum_journal_ledger_list ),
			 bank_upload->bank_date,
			 bank_upload->bank_description_embedded,
			 bank_upload->bank_amount );

	} while( list_next( bank_upload_list ) );

	pclose( output_pipe );

} /* bank_upload_table_display() */

void bank_upload_transaction_text_display( LIST *bank_upload_list )
{
	BANK_UPLOAD *bank_upload;
	char *transaction_memo;
	TRANSACTION *transaction;

	if ( !list_rewind( bank_upload_list ) ) return;

	do {
		bank_upload = list_get_pointer( bank_upload_list );

		if ( bank_upload->existing_transaction ) continue;

		transaction =
			bank_upload->
				feeder_phrase_match_build_transaction;

		if ( transaction )
		{
			transaction_memo =
				bank_upload_get_transaction_memo(
					transaction->
						full_name,
					transaction->
						street_address,
					transaction->
						transaction_date_time );

			ledger_list_text_display(
				transaction_memo,
				transaction->
					journal_ledger_list );
		}
		else
		if ( bank_upload->feeder_check_number_existing_journal_ledger )
		{
			bank_upload_journal_ledger_text_display(
				bank_upload->
				  feeder_check_number_existing_journal_ledger );
		}

	} while( list_next( bank_upload_list ) );

} /* bank_upload_transaction_text_display() */

void bank_upload_transaction_table_display( LIST *bank_upload_list )
{
	BANK_UPLOAD *bank_upload;
	char *transaction_memo;
	TRANSACTION *transaction;

	if ( !list_rewind( bank_upload_list ) ) return;

	do {
		bank_upload = list_get_pointer( bank_upload_list );

		if ( bank_upload->existing_transaction ) continue;

		transaction =
			bank_upload->
				feeder_phrase_match_build_transaction;

		if ( transaction )
		{
			transaction_memo =
				bank_upload_get_transaction_memo(
					transaction->
						full_name,
					transaction->
						street_address,
					transaction->
						transaction_date_time );

			ledger_list_html_display(
				transaction_memo,
				transaction->journal_ledger_list );
		}

	} while( list_next( bank_upload_list ) );

} /* bank_upload_transaction_table_display() */

BANK_UPLOAD *bank_upload_dictionary_extract(
				char *application_name,
				DICTIONARY *dictionary )
{
	char *key;
	BANK_UPLOAD *bank_upload;
	char *bank_date;
	char *bank_description;

	key = "bank_date";

	if ( ! ( bank_date = dictionary_fetch( dictionary, key ) ) )
	{
		return (BANK_UPLOAD *)0;
	}

	key = "bank_description";

	if ( ! ( bank_description =
			dictionary_fetch( dictionary, key ) ) )
	{
		return (BANK_UPLOAD *)0;
	}

	bank_upload =
		bank_upload_fetch(
			application_name,
			bank_date,
			bank_description );

	return bank_upload;

} /* bank_upload_dictionary_extract() */

char *bank_upload_get_transaction_memo(
					char *full_name,
					char *street_address,
					char *transaction_date_time )
{
	static char transaction_memo[ 256 ];
	char street_address_display[ 128 ];

	if ( street_address
	&&   *street_address
	&&   strcmp( street_address, "null" ) != 0 )
	{
		sprintf( street_address_display,
			 "/%s",
			 street_address );
	}
	else
	{
		*street_address_display = '\0';
	}

	sprintf(transaction_memo,
		"%s%s/%s",
		full_name,
		street_address_display,
		transaction_date_time );

	format_initial_capital(
		transaction_memo,
		transaction_memo );

	return transaction_memo;

} /* bank_upload_get_transaction_memo() */

boolean bank_upload_sha256sum_exists(
			char *application_name,
			char *file_sha256sum )
{
	char where[ 512 ];
	char sys_string[ 1024 ];

	sprintf( where,
		 "file_sha256sum = '%s'",
		 file_sha256sum );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=count			"
		 "			folder=bank_upload_event	"
		 "			where=\"%s\"			",
		 application_name,
		 where );

	return (boolean)atoi( pipe2string( sys_string ) );

} /* bank_upload_sha256sum_exists() */

int bank_upload_feeder_phrase_match_transaction_count(
			LIST *bank_upload_list )
{
	BANK_UPLOAD *b;
	int count = 0;

	if ( list_rewind( bank_upload_list ) )
	{
		do {
			b = list_get_pointer( bank_upload_list );

			if ( b->existing_transaction ) continue;

			if ( b->feeder_phrase_match_build_transaction )
				count++;

		} while( list_next( bank_upload_list ) );
	}

	return count;

} /* bank_upload_feeder_phrase_match_transaction_count() */

int bank_upload_get_file_row_count(
			LIST *bank_upload_list )
{
	BANK_UPLOAD *b;
	int count;

	if ( !list_rewind( bank_upload_list ) ) return 0;

	count = 0;

	do {
		b = list_get_pointer( bank_upload_list );

		if ( b->existing_transaction ) continue;

		count++;

	} while ( list_next( bank_upload_list ) );

	return count;

} /* bank_upload_get_file_row_count() */

boolean bank_upload_transaction_exists(
				char *application_name,
				char *bank_date,
				char *bank_description )
{
	char where[ 512 ];
	char sys_string[ 1024 ];
	char *results;

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"count(1)\"		"
		 "			folder=bank_upload_transaction	"
		 "			where=\"%s\"			",
		 application_name,
		 bank_upload_get_where(
			where,
			bank_date,
			bank_description ) );

	results = pipe2string( sys_string );

	if ( !results || *results == '0' )
		return 0;
	else
		return 1;

} /* bank_upload_transaction_exists() */

double bank_upload_fetch_bank_amount(
				char *application_name,
				char *bank_date,
				char *bank_description )
{
	BANK_UPLOAD *b;

	if ( ! ( b = bank_upload_fetch(
			application_name,
			bank_date,
			bank_description
				/* bank_description_embedded */ ) ) )
	{
		return 0.0;
	}
	else
	{
		return b->bank_amount;
	}

} /* bank_upload_fetch_bank_amount() */

/* -------------------------------------------------------- */
/* If found, returns transaction_date_time.		    */
/* If not found, returns null and message may be populated. */
/* -------------------------------------------------------- */
char *bank_upload_pending_amount_deposit(
				char *message,
				char **full_name,
				double amount )
{
	char main_expression[ 128 ];
	char sys_string[ 128 ];
	char *results;
	int transaction_count;
	char full_name_buffer[ 128 ];
	char transaction_date_time[ 32 ];

	*message = '\0';
	*full_name = "";
/*
select="transaction_date_time,full_name,street_address,debit_amount,credit_amount"
*/

	/* Check if more than 1 transaction for this amount. */
	/* ------------------------------------------------- */
	sprintf( main_expression,
		 "bank_upload_ledger_pending.sh | grep '\\^%.2lf\\^$'",
		 amount );

	sprintf( sys_string, "%s | wc -l", main_expression );

	results = pipe2string( sys_string );

	transaction_count = atoi( results );

	if ( transaction_count == 0 ) return (char *)0;

	if ( transaction_count > 1 )
	{
		sprintf( message,
"Warning: %d pending deposits for amount = %.2lf. Is this amount duplicated?",
			 transaction_count,
			 amount );

		return (char *)0;
	}

	results = pipe2string( main_expression );

	piece( transaction_date_time, '^', results, 0 );
	piece( full_name_buffer, '^', results, 1 );

	*full_name = strdup( full_name_buffer );

	return strdup( transaction_date_time );

} /* bank_upload_pending_amount_deposit() */

/* --------------------------------------------------- */
/* Returns transaction_date_time or null if not found. */
/* Message will help to explain not found.	       */
/* --------------------------------------------------- */
char *bank_upload_pending_amount_withdrawal(
				char *message,
				char **full_name,
				double amount )
{
	char main_expression[ 128 ];
	char sys_string[ 128 ];
	char *results;
	int transaction_count;
	char full_name_buffer[ 128 ];
	char transaction_date_time[ 32 ];

	*message = '\0';
	*full_name = "";
/*
select="transaction_date_time,full_name,street_address,debit_amount,credit_amount"
*/

	/* Check if more than 1 transaction for this amount. */
	/* ------------------------------------------------- */
	sprintf( main_expression,
		 "bank_upload_ledger_pending.sh | grep '\\^%.2lf$'",
		 amount );

	sprintf( sys_string, "%s | wc -l", main_expression );

	results = pipe2string( sys_string );

	transaction_count = atoi( results );

	if ( transaction_count == 0 ) return (char *)0;

	if ( transaction_count > 1 )
	{
		sprintf( message,
"Warning: %d pending withdrawals for amount = %.2lf. Is this amount duplicated?",
			 transaction_count,
			 amount );

		return (char *)0;
	}

	results = pipe2string( main_expression );

	piece( transaction_date_time, '^', results, 0 );
	piece( full_name_buffer, '^', results, 1 );

	*full_name = strdup( full_name_buffer );

	return strdup( transaction_date_time );

} /* bank_upload_pending_amount_withdrawal() */

BANK_UPLOAD *bank_upload_prior_fetch(
					char *application_name,
					int sequence_number )
{
	char *select;
	char *folder;
	char where[ 512 ];
	char sys_string[ 1024 ];
	char *results;

	select = "max( sequence_number )";
	folder = BANK_UPLOAD_TABLE_NAME;

	sprintf( where,
		 "sequence_number < %d",
		 sequence_number );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 folder,
		 where );

	results = pipe2string( sys_string );

	if ( !results || !*results )
	{
		return (BANK_UPLOAD *)0;
	}
	else
	{
		return bank_upload_sequence_number_fetch(
				application_name,
				0 /* prior_sequence_number */ );
	}

} /* bank_upload_prior_fetch() */

BANK_UPLOAD *bank_upload_sequence_number_fetch(
					char *application_name,
					int sequence_number )
{
	char *select;
	char *folder;
	char where[ 512 ];
	char sys_string[ 1024 ];
	char *input_row;

	select = bank_upload_get_select();
	folder = BANK_UPLOAD_TABLE_NAME;

	sprintf( where,
		 "sequence_number = %d",
		 sequence_number );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 folder,
		 where );

	input_row = pipe2string( sys_string );

	if ( !input_row || !*input_row )
	{
		return (BANK_UPLOAD *)0;
	}
	else
	{
		return bank_upload_parse_row( input_row );
	}

} /* bank_upload_sequence_number_fetch() */

BANK_UPLOAD *bank_upload_parse_row( char *input_row )
{
	BANK_UPLOAD *bank_upload = bank_upload_calloc();

	bank_upload_fetch_parse(
			&bank_upload->bank_date,
			&bank_upload->bank_description,
			&bank_upload->sequence_number,
			&bank_upload->bank_amount,
			&bank_upload->bank_running_balance,
			&bank_upload->check_number,
			input_row );

	return bank_upload;

} /* bank_upload_parse_row() */

double bank_upload_archive_fetch_latest_running_balance(
					char *application_name )
{
	char *select;
	char *folder;
	char where[ 512 ];
	char sys_string[ 1024 ];
	char *results;
	int max_sequence_number;

	/* Get the maximum sequence number */
	/* ------------------------------- */
	select = "max( sequence_number )";
	folder = "bank_upload_archive";

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=%s		",
		 application_name,
		 select,
		 folder );

	if ( ! ( results = pipe2string( sys_string ) ) ) return 0.0;

	max_sequence_number = atoi( results );

	/* Get the latest running balance */
	/* ------------------------------ */
	select = "bank_running_balance";

	sprintf( where,
		 "sequence_number = %d",
		 max_sequence_number );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=%s		"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 folder,
		 where );

	results = pipe2string( sys_string );

	if ( results && *results )
		return atof( results );
	else
		return 0.0;

} /* bank_upload_archive_fetch_latest_running_balance() */

/* Insert into BANK_UPLOAD_TRANSACTION */
/* ----------------------------------- */
void bank_upload_reconciliation_transaction_insert(
					char *bank_date,
					char *bank_description_embedded,
					LIST *transaction_list )
{
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *table;
	char *field;
	TRANSACTION *transaction;

	if ( !list_rewind( transaction_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty transaction_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	table = "bank_upload_transaction";

	field =
"bank_date,bank_description,full_name,street_address,transaction_date_time";

	sprintf( sys_string,
		 "insert_statement.e table=%s field=%s del='^'	|"
		 "sql.e						 ",
		 table,
		 field );

	output_pipe = popen( sys_string, "w" );

	do {
		transaction = list_get( transaction_list );

		fprintf( output_pipe,
			 "%s^%s^%s^%s^%s\n",
			 bank_date,
			 bank_description_embedded,
			 transaction->full_name,
			 transaction->street_address,
			 transaction->transaction_date_time );

	} while( list_next( transaction_list ) );

	pclose( output_pipe );

} /* bank_upload_reconciliation_transaction_insert() */

LIST *bank_upload_get_feeder_transaction_list(
				char *application_name,
				char *fund_name,
				char *bank_description,
				double abs_bank_amount,
				double exact_value,
				boolean select_debit )
{
	char sys_string[ 2048 ];
	char select[ 512 ];
	char *folder;
	char join_where[ 512 ];
	char where[ 1024 ];
	char *order;
	FILE *input_pipe;
	FILE *output_pipe;
	char input_buffer[ 1024 ];
	char *cash_account;
	char exact_where[ 128 ];
	char full_name[ 128 ];
	char street_address[ 128 ];
	char transaction_date_time[ 128 ];
	char transaction_amount[ 128 ];
	char bank_upload_feeder_phrase[ 128 ];
	char temp_output_file[ 128 ];
	LIST *transaction_list;
	char *amount_column;
	char *pipe_delimited_transaction_list_string;

	cash_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			LEDGER_CASH_KEY,
			0 /* not warning_only */,
			__FUNCTION__ );

	if ( select_debit )
		amount_column = "debit_amount";
	else
		amount_column = "credit_amount";

	if ( exact_value )
	{
		sprintf(exact_where,
		 	"ifnull( %s, 0 ) = %.2lf",
			amount_column,
		 	exact_value );
	}
	else
	{
		strcpy( exact_where, "1 = 1" );
	}

	sprintf( select,
"journal_ledger.full_name, journal_ledger.street_address, transaction_date_time, %s, bank_upload_feeder_phrase",
		 amount_column );

	folder = "journal_ledger,reoccurring_transaction";

	sprintf(
join_where,
"reoccurring_transaction.full_name = journal_ledger.full_name and	"
"reoccurring_transaction.street_address = journal_ledger.street_address	" );

	sprintf( where,
"account = '%s' and ifnull( %s, 0 ) <> 0 and %s and %s and %s",
		 cash_account,
		 amount_column,
		 bank_upload_transaction_journal_ledger_subquery(),
		 exact_where,
		 join_where );

	order = "transaction_date_time";

	sprintf( sys_string,
		 "get_folder_data	application=%s		 "
		 "			select=\"%s\"		 "
		 "			folder=%s		 "
		 "			where=\"%s\"		 "
		 "			order=%s		|"
		 "head -%d					 ",
		 application_name,
		 select,
		 folder,
		 where,
		 order,
		 TRANSACTIONS_CHECK_COUNT );

	input_pipe = popen( sys_string, "r" );

	sprintf( temp_output_file,
		 "/tmp/bank_upload_transaction_insert_%d",
		 getpid() );

	sprintf( sys_string,
		 "keys_match_sum.e %.2lf > %s",
		 abs_bank_amount,
		 temp_output_file );

	output_pipe = popen( sys_string, "w" );

	while ( get_line( input_buffer, input_pipe ) )
	{
		if ( character_count(
			FOLDER_DATA_DELIMITER,
			input_buffer ) != 4 )
		{
			fprintf( stderr,
			"Error in %s/%s()/%d: not 4 delimiters in (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 input_buffer );

			pclose( input_pipe );
			pclose( output_pipe );

			exit( 1 );
		}	

		piece( full_name, FOLDER_DATA_DELIMITER, input_buffer, 0 );
		piece( street_address, FOLDER_DATA_DELIMITER, input_buffer, 1 );

		piece(	transaction_date_time,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		piece(	transaction_amount,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			3 );

		piece(	bank_upload_feeder_phrase,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			4 );

		if ( !timlib_string_exists(
			bank_description /* string */,
			bank_upload_feeder_phrase /* substring */ ) )
		{
			continue;
		}

		fprintf( output_pipe,
			 "%s^%s^%s|%s\n",
			 full_name,
			 street_address,
			 transaction_date_time,
			 transaction_amount );
		
	} /* while( get_line() ) */

	pclose( input_pipe );
	pclose( output_pipe );

	if ( !timlib_file_populated( temp_output_file ) )
	{
		return (LIST *)0;
	}

	sprintf( sys_string,
		 "cat %s",
		 temp_output_file );

	/* ------------------------------------------------------------ */
	/* Format: full_name^street_address^transaction_date_time[|...] */
	/* ------------------------------------------------------------ */
	pipe_delimited_transaction_list_string = pipe2string( sys_string );

	sprintf( sys_string, "rm %s", temp_output_file );
	if ( system( sys_string ) ) {};

	transaction_list =
		bank_upload_transaction_list_string_parse(
			pipe_delimited_transaction_list_string,
			'|' );

	return transaction_list;

} /* bank_upload_get_feeder_transaction_list() */

LIST *bank_upload_transaction_list_string_parse(
	/* ------------------------------------------------------------ */
	/* Format: full_name^street_address^transaction_date_time[|...] */
	/* ------------------------------------------------------------ */
	char *transaction_list_string,
	char delimiter )
{
	TRANSACTION *transaction;
	LIST *transaction_list;
	char piece_buffer[ 1024 ];
	char full_name[ 128 ];
	char street_address[ 128 ];
	char transaction_date_time [ 32 ];
	int p;

	transaction_list = list_new();

	for(	p = 0;
		piece( piece_buffer, delimiter, transaction_list_string, p );
		p++ )
	{
		/* Assume carrot delimited string */
		/* ------------------------------ */
		if ( timlib_count_delimiters( '^', piece_buffer ) != 2 )
		{
			return (LIST *)0;
		}

		piece( full_name, '^', piece_buffer, 0 );
		piece( street_address, '^', piece_buffer, 1 );
		piece( transaction_date_time, '^', piece_buffer, 2 );

		transaction =
			ledger_transaction_new(
				strdup( full_name ),
				strdup( street_address ),
				strdup( transaction_date_time ),
				(char *)0 /* memo */ );

		list_append_pointer( transaction_list, transaction );
	}

	return transaction_list;

} /* bank_upload_transaction_list_string_parse() */

char *bank_upload_transaction_bank_upload_subquery( void )
{
	char *subquery;

	subquery =
		"not exists						"
		"(select 1 from bank_upload_transaction			"
		"	where bank_upload_transaction.bank_date =	"
		"	      bank_upload.bank_date and			"
		"	      bank_upload_transaction.bank_description ="
		"	      bank_upload.bank_description )		";

	return subquery;

} /* bank_upload_transaction_bank_upload_subquery() */

char *bank_upload_transaction_journal_ledger_subquery( void )
{
	char *subquery;

	subquery =
		"not exists						"
		"(select 1 from bank_upload_transaction			"
		"	where bank_upload_transaction.full_name =	"
		"	      journal_ledger.full_name and		"
		"	      bank_upload_transaction.street_address =	"
		"	      journal_ledger.street_address and		"
		"	      bank_upload_transaction.			"
		"		transaction_date_time =			"
		" 	      journal_ledger.transaction_date_time )	";

	return subquery;

} /* bank_upload_transaction_journal_ledger_subquery() */

void bank_upload_transaction_balance_propagate(
			char *bank_date )
{
	char sys_string [ 1024 ];

	sprintf( sys_string,
		 "bank_upload_sequence_propagate.sh \"%s\" 		|"
		 "sql.e 2>&1						|"
		 "html_paragraph_wrapper.e			 	 ",
		 bank_date );

	if ( system( sys_string ) ) {};

	sprintf( sys_string,
		 "bank_upload_balance_propagate.sh \"%s\" 		|"
		 "sql.e 2>&1						|"
		 "html_paragraph_wrapper.e			 	 ",
		 bank_date );

	if ( system( sys_string ) ) {};

} /* bank_upload_transaction_balance_propagate() */

char *bank_upload_get_account_html(
		char *application_name,
		boolean existing_bank_upload,
		boolean existing_transaction,
		TRANSACTION *feeder_phrase_match_build_transaction,
		JOURNAL_LEDGER *feeder_check_number_existing_journal_ledger,
		LIST *match_sum_existing_journal_ledger_list )
{
	if ( feeder_check_number_existing_journal_ledger )
	{
		char buffer[ 256 ];

		sprintf( buffer,
			 "Check #%d %s",
			 feeder_check_number_existing_journal_ledger->
				check_number,
			 feeder_check_number_existing_journal_ledger->
				full_name );

		return strdup( buffer );
	}
	else
	if ( list_length( match_sum_existing_journal_ledger_list ) )
	{
		/* Returns strdup() */
		/* ---------------- */
		return bank_upload_journal_ledger_list_html(
			match_sum_existing_journal_ledger_list );
	}
	else
	if ( feeder_phrase_match_build_transaction )
	{
		char buffer[ 512 ];

		return strdup(
			format_initial_capital(
				buffer,
				ledger_get_non_cash_account_name(
					application_name,
					feeder_phrase_match_build_transaction )
			) );
	}
	else
	if ( existing_bank_upload )
	{
		return "Existing bank upload";
	}
	else
	if ( existing_transaction )
	{
		return "Existing transaction";
	}
	else
	{
		return "<p style=\"color:red\">No</p> transaction";
	}

} /* bank_upload_get_account_html() */

char *bank_upload_description_crop( char *bank_description )
{
	if ( strlen( bank_description ) >  BANK_UPLOAD_DESCRIPTION_SIZE )
	{
		*( bank_description + BANK_UPLOAD_DESCRIPTION_SIZE ) = '\0';
	}

	return bank_description;

} /* bank_upload_description_crop() */

char *bank_upload_unique_bank_description(
				boolean exists_fund,
				char *fund_name,
				char *input_bank_description,
				char *bank_amount,
				char *bank_balance )
{
	static char bank_description[ 1024 ];
	char fund_portion[ 512 ];
	char balance_portion[ 512 ];

	*fund_portion = '\0';

	if ( exists_fund )
	{
		sprintf( fund_portion, " %s", fund_name );
	}

	*balance_portion = '\0';

	if ( *bank_balance )
	{
		sprintf( balance_portion, " %s", bank_balance );
	}

	sprintf( bank_description,
		 "%s%s %s%s",
		 input_bank_description,
	 	 fund_portion,
		 bank_amount,
		 balance_portion );

	return bank_description;

} /* bank_upload_unique_bank_description() */

LIST *bank_upload_fetch_transaction_date_time_list(
			char *application_name,
			char *minimum_transaction_date,
			char *account_name )
{
	char sys_string[ 1024 ];
	char where[ 256 ];
	char *select;
	char *folder;

	select = "transaction_date_time";
	folder = "journal_ledger";

	sprintf(	where,
			"transaction_date_time >= '%s' and	"
			"account = '%s'				",
			minimum_transaction_date,
			account_name );

	sprintf(	sys_string,
			"get_folder_data	application=%s		"
			"			select=%s		"
			"			folder=%s		"
			"			where=\"%s\"		"
			"			order=select		",
			application_name,
			select,
			folder,
			where );

	return pipe2list( sys_string );

} /* bank_upload_transaction_date_time_list() */

int bank_upload_parse_check_number( char *bank_description )
{
	char buffer[ 512 ];

	timlib_strcpy( buffer, bank_description, 512 );

	search_replace_string( buffer, "check", "" );
	search_replace_string( buffer, "#", "" );

	return atof( buffer );

} /* bank_upload_parse_check_number() */

/* Does ledger_propagate() */
/* ----------------------- */
void bank_upload_cleared_checks_update(
				char *application_name,
				char *fund_name,
				LIST *bank_upload_list )
{
	char sys_string[ 1024 ];
	FILE *output_pipe;
	BANK_UPLOAD *bank_upload;
	JOURNAL_LEDGER *a;
	char *key;
	char *cash_account;
	char *uncleared_checks_account;
	char *first_transaction_date_time = {0};

	if ( !list_rewind( bank_upload_list ) ) return;

	cash_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			LEDGER_CASH_KEY,
			0 /* not warning_only */,
			__FUNCTION__ );

	uncleared_checks_account =
		ledger_get_hard_coded_account_name(
			application_name,
			fund_name,
			LEDGER_UNCLEARED_CHECKS_KEY,
			0 /* not warning_only */,
			__FUNCTION__ );

	key = "full_name,street_address,transaction_date_time,account";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y		|"
		 "sql.e 2>&1						|"
		 "html_paragraph_wrapper.e				|"
		 "cat							 ",
		 LEDGER_FOLDER_NAME,
		 key );

	output_pipe = popen( sys_string, "w" );

	do {
		bank_upload = list_get_pointer( bank_upload_list );

		if ( bank_upload->feeder_check_number_existing_journal_ledger )
		{
			a = bank_upload->
				feeder_check_number_existing_journal_ledger;

			fprintf( output_pipe,
				 "%s^%s^%s^%s^account^%s\n",
				 a->full_name,
				 a->street_address,
				 a->transaction_date_time,
				 uncleared_checks_account,
				 cash_account );

			if ( !first_transaction_date_time )
				first_transaction_date_time =
					a->transaction_date_time;
		}

	} while( list_next( bank_upload_list ) );

	pclose( output_pipe );

	if ( first_transaction_date_time )
	{
		ledger_propagate(
			application_name,
			first_transaction_date_time,
			cash_account );

		ledger_propagate(
			application_name,
			first_transaction_date_time,
			uncleared_checks_account );
	} 

} /* bank_upload_cleared_checks_update() */

void bank_upload_journal_ledger_text_display(
				JOURNAL_LEDGER *journal_ledger )
{
	if ( !journal_ledger )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: empty journal_ledger.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	printf( "%s/%s/%s: account_name = %s\n",
		 journal_ledger->full_name,
		 journal_ledger->street_address,
		 journal_ledger->transaction_date_time,
		 journal_ledger->account_name );

} /* bank_upload_journal_ledger_text_display() */

char *bank_upload_get_insert_bank_upload_filename(
				char *bank_upload_filename )
{
	char tmp[ 1024 ];
	int session_characters_to_trim;
	char *start_trimming_here;

	static char insert_bank_upload_filename[
			BANK_UPLOAD_FILENAME_SIZE + 1 ] = {0};

	timlib_strcpy(
		tmp,
		basename_get_filename( bank_upload_filename ),
		1024 );

	/* Trim off _$SESSION */
	/* ------------------ */
	session_characters_to_trim = SESSION_MAX_SESSION_SIZE + 1;

	start_trimming_here =
		tmp + strlen( tmp ) - session_characters_to_trim;

	*start_trimming_here = '\0';

	strncpy(	insert_bank_upload_filename,
			tmp,
			BANK_UPLOAD_FILENAME_SIZE );

	return insert_bank_upload_filename;

} /* bank_upload_get_insert_bank_upload_filename() */

boolean bank_upload_exists(	char *application_name,
				char *bank_date,
				char *bank_description_embedded,
				char *minimum_bank_date )
{
	static LIST *bank_upload_key_list = {0};
	char key[ 1024 ];

	if ( !bank_upload_key_list )
	{
		bank_upload_key_list =
			bank_upload_fetch_key_list(
				application_name,
				minimum_bank_date );
	}

	sprintf( key, "%s^%s", bank_date, bank_description_embedded );

	return list_string_exists( bank_upload_key_list, key );

} /* bank_upload_exists() */

LIST *bank_upload_fetch_key_list(
				char *application_name,
				char *minimum_bank_date )
{
	char sys_string[ 1024 ];
	char *select;
	char where[ 128 ];

	select = "bank_date,bank_description";

	sprintf( where,
		 "bank_date >= '%s'",
		 minimum_bank_date );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=%s		"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 BANK_UPLOAD_TABLE_NAME,
		 where );

	return pipe2list( sys_string );

} /* bank_upload_fetch_key_list() */

/* Returns strdup() */
/* ---------------- */
char *bank_upload_journal_ledger_list_html(
			LIST *match_sum_existing_journal_ledger_list )
{
	char buffer[ 1024 ];
	char *ptr = buffer;
	JOURNAL_LEDGER *journal_ledger;
	LIST *l = match_sum_existing_journal_ledger_list;

	if ( !list_rewind( l ) )
	{
		return strdup( "" );
	}

	do {
		journal_ledger = list_get_pointer( l );

		if ( list_at_first( l ) )
		{
			ptr += sprintf( ptr,
					"%s/%s",
					journal_ledger->full_name,
					journal_ledger->transaction_date_time );
		}
		else
		{
			ptr += sprintf( ptr,
					"<br>%s/%s",
					journal_ledger->full_name,
					journal_ledger->transaction_date_time );
		}

	} while ( list_next( l ) );

	return strdup( buffer );

} /* bank_upload_journal_ledger_list_html() */

void bank_upload_match_sum_existing_journal_ledger_list(
			LIST *bank_upload_list,
			LIST *existing_cash_journal_ledger_list )
{
	BANK_UPLOAD *bank_upload;
	boolean check_debit;
	double bank_amount;

	if ( !list_rewind( bank_upload_list ) ) return;

	do {
		bank_upload = list_get_pointer( bank_upload_list );

		if ( bank_upload->existing_bank_upload ) continue;

		if ( bank_upload->feeder_check_number_existing_journal_ledger
		||   bank_upload->feeder_phrase_match_build_transaction )
		{
			continue;
		}

		if ( !bank_upload->
			feeder_match_sum_existing_journal_ledger_list )
		{
			if ( bank_upload->bank_amount < 0.0 )
			{
				bank_amount =
					float_abs(
						bank_upload->bank_amount );
				check_debit = 0;
			}
			else
			{
				bank_amount = bank_upload->bank_amount;
				check_debit = 1;
			}

		   	bank_upload->
			   feeder_match_sum_existing_journal_ledger_list =
			   /* ------------------------------------ */
			   /* Sets journal_ledger->match_sum_taken */
			   /* ------------------------------------ */
			   feeder_match_sum_existing_journal_ledger_list(
				existing_cash_journal_ledger_list,
				bank_amount,
				check_debit );
		}

	} while ( list_next( bank_upload_list ) );

} /* bank_upload_match_sum_existing_journal_ledger_list() */

/* Insert into BANK_UPLOAD_TRANSACTION */
/* ----------------------------------- */
void bank_upload_direct_bank_upload_transaction_insert(
					LIST *bank_upload_list )
{
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *table;
	char *field;
	BANK_UPLOAD *bank_upload;

	if ( !list_rewind( bank_upload_list ) ) return;

	table = "bank_upload_transaction";

	field =
"bank_date,bank_description,full_name,street_address,transaction_date_time";

	sprintf( sys_string,
		 "insert_statement.e table=%s field=%s del='^'	|"
	 	 "sql.e 2>&1					|"
	 	 "html_paragraph_wrapper.e			 ",
		 table,
		 field );

	output_pipe = popen( sys_string, "w" );

	do {
		bank_upload = list_get_pointer( bank_upload_list );

		if ( bank_upload->feeder_phrase_match_build_transaction )
		{
			fprintf( output_pipe,
			 	"%s^%s^%s^%s^%s\n",
			 	bank_upload->bank_date,
			 	bank_upload->bank_description_embedded,
			 	bank_upload->
					feeder_phrase_match_build_transaction->
					full_name,
			 	bank_upload->
					feeder_phrase_match_build_transaction->
					street_address,
			 	bank_upload->
					feeder_phrase_match_build_transaction->
					transaction_date_time );
		}
		else
		if ( bank_upload->feeder_check_number_existing_journal_ledger )
		{
			fprintf( output_pipe,
			 	"%s^%s^%s^%s^%s\n",
			 	bank_upload->bank_date,
			 	bank_upload->bank_description_embedded,
			 	bank_upload->
				   feeder_check_number_existing_journal_ledger->
					full_name,
			 	bank_upload->
				   feeder_check_number_existing_journal_ledger->
					street_address,
			 	bank_upload->
				   feeder_check_number_existing_journal_ledger->
					transaction_date_time );
		}
		else
		if ( list_rewind(
			bank_upload->
			     feeder_match_sum_existing_journal_ledger_list ) )
		{
			JOURNAL_LEDGER *j;
			LIST *l;

			l =
			bank_upload->
			     feeder_match_sum_existing_journal_ledger_list;

			do {
				j = list_get_pointer( l );

				fprintf( output_pipe,
			 		"%s^%s^%s^%s^%s\n",
			 		bank_upload->bank_date,
			 		bank_upload->bank_description_embedded,
					j->full_name,
					j->street_address,
					j->transaction_date_time );

			} while ( list_next( l ) );
		}

	} while( list_next( bank_upload_list ) );

	pclose( output_pipe );

} /* bank_upload_direct_bank_upload_transaction_insert() */

/* Returns static memory */
/* --------------------- */
char *bank_upload_minimum_bank_date(
				char *minimum_bank_date,
				LIST *bank_upload_list )
{
	static char return_date[ 16 ];
	BANK_UPLOAD *bank_upload;
	char *transaction_date;

	if ( !minimum_bank_date || !*minimum_bank_date )
		return (char *)0;

	timlib_strcpy( return_date, minimum_bank_date, 16 );

	if ( !list_rewind( bank_upload_list ) )
		return return_date;

	strcpy( return_date, minimum_bank_date );

	do {
		bank_upload = list_get( bank_upload_list );

		transaction_date =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			bank_upload_minimum_transaction_date(
			   bank_upload->
			      feeder_check_number_existing_journal_ledger,
			   bank_upload->
			      feeder_match_sum_existing_journal_ledger_list );

		if ( timlib_strcmp( transaction_date, return_date ) < 0 )
		{
			strcpy( return_date, transaction_date );
		}

	} while ( list_next( bank_upload_list ) );

	return return_date;

} /* bank_upload_minimum_bank_date() */

/* Returns static memory */
/* --------------------- */
char *bank_upload_minimum_transaction_date(
		JOURNAL_LEDGER *feeder_check_number_existing_journal_ledger,
		LIST *feeder_match_sum_existing_journal_ledger_list )
{
	static char return_date[ 16 ];
	char transaction_date[ 16 ];
	JOURNAL_LEDGER *j;
	LIST *l;

	j = feeder_check_number_existing_journal_ledger;
	l = feeder_match_sum_existing_journal_ledger_list;

	if ( j )
	{
		strcpy(	return_date,
			column( transaction_date,
				0,
				j->transaction_date_time ) );
	}
	else
	{
		*return_date = '\0';
	}

	if ( !list_rewind( l ) ) return return_date;

	do {
		j = list_get( l );

		column( transaction_date,
			0,
			j->transaction_date_time );

		if ( timlib_strcmp( transaction_date, return_date ) < 0 )
		{
			strcpy( return_date, transaction_date );
		}

	} while ( list_next( l ) );

	return return_date;

} /* bank_upload_minimum_transaction_date() */

