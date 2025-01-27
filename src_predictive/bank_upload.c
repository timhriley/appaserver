/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/bank_upload.c			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <unistd.h>
#include "timlib.h"
#include "String.h"
#include "date.h"
#include "date_convert.h"
#include "piece.h"
#include "environ.h"
#include "sql.h"
#include "column.h"
#include "session.h"
#include "appaserver_library.h"
#include "html_table.h"
#include "basename.h"
#include "sed.h"
#include "feeder_upload.h"
#include "account.h"
#include "reoccurring.h"
#include "journal.h"
#include "predictive.h"
#include "transaction.h"
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
			bank_upload_starting_sequence_number(
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

	p->file.minimum_bank_date =
		bank_upload_file_minimum_bank_date(
			p->file.input_filename,
			p->file.date_piece_offset );

	if ( !p->file.minimum_bank_date || !*p->file.minimum_bank_date )
	{
		bank_upload_exception = empty_transaction_rows;
		return (BANK_UPLOAD_STRUCTURE *)0;
	}

/* Sets:
		bank_upload->bank_date
		bank_upload->bank_description
		bank_upload->bank_description_embedded
		bank_upload->sequence_number
		bank_upload->bank_amount
		bank_upload->bank_running_balance
*/
	p->file.bank_upload_list =
		bank_upload_file_list(
			p->file.error_line_list,
			&p->file.file_sha256sum,
			p->file.minimum_bank_date,
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
		bank_upload_file_row_count(
			p->file.bank_upload_list );

	p->existing_cash_journal_list =
		bank_upload_existing_cash_journal_list(
			p->fund_name,
			feeder_account );

	uncleared_checks_back_date =
		date_yyyy_mm_dd_new(
			p->file.minimum_bank_date );

	date_increment_days(
		uncleared_checks_back_date,
		-15.0 );

	p->reoccurring_structure = reoccurring_structure_calloc();

	p->reoccurring_structure->
		reoccurring_transaction_list =
			reoccurring_transaction_list();

	p->bank_upload_date_time =
		date_now19(
			date_utc_offset() );

	return p;
}

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
LIST *bank_upload_file_list(
				LIST *error_line_list,
				char **file_sha256sum,
				char *minimum_bank_date,
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
	FILE *input_pipe;
	BANK_UPLOAD *bank_upload;
	LIST *bank_upload_list;
	int line_number = 0;

	if ( file_sha256sum )
	{
		*file_sha256sum = timlib_sha256sum( input_filename );
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

		if ( !bank_upload_bank_date_international(
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
		bank_upload->bank_description = strdup( bank_description );
		bank_upload->row_number = line_number;

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
			feeder_upload_description_embedded(
				bank_upload->bank_description
					/* bank_description_file */,
				fund_name,
				bank_upload->bank_amount,
				bank_upload->bank_running_balance );

		if ( bank_upload_exists(
			bank_upload->bank_date,
			bank_upload->bank_description_embedded,
			minimum_bank_date ) )
		{
			bank_upload->existing_bank_upload = 1;
		}

		list_set( bank_upload_list, bank_upload );
		starting_sequence_number++;
	}

	pclose( input_pipe );

	return bank_upload_list;
}

void bank_upload_event_insert(
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
	char *insert_bank_upload_filename;

	insert_bank_upload_filename =
		bank_upload_insert_bank_upload_filename(
			bank_upload_filename );

	exists_fund = predictive_fund_attribute_exists();

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

	sprintf( sys_string,
	 	 "insert_statement table=%s field=%s del='%c' 		  |"
	 	 "sql.e 2>&1						  |"
	 	 "html_paragraph_wrapper.e				   ",
	 	 "bank_upload_event",
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
}

void bank_upload_archive_insert(
			LIST *bank_upload_list,
			char *bank_upload_date_time )
{
	char sys_string[ 1024 ];
	FILE *bank_upload_archive_insert_pipe = {0};
	BANK_UPLOAD *bank_upload;

	if ( !list_rewind( bank_upload_list ) );

/*
#define INSERT_BANK_UPLOAD_ARCHIVE	\
	"bank_date,bank_description,sequence_number,bank_amount,bank_running_balance,bank_upload_date_time"
*/

	sprintf( sys_string,
		 "insert_statement table=%s field=%s del='%c' 		  |"
		 "sql.e 2>&1						  |"
		 "grep -vi duplicate					  |"
		 "html_paragraph_wrapper.e				  |"
		 "cat							   ",
		 "bank_upload_archive",
		 INSERT_BANK_UPLOAD_ARCHIVE,
		 '^' );

	bank_upload_archive_insert_pipe = popen( sys_string, "w" );

	do {
		bank_upload = list_get( bank_upload_list );

		if ( bank_upload->existing_bank_upload ) continue;

		if ( !bank_upload->bank_description_embedded )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: empty bank_description_embeded.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		fprintf(bank_upload_archive_insert_pipe,
			"%s^%s^%d^%.2lf^%.2lf^%s\n",
		 	bank_upload->bank_date,
			bank_upload->bank_description_embedded,
			bank_upload->sequence_number,
		 	bank_upload->bank_amount,
			bank_upload->bank_running_balance,
			bank_upload_date_time );

	} while( list_next( bank_upload_list ) );

	pclose( bank_upload_archive_insert_pipe );
}

void bank_upload_transaction_direct_insert(
			char *fund_name,
			char *bank_date,
			char *bank_description_embedded,
			char *full_name,
			char *street_address,
			char *transaction_date_time )
{
	char sys_string[ 1024 ];
	FILE *insert_pipe;
	char *field;

	if ( predictive_fund_exists() )
	{
		field =
"bank_date,bank_description,full_name,street_address,transaction_date_time,fund";
	}
	else
	{
		field =
"bank_date,bank_description,full_name,street_address,transaction_date_time";
	}

	sprintf( sys_string,
	 	 "insert_statement table=%s field=%s del='%c' 		  |"
	 	 "sql 2>&1						  |"
	 	 "html_paragraph_wrapper.e				   ",
	 	 "bank_upload_transaction",
	 	 field,
	 	 '^' );

	insert_pipe = popen( sys_string, "w" );

	if ( fund_name && *fund_name )
	{
		fprintf(insert_pipe,
			"%s^%s^%s^%s^%s^%s\n",
			bank_date,
			bank_description_embedded,
			full_name,
			street_address,
			transaction_date_time,
			fund_name );
	}
	else
	{
		fprintf(insert_pipe,
			"%s^%s^%s^%s^%s\n",
			bank_date,
			bank_description_embedded,
			full_name,
			street_address,
			transaction_date_time );
	}

	pclose( insert_pipe );
}

/* Returns table_insert_count */
/* -------------------------- */
int bank_upload_insert(
			char *fund_name,
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
	 	BANK_UPLOAD_TABLE_NAME,
	 	INSERT_BANK_UPLOAD,
	 	'^',
		error_filename );

	bank_upload_insert_pipe = popen( sys_string, "w" );

	do {
		bank_upload = list_get( bank_upload_list );

		if ( bank_upload->existing_bank_upload ) continue;

		if ( !bank_upload->bank_description_embedded )
		{
			bank_upload->bank_description_embedded =
				/* ----------------------- */
				/* Returns strdup() memory */
				/* ----------------------- */
				feeder_upload_description_embedded(
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
			bank_upload->bank_description_embedded,
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
}

int bank_upload_starting_sequence_number(
			char *input_filename,
			int date_piece_offset )
{
	int line_count;
	char sys_string[ 1024 ];

	line_count =
		bank_upload_line_count(
			input_filename,
			date_piece_offset );

	sprintf( sys_string,
		 "reference_number.sh %s %d",
		 environment_application(),
		 line_count );

	return atoi( pipe2string( sys_string ) );
}

int bank_upload_line_count(	char *input_filename,
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
}

boolean bank_upload_bank_date_international(
				char *bank_date_international,
				char *bank_date )
{
	date_convert_source_american(
		bank_date_international,
		international,
		bank_date );

	return date_convert_is_valid_international(
		bank_date_international );
}

char *bank_upload_select( void )
{
	return	"bank_date,"
		"bank_description,"
		"sequence_number,"
		"bank_amount,"
		"bank_running_balance";
}

char *bank_upload_primary_where(
				char *bank_date,
				char *bank_description_embedded )
{
	char where[ 512 ];
	char description_buffer[ 512 ];

	sprintf( where,
		 "bank_date = '%s' and		"
		 "bank_description = '%s'	",
		 bank_date,
		 escape_character(
			description_buffer,
			bank_description_embedded,
			'\'' ) );

	return strdup( where );
}

BANK_UPLOAD *bank_upload_fetch(
			char *bank_date,
			char *bank_description_embedded )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "echo \"select %s from %s where %s order by %s;\" | sql",
		 bank_upload_select(),
		 "bank_upload",
		 bank_upload_primary_where(
			bank_date,
			bank_description_embedded ),
		 bank_upload_select() );

	return bank_upload_parse( pipe2string( sys_string ) );
}

LIST *bank_upload_bank_upload_table_list(
					int starting_sequence_number,
					char *begin_date )
{
	LIST *bank_upload_list;
	char *select;
	char where[ 128 ];
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	FILE *input_pipe;

	bank_upload_list = list_new();
	select = bank_upload_select();

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
		 "echo \"select %s from %s where %s order by %s;\" | sql",
		 select,
		 "bank_upload",
		 where,
		 select );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		list_set(
			bank_upload_list,
			bank_upload_parse( input_buffer ) );
	}
	pclose( input_pipe );
	return bank_upload_list;
}

BANK_UPLOAD *bank_upload_parse( char *input )
{
	BANK_UPLOAD *bank_upload;
	char bank_date[ 128 ];
	char bank_description[ 512 ];
	char buffer[ 512 ];

	if ( !input || !*input ) return (BANK_UPLOAD *)0;

	piece( bank_date, SQL_DELIMITER, input, 0 );
	piece( bank_description, SQL_DELIMITER, input, 1 );

	bank_upload =
		bank_upload_new(
			strdup( bank_date ),
			strdup( bank_description ) );

	bank_upload->check_number =
		bank_upload_parse_check_number(
			bank_description );

	piece( buffer, SQL_DELIMITER, input, 2 );
	if ( *buffer )
		bank_upload->sequence_number = atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer )
		bank_upload->bank_amount = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer )
		bank_upload->bank_running_balance = atof( buffer );

	bank_upload->bank_description_embedded = bank_upload->bank_description;

	return bank_upload;
}

LIST *bank_upload_existing_cash_journal_list(
			char *fund_name,
			char *feeder_account )
{
	char sys_string[ 2048 ];
	char *uncleared_checks_account;
	char where[ 1024 ];
	char *join_where;
	char *subquery_join;
	char folder[ 128 ];
	char *timriley_where;

	uncleared_checks_account =
		account_hard_coded_account_name(
			fund_name,
			ACCOUNT_UNCLEARED_CHECKS_KEY,
			0 /* not warning_only */,
			__FUNCTION__ );

	sprintf( folder,
		 "%s,%s",
		 JOURNAL_TABLE,
		 TRANSACTION_TABLE );

	join_where = transaction_journal_join();

	subquery_join =
		bank_upload_transaction_journal_subquery();

	if ( strcmp( environment_application(), "timriley" ) == 0 )
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
		feeder_account,
		uncleared_checks_account,
		join_where,
		subquery_join,
		timriley_where );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s order by %s;\" | sql",
		 journal_check_number_select(),
		 folder,
		 where,
		 "transaction_date_time desc" );

	return journal_system_list(
			sys_string,
			1 /* fetch_check_number */,
			0 /* not fetch_memo */ );
}

/* ---------------------------------------------------------------------*/
/* Sets bank_upload->feeder_check_number_existing_journal		*/
/* or									*/
/* Sets bank_upload->feeder_match_sum_xisting_journal_list		*/
/* or									*/
/* Sets bank_upload->feeder_phrase_match_build_transaction		*/
/* --------------------------------------------------------------------	*/
void bank_upload_set_transaction(
				LIST *bank_upload_list,
				LIST *reoccurring_transaction_list,
				LIST *existing_cash_journal_list )
{
	bank_upload_check_number_existing_journal(
		bank_upload_list,
		existing_cash_journal_list );

	bank_upload_feeder_phrase_match_build_transaction(
		bank_upload_list,
		reoccurring_transaction_list );

	bank_upload_match_sum_existing_journal_list(
		bank_upload_list,
		existing_cash_journal_list );
}

void bank_upload_check_number_existing_journal(
			LIST *bank_upload_list,
			LIST *existing_cash_journal_list )
{
	BANK_UPLOAD *bank_upload;
	JOURNAL *journal;

	if ( !list_rewind( bank_upload_list ) ) return;

	do {
		bank_upload = list_get( bank_upload_list );

		if ( bank_upload->existing_bank_upload ) continue;

		if ( !bank_upload->check_number ) continue;

		if ( bank_upload->feeder_match_sum_existing_journal_list
		||   bank_upload->feeder_phrase_match_build_transaction )
		{
			continue;
		}

		if ( ( journal =
				feeder_check_number_existing_journal(
					existing_cash_journal_list,
					bank_upload->check_number ) ) )
		{
			bank_upload->
				feeder_check_number_existing_journal =
					journal;

			journal->match_sum_taken = 1;

			journal->check_number =
				bank_upload->check_number;
		}

	} while( list_next( bank_upload_list ) );
}

void bank_upload_feeder_phrase_match_build_transaction(
			LIST *bank_upload_list,
			LIST *reoccurring_transaction_list )
{
	BANK_UPLOAD *bank_upload;

	if ( !list_rewind( bank_upload_list ) ) return;

	do {
		bank_upload = list_get( bank_upload_list );

		if ( bank_upload->existing_bank_upload ) continue;

		if ( bank_upload->feeder_check_number_existing_journal
		||   bank_upload->feeder_match_sum_existing_journal_list )
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
}

LIST *bank_upload_transaction_list(
			LIST *bank_upload_list )
{
	BANK_UPLOAD *bank_upload;
	LIST *transaction_list;

	if ( !list_rewind( bank_upload_list ) ) return (LIST *)0;

	transaction_list = list_new();

	do {
		bank_upload = list_get( bank_upload_list );

		if ( bank_upload->existing_transaction ) continue;

		if ( !bank_upload->feeder_phrase_match_build_transaction )
			continue;

		list_set(
			transaction_list,
			bank_upload->
				feeder_phrase_match_build_transaction );

	} while ( list_next( bank_upload_list ) );

	return transaction_list;
}

void bank_upload_transaction_text_display( LIST *bank_upload_list )
{
	BANK_UPLOAD *bank_upload;
	char *transaction_memo;
	TRANSACTION *transaction;

	if ( !list_rewind( bank_upload_list ) ) return;

	do {
		bank_upload = list_get( bank_upload_list );

		if ( bank_upload->existing_transaction ) continue;

		transaction =
			bank_upload->
				feeder_phrase_match_build_transaction;

		if ( transaction )
		{
			transaction_memo =
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				bank_upload_transaction_memo(
					transaction->
						full_name,
					transaction->
						street_address,
					transaction->
						transaction_date_time );

			journal_list_text_display(
				transaction_memo,
				transaction->
					journal_list );
		}
		else
		if ( bank_upload->feeder_check_number_existing_journal )
		{
			bank_upload_journal_text_display(
				bank_upload->
				  feeder_check_number_existing_journal );
		}

	} while( list_next( bank_upload_list ) );
}

void bank_upload_transaction_table_display( LIST *bank_upload_list )
{
	BANK_UPLOAD *bank_upload;
	char *transaction_memo;
	TRANSACTION *transaction;

	if ( !list_rewind( bank_upload_list ) ) return;

	do {
		bank_upload = list_get( bank_upload_list );

		if ( bank_upload->existing_transaction ) continue;

		transaction =
			bank_upload->
				feeder_phrase_match_build_transaction;

		if ( transaction )
		{
			transaction_memo =
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				bank_upload_transaction_memo(
					transaction->
						full_name,
					transaction->
						street_address,
					transaction->
						transaction_date_time );

			journal_list_html_display(
				transaction->journal_list,
				transaction_memo );
		}

	} while( list_next( bank_upload_list ) );
}

BANK_UPLOAD *bank_upload_dictionary_extract(
			DICTIONARY *dictionary )
{
	char *key;
	BANK_UPLOAD *bank_upload;
	char *bank_date;
	char *bank_description;

	key = "bank_date";

	if ( ! ( bank_date = dictionary_fetch( key, dictionary ) ) )
	{
		return (BANK_UPLOAD *)0;
	}

	key = "bank_description";

	if ( ! ( bank_description =
			dictionary_fetch( key, dictionary ) ) )
	{
		return (BANK_UPLOAD *)0;
	}

	bank_upload =
		bank_upload_fetch(
			bank_date,
			bank_description );

	return bank_upload;
}

char *bank_upload_transaction_memo(
					char *full_name,
					char *street_address,
					char *transaction_date_time )
{
	static char transaction_memo[ 256 ];

	sprintf(transaction_memo,
		"%s/%s",
		/* Returns static memory */
		/* --------------------- */
		transaction_full_name_display(
			full_name,
			street_address ),
		transaction_date_time );

	return transaction_memo;
}

boolean bank_upload_sha256sum_exists(
			char *file_sha256sum )
{
	char where[ 512 ];
	char sys_string[ 1024 ];

	sprintf( where,
		 "file_sha256sum = '%s'",
		 file_sha256sum );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql",
		 "count(1)",
		 "bank_upload_event",
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
			b = list_get( bank_upload_list );

			if ( b->existing_transaction ) continue;

			if ( b->feeder_phrase_match_build_transaction )
				count++;

		} while( list_next( bank_upload_list ) );
	}

	return count;
}

int bank_upload_file_row_count(
			LIST *bank_upload_list )
{
	BANK_UPLOAD *b;
	int count;

	if ( !list_rewind( bank_upload_list ) ) return 0;

	count = 0;

	do {
		b = list_get( bank_upload_list );

		if ( b->existing_transaction ) continue;

		count++;

	} while ( list_next( bank_upload_list ) );

	return count;
}

boolean bank_upload_transaction_exists(
			char *bank_date,
			char *bank_description )
{
	char sys_string[ 1024 ];
	char *results;

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql",
		 "count(1)",
		 "bank_upload_transaction",
		 bank_upload_primary_where(
			bank_date,
			bank_description ) );

	results = pipe2string( sys_string );

	if ( !results || *results == '0' )
		return 0;
	else
		return 1;
}

double bank_upload_bank_amount(
			char *bank_date,
			char *bank_description )
{
	BANK_UPLOAD *b;

	if ( ! ( b = bank_upload_fetch(
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
}

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
}

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
}

BANK_UPLOAD *bank_upload_prior_fetch(
					int sequence_number )
{
	char *select;
	char where[ 512 ];
	char sys_string[ 1024 ];
	char *results;

	select = "max( sequence_number )";

	sprintf( where,
		 "sequence_number < %d",
		 sequence_number );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql",
		 select,
		 BANK_UPLOAD_TABLE_NAME,
		 where );

	results = pipe2string( sys_string );

	if ( !results || !*results )
	{
		return (BANK_UPLOAD *)0;
	}
	else
	{
		return bank_upload_sequence_number_fetch(
				0 /* prior_sequence_number */ );
	}
}

BANK_UPLOAD *bank_upload_sequence_number_fetch(
			int sequence_number )
{
	char *select;
	char where[ 512 ];
	char sys_string[ 1024 ];
	char *input_row;

	select = bank_upload_select();

	sprintf( where,
		 "sequence_number = %d",
		 sequence_number );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql",
		 select,
		 BANK_UPLOAD_TABLE_NAME,
		 where );

	input_row = pipe2string( sys_string );

	if ( !input_row || !*input_row )
	{
		return (BANK_UPLOAD *)0;
	}
	else
	{
		return bank_upload_parse( input_row );
	}
}

double bank_upload_archive_latest_running_balance( void )
{
	char *select;
	char where[ 512 ];
	char sys_string[ 1024 ];
	char *results;
	int max_sequence_number;

	select = "max( sequence_number )";

	sprintf( sys_string,
		 "echo \"select %s from %s;\" | sql",
		 select,
		 "bank_upload_archive" );

	if ( ! ( results = pipe2string( sys_string ) ) ) return 0.0;

	max_sequence_number = atoi( results );

	/* Get the latest running balance */
	/* ------------------------------ */
	select = "bank_running_balance";

	sprintf( where,
		 "sequence_number = %d",
		 max_sequence_number );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql",
		 select,
		 "bank_upload_archive",
		 where );

	results = pipe2string( sys_string );

	if ( results && *results )
		return atof( results );
	else
		return 0.0;
}

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
}

LIST *bank_upload_feeder_transaction_list(
				char *fund_name,
				char *bank_description,
				double abs_bank_amount,
				double exact_value,
				boolean select_debit )
{
	char sys_string[ 2048 ];
	char select[ 512 ];
	char folder[ 128 ];
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
		account_hard_coded_account_name(
			fund_name,
			ACCOUNT_CASH_KEY,
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

	sprintf(select,
		"%s.full_name,"
		"%s.street_address,"
		"transaction_date_time,"
		"%s,"
		"bank_upload_feeder_phrase",
		amount_column,
		JOURNAL_TABLE,
		JOURNAL_TABLE );

	sprintf( folder,
		 "%s,reoccurring_transaction",
		 JOURNAL_TABLE );

	sprintf(
join_where,
"reoccurring_transaction.full_name = %s.full_name and			"
"reoccurring_transaction.street_address = journal.street_address	",
		JOURNAL_TABLE );

	sprintf( where,
"account = '%s' and ifnull( %s, 0 ) <> 0 and %s and %s and %s",
		 cash_account,
		 amount_column,
		 bank_upload_transaction_journal_subquery(),
		 exact_where,
		 join_where );

	order = "transaction_date_time";

	sprintf( sys_string,
		 "echo \"select %s from %s where %s order by %s;\"	|"
		 "sql							|"
		 "head -%d						 ",
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
}

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
			transaction_new(
				strdup( full_name ),
				strdup( street_address ),
				strdup( transaction_date_time ) );

		list_set( transaction_list, transaction );
	}
	return transaction_list;
}

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
}

char *bank_upload_transaction_journal_subquery( void )
{
	char subquery[ 1024 ];

	sprintf(subquery,
		"not exists						"
		"(select 1 from bank_upload_transaction			"
		"	where bank_upload_transaction.full_name =	"
		"	      %s.full_name and				"
		"	      bank_upload_transaction.street_address =	"
		"	      %s.street_address and			"
		"	      bank_upload_transaction.			"
		"		transaction_date_time =			"
		" 	      %s.transaction_date_time )		",
		JOURNAL_TABLE,
		JOURNAL_TABLE,
		JOURNAL_TABLE );

	return strdup( subquery );
}

char *bank_upload_account_html(
		boolean existing_bank_upload,
		boolean existing_transaction,
		TRANSACTION *feeder_phrase_match_build_transaction,
		JOURNAL *feeder_check_number_existing_journal,
		LIST *match_sum_existing_journal_list )
{
	if ( feeder_check_number_existing_journal )
	{
		char buffer[ 256 ];

		sprintf( buffer,
			 "Check #%d %s",
			 feeder_check_number_existing_journal->
				check_number,
			 feeder_check_number_existing_journal->
				full_name );

		return strdup( buffer );
	}
	else
	if ( list_length( match_sum_existing_journal_list ) )
	{
		/* Returns strdup() */
		/* ---------------- */
		return bank_upload_journal_list_html(
			match_sum_existing_journal_list );
	}
	else
	if ( feeder_phrase_match_build_transaction )
	{
		char buffer[ 512 ];

		return strdup(
			format_initial_capital(
				buffer,
				account_non_cash_account_name(
					feeder_phrase_match_build_transaction->
						journal_list )
			) );
	}
	else
	if ( existing_bank_upload )
	{
		return "Posted bank upload";
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
}

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
}

LIST *bank_upload_transaction_date_time_list(
			char *minimum_transaction_date,
			char *account_name )
{
	char sys_string[ 1024 ];
	char where[ 256 ];
	char *select;
	char *folder;

	select = "transaction_date_time";
	folder = "journal";

	sprintf(	where,
			"transaction_date_time >= '%s' and	"
			"account = '%s'				",
			minimum_transaction_date,
			account_name );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql",
			select,
			folder,
			where );

	return pipe2list( sys_string );
}

int bank_upload_parse_check_number( char *bank_description )
{
	char buffer[ 512 ];

	timlib_strcpy( buffer, bank_description, 512 );

	search_replace_string( buffer, "check", "" );
	search_replace_string( buffer, "#", "" );

	return atof( buffer );
}

/* Does journal_propagate() */
/* ------------------------ */
void bank_upload_cleared_checks_update(
				char *fund_name,
				LIST *bank_upload_list )
{
	char sys_string[ 1024 ];
	FILE *output_pipe;
	BANK_UPLOAD *bank_upload;
	JOURNAL *journal;
	char *key;
	char *cash_account;
	char *uncleared_checks_account;
	char *earliest_transaction_date_time = {0};

	if ( !list_rewind( bank_upload_list ) ) return;

	cash_account =
		account_hard_coded_account_name(
			fund_name,
			ACCOUNT_CASH_KEY,
			0 /* not warning_only */,
			__FUNCTION__ );

	uncleared_checks_account =
		account_hard_coded_account_name(
			fund_name,
			ACCOUNT_UNCLEARED_CHECKS_KEY,
			0 /* not warning_only */,
			__FUNCTION__ );

	key = "full_name,street_address,transaction_date_time,account";

	sprintf( sys_string,
		 "update_statement table=%s key=%s carrot=y		|"
		 "tee_appaserver_error.sh				|"
		 "sql.e 2>&1						|"
		 "html_paragraph_wrapper.e				|"
		 "cat							 ",
		 JOURNAL_TABLE,
		 key );

	output_pipe = popen( sys_string, "w" );

	do {
		bank_upload = list_get( bank_upload_list );

		if ( bank_upload->feeder_check_number_existing_journal )
		{
			journal =
				bank_upload->
					feeder_check_number_existing_journal;

			fprintf( output_pipe,
				 "%s^%s^%s^%s^account^%s\n",
				 journal->full_name,
				 journal->street_address,
				 journal->transaction_date_time,
				 uncleared_checks_account,
				 cash_account );

			if ( !earliest_transaction_date_time )
			{
				earliest_transaction_date_time =
					journal->transaction_date_time;
			}
			else
			{
				if ( strcmp( 
					journal->transaction_date_time,
					earliest_transaction_date_time ) < 0 )
				{
					earliest_transaction_date_time =
						journal->transaction_date_time;
				}
			}
		}

	} while( list_next( bank_upload_list ) );

	pclose( output_pipe );

	if ( earliest_transaction_date_time )
	{
		journal_propagate(
			earliest_transaction_date_time,
			cash_account );

		journal_propagate(
			earliest_transaction_date_time,
			uncleared_checks_account );
	}
}

void bank_upload_journal_text_display(
			JOURNAL *journal )
{
	if ( !journal )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: empty journal.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	printf( "%s/%s/%s: account_name = %s\n",
		 journal->full_name,
		 journal->street_address,
		 journal->transaction_date_time,
		 journal->account_name );
}

char *bank_upload_insert_bank_upload_filename(
				char *bank_upload_filename )
{
	char tmp[ 1024 ];
	int session_characters_to_trim;
	char *start_trimming_here;

	static char insert_bank_upload_filename[
			BANK_UPLOAD_FILENAME_SIZE + 1 ] = {0};

	timlib_strcpy(
		tmp,
		basename_filename( bank_upload_filename ),
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
}

boolean bank_upload_exists(	char *bank_date,
				char *bank_description_embedded,
				char *minimum_bank_date )
{
	static LIST *key_list = {0};
	char key[ 1024 ];

	if ( !key_list )
	{
		key_list =
			bank_upload_key_list(
				minimum_bank_date );
	}

	sprintf( key, "%s^%s", bank_date, bank_description_embedded );

	return list_string_exists( key, key_list );
}

LIST *bank_upload_key_list(
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
		 "echo \"select %s from %s where %s;\" | sql",
		 select,
		 BANK_UPLOAD_TABLE_NAME,
		 where );

	return pipe2list( sys_string );
}

/* Returns strdup() */
/* ---------------- */
char *bank_upload_journal_list_html(
			LIST *match_sum_existing_journal_list )
{
	char buffer[ 1024 ];
	char *ptr = buffer;
	JOURNAL *journal;
	LIST *l = match_sum_existing_journal_list;

	if ( !list_rewind( l ) )
	{
		return strdup( "" );
	}

	do {
		journal = list_get( l );

		if ( list_at_first( l ) )
		{
			ptr += sprintf( ptr,
					"%s/%s",
					journal->full_name,
					journal->transaction_date_time );
		}
		else
		{
			ptr += sprintf( ptr,
					"<br>%s/%s",
					journal->full_name,
					journal->transaction_date_time );
		}

	} while ( list_next( l ) );

	return strdup( buffer );
}

void bank_upload_match_sum_existing_journal_list(
			LIST *bank_upload_list,
			LIST *existing_cash_journal_list )
{
	BANK_UPLOAD *bank_upload;
	boolean check_debit;
	double bank_amount;

	if ( !list_rewind( bank_upload_list ) ) return;

	do {
		bank_upload = list_get( bank_upload_list );

		if ( bank_upload->existing_bank_upload ) continue;

		if ( bank_upload->feeder_check_number_existing_journal
		||   bank_upload->feeder_phrase_match_build_transaction )
		{
			continue;
		}

		if ( !bank_upload->
			feeder_match_sum_existing_journal_list )
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
			   feeder_match_sum_existing_journal_list =
			   /* ----------------------------- */
			   /* Sets journal->match_sum_taken */
			   /* ----------------------------- */
			   feeder_match_sum_existing_journal_list(
				existing_cash_journal_list,
				bank_amount,
				check_debit );
		}

	} while ( list_next( bank_upload_list ) );
}

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
		bank_upload = list_get( bank_upload_list );

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
		if ( bank_upload->feeder_check_number_existing_journal )
		{
			fprintf( output_pipe,
			 	"%s^%s^%s^%s^%s\n",
			 	bank_upload->bank_date,
			 	bank_upload->bank_description_embedded,
			 	bank_upload->
				   feeder_check_number_existing_journal->
					full_name,
			 	bank_upload->
				   feeder_check_number_existing_journal->
					street_address,
			 	bank_upload->
				   feeder_check_number_existing_journal->
					transaction_date_time );
		}
		else
		if ( list_rewind(
			bank_upload->
			     feeder_match_sum_existing_journal_list ) )
		{
			JOURNAL *j;
			LIST *l;

			l =
			bank_upload->
			     feeder_match_sum_existing_journal_list;

			do {
				j = list_get( l );

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
}

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

	do {
		bank_upload = list_get( bank_upload_list );

		transaction_date =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			bank_upload_minimum_transaction_date(
			   bank_upload->
			      feeder_check_number_existing_journal,
			   bank_upload->
			      feeder_match_sum_existing_journal_list );

		if ( timlib_strcmp( transaction_date, return_date ) < 0 )
		{
			strcpy( return_date, transaction_date );
		}

	} while ( list_next( bank_upload_list ) );

	return return_date;
}

/* Returns static memory */
/* --------------------- */
char *bank_upload_minimum_transaction_date(
		JOURNAL *feeder_check_number_existing_journal,
		LIST *feeder_match_sum_existing_journal_list )
{
	static char return_date[ 16 ];
	char transaction_date[ 16 ];
	JOURNAL *j;
	LIST *l;

	j = feeder_check_number_existing_journal;
	l = feeder_match_sum_existing_journal_list;

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
}

void bank_upload_transaction_balance_propagate(
			char *bank_date,
			char *transaction_date_time )
{
	char sys_string[ 1024 ];
	char transaction_date[ 128 ];
	char *minimum_date;
	DATE *yesterday;

	if (	!bank_date
	||	!*bank_date
	||	strcmp( bank_date, "begin_date" ) == 0
	||	strcmp( bank_date, "bank_date" ) == 0 )
	{
		bank_date = "1960-01-01";
	}

	if ( transaction_date_time )
	{
		column( transaction_date, 0, transaction_date_time );
	}
	else
	{
		*transaction_date = '\0';
	}

	if ( *transaction_date
	&&   strcmp( transaction_date, bank_date ) < 0 )
	{
		minimum_date = transaction_date;
	}
	else
	{
		minimum_date = bank_date;
	}

	yesterday = date_yyyy_mm_dd_new( minimum_date );
	date_increment_days( yesterday, -1.0 );

	sprintf( sys_string,
		 "bank_upload_sequence_propagate.sh \"%s\" 		|"
		 "sql.e 2>&1						|"
		 "html_paragraph_wrapper.e			 	 ",
		 date_yyyy_mm_dd_display( yesterday ) );

	if ( system( sys_string ) ) {};

	sprintf( sys_string,
		 "bank_upload_balance_propagate.sh \"%s\" 		|"
		 "sql.e 2>&1						|"
		 "html_paragraph_wrapper.e			 	 ",
		 date_yyyy_mm_dd_display( yesterday ) );

	if ( system( sys_string ) ) {};
}

void bank_upload_table_display(
				LIST *bank_upload_list )
{
	BANK_UPLOAD *bank_upload;
	FILE *output_pipe = {0};
	char sys_string[ 1024 ];
	char *heading;
	LIST *match_sum_journal_list;

	heading =
	"Row,Account<br>Entity/Transaction,bank_date,bank_description,amount";

	sprintf( sys_string,
		 "html_table.e '' '%s' '^' right,left,left,left,right",
		 heading );

	if ( !list_rewind( bank_upload_list ) )
	{
		output_pipe = popen( sys_string, "w" );
		pclose( output_pipe );
		return;
	}

	do {
		if ( !output_pipe )
		{
			output_pipe = popen( sys_string, "w" );
		}

		bank_upload = list_get( bank_upload_list );

		match_sum_journal_list =
			bank_upload->
			feeder_match_sum_existing_journal_list;

		fprintf( output_pipe,
			 "%d^%s^%s^%s^%.2lf\n",
			 bank_upload->row_number,
			 bank_upload_account_html(
				bank_upload->existing_bank_upload,
				bank_upload->existing_transaction,
				bank_upload->
					feeder_phrase_match_build_transaction,
				bank_upload->
				    feeder_check_number_existing_journal,
				match_sum_journal_list ),
			 bank_upload->bank_date,
			 bank_upload->bank_description_embedded,
			 bank_upload->bank_amount );

		if ( bank_upload->feeder_phrase_match_build_transaction )
		{
			pclose( output_pipe );
			output_pipe = (FILE *)0;

			journal_list_html_display(
				bank_upload->
					feeder_phrase_match_build_transaction->
					journal_list,
				bank_upload->
					feeder_phrase_match_build_transaction->
					memo );
		}

	} while( list_next( bank_upload_list ) );

	if ( output_pipe ) pclose( output_pipe );
}

char *bank_upload_file_minimum_bank_date(
			char *input_filename,
			int date_piece_offset )
{
	char input_string[ 4096 ];
	char bank_date[ 128 ];
	char bank_date_international[ 128 ];
	char minimum_bank_date[ 128 ];
	FILE *input_file;

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot open %s for read.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			input_filename );
		exit( 1 );
	}

	*minimum_bank_date = '\0';

	while( timlib_get_line( input_string, input_file, 4096 ) )
	{
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

		if ( !bank_upload_bank_date_international(
				bank_date_international,
				bank_date ) )
		{
			continue;
		}

		if ( !*minimum_bank_date )
		{
			strcpy( minimum_bank_date, bank_date_international );
			continue;
		}

		if ( strcmp(	bank_date_international,
				minimum_bank_date ) < 0 )
		{
			strcpy( minimum_bank_date, bank_date_international );
		}
	}

	fclose( input_file );

	return strdup( minimum_bank_date );
}

