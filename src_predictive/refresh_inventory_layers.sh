#!/bin/bash
# -----------------------------------------------------------------
# $APPASERVER_HOME/src_predictive/refresh_inventory_layers.sh
# -----------------------------------------------------------------

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

if [ "$#" -eq 0 ]
then
	echo "Usage: $0 ignored [inventory]" 1>&2
	exit 1
fi

if [ "$#" -eq 2 ]
then
	inventory=$2
	propagate_inventory_sale_layers $application '' '' '' "$inventory" '' n
	exit 0
fi

echo "select inventory_name from inventory;"		|
sql.e							|
while read inventory
do
	propagate_inventory_sale_layers $application '' '' '' "$inventory" '' n
done

exit $?
