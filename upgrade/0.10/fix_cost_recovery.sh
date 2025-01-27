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

# Returns 0 if COST_RECOVERY.recovery_amount exists
# -------------------------------------------------
folder_attribute_exists.sh	$application		\
				cost_recovery		\
				recovery_amount

if [ $? -ne 0 ]
then
	exit 0
fi

table_name=cost_recovery

attribute=recovery_amount
echo "alter table ${table_name} modify $attribute float(12,2);" | sql.e
echo "update attribute set attribute_datatype = 'float', width = 12, float_decimal_places = 2 where attribute = '$attribute';" | sql.e

exit 0

