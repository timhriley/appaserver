#!/bin/sh
#create fixed_asset_purchase

if [ "$APPASERVER_DATABASE" != "" ]
then
	application=$APPASERVER_DATABASE
elif [ "$DATABASE" != "" ]
then
	application=$DATABASE
fi

if [ "$application" = "" ]
then
   echo "Error in `basename.e $0 n`: you must first:" 1>&2
   echo "$ . set_database" 1>&2
   exit 1
fi

# Returns 0 if BANK_UPLOAD.bank_date exists
# -----------------------------------------
folder_attribute_exists.sh $application bank_upload bank_date

if [ $? -ne 0 ]
then
	exit 1
fi

table_name=fixed_asset_purchase
echo "create table $table_name (asset_name char (30) not null,serial_label char (40) not null,full_name char (60),street_address char (40),purchase_date_time datetime,service_placement_date date,fixed_asset_cost double (12,2),units_produced_so_far integer,disposal_date date,cost_recovery_period char (10),cost_recovery_method char (25),cost_recovery_conversion char (11),depreciation_method char (25),estimated_useful_life_years integer,estimated_useful_life_units integer,estimated_residual_value integer,declining_balance_n double (3,1),cost_basis double (12,2),finance_accumulated_depreciation double (12,2),tax_adjusted_basis double (12,2)) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (asset_name,serial_label);" | sql.e
echo "create index ${table_name}_full_name on $table_name (full_name);" | sql.e


