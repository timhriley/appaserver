#!/bin/sh
# -------------------------------------------------------------
# $APPASERVER_HOME/src_hydrology/station_list.sh
# ----------------------------------------------
#
# No warranty and freely available software: see Appaserver.org
# -------------------------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 login_name station where" 1>&2
	exit 1
fi

login_name=$1
station=$2
where="$3"

if [ "$login_name" != "login_name" ]
then

	login_where_clause="appaserver_user_agency.login_name = '$login_name'"
	login_join_clause="appaserver_user_agency.agency = station.agency"
	from_clause="station, appaserver_user_agency"

else
	login_where_clause="1 = 1"
	login_join_clause="1 = 1"
	from_clause="station"
fi

if [ "$station" != "station" ]
then
	station_and_clause="station.station in (`single_quotes_around.e $station`)"
else
	station_and_clause="1 = 1"
fi

if [ "$where" != "$where" ]
then
	additional_where="$where"
else
	additional_where="1 = 1"
fi


echo "	select station.station						\
	from ${from_clause}						\
	where ${login_where_clause}					\
	  and ${login_join_clause}					\
	  and ${station_and_clause}					\
	  and ${additional_where}					\
	order by station.station;"					|
sql.e									|
toupper.e								|
cat

exit 0
