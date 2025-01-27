#!/bin/bash
# -------------------------------------------------------------------------
# $APPASERVER_HOME/src_predictive/finance_accumulated_depreciation_reset.sh
# -------------------------------------------------------------------------

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
	echo "\$ . set_database" 1>&2
	exit 1
fi

echo "	update fixed_asset_purchase				\
	set finance_accumulated_depreciation = (		\
		select sum( depreciation_amount )		\
		from depreciation				\
		where	depreciation.asset_name =		\
			fixed_asset_purchase.asset_name		\
		  and	depreciation.serial_label =		\
			fixed_asset_purchase.serial_label );"	|
sql.e

exit 0
