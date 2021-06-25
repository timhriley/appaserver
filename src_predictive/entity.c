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
#include "liability.h"
#include "journal.h"
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

double entity_liability_amount_due(
			LIST *entity_balance_zero_account_list )
{
	LIST *account_list;
	ACCOUNT *account;
	double amount_due;

	account_list = entity_balance_zero_account_list;

	if ( !list_rewind( account_list ) ) return 0.0;

	amount_due = 0.0;

	do {
		account = list_get( account_list );

		account->account_liability_due =
			journal_credit_difference_sum(
				account->liability_journal_list );

		amount_due += account->account_liability_due;

	} while ( list_next( account_list ) );

	return amount_due;
}

LIST *entity_balance_zero_account_list(
			LIST *account_list,
			char *full_name,
			char *street_address )
{
	ACCOUNT *account;
	JOURNAL *journal;
	LIST *return_account_list;
	LIST *journal_list;

	if ( !list_rewind( account_list ) ) return (LIST *)0;

	return_account_list = list_new();

	do {
		account =
			list_get(
				account_list );

		journal_list = account->account_balance_zero_journal_list;

		if ( !list_rewind( journal_list ) ) continue;

		do {
			journal = list_get( journal_list );

			if ( strcmp(	journal->full_name,
					full_name ) == 0
			&&   strcmp(	journal->street_address,
					street_address ) == 0 )
			{
				account =
					account_getset(
						return_account_list,
						account->account_name );

				if ( !account->liability_journal_list )
					account->liability_journal_list =
						list_new();

				list_set(
					account->liability_journal_list,
					journal );
			}

		} while ( list_next( journal_list ) );

	} while ( list_next( account_list ) );

	return return_account_list;
}

ENTITY *entity_liability_steady_state(
			ENTITY *entity,
			LIST *entity_balance_zero_account_list )
{
	if ( !entity )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty entity.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	entity->entity_liability_amount_due =
		/* ----------------------------------- */
		/* Sets account->account_liability_due */
		/* ----------------------------------- */
		entity_liability_amount_due(
			entity_balance_zero_account_list );

	entity->entity_liability_payment_amount =
		entity_liability_payment_amount(
			entity->dialog_box_payment_amount,
			entity->entity_liability_amount_due );

	entity->entity_liability_additional_payment_amount =
		entity_liability_additional_payment_amount(
			entity->dialog_box_payment_amount,
			entity->entity_liability_payment_amount );

	return entity;
}

double entity_liability_additional_payment_amount(
			double dialog_box_payment_amount,
			double entity_liability_payment_amount )
{
	double additional_payment_amount;

	if ( !dialog_box_payment_amount ) return 0.0;

	additional_payment_amount =
		dialog_box_payment_amount -
		entity_liability_payment_amount;

	if ( additional_payment_amount <= 0.0 )
		return 0.0;
	else
		return additional_payment_amount;
}

double entity_liability_payment_amount(
			double dialog_box_payment_amount,
			double entity_liability_amount_due )
{
	if ( !dialog_box_payment_amount )
		return entity_liability_amount_due;

	if ( dialog_box_payment_amount < entity_liability_amount_due )
		return dialog_box_payment_amount;
	else
		return entity_liability_amount_due;
}

double entity_liability_prepaid(
			char *payor_full_name,
			char *payor_street_address )
{
	LIST *entity_list = list_new();
	LIABILITY *liability = liability_calloc();
	ENTITY *entity;

	list_set(
		entity_list,
		entity_new(
			payor_full_name,
			payor_street_address ) );

	liability->liability_balance_zero_account_list =
		/* -------------------------------------- */
		/* Sets account_balance_zero_journal_list */
		/* -------------------------------------- */
		liability_balance_zero_account_list();

	liability->liability_entity_list =
		liability_entity_list(
			liability->liability_balance_zero_account_list,
			entity_list /* input_entity_list */,
			0.0 /* dialog_box_payment_amount */ );

	liability->liability_balance_zero_entity_list =
		liability_balance_zero_entity_list(
			liability->liability_entity_list,
			liability->liability_balance_zero_account_list );

	if ( !list_rewind(
		liability->liability_balance_zero_entity_list ) )
	{
		return 0.0;
	}

	entity =
		list_get(
			liability->
				liability_balance_zero_entity_list );

	if ( !list_length(
		entity->
			entity_balance_zero_account_list ) )
	{
		return 0.0;
	}

	if ( ( entity->entity_liability_amount_due =
		entity_liability_amount_due(
			entity->
			   entity_balance_zero_account_list ) ) > 0.0 )
	{
		return entity->entity_liability_amount_due;
	}

	return 0.0;
}

