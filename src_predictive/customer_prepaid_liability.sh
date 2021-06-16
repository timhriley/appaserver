:
# $APPASERVER_HOME/src_predictive/customer_prepaid_liability.sh
# -------------------------------------------------------------
#
# Freely available software: see Appaserver.org
# -------------------------------------------------------------

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

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 full_name street_address" 1>&2
	exit 1
fi

full_name="$1"
street_address="$2"

echo 1000

exit 0
