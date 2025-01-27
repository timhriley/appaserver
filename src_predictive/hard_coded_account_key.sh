:
# $APPASERVER_HOME/src_predictive/hard_coded_account_key.sh
# ---------------------------------------------------------------
#
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

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

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 account_name" 1>&2
	exit 1
fi

account_name=$1

select="hard_coded_account_key"

from="account"

where="account = '$account_name'"

echo "select $select from $from where $where;" | sql.e

exit 0

