#!/bin/bash
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

# Returns 0 if FIXED_ASSET_PURCHASE.estimated_residual_value exists
# -----------------------------------------------------------------
folder_attribute_exists.sh	$application		\
				fixed_asset_purchase	\
				estimated_residual_value

if [ $? -ne 0 ]
then
	exit 0
fi

table_name=fixed_asset_purchase

attribute=estimated_residual_value
echo "alter table ${table_name} modify $attribute integer;" | sql.e
echo "update attribute set attribute_datatype = 'integer' where attribute = '$attribute';" | sql.e

attribute=declining_balance_n
echo "update attribute set attribute_datatype = 'integer', float_decimal_places = null where attribute = '$attribute';" | sql.e

attribute=finance_accumulated_depreciation
echo "alter table ${table_name} modify $attribute float(14,2);" | sql.e
echo "update attribute set attribute_datatype = 'float', width = 14, float_decimal_places = 2 where attribute = '$attribute';" | sql.e

# Returns 0 if FIXED_ASSET_PURCHASE.cost_basis exists
# ---------------------------------------------------
folder_attribute_exists.sh	$application		\
				$table_name		\
				cost_basis

if [ $? -eq 0 ]
then

attribute=cost_basis
echo "alter table ${table_name} modify $attribute float(14,2);" | sql.e
echo "update attribute set attribute_datatype = 'float', width = 14, float_decimal_places = 2 where attribute = '$attribute';" | sql.e

fi

# Returns 0 if FIXED_ASSET_PURCHASE.cost_basis exists
# ---------------------------------------------------
folder_attribute_exists.sh	$application		\
				$table_name		\
				tax_adjusted_basis

if [ $? -eq 0 ]
then

attribute=tax_adjusted_basis
echo "alter table ${table_name} modify $attribute float(14,2);" | sql.e
echo "update attribute set attribute_datatype = 'float', width = 14, float_decimal_places = 2 where attribute = '$attribute';" | sql.e

fi

exit 0

