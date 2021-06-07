#!/bin/bash
# -----------------------------------------------------------------------
# $APPASERVER_HOME/src_predictive/recovery_adjusted_basis_fetch_update.sh
# -----------------------------------------------------------------------

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

if [ $# -ne 2 ]
then
	echo "Usage: $0 asset_name serial_label" 1>&2
	exit 1
fi

asset_name="$1"
serial_label="$2"

recovery_amount_total=`							\
	echo "	select sum( recovery_amount )				\
		from cost_recovery					\
		where asset_name = '$asset_name'			\
		  and serial_label = '$serial_label';"			|
	sql.e`

if [ "$recovery_amount_total" = "" ]
then
	recovery_amount_total=0
fi

echo "	update fixed_asset_purchase					\
	set tax_adjusted_basis = cost_basis - $recovery_amount_total	\
	where asset_name = '$asset_name'			\
	  and serial_label = '$serial_label';"			|
sql.e

exit $?
