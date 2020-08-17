/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/entity_self.c			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "appaserver_library.h"
#include "purchase.h"
#include "customer.h"
#include "entity_self.h"

ENTITY_SELF *entity_self_new(	char *full_name,
				char *street_address )
{
	ENTITY_SELF *s;

	if ( ! ( s = calloc( 1, sizeof( ENTITY_SELF ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	s->entity = entity_new( full_name, street_address );

	return s;

} /* entity_self_new() */

char *entity_self_get_select( char *application_name )
{
	char *select;
	boolean inventory_cost_method_exists;

	inventory_cost_method_exists =
		attribute_exists(
			application_name,
			"self"
				/* folder_name */,
			"inventory_cost_method"
				/* attribute_name */ );

	if ( inventory_cost_method_exists )
	{
		select =
"full_name, street_address, inventory_cost_method, payroll_pay_period, payroll_beginning_day, social_security_combined_tax_rate, social_security_payroll_ceiling, medicare_combined_tax_rate, medicare_additional_withholding_rate, medicare_additional_gross_pay_floor, federal_unemployment_wage_base, federal_unemployment_tax_minimum_rate, federal_unemployment_tax_standard_rate, federal_withholding_allowance_period_value, federal_nonresident_withholding_income_premium, state_unemployment_wage_base, state_unemployment_tax_rate, federal_unemployment_threshold_rate, state_withholding_allowance_period_value, state_itemized_allowance_period_value, state_sales_tax_rate";
	}
	else
	{
		select =
"full_name, street_address, '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', '', ''";
	}

	return select;

} /* entity_self_get_select() */

ENTITY_SELF *entity_self_load(	char *application_name )
{
	ENTITY_SELF *self;
	char full_name[ 128 ];
	char street_address[ 128 ];
	char piece_buffer[ 128 ];
	char sys_string[ 1024 ];
	char *select;
	char *results;

	select = entity_self_get_select( application_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=self		",
		 application_name,
		 select );

	results = pipe2string( sys_string );

	if ( !results ) return (ENTITY_SELF *)0;

	piece( full_name, FOLDER_DATA_DELIMITER, results, 0 );
	piece( street_address, FOLDER_DATA_DELIMITER, results, 1 );

	self = entity_self_new(
			strdup( full_name ),
			strdup( street_address ) );

	if ( !entity_load(	&self->entity->city,
				&self->entity->state_code,
				&self->entity->zip_code,
				&self->entity->phone_number,
				&self->entity->email_address,
				application_name,
				self->entity->full_name,
				self->entity->street_address ) )
	{
		return (ENTITY_SELF *)0;
	}

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 2 );
	self->inventory_cost_method =
		entity_get_inventory_cost_method(
			piece_buffer );

	piece(	piece_buffer, FOLDER_DATA_DELIMITER, results, 3 );
	self->payroll_pay_period =
		entity_get_payroll_pay_period(
			piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 4 );
	self->payroll_beginning_day = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 5 );
	self->social_security_combined_tax_rate = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 6 );
	self->social_security_payroll_ceiling = atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 7 );
	self->medicare_combined_tax_rate = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 8 );
	self->medicare_additional_withholding_rate = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 9 );
	self->medicare_additional_gross_pay_floor = atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 10 );
	self->federal_unemployment_wage_base = atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 11 );
	self->federal_unemployment_tax_minimum_rate = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 12 );
	self->federal_unemployment_tax_standard_rate = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 13 );
	self->federal_withholding_allowance_period_value = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 14 );
	self->federal_nonresident_withholding_income_premium =
		atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 15 );
	self->state_unemployment_wage_base = atoi( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 16 );
	self->state_unemployment_tax_rate = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 17 );
	self->federal_unemployment_threshold_rate = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 18 );
	self->state_withholding_allowance_period_value = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 19 );
	self->state_itemized_allowance_period_value = atof( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 20 );
	self->state_sales_tax_rate = atof( piece_buffer );

	return self;

} /* entity_self_load() */

ENTITY_SELF *entity_self_inventory_load(
			char *application_name,
			char *inventory_name )
{
	ENTITY_SELF *entity_self;

	if ( ! ( entity_self = entity_self_load( application_name ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot fetch from SELF.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	entity_self->sale_inventory =
		inventory_load_new(
			application_name,
			inventory_name );

	entity_self->inventory_purchase_hash_table =
		inventory_get_arrived_inventory_purchase_hash_table(
			&entity_self->
				sale_inventory->
				inventory_purchase_list,
			&entity_self->inventory_purchase_name_list,
			application_name,
			inventory_name );

	entity_self->inventory_sale_hash_table =
		inventory_get_completed_inventory_sale_hash_table(
			&entity_self->
				sale_inventory->
				inventory_sale_list,
			&entity_self->inventory_sale_name_list,
			application_name,
			inventory_name );

	entity_self->transaction_hash_table =
		ledger_get_transaction_hash_table(
			application_name,
			inventory_name );

	entity_self->journal_ledger_hash_table =
		ledger_get_journal_ledger_hash_table(
			application_name,
			inventory_name );

	entity_self->purchase_order_list =
		purchase_get_inventory_purchase_order_list(
			application_name,
			inventory_name,
			entity_self->inventory_purchase_hash_table,
			entity_self->inventory_purchase_name_list,
			entity_self->transaction_hash_table,
			entity_self->journal_ledger_hash_table );

	entity_self->inventory_list =
		entity_get_inventory_list(
			application_name );

	entity_self->customer_sale_list =
		customer_get_inventory_customer_sale_list(
			application_name,
			inventory_name,
			entity_self->inventory_sale_hash_table,
			entity_self->inventory_sale_name_list,
			entity_self->transaction_hash_table,
			entity_self->journal_ledger_hash_table,
			entity_self->inventory_list );

	return entity_self;

} /* entity_self_inventory_load() */

