/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/entity.c				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "timlib.h"
#include "piece.h"
#include "appaserver_library.h"
#include "account.h"
#include "transaction.h"
#include "entity.h"

enum payroll_pay_period entity_payroll_pay_period(
				char *payroll_pay_period_string )
{
	if ( strcasecmp( payroll_pay_period_string, "weekly" ) == 0 )
		return pay_period_weekly;
	else
	if ( strcasecmp( payroll_pay_period_string, "biweekly" ) == 0 )
		return pay_period_biweekly;
	else
	if ( strcasecmp( payroll_pay_period_string, "semimonthly" ) == 0 )
		return pay_period_semimonthly;
	else
	if ( strcasecmp( payroll_pay_period_string, "monthly" ) == 0 )
		return pay_period_monthly;
	else
		return pay_period_not_set;
}

ENTITY *entity_calloc( void )
{
	ENTITY *e;

	if ( ! ( e = calloc( 1, sizeof( ENTITY ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return e;
}

ENTITY *entity_new(	char *full_name,
			char *street_address )
{
	ENTITY *e;

	e = entity_calloc();

	e->full_name = full_name;
	e->street_address = street_address;
	return e;
}

/* ---------------------- */
/* Returns program memory */
/* ---------------------- */
char *entity_select( void )
{
	return
		"full_name,"
		"street_address,"
		"city,"
		"state_code,"
		"zip_code,"
		"phone_number,"
		"email_address";
}

ENTITY *entity_sales_tax_payable_entity( void )
{
	char full_name[ 128 ];
	char street_address[ 128 ];
	char sys_string[ 1024 ];
	char *select;
	char *folder;
	char *results;

	select = "full_name,street_address";
	folder = "sales_tax_payable_entity";

	sprintf( sys_string,
		 "echo \"select %s from %s;\" | sql | head -1",
		 select,
		 folder );

	results = pipe2string( sys_string );

	if ( !results ) return (ENTITY *)0;

	piece( full_name, SQL_DELIMITER, results, 0 );
	piece( street_address, SQL_DELIMITER, results, 1 );

	return entity_new(	strdup( full_name ),
				strdup( street_address ) );
}

char *entity_get_payroll_pay_period_string(
				enum payroll_pay_period
					payroll_pay_period )
{
	if ( payroll_pay_period == pay_period_not_set )
		return "weekly";
	else
	if ( payroll_pay_period == pay_period_weekly )
		return "weekly";
	else
	if ( payroll_pay_period == pay_period_biweekly )
		return "biweekly";
	else
	if ( payroll_pay_period == pay_period_semimonthly )
		return "semimonthly";
	else
	if ( payroll_pay_period == pay_period_monthly )
		return "monthly";

	fprintf( stderr,
"ERROR in %s/%s()/%d: unrecognized payroll_pay_period = %d.\n",
		 __FILE__,
		 __FUNCTION__,
		 __LINE__,
		 payroll_pay_period );

	exit( 1 );

}

ENTITY *entity_full_name_seek(	char *full_name,
				LIST *entity_list )
{
	ENTITY *entity;

	if ( !list_rewind( entity_list ) ) return (ENTITY *)0;

	do {
		entity = list_get( entity_list );

		if ( string_strcmp(	entity->full_name,
					full_name ) == 0 )
		{
			return entity;
		}

	} while( list_next( entity_list ) );

	return (ENTITY *)0;
}

ENTITY *entity_seek(	char *full_name,
			char *street_address,
			LIST *entity_list )
{
	ENTITY *entity;

	if ( !list_rewind( entity_list ) ) return (ENTITY *)0;

	do {
		entity = list_get( entity_list );

		if ( string_strcmp(	entity->full_name,
					full_name ) == 0
		&&   string_strcmp(	entity->street_address,
					street_address ) == 0 )
		{
			return entity;
		}

	} while( list_next( entity_list ) );

	return (ENTITY *)0;
}

ENTITY *entity_getset(	LIST *entity_list,
			char *full_name,
			char *street_address,
			boolean with_strdup )
{
	ENTITY *entity;

	if ( ! ( entity =
			entity_seek(
				full_name,
				street_address,
				entity_list ) ) )
	{
		if ( with_strdup )
		{
			entity = entity_new(	strdup( full_name ),
						strdup( street_address ) );
		}
		else
		{
			entity = entity_new( full_name, street_address );
		}

		list_set( entity_list, entity );
	}
	return entity;
}

char *entity_list_display( LIST *entity_list )
{
	ENTITY *entity;
	char buffer[ 65536 ];
	char *ptr = buffer;

	*ptr = '\0';

	if ( list_rewind( entity_list ) )
	{
		do {
			entity = list_get_pointer( entity_list );

			ptr += sprintf(	ptr,
					"Entity: %s/%s, amount_due = %.2lf\n",
					entity->full_name,
					entity->street_address,
					entity->entity_liability_amount_due );

		} while( list_next( entity_list ) );
	}

	return strdup( buffer );
}

boolean entity_list_exists(
			char *full_name,
			char *street_address,
			LIST *entity_list )
{
	if ( !entity_list )
		return 0;
	else
	if ( entity_seek(
				full_name,
				street_address,
				entity_list ) )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

ENTITY *entity_fetch(	char *full_name,
			char *street_address )
{
	char sys_string[ 1024 ];

	if ( !full_name || !street_address )
	{
		return (ENTITY *)0;
	}

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql",
		 /* ---------------------- */
		 /* Returns program memory */
		 /* ---------------------- */
		 entity_select(),
		 ENTITY_TABLE,
		 /* -------------------------- */
		 /* Safely returns heap memory */
		 /* -------------------------- */
		 entity_primary_where(
			full_name,
			street_address ) );

	return entity_parse( pipe2string( sys_string ) );
}

ENTITY *entity_parse( char *input )
{
	char full_name[ 128 ];
	char street_address[ 128 ];
	char piece_buffer[ 128 ];
	ENTITY *entity;

	if ( !input || !*input ) return (ENTITY *)0;

	piece( full_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );

	entity = entity_new(
			strdup( full_name ),
			strdup( street_address ) );

	piece( piece_buffer, SQL_DELIMITER, input, 2 );
	entity->city = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	entity->state_code = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	entity->zip_code = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 5 );
	entity->phone_number = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 6 );
	entity->email_address = strdup( piece_buffer );

	return entity;
}

char *entity_name_escape( char *full_name )
{
	return entity_escape_full_name( full_name );
}

char *entity_escape_name( char *full_name )
{
	return entity_escape_full_name( full_name );
}

char *entity_escape_full_name( char *full_name )
{
	static char escape_full_name[ 256 ];

	if ( !full_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty full_name.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return string_escape_quote( escape_full_name, full_name );
}

char *entity_primary_where(
			char *full_name,
			char *street_address )
{
	char where[ 512 ];

	sprintf( where,
		 "full_name = '%s' and	"
		 "street_address = '%s'	",
		 /* Returns static memory */
		 /* --------------------- */
		 entity_escape_full_name( full_name ),
		 street_address );

	return strdup( where );
}

char *entity_street_address( char *full_name )
{
	static LIST *entity_list = {0};
	ENTITY *entity;

	if ( !entity_list )
	{
		entity_list =
			entity_system_list(
				entity_sys_string(
					"1 = 1" ) );
	}

	if ( ( entity = entity_full_name_seek( full_name, entity_list ) ) )
	{
		return entity->street_address;
	}
	else
	{
		return (char *)0;
	}
}

FILE *entity_insert_open(
			char *error_filename )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"insert_statement table=%s field=\"%s\" delimiter='%c'	|"
		"sql 2>&1						|"
		"grep -vi duplicate					|"
		"cat >%s 2>&1						 ",
		ENTITY_TABLE,
		ENTITY_INSERT_COLUMNS,
		SQL_DELIMITER,
		error_filename );

	return popen( sys_string, "w" );
}

void entity_insert_pipe(
			FILE *insert_pipe,
			char *full_name,
			char *street_address,
			char *email_address )
{
	fprintf(insert_pipe,
		"%s^%s^%s\n",
		entity_escape_name( full_name ),
		street_address,
		(email_address)
			? email_address
			: "" );
}

char *entity_name_display(
			char *full_name,
			char *street_address )
{
	static char display[ 256 ];

	if ( !full_name || !*full_name ) return "";

	if ( !street_address
	||   !*street_address
	||   timlib_strcmp( street_address, "unknown" ) == 0
	||   timlib_strcmp( street_address, "null" ) == 0 )
	{
		strcpy( display, full_name );
	}
	else
	{
		sprintf( display, "%s/%s", full_name, street_address );
	}

	return display;
}

LIST *entity_liability_account_list(
			LIST *exclude_account_name_list )
{
	char where[ 256 ];
	char sys_string[ 1024 ];
	LIST *entire_account_list;
	LIST *return_account_list;
	ACCOUNT *account;
	char in_clause_where[ 1024 ];
	char *in_clause;

	if ( list_length( exclude_account_name_list ) )
	{
		in_clause =
			timlib_with_list_get_in_clause(
				exclude_account_name_list );

		sprintf( in_clause_where,
			 "account not in (%s)",
			 in_clause );
	}
	else
	{
		strcpy( in_clause_where, "1 = 1" );
	}

	sprintf( where,
		 "subclassification = 'current_liability' and	"
		 "account <> 'uncleared_checks' and		"
		 "%s						",
		 in_clause_where );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s order by %s;\" | sql",
		 account_select(),
		 "account",
		 where,
		 "account" );

	entire_account_list = account_system_list( sys_string );

	if ( !list_rewind( entire_account_list ) ) return (LIST *)0;

	return_account_list = list_new();

	do {
		account = list_get( entire_account_list );

		account->account_after_balance_zero_journal_list =
			account_after_balance_zero_journal_list(
				account->account_name );

		if ( list_length( account->journal_list ) )
		{
			list_set( return_account_list, account );
		}

	} while ( list_next( entire_account_list ) );

	return return_account_list;
}

LIST *entity_system_list( char *sys_string )
{
	FILE *input_pipe;
	char input[ 1024 ];
	LIST *entity_list;

	if ( !sys_string ) return (LIST *)0;

	entity_list = list_new();

	input_pipe = popen( sys_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set( entity_list, entity_parse( input ) );
	}

	pclose( input_pipe );
	return entity_list;
}

char *entity_sys_string( char *where )
{
	char sys_string[ 1024 ];

	if ( !where ) return (char *)0;

	sprintf( sys_string,
		 "select.sh '%s' %s \"%s\" select",
		 /* ---------------------- */
		 /* Returns program memory */
		 /* ---------------------- */
		 entity_select(),
		 ENTITY_TABLE,
		 where );

	return strdup( sys_string );
}

LIST *entity_full_street_list(
			LIST *full_name_list,
			LIST *street_address_list )
{
	LIST *entity_list;

	if ( !list_length( full_name_list ) ) return (LIST *)0;

	if (	list_length( full_name_list ) !=
		list_length( street_address_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: list_lengths not same [%d vs. %d]\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			list_length( full_name_list ),
			list_length( street_address_list ) );
		exit( 1 );
	}

	list_rewind( full_name_list );
	list_rewind( street_address_list );
	entity_list = list_new();

	do {
		list_set(
			entity_list,
			entity_new(
				list_get( full_name_list ),
				list_get( street_address_list ) ) );

		list_next( street_address_list );

	} while( list_next( full_name_list ) );

	return entity_list;
}


ENTITY *entity_full_name_entity(
			/* ------------------- */
			/* Expect stack memory */
			/* ------------------- */
			char *full_name )
{
	ENTITY *entity;
	char *street_address;

	if ( !full_name ) return (ENTITY *)0;

	if ( ( street_address =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			entity_street_address(
				full_name ) ) )
	{
		entity =
			entity_new(
				strdup( full_name ),
				street_address );
	}
	else
	{
		entity =
			entity_new(
				strdup( full_name ),
				ENTITY_STREET_ADDRESS_UNKNOWN );
	}
	return entity;
}

double entity_liability_amount_due( LIST *journal_list )
{
	JOURNAL *journal;
	double amount_due;
	double difference;

	if ( !list_rewind( journal_list ) ) return 0.0;

	amount_due = 0.0;

	do {
		journal = list_get( journal_list );

		difference =	journal->credit_amount -
				journal->debit_amount;

		amount_due += difference;

	} while ( list_next( journal_list ) );

	return amount_due;
}

char *entity_liability_debit_account_name(
			char *account_name )
{
	return account_name;
}

LIST *entity_liability_journal_list(
			LIST *liability_account_list,
			char *full_name,
			char *street_address )
{
	ACCOUNT *account;
	JOURNAL *journal;
	LIST *return_journal_list;
	LIST *journal_list;

	if ( !list_rewind( liability_account_list ) )
		return (LIST *)0;

	return_journal_list = list_new();

	do {
		account =
			list_get(
				liability_account_list );

		journal_list =
			account->
				transaction_after_balance_zero_journal_list;

		if ( !list_rewind( journal_list ) ) continue;

		do {
			journal = list_get( journal_list );

			if ( strcmp(	journal->full_name,
					full_name ) == 0
			&&   strcmp(	journal->street_address,
					street_address ) == 0 )
			{
				list_set( return_journal_list, journal );
			}

		} while ( list_next( journal_list ) );

	} while ( list_next( liability_account_list ) );

	return return_journal_list;
}

