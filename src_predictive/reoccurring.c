/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/reoccurring.c			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <unistd.h>
#include "timlib.h"
#include "date_convert.h"
#include "piece.h"
#include "appaserver_library.h"
#include "html_table.h"
#include "reoccurring.h"

REOCCURRING_STRUCTURE *reoccurring_calloc( void )
{
	REOCCURRING_STRUCTURE *p =
		(REOCCURRING_STRUCTURE *)
			calloc( 1, sizeof( REOCCURRING_STRUCTURE ) );

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

} /* reoccurring_calloc() */

REOCCURRING_STRUCTURE *reoccurring_structure_new( void )
{
	REOCCURRING_STRUCTURE *p =
		(REOCCURRING_STRUCTURE *)
			calloc( 1, sizeof( REOCCURRING_STRUCTURE ) );

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

} /* reoccurring_structure_new() */

REOCCURRING_TRANSACTION *reoccurring_transaction_calloc( void )
{
	REOCCURRING_TRANSACTION *p;

	if ( ! ( p = calloc( 1, sizeof( REOCCURRING_TRANSACTION ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return p;
}

REOCCURRING_TRANSACTION *reoccurring_transaction_fetch(
					char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_description,
					double transaction_amount )
{
	REOCCURRING_TRANSACTION *reoccurring_transaction;

	reoccurring_transaction = reoccurring_transaction_calloc();

	reoccurring_transaction->full_name = full_name;
	reoccurring_transaction->street_address = street_address;

	reoccurring_transaction->transaction_description =
		transaction_description;

	if ( !reoccurring_transaction_load(
			&reoccurring_transaction->debit_account,
			&reoccurring_transaction->credit_account,
			&reoccurring_transaction->transaction_amount,
			&reoccurring_transaction->accrued_daily_amount,
			&reoccurring_transaction->accrued_monthly_amount,
			application_name,
			reoccurring_transaction->full_name,
			reoccurring_transaction->street_address,
			reoccurring_transaction->transaction_description ) )
	{
		return (REOCCURRING_TRANSACTION *)0;
	}

	if ( !timlib_dollar_virtually_same( transaction_amount, 0.0 ) )
	{
		reoccurring_transaction->transaction_amount =
			transaction_amount;
	}

	return reoccurring_transaction;

} /* reoccurring_transaction_fetch() */

boolean reoccurring_transaction_load(
				char **debit_account,
				char **credit_account,
				double *transaction_amount,
				double *accrued_daily_amount,
				double *accrued_monthly_amount,
				char *application_name,
				char *full_name,
				char *street_address,
				char *transaction_description )
{
	char sys_string[ 1024 ];
	char *select;
	char where[ 512 ];
	char *results;
	char entity_buffer[ 128 ];
	char description_buffer[ 128 ];
	REOCCURRING_TRANSACTION *reoccurring_transaction;

	select = reoccurring_transaction_select();

	sprintf( where,
		 "full_name = '%s' and			"
		 "street_address = '%s' and		"
		 "transaction_description = '%s'	",
		 escape_character(	entity_buffer,
					full_name,
					'\'' ),
		 street_address,
		 escape_character(	description_buffer,
					transaction_description,
					'\'' ) );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=%s		"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 REOCCURRING_TRANSACTION_FOLDER_NAME,
		 where );

	results = pipe2string( sys_string );

	if ( !results ) return 0;

	reoccurring_transaction = reoccurring_transaction_calloc();

	reoccurring_transaction_parse(
			&reoccurring_transaction->
				full_name,
			&reoccurring_transaction->
				street_address,
			&reoccurring_transaction->
				debit_account,
			&reoccurring_transaction->
				credit_account,
			&reoccurring_transaction->
				transaction_amount,
			&reoccurring_transaction->
				bank_upload_feeder_phrase,
			&reoccurring_transaction->
				accrued_daily_amount,
			&reoccurring_transaction->
				accrued_monthly_amount,
			results );

	if ( reoccurring_transaction->debit_account )
	{
		*debit_account = reoccurring_transaction->debit_account;
	}
	else
	{
		return 0;
	}

	if ( reoccurring_transaction->credit_account )
	{
		*credit_account = reoccurring_transaction->credit_account;
	}
	else
	{
		return 0;
	}

	*transaction_amount = reoccurring_transaction->transaction_amount;
	*accrued_daily_amount = reoccurring_transaction->accrued_daily_amount;

	*accrued_monthly_amount =
		reoccurring_transaction->
			accrued_monthly_amount;

	return 1;

} /* reoccurring_transaction_load() */

void reoccurring_transaction_parse(
					char **full_name,
					char **street_address,
					char **debit_account,
					char **credit_account,
					double *transaction_amount,
					char **bank_upload_feeder_phrase,
					double *accrued_daily_amount,
					double *accrued_monthly_amount,
					char *input_buffer )
{
	char buffer[ 256 ];

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
	if ( *buffer )
		*full_name = strdup( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
	if ( *buffer )
		*street_address = strdup( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
	if ( *buffer )
		*debit_account = strdup( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
	if ( *buffer )
		*credit_account = strdup( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 4 );
	if ( *buffer )
		*transaction_amount = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 5 );
	if ( *buffer )
		*bank_upload_feeder_phrase = strdup( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 6 );
	if ( *buffer )
		*accrued_daily_amount = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, input_buffer, 7 );
	if ( *buffer )
		*accrued_monthly_amount = atof( buffer );

} /* reoccurring_transaction_parse() */

char *reoccurring_transaction_select( void )
{
	char *select;

	select = "full_name,			"
		 "street_address,		"
		 "debit_account,		"
		 "credit_account,		"
		 "transaction_amount,		"
		 "bank_upload_feeder_phrase,	"
		 "accrued_daily_amount,		"
		 "accrued_monthly_amount	";

	return select;
}

LIST *reoccurring_fetch_reoccurring_transaction_list(
					char *application_name )
{
	LIST *reoccurring_transaction_list;
	REOCCURRING_TRANSACTION *reoccurring_transaction;
	char *select;
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	FILE *input_pipe;
	char *where;

	reoccurring_transaction_list = list_new();

	select = reoccurring_transaction_select();

	where = "bank_upload_feeder_phrase is not null";

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=reoccurring_transaction	"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		reoccurring_transaction =
			reoccurring_transaction_calloc();

		reoccurring_transaction_parse(
				&reoccurring_transaction->
					full_name,
				&reoccurring_transaction->
					street_address,
				&reoccurring_transaction->
					debit_account,
				&reoccurring_transaction->
					credit_account,
				&reoccurring_transaction->
					transaction_amount,
				&reoccurring_transaction->
					bank_upload_feeder_phrase,
				&reoccurring_transaction->
					accrued_daily_amount,
				&reoccurring_transaction->
					accrued_monthly_amount,
				input_buffer );

		if ( reoccurring_transaction->debit_account
		&&   reoccurring_transaction->credit_account )
		{
			list_append_pointer(
				reoccurring_transaction_list,
				reoccurring_transaction );
		}
	}

	pclose( input_pipe );

	return reoccurring_transaction_list;

} /* reoccurring_fetch_reoccurring_transaction_list() */

REOCCURRING_TRANSACTION *reoccurring_seek_bank_upload_feeder_phrase(
				LIST *reoccurring_transaction_list,
				char *bank_description )
{
	REOCCURRING_TRANSACTION *reoccurring_transaction;
	char feeder_phrase_piece[ 1024 ];
	int p;

	if ( !list_rewind( reoccurring_transaction_list ) )
		return (REOCCURRING_TRANSACTION *)0;

	do {
		reoccurring_transaction =
			list_get(
				reoccurring_transaction_list );

		for(	p = 0;
			piece(	feeder_phrase_piece,
				FEEDER_PHRASE_DELIMITER,
				reoccurring_transaction->
					bank_upload_feeder_phrase,
				p );
			p++ )
		{
			if ( timlib_exists_string(
				bank_description /* string */,
				feeder_phrase_piece /* substring */ ) )
			{
				return reoccurring_transaction;
			}
		}

	} while( list_next( reoccurring_transaction_list ) );

	return (REOCCURRING_TRANSACTION *)0;

} /* reoccurring_seek_bank_upload_feeder_phrase() */

/* -------------------------- */
/* Returns 256K static memory */
/* -------------------------- */
char *reoccurring_memo(
				char *transaction_description,
				char *credit_account )
{
	static char memo[ 256 ];

	sprintf( memo,
		 "%s/%s",
		 transaction_description,
		 credit_account );

	return memo;

} /* reoccurring_memo() */

int reoccurring_days_between_last_transaction(
			char *application_name,
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *debit_account,
			char *credit_account )
{
	char sys_string[ 2048 ];
	int current_year;
	char where[ 1024 ];
	char sub_query[ 1024 ];
	char name_buffer[ 256 ];
	char *select;
	char *folder;
	char *max_transaction_date;
	char end_date_string[ 16 ];
	int days_between;

	if ( !transaction_date_time
	||   ! ( current_year = atoi( transaction_date_time ) ) )
	{
		return 0;
	}

	column( end_date_string, 0, transaction_date_time );

	select = "max( transaction_date_time )";
	folder = "transaction";

	post_reoccurring_transaction_subquery(
		sub_query,
		debit_account,
		credit_account );

	sprintf( where,
		 "full_name = '%s' and				"
		 "street_address = '%s' and			"
		 "%s						",
		 escape_character(	name_buffer,
					full_name,
					'\'' ),
		 street_address,
		 sub_query );

	sprintf( sys_string,
		 "get_folder_data	application=%s			 "
		 "			select=\"%s\"			 "
		 "			folder=%s			 "
		 "			where=\"%s\"			|"
		 "column.e 0						 ",
		 application_name,
		 select,
		 folder,
		 where );

	max_transaction_date = pipe2string( sys_string );

	if ( !timlib_strlen( max_transaction_date ) )
	{
		days_between = 1;
	}
	else
	{
		days_between =
			date_days_between(
				max_transaction_date /* from_date */,
				end_date_string /* to_date */ );
	}

	return days_between;

} /* reoccurring_days_between_last_transaction() */

