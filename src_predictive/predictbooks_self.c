/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/predictbooks_self.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "piece.h"
#include "appaserver_error.h"
#include "appaserver.h"
#include "sql.h"
#include "entity.h"
#include "predictbooks_self.h"

PREDICTBOOKS_SELF *predictbooks_self_fetch(
		const char *predictbooks_self_select,
		const char *predictbooks_self_table,
		boolean contact_key_boolean )
{
	char *system_string;
	PREDICTBOOKS_SELF *predictbooks_self;
	char *input;

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			(char *)predictbooks_self_select,
			(char *)predictbooks_self_table,
			(char *)0 /* where */ );

	if ( ! ( input =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			string_pipe_input(
				system_string ) ) )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"string_pipe_input(%s) returned empty.",
			system_string );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( ! ( predictbooks_self =
			predictbooks_self_parse(
				contact_key_boolean,
				input ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"predictbooks_self_parse() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return predictbooks_self;
}

enum payroll_pay_period predictbooks_self_payroll_pay_period(
		char *pay_period_string )
{
	if ( !pay_period_string )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"pay_period_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( strcasecmp( pay_period_string, "weekly" ) == 0 )
		return weekly;
	else
	if ( strcasecmp( pay_period_string, "biweekly" ) == 0 )
		return biweekly;
	else
	if ( strcasecmp( pay_period_string, "semimonthly" ) == 0 )
		return semimonthly;
	else
	if ( strcasecmp( pay_period_string, "monthly" ) == 0 )
		return monthly;
	else
		return payroll_pay_period_unknown;
}

enum inventory_cost_method predictbooks_self_inventory_cost_method(
		char *cost_method_string )
{
	if ( !cost_method_string )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"cost_method_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}
	if ( strcasecmp( cost_method_string, "average" ) == 0 )
		return average;
	else
	if ( strcasecmp( cost_method_string, "fifo" ) == 0 )
		return fifo;
	else
	if ( strcasecmp( cost_method_string, "lifo" ) == 0 )
		return lifo;
	else
		return inventory_cost_method_unknown;
}

PREDICTBOOKS_SELF *predictbooks_self_new( char *full_name )
{
	PREDICTBOOKS_SELF *predictbooks_self;

	if ( !full_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"full_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	predictbooks_self = predictbooks_self_calloc();
	predictbooks_self->full_name = full_name;

	return predictbooks_self;
}

PREDICTBOOKS_SELF *predictbooks_self_calloc( void )
{
	PREDICTBOOKS_SELF *predictbooks_self;

	if ( ! ( predictbooks_self =
			calloc( 1, sizeof ( PREDICTBOOKS_SELF ) ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return predictbooks_self;
}

char *predictbooks_self_payroll_pay_period_string(
		enum payroll_pay_period payroll_pay_period )
{
	char *pay_period_string = "payroll_pay_period_unknown";

	if ( payroll_pay_period == weekly )
		pay_period_string = "weekly";
	else
	if ( payroll_pay_period == biweekly )
		pay_period_string = "biweekly";
	else
	if ( payroll_pay_period == semimonthly )
		pay_period_string = "semimonthly";
	else
	if ( payroll_pay_period == monthly )
		pay_period_string = "monthly";

	return pay_period_string;
}

PREDICTBOOKS_SELF *predictbooks_self_parse(
		boolean contact_key_boolean,
		char *input )
{
	PREDICTBOOKS_SELF *predictbooks_self;
	char buffer[ 128 ];

	if ( !input || !*input ) return NULL;

	/* See predictbooks_self_select_string() */
	/* ------------------------------------- */
	piece( buffer, SQL_DELIMITER, input, 0 );

	predictbooks_self =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		predictbooks_self_new(
			buffer /* full_name */ );

	piece( buffer, SQL_DELIMITER, input, 1 );
	if ( *buffer )
		predictbooks_self->inventory_cost_method =
			predictbooks_self_inventory_cost_method(
				buffer /* inventory_cost_method_string */ );

	piece( buffer, SQL_DELIMITER, input, 2 );
	if ( *buffer )
		predictbooks_self->payroll_pay_period =
			predictbooks_self_payroll_pay_period(
				buffer /* payroll_pay_period_string */ );

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer )
		predictbooks_self->payroll_begin_day =
			strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	if ( *buffer )
		predictbooks_self->social_security_combined_tax_rate =
			atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 5 );
	if ( *buffer )
		predictbooks_self->social_security_payroll_ceiling =
			atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 6 );
	if ( *buffer )
		predictbooks_self->medicare_combined_tax_rate =
			atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 7 );
	if ( *buffer )
		predictbooks_self->medicare_additional_withholding_rate =
			atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 8 );
	if ( *buffer )
		predictbooks_self->medicare_additional_gross_pay_floor =
			atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 9 );
	if ( *buffer )
		predictbooks_self->federal_withholding_allowance_period_value =
			atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 10 );
	if ( *buffer )
		predictbooks_self->
			federal_nonresident_withholding_income_premium =
				atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 11 );
	if ( *buffer )
		predictbooks_self->state_withholding_allowance_period_value =
			atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 12 );
	if ( *buffer )
		predictbooks_self->state_itemized_allowance_period_value =
			atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 13 );
	if ( *buffer )
		predictbooks_self->federal_unemployment_wage_base =
			atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 14 );
	if ( *buffer )
		predictbooks_self->federal_unemployment_tax_standard_rate =
			atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 15 );
	if ( *buffer )
		predictbooks_self->federal_unemployment_threshold_rate =
			atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 16 );
	if ( *buffer )
		predictbooks_self->federal_unemployment_tax_minimum_rate =
			atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 17 );
	if ( *buffer )
		predictbooks_self->state_unemployment_wage_base =
			atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 18 );
	if ( *buffer )
		predictbooks_self->state_unemployment_tax_rate =
			atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 19 );
	if ( *buffer )
		predictbooks_self->state_sales_tax_rate =
			atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 20 );
	if ( *buffer )
		predictbooks_self->energy_charge_kilowatts_per_hour =
			atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 21 );
	if ( *buffer )
		predictbooks_self->paypall_cash_account_name =
			strdup( buffer );

	if ( contact_key_boolean )
	{
		piece( buffer, SQL_DELIMITER, input, 22 );
		if ( *buffer )
			predictbooks_self->contact_key =
				strdup( buffer );
	}

	return predictbooks_self;
}

char *predictbooks_self_paypal_cash_account_name(
		boolean contact_key_boolean )
{
	PREDICTBOOKS_SELF *predictbooks_self;

	predictbooks_self =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		predictbooks_self_fetch(
			PREDICTBOOKS_SELF_SELECT,
			PREDICTBOOKS_SELF_TABLE,
			contact_key_boolean );

	return predictbooks_self->paypall_cash_account_name;
}

double predictbooks_self_state_sales_tax_rate(
		boolean contact_key_boolean )
{
	PREDICTBOOKS_SELF *predictbooks_self;

	predictbooks_self =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		predictbooks_self_fetch(
			PREDICTBOOKS_SELF_SELECT,
			PREDICTBOOKS_SELF_TABLE,
			contact_key_boolean );

	return predictbooks_self->state_sales_tax_rate;
}

char *predictbooks_self_select_string(
		const char *predictbooks_self_select,
		const char *entity_contact_key_column,
		boolean contact_key_boolean )
{
	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	entity_select_string(
		predictbooks_self_select /* entity_select */,
		entity_contact_key_column,
		contact_key_boolean );
}
