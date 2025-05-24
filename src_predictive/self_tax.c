/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/self_tax.c				*/
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
#include "self_tax.h"

SELF_TAX *self_tax_fetch(
		const char *self_tax_table,
		char *full_name,
		char *street_address )
{
	char *where;
	char *system_string;
	SELF_TAX *self_tax;
	char *input;

	if ( !full_name
	||   !street_address )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_primary_where(
			full_name,
			street_address );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			SELF_TAX_SELECT,
			(char *)self_tax_table,
			where );

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

	if ( ! ( self_tax =
			self_tax_parse(
				full_name,
				street_address,
				input ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"self_tax_parse() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	self_tax->payroll_pay_period =
		self_tax_resolve_payroll_pay_period(
			self_tax->payroll_pay_period_string );

	return self_tax;
}

enum self_tax_payroll_pay_period self_tax_resolve_payroll_pay_period(
		char *payroll_pay_period_string )
{
	if ( !payroll_pay_period_string )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"payroll_pay_period_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

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
		return pay_period_unknown;
}

SELF_TAX *self_tax_new(
		char *full_name,
		char *street_address )
{
	SELF_TAX *self_tax;

	if ( !full_name
	||   !street_address )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	self_tax = self_tax_calloc();

	self_tax->full_name = full_name;
	self_tax->street_address = street_address;

	return self_tax;
}

SELF_TAX *self_tax_calloc( void )
{
	SELF_TAX *self_tax;

	if ( ! ( self_tax = calloc( 1, sizeof ( SELF_TAX ) ) ) )
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

	return self_tax;
}

char *self_tax_resolve_payroll_pay_period_string(
		enum self_tax_payroll_pay_period
			payroll_pay_period )
{
	char *pay_period_string = "unknown";

	if ( payroll_pay_period == pay_period_weekly )
		pay_period_string = "weekly";
	else
	if ( payroll_pay_period == pay_period_biweekly )
		pay_period_string = "biweekly";
	else
	if ( payroll_pay_period == pay_period_semimonthly )
		pay_period_string = "semimonthly";
	else
	if ( payroll_pay_period == pay_period_monthly )
		pay_period_string = "monthly";

	return pay_period_string;
}

SELF_TAX *self_tax_parse(
		char *full_name,
		char *street_address,
		char *input )
{
	SELF_TAX *self_tax;
	char piece_buffer[ 128 ];

	if ( !full_name
	||   !street_address
	||   !input
	||   !*input )
	{
		return NULL;
	}

	self_tax =
		/* Safely returns */
		/* -------------- */
		self_tax_new(
			full_name,
			street_address );

	/* See SELF_TAX_SELECT */
	/* ------------------- */
	piece( piece_buffer, SQL_DELIMITER, input, 0 );
	if ( *piece_buffer )
		self_tax->inventory_cost_method =
			strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 1 );
	if ( *piece_buffer )
		self_tax->payroll_pay_period_string =
			strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 2 );
	if ( *piece_buffer )
		self_tax->payroll_begin_day =
			strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	if ( *piece_buffer )
		self_tax->social_security_combined_tax_rate =
			atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	if ( *piece_buffer )
		self_tax->social_security_payroll_ceiling =
			atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 5 );
	if ( *piece_buffer )
		self_tax->medicare_combined_tax_rate =
			atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 6 );
	if ( *piece_buffer )
		self_tax->medicare_additional_withholding_rate =
			atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 7 );
	if ( *piece_buffer )
		self_tax->medicare_additional_gross_pay_floor =
			atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 8 );
	if ( *piece_buffer )
		self_tax->federal_withholding_allowance_period_value =
			atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 9 );
	if ( *piece_buffer )
		self_tax->federal_nonresident_withholding_income_premium =
			atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 10 );
	if ( *piece_buffer )
		self_tax->state_withholding_allowance_period_value =
			atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 11 );
	if ( *piece_buffer )
		self_tax->state_itemized_allowance_period_value =
			atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 12 );
	if ( *piece_buffer )
		self_tax->federal_unemployment_wage_base =
			atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 13 );
	if ( *piece_buffer )
		self_tax->federal_unemployment_tax_standard_rate =
			atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 14 );
	if ( *piece_buffer )
		self_tax->federal_unemployment_threshold_rate =
			atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 15 );
	if ( *piece_buffer )
		self_tax->federal_unemployment_tax_minimum_rate =
			atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 16 );
	if ( *piece_buffer )
		self_tax->state_unemployment_wage_base =
			atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 17 );
	if ( *piece_buffer )
		self_tax->state_unemployment_tax_rate =
			atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 18 );
	if ( *piece_buffer )
		self_tax->state_sales_tax_rate =
			atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 19 );
	if ( *piece_buffer )
		self_tax->energy_charge_kilowatts_per_hour =
			atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 20 );
	if ( *piece_buffer )
		self_tax->paypall_cash_account_name =
			strdup( piece_buffer );

	return self_tax;
}

char *self_tax_paypal_cash_account_name(
		char *full_name,
		char *street_address )
{
	SELF_TAX *self_tax;

	if ( !full_name
	||   !street_address )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	self_tax =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		self_tax_fetch(
			SELF_TAX_TABLE,
			full_name,
			street_address );

	return self_tax->paypall_cash_account_name;
}

double self_tax_state_sales_tax_rate(
		char *full_name,
		char *street_address )
{
	SELF_TAX *self_tax;

	if ( !full_name
	||   !street_address )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	self_tax =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		self_tax_fetch(
			SELF_TAX_TABLE,
			full_name,
			street_address );

	return self_tax->state_sales_tax_rate;
}

