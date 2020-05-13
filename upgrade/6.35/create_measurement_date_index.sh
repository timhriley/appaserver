#!/bin/bash

exit  0

# Expect 5 hours, 50 minutes.
# ---------------------------
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

if [ "$application" != hydrology -a "$application" != "audubon" ]
then
	exit 0
fi

echo "create index measurement_measurement_date on measurement (measurement_date);"				|
sql.e

exit 0
