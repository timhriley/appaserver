#!/bin/bash
# -------------------------------------------------------
# $APPASERVER_HOME/src_education/populate_entity.sh
# -------------------------------------------------------

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

echo $0 $* 1>&2

select="full_name,street_address"

table="entity"

echo "select $select from $table order by $select;"	|
sql.e

exit 0
