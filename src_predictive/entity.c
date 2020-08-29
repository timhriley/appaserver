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

char *entity_title_passage_rule_string(
				enum title_passage_rule title_passage_rule )
{
	if ( title_passage_rule == title_passage_rule_null )
		return TITLE_PASSAGE_RULE_NULL;
	else
	if ( title_passage_rule == FOB_shipping )
		return TITLE_PASSAGE_RULE_SHIPPED_DATE;
	else
	if ( title_passage_rule == FOB_destination )
		return TITLE_PASSAGE_RULE_ARRIVED_DATE;
	else
		return TITLE_PASSAGE_RULE_NULL;
}

enum title_passage_rule entity_title_passage_rule_resolve(
				char *title_passage_rule_string )
{
	if ( !title_passage_rule_string )
	{
		return title_passage_rule_null;
	}
	else
	if ( timlib_strcmp(
			title_passage_rule_string,
			TITLE_PASSAGE_RULE_SHIPPED_DATE ) == 0 )
	{
		return FOB_shipping;
	}
	else
	if ( timlib_strcmp(
			title_passage_rule_string,
			TITLE_PASSAGE_RULE_ARRIVED_DATE ) == 0 )
	{
		return FOB_destination;
	}
	else
	{
		return title_passage_rule_null;
	}
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

} /* entity_get_payroll_pay_period_string() */

ENTITY *entity_seek(		LIST *entity_list,
				char *full_name,
				char *street_address )
{
	ENTITY *entity;

	if ( !entity_list )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: entity_list is null.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( entity_list ) ) return (ENTITY *)0;

	do {
		entity = list_get( entity_list );

		if ( timlib_strcmp(	entity->full_name,
					full_name ) == 0
		&&   timlib_strcmp(	entity->street_address,
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
				entity_list,
				full_name,
				street_address ) ) )
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
					"Entity: %s/%s, sum_balance = %.2lf\n",
					entity->full_name,
					entity->street_address,
					entity->sum_balance );

		} while( list_next( entity_list ) );
	}

	return strdup( buffer );
}

boolean entity_list_exists(	LIST *entity_list,
				char *full_name,
				char *street_address )
{
	if ( !entity_list )
		return 0;
	else
	if ( entity_seek(
					entity_list,
					full_name,
					street_address ) )
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
		 "entity",
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
	ENTITY *entity;

	if ( !input || !*input ) return (ENTITY *)0;

	piece( full_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );

	entity = entity_new(
			strdup( full_name ),
			strdup( street_address ) );

	return entity;
}

char *entity_escape_full_name(
			char *full_name )
{
	static char escape_full_name[ 256 ];

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


